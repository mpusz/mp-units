# Extracting Numeric Values

Learn when and how to extract raw numbers from quantities.

**Goal**: Get numeric values in specific units for output or legacy APIs  
**Time**: ~20 minutes

## Why Extract Values?

Sometimes you need just the number:

- **Interfacing with external systems**:
    - Displaying to users
    - Calling legacy APIs that take raw numbers
    - Storing in databases
    - Plotting graphs
- **Performance-critical computations**:
    - Linear algebra operations on large matrices/vectors
    - High-Performance Computing (HPC) simulations
    - Data crunching and numerical analysis
    - GPU computations requiring contiguous memory arrays

In performance-critical scenarios, you may need to extract values into uniform arrays for
efficient processing, trading type safety for computational speed.

## The `.numerical_value_in()` Method

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

  quantity distance = 5.0 * km;

  // Extract numeric value in specific unit
  double meters = distance.numerical_value_in(m);
  double kilometers = distance.numerical_value_in(km);
  double miles = distance.numerical_value_in(mi);

  std::cout << "Distance: " << distance << "\n\n";
  std::cout << "As number in meters: " << meters << "\n";
  std::cout << "As number in kilometers: " << kilometers << "\n";
  std::cout << "As number in miles: " << miles << "\n";
}
```

**Key insight**: `.numerical_value_in()` extracts the raw numeric value in a specified unit,
completely removing type safety. The result is just a number with no compile-time protection
against unit errors.

!!! warning

    After extraction, the number has NO unit safety! Use carefully.

## Difference: `.in()` vs `.numerical_value_in()`

```cpp
// ce-embed height=550 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  quantity speed = 100.0 * km / h;

  // .in() returns a quantity (still type-safe!)
  quantity speed_mps = speed.in(m / s);
  std::cout << "With .in(): " << speed_mps << " (still a quantity)\n";

  // .numerical_value_in() returns just the number
  double speed_value = speed.numerical_value_in(m / s);
  std::cout << "With .numerical_value_in(): " << speed_value << " (raw double)\n";

  // Calculate distance after 30 minutes
  quantity time = 30.0 * min;

  // With quantity: automatic unit handling
  quantity distance = speed_mps * time;
  std::cout << "\nDistance with quantity: " << distance.in(km) << "\n";

  // With raw number: you must handle units manually!
  double time_seconds = time.numerical_value_in(s);  // Need to extract in compatible unit
  double distance_meters = speed_value * time_seconds;  // Manual calculation
  std::cout << "Distance with raw numbers: " << distance_meters << " m\n";
  std::cout << "  (had to manually ensure compatible units and track result unit!)\n";

  // If you need to calculate speed again from these raw numbers:
  double calculated_speed = distance_meters / time_seconds;  // Just a double!
  std::cout << "\nCalculated speed from raw numbers: " << calculated_speed << " m/s\n";
  std::cout << "  (no automatic derived quantity type, no dimensional analysis!)\n";
}
```

**Key insight**: When you extract numerical values, you lose not only type safety, but also
the ability to scale the unit easily or get automatically derived quantity types. The result
of `double / double` is just a `double`, not a `quantity<speed>`. Prefer `.in()` when you
can stay type-safe; use `.numerical_value_in()` only when necessary.

## Common Use Case: Legacy APIs

```cpp
// ce-embed height=650 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <iostream>

// Legacy function that takes raw numbers (be careful!)
void legacy_draw_circle(double x, double y, double radius)
{
  std::cout << "Drawing circle at (" << x << ", " << y << ") with radius " << radius << "\n";
}

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  // Work with type-safe quantities
  quantity x_pos = 100.0 * mm;
  quantity y_pos = 50.0 * mm;
  quantity radius = 25.0 * mm;

  // Extract only when calling legacy API
  legacy_draw_circle(x_pos.numerical_value_in(mm),
                     y_pos.numerical_value_in(mm),
                     radius.numerical_value_in(mm));

  // Stay type-safe for your own calculations
  quantity diameter = 2.0 * radius;
  // Note: `pi` is a compile-time unit constant provided by mp-units
  quantity circumference = 2.0 * pi * radius;

  std::cout << "\nDiameter: " << diameter << "\n";
  std::cout << "Circumference: " << circumference << " = " << circumference.in(mm) << "\n";
}
```

**Key insight**: Extract values only at system boundaries (legacy APIs, databases, external
libraries) while keeping all internal calculations type-safe with `quantity`. This minimizes
the surface area where unit errors can occur.

## Custom Text Output

```cpp
// ce-embed height=650 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <mp-units/systems/yard_pound.h>
#include <iostream>
#include <format>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;
  using namespace mp_units::yard_pound::unit_symbols;

  quantity mass = 72.5 * kg;
  quantity speed = 65.3 * (mi / h);
  quantity distance = 142.7 * mi;

  // mp-units default formatting may not satisfy your requirements
  std::println("\nWith automatic formatting:");
  std::println("Mass: {}", mass);
  std::println("Speed: {}", speed);
  std::println("Distance: {}", distance);

  // You might be tempted to extract numerical value explicitly
  std::println("\nUsing extracted values in specified unit:");
  std::println("Mass: {:.1f} in kg", mass.numerical_value_in(kg));
  std::println("Speed: {:.0f} in mph", speed.numerical_value_in(mi / h));
  std::println("Distance: {:.1f} in miles", distance.numerical_value_in(mi));

  // Typically it is better to benefit from the mp-units rich formatting options
  std::println("\nUsing format specifiers:");
  std::println("Mass: {:%N in %U:N[.1f]}", mass.in(kg));
  std::println("Speed: {:%N:N[.0f]} in mph", speed.in(mi / h));
  std::println("Distance: {:%N:N[.1f]} in miles", distance.in(mi));
}
```

**Key insight**: Prefer format specifiers (`%N`, `%U`) over `.numerical_value_in()` for output
formatting. Use `%U` when the unit symbol matches your desired text (e.g., "kg"), and provide
custom text when you need specific names (e.g., "mph" instead of "mi/h"). This keeps your
code type-safe while providing flexible formatting.

## Best Practices

✅ **DO**: Keep calculations in `quantity` as long as possible  
✅ **DO**: Extract values only at boundaries (display, APIs, storage)  
✅ **DO**: Document the expected unit when extracting  
❌ **DON'T**: Extract early and pass raw numbers around  
❌ **DON'T**: Mix extracted values from different units

## Challenges

1. **Database storage**: Extract mass in kg, volume in L, temperature in K for storage
2. **Plotting**: Create 5 time values (0-100 s) and 5 distance values, extract for plotting
3. **User input**: Read a number as mph, create quantity, extract as km/h for display

## What You Learned?

✅ Use `.numerical_value_in(unit)` to extract raw numbers (removes all type safety)  
✅ Use format specifiers (`%N`, `%U`) for output instead of extracting values  
✅ Use `%U` when unit symbol matches desired text, custom text otherwise  
✅ `.in()` keeps type safety, `.numerical_value_in()` loses it  
✅ Calculations with raw numbers return `double`, not derived `quantity` types  
✅ Extract values only at system boundaries (APIs, display, storage, performance)  
✅ Keep all internal calculations in `quantity` as long as possible  
✅ Document expected units when extracting values
