# Workshop 7: Custom Units for Dimensionless Counters

Many real-world applications involve counting discrete items—products, packages, vehicles—that
have no physical dimensions but follow fixed scaling relationships. A carton contains a specific
number of units, a pallet holds a specific number of cartons, and so on.

Traditional code treats these as raw integers or floating-point numbers, leading to:

- **Scaling errors**: Forgetting to multiply/divide by conversion factors (12 units per carton)
- **Type confusion**: Accidentally adding units and cartons, or units and trucks
- **Silent bugs**: Calculations compile but produce nonsense results

This workshop demonstrates how **mp-units** extends beyond physical quantities to create
strongly-typed dimensionless counters for business and logistics domains, preventing these
errors through compile-time type checking and automatic conversions.

## Problem Statement

Consider a production and logistics scenario: an electronics factory produces light bulbs,
packages them into cartons (12 units per carton), and ships them via trucks (each truck
carries a fixed number of cartons).

The production manager needs to calculate weekly production totals, packaging requirements,
and shipping capacity based on production rate, operating hours, and packing ratios.

Traditional code using raw integers and constants faces several problems:

```cpp
// Traditional approach - no type safety
const int UNITS_PER_CARTON = 12;

int weekly_units = /* some calculation */;
int weekly_cartons = weekly_units / UNITS_PER_CARTON;  // Easy to forget!

// Nothing prevents these errors:
int bad1 = weekly_units + weekly_cartons;  // Mixing units and cartons!
int bad2 = weekly_units + trucks_needed;   // Mixing units and trucks!
```

**Problems with this approach:**

1. **No type safety**: Can accidentally add units, cartons, and trucks together
2. **Manual conversions**: Must remember scaling factors (12, 60, etc.) everywhere
3. **Easy to forget**: Time units (minutes vs hours) and conversion factors get mixed up
4. **Rounding errors**: Integer division and ceiling operations must be done manually
5. **Silent bugs**: Wrong calculations compile successfully and produce plausible-looking
  results

## Your task

1. Define custom dimensionless units for `unit` and `carton` with fixed
   compile-time scaling (1 carton = 12 units).
2. Given:
    - Production rate: `3 * unit / min`
    - Working hours per day: `14 * h`
    - Working days per week: `5`
    - Truck capacity: `60 * carton / truck`
3. Calculate:
    - Total weekly production in units and cartons
    - Number of cartons needed for weekly production (every carton should be full of goods)
    - Number of trucks required for shipment
4. Use strongly-typed quantities and automatic conversions for all calculations.
   Show how type safety prevents scaling errors.

```cpp
// ce-embed height=650 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/core.h>
#include <mp-units/systems/si.h>
#include <iostream>

namespace electronics {

// TODO: replace with strongly typed units
constexpr int unit = 1;
constexpr int carton = 12 * unit;
constexpr int truck = 1;

}

int main()
{
  using namespace electronics;
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  quantity production_rate = 3. * unit / min;
  quantity hours_per_day = 14 * h;
  quantity days_per_week = 5;
  quantity truck_capacity = 60 * carton / truck;

  quantity weekly_production = production_rate * hours_per_day * days_per_week;

  std::cout << "Weekly production:\n"
            // TODO: use correct units below
            << "- " << weekly_production.numerical_value_in(one) << " unit\n"
            << "- " << weekly_production.numerical_value_in(one) / carton << " carton\n";

  // TODO: use mp-units math functions instead
  quantity cartons_needed = std::floor((weekly_production / carton).numerical_value_in(one));
  std::cout << "Cartons needed for packaging: " << cartons_needed << "\n";

  quantity trucks_needed = std::ceil((weekly_production / truck_capacity).numerical_value_in(one));
  std::cout << "Trucks needed for shipment: " << trucks_needed << "\n";

  // TODO: the below should not compile
  quantity bad1 = 1 * unit + 1;
  quantity bad2 = 1 * unit + 1 * truck;
}
```

??? tip "Solution"

    ```cpp
    #include <mp-units/core.h>
    #include <mp-units/systems/si.h>
    #include <iostream>

    namespace electronics {

    inline constexpr struct unit final : mp_units::named_unit<"unit", mp_units::kind_of<mp_units::dimensionless>> {} unit;
    inline constexpr struct carton final : mp_units::named_unit<"carton", mp_units::mag<12> * unit> {} carton;
    inline constexpr struct truck final : mp_units::named_unit<"truck", mp_units::kind_of<mp_units::dimensionless>> {} truck;

    }

    int main()
    {
      using namespace electronics;
      using namespace mp_units;
      using namespace mp_units::si::unit_symbols;

      quantity production_rate = 3 * unit / min;
      quantity hours_per_day = 14 * h;
      quantity days_per_week = 5;
      quantity truck_capacity = 60 * carton / truck;

      quantity weekly_production = production_rate * hours_per_day * days_per_week;

      std::cout << "Weekly production:\n"
                << "- " << weekly_production.in(unit) << "\n"
                << "- " << weekly_production.in(carton) << "\n";

      quantity cartons_needed = floor<carton>(weekly_production);
      std::cout << "Cartons needed for packaging: " << cartons_needed << "\n";

      quantity trucks_needed = ceil<truck>(weekly_production / truck_capacity);
      std::cout << "Trucks needed for shipment: " << trucks_needed << "\n";

      // quantity bad1 = 1 * unit + 1;          // does not compile
      // quantity bad2 = 1 * unit + 1 * truck;  // does not compile
    }
    ```


??? abstract "What you learned?"

    ### Strongly-typed dimensionless units prevent scaling errors

    Raw integers and constants lead to easy-to-make mistakes:

    ```cpp
    // ❌ Traditional approach - no type safety
    const int UNITS_PER_CARTON = 12;
    int weekly_units = 25200;
    int weekly_cartons = weekly_units / UNITS_PER_CARTON;  // Easy to forget!

    // Nothing prevents these errors:
    int bad = weekly_units + weekly_cartons;  // Compiles! Wrong!
    ```

    With **mp-units**, scaling relationships are encoded in the type system:

    ```cpp
    // ✅ Type-safe approach
    inline constexpr struct unit final : named_unit<"unit", kind_of<dimensionless>> {} unit;
    inline constexpr struct carton final : named_unit<"carton", mag<12> * unit> {} carton;

    quantity weekly_production = 3 * unit / min * 14 * h * 5;
    quantity in_cartons = weekly_production.in(carton);  // Automatic conversion!

    // quantity bad = 1 * unit + 1 * carton;  // Does not compile!
    ```

    The `mag<12>` magnitude specifies the exact scaling relationship at compile time.

    ### Automatic conversions handle scaling for you

    Once you define the unit relationships, conversions happen automatically:

    ```cpp
    quantity production = 25200 * unit;  // Integer quantity (counting discrete items)

    // Automatic conversion when no precision loss:
    std::cout << production.in(unit);           // "25200 unit"

    // For conversions that could truncate, specify representation type:
    std::cout << production.in<double>(carton); // "2100 carton" (divided by 12)

    // Or use force_in for explicit truncation:
    std::cout << production.force_in(carton);   // "2100 carton" (truncates if needed)

    quantity from_cartons = 2100 * carton;
    std::cout << from_cartons.in(unit);         // "25200 unit" (multiplied by 12)
    ```

    No manual multiplication or division—the library handles it based on the magnitude.

    ### Rounding operations with `floor` and `ceil`

    When dealing with discrete units (cartons, trucks), rounding is common:

    ```cpp
    quantity weekly_production = production_rate * hours_per_day * days_per_week;

    // Must ship in full cartons only:
    quantity full_cartons = floor<carton>(weekly_production);  // Round down

    // Must ship in whole trucks only:
    quantity trucks = ceil<truck>(weekly_production / truck_capacity);  // Round up
    ```

    - `floor<Unit>(q)` rounds down to the nearest whole unit
    - `ceil<Unit>(q)` rounds up to the nearest whole unit
    - Both preserve strong typing and prevent errors

    ### Beyond physics: Business and logistics applications

    **mp-units** isn't limited to physical quantities. Use it anywhere numeric confusion is possible:

    ```cpp
    // Inventory management:
    inline constexpr struct crate final : named_unit<"crate", mag<24> * bottle> {} crate;
    inline constexpr struct pallet final : named_unit<"pallet", mag<50> * crate> {} pallet;

    // Currency subdivisions:
    inline constexpr struct dollar final : named_unit<"dollar", kind_of<dimensionless>> {} dollar;
    inline constexpr struct dime final : named_unit<"dime", mag_ratio<1, 10> * dollar> {} dime;
    inline constexpr struct cent final : named_unit<"cent", mag_ratio<1, 100> * dollar> {} cent;

    // Order processing:
    inline constexpr struct item final : named_unit<"item", kind_of<dimensionless>> {} item;
    inline constexpr struct dozen final : named_unit<"dozen", mag<12> * item> {} dozen;
    ```

    **Benefits:**

    - Compile-time prevention of mixing incompatible units
    - Automatic scaling without manual constants
    - Self-documenting code (units are visible in types)
    - Zero runtime overhead (all conversions are compile-time)


## References

- [User's Guide: Systems of Units](../users_guide/framework_basics/systems_of_units.md)
- [User's Guide: Dimensionless Quantities](../users_guide/framework_basics/dimensionless_quantities.md)
- [Systems Reference: Dimensionless Quantity Hierarchy](../reference/systems_reference/hierarchies/dimensionless.md)
- [API Reference](../reference/api_reference.md)


## Takeaways

- Strongly-typed wrappers for dimensionless counters prevent scaling and mixing errors.
- Automatic conversions make calculations safe and clear.
- This approach is valuable in production, logistics, and order processing domains.
- **mp-units** is not limited to physics—use it for any domain where numeric confusion
  is possible.
