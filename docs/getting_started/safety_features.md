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
6. **Affine Space Safety** - Distinguishes points and deltas

**mp-units** provides all six levels. The following sections introduce each level
with simple examples.


## Dimension Safety

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


## Unit Safety

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
see [Working with Legacy Interfaces](../../how_to_guides/integration/working_with_legacy_interfaces.md).


## Representation Safety

**Representation safety** protects against numerical issues like overflow, underflow, and
precision loss during conversions and arithmetic operations.

```cpp
// Truncation prevention (inspired by std::chrono::duration)
quantity length = 1500 * m;
// quantity<km, int> distance = length;             // ❌ Compile-time error!
// Error: 1500 m → 1.5 km truncates with int

quantity distance2 = length.force_in(km);           // ✅ OK: explicit truncation
quantity<mm, int> length_mm = length;               // ✅ OK: 1'500'000 mm (no truncation)

// Scaling overflow detection
quantity length1 = std::int8_t{2} * m;
// quantity<mm, std::int8_t> length_mm1 = length1;  // ❌ Compile-time error!
// Error: 2 m → 2000 mm overflows int8_t (max 127)

quantity length2 = std::int16_t{2} * m;
quantity<mm, std::int16_t> length_mm2 = length2;    // ✅ OK: 2000 fits in int16_t
```

**mp-units** provides strong representation safety through truncation prevention (inspired
by `std::chrono::duration`) and compile-time scaling overflow detection.

!!! warning "Runtime Overflow Limitations"

    No library can prevent runtime arithmetic overflow at compile time (e.g., `quantity * 2`).
    Additionally, no library prevents overflow or underflow for floating-point types.
    For such cases, use custom representation types with runtime checks.


## Quantity Kind Safety

**Quantity kind safety** distinguishes between quantities that share the same dimension but
represent different physical concepts.

```cpp
quantity<si::radian> plane_angle = 1.5 * rad;
quantity<si::steradian> solid_angle = 2.0 * sr;

// auto result = plane_angle + solid_angle;                 // ❌ Compile-time error!
// Error: cannot add plane angle and solid angle (both dimensionless, but different kinds)

// QuantityOf<isq::angular_measure> auto alpha = 2.5 * sr;  // ❌ Compile-time error!
// Error: cannot initialize plane angle with solid angle (different quantity kinds)
```

Examples of quantities with same dimension but different kinds:

- **Frequency (Hz)** and **Activity (Bq)**: Both `1/time`
- **Absorbed dose (Gy)** and **Dose equivalent (Sv)**: Both `length²/time²`
- **Plane angle (rad)** and **Solid angle (sr)**: Both dimensionless

!!! important

    **mp-units is the only C++ library implementing quantity kind safety.** It fully
    distinguishes all SI quantity kinds including Hz/Bq, rad/sr, and Gy/Sv.


## Quantity Safety

**Quantity safety** is the highest level, providing semantic correctness through:

1. **Quantity Type Correctness** - Hierarchies, conversions, and quantity equation
  ingredients validation
2. **Quantity Character Correctness** - Representation types and character-specific operations

```cpp
// Quantity Type: Hierarchy prevents mixing energy types
void process_kinetic(quantity<isq::kinetic_energy[J]> ke) { /* ... */ }

quantity pe = isq::potential_energy(100 * J);
// process_kinetic(pe);                                                   // ❌ Compile-time error!
// Error: cannot pass potential_energy where kinetic_energy is required

// Quantity Type: Ingredient validation requires specific quantity types
quantity<isq::height[m]> h = 5 * m;
quantity<gravitational_potential_energy[J]> Ep = mass * g * height;  
// quantity<gravitational_potential_energy[J]> wrong = mass * g * width;  // ❌ Compile-time error!
// Error: cannot form gravitational potential energy from width

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

!!! tip "Gradual Adoption"

    **mp-units** allows choosing your safety level. Start with
    [simple quantities](../users_guide/framework_basics/simple_and_typed_quantities.md#simple-quantities)
    (dimension safety only) and gradually adopt
    [typed quantities](../users_guide/framework_basics/simple_and_typed_quantities.md#typed-quantities)
    (full quantity safety) as needs evolve.


## Affine Space Safety

**Affine Space safety** distinguishes between **quantity points** (absolute positions on
a scale) and **quantity deltas** (differences/displacements).

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

Examples where affine space safety prevents errors:

- **Temperature**: Cannot add 20 °C + 10 °C (meaningless), but can compute difference (10 K)
- **Time**: Cannot add two _timestamps_, but can subtract them to get _duration_
- **Position**: Cannot add two GPS coordinates, but can compute displacement between them
- **Altitude**: Cannot add two _altitudes_ above sea level, but can compute _height_ difference

**mp-units** implements affine space safety using `quantity_point` for positions and `quantity`
for differences.


## Learn More

To dive deeper into **mp-units** safety features:

- [Simple and Typed Quantities](../users_guide/framework_basics/simple_and_typed_quantities.md) -
  Choose your level of type safety
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
