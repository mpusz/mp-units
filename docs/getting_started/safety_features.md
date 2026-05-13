# Safety Features

One of the primary reasons to use a physical quantities and units library is to **prevent
errors at compile time**. **mp-units** provides multiple layers of safety, each addressing
different aspects of dimensional analysis and type safety.

!!! tip

    Safety features are not just about preventing bugs—they're about encoding domain knowledge
    directly into the type system, making incorrect code literally impossible to write.

!!! info "Deep Dive Available"

    This page provides a quick introduction to safety levels in **mp-units**. For in-depth
    analysis, library comparisons, and the rationale behind each level, see the blog post
    [Understanding Safety Levels in Physical Units Libraries](../blog/posts/understanding-safety-levels.md).


## Overview of Safety Levels

A comprehensive physical quantities and units library can provide six distinct safety levels:

1. **Dimension Safety** - Prevents mixing incompatible dimensions
2. **Unit Safety** - Prevents unit mismatches and eliminates manual scaling factors
3. **Representation Safety** - Protects against overflows and precision loss
4. **Quantity Kind Safety** - Prevents arithmetic on quantities of different kinds
5. **Quantity Safety** - Enforces correct quantity relationships and equations
6. **Mathematical Space Safety** - Distinguishes points and deltas

**mp-units** provides all six levels. The following sections introduce each level
with simple examples.


## Level 1: Dimension Safety

**Dimension safety** prevents mixing quantities with incompatible dimensions through
automatic dimensional analysis.

```cpp
quantity<si::metre / si::second> speed = 100 * km / h;  // ✅ OK: km/h is a unit of speed (same as m/s)
quantity<si::second> time = 2 * h;                      // ✅ OK: hour is a unit of time (same as second)
// quantity<si::metre> distance = 2 * h;                // ❌ Compile-time error!
// quantity<si::metre> distance = speed / time;         // ❌ Compile-time error!
quantity<si::metre> distance = speed * time;            // ✅ OK: length

// auto result = distance + time;                       // ❌ Compile-time error!
// Error: cannot add length and time (incompatible dimensions)
```

**All major C++ units libraries provide dimension safety.** This is the foundational
feature that enables dimensional analysis.


## Level 2: Unit Safety

**Unit safety** ensures compatible units at interface boundaries—function arguments, return
types, and component integration.

```cpp
// Function accepts any length and time units
quantity<si::metre / si::second> avg_speed(quantity<si::metre> d, quantity<si::second> t)
{
  return d / t;
}

quantity distance = 220 * km;
quantity time = 2 * h;
quantity<km / h> speed = avg_speed(distance, time);  // ✅ 110 km/h - automatic conversion
```

Common errors for solutions without unit safety (using raw `double` values) include:

- **Function arguments**: Passing raw value in meters when kilometers are expected
- **Return types**: Misinterpreting what unit a returned raw value represents
- **Component integration**: Mixing raw values with different implicit unit assumptions
- **Value extraction**: Extracting raw numbers without specifying or verifying the unit
- **Refactoring**: Changing function's expected unit without updating all call sites
- **Manual scaling**: Error-prone manual conversion factors (e.g., forgetting to multiply
  by 1000 when converting km to m)

Unit safety eliminates these issues through automatic unit conversions and type-safe interfaces.

**mp-units provides stronger unit safety than most libraries**, especially in value
extraction. While `std::chrono::duration::count()` returns raw numbers without verification,
`mp-units::numerical_value_in(Unit)` requires specifying the exact unit. For more details,
see [Working with Legacy Interfaces](../how_to_guides/integration/working_with_legacy_interfaces.md).


## Level 3: Representation Safety

**Representation safety** protects against numerical issues like overflow, underflow, and
precision loss during conversions and arithmetic operations.

```cpp
// Truncation prevention (inspired by std::chrono::duration)
quantity length = 1500 * m;
// quantity<km, int> distance = length;             // ❌ Compile-time error!
// Error: 1500 m → 1.5 km truncates with int

quantity distance2 = length.force_in(km);           // ✅ OK: explicit truncation (returns quantity)
auto km_count = length.force_numerical_value_in(km);// ✅ OK: explicit truncation (returns raw int: 1)
quantity<mm, int> length_mm = length;               // ✅ OK: 1'500'000 mm (no truncation)

// Scaling overflow detection
quantity length1 = std::int8_t{2} * m;
// quantity<mm, std::int8_t> length_mm1 = length1;  // ❌ Compile-time error!
// Error: 2 m → 2000 mm overflows int8_t (max 127)

quantity length2 = std::int16_t{2} * m;
quantity<mm, std::int16_t> length_mm2 = length2;    // ✅ OK: 2000 fits in int16_t

// Unit-qualified construction: raw integers cannot construct a quantity
// std::chrono allows: delays.emplace_back(42) — silently 1000× wrong if type changes
std::vector<quantity<si::milli<si::second>>> delays;
// delays.emplace_back(42);    // ❌ Compile-time error — unit required!
delays.emplace_back(42 * ms);  // ✅ Explicit unit — safe regardless of container type
delays.emplace_back(42, ms);   // ✅ OK
```

**mp-units** provides strong representation safety through truncation prevention (inspired
by `std::chrono::duration`), compile-time scaling overflow detection, and mandatory
unit-qualified construction.

### Runtime Safety Infrastructure

No library can prevent runtime arithmetic overflow at compile time (e.g., `quantity * 2`).
**mp-units** provides built-in tools for domains that need guaranteed runtime enforcement:

```cpp
#include <mp-units/safe_int.h>
#include <mp-units/constrained.h>

// Built-in safe_int detects arithmetic overflow at runtime
quantity distance = safe_int{std::int16_t{100}} * m;
// quantity overflow = distance * std::int16_t{1'000};  // throws std::overflow_error at runtime

// constrained<T> tags a type with an error policy for bounds checking
using safe_double = constrained<double, throw_policy>;
using latitude = quantity_point<geo_latitude[deg], equator, safe_double>;
latitude lat{95.0 * deg, equator};  // throws std::domain_error (out of [-90, 90])
```

!!! tip "Deep Dive"

    For complete coverage, see:

    - [`safe_int<T>`](../users_guide/framework_basics/safe_int.md) —
      overflow-safe integer arithmetic reference
    - [Preventing Integer Overflow in Physical Computations](../blog/posts/preventing-integer-overflow.md) —
      in-depth narrative on automatic scaling overflow and design tradeoffs
    - [Ensure Ultimate Safety](../how_to_guides/advanced_usage/ultimate_safety.md) —
      combining `safe_int`, `constrained`, and origin bounds policies
    - [Representation Types: `constraint_violation_handler`](../users_guide/framework_basics/representation_types.md#constraint-violation-handler) —
      customization point reference


### Non-Negative Quantities

The library also tracks which physical quantities are inherently non-negative (e.g.,
_length_, _mass_, _duration_) through the type system. This property propagates through
derived equations and is automatically inherited by named real-scalar children:

```cpp
static_assert(is_non_negative(isq::length));      // ✅ Tagged in ISQ definitions
static_assert(is_non_negative(isq::mass));        // ✅ Tagged in ISQ definitions
static_assert(is_non_negative(isq::distance));    // ✅ Named real-scalar child of length
static_assert(is_non_negative(isq::speed));       // ✅ Derived: length / duration
static_assert(!is_non_negative(isq::velocity));   // ❌ Vector character — excluded from inheritance
```

!!! question "How is non-negativity enforced?"

    Non-negativity is enforced at **two levels**:

    **Compile time (always)**
    :   The `non_negative` flag is tracked in the type system and propagated through
        dimensional equations, so the compiler knows whether any derived quantity is
        non-negative.

    **Runtime (automatic for `quantity_point` with a natural origin)**
    :   Every `quantity_point` whose reference is rooted at the natural point origin of a
        non-negative quantity spec automatically has `check_non_negative` bounds attached.
        This means construction of, or arithmetic on, such a point checks the value:

        - If the representation type has a
          [`constraint_violation_handler`](../users_guide/framework_basics/representation_types.md#constraint-violation-handler)
          specialization (e.g., a `constrained<T, Policy>` rep), the handler is invoked on
          violation — behaviour is guaranteed regardless of build mode.
        - For plain types (like `double`) the library falls back to
          [`MP_UNITS_EXPECTS`](../how_to_guides/integration/wide_compatibility.md#contract-checking-macros),
          which may be disabled in release builds.

        You can override the default policy for a specific quantity spec by defining a
        custom origin with different bounds:

        ```cpp
        // Replace error-on-negative with silent clamp-to-zero (e.g., for FP noise):
        inline constexpr struct clamped_length_origin :
            absolute_point_origin<isq::length, clamp_non_negative{}> {} clamped_length_origin;
        ```

        See [Tutorial: Custom Contract Handlers](../tutorials/affine_space/custom_contract_handlers.md)
        for a step-by-step guide to implementing custom error policies with `constrained<T, ErrorPolicy>`.

    For quantities modeled only as a `quantity` displacement (not as a `quantity_point`),
    non-negativity remains a compile-time property only.


## Level 4: Quantity Kind Safety

**Quantity kind safety** distinguishes between quantities that share the same dimension but
represent different physical concepts.

```cpp
quantity absorbed_dose = 1.5 * Gy;
quantity dose_equivalent = 2.0 * Sv;

// auto result = absorbed_dose + dose_equivalent;           // ❌ Compile-time error!
// Error: cannot add absorbed dose and dose equivalent (both L²T⁻², but different kinds)

// auto equal = (absorbed_dose == dose_equivalent);          // ❌ Compile-time error!
// Error: cannot compare absorbed dose and dose equivalent
```

Examples of quantities with same dimension but different kinds:

- **Absorbed dose (Gy)** and **Dose equivalent (Sv)**: Both `length²/time²`
- **Frequency (Hz)** and **Activity (Bq)**: Both `1/time`
- **Plane angle (rad)** and **Solid angle (sr)**: Both dimensionless
- **Area (m²)** and **Fuel consumption (L/100km)**: Both `length²` (dimension L²)

!!! important

    **mp-units is the only C++ library implementing quantity kind safety.** It fully
    distinguishes all SI quantity kinds including Gy/Sv, Hz/Bq, and rad/sr.


## Level 5: Quantity Safety

**Quantity safety** is the highest level, providing semantic correctness through:

1. **Quantity Type Correctness** - Hierarchies, conversions, and quantity equation
  ingredients validation
2. **Quantity Character Correctness** - Representation types and character-specific operations

```cpp
// Quantity Type: Hierarchy prevents mixing energy types
void process_kinetic(quantity<isq::kinetic_energy[J]> ke) { /* ... */ }

// Quantity Type: Ingredient validation requires specific quantity types
constexpr quantity g0 = isq::acceleration_of_free_fall(1 * si::standard_gravity);
quantity<isq::height[m]> height = 5 * m;
quantity<gravitational_potential_energy[J]> Ep = mass * g0 * height;  
// quantity<gravitational_potential_energy[J]> wrong = mass * g0 * width;  // ❌ Compile-time error!
// Error: cannot form gravitational potential energy from width

// process_kinetic(pe);                                                   // ❌ Compile-time error!
// Error: cannot pass potential_energy where kinetic_energy is required

// Quantity Character: Vector vs. scalar distinction
quantity<isq::speed[m/s]> speed = cartesian_vector{1, 2, 3} * m / s;      // ❌ Compile-time error!
// Error: cannot initialize scalar quantity (speed) with vector representation
```

Quantity safety distinguishes:

- **_Length_ specializations**: _Height_, _width_, _radius_ (all _lengths_, but specific
  uses in equations)
- **Vector vs. Scalar**: _Velocity_ (vector) vs. _speed_ (scalar magnitude)
- **_Energy_ types**: _Kinetic_, _potential_, _thermal_, _enthalpy_ (same dimension,
  different meanings)
- **_Power_ types**: _Active_ (W), _reactive_ (var), _apparent_ (VA) _power_ in AC circuits

**mp-units** is the only C++ library providing quantity safety through ISO 80000 (ISQ)
implementation. Full character-specific operations planned for V3.


## Level 6: Mathematical Space Safety

**Mathematical space safety** distinguishes between **quantity points** (absolute positions
on a scale) and **quantity deltas** (differences/displacements).

```cpp
// Points: Positions on a scale with an origin
quantity_point room_temp = point<deg_C>(20.);
quantity_point outside_temp = point<deg_C>(5.);

quantity temp_diff = room_temp - outside_temp;      // ✅ OK: 15 K (delta)
// auto temp_sum = room_temp + outside_temp;        // ❌ Compile-time error!
// Error: cannot add points (meaningless: what is 20 °C + 5 °C?)

// Deltas: Differences between values
quantity temp_change = delta<K>(10);
quantity_point new_temp = room_temp + temp_change;  // ✅ OK: point + delta = 30 °C
quantity total_change = temp_change + temp_change;  // ✅ OK: delta + delta = 20 K

// auto wrong = temp_change - room_temp;            // ❌ Compile-time error!
// Error: cannot subtract point from delta (meaningless: what is 10 K - 20 °C?)
```

Key operations:

- **Point - Point** → Delta (temperature difference)
- **Point + Delta** → Point (apply change to position)
- **Delta + Delta** → Delta (combine changes)
- **Point + Point** → ❌ Error (meaningless operation)
- **Delta - Point** → ❌ Error (meaningless operation)

Examples where mathematical space safety prevents errors:

- **Temperature**: Cannot add 20 °C + 10 °C (meaningless), but can compute difference (10 K)
- **Time**: Cannot add two _timestamps_, but can subtract them to get _duration_
- **Position**: Cannot add two GPS coordinates, but can compute displacement between them
- **Altitude**: Cannot add two _altitudes_ above sea level, but can compute _height_ difference

**mp-units** implements mathematical space safety using `quantity_point` for positions and
`quantity` for differences.


## Choosing a Safety Level

[**Simple quantities**](../users_guide/framework_basics/simple_and_typed_quantities.md#simple-quantities)
(`quantity<unit, Rep>`) are the natural starting point and already cover Levels 1–4 with
no extra effort. From there, two independent opt-in choices extend coverage further:

- **`quantity_point`** brings Level 6 (mathematical space safety) for domains involving
  affine spaces — _time_ instants, _temperatures_, _positions_ — at no additional code overhead.
  If adding two quantities of the same kind makes no physical or domain sense — as with
  two _timestamps_ or two absolute _temperatures_ — they should be modeled as points.
  Additionally, quantity points support
  [range validation](../users_guide/framework_basics/the_affine_space.md#range-validated-quantity-points):
  bounds policies passed as template parameters to point origins (such as `check_in_range`,
  `clamp_to_range`, `wrap_to_range`, `reflect_in_range`) enforce
  valid ranges for geographic coordinates, sensor operating limits, and similar domains.
- [**Typed quantities**](../users_guide/framework_basics/simple_and_typed_quantities.md#typed-quantities)
  (`quantity<isq::quantity[unit], Rep>`) add Level 5: full ISQ quantity hierarchy
  enforcement (e.g., `isq::height` vs `isq::width` vs `isq::distance`). The tradeoffs are:

    - more verbose code at the call site, though more expressive and self-documenting,
    - longer type names in compiler diagnostics and during debugging.

  Use them where multiple distinct quantities of the same kind are in play and that
  distinction matters.


## Learn More

To dive deeper into **mp-units** safety features:

- [Understanding Safety Levels (Blog)](../blog/posts/understanding-safety-levels.md) -
  In-depth analysis, library comparisons, and why safety matters for C++ standardization
- [Simple and Typed Quantities](../users_guide/framework_basics/simple_and_typed_quantities.md) -
Choose your level of type safety
- [Representation Types](../users_guide/framework_basics/representation_types.md) -
  Customization points including `constraint_violation_handler`
- [Range-Validated Quantity Points](../users_guide/framework_basics/the_affine_space.md#range-validated-quantity-points) -
  Bounded quantity points with overflow policies
- [Ensure Ultimate Safety](../how_to_guides/advanced_usage/ultimate_safety.md) -
  Combining `constrained` reps, `safe_int`, and bounded quantity points
- [Systems of Quantities](../users_guide/framework_basics/systems_of_quantities.md) -
  Understanding the ISQ hierarchy
- [Character of a Quantity](../users_guide/framework_basics/character_of_a_quantity.md) -
  Scalar, vector, and tensor quantities
- [Value Conversions](../users_guide/framework_basics/value_conversions.md) -
  Safe vs. potentially lossy conversions
- [The Affine Space](../users_guide/framework_basics/the_affine_space.md) -
  Distinguishing quantities and quantity points
- [Introducing Absolute Quantities (Blog)](../blog/posts/introducing-absolute-quantities.md) -
  V3 design for points, absolute quantities, and deltas
- [Bringing Quantity-Safety To The Next Level (Blog)](../blog/posts/bringing-quantity-safety-to-the-next-level.md) -
  Future of quantity character correctness
