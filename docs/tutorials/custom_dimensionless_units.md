# Tutorial 7: Custom Units for Dimensionless Counters

Many real-world applications involve counting discrete items—products, packages, vehicles—that
have no physical dimensions but follow fixed scaling relationships. A carton contains a specific
number of units, a pallet holds a specific number of cartons, and so on.

Traditional code treats these as raw integers or floating-point numbers, leading to:

- **Scaling errors**: Forgetting to multiply/divide by conversion factors (12 units per carton)
- **Type confusion**: Accidentally adding units and cartons, or units and trucks
- **Silent bugs**: Calculations compile but produce nonsense results

This tutorial demonstrates how **mp-units** extends beyond physical quantities to create
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
5. **Silent bugs**: Wrong calculations compile successfully and produce plausible-looking results

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

??? "Solution"

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


## References

- [User's Guide: Systems of Units](../users_guide/framework_basics/systems_of_units.md)
- [User's Guide: Dimensionless Quantities](../users_guide/framework_basics/dimensionless_quantities.md)
- [API Reference](../api_reference.md)


## Takeaways

- Strongly-typed wrappers for dimensionless counters prevent scaling and mixing errors.
- Automatic conversions make calculations safe and clear.
- This approach is valuable in production, logistics, and order processing domains.
- **mp-units** is not limited to physics—use it for any domain where numeric confusion
  is possible.
