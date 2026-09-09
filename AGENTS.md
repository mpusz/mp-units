# AGENTS.md

Orientation for AI coding agents working on **mp-units**, and a quick map for humans. This
file is deliberately short: it says how an agent should behave here, then points to the real
documentation instead of copying it. The full contributor guide is
[`CONTRIBUTING.md`](CONTRIBUTING.md); the design docs live in [`docs/`](docs/) and at
<https://mpusz.github.io/mp-units>.

## If you are an AI agent, read this first

mp-units is a bleeding-edge C++20/23/26 library, and the bar for a contribution does not drop
because a machine wrote the code. Follow the
[AI-generated contributions policy](CONTRIBUTING.md#ai-generated-contributions) in full. The
essentials:

- **You are the author.** Understand and be able to defend every line, however much a model
  wrote. A change nobody can explain is not ready.
- **Build and test it yourself first**, in the provided dev container, rather than leaving
  that to CI or to the reviewer. Any tests you add must encode the *intended* behavior, not
  lock in whatever the code currently does.
- **Keep the diff minimal.** No unrequested reformatting, renaming, or file-wide rewrites.
- **Watch for confident-but-wrong code**, especially around the newest language features and
  template metaprogramming, where models are weakest.
- **Propose before you build.** For anything touching concept models, type-system semantics,
  or the shape of the public API, lay out the options and their consequences and get
  agreement first. Do not implement one interpretation at a time and let the reviewer correct
  each one.

## Build, test, and conventions

All of this is documented in [`CONTRIBUTING.md`](CONTRIBUTING.md); do not reinvent it:

- Dev environment (Codespaces, dev container, Docker image):
  [Preconfigured Development Environment](CONTRIBUTING.md#preconfigured-development-environment).
- Build and test commands, including `.devcontainer/check_all.sh build` for a representative
  multi-compiler pass: [Helpful commands](CONTRIBUTING.md#helpful-commands).
- Conan and CMake options:
  [Configuration Options](CONTRIBUTING.md#configuration-options-for-developers-and-contributors).
- Naming, formatting, and backward compatibility:
  [Before committing git changes](CONTRIBUTING.md#before-committing-git-changes).

Tests live in two places, and a change usually belongs in one of them:

- `test/static/` — compile-time tests built as the `unit_tests_static` library (plus
  `unit_tests_static_truncating` for the value-truncating cases). Nothing runs; a successful
  compile *is* the pass. This is where most of the library is tested.
- `test/runtime/` — [Catch2](https://github.com/catchorg/Catch2) tests in the
  `unit_tests_runtime` executable, for behavior that only shows up when the code executes.

Whenever you touch a public header, verify it is still self-contained:

```bash
cmake --build --preset <preset> --target all_verify_interface_header_sets
```

Run the hooks on the files you changed, not the whole repository, before committing. You know
your own diff, so name the files explicitly rather than sweeping the repository:

```bash
pre-commit run --files <the files you changed>
# pre-commit run --all-files      # only when you really need a full sweep
```

## Understanding the design

Read these before changing core behavior:

- [Design overview](docs/users_guide/framework_basics/design_overview.md) — the type system
  and the core model.
- [Systems of quantities and units](docs/users_guide/framework_basics/concepts.md) — the
  fundamental concepts.
- [The character of a quantity](docs/users_guide/framework_basics/character_of_a_quantity.md)
  — scalar, vector, and tensor quantity kinds.
- [Project structure](docs/getting_started/project_structure.md) — how the repository is laid
  out.
- [C++ compiler support](docs/getting_started/cpp_compiler_support.md) — the supported matrix.

The rest of [`docs/users_guide/framework_basics/`](docs/users_guide/framework_basics/)
explains each core abstraction in depth.

## Architecture at a glance

Layered `core → systems → utility`, plus an opt-in `integrations` tier. Each layer is
consumable either as headers or as a C++20 module:

- **`mp-units::core`** (`src/core/include/mp-units/`) — the framework and math: quantities,
  quantity specs, units, dimensions, references, quantity points. The slice targeted for
  standardization, kept minimal.
- **`mp-units::systems`** (`src/systems/include/mp-units/systems/`) — concrete unit systems
  (ISQ, SI, CGS, IEC, imperial, international, USC, yard-pound, typographic, natural, HEP,
  IAU, astronomical, angular) and the CODATA constants.
- **`mp-units::utility`** (`src/utility/include/mp-units/utility/`) — non-standardizable
  add-ons: the `cartesian_vector`, `cartesian_tensor`, `polar_vector`, and `spherical_vector`
  representation types, `uncertain`, and the `random` generators.
- **`mp-units::integrations`** (`src/integrations/include/mp-units/integrations/`) — opt-in
  glue for third-party linear algebra libraries (`eigen.h`, `glm.h`, `blaze.h`), each also
  exposed as its own `mp-units::integrations-<name>` target.

The library is header-only in the sense that consuming it needs no compiled artifact, but
every layer *also* ships a module interface unit (`src/mp-units.cpp` and the per-layer
`src/*/mp-units-*.cpp`), selected by the `cxx_modules` and `import_std` Conan options. A
change that compiles in header mode can still break the module build, so do not treat the
headers as the only interface.

Public namespaces: `mp_units` (stable, standardization-targeted), `mp_units::utility` (public
extension tier), `mp_units::detail` (private; do not depend on it). Non-public helpers live
under `src/core/include/mp-units/bits/`. See
[Project structure](docs/getting_started/project_structure.md) for the full map.


## Conventions that are easy to get wrong

These are the repository's own rules, and none of them can be guessed from the surrounding
code:

- **Qualification runs in both directions.** In user-facing code — examples, documentation,
  tests — call the framework's functions *unqualified* (`round<m>(q)`, not
  `mp_units::round<m>(q)`): ADL finds the right overload, whereas a `std::` alternative fails
  SFINAE on an mp-units argument. In the framework implementation do the opposite and fully
  qualify every call, which speeds up lookup and cuts compile times. The exception
  either way is a hidden friend: only ADL can find one, so those are always called
  unqualified.
- **No one-letter lowercase locals or parameters.** `unit_symbols` defines `m`, `s`, `l`,
  `h`, and others, so a one-letter local collides with one of them and MSVC reports it
  as a shadowing warning (C4456–C4459) — which never reproduces under GCC or Clang. Use
  descriptive names, and `lhs`/`rhs` for the two sides of a binary operator. Single-uppercase
  template parameters (`T`, `Rep`, `To`) are idiomatic and fine.
- **Express preconditions as constraints, never as a hard `static_assert`.** A `static_assert`
  inside an instantiation hard-errors, so it cannot be SFINAE-probed and the misuse it guards
  becomes untestable. To keep a good diagnostic on a constraint, use
  `detail::unsatisfied<"message {}">(args...)` from `bits/unsatisfied.h`: it evaluates to
  `false` normally and throws the formatted message when `MP_UNITS_API_THROWING_CONSTRAINTS`
  is enabled. `ImplicitScaling` in `framework/quantity.h` is the canonical example.
- **Write must-not-compile tests as a custom concept**, negating that constraint and
  asserted once with `static_assert` — the `invalid_*` concepts in
  `test/static/quantity_test.cpp`. An inline `static_assert(!requires { ... })` on an
  overloaded name can hard-error inside the
  `requires` clause instead of yielding `false`.
- **A forcing cast is a diagnosis, not a fix.** When `value_cast`, `quantity_cast`, or an
  explicit quantity-spec conversion looks necessary, find out *why* the strict version is
  refused — probe the specs with `implicitly_convertible`. The cause is normally wrong physics
  in the calling code, a quantity definition that needs fixing, or a real gap in the framework
  worth reporting. Never widen a specific spec to a generic one just to make an expression
  compile.
- **`bits/` exports nothing.** A header under a `bits/` subdirectory carries
  implementation details only, so no `MP_UNITS_EXPORT` or `MP_UNITS_EXPORT_BEGIN` may
  appear there. Anything users can see, or that an exported declaration has to name,
  belongs in a public directory such as `framework/`. This is about the directory, not
  the namespace: an entity can stay in `mp_units::detail` while living in a public
  header, since being reachable to importers and being supported API are different
  things.
- **Never put an `#include` in the middle or at the end of a header.** When splitting a
  large header, decompose it into self-contained components in a linear dependency chain
  plus a thin aggregator whose includes all sit at the top, and keep the public aggregator
  path stable so existing include sites are unaffected. Propose the layout before carrying
  out such a split.

## Git and GitHub

- **Close issues from the commit message.** Put `Resolves #XXX` (or `Closes`/`Fixes`) in the
  commit body rather than closing the issue by hand, so that GitHub closes it when the change
  actually reaches `master` — not before, and not without it.
- **Markdown that GitHub renders keeps one paragraph per line.** Issue and pull request
  bodies, review comments, and job summaries are GitHub-flavored Markdown with hard line
  breaks enabled, so a newline inside a paragraph arrives as a `<br>` and source-wrapped prose
  renders as a ragged column. Only checked-in `docs/**/*.md` is wrapped, to 90 columns, where
  MkDocs joins the soft breaks back together.
