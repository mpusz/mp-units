# Tutorial 8: Custom Units for Dimensionless Counters

This tutorial demonstrates how to use **mp-units** to create strongly-typed wrappers for
common dimensionless counters in business and logistics, such as units and cartons.
These are not related to physics, but benefit from type safety and automatic conversions.

## Scenario

A small electronics factory produces light bulbs. The production line outputs 3 units every
minute, operates 14 hours per day, 5 days per week. Products are packed into cartons
(1 carton = 12 units). For shipping, each truck can carry a fixed number of cartons.

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
// ce-embed height=650 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3"
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

    inline constexpr struct unit final : mp_units::named_unit<"unit", kind_of<dimensionless>> {} unit;
    inline constexpr struct carton final : mp_units::named_unit<"carton", mp_units::mag<12> * unit> {} carton;
    inline constexpr struct truck final : mp_units::named_unit<"truck", kind_of<dimensionless>> {} truck;

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


## Takeaways

- Strongly-typed wrappers for dimensionless counters prevent scaling and mixing errors.
- Automatic conversions make calculations safe and clear.
- This approach is valuable in production, logistics, and order processing domains.
- **mp-units** is not limited to physics—use it for any domain where numeric confusion
  is possible.
