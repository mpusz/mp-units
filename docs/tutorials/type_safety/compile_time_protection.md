# Compile-Time Protection

See how **mp-units** catches unit errors at compile time, before your code runs.

**Goal**: Understand the compile-time safety guarantees  
**Time**: ~15 minutes

## Unit Mismatch Errors

**mp-units** prevents nonsensical operations:

```cpp
// ce-embed height=550 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  quantity distance = 100.0 * m;
  quantity time = 10.0 * s;
  quantity mass = 5.0 * kg;
  // standard_gravity is a unit constant provided by mp-units (~9.80665 m/s²)
  quantity weight = mass * si::standard_gravity;  // Force (weight = mass × gravity)

  // These work - compatible units
  quantity total_distance = distance + 50.0 * m;
  quantity speed = distance / time;

  std::cout << "Total distance: " << total_distance << "\n";
  std::cout << "Speed: " << speed << "\n";
  std::cout << "Weight: " << weight << " = " << weight.in(N) <<"\n";

  // Try uncommenting these - they won't compile!
  // quantity bad = distance + time;        // ❌ Can't add meters and seconds
  // quantity bad2 = mass + weight;         // ❌ Can't add mass (kg) and weight (N)!
  // if (distance < time) { }               // ❌ Can't compare meters and seconds

  // This DOES compile - multiplication creates new derived units:
  quantity combined = distance * time * mass;  // ✅ Creates m·s·kg
  std::cout << "Combined: " << combined << "\n";
}
```

**Key insight**: The type system prevents nonsensical operations (adding meters to seconds,
comparing _mass_ to _force_) at compile time. However, multiplication and division are always
allowed because they create new derived units through dimensional analysis - this is
mathematically valid and fundamental to physics calculations.

## Dimensional Analysis Enforcement

The library tracks dimensions through calculations:

```cpp
// ce-embed height=650 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  quantity distance = 1000.0 * m;
  quantity duration = 50.0 * s;
  quantity mass = 70.0 * kg;

  // Compiler verifies the result matches the declared type
  quantity<si::metre / si::second> speed = distance / duration;
  std::cout << "Speed: " << speed << "\n";

  // Acceleration
  quantity<si::metre / square(si::second)> acceleration = speed / duration;
  std::cout << "Acceleration: " << acceleration << "\n";

  // Force = mass × acceleration
  quantity<si::newton> force = mass * acceleration;
  std::cout << "Force: " << force << "\n";

  // Energy: W = F × d
  quantity<si::joule> energy = force * distance;
  std::cout << "Energy: " << energy << "\n";

  // Each operation creates the correct derived unit!
  // Try assigning to wrong types - won't compile
  // quantity<si::metre> bad = speed;   // ❌ speed is m/s, not m
  // quantity<si::watt> bad2 = energy;  // ❌ energy is J, not W
}
```

**Key insight**: The compiler tracks dimensions through every operation. When you declare
a specific quantity type (like `quantity<si::newton>`), the compiler verifies that the
calculation produces that exact dimension, catching errors at compile time.

## Type Safety in Function Signatures

```cpp
// ce-embed height=700 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <iostream>

using namespace mp_units;

// Create a unit for fuel consumption: L/100km
inline constexpr Unit auto L_per_100km = si::litre / (mag<100> * si::kilo<si::metre>);

// Function calculates fuel needed for a trip
quantity<si::litre> fuel_needed(quantity<si::kilo<si::metre>> distance,
                                quantity<L_per_100km> consumption)
{
  return distance * consumption;

  // This won't compile - wrong return type:
  // return distance / consumption;  // ❌ Returns km²/L, not L!
}

int main()
{
  using namespace mp_units::si::unit_symbols;

  quantity trip_distance = 450 * km;
  quantity car_consumption = 6.5 * L_per_100km;  // 6.5 L/100km

  quantity fuel = fuel_needed(trip_distance, car_consumption);

  std::cout << "Trip distance: " << trip_distance << "\n";
  std::cout << "Consumption rate: " << car_consumption << "\n";
  std::cout << "Fuel needed: " << fuel << "\n";

  // These won't compile - wrong types!
  // fuel_needed(car_consumption, trip_distance);  // ❌ Swapped arguments
  // fuel_needed(40 * L, trip_distance);           // ❌ fuel (L) is not distance (km)
}
```

**Key insight**: Function signatures document expected dimensions - the compiler enforces
it!

## Challenges

1. **Experience compiler errors**: Go back to the examples above and uncomment the lines
   marked with ❌ to see the actual compiler error messages
2. **Write a typed function**: Create a function that takes two _lengths_ and returns an _area_,
   with explicit `quantity<...>` types in the signature
3. **Cross-system compatibility**: Write a `below_speed_limit()` function that takes a _speed_
   in km/h (use `quantity<si::kilo<si::metre> / si::hour>` in signature). Then test it by
   passing _speeds_ in mi/h using the `yard_pound` system. The compiler verifies the
   dimensions match and automatically converts between units!

## What You Learned?

✅ mp-units catches unit mismatches at compile time  
✅ You cannot add, subtract, or compare incompatible units  
✅ Dimensional analysis is enforced through all calculations  
✅ Function signatures can require specific dimensions  
✅ Errors are caught in seconds, not hours  
