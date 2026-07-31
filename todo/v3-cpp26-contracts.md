# C++26 Contracts (P2900) — Findings and V3 Interface Design Notes

Status: initial support landed on master (#682). This note captures what we learned so the
V3 interfaces can be designed with contracts in mind.

## What landed

- `MP_UNITS_API_CONTRACTS=1` (CMake `STD`, Conan `contracts=std`) maps `MP_UNITS_EXPECTS`
  and `MP_UNITS_ASSERT` to `MP_UNITS_CONTRACT_ASSERT`, and the `_DEBUG` variants to the same
  gated on `NDEBUG` (mirroring the ms-gsl backend). `MP_UNITS_CONTRACT_ASSERT` uses
  `contract_assert` only at runtime (`if !consteval`), and even there only behind the
  non-constexpr `detail::runtime_contract_check()` helper - some GCC 16 snapshots (e.g.
  20260315, used by CI) choke on a `contract_assert` even in the not-taken branch of
  `if consteval`, and a non-constexpr function body is invisible to the constant evaluator.
  The price is a less precise runtime violation message (it points to the helper, not the
  call site). During constant evaluation a violation is reported by calling the
  non-constexpr `mp_units::detail::contract_violation()`, because GCC 16 cannot yet
  constant-evaluate `contract_assert` predicates reliably (see below).
  Side effect: compile-time checks stay active even under
  `-fcontract-evaluation-semantic=ignore` - arguably a feature, but a semantic divergence
  from pure P2900 worth remembering.
- Auto-detection: when `MP_UNITS_API_CONTRACTS` is not defined, `__cpp_contracts >= 202502L`
  selects the native backend before the gsl-lite/ms-gsl `__has_include` probes.
- Each macro belongs to exactly one vocabulary (final design, after review):
    - `MP_UNITS_EXPECTS*` are **GSL-only** (no-op under the native backend) - their name and
      semantics stay consistent with GSL forever, so no breaking change for GSL users.
    - `MP_UNITS_PRE(...)` / `MP_UNITS_POST(...)` are **native-only** declaration contracts
      (empty under GSL backends). A `pre()`-declared function keeps `MP_UNITS_EXPECTS` in
      its body for the GSL backends - the predicate is typed twice but evaluated once per
      backend. Piloted on `basic_fixed_string::operator[]`, `front()`, and `back()`.
    - `MP_UNITS_PRE_BODY*` are **native-only** in-body precondition checks, for functions
      that cannot declare their precondition with `pre()` (deep constexpr frames - see the
      GCC bug below - lambdas, or not-yet-migrated code).
    - `MP_UNITS_PRECONDITION*` are the **backend-independent aggregates**
      (`MP_UNITS_EXPECTS` + `MP_UNITS_PRE_BODY`; exactly one expands to a check). All
      not-yet-migrated in-body precondition sites in the library use these.
    - `MP_UNITS_ASSERT*` stay backend-independent by nature: an invariant has no declaration
      form in P2900, so `contract_assert` is its native equivalent.
- CMake adds `-fcontracts` (GCC >= 16 only) via the `mp-units-contracts` INTERFACE target;
  Conan appends it to `cxxflags` in `package_info()`.

## Toolchain status (July 2026)

- **GCC 16 trunk** is the only compiler with P2900 support: `-std=c++2c -fcontracts`,
  defines `__cpp_contracts == 202502L`. Evaluation semantic is selected with
  `-fcontract-evaluation-semantic=ignore|observe|enforce|quick-enforce`. The default
  violation handler (`::handle_contract_violation(const std::contracts::contract_violation&)`)
  lives in `libstdc++exp` - executables with runtime checks need `-lstdc++exp`.
- GCC 15's `-fcontracts` is the old C++20-era attribute syntax; it rejects P2900 and does
  not define `__cpp_contracts`, so auto-detection stays off there.
- Clang 21/22 and MSVC have nothing yet.

## Key findings

### Constant evaluation is the killer feature

A violated `pre()`/`contract_assert` predicate during constant evaluation is a hard compile
error ("contract predicate is false in constant expression"). For a library as constexpr-heavy
as mp-units this turns every runtime precondition into a compile-time check whenever the call
is constant-evaluated - strictly better than the gsl-lite behaviour and with a clearer
diagnostic.

### GCC 16 bug: `contract_assert` predicates fail constant evaluation

GCC 16 trunk (r16-8246) rejects perfectly constant predicates with "contract condition is
not constant" in deep realistic contexts. Three distinct in-library predicates failed:
`size() < capacity()` (member call), `size_ < capacity()` (direct member read, in a deeper
instantiation), and `value != get_zero(value)` (plain `const int&` parameter in
`detail::get_one`). Each passes in a shallow repro; failures appear only in deep constexpr
frames (unit symbol generation, `in(km, rounded)` conversion chains), so contorting
predicates is a losing game - that is why the backend routes constant evaluation around
`contract_assert` entirely (see "What landed").

The smallest self-contained repro found (~35 lines, fails on r16-8246): a class template
whose contract-carrying member delegates to another member that mutates via
`std::construct_at()`, called twice inside a `consteval` IIFE initializing a `constexpr`
local (the `unit_symbol_impl` pattern). **TODO: file a GCC bug report.**

```cpp
// g++-16 -std=c++26 -fcontracts
#include <memory>
template<typename T, int N>
struct vec {
  T data_[N] = {};
  int n = 0;
  constexpr int size() const { return n; }
  constexpr T* end() { return data_ + n; }
  template<typename... Args>
  constexpr T* try_emplace_back(Args&&... args)
  {
    if (size() == N) return nullptr;
    std::construct_at(end(), std::forward<Args>(args)...);
    ++n;
    return &data_[n - 1];
  }
  template<typename... Args>
  constexpr T& emplace_back(Args&&... args)
  {
    contract_assert(size() < N);  // error: contract condition is not constant
    return *try_emplace_back(std::forward<Args>(args)...);
  }
};
consteval int make()
{
  constexpr auto text = []() consteval {
    vec<char, 8> v;
    v.emplace_back('a');
    v.emplace_back('b');  // second call triggers it
    return v;
  }();
  return text.size();
}
static_assert(make() == 2);
```

## V3 interface design questions

### 1. Single-sourcing predicates (declaration vs body)

`pre()`/`post()` attach to declarations; the gsl backends can only check inside bodies. The
pilot keeps both, so under the native backend the predicate is evaluated twice (once by
`pre()`, once by the body `contract_assert`). Options for V3:

- **(a) Duplicate.** Simple, no behaviour change for gsl users; double evaluation under
  native contracts (harmless for idempotent predicates, but sloppy).
- **(b) Body-mirror pair (current pilot).** `MP_UNITS_PRE(cond)` on the declaration
  (native-only) plus `MP_UNITS_EXPECTS(cond)` in the body (GSL-only), so exactly one
  expands per backend. Zero waste, but every precondition is written twice in the source.
- **(c) Declaration-only.** Write `MP_UNITS_PRE(cond)` only; gsl backends lose the check.
  Acceptable only if V3 drops the gsl backends and keeps `NONE`/`STD`.

Decision needed before freezing V3 signatures. If C++26 contracts are the standardization
target anyway (P3045 ships against C++29), (c) with a long transition where (b) is used for
the most valuable checks may be the right trade-off.

### 2. Which body asserts become declared preconditions

Candidates found while piloting: rounding/overflow policy checks (`overflow_policies.h`),
`inplace_vector` capacity, `fixed_string` bounds/termination, `quantity_point` origin
arithmetic, `symbol_text` invariants. Postconditions (`post(r: ...)`) have no gsl equivalent
at all, so they are pure wins wherever we add them.

### 3. Interactions still to verify

- `pre()` on constructors and on deducing-this/CRTP members (works on `fixed_string` members;
  not yet exercised on `quantity`'s deducing-this API).
- Module builds (`cxx_modules=True`) with `-fcontracts` - untested; GCC modules + contracts
  is uncharted territory.
- `import_std` + contracts: the old restriction (contracts must be `none`) existed only
  because third-party GSL headers textually include std headers, which conflict with
  `import std;`. Lifted for `std` and verified with a full gcc-16 build. It exposed a latent
  bug: `ratio.h` used the `INTMAX_MAX` macro (unavailable via `import std;`), invisible
  before only because contracts were always disabled in import_std builds; replaced with
  `std::numeric_limits<std::intmax_t>::max()`. Lesson: assert predicates are dead
  preprocessor tokens in some configurations - only a build with checks enabled validates
  them.
- Freestanding: lifted (the `NONE`-only restriction was GSL-era, like `import_std`).
  Verified: contract annotations compile with `-ffreestanding`, and the object files
  reference only `handle_contract_violation` (under `enforce`/`observe` - replaceable by
  the application without `libstdc++exp`) or just `std::terminate` (under `quick_enforce`,
  which is freestanding-viable). The build system links `stdc++exp` only in hosted builds;
  a freestanding application must provide its own handler or use `quick_enforce`.
  Caveat: a full gcc-16 freestanding library build cannot be verified right now because of
  an unrelated libstdc++ trunk regression (`#include <ranges>` with `-ffreestanding` and
  `-std=c++26` fails with "'range_format' does not name a type" in `<optional>`; C++23 and
  gcc-15 are fine) - it breaks gcc-16 C++26 freestanding builds for *any* contracts setting.
  Since `contracts=std` requires C++26, its freestanding CI coverage is disabled (TODO in
  `generate-job-matrix.py`) until upstream is fixed; the build-system support stays.
- Virtual functions and `pre` are banned in P2900 - irrelevant for mp-units (no virtuals).

### 4. `pre()`/`post()` and the GCC bug

The `if consteval` escape hatch works for in-body assertions but is impossible for
declaration `pre()`/`post()` - those always go through the compiler's contract machinery.
The `fixed_string` pilot predicates (`pos < N`, `!empty()`) survived the full build, but a
`pre()` whose predicate lands in a deep constexpr frame hits the same GCC bug with no
workaround available - verified by moving `detail::get_one`'s check to
`MP_UNITS_PRE(value != get_zero(value))`, which breaks
`static_assert((1234 * m).in(km, rounded)...)` with "contract condition is not constant".

**Consequence: the intended end state (all preconditions as `pre()` on declarations) is
blocked by the compiler, not by design.** The migration plan is:

1. Now: `contract_assert`-based backend with the `if consteval` fallback (landed), plus the
   shallow `fixed_string` pilot. File the GCC bug.
2. When GCC constant-evaluates contract predicates reliably: mechanically migrate the
   `MP_UNITS_PRECONDITION` sites to `MP_UNITS_PRE` on declarations paired with
   `MP_UNITS_EXPECTS` in the body (predicate typed twice, evaluated once per backend).
   `MP_UNITS_ASSERT*` sites are mid-function invariants and stay as contract assertions.
3. V3: if the gsl backends are retired (plausible if C++26 contracts are the P3045 target),
   drop the fallback and keep declaration-only `pre()`/`post()` - the single-source end
   state.
