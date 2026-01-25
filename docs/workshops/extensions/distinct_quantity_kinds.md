# Preventing Confusion with Distinct Kinds

Many engineering domains have quantities that share the same physical dimension but represent
fundamentally different concepts. Hydraulic engineering uses "_head_"—a measure of
**_potential energy per unit weight_** expressed as an equivalent _height_—in two
incompatible ways:

- **_Fluid head_**: _Potential energy_ normalized to the actual fluid's _density_
  (e.g., 2 m of mercury)
- **_Water head_**: _Potential energy_ normalized to water's _density_ (e.g., 27.2 m water
  equivalent)

Both express **_energy_** using _length_ dimensions, but mixing them produces physically
meaningless results—like mixing _gauge_ and _absolute pressure_ without conversion.
Traditional code using raw `double` values allows such mistakes to silently compile.
Remarkably, even units libraries from C++ and also other programming languages cannot
prevent this error—they only check dimensional compatibility, not physical meaning.

This workshop demonstrates how **mp-units** uses `is_kind` to create distinct quantity
**subkinds within an existing hierarchy**—a capability unique among units libraries worldwide.
The key insight: `is_kind` lets quantities **inherit** properties (dimension, unit) from a
parent while **isolating** them from each other. Just as the library prevents mixing
_plane angles_ and _solid angles_ (both subkinds of _dimensionless_), you can create
custom subkinds like _fluid head_ and _water head_ (both subkinds of _height_) that
cannot be accidentally mixed.


## Problem Statement

Consider a pump system design for a chemical processing plant. Engineers must verify that
the _pump capacity_ is adequate for the fluid being handled. This requires comparing:

- **System requirement**: The fluid column that must be lifted, expressed as _fluid head_
  (_energy_ normalized to the actual fluid's _density_)
- **Pump specifications**: Rated in _water head_ (_energy_ normalized to _water density_)

The relationship between _fluid head_ and _water head_ reflects _energy conservation_
with different _density_ normalizations:

$$H_{water} = H_{fluid} \cdot SG$$

$$H_{fluid} = \frac{H_{water}}{SG}$$

Where _specific gravity_ is the dimensionless ratio of _fluid density_ to _water density_.
The same _potential energy_ is expressed as a larger _height_ for lighter fluids (water)
and a smaller _height_ for denser fluids (mercury).

Here's how different approaches handle (or fail to handle) this scenario:

=== "Raw doubles"

    ```cpp
    // Traditional approach - all heights are just doubles
    double h_mercury_m = 2.0;        // Height of mercury column
    double h_pump_rating_m = 10.0;   // Pump rated for water
    double sg_mercury = 13.6;        // Specific gravity

    // Direct addition - compiles but physically wrong!
    double total_head = h_mercury_m + h_pump_rating_m;  // 12 m - WRONG!
    // This treats 2 m of mercury as if it were 2 m of water

    // Correct calculation requires manual tracking:
    double h_mercury_as_water = h_mercury_m * sg_mercury;  // 27.2 m

    // Compare system requirement vs pump capacity
    if (h_mercury_as_water > h_pump_rating_m) {
      std::cout << "Pump is undersized!\n";  // This will trigger!
    }
    ```

=== "Boost.Units (C++)"

    ```cpp
    #include <boost/units/systems/si.hpp>

    using namespace boost::units;
    using namespace boost::units::si;

    quantity<length> h_mercury = 2.0 * meters;
    quantity<length> h_pump_rating = 10.0 * meters;
    double sg_mercury = 13.6;

    // Direct addition - compiles but physically wrong!
    quantity<length> total_head = h_mercury + h_pump_rating;  // WRONG!
    // Both are lengths, so Boost.Units allows this

    // Correct calculation still requires manual tracking:
    quantity<length> h_mercury_as_water = h_mercury * sg_mercury;

    // Compare system requirement vs pump capacity
    if (h_mercury_as_water > h_pump_rating) {
      std::cout << "Pump is undersized!\n";  // This will trigger!
    }
    ```

    **Problem**: Boost.Units checks dimensional compatibility (both are lengths), but cannot
    distinguish between physically incompatible types of length.

=== "Pint (Python)"

    ```python
    import pint
    ureg = pint.UnitRegistry()

    h_mercury = 2.0 * ureg.meter
    h_pump_rating = 10.0 * ureg.meter
    sg_mercury = 13.6

    # Direct addition - works but physically wrong!
    total_head = h_mercury + h_pump_rating  # WRONG!
    # Both have dimension [length], so Pint allows this

    # Correct calculation still requires manual tracking:
    h_mercury_as_water = h_mercury * sg_mercury

    # Compare system requirement vs pump capacity
    if h_mercury_as_water > h_pump_rating:
        print("Pump is undersized!")  # This will trigger!
    ```

    **Problem**: Pint prevents dimensional errors but cannot distinguish between different
    physical meanings of the same dimension.

The fundamental limitation: **Units libraries check dimensional compatibility
(_length_ + _length_ = OK), but cannot enforce that quantities with the same dimension
may represent incompatible physical concepts.** This is where **mp-units** breaks new
ground with its `is_kind` feature.

**Problems common to all these approaches:**

1. **No distinction**: Both _fluid head_ and _water head_ have the same dimensional type
   (_length_), making them indistinguishable to the type system
2. **Silent errors**: Adding incompatible _head_ types compiles successfully but produces
   physically nonsense results
3. **Manual tracking**: Programmers must remember which variables represent which type
   of _head_—the type system provides no help
4. **Comparison confusion**: `2 m < 10 m` numerically, but `2 m` of mercury represents
   far more _energy_ (and _pressure_) than `10 m` of water
5. **Easy to forget**: Forgetting the SG conversion factor leads to severely undersized
   equipment—a potentially catastrophic error in chemical plants

**Real-world scenario:**

A chemical plant pump system must:

- Handle mercury (SG = 13.6) from a 2 m column in a reactor vessel
- Verify a pump rated for 10 m _water head_ can handle this load
- Convert the mercury _fluid head_ to equivalent _water head_ for comparison
- Prevent accidentally mixing _fluid head_ and _water head_ values
- Require explicit conversion through _specific gravity_

**The challenge:** Both are _heights_ (dimension: _length_), but they're physically
incompatible without conversion through _specific gravity_.


## Your task

Implement a type-safe hydraulic head calculation system using **mp-units** that prevents
mixing fluid head and water head without explicit conversion.

Create:

1. **Distinct kinds**: Define `fluid_head` and `water_head` as separate kinds derived from
  `isq::height`
2. **Specific gravity type**: Define `specific_gravity` as a dimensionless `quantity_spec`
3. **Conversion functions**: Implement type-safe conversions between the two head types:
   - `to_water_head(h_fluid, sg)` — converts _fluid head_ to _water head_ using SG
   - `to_fluid_head(h_water, sg)` — converts _water head_ to _fluid head_ using SG

The solution should:

- Prevent direct addition or comparison of _fluid head_ and _water head_ (compile-time error)
- Require explicit conversion through _specific gravity_
- Use `QuantityOf` constraints for type safety
- Work with any units of _length_ (meters, feet, etc.)

```cpp
// ce-embed height=800 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <iostream>

using namespace mp_units;

// TODO: Define fluid_head as a distinct kind derived from isq::height
// TODO: Define water_head as a distinct kind derived from isq::height
// TODO: Define specific_gravity as a dimensionless quantity_spec

// TODO: Implement to_water_head conversion function
// Formula: H_water = H_fluid * SG
// Hint: Return type should be QuantityOf<water_head> auto

// TODO: Implement to_fluid_head conversion function
// Formula: H_fluid = H_water / SG

int main()
{
  using namespace si::unit_symbols;

  // Scenario: Chemical reactor with 2m mercury column (SG = 13.6)
  quantity h_mercury = fluid_head(2 * m);
  quantity sg_mercury = specific_gravity(13.6 * one);

  // Pump rated for 10m water head
  quantity h_pump_rating = water_head(10 * m);

  std::cout << "Pump System Design Analysis\n";
  std::cout << "============================\n\n";

  std::cout << "Mercury column height: " << h_mercury << "\n";
  std::cout << "Mercury specific gravity: " << sg_mercury << "\n";
  std::cout << "Pump rating (water head): " << h_pump_rating << "\n\n";

  // Safety check: This should NOT compile!
  // quantity wrong = h_mercury + h_pump_rating;  // Error: cannot mix kinds

  // Convert mercury fluid head to equivalent water head
  quantity h_mercury_as_water = to_water_head(h_mercury, sg_mercury);

  std::cout << "Mercury equivalent (water head): " << h_mercury_as_water << "\n\n";

  // Verify pump capacity against system requirement
  if (h_mercury_as_water > h_pump_rating) {
    std::cout << "WARNING: System requirement (" << h_mercury_as_water
              << ") exceeds pump rating (" << h_pump_rating << ")!\n";
    std::cout << "Pump is UNDERSIZED for this application.\n";
  }
  else {
    quantity excess_capacity = h_pump_rating - h_mercury_as_water;
    std::cout << "Pump capacity is adequate.\n";
    std::cout << "Excess capacity: " << excess_capacity << "\n";
  }

  // Demonstrate reverse conversion
  quantity h_back_to_fluid = to_fluid_head(h_mercury_as_water, sg_mercury);
  std::cout << "\nVerification - converted back: " << h_back_to_fluid << "\n";
}
```

??? tip "Solution"

    ```cpp
    #include <mp-units/systems/si.h>
    #include <iostream>

    using namespace mp_units;

    // 1. Define the distinct kinds (The Safety Layer)
    inline constexpr struct fluid_head final : quantity_spec<isq::height, is_kind> {} fluid_head;
    inline constexpr struct water_head final : quantity_spec<isq::height, is_kind> {} water_head;

    // 2. Define a type for Specific Gravity (Dimensionless)
    inline constexpr struct specific_gravity final : quantity_spec<dimensionless> {} specific_gravity;

    // 3. Define Conversion Helpers
    // Formula: H_water = H_fluid * SG
    constexpr QuantityOf<water_head> auto to_water_head(QuantityOf<fluid_head> auto h_fluid,
                                                        QuantityOf<specific_gravity> auto sg)
    {
      // We explicitly cast the result to water_head because we know the physics is correct
      return water_head(isq::height(h_fluid) * sg);
    }

    // Formula: H_fluid = H_water / SG
    constexpr QuantityOf<fluid_head> auto to_fluid_head(QuantityOf<water_head> auto h_water,
                                                        QuantityOf<specific_gravity> auto sg)
    {
      return fluid_head(isq::height(h_water) / sg);
    }

    int main()
    {
      using namespace si::unit_symbols;

      // Scenario: Chemical reactor with 2m mercury column (SG = 13.6)
      quantity h_mercury = fluid_head(2 * m);
      quantity sg_mercury = specific_gravity(13.6 * one);

      // Pump rated for 10m water head
      quantity h_pump_rating = water_head(10 * m);

      std::cout << "Pump System Design Analysis\n";
      std::cout << "============================\n\n";

      std::cout << "Mercury column height: " << h_mercury << "\n";
      std::cout << "Mercury specific gravity: " << sg_mercury << "\n";
      std::cout << "Pump rating (water head): " << h_pump_rating << "\n\n";

      // Safety check: This would NOT compile!
      // quantity wrong = h_mercury + h_pump_rating;  // Error: cannot mix kinds

      // Convert mercury fluid head to equivalent water head
      quantity h_mercury_as_water = to_water_head(h_mercury, sg_mercury);

      std::cout << "Mercury equivalent (water head): " << h_mercury_as_water << "\n\n";

      // Verify pump capacity against system requirement
      if (h_mercury_as_water > h_pump_rating) {
        std::cout << "WARNING: System requirement (" << h_mercury_as_water
                  << ") exceeds pump rating (" << h_pump_rating << ")!\n";
        std::cout << "Pump is UNDERSIZED for this application.\n";
      }
      else {
        quantity excess_capacity = h_pump_rating - h_mercury_as_water;
        std::cout << "Pump capacity is adequate.\n";
        std::cout << "Excess capacity: " << excess_capacity << "\n";
      }

      // Demonstrate reverse conversion
      quantity h_back_to_fluid = to_fluid_head(h_mercury_as_water, sg_mercury);
      std::cout << "\nVerification - converted back: " << h_back_to_fluid << "\n";
    }
    ```

    **How the solution works:**

    By marking `fluid_head` and `water_head` with `is_kind`, we create distinct quantity types
    that cannot be mixed despite sharing the `length` dimension:

    - **Compile-time prevention**: Direct addition, comparison, or assignment between fluid head
      and water head results in a compile error

    - **Explicit conversion required**: The `to_water_head` and `to_fluid_head` functions
      perform the physics-based conversion through specific gravity, making the conversion
      visible and intentional in the code

    - **Type safety at boundaries**: Functions accepting `QuantityOf<fluid_head>` or
      `QuantityOf<water_head>` cannot accidentally receive the wrong type

    - **Base quantity access**: When needed, both can be converted to `isq::height` using
      `isq::height(h)`, allowing generic height operations while preserving type safety
      at domain boundaries

    This pattern is similar to how mp-units prevents mixing plane angles and solid angles—
    both dimensionless quantities that share the same dimension but represent fundamentally
    different physical concepts that cannot be meaningfully combined.


??? abstract "What you learned?"

    ### `is_kind` creates absolute type separation

    Unlike regular `quantity_spec` inheritance where subtypes can implicitly convert to their
    parent, `is_kind` creates **incompatible types** even when they share the same dimension:

    ```cpp
    // Regular inheritance - implicit conversions allowed:
    inline constexpr struct length_a : quantity_spec<isq::length> {} length_a;
    inline constexpr struct length_b : quantity_spec<isq::length> {} length_b;

    quantity a = length_a(5 * m);
    quantity b = length_b(10 * m);
    quantity sum = a + b;  // ✅ Compiles! Result: isq::length

    // With is_kind - no implicit conversions:
    inline constexpr struct fluid_head : quantity_spec<isq::height, is_kind> {} fluid_head;
    inline constexpr struct water_head : quantity_spec<isq::height, is_kind> {} water_head;

    quantity h_fluid = fluid_head(2 * m);
    quantity h_water = water_head(10 * m);
    // quantity bad = h_fluid + h_water;  // ❌ Does not compile!
    ```

    This absolute separation is crucial when quantities have the same dimension but represent
    incompatible physical concepts that require explicit conversion.

    ### Real-world physics requires conversion formulas

    _Energy_ conservation with different _density_ normalizations:

    ```cpp
    // H_water = H_fluid × SG (lighter reference needs larger height for same energy)
    constexpr QuantityOf<water_head> auto to_water_head(QuantityOf<fluid_head> auto h_fluid,
                                                        QuantityOf<specific_gravity> auto sg)
    {
      return water_head(isq::height(h_fluid) * sg);
    }

    // H_fluid = H_water / SG (denser reference needs smaller height for same energy)
    constexpr QuantityOf<fluid_head> auto to_fluid_head(QuantityOf<water_head> auto h_water,
                                                        QuantityOf<specific_gravity> auto sg)
    {
      return fluid_head(isq::height(h_water) / sg);
    }
    ```

    The conversion functions:

    - Make the physical relationship explicit (_energy_ = _weight_ × _height_)
    - Require specific _gravity_ (_density ratio_) as a parameter
    - Document the direction of conversion in the function name
    - Preserve dimensional correctness (both inputs and outputs are _lengths_)

    ### Converting to base type for generic algorithms

    When you need to work with the underlying dimension:

    ```cpp
    quantity h_fluid = fluid_head(2 * m);

    // ❌ Cannot pass directly to generic length function:
    // void process_length(QuantityOf<isq::length> auto len);
    // process_length(h_fluid);  // Does not compile!

    // ✅ Explicit conversion to base quantity:
    process_length(isq::height(h_fluid));  // Now compiles!
    ```

    This explicit conversion requirement:

    - Prevents accidental mixing of incompatible concepts
    - Makes domain boundary crossings visible in code
    - Allows generic algorithms to work with base quantities
    - Documents when physical meaning is intentionally discarded

    ### Comparison with built-in protections

    **mp-units** uses `is_kind` internally for dimensionless angle types:

    ```cpp
    // Built-in to mp-units:
    inline constexpr struct angular_measure : quantity_spec<dimensionless, is_kind> {} angular_measure;
    inline constexpr struct solid_angular_measure : quantity_spec<dimensionless, is_kind> {} solid_angular_measure;

    quantity angle = angular_measure(π * rad);
    quantity solid = solid_angular_measure(4*π * sr);
    // quantity bad = angle + solid;  // ❌ Does not compile!
    ```

    Your domain can have the same protection:

    - _Hydraulic head_ (fluid vs water reference)
    - _Pressure_ (gauge vs absolute)
    - _Voltage_ (RMS vs peak)
    - _Power_ (true vs apparent)

    ### When to use `is_kind` vs regular `quantity_spec`

    **Use regular `quantity_spec` when:**

    - Subtypes are conceptually compatible (_height_, _width_, _radius_ are all _lengths_)
    - Addition/comparison makes physical sense
    - Common ancestor captures the relationship

    **Use `is_kind` when:**

    - Same dimension but fundamentally different meanings
    - Direct mixing produces physically nonsense results
    - Conversion requires a formula (not just unit conversion)


## References

- [User's Guide: Systems of Quantities](../../users_guide/framework_basics/systems_of_quantities.md)
- [GitHub Discussion #757: Hydraulic Head](https://github.com/mpusz/mp-units/discussions/757)


## Takeaways

- **`is_kind` creates incompatible types**: Even when quantities share the same dimension,
  `is_kind` prevents mixing them without explicit conversion
- **Domain-specific safety**: Hydraulic engineering's distinction between _energy_ measurements
  in different reference frames (_fluid head_ vs _water head_) becomes a compile-time guarantee
- **Explicit conversions**: Physics-based conversions (through _specific gravity_) are visible
  and required in the code
- **Prevents subtle bugs**: The classic mistake of treating 2 m of mercury as 2 m of water
  becomes a compile error
- **Type system as documentation**: The code itself documents that these are different
  physical concepts requiring conversion
- **Similar to built-in protections**: Just as **mp-units** prevents mixing radians and
  steradians (_angular measure_ vs _solid angular measure_), your domain can have custom
  protections
- **Explicit base conversion when needed**: Both can convert to generic `isq::height` using
  `isq::height(h)` for algorithms that work on any _length_—but this requires an explicit
  conversion call; implicit conversion will fail to compile, preserving type safety at
  domain boundaries
- **Real-world safety**: Equipment undersizing due to _head_ calculation errors can be
  catastrophic in chemical plants—type safety prevents this
- **Pattern for other domains**: This technique applies anywhere quantities share dimensions
  but represent incompatible concepts—particularly _energy_ or _power_ measurements in
  different reference frames (e.g., _gauge_ vs _absolute pressure_, _RMS_ vs _peak voltage_,
  _true_ vs _apparent power_)
