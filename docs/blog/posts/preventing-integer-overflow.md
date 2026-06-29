---
date: 2026-04-11
authors:
 - mpusz
categories:
 - Metrology
comments: true
---

# Preventing Integer Overflow in Physical Computations

Integers overflow. That is not a controversial statement. What _is_ surprising
is how easily overflow can hide behind the abstraction of a units library.

Most developers immediately think of **explicit or implicit scaling operations** —
calling `.in(unit)` to convert a quantity, constructing a quantity from a different
unit, or assigning between quantities with different units. These are indeed
places where overflow can occur, and the library cannot prevent it at compile time
when the values are only known at runtime. But at least these operations are
**visible in your code**: you wrote the conversion, you asked for the scaling,
and you can reason about whether the multiplication or division might overflow
your integer type.

The far more insidious problem is what happens when you _don't_ ask for a conversion.

When you write `1 * m + 1 * ft`, the library must automatically convert both
operands to a common unit before performing the addition. That conversion —
which you never explicitly requested — involves multiplication or division by
scaling factors. With integer representations, those scaling operations can
overflow silently, producing garbage results that propagate through your
calculations undetected.

**No compile-time programming can prevent this.** The values are only known at
runtime. But very few libraries provide proper tools to detect it.

This article explains why that limitation is real, how other libraries have
tried to work around it, and what **mp-units** provides to close the gap as
tightly as the language allows.

<!-- more -->

## The Landscape: No Library Provides Full Compile Time Coverage

### What libraries _can_ catch at compile time?

Only a very small number of units libraries — notably **mp-units** and **Au** —
will refuse to compile conversions where the scaling factor is known to overflow
the representation type:

```cpp
quantity length = std::int8_t{2} * m;
quantity<mm, std::int8_t> length_mm = length;    // ❌ Compile-time error!
// 2 m → 2000 mm; ×1000 is guaranteed to overflow int8_t (max 127)
```

This is useful and important. It catches the "static overflow" where both the
magnitude of the conversion factor and the magnitude of the stored value are
fixed at compile time and the result definitely doesn't fit.

!!! info "Comparing mp-units and Au Approaches"

    Both libraries take different approaches to compile-time overflow protection, each with its own tradeoffs.
    It's difficult to judge which is objectively better — they represent different design philosophies.

    **Au** uses an **aggressive compile-time heuristic** with a
    [threshold of 2'147](https://aurora-opensource.github.io/au/main/discussion/concepts/overflow/#adapt):
    if a value as small as 2'147 would overflow after scaling, the conversion is rejected at compile time.
    This provides **more aggressive protection** — catching potential overflows earlier — but produces
    **false positives** (blocking valid conversions). This is why Au provides
    [escape hatches](https://aurora-opensource.github.io/au/main/discussion/concepts/conversion_risks/#opting-out-of-safety-checks)
    like passing `ignore(OVERFLOW_RISK)` to `.as()`: for example,
    `seconds(4u).as(nano(seconds), ignore(OVERFLOW_RISK))`. Once the opt-out is used, that
    particular conversion is no longer checked. Au also provides runtime checking functions like
    [`is_conversion_lossy()`](https://aurora-opensource.github.io/au/main/discussion/concepts/conversion_risks/#provided-checkers)
    for runtime checks on specific conversions.

    **mp-units** uses a **more conservative approach** — the only value that survives a
    definitely-overflowing conversion is zero, so blocking it is unambiguous and requires
    no opt-out. When **mp-units** fires a compile-time error, it **always prevents a real error**
    (no false positives), but it accepts some conversions that might overflow at runtime
    depending on the actual value. This is why **mp-units** provides `safe_int<T>` — a
    representation wrapper that detects all arithmetic overflow at runtime, regardless of
    where it occurs.

    For a comprehensive discussion of overflow risks and approaches, see Au's
    [Overflow Discussion](https://aurora-opensource.github.io/au/main/discussion/concepts/overflow/).

### What no library can catch at compile time?

The obvious case is plain arithmetic overflow:

```cpp
quantity<cm, std::int16_t> dist = 100 * m;           // 10'000
quantity<cm, std::int16_t> dist2 = dist + dist;      // 20'000 — fine
quantity<cm, std::int16_t> dist4  = dist2 + dist2;   // ⚠️ 40'000 — overflows int16_t!
```

The values involved in arithmetic operations are only known at runtime,
so the compiler cannot predict whether overflow will occur:

```cpp
quantity<m, std::int8_t> small = 100 * m;
quantity<m, std::int8_t> overflow = small * 2;  // ⚠️ Runtime overflow! No compile-time detection possible
```

**No general-purpose units library can prevent all cases of runtime arithmetic
overflow at compile time.** Compile-time programming can only detect when
conversions or scaling factors themselves are guaranteed to overflow the
representation type. But for general arithmetic, the actual values are only
known at runtime — this is fundamentally impossible in C++.

!!! info

    For more context, see
    [Understanding Safety Levels: Representation Safety](understanding-safety-levels.md#level-3-representation-safety).

### The Hidden Danger: Automatic Common-Unit Scaling

The overflow risks mentioned above are serious, but at least they're **visible**. When you
write `distance * 2` or `length.in(mm)`, you're explicitly performing an operation that
involves your values. You can reason about whether the multiplication might overflow, or
whether the conversion factor is safe for your specific use case. The overflow potential
is right there in your code, tied to an operation you chose to write.

Far more dangerous is **automatic common-unit scaling** — hidden conversions that happen
transparently when you combine quantities with different units, without any explicit
request from you.

When you add or compare quantities with different units, the library must convert
them to a **common unit** before performing the operation. This conversion happens
**automatically and silently** — you write `meters + feet`, but internally the
library picks a common representation unit and scales both operands to match.

The choice of common unit is implementation-defined. It might be the first operand's
unit, the result type's unit, or something else entirely. What matters is that
**scaling always happens**, and with integer representations, that scaling can overflow.

To see how this works concretely, consider `1 * m + 1 * ft`. Since 1 ft = 0.3048 m =
381/1250 m exactly, the common unit is the GCD of their representations: **m/1250**
(or equivalently **ft/381**). To perform the addition, the library scales both operands:

- `1 * m` becomes `1 × 1250 = 1250` common units
- `1 * ft` becomes `1 × 381 = 381` common units  
- Result: `1631` common units = `1631/1250 m` ≈ `1.3048 m`

Those multiplications by `1250` and `381` are automatic, invisible, and with
integer types, can overflow during the intermediate computation — even when the
final result would fit perfectly in the representation type.

!!! info "Not Just Meters and Feet"

    If mixing meters and feet seems niche, consider everyday scenarios like **kilograms ↔ pounds**
    (scaling factor: 100 million) or **liters ↔ US gallons** (scaling factor: 125 million). These
    common conversions hit overflow thresholds **80-100× sooner** than meters/feet!

    The problem becomes even more acute for **higher-dimensional quantities**. When dealing with
    areas and volumes, the scaling factors are **squared or cubed**:

    - **Linear** (m ↔ ft): scaling factor 1,250
    - **Areas** (m² ↔ ft²): scaling factor (1,250)² = 1,562,500
    - **Volumes** (m³ ↔ ft³): scaling factor (1,250)³ ≈ 1,953,125

    We use meters/feet throughout this article because the more modest linear factor (1,250) makes
    these progressions easier to follow pedagogically.

Consider this example with `int16_t` (max value: 32,767):

```cpp
quantity<m, std::int16_t> meters = std::int16_t{20'000} * m;
quantity<ft, std::int16_t> feet = std::int16_t{10'000} * ft;  // 1 ft = 381/1250 m
quantity total = meters + feet;
std::cout << "Result: " << total.force_in(m) << '\n';

// Common unit is m/1250 (or equivalently ft/381) — GCD of the unit representations
// Scaling to common unit:
//   20'000 m  → 20'000 × 1'250 = 25'000'000 (overflows int16_t!)
//   10'000 ft → 10'000 × 381   =  3'810'000 (overflows int16_t!)
// Sum in common unit: 28'810'000            (overflows int16_t!)
// Output: Result: 23048 m                   (fits int16_t!)
```

**This is the hidden danger.** You wrote a simple addition that looks safe. Both input
values (20,000 and 10,000) fit comfortably in `int16_t`. The final answer (23,048 m)
also fits in `int16_t`. Yet the intermediate scaling arithmetic — which the library
performs automatically and invisibly — overflows `int16_t`:

- `20'000 × 1'250 = 25'000'000` (far exceeds `int16_t` max of 32,767)
- `10'000 × 381 = 3'810'000` (also exceeds `int16_t` max)

What happens next depends entirely on the library's design choices:

1. **What type is used for the intermediate multiplication?** (`int16_t`? `int64_t`? `__int128`?)
2. **What type is the result?** (Does it respect C++ integral promotion rules, or use `std::common_type`?)
3. **Is overflow detected, or does it silently produce wrong results?**

Different libraries make different tradeoffs. Some produce undefined behavior. Some
violate C++ arithmetic rules. Some get the right answer. Some detect the overflow and
report an error.

---

## Approach 1: `std::common_type` (e.g., `std::chrono::duration`)

Many libraries follow [`std::chrono::duration`'s precedent](https://en.cppreference.com/w/cpp/chrono/duration/operator_arith.html)
and use [`std::common_type`](https://en.cppreference.com/w/cpp/chrono/duration/common_type.html)
to determine both the intermediate and result types for arithmetic:

```cpp
// std::common_type<int16_t, int16_t> = int16_t
// No integral promotion, no widening for intermediate calculations
auto total = meters + feet;  // decltype(total) has rep type int16_t
// Computation: (20'000 * 1'250) + (10'000 * 381) — both overflow int16_t → UB
```

This approach has two critical problems:

1. **Violates C++ integral promotion rules**: `int16_t + int16_t` returns `int` in C++,
   but `std::common_type<int16_t, int16_t>` is `int16_t`, forcing a narrowing conversion
2. **No intermediate widening**: The scaling multiplication `v * num` is computed directly
   in `int16_t`, causing immediate undefined behavior when it overflows

In our example, what would actually happen if a units library followed
`std::chrono::duration`'s interface? The intermediate arithmetic would overflow `int16_t`
(undefined behavior), and even if you somehow got past that, the final narrowing
conversion would produce garbage:

```cpp
// Hypothetical std::common_type-based library behavior:
auto total = meters + feet;      // decltype(total)::rep = int16_t (not int!)
std::cout << "Result: " << total.force_in(m) << '\n';
// Output: Result: -20 m ❌ (or anything — it's UB!)
```

The value `-20 m` comes from the overflow wrapping at the narrowing conversion. But this is
being generous — the intermediate multiplication itself invokes undefined behavior, so the
compiler is free to produce literally any result (or crash).

---

## Approach 2: **mp-units 2.5.0** — Two-Layer Protection

**mp-units** (up to version 2.5.0) introduced two separate improvements over Approach 1,
addressing two distinct categories of operations:

1. **Arithmetic operations** (addition, subtraction, multiplication, division between quantities):
   the result type follows C++ integral promotion rules
2. **Unit conversions** (explicit conversions like `.in()`, and implicit conversions during
   common-unit scaling): intermediate arithmetic is widened to prevent UB

These improvements are independent and address different aspects of overflow safety.

### Integral Promotion for Arithmetic Operations (Result Types)

**mp-units** [correctly respects integral promotion rules](https://en.cppreference.com/w/cpp/language/implicit_conversion#Integral_promotion)
for the *result type* of arithmetic operations:

```cpp
static_assert(std::is_same_v<decltype(total)::rep, int>);
// Result type is int, not int16_t — consistent with C++!
```

This behavior is **consistent with Au** and other well-designed units libraries — when you
perform arithmetic on quantities with small integer types (`int8_t`, `int16_t`), the result
uses the promoted type (`int`) just like C++ arithmetic. This matches the "Do as `ints` do"
principle.

Such **result type promotion** provides an important safety margin: even before considering
intermediate arithmetic, the result is stored in a wider type (`int` instead of `int16_t`).
This prevents overflow for typical use cases at the `int8_t` and `int16_t` levels. For larger
types like `int`, users working with extreme values typically scale to appropriate units or
prefixes first, naturally limiting the scope of overflow issues.

### Widened Intermediate Arithmetic for Unit Conversions (`intmax_t`)

However, integral promotion rules provide no help if the source arguments already use `int`
or larger types — there's no wider result type to promote to. And even for smaller types,
the result type alone doesn't prevent undefined behavior during the **intermediate** scaling
arithmetic *within* the conversion.

**This is where mp-units and Au differ.** While both libraries use promoted types for
arithmetic operations, they take different approaches to unit conversion scaling:

- **Au** performs unit conversions in the promoted type (benefiting from integer promotion
  for the smallest types like `int8_t` and `int16_t`), but doesn't widen beyond that
- **mp-units 2.5.0** explicitly widens intermediate calculations to `std::intmax_t`
  (typically `int64_t`), even when working with smaller types like `int16_t`

mp-units' widening approach addresses the intermediate scaling issue:

```cpp
// Pseudo-code for mp-units 2.5.0 scaling:
auto scale(int16_t v, intmax_t num, intmax_t den)
{
  // Intermediate: widened to intmax_t to prevent UB
  auto intermediate = static_cast<intmax_t>(v) * num / den;
  // Result: cast back to expected type (following integral promotion)
  return static_cast<int>(intermediate);
}
// For our example (using intmax_t = int64_t):
//   20'000 * 1'250 = 25'000'000 (computed in int64_t, prevents UB) ✓
//   10'000 * 381   =  3'810'000 (computed in int64_t, prevents UB) ✓
// Result: 23'048 m as int (fits) ✓
```

The key insight: **the intermediate product `value × numerator` can overflow even when the
final result `(value × numerator) / denominator` fits**. For example, converting feet to
metres multiplies by 3048 before dividing by 10000 (since 1 ft = 3048/10000 m exactly).
Widening to `intmax_t` prevents undefined behavior during this intermediate multiplication.

??? question "Why not divide first?"

    A natural question: could we avoid overflow by reordering the calculation to
    `(value / denominator) × numerator` when the value is divisible by the denominator?
    Or use compile-time heuristics to choose the safer ordering?

    **mp-units explored this approach** but ultimately rejected it due to unresolved corner
    cases and complexity:

    1. **Correctness is fragile**: The "divide first" optimization only works when `value %
       denominator == 0`. Otherwise, you lose precision from integer truncation. Detecting
       this at compile time is impossible; checking at runtime adds branching overhead to
       every conversion.

    2. **Partial solutions don't help**: Even when division is safe for one operand, you
       still need widened arithmetic for the other operand in mixed-unit operations
       like `meters + feet`. You can't eliminate the widening infrastructure.

    3. **Increased complexity**: Compile-time heuristics to choose ordering (e.g., "multiply
       first if numerator is smaller") create unpredictable behavior where identical operations
       on different types have different performance characteristics and failure modes.

    4. **Still fails for extreme values**: When `value × numerator` would overflow `intmax_t`,
       no reordering solves the problem — you fundamentally need **wider** intermediate types.
       The divide-first optimization merely postpones overflow; it doesn't eliminate it.

    **The widened fixed-point approach** is simpler, more predictable, and covers all
    cases uniformly: it **greatly reduces UB risk** by widening intermediate multiplication,
    regardless of value divisibility or the specific types involved. While extreme scaling
    factors (e.g., astronomical units to nanometers) can still overflow even `__int128`,
    this handles the vast majority of real-world scenarios safely.

For our `int16_t` example, this works perfectly — the intermediate `intmax_t` calculation
prevents UB, and the final value fits in the promoted `int` type.

This approach eliminates UB for most practical cases, but has a critical limitation with
**`int64_t`**: since `intmax_t` typically **is** `int64_t` on most platforms, widening
provides no benefit.

Consider converting a large volume from liters to US gallons (1 L = 1000000/3785411784 gal
exactly):

```cpp
quantity<L, std::int64_t> volume = 3'689'348'814'741LL * L;  // 3.7×10¹² L
quantity in_gal = volume.in(gal);
// Scaling: value × 1000000 / 3785411784
// Intermediate: 3.7×10¹² × 10⁶ = 3.7×10¹⁸ (within int64_t range)
// Result: 3.7×10¹⁸ / 3.79×10⁹ ≈ 974'667'730'000 gal (fits in int64_t) ✓
// mp-units 2.5.0: 0 gal ❌ (overflow produces wrong result)
```

The final result (~975 billion gallons) fits comfortably in `int64_t`, but the
intermediate multiplication overflows. Since `intmax_t` is `int64_t`,
widening provides no protection, resulting in a nonsensical `0 gal` output.

**Comparisons face the same issue.** When comparing quantities with different units, the
library must scale to a common unit internally:

```cpp
quantity<L, std::int64_t> vol1 = 5'000'000'000'000LL * L;  // 5 trillion liters
quantity<gal, std::int64_t> vol2 = 3'000'000'000LL * gal;  // 3 billion gal ≈ 11.4B L  
bool greater = (vol1 > vol2);  // Should be true (5T L >> 11.4B L)
// Common unit: L (vol1's unit)  
// Intermediate for vol2: 3×10⁹ × 3'785'411'784 / 10⁶ (overflows int64_t)
// mp-units 2.5.0: UB ❌ (intermediate overflow produces unpredictable comparison)
```

Even though the comparison only returns `bool`, the intermediate scaling for `vol2` causes
undefined behavior in 2.5.0, potentially producing incorrect comparison results.

!!! info "Real-World Context"

    While floating-point representations are often more practical for large-scale volumetric
    calculations, there are legitimate scenarios where `int64_t` is preferred:

    **Exact integer arithmetic requirements**: Systems that need precise, reproducible
    calculations without floating-point rounding errors (e.g., billing systems, regulatory
    reporting, distributed consensus algorithms).

    **Domains using `int64_t` for large volumes**:

    - **Water resource management**: reservoir systems, lake volumes (Lake Tahoe: ~150 trillion gallons)
    - **Oil & gas industry**: reserves, production volumes, daily global consumption (~35 billion gallons/day)
    - **Environmental modeling**: oceanic or atmospheric volumes, climate simulations
    - **Infrastructure**: municipal water systems, large-scale fluid distribution networks

    When an application deliberately chooses `int64_t` for volume quantities (rather than
    `double`), it's precisely because it needs exactness or compatibility with integer-based
    protocols. The overflow problem in unit conversions is not theoretical — it affects real
    calculations in these domains when different unit systems are mixed (liters vs gallons,
    cubic meters vs cubic feet).

---

## Approach 3: **mp-units** — widened intermediate arithmetic

**mp-units** improves upon the 2.5.0 approach by using **widened intermediate arithmetic
specifically for unit conversion scaling operations**: `int64_t` for all types up to `int32_t`,
and 128-bit arithmetic for `int64_t`. This widening applies when the library performs
internal scaling to convert between units (both explicit conversions like `.in()` and
implicit conversions during common-unit operations like `meters + feet`), but **not** to
plain arithmetic operations, which follow standard C++ integral promotion rules.

```cpp
// Pseudo-code for mp-units scaling with int16_t:
auto scale(int16_t v, int16_t num, int16_t den)
{
  // Factor widened to int64_t; C++ promotion widens v automatically in the product.
  // For wrapper types (safe_int, cartesian_vector), the wider factor propagates
  // through the type's own operator*, preserving overflow checks / element-wise ops.
  auto intermediate = v * static_cast<std::int64_t>(num) / den;
  // Result in promoted type (int for int16_t), following integral promotion rules
  return static_cast<int>(intermediate);
}
```

The key improvement is **widened intermediate calculations**:

- `int8_t`, `int16_t`, `int32_t` → intermediate calculation in `int64_t`
- **`int64_t` → intermediate calculation in 128-bit type** (`__int128` when available, or
  a custom 128-bit type otherwise)

!!! note "128-bit Arithmetic Portability"

    When `__int128` is not available (e.g., on some compilers or architectures), **mp-units**
    uses a custom 128-bit integer type that simulates the same behavior. This ensures consistent
    overflow protection across all platforms, though native `__int128` support provides better
    performance when available.

**Result types** still follow integral promotion rules (same as Approach 2). The key difference
is that current **mp-units** provides **better intermediate protection** — particularly crucial
for `int64_t`, where 2.5.0's `intmax_t` widening provided no benefit (since `intmax_t` typically
*is* `int64_t`), but 128-bit widening does.

!!! info

    For implementation details, see [Built-in Scaling Algorithm](../../users_guide/framework_basics/representation_types.md#built-in-scaling-algorithm).

??? question "Why not just use floating-point?"

    If integer overflow is such a problem, why not convert to `double` for the scaling arithmetic?
    The answer comes down to **precision, exactness, and platform constraints**:

    1. **Lossless conversions must stay exact**: `42 * m` converted to `mm` must give exactly
       `42000 * mm`. Integer multiplication achieves this; going via `double` would produce correct
       results for small values but lose exactness near the precision limit (53-bit mantissa ≈ 9×10¹⁵).

    2. **Rational factors introduce rounding errors**: Converting feet to metres (×3048/10000) using
       floating-point arithmetic introduces rounding because 0.3048 is not exactly representable in
       binary floating-point. Integer arithmetic preserves the exact rational calculation.

    3. **Platform and policy constraints**: Many embedded platforms lack FP hardware (making FP
       operations slow and unpredictable via software emulation), and some projects enforce no-FP
       policies. Widened integers respect the user's choice of integer representation throughout
       unit conversions.

    **The core insight**: The intermediate product `value × numerator` can overflow even when the final
    result `(value × numerator) / denominator` fits perfectly in the target type. Widened integers
    absorb that intermediate growth while preserving exactness.

This is particularly valuable for **`int64_t`**, where 2.5.0's `intmax_t` widening provided
no benefit, but 128-bit widening does. Recall the liters-to-gallons conversion that failed
in 2.5.0:

=== "mp-units (current)"

    ```cpp
    quantity<L, std::int64_t> volume = 3'689'348'814'741LL * L;  // ~3.7×10¹² L
    quantity in_gal = volume.in(gal);
    // Intermediate: 3.69×10¹² × 10⁶ = 3.69×10¹⁸
    // Result: 3.69×10¹⁸ / 3.79×10⁹ ≈ 974'667'730'000 gal (fits in int64_t) ✓
    // Output: 974'667'730'000 gal ✓ (intermediate computed in 128-bit type)
    ```

=== "mp-units 2.5.0"

    ```cpp
    quantity<L, std::int64_t> volume = 3'689'348'814'741LL * L;  // ~3.7×10¹² L
    quantity in_gal = volume.in(gal);
    // Intermediate: 3.69×10¹² × 10⁶ = 3.69×10¹⁸ (overflows int64_t!)
    // Result: 3.69×10¹⁸ / 3.79×10⁹ ≈ 974'667'730'000 gal (fits in int64_t) ✓
    // Output: 0 gal ❌ (intermediate overflow produces wrong result)
    ```

**Comparisons benefit even more** from 128-bit arithmetic. Since they only return `bool`,
**mp-units** can freely use widened intermediate arithmetic internally without affecting
the API:

=== "mp-units (current)"

    ```cpp
    quantity<L, std::int64_t> vol1 = 5'000'000'000'000LL * L;  // 5 trillion liters
    quantity<US::gal, std::int64_t> vol2 = 3'000'000'000LL * US::gal;  // 3B gal ≈ 11.4B L
    bool greater = (vol1 > vol2);  // Should be true (5T L >> 11.4B L)
    // Common unit: L (vol1's unit)
    // Intermediate for vol2: 3×10⁹ × 3'785'411'784 / 10⁶ (overflows int64_t)
    // Output: true ✓ (intermediate computed in 128-bit type, result is bool)
    ```

=== "mp-units 2.5.0"

    ```cpp
    quantity<L, std::int64_t> vol1 = 5'000'000'000'000LL * L;  // 5 trillion liters
    quantity<US::gal, std::int64_t> vol2 = 3'000'000'000LL * US::gal;  // 3B gal ≈ 11.4B L
    bool greater = (vol1 > vol2);  // Should be true (5T L >> 11.4B L)
    // Common unit: L (vol1's unit)
    // Intermediate for vol2: 3×10⁹ × 3'785'411'784 / 10⁶ (overflows int64_t)
    // Output: UB ❌ (intermediate overflow, result unpredictable)
    ```

128-bit widening is not just theoretical — it's essential for correctness when comparing
quantities with very different unit magnitudes. The following examples show cases where
**both operand values fit comfortably in `int64_t`**, but the common-unit scaling factors
are so large that the intermediate results would overflow `int64_t` without 128-bit
arithmetic:

```cpp
// lb vs kg: common unit is their GCD (≈ 1.7×10⁻⁷ g), producing huge scale factors.
//   47 ×  45'359'237 = 2'131'884'139 (0x7F11'F86B) — largest value that fits int32
//   47 × 100'000'000 = 4'700'000'000 (0x1'1824'4F00) — truncated to 0x1824'4F00 in int32
static_assert(47 * isq::mass[lb] < 47 * isq::mass[si::kilogram]);

// 10¹⁴ easily fits int64 (range ~10¹⁹), but scaled to GCD it overflows int64:
//   100'000'000'000'000 (10¹⁴) × factor_lb ≈ 4.54×10²¹ > INT64_MAX
static_assert(100'000'000'000'000 * isq::mass[lb] ==
              45'359'237'000'000 * isq::mass[si::kilogram]);

// mi vs km: common unit is 64 mm (GCD of 1'609'344 mm and 1'000'000 mm)
//   km→cu scale factor = 15'625, mi→cu scale factor = 25'146
//   590'295'810'380'364 × 15'625 ≈ 9.223×10¹⁸ > INT64_MAX
//   366'792'811'468'750 × 25'146 ≈ 9.223×10¹⁸ > INT64_MAX
static_assert(590'295'810'380'364LL * isq::length[km] ==
              366'792'811'468'750LL * isq::length[mi]);
static_assert(590'295'810'380'364LL * isq::length[km] >
              366'792'811'468'749LL * isq::length[mi]);
static_assert(590'295'810'380'364LL * isq::length[km] <
              366'792'811'468'751LL * isq::length[mi]);
```

In each case, **mp-units uses `__int128`** (or equivalent 128-bit type) for the
intermediate scaling arithmetic, ensuring the comparison is mathematically correct even
when the scaled values exceed `INT64_MAX`. Without this widening, the comparisons would
silently produce wrong results.

---

## Remaining limitations

While widened intermediate arithmetic (128-bit for `int64_t`) prevents UB in the vast
majority of practical scenarios, it cannot eliminate **all** overflow risks:

**1. Extreme runtime values with large scaling factors.** While 128-bit arithmetic provides
enormous headroom (±10³⁸), it's not unlimited. When runtime values approach `int64_t` limits
and scaling factors are large (thousands or millions), even 128-bit intermediate arithmetic
can overflow. This is rare in practice, since applications needing such extreme ranges
typically use floating-point or choose units that minimize scaling factors.

**2. Overflow in final arithmetic after successful scaling.** This is the most insidious
limitation. Even when intermediate scaling succeeds, the final arithmetic operation can
overflow. Recall from
["What no library can catch at compile time?"](#what-no-library-can-catch-at-compile-time)
that plain arithmetic overflow is fundamentally unavoidable at compile time. This applies
equally to the results of common-unit conversions:

```cpp
// Large volumes in different units
quantity<m3, std::int64_t> vol_m3 = 4'800'000'000'000LL * m3;  // 4.8 trillion m³
quantity<pow<3>(ft), std::int64_t> vol_ft3 = 169'000'000'000'000LL * pow<3>(ft);
quantity total = vol_m3 + vol_ft3;

// Common unit: m³/1953125 (or ft³/55306341)
// Intermediate scaling (both safe in __int128):
//   4.8×10¹² × 1'953'125 = 9.375×10¹⁸ ✓
//   169×10¹² × 55'306'341 = 9.347×10¹⁸ ✓
// But final sum in common unit:
//   9.375×10¹⁸ + 9.347×10¹⁸ = 1.87×10¹⁹ (exceeds int64_t max!) ❌
// Result: -1673605815194890240 [(1/1953125 × 10⁻³ m³), (1/55306341 ft³)]
// (should be ~9.6 trillion m³)

// mp-units (current): Silent overflow ❌ (intermediate scaling safe, but final sum overflows)
```

The 128-bit intermediate arithmetic successfully prevents UB during the **unit conversion
scaling**, but cannot prevent overflow when the **sum of the scaled values** exceeds `int64_t`.
The library must return a quantity in a type the user can work with, so widening the final
result indefinitely isn't viable.

These limitations motivate **`safe_int<T>`**: a representation wrapper that detects overflow
**at all stages** — not just intermediate scaling, but also final arithmetic operations.

---

## Summary of approaches

| Approach            | Respects integral promotion? | Intermediate type           | UB during unit scaling?          | Detects overflow?      |
|---------------------|------------------------------|-----------------------------|----------------------------------|------------------------|
| `std::common_type`  | ❌ No                         | Same as input (`int16_t`)   | ✅ Yes (common)                   | ❌ No                   |
| **mp-units 2.5.0**  | ✅ Yes                        | Widened to `intmax_t`       | ⚠️ If value × factor > `int64_t` | ❌ No                   |
| **mp-units**        | ✅ Yes                        | Widened (`int64_t`/128-bit) | ⚠️ If value × factor > 128-bit   | ❌ No                   |
| **+ `safe_int<T>`** | ✅ Yes                        | Widened + overflow checks   | ❌ No (throws/terminates)         | ✅ Yes (all operations) |

The progression is clear:

- **2.5.0**: Eliminates UB in intermediate scaling for most types (up to `int32_t` without
  issues)
- **Current mp-units**: Extends intermediate scaling protection to `int64_t` via 128-bit
  arithmetic — prevents UB in the vast majority of practical unit conversions, but cannot
  protect against final arithmetic overflow or extreme scaling factors
- **Current mp-units + `safe_int<T>`**: Comprehensive protection — detects overflow in
  **all** operations: intermediate scaling, final arithmetic, extreme values, and mixed-unit
  operations

For most engineering applications, mp-units' widened intermediate arithmetic is sufficient.
When you need **guaranteed detection** of all overflow scenarios — including edge cases with
extreme values or mixed-unit operations that overflow after scaling — use `safe_int<T>`.

---

## How Au Addresses the Problem

Before examining **mp-units**' approach, it's worth highlighting
[Au](https://aurora-opensource.github.io/au/main/) — one of the very few units libraries
that takes integer overflow seriously and implements concrete safeguards. While most libraries
in the ecosystem provide no overflow protection beyond compile-time detection of extreme
scaling factors, Au stands out for actively addressing this challenge.

Au and **mp-units** make different design choices in how they tackle overflow, reflecting
different philosophies about where safety checks should live and how errors should be handled.
Neither approach is universally "better" — they represent valid tradeoffs optimized for
different use cases and development contexts. Examining Au's solutions helps illustrate the
design space and motivates **mp-units**' complementary approach.

The [Au library](https://aurora-opensource.github.io/au/main/) takes two
approaches to partial mitigation:

### 1. The Overflow Safety Surface (compile-time heuristic)

Au blocks unit conversions where the scaling factor is large enough that a
"typical" value would overflow. Specifically, it uses a
[threshold of 2'147](https://aurora-opensource.github.io/au/main/discussion/concepts/overflow/#adapt):
if a value as small as 2'147 would overflow after scaling, the conversion is
rejected at compile time.

This is a step beyond what most libraries do. The "overflow safety surface"
visualises the threshold for each integer type, giving a useful picture of which
conversions are safe to perform implicitly.

**Scope of the heuristic:**

- The threshold is deliberately conservative — it may block some valid conversions. Au
  [provides opt-outs](https://aurora-opensource.github.io/au/main/discussion/concepts/conversion_risks/#opting-out-of-safety-checks)
  like `ignore(OVERFLOW_RISK)` or `ignore(TRUNCATION_RISK)` for these cases.
- Multi-step conversions are checked independently:
  `meters(10u).as(milli(meters)).as(nano(meters))` passes because each step is within
  the threshold, even though the single-step equivalent would be caught.
- The safety surface covers unit _conversions_, not general arithmetic; the inner-loop
  computation `distance + distance` is outside its scope.
- Automatic common-unit scaling (e.g. `m + ft`) is verified at compile time against the
  threshold, but actual runtime *values* are not examined — a scaling factor that passes
  the threshold test can still overflow at runtime with a large enough value.

### 2. Runtime conversion checkers (`is_conversion_lossy`)

Au provides
[`is_conversion_lossy(quantity, target_unit)`](https://aurora-opensource.github.io/au/main/discussion/concepts/conversion_risks/#provided-checkers)
and related predicates (`will_conversion_overflow`, `will_conversion_truncate`) that tell
you whether a specific runtime value would overflow or truncate on a given conversion.
Users can write their own wrapper combining runtime checks with opt-outs:

```cpp
template<typename Target, typename U, typename R>
std::optional<Quantity<TargetUnit, R>> checked_conversion(Quantity<U, R> q, Target u)
{
  return will_conversion_truncate(q, u)
      ? std::nullopt
      : std::make_optional(q.as(u, ignore(TRUNCATION_RISK)));
}
```

This is accurate and efficient for the specific case of unit conversions. It
does not cover arithmetic overflow, and at the time of writing, it also
[does not yet cover `QuantityPoint`](https://github.com/aurora-opensource/au/issues/352).

The two mechanisms have different scopes:

- **The overflow safety surface**: Applies to **all** conversions — both explicit (like
  `.as(unit)`) and implicit (automatic common-unit scaling inside operations like
  `meters + feet`). It examines the scaling factor itself, blocking conversions where
  even a small value like 2'147 would overflow. However, while explicit conversions can
  be opted out using `ignore(OVERFLOW_RISK)`, **there is no way to specify opt-outs for
  implicit conversions** — this means false positives in automatic common-unit scaling
  cannot be bypassed, potentially blocking valid operations.

- **`is_conversion_lossy` and related checkers**: Focus on **explicit** conversion calls
  made by the user. They check the actual runtime value to determine if *that specific*
  conversion would overflow or truncate, allowing precise per-value validation.

---

## The mp-units Approach: `safe_int<T>`

**mp-units** takes a different, complementary approach. Rather than adding heuristics
to the library's conversion machinery, we provide a **representation-type wrapper**
— [`safe_int<T, ErrorPolicy>`](../../users_guide/framework_basics/safe_int.md) — that
detects _all_ arithmetic overflow, regardless of where it occurs.

### Core idea

`safe_int<T>` is a thin wrapper around an integral type `T`. It participates in
the ordinary C++ arithmetic system and satisfies all the concepts that **mp-units**
requires of a representation type. Every arithmetic operation is checked:

| Operation | Overflow condition checked                                               |
|-----------|--------------------------------------------------------------------------|
| `a + b`   | signed: both-same-sign sum crosses boundary; unsigned: `lhs > max - rhs` |
| `a - b`   | symmetric to addition                                                    |
| `a * b`   | widened multiplication; result outside `[min, max]`                      |
| `a / b`   | divide-by-zero; signed: `INT_MIN / -1`                                   |
| `-a`      | signed: `INT_MIN`; unsigned: any non-zero                                |

The check uses widened intermediate arithmetic for multiplication (e.g., `int32_t`
promotes to `int64_t` for the product), so there is no dependency on UB.

!!! warning "Runtime Overhead"

    **`safe_int<T>` adds runtime overhead to every arithmetic operation.** Each addition,
    subtraction, multiplication, division, and negation includes an overflow check that
    executes at runtime. For many applications — especially those where correctness is
    paramount or integer types are subject to promotion (`int8_t`, `int16_t`) — this cost
    is well worth paying. However, users should be aware of the performance trade-off:
    safety comes at the cost of additional instructions per operation.

    The overhead is typically small (a few extra comparisons or branches per operation), but
    in tight inner loops with millions of iterations, it can be measurable. Profile your
    specific use case to determine if the safety benefit justifies the performance cost.

### Policy-based error handling

What happens when overflow is detected is controlled by the `ErrorPolicy`
template parameter. **mp-units** ships two policies:

| Policy                      | Behaviour                    | Environment           |
|-----------------------------|------------------------------|-----------------------|
| `safe_int_terminate_policy` | `std::abort()` immediately   | freestanding + hosted |
| `safe_int_throw_policy`     | throws `std::overflow_error` | hosted only           |

The policy is a type parameter, so the choice is made once per type alias and
does not require any runtime branching beyond the overflow check itself. The default
policy is `safe_int_throw_policy` on hosted platforms and `safe_int_terminate_policy`
on freestanding platforms.

Convenience aliases with the default policy are provided:

```cpp
using safe_i16 = mp_units::utility::safe_int<std::int16_t>;  // default policy (throw on hosted, terminate on freestanding)
using safe_i32 = mp_units::utility::safe_int<std::int32_t>;  // default policy (throw on hosted, terminate on freestanding)

// For explicit policy control, use the full template:
mp_units::utility::safe_int<std::int32_t, mp_units::utility::safe_int_throw_policy> explicit_throw;
mp_units::utility::safe_int<std::int32_t, mp_units::utility::safe_int_terminate_policy> explicit_terminate;
```

All standard fixed-width integer aliases are available: `safe_i8`, `safe_i16`, `safe_i32`,
`safe_i64`, `safe_u8`, `safe_u16`, `safe_u32`, `safe_u64`.

!!! tip "Custom Error Policies"

    You can define your own error policy to integrate with custom logging or diagnostics systems:

    ```cpp
    #include <mp-units/utility/safe_int.h>

    struct logging_policy {
      [[noreturn]] static void on_overflow(std::string_view msg)
      {
        // Log overflow to custom diagnostics system before terminating
        log_critical_error("Arithmetic overflow", msg);
        std::abort();
      }

      // Optional: for quantity point bounds checking (origin bounds policies)
      [[noreturn]] static void on_constraint_violation(std::string_view msg) noexcept
      {
        log_critical_error("Constraint violation", msg);
        std::abort();
      }
    };

    using logged_int = mp_units::utility::safe_int<std::int32_t, logging_policy>;
    ```

    The policy must provide a `static void on_overflow(std::string_view)` method for arithmetic
    overflow. If you also want to use the same policy for quantity point bounds checking
    ([origin bounds policies](../../users_guide/framework_basics/the_affine_space.md#range-validated-quantity-points)),
    provide `on_constraint_violation(std::string_view)` as well.

    Custom policies are useful for integrating with application-specific error handling:
    logging to telemetry systems, sending alerts, recording diagnostics before termination, etc.

### Respecting integral promotion rules

`safe_int<T>` preserves C++ integral promotion behavior. When you perform arithmetic on
`safe_int<int16_t>`, the result type is `safe_int<int>`, not `safe_int<int16_t>` — exactly
matching what happens with the underlying types:

```cpp
// Underlying types: int16_t + int16_t → int (integral promotion)
static_assert(std::is_same_v<decltype(std::int16_t{1} + std::int16_t{1}), int>);

// safe_int preserves this: safe_int<int16_t> + safe_int<int16_t> → safe_int<int>
static_assert(std::is_same_v<decltype(safe_i16{1} + safe_i16{1}), mp_units::utility::safe_int<int>>);

// This propagates through quantity arithmetic:
static_assert(std::is_same_v<decltype(safe_i16{1} * m + safe_i16{1} * m), quantity<m, mp_units::utility::safe_int<int>>>);
```

This design ensures that `safe_int` acts as a **transparent wrapper** — it adds overflow
detection without changing the fundamental arithmetic behavior. The type system still provides
the natural widening that C++ integer arithmetic relies on for safety.

### Drop-in replacement for integral types

`safe_int<T>` satisfies all the same representation concepts as `T`, making it a true
drop-in replacement — only the representation type changes, everything else stays identical:

=== "plain int16_t"

    ```cpp
    quantity<m, std::int16_t> q{30'000 * m};
    quantity<m, std::int16_t> doubled{q + q};  // ⚠️ overflows silently
    ```

=== "safe_int<T>"

    ```cpp
    quantity<m, safe_i16> q{30'000 * m};
    quantity<m, safe_i16> doubled{q + q};  // throws std::overflow_error ✓
    ```

The overflow is caught because `q + q` promotes to `safe_int<int>` via integral promotion
(just as `int16_t + int16_t → int`), and the `quantity<m, safe_i16>` constructor then
narrows the result back to `int16_t` — that narrowing is where safe_int detects that
60,000 doesn't fit and throws.

### Coverage across all contexts

Conversion-level checkers (like Au's `will_conversion_overflow` or `is_conversion_lossy`)
examine explicit conversion calls — they are scoped to the `.as(target_unit)` /
`.in(unit)` surface. Overflow that happens inside the library's operators during automatic
common-unit scaling, or in ordinary arithmetic between same-unit quantities, is outside
their scope.

`safe_int<T>` operates at a different level. Because it hooks into the fundamental C++
arithmetic operators, every operation is checked — regardless of context:

=== "Construction"

    ```cpp
    int value = 40'000;

    // Constructing a safe_int quantity from a plain integer that doesn't fit the rep type
    quantity<m, safe_i16> q{value * m};  // throws — 40,000 > INT16_MAX (32,767)
    ```

=== "Conversion"

    ```cpp
    quantity q = 40'000 * m;

    // Converting from a quantity with too large numerical value for the rep type
    quantity<m, safe_i16> q_safe{q};  // throws — 40,000 > INT16_MAX (32,767)
    ```

=== "Arithmetic"

    ```cpp
    // Same-unit addition — no unit conversion at all, plain safe_int arithmetic
    quantity dist = safe_i32{2'000'000'000} * m;
    quantity total = dist + dist;         // throws — 4×10⁹ overflows int32_t

    // Cross-quantity multiplication: speed × time
    quantity speed = safe_i32{50'000} * m / s;
    quantity time  = safe_i32{50'000} * s;
    quantity distance = speed * time;     // throws — 2.5×10⁹ overflows int32_t
    ```

=== "Explicit .in()"

    ```cpp
    // Explicit unit conversion — also covered by conversion-level checkers
    quantity dist = safe_i32{2'200'000} * m;
    quantity huge = dist.in(um);         // throws — ×1,000,000 factor overflows int32_t
    ```

=== "Implicit scaling"

    ```cpp
    // Automatic common-unit scaling — outside conversion-level checkers' scope
    quantity dist_m  = safe_i32{1'500'000'000} * m;
    quantity dist_km = safe_i32{1'000'000} * km;
    quantity total = dist_m + dist_km;    // throws — scaling 10⁶ km → m overflows int32_t
    ```

Conversion-level checkers cover the "Explicit `.in()`" case. The other contexts —
narrowing construction, same-unit arithmetic, and implicit scaling inside `+` — are
outside their scope but caught by `safe_int` at the point they occur.

### A note on comparisons

Comparison operators (`==`, `<`, `>`, etc.) are structurally safer than arithmetic: they
return only `bool`, so the widened intermediate values are used to produce the boolean
result and then discarded. Because the intermediate never becomes a user-visible value,
**mp-units** widens it freely (to `int64_t` or 128-bit), and for the vast majority of
practical scenarios comparisons are correct with plain integer types — `safe_int` adds
no extra benefit here. For extreme corner cases (overflowing even 128 bits), see the
`static_assert` examples in [Approach 3](#approach-3-mp-units-widened-intermediate-arithmetic).

!!! note "`safe_int` and `constexpr`"

    `safe_int<T>` arithmetic is fully `constexpr`. In C++, any overflow that occurs during
    constant expression evaluation is always a compile-time hard error — for both `safe_int`
    and plain integers. The difference emerges only at **runtime**, where `safe_int` catches
    overflows that plain integers silently ignore.

---

## Comparison

| Mechanism                               | Scope                                   | False positives | Opt-out?                | Detects auto-scaling overflow?    |
|-----------------------------------------|-----------------------------------------|-----------------|-------------------------|-----------------------------------|
| **mp-units** compile-time scaling check | rejects guaranteed-overflow conversions | no              | none needed             | N/A                               |
| Au overflow safety surface              | scaling conversions up to threshold     | yes (by design) | `ignore(OVERFLOW_RISK)` | factors only (not runtime values) |
| Au `is_conversion_lossy`                | explicit conversions (per-value)        | no              | manual wrapper          | no                                |
| `safe_int<T>` (**mp-units**)            | all arithmetic, all contexts            | no              | none needed             | yes                               |

The approaches are orthogonal and can be combined: use **mp-units**' compile-time
scaling check to catch statically-obvious conversions early, and use `safe_int<T>`
for a complete runtime guarantee on every arithmetic operation — including the
automatic conversions the library performs transparently.

!!! tip "Understanding the scaling algorithm"

    For a deep dive into how **mp-units** performs unit conversions and chooses
    common units, see [How Scaling Works](../../users_guide/framework_basics/representation_types.md#how-scaling-works)
    in the Representation Types guide. The library uses sophisticated algorithms
    including widened arithmetic for integer types to avoid UB during scaling
    operations, but `safe_int` is still necessary to catch overflow in the **result**
    of those operations.

---

## Integral Division: An Additional Hazard

One further integer-specific hazard deserves mention:
[**divide-before-convert**](understanding-safety-levels.md#level-3-representation-safety).

```cpp
// Apparent time ratio — how many 40-minute intervals in 8 hours?
hours(8) / minutes(40)   // Expected: 12. Actual with integer division:
                         // 8 / 40 = 0, then unit factor applied → wrong!
```

The Au library [forbids this category of division](https://aurora-opensource.github.io/au/main/troubleshooting/#integer-division-forbidden)
by default, requiring an explicit opt-in. **mp-units** currently follows plain C++
and allows it, consistent with `std::chrono::duration`. Whether to adopt a stricter
stance is an open design question.

---

## Summary

Compile-time overflow detection has hard limits: values are only known at runtime.
Every approach that operates purely at the type-and-unit level is a heuristic with edges.

But the deeper problem is **hidden automatic scaling**. When you write `meters + yards`
or `dist_m > dist_km`, the library must convert to a common unit — and that conversion
happens silently. You don't see it in the code. You didn't ask for it. But it's
multiplying or dividing your values by scaling factors that can overflow integer types.

Different libraries handle this differently:

- **`std::chrono::duration` and libraries using `std::common_type`**: Compute intermediate
  scaling in the same narrow type → undefined behavior on overflow
- **mp-units 2.5.0**: Respect integral promotion and widen to `intmax_t` (typically `int64_t`)
  for intermediate arithmetic → prevents most UB, but vulnerable when using `int64_t` or huge
  scaling factors
- **mp-units (current)**: Use widened intermediate arithmetic (`int64_t` for
  `int8_t`/`int16_t`/`int32_t`, 128-bit arithmetic for `int64_t`) for intermediate scaling
  → **greatly reduces UB risk**, handling typical real-world cases safely, but extreme
  scenarios can still overflow (e.g., AU ↔ nm, `pow<3>` quantities, or very large
  `int64_t` values with moderate scaling)
- **mp-units + `safe_int<T>`**: Detect **every** overflow — intermediate and final —
  with policy-based error handling

`safe_int<T>` completes the protection: it operates at the **arithmetic level**, catching
overflow in every operation — including the automatic scaling that units libraries perform
transparently to make cross-unit arithmetic possible. The policy template parameter gives
you control over what "handling" means, from a hard abort to a `std::expected`-based error
return.

The combination of **mp-units**' layered approach gives you choices based on your needs:

| Threat                                         | mp-units (plain types)                        | + `safe_int<T>`                               |
|------------------------------------------------|-----------------------------------------------|-----------------------------------------------|
| Statically-obvious conversion overflow         | Compile-time error ✓                          | Compile-time error ✓                          |
| UB during intermediate scaling                 | Greatly reduced (widened arithmetic) ⚠️       | Eliminated (widened + checks) ✓               |
| **Hidden automatic scaling overflow (result)** | **Silent wrap** ⚠️                            | **Detected and handled** ✓                    |
| Plain arithmetic overflow                      | Silent wrap ⚠️                                | Detected and handled ✓                        |
| Values outside physical domain                 | No protection                                 | + origin bounds policy ✓                      |
| Divide-before-convert truncation               | (open question; Au's approach is a reference) | (open question; Au's approach is a reference) |

**Key insight**: mp-units' widened intermediate arithmetic (`int64_t` for types up to
`int32_t`, 128-bit for `int64_t`) **greatly reduces** the risk of undefined behavior
during automatic scaling, handling typical real-world scenarios safely. However, extreme
cases (huge scaling factors like AU↔nm, or `pow<3>` quantities with large values) can
still overflow even 128-bit arithmetic. For **guaranteed** overflow detection in all
cases, use `safe_int<T>`.

For the full `safe_int<T>` reference, see the
[User's Guide](../../users_guide/framework_basics/safe_int.md). For usage patterns
combining `safe_int` with origin bounds policies, see
[Ensure Ultimate Safety](../../how_to_guides/advanced_usage/ultimate_safety.md).

!!! info "Comparing mp-units and Au Approaches"

    Au's compile-time overflow safety surface and `is_conversion_lossy` checker take a
    different approach — see [How Au Addresses the Problem?](#how-au-addresses-the-problem)
    for a detailed comparison and the
    [summary table](#summary-of-approaches) for a side-by-side overview.

    **Integral division**: [Au rejects integer Quantity/Quantity division](https://aurora-opensource.github.io/au/main/troubleshooting/#integer-division-forbidden)
    whenever the denominator's unit is not quantity-equivalent to the numerator's, protecting
    against divide-before-convert errors like `hours(8) / minutes(40)` returning 0. Escape
    hatches are `unblock_int_div()` and `divide_using_common_unit()`. **mp-units** currently
    permits all integer quantity division, consistent with plain C++ behavior — whether to
    adopt a stricter stance is an open design question.

    For a complete comparison across all safety levels, see
    [Understanding Safety Levels: Representation Safety](understanding-safety-levels.md#level-3-representation-safety).
