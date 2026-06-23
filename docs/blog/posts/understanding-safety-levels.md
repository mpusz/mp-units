---
date: 2026-03-23
authors:
 - mpusz
categories:
 - Metrology
comments: true
---

# Understanding Safety Levels in Physical Units Libraries

Physical quantities and units libraries exist primarily to prevent errors at compile time.
However, not all libraries provide the same level of safety. Some focus only on dimensional
analysis and unit conversions, while others go further to prevent representation errors,
semantic misuse of same-dimension quantities, and even errors in the mathematical structure
of equations.

This article explores six distinct safety levels that a comprehensive quantities and units
library can provide. We'll examine each level in detail with practical examples, then compare
how leading C++ libraries and units libraries from other languages perform across these
safety dimensions. Finally, we'll analyze the performance and memory costs associated
with different approaches, helping you understand the trade-offs between safety guarantees
and runtime efficiency.

We'll pay particular attention to the upper safety levels—especially **quantity kind
safety** (distinguishing dimensionally equivalent concepts such as work vs. torque, or
Hz vs. Bq) and **quantity safety** (enforcing correct quantity hierarchies and
scalar/vector/tensor mathematical rules)—which are well-established concepts in metrology
and physics, yet remain widely overlooked in the C++ ecosystem. Most units library authors
and users simply do not realize these guarantees are achievable, or how much they matter
in practice. These levels go well beyond dimensional analysis, preventing subtle semantic
errors that unit conversions alone cannot catch, and are essential for realizing truly
strongly-typed numerics in C++.

<!-- more -->

!!! tip "Quick Overview"

    For a quick introduction to safety features in **mp-units**, see the
    [Safety Features](../../getting_started/safety_features.md) page in Getting Started.
    This blog post provides in-depth analysis, library comparisons, and the rationale
    behind each safety level.


## The Six Safety Levels

A comprehensive physical quantities and units library can provide six distinct layers
of safety:

1. **Dimension Safety** - Prevents mixing incompatible dimensions
2. **Unit Safety** - Prevents unit mismatches and eliminates manual scaling factors
3. **Representation Safety** - Protects against overflows and precision loss
4. **Quantity Kind Safety** - Prevents arithmetic on quantities of different kinds
5. **Quantity Safety** - Enforces correct quantity relationships and equations
6. **Mathematical Space Safety** - Distinguishes points, absolute quantities, and deltas

The first five levels form a progression in dimensional analysis, type safety, and
numeric safety, building upon the previous levels to create increasingly sophisticated
protection against errors.

Mathematical Space Safety addresses an orthogonal concern: the mathematical distinction
between points and deltas (and, with V3, also
[absolute quantities](introducing-absolute-quantities.md) on a ratio scale),
providing complementary protection that works alongside the other safety levels.


## Level 1: Dimension Safety

**All major C++ units libraries provide dimension safety**—this is the foundational feature
that enables dimensional analysis.

### What It Prevents

Without dimension safety:

```cpp
auto speed = 100;              // 100 what? No dimension information
auto time = 2;                 // 2 what? No dimension information  
auto wrong = speed + time      // Wrong logic, but compiles!  
auto distance = speed / time;  // Wrong logic! Should be speed * time, but compiles
```

Dimension safety provides:

- **Dimensional analysis**: Operations check dimension compatibility
    - can't add `length + time`
- **Dimension verification**: Values have units of a correct dimensions
    - meters for _length_
    - seconds for _time_
- **Derived dimensions**: Arithmetic produces correct dimensions
    - `speed / time` does not result it `distance`

??? danger "Plain `double` daily reality"

    Any C++ codebase that uses plain `double` (or any other fundamental type) to express
    quantities has exactly the above issues — and this is still surprisingly common in
    production code today. Strong types that carry dimension information are the only
    reliable way to express independent physical abstractions at the type level.

### Example

```cpp
quantity speed = 100 * km / h;
quantity time = 2 * h;
// quantity<si::kilo<si::metre>> distance = 2 * h;          // ❌ Compile-time error!
// quantity<si::kilo<si::metre>> distance = speed / time;   // ❌ Compile-time error!
quantity<si::kilo<si::metre>> distance = speed * time;      // ✅ OK

// quantity result = distance + time;                       // ❌ Compile-time error!
// Error: cannot add length and time (incompatible dimensions)
```


## Level 2: Unit Safety

**Unit safety** ensures that quantities are constructed, assigned, and passed across
interface boundaries only with compatible units. It covers function arguments, return
types, and component integration.

### What It Prevents

Even when the dimension is correct, using the wrong unit silently produces wrong values:

```cpp
void set_speed_limit(double speed_km_h);  // Expects km/h, but what if you pass m/s?
set_speed_limit(25.0);                    // 25 what? km/h? m/s? mph? Compiles but dangerous!

double diameter_in = 12.5;   // inches
double diameter_mm = diameter_in * 25.4;      // Is this magic number really correct?
double area_cm2 = diameter_mm * diameter_mm;  // Bug: forgot to convert mm → cm first!
```

Common scenarios where unit safety matters:

- **Function arguments**: Passing meters when kilometers expected
- **Return types**: Misinterpreting returned values
- **Component integration**: Mixing units between modules
- **Value extraction**: Getting raw numbers without unit verification
- **Refactoring**: Changing function argument units without updating call sites
- **Manual conversions**: Error-prone scaling factors

??? danger "Historical Mishap: Mars Climate Orbiter (1999)"

    In 1999, NASA's **Mars Climate Orbiter** was lost due to unit confusion between its
    subsystems: one software component produced thruster impulse data in **pound-force-seconds**
    (lbf·s, imperial), while the navigation software expected **newton-seconds** (N·s, SI).
    The spacecraft entered the Martian atmosphere at the wrong angle and was destroyed.

    **Cost: $327 million.** Unit safety would have made this a compile-time error.

??? danger "Real-World Consequence: The 'Magic 1.0' Problem in HEP"

    Different HEP frameworks silently use different base units for the same quantity:

    | Quantity   | Geant4            | ROOT              |
    |------------|-------------------|-------------------|
    | **Length** | `1.0` = **1 mm**  | `1.0` = **1 cm**  |
    | **Time**   | `1.0` = **1 ns**  | `1.0` = **1 s**   |
    | **Angle**  | `1.0` = **1 rad** | `1.0` = **1 deg** |
    | **Energy** | `1.0` = **1 MeV** | `1.0` = **1 GeV** |

    Bridge code between frameworks requires manual scaling factors everywhere.
    One missing `/ CLHEP::cm` makes a detector geometry **10× smaller** — silently, at runtime:

    ```cpp
    double radius = rootVolume->GetRmax();  // Returns 10.0 — meaning 10 cm in ROOT
    G4Tubs g4Tube("Tube", 0, radius, ...);  // Geant4 reads it as 10 mm!
    ```

    With **mp-units**, ROOT and Geant4 quantities carry their unit in the type, so mixing them
    **automatically applies the correct conversion** — no manual scaling factors, no silent bugs.

### Example: The Real Cost of Missing Unit Safety

Unit errors arise at construction, assignment, and interface boundaries:

=== "Without unit safety: Silent wrong values"

    ```cpp
    // Interface: caller must manually track the expected unit
    double avg_speed(double distance_m, double time_s) { return distance_m / time_s; }

    double distance_m = 220'000.0;                      // 220 km in meters
    double distance_km = distance_m / 1000.0;           // converted to km (220.0)
    double time_h = 2.0;                                // 2 hours
    double speed_mps = avg_speed(distance_km, time_h);  // Bug! km and h passed, m and s expected
    // avg_speed(220.0, 2.0) = 110 — not ~30.56 m/s (silently wrong: km treated as m, h as s)
    double speed_kmph = speed_mps / 3.6;                // Bug! should be × 3.6 (m/s → km/h)
    ```

=== "With mp-units: Automatic & Safe"

    ```cpp
    // Interface: units are enforced at every call site
    quantity<m / s> avg_speed(quantity<m> d, quantity<s> t) { return d / t; }

    quantity<km> distance = 220'000.0 * m;              // ✅ automatic conversion (220 km)
    quantity time = 2.0 * h;                            // 2 hours
    quantity<km / h> speed = avg_speed(distance, time); // ✅ auto-converted (km → m, h → s, m/s → km/h)
    // avg_speed(220 km, 2 h) = 110 km/h — correct result, no manual scaling needed
    ```

### The Value Extraction Problem

A critical aspect of unit safety is how libraries handle extracting raw numeric values.
Consider `std::chrono::duration`:

```cpp
std::chrono::seconds sec(100);
auto count = sec.count();  // Returns 100 - but is it seconds? milliseconds?
// No unit verification! User must remember and hope it's correct and will not change in the future
```

??? danger "`std::chrono::duration_cast` is not the right solution"

    The `std::chrono` designers were aware of this problem — `std::chrono::duration_cast`
    was introduced in C++11 precisely to force an explicit unit acknowledgement before
    extracting a raw value.

    In practice, however, this discipline is rarely maintained consistently. Engineers
    skip the cast because they don't know about it, forget it when the current source and
    destination units happen to match, or assume it isn't needed "just this once." The
    result is that `count()` is called without a cast throughout production codebases.

    The root cause is the interface itself: `.count()` should never have been callable
    without specifying a unit. Relying on user discipline to compensate for an unsafe
    API does not scale.

For a complete guide on safely extracting values for legacy interfaces, see
[Working with Legacy Interfaces](../../how_to_guides/integration/working_with_legacy_interfaces.md).

**mp-units** takes a stricter approach:

```cpp
quantity distance = 1500. * m;
// auto value = distance.count();  // Error: no count() method!

auto value = distance.numerical_value_in(km);  // Must specify unit explicitly
// Forces users to think about and document the unit being used
```

!!! warning "Unit Safety Does Not Distinguish Same-Dimension Kinds"

    Unit safety catches mismatches between *different* dimensions, but it cannot distinguish
    between quantities that share the same dimension. Even a library rated **Full** at this
    level will silently accept:

    - `rad` and `sr` (both dimensionless)
    - `Hz` and `Bq` (both s⁻¹)
    - `Gy` and `Sv` (both J/kg = m²·s⁻²)

    Addition, subtraction, and comparison between these pairs all compile without error,
    despite being physically meaningless. This limitation is fundamental — unit safety
    operates on *dimensions*, not on *quantity kinds*. Addressing it requires [**Level 4:
    Quantity Kind Safety**](#level-4-quantity-kind-safety).


## Level 3: Representation Safety

**Representation safety** protects against numerical issues: overflow, underflow, and
precision loss during conversions and arithmetic.

### What It Prevents

Many libraries carry unit types but do not validate the representation type on conversion.
Truncation and overflow happen silently:

```cpp
namespace bu = boost::units;
using kilometer_unit = bu::make_scaled_unit<bu::si::length, bu::scale<10, bu::static_rational<3>>>::type;

bu::quantity<bu::si::length, int>         distance_m  = 32500 * bu::si::meter;
bu::quantity<kilometer_unit, std::int8_t> distance_km(distance_m);  // Unit conversion (m → km): Truncation: 32, not 32.5
bu::quantity<bu::si::length, std::int8_t> back_to_m(distance_km);   // Unit conversion (km → m): Overflow: wraps around
```

### Two Approaches to Representation Safety

#### Truncation Prevention

**mp-units** follows `std::chrono::duration`'s approach for truncation:

```cpp
quantity length = 1500 * m;
quantity<km, int> distance = length;         // ❌ Compile-time error!
// Error: 1500 m → 1.5 km truncates with int

// Must be explicit about potentially lossy conversions
quantity distance2 = length.force_in(km);                  // ✅ OK: explicit truncation
auto km_count = length.force_numerical_value_in(km);       // ✅ OK: returns raw int (1)
quantity distance3 = length.in<double>(km);                // ✅ OK: floating point avoids loss

// Value-preserving conversions work implicitly
quantity<mm, int> length_mm = length;        // ✅ OK: 1'500'000 mm (no truncation)
```

#### Scaling Overflow Detection

**mp-units** detects when unit scaling exceeds representation limits:

```cpp
quantity length1 = std::int8_t{2} * m;
quantity<mm, std::int8_t> length_mm1 = length1;   // ❌ Compile-time error!
// Error: 2 m → 2000 mm overflows int8_t (max 127)

quantity length2 = std::int16_t{2} * m;
quantity<mm, std::int16_t> length_mm2 = length2;  // ✅ OK: 2000 fits in int16_t
```

!!! warning "Compile-Time Limitations for Runtime Overflow Detection"

    The values involved in arithmetic operations are only known at runtime,
    so the compiler cannot predict whether overflow will occur:

    ```cpp
    quantity<m, std::int8_t> small = 100 * m;
    quantity overflow = small * 2;  // ⚠️ Runtime overflow! No compile-time detection possible
    ```
    **No general-purpose units library can prevent all cases of runtime arithmetic overflow at
    compile time.** Compile-time programming can only detect when conversions or scaling
    factors themselves are guaranteed to overflow the representation type (as shown in the
    integer examples above). But for general arithmetic, the actual values are only known at
    runtime, so no general-purpose units library can prevent all cases of runtime arithmetic
    overflow at compile time—this is fundamentally impossible in C++.

    **Solution: Built-in Runtime Safety Infrastructure**

    **mp-units** ships built-in tools for runtime overflow and bounds protection:

    - **`safe_int<T>`** — a drop-in integer wrapper that detects arithmetic overflow at runtime
      (`#include <mp-units/safe_int.h>`)
    - **`constrained<T, ErrorPolicy>`** — a transparent wrapper that tags a representation type
      with an error policy, enabling guaranteed bounds enforcement via `constraint_violation_handler`
      (`#include <mp-units/constrained.h>`)
    - **Bounds policies on point origins** — a template parameter on the origin that attaches a validation policy
      (e.g. `check_in_range`, `clamp_to_range`, `wrap_to_range`, `reflect_in_range`) to a
      quantity point origin

    ```cpp
    #include <mp-units/safe_int.h>
    #include <mp-units/constrained.h>

    // Built-in safe_int detects arithmetic overflow at runtime
    quantity distance = safe_int{std::int16_t{100}} * m;
    // quantity overflow = distance * std::int16_t{1'000};  // throws std::overflow_error at runtime

    // Cyclic longitude points that wrap at the anti-meridian
    inline constexpr struct prime_meridian :
      absolute_point_origin<geo_longitude, wrap_to_range{-180 * deg, 180 * deg}> {} prime_meridian;

    using safe_double = mp_units::constrained<double, mp_units::throw_policy>;
    using longitude = quantity_point<geo_longitude[deg], prime_meridian, safe_double>;
    longitude lon{190.0 * deg, prime_meridian};  // wraps to -170°
    ```

    For the full walkthrough, see
    [Ensure Ultimate Safety](../../how_to_guides/advanced_usage/ultimate_safety.md).

!!! question "Should Libraries Check Floating-Point Unit Scaling Overflow?"

    A more subtle issue: **no library currently checks whether unit scaling factors would cause
    floating-point overflow or underflow during conversions**:

    ```cpp
    quantity<m, double> huge = 1.5e308 * m;       // Near double's max (~1.8e308)
    quantity<mm, double> overflow = huge;         // ⚠️ 1.5e308 × 1000 → inf (overflow!)

    quantity<km, double> tiny = 1e-320 * km;      // Near double's min
    quantity<m, double> underflow = tiny;         // ⚠️ 1e-320 × 1000 → 0 (underflow!)
    ```

    **Should libraries detect and prevent this?** The question remains open:

    - Most real-world values are nowhere near floating-point limits
    - Checking would add runtime overhead or complex compile-time analysis
    - IEEE 754 provides `inf` and gradual underflow, which may be acceptable behavior
    - No consensus exists in the community on whether this is worth addressing

    This is an area where library designers must balance safety, performance, and practical
    utility. Feedback and use cases from the community would help inform future decisions.

!!! note "Comparison: Au's Approach to Representation Safety"

    Au differs from mp-units in two compile-time aspects:

    **Magic number heuristic**

    The [Au library](https://aurora-opensource.github.io/au/main/discussion/concepts/overflow/#adapt)
    uses a stricter threshold than mp-units: it blocks conversions where the scaling factor
    would overflow their chosen "magic number" value of 2'147, catching cases where the
    scaling factor is within the type's range but a typical value might still overflow. The trade-off: the heuristic produces false positives (blocking valid
    conversions), which is why Au must also provide an
    [opt-out mechanism](https://aurora-opensource.github.io/au/main/discussion/concepts/conversion_risks/#opting-out-of-safety-checks).
    Once the opt-out is used, the safety guarantee is gone — the same silence returns.
    Critically, neither Au's heuristic nor its runtime checkers cover overflow in hidden
    common-unit arithmetic (e.g. `m + yd` with `int32_t`) — the most surprising case for users.

    **mp-units** uses the simpler threshold model — the only value that survives a definitely-overflowing
    conversion is zero, so blocking it is unambiguous and requires no opt-out. For runtime
    coverage of *all* arithmetic including common-unit operations, use `safe_int<T>` as the
    representation type.

    **Integral division disallowed**

    [Au rejects integer `Quantity / Quantity` division](https://aurora-opensource.github.io/au/main/troubleshooting/#integer-division-forbidden)
    whenever the denominator's unit is not quantity-equivalent to the numerator's —
    covering both cross-dimension cases (`meters(60) / (miles/hour)(65)`) and
    same-dimension cases with different magnitudes (`hours(8) / minutes(40)`). The hazard
    is not ordinary truncation but *divide-before-convert*: `hours(8) / minutes(40)`
    computes `8 / 40 = 0` in integer arithmetic and then applies the unit division,
    giving `(hours/minute)(0)` instead of the expected `12`. The escape hatches are
    `unblock_int_div(denominator)` (opt-in, use with caution) and
    `divide_using_common_unit(a, b)` (converts both operands to their common unit first,
    which is safe). Division by a raw integer or by a quantity-equivalent unit is always
    allowed.

    **mp-units** currently permits all integer quantity division, consistent with plain
    C++ behaviour. Whether to adopt Au's stricter stance is an open question that may be
    evisited based on ISO C++ Committee guidance or production experience.

### Unit-Qualified Construction

Beyond conversions, representation safety also governs construction itself.
`std::chrono::duration` provides an `explicit` constructor from a raw integer, which
prevents implicit conversions but does **not** protect against in-place construction via
`emplace_back`:

```cpp
std::vector<std::chrono::milliseconds> delays;
delays.emplace_back(42);  // ✅ Compiles — stores 42ms

// Refactor: switch to microseconds for higher precision
std::vector<std::chrono::microseconds> delays;
delays.emplace_back(42);  // ✅ Still compiles — but now stores 42µs!
                          //    Silent 1000× regression, zero diagnostic
```

`explicit` makes the intent clear at declaration sites, but direct in-place construction
bypasses it entirely. Changing the element type causes a silent, factor-of-1000 regression
with no clue from the compiler.

**mp-units** takes a stricter stance: quantity construction *always* requires both a number
and a unit. A raw integer never constructs a quantity — not through direct initialization,
assignment, or `emplace_back`:

```cpp
std::vector<quantity<si::milli<si::second>>> delays;
// delays.emplace_back(42);    // ❌ Compile-time error — unit required!
delays.emplace_back(42 * ms);  // ✅ Unit is explicit — intent is unambiguous
delays.emplace_back(42, ms);   // ✅ OK

// Refactor: switch to microseconds
std::vector<quantity<si::micro<si::second>>> delays;
// delays.emplace_back(42);    // ❌ Compile-time error — still requires a unit!
delays.emplace_back(42 * ms);  // ✅ 42ms → 42000µs: explicit, value-preserving conversion
delays.emplace_back(42, ms);   // ✅ OK
delays.emplace_back(42 * us);  // ✅ Or 42µs if that was the actual intent
delays.emplace_back(42, us);   // ✅ OK
```

There is no construction path that silently discards unit information.

### Non-Negative Quantity Tracking

Beyond numeric representation, **mp-units** also encodes domain constraints at the quantity
specification level. Many physical quantities are inherently non-negative: _length_, _mass_,
_duration_, _thermodynamic temperature_, _amount of substance_, and _luminous intensity_ can
never be negative. The ISQ base quantity definitions in **mp-units** carry this fact as a
compile-time property:

```cpp
static_assert(is_non_negative(isq::length));
static_assert(is_non_negative(isq::mass));
static_assert(is_non_negative(isq::duration));
static_assert(!is_non_negative(isq::electric_current));  // can be negative
```

This property is **explicitly declared** at each non-negative root derived quantity in the
ISQ hierarchy, and is automatically inherited by named real-scalar children:

```cpp
static_assert(is_non_negative(isq::speed));      // explicit non_negative tag in definition
static_assert(is_non_negative(isq::area));       // explicit non_negative tag in definition
static_assert(is_non_negative(isq::distance));   // named real-scalar child of length
static_assert(!is_non_negative(isq::velocity));  // vector character — excluded from inheritance
```

The tag is **never inferred from equation factors** — even when all factors are
non-negative, the defining equation captures only dimensional relationships, not the
full sign domain of the physical quantity. For example, _reactive power_ is defined via
$Q = U \cdot I \cdot \sin\varphi$, and the _Massieu function_ as $J = -A/T$ — both have
all-non-negative dimensional factors yet can take negative values in practice:

```cpp
// All dimensional factors are non-negative, yet the quantities are NOT non_negative:
static_assert(!is_non_negative(isq::reactive_power));   // signed: Q = U·I·sin(φ)
static_assert(!is_non_negative(isq::Massieu_function)); // signed: J = −A/T
```

!!! note

    `kind_of<QS>` is **never** non-negative, even when `QS` itself is tagged `non_negative`,
    because `kind_of<QS>` represents the entire quantity tree including vector quantities
    and signed coordinates. This matters when using CTAD with bare SI units:

    ```cpp
    quantity_point generic{5.0 * m};                  // origin uses kind_of<isq::length> — NOT auto-bounded
    quantity_point dist{distance_traveled(5.0 * m)};  // uses isq::distance — auto-bounded
    ```

This metadata is automatically enforced at runtime for `quantity_point` types: when a
`quantity_point` uses a natural origin and the associated quantity spec is non-negative,
the library automatically attaches a `check_non_negative` policy through conditional
inheritance — no explicit bounds definition is needed. You can still override the default
by defining a custom origin with different bounds:

```cpp
// Define a custom origin with clamp_non_negative instead of the automatic check_non_negative:
inline constexpr struct clamped_length_origin :
    absolute_point_origin<isq::length, clamp_non_negative{}> {} clamped_length_origin;
```

The metadata is also available for tooling, documentation, and static analysis — for
example, to automatically select signed vs. unsigned representations.

### Bounded Quantity Points

Representation safety also extends to **quantity points** (Level 6). Bounds policies
passed as template parameters to point origins let you attach validation to
any quantity point origin, so geographic coordinates, sensor ranges, and similar
constrained domains are validated at construction and during arithmetic:

```cpp
inline constexpr struct geo_longitude : quantity_spec<isq::angular_measure> {} geo_longitude;
inline constexpr struct prime_meridian :
  absolute_point_origin<geo_longitude, wrap_to_range{-180 * deg, 180 * deg}> {} prime_meridian;

using longitude = quantity_point<geo_longitude[deg], prime_meridian>;
longitude lon{190.0 * deg};  // wraps to -170°
```

The library ships six overflow policies (`check_in_range`, `clamp_to_range`,
`wrap_to_range`, `reflect_in_range`, `check_non_negative`, `clamp_non_negative`),
and the interface is extensible — you can write your own policy (e.g. a one-sided policy
for custom bounds that are neither zero-based nor symmetric)
as long as it provides `V operator()(V)`. Combined with the `constrained<T, ErrorPolicy>`
wrapper described above, `check_in_range` provides **guaranteed enforcement** in every
build mode — not just debug builds.

`check_non_negative` and `clamp_non_negative` are specifically designed for halflines
`[0, +∞)`: the former reports a violation when the value is negative, while the latter
silently clamps negative values to zero.

For the complete walkthrough, see
[Range-Validated Quantity Points](../../users_guide/framework_basics/the_affine_space.md#range-validated-quantity-points)
(including [Custom Policies](../../users_guide/framework_basics/the_affine_space.md#custom-policies-one-sided-bounds))
and [Ensure Ultimate Safety](../../how_to_guides/advanced_usage/ultimate_safety.md).


## Level 4: Quantity Kind Safety

**Quantity kind safety** distinguishes between quantities that share the same dimension but
represent different physical concepts—different "kinds" of quantities.

### What It Prevents

Many physical quantities share dimensions but are conceptually distinct:

- **Absorbed dose (Gy)** and **Dose equivalent (Sv)**: Both `length²/time²` (dimension L²T⁻²)
- **Frequency (Hz)** and **Activity (Bq)**: Both `1/time` (dimension T⁻¹)
- **Plane angle (rad)** and **Solid angle (sr)**: Both dimensionless ratios
- **Area (m²)** and **Fuel consumption (L/100km)**: Both `length²` (dimension L²)
- **Fluid head (m)** and **Water head (m)**: Both `length` (dimension L)
- Various counts and ratios: All `dimensionless` (dimension 1)

### Why This Is Challenging

Quantity kind safety requires going beyond the seven base dimensions and recognizing that
quantities sharing the same dimension can represent conceptually distinct physical concepts.
This applies both to quantities with special unit names (Gy vs. Sv, Hz vs. Bq) and to
those without (area vs. fuel consumption, various dimensionless counts and ratios).
**mp-units is the only C++ library implementing this level of safety fully.**

### Quantity Kind Correctness

The following examples illustrate how easily same-dimension quantities can be confused
in practice—and why keeping them as distinct types is essential for correctness.

#### Distinguishing among doses

**_Absorbed dose_** (Gy = J/kg) measures the raw _energy_ deposited in tissue by ionizing
radiation. **_Dose equivalent_** (Sv = J/kg) weights that _energy_ by a biological effectiveness
factor that depends on the type of radiation—a neutron causes far more cellular damage than
an X-ray delivering the same _absorbed dose_. Numerically, 1 Gy of neutrons may correspond
to 20 Sv of _dose equivalent_. Treating them as interchangeable is not just imprecise—in
radiation protection, it can directly lead to under- or over-estimating health risk, with
life-safety consequences. Yet both quantities share the same dimension (L²T⁻²), so without
quantity kind safety, any library will silently allow assigning one to the other.

=== "Without quantity kind safety: Kinds can be freely mixed up"

    ```cpp
    namespace bu = boost::units;

    bu::quantity<bu::si::absorbed_dose> absorbed_dose = 2.5 * bu::si::gray;
    bu::quantity<bu::si::dose_equivalent> dose_equivalent = absorbed_dose;  // Oops! Compiles!
    auto equal = (absorbed_dose == 2.5 * bu::si::sievert);                  // Oops! Compiles!
    // Danger: Sv (dose equivalent) accounts for biological effectiveness; Gy (absorbed dose) doesn't!
    ```

=== "With mp-units: Distinct kinds"

    ```cpp
    quantity absorbed_dose = 1.5 * Gy;
    quantity dose_equivalent = 2.0 * Sv;

    // auto result = absorbed_dose + dose_equivalent;           // ❌ Compile-time error!
    // Error: cannot add absorbed dose and dose equivalent

    // auto equal = (absorbed_dose == dose_equivalent);         // ❌ Compile-time error!
    // Error: cannot compare absorbed dose and dose equivalent (no interop between different quantity kinds)
    ```

!!! question "How Serious Is This Problem?"

    The risk of confusing Gy and Sv is considered so serious that some libraries have chosen
    to omit one of these units entirely rather than risk users mixing them up. For example,
    the Au library [decided not to provide the Sievert unit](https://github.com/aurora-opensource/au/pull/157)
    specifically because it shares the same dimension as Gray but represents a fundamentally
    different concept. This design choice—sacrificing completeness for safety—highlights why
    quantity kind safety is essential for libraries working in safety-critical domains like
    medical physics and radiation protection.

#### Distinguishing hydraulic heads

In hydraulic engineering, [hydraulic head](https://en.wikipedia.org/wiki/Hydraulic_head)
can be expressed in two distinct ways that both have the dimension of _length_ but must not
be mixed: **_fluid head_** (expressed in terms of the actual fluid) and **_water head_**
(normalized against specific gravity, expressed in equivalent meters of water). Mixing
them silently produces numerically plausible but physically meaningless results—for
example, 2 m of mercury _fluid head_ is equivalent to ~27.2 m of _water head_, not 2 m.

This is a [real-world use case reported by mp-units users](https://github.com/mpusz/mp-units/issues/718).
With quantity kind safety, both can be modelled as sub-kinds of `isq::height`.
To convert between them, a function accepting the _specific gravity_ (SG) must be called
explicitly — the type system enforces that the conversion is intentional.

=== "Without quantity kind safety: Kinds can be freely mixed up"

    ```cpp
    namespace bu = boost::units;

    // Both are just 'length' in Boost.Units — it cannot distinguish fluid from water head:
    bu::quantity<bu::si::length> fluid_head = 2.0 * bu::si::meters;
    bu::quantity<bu::si::length> water_head = 4.0 * bu::si::meters;

    auto wrong_sum = fluid_head + water_head;  // Oops! Compiles! 6 m, but physically meaningless
    if (fluid_head < water_head) { /* ... */ } // Oops! Compares magnitudes, not water equivalents
                                               // (2 m of mercury ≈ 27.2 m of water!)

    // No way to enforce that conversion via specific gravity must happen first:
    use_water_head(fluid_head);                // Oops! Passes fluid head where water head expected
    ```

=== "With mp-units: Distinct kinds"

    ```cpp
    inline constexpr struct fluid_head : quantity_spec<isq::height, is_kind> {} fluid_head;
    inline constexpr struct water_head : quantity_spec<isq::height, is_kind> {} water_head;
    inline constexpr struct specific_gravity : quantity_spec<dimensionless> {} specific_gravity;

    constexpr QuantityOf<water_head> auto to_water_head(QuantityOf<fluid_head> auto h_fluid,
                                                        QuantityOf<specific_gravity> auto sg)
    {
      // We explicitly cast the result to water_head because we know the physics is correct
      return water_head(isq::height(h_fluid) * sg);
    }

    quantity fluid = fluid_head(2 * m);
    quantity water = water_head(4 * m);
    use_water_head(water);           // ✅ OK

    // use_fluid_head(water);        // ❌ Compile-time error!
    // use_water_head(fluid);        // ❌ Compile-time error!
    // quantity q = fluid + water;   // ❌ Compile-time error!
    // if (fluid == water) {}        // ❌ Compile-time error!

    use_height(isq::height(fluid));  // ✅ OK: explicit upcast to generic length
    use_height(isq::height(water));  // ✅ OK: explicit upcast to generic length

    // Convert mercury fluid head to equivalent water head
    quantity sg_mercury = specific_gravity(13.6);
    quantity mercury_as_water = to_water_head(fluid, sg_mercury);
    ```

#### Distinguishing between counts

Quantity kind safety is also essential for **strongly-typed numeric types**. Using dimensionless
quantity kinds, you can easily create distinct types for counts, identifiers, ratios, and
other numeric values that shouldn't be mixed:

=== "Without quantity kind safety: Kinds can be freely mixed up"

    ```cpp
    namespace bu = boost::units;

    // Both are just 'dimensionless' in Boost.Units — it cannot distinguish different counts:
    bu::quantity<bu::si::dimensionless> items = 10.0 * bu::si::si_dimensionless;
    bu::quantity<bu::si::dimensionless> widgets = 20.0 * bu::si::si_dimensionless;

    auto mixed = items + widgets;                     // Oops! Compiles! But mixes different kinds
    bu::quantity<bu::si::dimensionless> bad = items;  // Oops! Silently assigns item count to widget count variable
    ```

=== "With mp-units: Distinct kinds"

    ```cpp
    inline constexpr struct item_count : quantity_spec<dimensionless, is_kind> {} item_count;
    inline constexpr struct widget_count : quantity_spec<dimensionless, is_kind> {} widget_count;

    quantity items = item_count(10);
    quantity widgets = widget_count(20);

    // auto mixed = items + widgets;  // ❌ Compile-time error!
    // Error: cannot mix different quantity kinds (even though both dimensionless)

    // quantity<widget_count[one]> bad = item_count(10);  // ❌ Compile-time error!
    // Error: cannot initialize with a different kind (even though both dimensionless)
    ```

For detailed examples, see
[Using dimensionless quantities as strongly-typed numeric types](../../users_guide/framework_basics/dimensionless_quantities.md#using-dimensionless-quantities-as-strongly-typed-numeric-types).


## Level 5: Quantity Safety

**Quantity safety** is the highest and most sophisticated level, ensuring that the _right
quantity_ in the _right form_ is used in each equation. This encompasses two complementary
aspects:

1. **Quantity Type Correctness** - Hierarchies, conversions, and quantity equation ingredients
  validation
2. **Quantity Character Correctness** - Representation types and character-specific operations

Both depend fundamentally on quantity hierarchies.

### Why Quantity Safety Matters: Insights from Library Authors

In a recent discussion with authors of other powerful C++ units libraries, the question arose:
what would we lose without quantity hierarchies?

Without hierarchies, we lose the ability to:

- **Distinguish specialized lengths**: _height_, _width_, _depth_, _radius_, _wavelength_
- **Differentiate energy types**: _kinetic_, _potential_, _thermal_, _enthalpy_
- **Separate angular quantities**: _plane angle_ vs. _solid angle_ (critical distinction!)
- **Validate correct ingredients**: requiring _height_ (not generic _length_) for _gravitational
  potential energy_
- **Discriminate dimensionless quantities**: _storage capacity_, _fuel consumption_,
  _efficiency ratios_, _counts_
- **Track quantity characters**: scalar, vector, tensor
- **Enforce correct equations**: `work` from `scalar_product(force, displacement)`, not from
  `vector_product`
- **Distinguish power types and their units**: _active power_ (W), _reactive power_ (var),
  _complex power_ and _apparent power_ (VA)
- **Use quantities as strongly-typed numeric types**: for domain-specific counts and values

!!! important

    **Quantity safety is essential for making mp-units a proposal for the strongly-typed numeric
    types that C++ has long needed.**

### Quantity Type Correctness

Quantity hierarchies allow distinguishing specialized quantities within the same dimension.

#### Energy Hierarchy Example

=== "Without quantity safety: All energies look the same"

    ```cpp
    namespace bu = boost::units;

    // No way to require a specific kind of energy in a function signature:
    void process_potential_energy(bu::quantity<bu::si::energy> e) { /* ... */ }

    // Boost.Units has a single 'energy' type — potential and kinetic are indistinguishable:
    bu::quantity<bu::si::energy> Ep = 100.0 * bu::si::joules;  // potential energy
    bu::quantity<bu::si::energy> Ek = 50.0 * bu::si::joules;   // kinetic energy

    bu::quantity<bu::si::energy> total = Ep + Ek;  // 150 J — but what kind of energy?

    // Silently assigns kinetic energy to a potential energy variable:
    Ep = Ek;  // Oops! Compiles without complaint

    process_potential_energy(Ek);  // Oops! Passes kinetic energy — compiles!
    ```

=== "With mp-units: Distinct quantity types"

    ```cpp
    // Functions requiring specific quantity types won't accept siblings
    void process_potential_energy(quantity<isq::potential_energy[J]> pe) { /* ... */ }

    // Potential and kinetic energy are siblings in the hierarchy under mechanical_energy
    quantity Ep = isq::potential_energy(100 * J);
    quantity Ek = isq::kinetic_energy(50 * J);

    // Can add siblings - result is their first common parent (mechanical_energy)
    quantity<isq::mechanical_energy[J]> total = Ep + Ek;  // ✅ OK: 150 J

    // Implicit conversion from child to parent
    quantity<isq::mechanical_energy[J]> mech1 = Ep;  // ✅ OK: upcast
    quantity<isq::energy[J]> e1 = Ep;                // ✅ OK: upcast to grandparent

    // Explicit conversion needed from parent to child
    // quantity<isq::potential_energy[J]> wrong = total;  // ❌ Error: downcast not allowed
    quantity<isq::potential_energy[J]> pe = isq::potential_energy(total);  // ✅ OK: explicit

    // Siblings cannot be directly converted
    // quantity<isq::kinetic_energy[J]> bad = Ep;  // ❌ Error: cannot convert between siblings
    quantity<isq::kinetic_energy[J]> ke = quantity_cast<isq::kinetic_energy>(Ep);  // ✅ OK: forced cast

    process_potential_energy(Ep);     // ✅ OK: exact match
    // process_potential_energy(Ek);  // ❌ Error: kinetic_energy is not potential_energy
    process_potential_energy(isq::potential_energy(total));  // ✅ OK: explicit conversion
    ```

#### Ingredient Validation

Some quantities require specific inputs, not just dimensionally correct ones:

=== "Without quantity safety: Any length is accepted"

    ```cpp
    namespace bu = boost::units;

    // Boost.Units has no concept of 'height' vs generic 'length':
    bu::quantity<bu::si::mass> mass = 10.0 * bu::si::kilograms;
    bu::quantity<bu::si::acceleration> g0 = 9.81 * bu::si::meters_per_second_squared;

    bu::quantity<bu::si::length> height = 5.0 * bu::si::meters;  // height
    bu::quantity<bu::si::length> width  = 2.0 * bu::si::meters;  // generic length

    // Both compute fine — no way to enforce that 'height' (not 'width') is required:
    auto Ep1 = mass * g0 * height;  // OK
    auto Ep2 = mass * g0 * width;   // Oops! Compiles! Physics is wrong but types are satisfied
    ```

=== "With mp-units: Specific ingredient required"

    ```cpp
    inline constexpr struct gravitational_potential_energy :
        quantity_spec<isq::potential_energy,
                      isq::mass * isq::acceleration_of_free_fall * isq::height> {} gravitational_potential_energy;

    constexpr quantity g0 = isq::acceleration_of_free_fall(1 * si::standard_gravity);
    quantity<isq::mass[kg]> mass = 10 * kg;
    quantity<isq::height[m]> height = 5 * m;

    // Gravitational potential energy requires height specifically
    quantity<gravitational_potential_energy[J]> Ep = mass * g0 * height;  // ✅ OK

    // Cannot substitute generic length for height
    quantity<isq::length[m]> width = 2 * m;
    // quantity<gravitational_potential_energy[J]> wrong = mass * g0 * width;  // ❌ Error: generic length
    ```

### Quantity Character Correctness

Quantities have intrinsic character based on their physical nature:

- **Real scalar quantities**: Real magnitude only (_speed_, _mass_, _temperature_, _work_)
- **Complex scalar quantities**: Complex magnitude with real and imaginary parts
  (_complex power_, _impedance_)
- **Vector quantities**: Magnitude and direction (_velocity_, _force_, _displacement_, _momentum_)
- **Tensor quantities**: Multi-dimensional arrays (_stress tensor_, _moment of inertia_)

Key distinctions:

- **Velocity vs. Speed**: _Velocity_ is vector; _speed_ is its scalar magnitude
- **Work vs. Moment of force**: _Work_ is real scalar from `scalar_product(force, displacement)`;
  _moment of force_ is vector from `vector_product(position_vector, force)`
- **Complex power**: Complex scalar with real part (_active power_), imaginary part
  (_reactive power_), and modulus (_apparent power_)

#### Representation Type Validation

A library with quantity character awareness can enforce that the representation type
matches the expected character of a quantity — preventing a scalar from being used where
a vector is required, or a real type where a complex one is required, and vice versa.

##### Scalar vs. Vector

=== "Without quantity character safety: Any representation is accepted"

    ```cpp
    namespace bu = boost::units;
    using vec3 = cartesian_vector<double>;

    // Boost.Units has no quantity character concept — any Rep is accepted for any quantity:
    auto m = bu::quantity<bu::si::mass, vec3>::from_value(vec3{10., 0., 0.});  // Oops! Vector mass — compiles!
    auto t = bu::quantity<bu::si::time, vec3>::from_value(vec3{5., 0., 0.});   // Oops! Vector time — compiles!

    // 'speed' and 'velocity' are the same unit (length/time), both accept scalar or vector Rep:
    auto speed    = bu::quantity<bu::si::velocity, vec3>::from_value(vec3{0., 0., -60.});  // Oops! Vector speed — compiles!
    auto velocity = bu::quantity<bu::si::velocity, double>::from_value(60.0);              // OK
    // No way to enforce that one should be speed (scalar) and the other velocity (vector)
    ```

=== "With mp-units: Representation type must match quantity character"

    ```cpp
    using vec3 = cartesian_vector<double>;

    // quantity<isq::mass[kg], vec3> m = /* ... */;           // ❌ Compile-time error!
    // quantity<isq::duration[s], vec3> t = /* ... */;        // ❌ Compile-time error!

    quantity q1 = isq::speed(60 * km / h);                    // ✅ OK: scalar speed
    // quantity q2 = isq::speed(vec3{0, 0, -60} * km / h);    // ❌ Compile-time error!
    // Error: speed is a scalar quantity; vector representation not allowed

    quantity q3 = isq::velocity(vec3{0, 0, -60} * km / h);    // ✅ OK: vector velocity
    quantity q4 = isq::velocity(60 * km / h);                 // ✅ OK: scalar acting as 1D vector
    ```

!!! info "Scalars as one-dimensional vectors"

    It is a common engineering practice to use fundamental types for vector quantities when we know
    the direction of the vector. For example, in many cases, we use `double` to express _velocity_
    or _acceleration_. In such cases, negative values mean moving backward or decelerating. This
    is why we also decided to allow such use cases in the library. A scalar representation type
    that provides `abs()` member or non-member function or works with `std::abs()` is considered
    a one-dimensional vector type.

!!! note "Scalar base quantities do not preclude vector children"

    ISQ defines base quantities such as _mass_ and _duration_ as scalars, and the library
    enforces this for `isq::mass` and `isq::duration` themselves. However, it does **not**
    prevent users from adding specialized child quantities with a different character further
    down their own hierarchy.

    The simple quantity syntax reflects this: `vec3{1, 2, 3} * kg` is accepted because `* kg`
    creates a quantity of the _entire kind tree_ rooted at _mass_, not specifically
    `isq::mass`. Whether any physics actually justifies a vector or complex _mass_ is a
    separate question — but libraries exist to serve their users, and we never know what
    ingenious (or just unusual) domain model someone might need to express. The framework
    deliberately stays out of the way when the quantity type itself carries no ISQ-mandated
    character constraint.

##### Real vs. Complex

AC circuit power involves both real-valued scalar quantities (_active power_,
_reactive power_, _apparent power_) and a complex-valued one (_complex power_).
A library with quantity character awareness requires `std::complex` for complex
quantities and rejects it for real ones — preventing silent representation
mismatches even before any equations are evaluated.

=== "Without quantity character safety: Any representation is accepted"

    ```cpp
    namespace bu = boost::units;

    // Boost.Units has no concept of complex quantity character
    bu::quantity<bu::si::power> P = 800.0 * bu::si::watts;   // Oops! Incorrect unit for active power
    bu::quantity<bu::si::power> Q = 600.0 * bu::si::watts;   // Oops! Incorrect unit for reactive power
    bu::quantity<bu::si::power> S = 1000.0 * bu::si::watts;  // Oops! Incorrect unit for apparent power
    auto complex1 = P + Q;                                   // Oops! Nonsense!

    // No knowledge about which units should match which power quantity
    auto complex2 = std::complex{Q.value(), P.value()} * bu::si::watts;    // Oops! Incorrect unit and reversed arguments

    // Nothing enforces which rep is appropriate — complex rep accepted for any power quantity:
    bu::quantity<bu::si::power> complex3 = /* ... */;                      // Oops! `double` representation for complex power
    bu::quantity<bu::si::power, std::complex<double>> active = /* ... */;  // Oops! `std::complex` representation for active power
    ```

=== "With mp-units: Representation type must match quantity character"

    ```cpp
    constexpr auto VA = V * A;

    quantity P = isq::active_power(800. * W);                     // ✅ OK: real scalar and correct unit
    quantity Q = isq::reactive_power(600. * var);                 // ✅ OK: real scalar and correct unit
    quantity S = isq::apparent_power(1000. * VA);                 // ✅ OK: real scalar and correct unit

    // Complex power — must use complex representation
    quantity complex = isq::complex_power((800. +  600.i) * VA);  // ✅ OK: complex scalar and correct unit

    // Cannot use complex representation for real power quantities:
    // quantity<isq::active_power[W], std::complex<double>> wrong1 = /* ... */;     // ❌ Compile-time error!
    // quantity<isq::reactive_power[var], std::complex<double>> wrong2 = /* ... */; // ❌ Compile-time error!
    // quantity<isq::apparent_power[VA], std::complex<double>> wrong3 = /* ... */;  // ❌ Compile-time error!

    // Cannot use real representation for complex power:
    // quantity<isq::complex_power[VA]> wrong4 = /* ... */;                         // ❌ Compile-time error!
    ```


#### Character-Specific Operations (V3 Planned)

In **mp-units** V3, operations will be restricted to appropriate quantity characters:

#### _speed_ vs _velocity_

_Velocity_ is a vector quantity — it carries both magnitude and direction. _Speed_ is its
scalar magnitude. They share the same dimension (LT⁻¹) and the same unit (m/s), yet are
fundamentally different: assigning a _velocity_ to a _speed_ silently discards direction,
and calling `magnitude()` on an already-scalar _speed_ is physically meaningless. Without
character enforcement, both mistakes compile unnoticed.

```cpp
// Velocity is a vector quantity
quantity velocity = isq::velocity(vec3{3, 4, 0} * m / s);

// Speed is the scalar magnitude
quantity<isq::speed[m/s]> speed = magnitude(velocity);             // ✅ 5 m/s

// Cannot accidentally mix them
// quantity<isq::speed[m/s]> s = velocity;                         // ❌ Compile-time error!
// quantity<isq::velocity[m/s]> v1 = speed;                        // ❌ Compile-time error!
// quantity<isq::velocity[m/s]> v2 = magnitude(velocity);          // ❌ Compile-time error!

// Cannot call magnitude() twice — speed is already a scalar, not a vector:
// quantity<isq::speed[m/s]> s2 = magnitude(magnitude(velocity));  // ❌ Compile-time error!
// quantity<isq::speed[m/s]> s3 = magnitude(speed);                // ❌ Compile-time error!
```

#### No vector multiplication

ISQ defines three related but distinct N·m quantities relevant here:

| Quantity          | Kind                                                     | Unit | Formula                                   | Character |
|-------------------|----------------------------------------------------------|------|-------------------------------------------|-----------|
| _Work_            | _energy_ transferred by a _force_ over a _displacement_  | J    | $W = \vec{F} \cdot \vec{d}$               | scalar    |
| _Moment of force_ | 3D rotational effect of a _force_ about a reference axis | N·m  | $\vec{M} = \vec{r} \times \vec{F}$        | vector    |
| _Torque_          | scalar magnitude of _moment of force_                    | N·m  | $\tau = \|\vec{r}\|\|\vec{F}\|\sin\theta$ | scalar    |

All three share the same dimension (N·m = J), yet are physically and mathematically
distinct. _Work_ and _moment of force_ further differ in which input vector is involved
($\vec{d}$ — _displacement_ of the application point, vs. $\vec{r}$ — lever arm from the
reference axis) and in the operation applied (dot product vs. cross product):

$$W = \vec{F} \cdot \vec{d} = |\vec{F}||\vec{d}|\cos\theta_d$$

$$|\vec{M}| = |\vec{r}||\vec{F}|\sin\theta_r$$

`operator*` on two scalar quantities cannot represent either of these correctly — the angle
and the distinction between $\vec{d}$ and $\vec{r}$ are both lost. The result is just a
product of magnitudes that is dimensionally valid for any of the four quantities:

```cpp
namespace bu = boost::units;

// Scalar case — all libraries accept this, but it is fundamentally ambiguous:
bu::quantity<bu::si::force>  force_1d = 10. * bu::si::newton;
bu::quantity<bu::si::length> disp_1d  =  5. * bu::si::meter;
auto result = force_1d * disp_1d;  // ✅ All libraries: 50 N·m
// No library can tell: is this work? mechanical energy? moment of force? torque?
// The angle and the input vector identity are both gone.
```

Only when the representation type carries the full 3D vector does the distinction become
meaningful — and then `operator*` is no longer sufficient, because `vec3 * vec3` is
undefined:

```cpp
namespace bu = boost::units;
using vec3 = cartesian_vector<double>;

// Vector case — fails to compile in every library:
auto force_3d = bu::quantity<bu::si::force,  vec3>::from_value(vec3{10., 0., 0.});
auto disp_3d  = bu::quantity<bu::si::length, vec3>::from_value(vec3{ 5., 0., 0.});
// auto wrong = force_3d * disp_3d;  // ❌ All libraries: compile error!
// `vec3 * vec3` is not defined — the error comes from the representation type,
// not from the units library itself. The fix is to use scalar_product / vector_product.
```

!!! note "The 'vector of quantities' workaround — and why it falls short"

    Faced with this limitation, a common workaround is to store a physical vector quantity
    not as a _quantity of a vector_ (`quantity<isq::force[N], vec3>`) but as a
    _vector of quantities_ (`cartesian_vector<quantity<isq::force[N]>>`).

    This makes `operator*` available component-wise, but it is **semantically wrong** for
    describing a single physical quantity with direction:

    - A `cartesian_vector<quantity<N>>` is just a container. It carries no information that the
      three components together form a single _force_ vector.
    - The units library can no longer enforce character rules — nothing stops you from
      multiplying a "force array" by a "displacement array" component-wise, which is not
      the dot product.
    - Adding or subtracting arrays of different quantity kinds compiles silently.

    _Vector of quantities_ is a perfectly valid pattern for **collections** — state vectors in
    a Kalman filter, rows of a matrix, or per-axis sensor readings. It is **not** a substitute
    for a quantity whose physical nature is inherently directional. For that, only a
    _quantity of a vector_ correctly captures the semantic.

**mp-units V3** solves this at the library level: with a proper vector representation type,
`operator*` between two vector quantities is disabled entirely — the only valid operations
are `scalar_product` and `vector_product`, which produce the correct result type:

```cpp
quantity force = isq::force(vec3{10., 0., 0.} * N);
quantity displacement = isq::displacement(vec3{5., 0., 0.} * m);
quantity position = isq::position_vector(vec3{1., 2., 0.} * m);

// quantity wrong = force * displacement;                                             // ❌ Compile-time error!

// Work is scalar product → scalar result
quantity<isq::work[J]> work = scalar_product(force, displacement);                    // Returns scalar

// Moment of force is vector product → vector result
quantity<isq::moment_of_force[N * m], vec3> moment = vector_product(position, force); // Returns vector

// ❌ Compile-time errors!
// quantity<isq::work[J]> work = vector_product(force, displacement);
// quantity<isq::work[J]> work = magnitude(vector_product(force, displacement));
// quantity<isq::moment_of_force[N * m], vec3> moment = vector_product(force, position);

// Torque is the scalar magnitude of moment of force
quantity<isq::torque[N * m]> torque = magnitude(moment);                              // Returns scalar

// Even though work and torque are both scalars with the same dimension (N·m = J),
// they are distinct quantity kinds — assigning one to the other is an error:
// quantity<isq::torque[N * m]> wrong = work;                                         // ❌ Compile-time error!
// quantity<isq::work[J]> wrong = torque;                                             // ❌ Compile-time error!
// quantity<isq::work[J]> wrong = magnitude(moment);                                  // ❌ Compile-time error!
```

#### AC Power Relationships

AC circuit analysis involves four _power_ quantities that are dimensionally compatible yet
physically and mathematically distinct:

| Quantity         | Unit | Character      | Relationship                                                              |
|------------------|------|----------------|---------------------------------------------------------------------------|
| _Active power_   | W    | real scalar    | _energy_ actually consumed per unit _time_                                |
| _Reactive power_ | var  | real scalar    | _energy_ oscillating between source and load                              |
| _Complex power_  | VA   | complex scalar | $\underline{S} = P + jQ$ — full phasor                                    |
| _Apparent power_ | VA   | real scalar    | $S = \|\underline{S}\| = \sqrt{P^2 + Q^2}$ — magnitude of _complex power_ |

Although W, var, and VA are all dimensionally equivalent to J/s, they are not
interchangeable: _active_ and _reactive_ power cannot be added directly (their sum is
physically meaningless), _apparent power_ must be computed via $\sqrt{P^2 + Q^2}$, and
_complex power_ requires a specific argument order. Without quantity safety, all four
collapse to a single "watts" type and every mistake silently compiles:

```cpp
namespace bu = boost::units;

// All four power quantities collapse to the same type — no distinction is possible:
bu::quantity<bu::si::power> P = 800. * bu::si::watts;
bu::quantity<bu::si::power> Q = 600. * bu::si::watts;   // Oops! var, not W
bu::quantity<bu::si::power> S = 1000. * bu::si::watts;  // Oops! VA, not W

auto nonsense = P + Q;  // Oops! Compiles, but physically meaningless

// apparent power should be hypot(P, Q), not P + Q:
bu::quantity<bu::si::power> S_wrong = P + Q;  // Oops! Wrong formula, wrong unit — silently accepted

// No way to enforce argument order for complex power:
auto complex1 = std::complex{P.value(), Q.value()} * bu::si::watts;  // OK
auto complex2 = std::complex{Q.value(), P.value()} * bu::si::watts;  // Oops! Reversed — silently accepted
```

!!! quote "Production Feedback: Why This Matters"

    At CppCon, an engineer from the power systems domain emphasized: **similar errors mixing
    _active_, _reactive_, and _apparent power_ are prevalent in their field**. He stated that any
    units library—even if standardized—would be **useless in production** unless it correctly
    prevents these mistakes at compile time. This real-world feedback highlights why quantity
    safety is not just theoretical—it's essential for domains where such distinctions are
    safety-critical.

mp-units V3 addresses all of the above concerns directly:

```cpp
quantity P = isq::active_power(800. * W);
// quantity Q = isq::reactive_power(600 * W);             // ❌ Error: wrong unit
quantity Q = isq::reactive_power(600. * var);             // ✅ OK

// Apparent power computed from proper equation
quantity<isq::apparent_power[VA]> S = hypot<VA>(P, Q);    // ✅ OK
// quantity wrong = S.in(W);                              // ❌ Error: wrong unit

// Cannot mix them directly
// auto sum = P + Q;                                      // ❌ Error: invalid operation

// Need to use make_complex_quantity<Rep, Unit>() factory function
quantity<isq::complex_power[VA], std::complex<double>> complex =
    make_complex_quantity(P, Q);                          // ✅ OK
// quantity<isq::complex_power[VA], std::complex<double>> complex =
    make_complex_quantity(Q, P);                          // ❌ Error: wrong order

// quantity<isq::apparent_power[VA]> S2 = imag(complex);  // ❌ Error: wrong quantity kind
quantity<isq::apparent_power[VA]> S2 = modulus(complex);  // ✅ OK

// Can extract active power from real part
// quantity<isq::active_power[W]> P2 = imag(complex);     // ❌ Error: wrong quantity kind
// quantity<isq::reactive_power[var]> Q2 = real(complex); // ❌ Error: wrong quantity kind
quantity<isq::active_power[W]> P2 = real(complex);        // ✅ OK: 800 W
quantity<isq::reactive_power[var]> Q2 = imag(complex);    // ✅ OK: 600 var
```

For more details, see the blog post
[Bringing Quantity-Safety To The Next Level](bringing-quantity-safety-to-the-next-level.md).


## Level 6: Mathematical Space Safety

**Mathematical space safety** distinguishes between fundamental abstractions based on their
mathematical properties:

1. **Points** - Values on an interval scale with an arbitrary or conventional origin
   (e.g., _temperatures_ in °C, _positions_ relative to sea level)
2. **Deltas** - Differences between values (e.g., _temperature_ changes, _displacements_)
3. **Absolute quantities** *(V3 planned)* - Ratio-scale amounts anchored at a true
   physical zero (e.g., _mass_ in kg, _temperature_ in K, _length_ as a size); distinct
   from both point and deltas

### What It Prevents

??? danger "Historical Mishap: Hochrheinbrücke (2003–2004)"

    During construction of the Hochrheinbrücke bridge between Germany and Switzerland,
    engineers needed to account for different national _height_ reference systems:
    Germany's **NHN** (Normalhöhennull) and Switzerland's **"Meter über Meer"**, which
    differ by **27 cm**. The plan was to build the Swiss abutment 27 cm _higher_ to
    compensate. Due to a **sign error** in the offset calculation, it was built 27 cm
    _lower_ instead — a total misalignment of **54 cm**.

    The error was discovered during a site inspection in December 2003, requiring
    costly corrections before the bridge could be completed.

    Mathematical space safety would have made this class of error visible at compile time:
    _heights_ anchored to NHN and Swiss reference are `quantity_point` values with
    different `point_origin` types, and mixing them without an explicit conversion
    is a type error.

Without mathematical space safety, code can perform nonsensical operations:

```cpp
// Without mathematical space safety - all just "temperature":
auto boiling = 100.0;               // °C
auto freezing = 0.0;                // °C
auto sum = boiling + freezing;      // Meaningless: what does 100°C + 0°C mean?
auto doubled = boiling * 2;         // Meaningless: what does 2 × 100°C mean?
auto ratio = boiling / freezing;    // Division by zero or nonsense ratio
```

A dimension-safe library is no better if it provides no affine space support.
[nholthaus/units](https://github.com/nholthaus/units) — one of the most widely used C++
units libraries — has no point/delta distinction: temperature points and deltas share the
same type and every nonsensical operation compiles silently:

```cpp
// nholthaus/units — no affine space support; points and deltas share the same type:
units::temperature::celsius_t room_temp{20.};    //  20°C — a temperature point
units::temperature::celsius_t outside_temp{5.};  //   5°C — another temperature point
units::temperature::celsius_t temp_delta{10.};   //  10 K — intended as a temperature delta

// All three have the same type — no distinction between temperature points and deltas:
auto sum     = room_temp + outside_temp;  // Oops! Adding temperature points is meaningless
auto doubled = room_temp * 2.;            // Oops! 2 × 20°C is physically meaningless

// No way to enforce point + delta vs. point + point:
auto wrong = room_temp + outside_temp;    // Oops! Silently accepted
auto right = room_temp + temp_delta;      // ✅ Correct, but indistinguishable from wrong above
```

### Example

mp-units V3 addresses all of the above concerns directly:

```cpp
// Points: Values on interval scale (arbitrary origin)
quantity_point room_temp    = point<deg_C>(20.);        // quantity<point<deg_C>>
quantity_point outside_temp = point<deg_C>(5.);         // quantity<point<deg_C>>

// auto sum = room_temp + outside_temp;                 // ❌ Compile-time error!
// Error: cannot add two points

quantity temp_diff = room_temp - outside_temp;          // ✅ OK: quantity<delta<K>> — 15 K
quantity_point new_temp  = room_temp + temp_diff;       // ✅ OK: quantity<point<deg_C>>

// Deltas: Differences between values  
quantity temp_change  = delta<K>(10);                   // quantity<delta<K>>
quantity displacement = delta<m>(-5);                   // quantity<delta<m>> — can be negative

quantity total_change = temp_change + temp_change;      // ✅ OK: quantity<delta<K>>, 20 K
quantity_point warmed_room  = room_temp + temp_change;  // ✅ OK: quantity<point<deg_C>>
```

### The Three Abstractions

V2 provides two types; V3 adds **absolute quantities** as a first-class third abstraction:

| Feature                  |                     Point                     |             Absolute *(V3 planned)*             |                Delta                |
|--------------------------|:---------------------------------------------:|:-----------------------------------------------:|:-----------------------------------:|
| **Physical Model**       |                Interval Scale                 |                   Ratio Scale                   |             Difference              |
| **Example**              |  $20\ \mathrm{°C}$, $100\ \mathrm{m\ AMSL}$   |    $293.15\ \mathrm{K}$, $100\ \mathrm{kg}$     | $10\ \mathrm{K}$, $-5\ \mathrm{m}$  |
| **Has True Zero?**       |          No (arbitrary/conventional)          |                 Yes (physical)                  |                 N/A                 |
| **Allows Negative?**     |         Yes (below arbitrary origin)          |                   No (opt-in)                   |       Yes (direction matters)       |
| **Addition (A + A)**     | ❌ Error ($20\ \mathrm{°C} + 10\ \mathrm{°C}$) | ✅ Absolute ($10\ \mathrm{kg} + 5\ \mathrm{kg}$) |               ✅ Delta               |
| **Subtraction (A − A)**  | ✅ Delta ($30\ \mathrm{°C} - 10\ \mathrm{°C}$) |  ✅ Delta ($50\ \mathrm{kg} - 5\ \mathrm{kg}$)   |               ✅ Delta               |
| **Multiplication (k×A)** |     ❌ Error ($2 \times 20\ \mathrm{°C}$)      |     ✅ Absolute ($2 \times 10\ \mathrm{kg}$)     | ✅ Delta ($-2 \times 5\ \mathrm{m}$) |
| **Division (A / A)**     |                    ❌ Error                    |  ✅ Scalar ($10\ \mathrm{kg} / 5\ \mathrm{kg}$)  |          ✅ Scalar (ratio)           |
| **A + Delta**            |                ✅ Point (shift)                |                     ✅ Delta                     |               ✅ Delta               |
| **API**                  |            `quantity<point<...>>`             |                 `quantity<...>`                 |       `quantity<delta<...>>`        |

### Bounds Checking

mp-units provides optional runtime bounds checking for mathematical space conversions:

- **Point bounds checking**: When constructing a quantity point optional bounds checking
  can verify that the resulting point value stays within valid ranges. This is especially
  useful for physical quantities with natural bounds (e.g., preventing negative absolute
  _temperatures_ when working with Kelvin, or wrapping _longitude_ around a
  _prime meridian_ origin). Bounds are
  automatically scaled and translated to remain consistent when the unit changes
  (e.g., bounds specified in meters remain correct when converting to kilometers).
  The library supports multiple boundary semantics:

    - **Clamping**: Values outside the range are clamped to the nearest boundary
    - **Wrap-around**: Values wrap cyclically
    - **Reflect**: Values bounce back at boundaries

- **Absolute quantity bounds checking** *(V3 planned)*: When converting deltas to absolute
  quantities (via `.absolute()`), optional runtime precondition checks verify non-negativity.
  This ensures that negative values can't accidentally become absolute quantities representing
  physical magnitudes that should always be non-negative (like _mass_, _distance_, _duration_).
  The precondition check fails at runtime if the value is negative, providing an additional
  safety layer beyond compile-time type distinctions.

### Why This Matters

**Mathematical space safety prevents an entire class of conceptual errors** that dimension
safety alone cannot catch. Consider these examples:

- **Temperature control**: _Room temperature_ (point on °C scale) vs. _temperature change_
  (delta in K)
- **GPS navigation**: _Position_ (point) vs. _displacement_ (delta/vector)
- **Timestamps**: _Timestamp_ (point — specific instant relative to an epoch) vs.
  _duration_ (absolute — always non-negative elapsed time)
- **Absolute amounts vs. deltas**: In V2, `quantity<kg>` is used for both an absolute
  amount of _mass_ (e.g., total _mass_ of a sample — a ratio-scale value anchored at true
  zero) and a _mass_ delta (e.g., the change in _mass_ — a signed difference). The type
  system cannot distinguish them, so argument ordering mistakes and other semantic errors
  compile silently. V3 fixes this by making absolute quantities a first-class type.

### Evolution in mp-units

=== "V2 (Current)"

    ```cpp
    // Both 'water_lost' and 'total_initial' have the same type quantity<kg>.
    // Nothing prevents passing them in the wrong order:
    quantity<percent> moisture_content_change(quantity<kg> water_lost,
                                              quantity<kg> total) { ... };

    quantity<kg> initial[] = { 2.34 * kg, 1.93 * kg, 2.43 * kg };
    quantity<kg> dried[]   = { 1.89 * kg, 1.52 * kg, 1.92 * kg };

    quantity<kg> total_initial = std::reduce(...);
    quantity<kg> total_dried   = std::reduce(...);
    quantity<kg> water_lost    = total_initial - total_dried;  // delta (difference)

    moisture_change(total_initial, water_lost);  // 🤔 Swapped arguments — compiles silently!
    ```

    Two abstractions:

    - `quantity_point<...>` for points
    - `quantity<...>` for deltas (also used for absolute amounts — no distinction)

    **`quantity<T>` serves double duty as both an absolute amount and a delta** — the type
    system cannot distinguish them. This makes function signatures ambiguous and lets
    argument-ordering mistakes slip through silently.

=== "V3 (Planned)"

    ```cpp
    // Function signature now expresses distinct roles — types are incompatible:
    quantity<percent> moisture_content_change(quantity<delta<kg>> water_lost,
                                              quantity<kg>        total) { ... };

    quantity<kg> initial[] = { 2.34 * kg, 1.93 * kg, 2.43 * kg };
    quantity<kg> dried[]   = { 1.89 * kg, 1.52 * kg, 1.92 * kg };

    quantity<kg>        total_initial = std::reduce(...);            // absolute (default in V3)
    quantity<kg>        total_dried   = std::reduce(...);
    quantity<delta<kg>> water_lost    = total_initial - total_dried; // delta (explicit)

    // moisture_change(total_initial, water_lost);  // ❌ Compile-time error! Types don't match
    moisture_change(water_lost, total_initial);      // ✅ OK
    ```

    Three first-class abstractions:

    - `quantity<...>` for **absolute quantities** (new default — ratio scale, true physical zero)
    - `quantity<delta<...>>` for deltas (signed differences, always explicit)
    - `quantity<point<...>>` for points (replaces `quantity_point<...>`)

    All three roles are now explicit and distinct — argument-ordering mistakes become
    compile-time errors.

The arithmetic rules encode physical validity at the type level:

| Operation             | Result   | Notes                                |
|:----------------------|:---------|:-------------------------------------|
| `absolute + absolute` | Absolute | Sum of two non-negative amounts      |
| `absolute - absolute` | Delta    | Difference — may be negative         |
| `absolute + delta`    | Delta    | Result sign unknown at compile time  |
| `delta + delta`       | Delta    | Combined signed change               |
| `norm(vector_delta)`  | Absolute | Euclidean norm — always non-negative |

When an absolute result is needed from an operation that conservatively yields a delta,
call `.absolute()` explicitly — this checks the non-negativity precondition at runtime:

```cpp
quantity<kg> remaining = (total - used).absolute();  // explicit, runtime-checked
```

This converts a V2 silent assumption into an explicit, checked V3 contract.

For more details, see [Introducing Absolute Quantities](introducing-absolute-quantities.md).


## Error Classes Prevented by Each Level

Each safety level prevents a different class of errors:

1. **Dimension Safety** → Dimensional analysis errors
    - Prevents: Adding meters to seconds, multiplying incompatible dimensions
    - Impact: Eliminates basic dimension mistakes

2. **Unit Safety** → Interface boundary errors
    - Prevents: Passing m/s when km/h expected, unsafe value extraction
    - Impact: Eliminates unit mismatch bugs at integration points

3. **Representation Safety** → Numerical errors
    - Prevents: Silent truncation, overflow, underflow
    - Impact: Protects against data loss

4. **Quantity Kind Safety** → Conceptual errors
    - Prevents: Mixing _frequency_ (Hz) with _activity_ (Bq), _plane angles_ (rad)
      with _solid angles_ (sr)
    - Impact: Distinguishes physically different concepts with same dimension

5. **Quantity Safety** → Hierarchy and equation errors
    - Prevents: Using generic _length_ instead of _height_ in _potential energy_; mixing scalar
      and vector operations
    - Impact: Ensures physics equations use correct specialized quantities

6. **Mathematical Space Safety** → Point/absolute/delta confusion
    - Prevents: Adding absolute _temperatures_; using deltas where absolute quantities expected;
      arithmetic on interval-scale values
    - Impact: Enforces correct mathematical model (interval vs. ratio scale)


## Comprehensive Library Comparison

Now that we've explored all six safety levels in detail, let's examine how **mp-units**
compares to other units libraries—both within the C++ ecosystem and across programming
languages. This comparison will help contextualize **mp-units**' capabilities and design
choices relative to industry-leading alternatives.

### C++ Libraries

The C++ ecosystem offers several mature units libraries, each with different design philosophies
and trade-offs. We compare mp-units against the most prominent alternatives:
[Boost.Units](https://www.boost.org/doc/libs/release/doc/html/boost_units.html)
(pioneering pre-C++14 solution), [nholthaus/units](https://github.com/nholthaus/units)
(modern C++14 library), [bernedom/SI](https://github.com/bernedom/SI) (C++17 minimalist
approach), and [Au](https://github.com/aurora-opensource/au) (production-tested C++14
library from Aurora Innovation).

[![Star History Chart - C++ Libraries](https://api.star-history.com/svg?repos=mpusz/mp-units,nholthaus/units,bernedom/SI,aurora-opensource/au,boostorg/units&type=Date&legend=top-left)](https://star-history.com/#mpusz/mp-units&nholthaus/units&bernedom/SI&aurora-opensource/au&boostorg/units&Date&legend=top-left)

The following table compares safety features across major C++ units libraries:

<!-- markdownlint-disable MD013 -->
<div style="overflow: visible;">
<table class="comparison-table">
<thead>
<tr>
<th>Safety Feature</th>
<th>mp-units<br/>(current)</th>
<th>mp-units<br/>(V3 planned)</th>
<th>Boost.Units</th>
<th>nholthaus</th>
<th>SI</th>
<th>Au</th>
</tr>
</thead>
<tbody>
<tr>
<td><strong>Minimum Requirement</strong></td>
<td>C++20</td>
<td>C++20</td>
<td>C++11/14</td>
<td>C++14</td>
<td>C++17</td>
<td>C++14</td>
</tr>
<tr>
<td><strong>Dimension Safety</strong></td>
<td>✅ Full</td>
<td>✅ Full</td>
<td>✅ Full</td>
<td>✅ Full</td>
<td>✅ Full</td>
<td>✅ Full</td>
</tr>
<tr>
<td><strong>Unit Safety</strong></td>
<td>✅ Full</td>
<td>✅ Full</td>
<td><span class="tooltip">🔶 Limited<span class="tooltiptext">Cross-unit conversion requires explicit construction: <code>quantity&lt;target_unit&gt;(q)</code>; implicit conversion is only allowed when units reduce to identical base units (e.g., SI seconds ↔ CGS seconds). Working with prefixed SI units (e.g., kilometres, milliseconds) or custom units requires verbose boilerplate — defining base units, scaled units, and conversion factors separately</span></span></td>
<td><span class="tooltip">🟡 Partial<span class="tooltiptext">Implicit conversions between compatible units; less strict than mp-units</span></span></td>
<td><span class="tooltip">🔶 Limited<span class="tooltiptext">Basic unit type system; limited compile-time enforcement of unit correctness</span></span></td>
<td>✅ Full</td>
</tr>
<tr>
<td><strong>Representation Safety</strong></td>
<td><span class="tooltip">⭐ Strong<span class="tooltiptext">Compile-time: blocks conversions where the scaling factor definitely overflows the representation type; fixed-point arithmetic prevents intermediate overflow during non-integer unit scaling; does not suppress <code>-Wconversion</code> in internal calculations so compiler warnings remain actionable. Runtime: <code>safe_int&lt;T&gt;</code> drop-in wrapper detects arithmetic overflow for all operations including hidden common-unit arithmetic in <code>operator+</code>/<code>operator==</code>; origin bounds policies enforce domain range constraints on quantity points.</span></span></td>
<td><span class="tooltip">⭐ Strong<span class="tooltiptext">Same compile-time and runtime guarantees as current mp-units, extended to cover absolute quantities and delta types introduced in V3.</span></span></td>
<td><span class="tooltip">🔶 Limited<span class="tooltiptext">No systematic overflow detection; representation is a template parameter (<code>quantity&lt;Unit, Y&gt;</code>), so safety depends entirely on the chosen <code>Y</code> — no defaults, no guidance, and no built-in checks</span></span></td>
<td><span class="tooltip">🟡 Partial<span class="tooltiptext">Uses floating-point by default, which avoids truncation in practice; no systematic overflow detection. Integer representations silently truncate on unit conversion — no compile-time guard</span></span></td>
<td><span class="tooltip">🔶 Limited<span class="tooltiptext">Minimal representation type checking; primarily focused on correct dimensions</span></span></td>
<td><span class="tooltip">⭐ Strong<span class="tooltiptext">Compile-time: adaptive "smallest overflowing value" threshold (2'147) — more aggressive than mp-units but produces false positives requiring opt-out via <code>ignore(OVERFLOW_RISK)</code> or <code>ignore(TRUNCATION_RISK)</code>; disallows risky integer <code>Quantity / Quantity</code> division to prevent divide-before-convert trap, with escape hatches <code>unblock_int_div()</code> and <code>divide_using_common_unit()</code>. Runtime: opt-in helpers (<code>will_conversion_overflow</code>, <code>will_conversion_truncate</code>, <code>is_conversion_lossy</code>) check explicit conversions only — no automatic detection of overflow in common-unit arithmetic or equivalent to <code>safe_int</code>.</span></span></td>
</tr>
<tr>
<td><strong>Quantity Kind Safety</strong></td>
<td>✅ Full</td>
<td>✅ Full</td>
<td><span class="tooltip">🔶 Limited<span class="tooltiptext">Custom quantity types possible but not systematically enforced</span></span></td>
<td>❌ None</td>
<td>❌ None</td>
<td><span class="tooltip">❌ None<span class="tooltiptext">Au documentation states 'No plans at present to support' quantity kind types; intentional design tradeoff to reduce learning curve and improve compiler error readability</span></span></td>
</tr>
<tr>
<td><strong>Quantity Safety</strong></td>
<td><span class="tooltip">🟡 Partial<span class="tooltiptext">Provides quantity hierarchy with named types; V3 will add proper vector, tensor, and complex number support</span></span></td>
<td>✅ Full</td>
<td>❌ None</td>
<td>❌ None</td>
<td>❌ None</td>
<td><span class="tooltip">❌ None<span class="tooltiptext">No support for quantity character (scalar/vector/complex) or quantity hierarchies. Vector support is planned (GitHub issue #70)</span></span></td>
</tr>
<tr>
<td><strong>Mathematical Space Safety</strong></td>
<td><span class="tooltip">⭐ Strong<span class="tooltiptext">Points (<code>quantity_point</code>) with sophisticated multi-layered origin system (<code>natural_point_origin</code>, <code>absolute_point_origin</code>, <code>relative_point_origin</code> that can be hierarchically stacked) and deltas (<code>quantity</code>) are fully distinct; promoted for widespread use (timestamps, altitudes, odometer readings, etc.). Supports optional runtime bounds checking for points. Key limitation: no first-class absolute quantity type until V3 — absolute amounts (ratio-scale values) share <code>quantity&lt;T&gt;</code> with deltas</span></span></td>
<td><span class="tooltip">✅ Full<span class="tooltiptext">Three first-class abstractions: absolute quantities (ratio-scale, new default), explicit deltas (<code>quantity&lt;delta&lt;...&gt;&gt;</code>), and points (<code>quantity&lt;point&lt;...&gt;&gt;</code>); covers all mathematical space scenarios</span></span></td>
<td><span class="tooltip">🔶 Limited<span class="tooltiptext">Generic <code>absolute&lt;&gt;</code> wrapper distinguishes points from deltas with correct basic semantics: <code>absolute&lt;T&gt; +/- T → absolute&lt;T&gt;</code> and <code>absolute&lt;T&gt; - absolute&lt;T&gt; → T</code>. Works for temperature and other use cases. Key limitation: no typed origins, so two points in different reference frames (e.g., heights above NHN vs. MüM) are the same type and mixing them still silently compiles</span></span></td>
<td><span class="tooltip">❌ None<span class="tooltiptext">nholthaus does have offset-aware unit conversions (e.g., <code>celsius_t</code> ↔ <code>kelvin_t</code> applies the +273.15 offset correctly), but this is purely a conversion feature. There is no separate point type: <code>celsius_t</code> and a “temperature delta in °C” are the same type, so adding two temperature points, or scaling one, compiles silently</span></span></td>
<td>❌ None</td>
<td><span class="tooltip">🟡 Partial<span class="tooltiptext"><code>QuantityPoint</code> type with typed origins (origins embedded in unit definitions) prevents mixing points from different reference frames; used selectively for temperature and special cases, not promoted as general-purpose tool; supports bidirectional conversions via <code>CorrespondingQuantity</code>. Key gap: no first-class absolute quantity type — ratio-scale amounts (e.g., mass, duration) share the same type as deltas, just as in mp-units V2</span></span></td>
</tr>
</tbody>
</table>
</div>
<!-- markdownlint-enable MD013 -->

**Legend:**

- ✅ **Full**: Comprehensive implementation covering all aspects
- ⭐ **Strong**: Robust implementation with minor limitations
- 🟡 **Partial**: Some aspects implemented but not comprehensive
- 🔶 **Limited**: Basic support with significant gaps
- ❌ **None**: This safety feature is not provided

**Key Observations:**

- **Dimension Safety**: All modern C++ units libraries provide this foundational feature
- **Unit Safety**: mp-units and Au provide complete unit-safe interfaces; other libraries
  allow varying degrees of implicit conversions
- **Representation Safety**: mp-units and Au both provide strong compile-time overflow/
  truncation protection; Au uses adaptive "smallest overflowing value" threshold (2'147),
  mp-units checks scaling factor magnitude; Au provides opt-out mechanisms and runtime
  helpers; mp-units additionally provides `safe_int<T>` for comprehensive runtime
  coverage of all arithmetic including common-unit operations
- **Quantity Kind Safety**: Only mp-units provides full quantity kind safety, distinguishing
  Hz/Bq, rad/sr, Gy/Sv—other libraries either lack this feature or provide partial support
- **Quantity Safety**: mp-units is unique in providing systematic quantity hierarchies;
  this level of semantic type safety is absent from other C++ libraries
- **Mathematical Space Safety**: mp-units provides the most sophisticated point/delta
  system with multi-layered typed origins (natural/absolute/relative, hierarchically
  stackable) + runtime bounds checking for point conversions, promoted for widespread
  use; Au offers QuantityPoint with typed origins embedded in units, used selectively for
  temperature/special cases; Boost.Units provides only basic `absolute<>` wrapper without
  typed origins (cannot prevent mixing points from different reference frames). None have
  first-class absolute quantity types — that is unique to mp-units V3, which adds
  ratio-scale absolute quantities as a distinct third abstraction with optional runtime
  bounds checking for non-negativity
- **C++ Standard Requirement**: mp-units requires C++20 — a higher entry bar than Au (C++14)
  or nholthaus/units (C++14), which matters for industrial and embedded projects still on
  legacy toolchains; Au demonstrates that strong safety guarantees are achievable on C++14,
  but C++20 features — NTTPs, `concepts`, and class non-type template parameters — allow
  mp-units to expose them through a significantly more ergonomic, user-friendly API

### Cross-Language Libraries

According to [star-history.com](https://www.star-history.com),
**mp-units** directly competes with industry-leading units libraries from other languages.
While **mp-units** operates within the C++ ecosystem, comparing it to cross-language
competitors provides important context about its capabilities and design sophistication
at the industry level. We compare against: [Pint](https://github.com/hgrecco/pint) (Python),
[JSR-385](https://github.com/unitsofmeasurement/unit-api) (Java),
[UOM](https://github.com/iliekturtles/uom) (Rust),
[UnitsNet](https://github.com/angularsen/UnitsNet) (.NET),
[Unitful.jl](https://github.com/PainterQubits/Unitful.jl) (Julia),
and [F# Units of Measure](https://learn.microsoft.com/en-us/dotnet/fsharp/language-reference/units-of-measure)—a
unique case where dimensional analysis is a first-class language feature.

[![Star History Chart - Cross-Language](https://api.star-history.com/svg?repos=mpusz/mp-units,angularsen/UnitsNet,hgrecco/pint,unitsofmeasurement/indriya,iliekturtles/uom,PainterQubits/Unitful.jl&type=Date&legend=top-left)](https://star-history.com/#mpusz/mp-units&angularsen/UnitsNet&hgrecco/pint&unitsofmeasurement/indriya&iliekturtles/uom&PainterQubits/Unitful.jl&Date&legend=top-left)

The following table compares **mp-units** to leading units libraries from other programming
languages:

<!-- markdownlint-disable MD013 -->
<div style="overflow: visible;">
<table class="comparison-table">
<thead>
<tr>
<th>Safety Feature</th>
<th>mp-units<br/>(current)</th>
<th>mp-units<br/>(V3 planned)</th>
<th>Pint</th>
<th>JSR-385</th>
<th>UOM</th>
<th>UnitsNet</th>
<th>Unitful.jl</th>
<th>F# Units</th>
</tr>
</thead>
<tbody>
<tr>
<td><strong>Language</strong></td>
<td>C++20</td>
<td>C++20</td>
<td>Python</td>
<td>Java</td>
<td>Rust</td>
<td>C#</td>
<td>Julia</td>
<td>F#</td>
</tr>
<tr>
<td><strong>Dimension Safety</strong></td>
<td>✅ Full</td>
<td>✅ Full</td>
<td>✅ Full</td>
<td>✅ Full</td>
<td>✅ Full</td>
<td>✅ Full</td>
<td>✅ Full</td>
<td>✅ Full</td>
</tr>
<tr>
<td><strong>Unit Safety</strong></td>
<td>✅ Full</td>
<td>✅ Full</td>
<td><span class="tooltip">✅ Full<span class="tooltiptext">Automatically converts mixed-unit operations (e.g., <code>meters + feet</code>) through UnitRegistry; however, lacks enforcement against mixing quantities from different registry instances</span></span></td>
<td><span class="tooltip">🟡 Partial<span class="tooltiptext">Statically typed via Java generics (<code>Quantity&lt;Length&gt;</code>) prevents mixing incompatible dimensions at compile time; mixed-unit operations correctly convert to SI system units first per spec (e.g., <code>4 cm + 1 inch = 0.0654 m</code>); however value extraction via <code>.getValue()</code> returns raw numeric with no required unit specification, and unit tracking is runtime-only with no compile-time unit in the type</span></span></td>
<td>✅ Full</td>
<td><span class="tooltip">🟡 Partial<span class="tooltiptext">Automatic conversions between compatible units (e.g., <code>m + ft</code>), but lacks strict enforcement against mixed-unit operations in all runtime contexts</span></span></td>
<td>✅ Full</td>
<td>✅ Full</td>
</tr>
<tr>
<td><strong>Representation Safety</strong></td>
<td>⭐ Strong</td>
<td>⭐ Strong</td>
<td><span class="tooltip">🔶 Limited<span class="tooltiptext">No compile-time representation safety: all checks are runtime-only and manifest as <code>DimensionalityError</code> or silent precision loss. Handles large integers via Python's arbitrary-precision <code>int</code>, but floating-point uses standard IEEE 754 <code>float</code> with potential precision loss; no overflow protection for dimensioned calculations</span></span></td>
<td><span class="tooltip">🔶 Limited<span class="tooltiptext">Defaults to <code>double</code> for most quantities; supports custom representations via <code>Quantity&lt;Q, N&gt;</code> but lacks automatic precision handling for edge cases like overflow; narrowing conversions between numeric types require explicit casts but are not systematically prevented at the API boundary</span></span></td>
<td><span class="tooltip">⭐ Strong<span class="tooltiptext">Rust's type system prohibits implicit narrowing conversions at the language level (e.g., assigning <code>f64</code> to <code>f32</code> without an explicit cast is a compile error); uom inherits this guarantee — any precision-losing conversion must be explicit and is therefore intentional. uom is also generic over any <code>Num</code>-constrained representation type, giving full control over numeric precision</span></span></td>
<td><span class="tooltip">🟡 Partial<span class="tooltiptext">Uses <code>QuantityValue</code> to specify numeric types (e.g., <code>double</code>, <code>decimal</code>); supports saturation arithmetic but doesn't enforce representation constraints at type level</span></span></td>
<td><span class="tooltip">🟡 Partial<span class="tooltiptext">Accepts any Julia numeric type (including custom types with appropriate traits) but doesn't enforce specific types for physical correctness or overflow prevention</span></span></td>
<td><span class="tooltip">🔶 Limited<span class="tooltiptext">Limited to numeric primitives (<code>int</code>, <code>float</code>, <code>decimal</code>); F# does not permit implicit narrowing between <em>different</em> primitive types, but provides no systematic overflow or precision-loss protection within a single type — inherits standard .NET numeric behavior</span></span></td>
</tr>
<tr>
<td><strong>Quantity Kind Safety</strong></td>
<td>✅ Full</td>
<td>✅ Full</td>
<td><span class="tooltip">❌ None<span class="tooltiptext">Uses dimensions to categorize quantities but does not systematically distinguish dimensionally equivalent kinds like Torque vs Energy or Gray vs Sievert</span></span></td>
<td><span class="tooltip">🟡 Partial<span class="tooltiptext">Provides separate interfaces for most quantity kinds including dimensionally equivalent pairs: <code>Frequency</code> vs <code>Radioactivity</code> (both s⁻¹) and <code>RadiationDoseAbsorbed</code> vs <code>RadiationDoseEffective</code> (Gy/Sv); however no separate <code>Torque</code> interface — torque and energy both fall under <code>Energy</code></span></span></td>
<td><span class="tooltip">🟡 Partial<span class="tooltiptext">Dimensionally equivalent quantities (e.g., <code>Energy</code> and <code>Torque</code>, both <code>kg·m²·s⁻²</code>; <code>Frequency</code> and <code>Radioactivity</code>, both <code>s⁻¹</code>) map to the same Rust generic type <code>Quantity&lt;ISQ&lt;...&gt;, SI, V&gt;</code> at compile time and are freely interchangeable; the optional <code>Kind</code> trait can differentiate them but is not applied to the built-in SI quantities</span></span></td>
<td><span class="tooltip">🟡 Partial<span class="tooltiptext">Code-generates a separate, strongly-typed C# struct for each quantity kind (e.g., <code>Energy</code>, <code>Torque</code>, <code>Frequency</code>, <code>Radioactivity</code>, <code>Angle</code>, <code>SolidAngle</code>); mixing incompatible kinds is a compile-time type error; does not formally follow ISO 80000 / ISQ but covers most practically relevant kind distinctions including Hz/Bq and Gy/Sv as separate types</span></span></td>
<td><span class="tooltip">🟡 Partial<span class="tooltiptext">Distinguishes quantities by units but not by kind within same dimensions; torque and energy are both <code>Quantity{Float64, 𝐋²𝐌𝐓⁻², ...}</code> without semantic separation</span></span></td>
<td><span class="tooltip">🔶 Limited<span class="tooltiptext">Does not distinguish dimensionally equivalent quantities at type level (e.g., torque and energy are both represented as <code>float&lt;N·m&gt;</code> with identical type signatures)</span></span></td>
</tr>
<tr>
<td><strong>Quantity Safety</strong></td>
<td><span class="tooltip">🟡 Partial<span class="tooltiptext">Provides quantity hierarchy with named types; V3 will add proper vector, tensor, and complex number support</span></span></td>
<td>✅ Full</td>
<td>❌ None</td>
<td>❌ None</td>
<td>❌ None</td>
<td>❌ None</td>
<td>❌ None</td>
<td>❌ None</td>
</tr>
<tr>
<td><strong>Mathematical Space Safety</strong></td>
<td><span class="tooltip">🟡 Partial<span class="tooltiptext">Points (<code>quantity_point</code>) and deltas (<code>quantity</code>) are fully distinct; however, absolute amounts (ratio-scale values) share <code>quantity&lt;T&gt;</code> with deltas — no first-class absolute quantity type until V3</span></span></td>
<td><span class="tooltip">✅ Full<span class="tooltiptext">Three first-class abstractions: absolute quantities (ratio-scale, new default), explicit deltas (<code>quantity&lt;delta&lt;...&gt;&gt;</code>), and points (<code>quantity&lt;point&lt;...&gt;&gt;</code>); covers all mathematical space scenarios</span></span></td>
<td><span class="tooltip">🔶 Limited<span class="tooltiptext">Explicit delta units (<code>delta_degC</code>, <code>delta_degF</code>) and offset-unit semantics for temperature only: subtracting two temperature points yields a delta, adding a point and a delta works correctly; not a general point/delta type system — no typed origins or QuantityPoint abstraction</span></span></td>
<td><span class="tooltip">🔶 Limited<span class="tooltiptext"><code>Quantity.Scale.ABSOLUTE</code> / <code>RELATIVE</code> enum (since v2.0) distinguishes absolute-scale (e.g., Kelvin) from relative-scale (e.g., °C/°F) temperatures; however scale is a runtime property only — no compile-time type-level distinction — and there is no general point/delta abstraction beyond temperature</span></span></td>
<td><span class="tooltip">🔶 Limited<span class="tooltiptext">Separate <code>ThermodynamicTemperature</code> (point) and <code>TemperatureInterval</code> (delta) types enforce correct semantics for temperature: adding two <code>ThermodynamicTemperature</code> values is a compile error, but <code>temperature + interval</code> works; temperature-specific — no general affine space abstraction for arbitrary quantities</span></span></td>
<td>❌ None</td>
<td><span class="tooltip">🔶 Limited<span class="tooltiptext">Built-in <code>AffineQuantity</code> type for <code>°C</code>/<code>°F</code> (relative-scale temperatures); invalid operations like <code>32°F + 1°F</code> throw <code>AffineError</code>; the <code>@affineunit</code> macro allows defining custom affine units beyond temperature; however no formal three-way point/delta/absolute distinction and no typed origins</span></span></td>
<td>❌ None</td>
</tr>
<tr>
<td><strong>Runtime Performance Overhead</strong></td>
<td><span class="tooltip">⚡ None<span class="tooltiptext">Zero-cost abstraction; units completely erased at compile-time</span></span></td>
<td><span class="tooltip">⚡ None<span class="tooltiptext">Zero-cost abstraction; units completely erased at compile-time</span></span></td>
<td><span class="tooltip">🔴 Very High<span class="tooltiptext">Reported 38-1461× slower for basic operations; actual overhead highly dependent on usage patterns and wrapping strategies (can be reduced to ~7× with proper wrapping)</span></span></td>
<td><span class="tooltip">🔴 Very High<span class="tooltiptext">650× slower for addition (2.88 μs vs 4.43 ns); 1470× for bulk operations</span></span></td>
<td><span class="tooltip">⚡ None<span class="tooltiptext">Zero-cost verified through assembly inspection; monomorphization produces identical code to raw <code>f64</code></span></span></td>
<td><span class="tooltip">🟡 Low<span class="tooltiptext">2.4× for arithmetic operations; 10× for unit conversions due to runtime lookup tables</span></span></td>
<td><span class="tooltip">⚡ None<span class="tooltiptext">Zero-cost when type-stable (verified via LLVM IR); 130× slower with type instability</span></span></td>
<td><span class="tooltip">⚡ None<span class="tooltiptext">Zero-cost verified through IL inspection; units completely erased at compile-time</span></span></td>
</tr>
<tr>
<td><strong>Memory Overhead</strong></td>
<td><span class="tooltip">⚡ None<span class="tooltiptext">Zero overhead; stores only the numeric value (8 bytes for <code>double</code>)</span></span></td>
<td><span class="tooltip">⚡ None<span class="tooltiptext">Zero overhead; stores only the numeric value (8 bytes for <code>double</code>)</span></span></td>
<td><span class="tooltip">🔴 Very High<span class="tooltiptext">~700 bytes retained size (includes unit registry references); ~56 bytes direct object size vs 24 bytes for <code>float</code></span></span></td>
<td><span class="tooltip">🔴 Very High<span class="tooltiptext">~500 bytes per Quantity including JVM object header, value reference, and unit metadata graph</span></span></td>
<td><span class="tooltip">⚡ None<span class="tooltiptext">Stores only the numeric value (8 bytes for <code>f64</code>); units exist only in type system</span></span></td>
<td><span class="tooltip">🟡 Low<span class="tooltiptext">12-16 bytes (8-byte <code>double</code> + 4-byte unit enum + padding) vs 8 bytes for raw <code>double</code></span></span></td>
<td><span class="tooltip">⚡ None<span class="tooltiptext">Immutable structs stored inline; 8 bytes for <code>Float64</code>, no additional runtime unit storage</span></span></td>
<td><span class="tooltip">⚡ None<span class="tooltiptext">Identical memory layout to primitive <code>float</code> (8 bytes); units erased during compilation</span></span></td>
</tr>
<tr>
<td><strong>Compile-time Cost</strong></td>
<td><span class="tooltip">🔴 High<span class="tooltiptext">Heavy template metaprogramming; incremental compilation and precompiled modules help mitigate costs</span></span></td>
<td><span class="tooltip">🔴 High<span class="tooltiptext">Heavy template metaprogramming; incremental compilation and precompiled modules help mitigate costs</span></span></td>
<td><span class="tooltip">⚡ Minimal<span class="tooltiptext">Dynamic language with minimal compilation overhead; units checked at runtime</span></span></td>
<td><span class="tooltip">🟢 Low<span class="tooltiptext">JVM bytecode compilation relatively fast; generic types add modest overhead</span></span></td>
<td><span class="tooltip">🟡 Moderate<span class="tooltiptext">Rust's trait system and monomorphization add compile-time cost but less than C++ templates</span></span></td>
<td><span class="tooltip">🟢 Low<span class="tooltiptext">JIT compilation; struct-based design adds minimal overhead</span></span></td>
<td><span class="tooltip">🟡 Moderate<span class="tooltiptext">Julia's type specialization over physical unit types adds non-trivial first-run JIT compilation cost; complex expressions with many unit types exacerbate this (the well-known "time-to-first-X" problem applies to Unitful.jl-heavy code)</span></span></td>
<td><span class="tooltip">⚡ Minimal<span class="tooltiptext">Units completely erased at compile-time; minimal impact on compilation speed</span></span></td>
</tr>
</tbody>
</table>
</div>
<!-- markdownlint-enable MD013 -->

**Key Observations:**

- **Dimension Safety**: All mature units libraries provide this foundational safety level
- **Unit Safety**: mp-units, UOM, Unitful.jl, and F# Units provide the strongest compile-time
  guarantees; F# is unique as units are a built-in language feature with zero runtime cost;
  JSR-385 enforces dimension compatibility via Java generics but tracks units at runtime only
- **Enforcement Timing**: This is a critical distinction often overlooked: C++, Rust, and
  F# libraries enforce all safety levels at *compile time* — violations are build errors
  with zero runtime cost; Python (Pint) and Java (JSR-385) enforce safety at *runtime* —
  violations are exceptions or silent errors that only appear during execution; a
  `DimensionalityError` in Pint is a crash, not a compiler diagnostic
- **Representation Safety**: mp-units provides strong compile-time overflow/truncation
  protection; UOM benefits from Rust's explicit integer handling; runtime libraries
  (Pint, JSR-385) provide no representation safety
- **Quantity Kind Safety**: mp-units uniquely provides full quantity kind safety based on
  ISO 80000; UnitsNet and JSR-385 cover most practical distinctions (Hz/Bq, Gy/Sv, etc.)
  as separate types but without a formal ISQ basis; UOM's built-in SI quantities make
  dimensionally equivalent kinds (e.g., Energy/Torque) accidentally interchangeable unless
  the optional Kind trait is applied
- **Quantity Safety**: mp-units V3 will be the only **zero-overhead, compile-time** library
  with systematic quantity hierarchies based on ISO 80000; JSR-385 (Java) provides some
  runtime quantity separation but with substantial heap overhead and no ISQ-grounded hierarchy;
  no other library in this comparison provides scalar/vector/tensor character support
- **Mathematical Space Safety**: mp-units V3 will provide comprehensive abstraction for
  points, absolute quantities (with optional runtime bounds checking for non-negativity),
  and deltas; mp-units V2 already offers the most sophisticated point/delta system with
  multi-layered typed origins + runtime bounds checking for point conversions; Pint,
  JSR-385, UOM, and Unitful.jl each offer partial temperature-specific affine support with
  varying levels of generality and compile-time enforcement; UnitsNet and F# Units provide
  no mathematical space abstractions
- **Language Integration**: F# demonstrates what's possible when units are a first-class
  language feature
- **Python's Zero-Overhead Path**: While Pint relies on runtime object wrapping,
  [`impunity`](https://github.com/achevrot/impunity) (TU Delft) demonstrates that AST rewriting
  at function-definition time can achieve dimension-safety with zero runtime overhead — using
  Python's annotation syntax to check units statically rather than tracking them at runtime;
  `impunity` is limited to dimension-level checks and does not support quantity kinds, affine
  spaces, or character safety, but it shows that Python's "runtime tax" on unit safety is
  not inevitable
- **Performance Architecture**: The overhead spectrum divides into three categories:
    - **Zero-cost compiled** (C++, Rust, F#): Units verified at compile-time, completely
      erased at runtime
    - **Optimizable JIT** (Julia, C#): Type-stable code achieves zero/minimal cost;
      UnitsNet shows ~2.4× overhead for structs
    - **Object-wrapped dynamic** (Python, Java): Object instantiation dominates; Pint
      shows 38-1460× overhead, JSR-385 ~650×
- **Memory Trade-offs**: Heap-allocated objects incur massive overhead—Pint (~700 bytes)
  and JSR-385 (~500 bytes) vs zero overhead for compiled languages and type-erased
  implementations; UnitsNet's struct approach adds only 4-8 bytes
- **The "Type Stability Tax"**: Julia and managed runtimes demonstrate that zero-cost is achievable
  in JIT environments only when types are known at compile-time; dynamic dispatch reintroduces
  substantial overhead (Julia: 130× when type-unstable)
- **Compilation Cost**: Compile-time type safety comes with build-time costs—mp-units and
  UOM pay the highest price for their strong safety guarantees, while dynamic languages
  have minimal compilation overhead but sacrifice compile-time error detection
- **Official Standards**: JSR-385 represents Java's official units API, demonstrating that
  even standardized approaches face fundamental runtime overhead in managed environments

!!! info "A Note on Performance Metrics"

    The runtime and memory overhead figures cited in the table reflect the fundamental
    architectural costs of each library's design and are based on community micro-benchmarks
    comparing basic unit-wrapped arithmetic against equivalent raw primitive operations:

    - **Compiled zero-cost languages (C++, Rust, F#)**: "None" claims are verified via
      assembly/IL inspection (e.g., Compiler Explorer), confirming complete compile-time
      erasure of unit metadata — the machine code is identical to raw `double` or `f64`.
    - **Managed/JIT environments (Java, C#, Julia)**: Overhead factors (e.g., ~650× for
      JSR-385) reflect formal micro-benchmarking (JMH, BenchmarkDotNet). These capture the
      cost of heap allocation, GC pressure, and dynamic dispatch when type structures cannot
      be fully flattened at compile time. Julia achieves zero cost only when type-stable.
    - **Dynamic languages (Python)**: Multipliers for Pint are from standard profiling
      (`timeit`) and documented community benchmarks, highlighting the object-instantiation
      and dictionary-lookup cost of tracking dimensions at runtime.

    Actual production overhead will vary with application architecture, JIT warmup, and
    object reuse strategies, but the multipliers accurately reflect each paradigm's
    fundamental characteristics.

## Conclusion

Safety in quantities and units libraries exists on a spectrum, from basic dimensional
analysis to sophisticated semantic correctness. **mp-units** is the only such library
currently providing all six safety levels (with full quantity character support planned
for V3), making it the most complete implementation of metrologically sound,
strongly-typed numerics available in the world today.

!!! tip "Choosing a safety level in mp-units"

    [**Simple quantities**](../../users_guide/framework_basics/simple_and_typed_quantities.md#simple-quantities)
    (`quantity<unit, Rep>`) are the natural starting point and already cover Levels 1–4 with
    no extra effort. From there, two independent opt-in choices extend coverage further:

    - **`quantity_point`** brings Level 6 (mathematical space safety) for domains involving
      affine spaces — time instants, temperatures, positions — at no additional code overhead.
      If adding two quantities of the same kind makes no physical or domain sense — as with
      two timestamps or two absolute temperatures — they should be modeled as points.
    - [**Typed quantities**](../../users_guide/framework_basics/simple_and_typed_quantities.md#typed-quantities)
      (`quantity<isq::quantity[unit], Rep>`) add Level 5: full ISQ quantity hierarchy
      enforcement (e.g., `isq::height` vs `isq::width` vs `isq::distance`). The tradeoffs are:

        - more verbose code at the call site, though more expressive and self-documenting,
        - longer type names in compiler diagnostics and during debugging.

      Use them where multiple distinct quantities of the same kind are in play and that
      distinction matters. mp-units V3 further extends typed quantities with quantity character
      safety (scalar, vector, tensor).


## References

- [mp-units Documentation](https://mpusz.github.io/mp-units/)
- [Safety Features Getting Started](../../getting_started/safety_features.md)
- [Bringing Quantity-Safety To The Next Level](bringing-quantity-safety-to-the-next-level.md)
- [Introducing Absolute Quantities](introducing-absolute-quantities.md)
- [ISO 80000 (ISQ) Series](isq-part-1-introduction.md)
