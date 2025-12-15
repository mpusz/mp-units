# Cheat Sheet

This page provides a concise overview of the most common **mp-units** operations and
patterns. For detailed explanations, refer to the
[Users Guide](../users_guide/framework_basics/interface_introduction.md).

## Setup

- **CMake:** `mp-units::mp-units`
- **Conan:** `mp-units`

=== "C++ modules"

    ```cpp
    import mp_units;

    using namespace mp_units;
    using namespace mp_units::si::unit_symbols; // (1)!
    ```

     1. The short unit symbols (like `m`, `kg`, `s`, etc.) are not available by
         default. These short identifiers are too common and may conflict with other
         code or libraries. This is why they are provided by the
         `mp_units::si::unit_symbols` namespace, which is opt-in.

=== "Header files"

    ```cpp
    #include <mp-units/core.h>
    #include <mp-units/systems/si.h>

    using namespace mp_units;
    using namespace mp_units::si::unit_symbols; // (1)!
    ```

     1. The short unit symbols (like `m`, `kg`, `s`, etc.) are not available by
         default. These short identifiers are too common and may conflict with other
         code or libraries. This is why they are provided by the
         `mp_units::si::unit_symbols` namespace, which is opt-in.


## Quantities

### Simple vs Typed Quantities

**mp-units** supports two modes for creating quantities that can be freely mixed:

=== "Simple Quantities"

    Focus on **units** and **quantity kinds**. Best for straightforward calculations:

    ```cpp
    quantity speed = 60 * km / h;                        // Unit-based (simple)
    quantity length = 10 * m;                            // Simple length
    ```

    - **Pros**: Concise, easier to read, shorter compilation errors
    - **Use when**: Working with quantities of different kinds or when type safety isn't critical

=== "Typed Quantities"

    Add **ISQ quantity types** for stronger type safety:

    ```cpp
    quantity speed = isq::speed(60 * km / h);            // Typed with ISQ quantity
    quantity height = isq::height(2 * m);                // Different from isq::width or isq::radius
    quantity width = isq::width(3 * m);                  // Can't be mixed with height
    ```

    - **Pros**: Prevents accidental mixing of quantities of the same kind (_height_ vs _width_)
    - **Use when**: Dealing with multiple quantities of the same kind that shouldn't be interchangeable

!!! tip "When to use typed quantities"

    Use typed quantities when you need to distinguish between quantities of the same kind
    (e.g., _height_ vs _width_ vs _radius_). Otherwise, simple quantities are more concise.

### Creating Quantities

The value of the quantity is the product of the **number** and the **unit**:

=== "Simple Quantities"

    ```cpp
    // Basic quantities
    quantity time = 20 * min;
    quantity length = 42.f * m;
    quantity mass = 2.5 * kg;
    quantity temperature = delta<deg_C>(3);  // (1)!

    // Derived quantities
    quantity area = 10 * m * 12 * m;
    quantity speed = 100 * km / (2 * h);  // (2)!
    quantity energy = 0.5 * mass * pow<2>(speed); // (3)!
    quantity power = energy / time;
    ```

    1. Temperature is an exception. Its quantities can't be created with the multiply syntax.
        Use `delta<deg_C>(...)` for temperature differences (relative quantities), and
        `point<deg_C>(...)` for absolute temperatures. See the
        [Temperature support](../users_guide/framework_basics/the_affine_space.md#temperature-support)
        in the User's Guide for details.
    2. Parenthesis is required here to result with a proper dimension for a _speed_ quantity.
    3. Exponent must be known at compile time to result with a proper quantity type
        (contrarily to `std::pow`).

=== "Typed Quantities"

    ```cpp
    // Basic ISQ quantities
    quantity distance = isq::distance(110 * km);
    quantity duration = isq::time(2 * h);
    quantity height = isq::height(10 * m);
    quantity width = 5 * isq::width[m];

    // Derived ISQ quantities
    quantity speed = isq::speed(distance / duration);
    quantity<isq::area[m2]> area = height * width;     // convertible to isq::area
    ```

### Quantity Arithmetic

Quantity arithmetic affects not only the final value but also its **type**:

```cpp
// Basic operations
quantity total_length = 10 * m + 5 * m;               // 15 m
quantity length_diff = 10 * m - 3 * m;                // 7 m
quantity area = 5 * m * 4 * m;                        // 20 m²
quantity speed = (100 * km) / (2 * h);                // 50 km/h

// Mixed unit arithmetic
quantity total_distance = 1 * km + 500 * m;           // 1500 m
quantity total_time = 1 * h + 30 * min;               // 90 min

// Powers and roots
quantity volume = pow<3>(2 * m);                      // 8 m³
quantity side_length = cbrt(27 * m3);                 // 3 m
quantity rms_speed = sqrt(pow<2>(speed));             // Speed magnitude
```

### ISQ: Quantity Hierarchies and Kinds

The International System of Quantities (ISQ) defines **hierarchies of quantities** of
the same kind, enabling sophisticated type safety:

```cpp
// Quantities of the same kind (length) form a hierarchy
quantity w = isq::width(2 * m);
quantity h = isq::height(3 * m);
quantity l = isq::length(5 * m);                             // Root of the hierarchy

// Implicit conversions UP the hierarchy (specific → general)
quantity<isq::length[m]> len1 = w;                           // ✓ width is a length
quantity<isq::length[m]> len2 = h;                           // ✓ height is a length

// Explicit conversions DOWN the hierarchy (general → specific)
quantity<isq::width[m]> w2 = isq::width(l);                  // ✓ Explicit conversion
// quantity<isq::width[m]> w3 = l;                           // ✗ Error: no implicit conversion

// Explicit casts for sibling quantities (same level)
quantity<isq::height[m]> h2 = quantity_cast<isq::height>(w); // ✓ Forces conversion
// quantity<isq::height[m]> h3 = isq::height(w);             // ✗ Error: can't convert siblings

// Operations result in common quantity type
quantity sum = w + h;                                        // Result: isq::length (common parent)
```

!!! note "Preventing incorrect operations"

    ISQ prevents operations that don't make physical sense:

    ```cpp
    // ✗ Can't add quantities of different kinds (even if same dimension)
    // auto invalid = 1 * Hz + 1 * Bq;               // Error: frequency ≠ activity
    // auto wrong = area + fuel_consumption;         // Error: different kinds
    ```

### ISQ: Generic Interfaces with `QuantityOf`

Use `QuantityOf` concepts to create safe, generic functions that accept any quantity that
is convertible to a provided type:

```cpp
// Generic function accepting any speed quantity in any unit
constexpr QuantityOf<isq::speed> auto avg_speed(QuantityOf<isq::length> auto distance, // (1)!
                                                QuantityOf<isq::time> auto time)
{
  return distance / time;
}

quantity v1 = avg_speed(220 * km, 2 * h);                    // 110 km/h
quantity v2 = avg_speed(isq::length(100 * mi), 2 * h);       // 50 mi/h
quantity v3 = avg_speed(isq::distance(1500 * m), 50 * s);    // 30 m/s
// quantity v4 = avg_speed(50 * s, 1500 * m);                // ✗ Error
```

1. `QuantityOf<isq::speed>` not only documents what the function returns, but also serves
   as a compile-time unit test verifying that the quantity equation implemented in
   the function returns a correct quantity type.


### ISQ: Defining Custom Quantities

Create your own quantity types to extend ISQ hierarchies:

```cpp
// Define a custom quantity in the length hierarchy
inline constexpr struct horizontal_length final :
    quantity_spec<isq::length> {} horizontal_length;

// Define a derived quantity with a constrained equation
inline constexpr struct horizontal_area final :
    quantity_spec<isq::area, horizontal_length * isq::width> {} horizontal_area;

// Use custom quantities for stronger type safety
quantity base_length = horizontal_length(100 * m);
quantity base_width = isq::width(50 * m);
quantity<horizontal_area[m2]> base_area = base_length * base_width;        // ✓ OK
// quantity<horizontal_area[m2]> wrong = isq::height(3 * m) * base_width;  // ✗ Error
```

### Quantity Points

```cpp
// Time points (absolute time)
inline constexpr struct epoch final : absolute_point_origin<kind_of<isq::time>> {} epoch;
quantity_point later = epoch + 3600 * s;                 // 1 hour later
quantity duration = later - epoch;                       // 3600 s

// Temperature points (absolute temperatures)
quantity_point temp_point = point<deg_C>(20);            // Absolute temperature
quantity_point temp_kelvin = temp_point.in(K);           // 293.15 K

// Temperature differences (relative quantities)
quantity temp_diff = delta<K>(10);                       // Temperature difference
quantity_point new_temp = temp_point + temp_diff;        // 30 °C
```

### Dimensionless Quantities

```cpp
// Counts, ratios, and efficiencies
quantity count = 42 * one;                               // 42
quantity amount = 23 * percent;                          // 23 %
quantity efficiency = 850. * W / (1000. * W);            // 0.85
quantity ratio = (5. * km) / (2. * m);                   // 2.5 km/m

// Angular quantities
quantity angle_deg = 90. * deg;                          // Angle in degrees
quantity angle_rad = angle_deg.in(rad);                  // π/2 rad
quantity sin_value = si::sin(angle_rad);                 // 1.0
```


## Units

### Defining Units

```cpp
inline constexpr struct second final : named_unit<"s", kind_of<isq::time>> {} second;
inline constexpr struct metre final : named_unit<"m", kind_of<isq::length>> {} metre;
inline constexpr struct gram final : named_unit<"g", kind_of<isq::mass>> {} gram;
inline constexpr auto kilogram = si::kilo<gram>;
inline constexpr struct newton final : named_unit<"N", kilogram * metre / square(second)> {} newton;
inline constexpr struct bar final : named_unit<"bar", mag<100> * kPa> {} bar;
inline constexpr struct yard final : named_unit<"yd", mag_ratio<9'144, 10'000> * si::metre> {} yard;
inline constexpr struct dalton final :
    named_unit<"Da", mag_ratio<16'605'390'666'050, 10'000'000'000'000> * mag_power<10, -27> * kg> {} dalton;
```

### Working with Units

```cpp
// "importing" specific unit symbols
using mp_units::si::unit_symbols::kg;

// User-defined aliases for SI units
constexpr Unit auto m = si::metre;
constexpr Unit auto km = si::kilo<si::metre>;
constexpr Unit auto h = non_si::hour; // (1)!

// Derived units
constexpr Unit auto m2 = square(m);
constexpr Unit auto m3 = cubic(si::metre);
constexpr Unit auto mps = m / s;

// Constants as a unit
constexpr Unit auto two_pi = mag<2> * mag<pi> * one;
```

1. Non-SI unit accepted for use with SI

### Unit Conversions

```cpp
// Explicit conversions
quantity distance = (5 * km).in(m);                                  // 5000 m
quantity speed = (60 * km / h).force_in(m / s);                      // 16 m/s (1)
quantity temp = delta<deg_C>(3).in<double>(deg_F);                   // 5.4 ℉
quantity pressure = (101325. * Pa).in(bar);                          // 1.01325 bar

// Value extraction
int distance_value = (42 * m).numerical_value_in(m);                 // 42
double speed_value = (100 * km / h).force_numerical_value_in(m / s); // 27 (2)
```

1. Potential value truncation.
2. Potential value truncation.

## Output & Formatting

=== "C++20"

    ```cpp
    #include <format>
    #include <iostream>

    // Basic formatting
    std::cout << std::format("{}\n",                42 * m);      // 42 m
    std::cout << std::format("{:%N in %U of %D}\n", 42 * m);      // 42 in m of L
    std::cout << std::format("{}\n",         3.14159 * m);        // 3.14159 m
    std::cout << std::format("{::N[.2f]}\n", 3.14159 * m);        // 3.14 m

    // Unit symbols
    std::cout << std::format("{}\n",       42 * kΩ);              // 42 kΩ
    std::cout << std::format("{::U[P]}\n", 42 * kΩ);              // 42 kohm
    std::cout << std::format("{:%U}\n",    42 * kΩ);              // kΩ

    // Derived units
    std::cout << std::format("{}\n",       9.81 * m / s2);        // 9.81 m/s²
    std::cout << std::format("{::U[P]}\n", 9.81 * m / s2);        // 9.81 m/s^2
    std::cout << std::format("{::U[n]}\n", 9.81 * m / s2);        // 9.81 m s⁻²
    ```

=== "C++23"

    ```cpp
    #include <print>


    // Basic formatting
    std::println("{}",                42 * m);                    // 42 m
    std::println("{:%N in %U of %D}", 42 * m);                    // 42 in m of L
    std::println("{}",         3.14159 * m);                      // 3.14159 m
    std::println("{::N[.2f]}", 3.14159 * m);                      // 3.14 m

    // Unit symbols
    std::println("{}",       42 * kΩ);                            // 42 kΩ
    std::println("{::U[P]}", 42 * kΩ);                            // 42 kohm
    std::println("{:%U}",    42 * kΩ);                            // kΩ

    // Derived units
    std::println("{}",       9.81 * m / s2);                      // 9.81 m/s²
    std::println("{::U[P]}", 9.81 * m / s2);                      // 9.81 m/s^2
    std::println("{::U[n]}", 9.81 * m / s2);                      // 9.81 m s⁻²
    ```

## Compile-Time Safety

=== "Representation Safety"

    Prevents lossy conversions that would truncate or lose precision:

    ```cpp
    quantity distance = (5 * km).in(m);                       // ✓ 5000 m (widening)
    quantity height = (186. * cm).in(m);                      // ✓ 1.86 m (floating-point)
    quantity length = (short{2} * m).in<int>(mm);             // ✓ 2000 mm (explicit)
    quantity duration = (90 * min).force_in(h);               // ✓ 1 h (forced, may truncate)

    // quantity wrong = (42 * m).in(km);                      // ✗ Error: truncating int conversion
    // quantity bad = (1.5 * m).in<int>();                    // ✗ Error: double → int needs explicit cast
    // quantity<si::metre, int> invalid = 1.5 * m;            // ✗ Error: double → int needs explicit cast
    ```

=== "Affine Space Safety"

    Distinguishes absolute points from relative differences (vectors):

    ```cpp
    quantity temp_diff = delta<deg_C>(3) + delta<deg_C>(2);   // ✓ 5 °C (diff + diff = diff)
    quantity_point temp = point<deg_C>(20) + delta<deg_C>(3); // ✓ 23 °C (point + diff = point)
    quantity diff = point<deg_C>(25) - point<deg_C>(20);      // ✓ 5 °C (point - point = diff)

    // auto illegal = point<deg_C>(18) + point<deg_C>(10);    // ✗ Error: can't add two points
    // auto wrong = delta<deg_C>(5) - point<deg_C>(20);       // ✗ Error: can't subtract point from diff
    ```

=== "Dimension Safety"

    Ensures operations are dimensionally consistent:

    ```cpp
    quantity energy = 0.5 * mass * pow<2>(speed);          // ✓ kg⋅m²/s² (energy dimension)
    quantity force = mass * si::standard_gravity;          // ✓ kg⋅m/s² (force dimension)
    quantity sum = 100 * m + 50 * m;                       // ✓ 150 m (same dimension)

    // quantity<si::metre> invalid = 2 * kg;               // ✗ Error: mass ≠ length
    // quantity wrong = 10 * m + 5 * s;                    // ✗ Error: length + time
    // quantity bad = si::sin(42 * m);                     // ✗ Error: sin requires dimensionless angular quantity
    ```

=== "Quantity Kind Safety"

    Prevents mixing quantities of different kinds even with the same dimension:

    ```cpp
    quantity angle1 = 2 * rad + 4 * rad;                   // ✓ 6 rad (plane angle)
    quantity solid1 = 3 * sr + 5 * sr;                     // ✓ 8 sr (solid angle)
    quantity freq = 50 * Hz + 10 * Hz;                     // ✓ 60 Hz (frequency)
    quantity activity = 100 * Bq + 50 * Bq;                // ✓ 150 Bq (radioactive activity)

    // quantity wrong1 = 1 * rad + 1 * sr;                 // ✗ Error: plane angle ≠ solid angle
    // quantity wrong2 = 1 * Hz + 1 * Bq;                  // ✗ Error: frequency ≠ activity
    ```

=== "Quantity Type Safety"

    Distinguishes between different quantities of the same kind using ISQ:

    ```cpp
    // Different quantities of the same kind (length)
    quantity width = isq::width(2 * m);
    quantity height = isq::height(3 * m);

    // ✓ Implicit conversions to common parent (specific → general)
    quantity<isq::length[m]> length = width;                  // ✓ width is a length

    // ✓ Operations find common quantity type
    quantity sum = width + height;                            // ✓ Result: isq::length
    quantity<isq::area[m2]> area = width * height;            // ✓ width × height → area
    quantity<isq::volume[m3]> vol = area * height;            // ✓ area × height → volume

    // Different quantities of the same kind (energy)
    quantity Ek = isq::kinetic_energy(250 * J);               // ✓ Kinetic energy
    quantity Ep = isq::potential_energy(500 * J);             // ✓ Potential energy
    quantity<isq::mechanical_energy[J]> Em = Ek + Ep;         // ✓ Result: mechanical energy

    // ✗ No implicit conversions between siblings or downward
    // quantity<isq::width[m]> width2 = height;               // ✗ Error: height → width (siblings)
    // quantity<isq::height[m]> height2 = length;             // ✗ Error: length → height (general → specific)
    // quantity<isq::kinetic_energy[J]> Ek2 = Ep;             // ✗ Error: potential → kinetic (siblings)

    // Use explicit conversion when you know it's safe
    quantity q = quantity_cast<isq::height>(width);           // ✓ Explicit cast
    ```

!!! note "All safety checks are compile-time errors"

    Every commented-out line marked with `✗ Error` in the examples above will produce
    a **compile-time error** if uncommented. The library catches these mistakes during
    compilation, not at runtime.


## Learn More

- **Getting Started**: [Introduction](introduction.md) and [Installation](installation_and_usage.md)
- **Simple vs Typed**: [Simple and Typed Quantities](../users_guide/framework_basics/simple_and_typed_quantities.md)
- **ISQ Deep Dive**: [ISQ Blog Series](../blog/posts/isq-part-1-introduction.md) - Understanding quantity hierarchies and kinds
- **Examples**: [Hello Units](../examples/hello_units.md) and [Average Speed](../examples/avg_speed.md)
- **FAQ**: [Frequently Asked Questions](faq.md)
