# Unit Conversions

Learn to convert quantities between different units safely and explicitly.

**Goal**: Convert the same quantity to different units  
**Time**: ~15 minutes

## The `.in()` Method

To express a quantity in different units, use `.in()`:

```cpp
// ce-embed height=600 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <mp-units/systems/yard_pound.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;
  using namespace mp_units::yard_pound::unit_symbols;

  // Distance in meters
  quantity distance = 5.0 * km;

  std::cout << "Original: " << distance << "\n";
  std::cout << "In meters: " << distance.in(m) << "\n";
  std::cout << "In miles: " << distance.in(mi) << "\n";
  std::cout << "In feet: " << distance.in(ft) << "\n";

  // Speed conversion
  quantity speed = 100.0 * km / h;

  std::cout << "\nSpeed: " << speed << "\n";
  std::cout << "In m/s: " << speed.in(m / s) << "\n";
  std::cout << "In mph: " << speed.in(mph) << "\n";
}
```

**Key insight**: `.in()` creates a new quantity in the target unit - the original stays unchanged!

!!! tip "Discover available units"

    Want to see all available units in different systems? Check out the
    [Units Index](../../reference/systems_reference/units_index.md) for a complete alphabetical list!

## Conversion in Expressions

You can convert at any point:

```cpp
// ce-embed height=550 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <mp-units/systems/yard_pound.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;
  using namespace mp_units::yard_pound::unit_symbols;

  // Full IronMan triathlon distances
  quantity swim_distance = 2.4 * mi;      // 2.4 miles
  quantity bike_distance = 112.0 * mi;    // 112 miles
  quantity run_distance = 26.2 * mi;      // Marathon

  // Mix units in calculations, convert result
  quantity total = (swim_distance + bike_distance + run_distance).in(km);

  std::cout << "Full IronMan total: " << total << "\n";

  // Or convert before adding (same result)
  quantity total2 = swim_distance.in(km) + bike_distance.in(km) + run_distance.in(km);

  std::cout << "Also: " << total2 << "\n";
}
```

**Key insight**: You can convert at any point in a calculation - before or after operations.
The result is the same because conversions are just scaling factors. Convert early to
choose your preferred output unit, or late to work in input units.

## Common Conversions

```cpp
// ce-embed height=650 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <mp-units/systems/yard_pound.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;
  using namespace mp_units::yard_pound::unit_symbols;

  // Length
  std::cout << "=== Length ===\n";
  std::cout << (1.0 * km).in(m) << "\n";
  std::cout << (1.0 * mi).in(km) << "\n";
  std::cout << (1.0 * ft).in(m) << "\n";

  // Speed
  std::cout << "\n=== Speed ===\n";
  std::cout << (60.0 * km / h).in(m / s) << "\n";
  std::cout << (60.0 * mi / h).in(km / h) << "\n";

  // Time
  std::cout << "\n=== Time ===\n";
  std::cout << (1.0 * h).in(min) << "\n";
  std::cout << (90.0 * min).in(h) << "\n";
  std::cout << (1.0 * d).in(h) << "\n";

  // Mass
  std::cout << "\n=== Mass ===\n";
  std::cout << (1.0 * kg).in(g) << "\n";
  std::cout << (1.0 * lb).in(kg) << "\n";
}
```

**Key insight**: Common conversions like km ↔ m, hours ↔ minutes, and kg ↔ g are
straightforward with `.in()`. The library handles conversion factors automatically,
including complex derived units like km/h ↔ m/s.

## Function Interfaces with Automatic Conversion

When you specify exact units in function parameters, the library automatically converts
compatible units at the call site (if conversion is safe):

```cpp
// ce-embed height=700 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <iostream>

using namespace mp_units;

// Function expects kilometers
quantity<si::kilo<si::metre>> round_trip(quantity<si::kilo<si::metre>> one_way)
{
  return 2 * one_way;
}

int main()
{
  using namespace mp_units::si::unit_symbols;

  // Pass km directly - no conversion needed
  quantity distance1 = 50.0 * km;
  std::cout << "Round trip (km): " << round_trip(distance1) << "\n";

  // Pass meters - automatically converts to km!
  quantity distance2 = 30'000.0 * m;
  std::cout << "Round trip (m→km): " << round_trip(distance2) << "\n";
}
```

**Key insight**: This automatic conversion prevents the classic bug where you pass a
value in the wrong unit scale to a `double` parameter. The function signature now
_enforces_ the expected unit, and the library safely converts when needed!

## Challenges

1. **Function with conversion**: Create a function that accepts `quantity<si::metre>` and
   call it with both `50 * m` and `0.05 * km` to verify automatic conversion works
2. **Convert your height**: Create your _height_ in feet/inches, convert to centimeters
   (or vice versa depending where you live 😉)
3. **Microwave power**: Create 1200 W, convert to horsepower (hp) and kilowatts (kW)
4. **Tire pressure**: Create 32 psi (pounds per square inch), convert to Pa and kPa
5. **Battery capacity**: Create 500 Wh (watt-hours), convert to kWh and megajoules (MJ)

## What You Learned?

✅ Use `.in(unit)` to convert quantities explicitly  
✅ Original quantity remains unchanged after conversion  
✅ Functions with specific unit requirements automatically convert compatible
  units safely  
✅ Explicit conversions prevent accidental unit confusion  
✅ You can convert at any point in calculations  
✅ Mixing units is safe - the library handles it!
