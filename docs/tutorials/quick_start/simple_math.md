# Simple Math

Learn to perform calculations with quantities. The library handles units automatically!

**Goal**: Add, subtract, multiply, and divide quantities  
**Time**: ~10 minutes

## Math with Quantities

One of **mp-units**' superpowers: **units are handled automatically** during calculations.

When you divide distance by time, you get speed (km/h) - no manual conversions needed!

## Try It: Basic Operations

```cpp
// ce-embed height=600 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <format>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  // Journey distances
  quantity trip1 = 6 * km;
  quantity trip2 = 2700 * m;

  // Addition: km + m = m (common unit!)
  quantity total_distance = trip1 + trip2;
  std::cout << std::format("Total distance: {}\n", total_distance);

  // Time taken
  quantity time = 2 * h;

  // Division: km / h = km/h (speed!)
  quantity average_speed = total_distance / time;
  std::cout << std::format("Average speed: {}\n", average_speed);

  // Multiplication: speed * time = distance
  quantity distance_traveled = average_speed * time;
  std::cout << std::format("Distance traveled: {}\n", distance_traveled);

  // Subtraction: km - m = m (common unit!)
  quantity difference = trip1 - trip2;
  std::cout << std::format("Difference between trips: {}\n", difference);
}
```

**What happened:**

- **Addition/Subtraction**: Only works with compatible units (km + m ✅, km + hours ❌)
    - Notice: Adding `km + m` automatically converted to a common unit (`m`)
- **Multiplication/Division**: Creates new units automatically (km ÷ h = km/h)
- **Dimensional analysis**: The library tracks units through all operations

**Key insight**: The library automatically handles unit conversions and dimensional analysis
during calculations.

## Valid Operations

| Operation       | Example                 | Result Unit |
|-----------------|-------------------------|-------------|
| Addition        | `120 * km + 80 * km`    | `200 km`    |
| Subtraction     | `120 * km - 80 * km`    | `40 km`     |
| Division        | `120 * km / (2 * h)`    | `60 km/h`   |
| Multiplication  | `60 * km / h * (2 * h)` | `120 km`    |
| Scalar multiply | `50 * km * 2`           | `100 km`    |
| Scalar divide   | `100 * km / 4`          | `25 km`     |

## Common Units in Addition/Subtraction

When adding quantities with **different units** (like km + m above), the library
picks a **common unit** for the result - typically the more fundamental unit:

```cpp
quantity result = 1 * km + 2000 * m;  // Result: 3000 m
```

!!! info

    When mixing different unit systems (e.g., SI km + imperial mi), the library finds a
    common unit that may differ from both inputs. You'll often need to explicitly convert
    to your preferred unit with `.in(unit)`. Learn more about this in
    [Generic Interfaces](../type_safety/generic_interfaces.md).

## Challenges

Try these in the editor above:

1. **Calculate fuel efficiency**: `quantity fuel = 50 * l;` then `total_distance / fuel` (km/L)
2. **Multiple operations**: Try `(trip1 + trip2) / (time * 2)`
3. **Acceleration**: `quantity velocity = 30 * m / s; quantity duration = 5 * s;` then
  `velocity / duration`
4. **See compile errors**: Try `trip1 + time` or `trip1 < time` - observe the compiler's
  error message! (We'll explore this compile-time safety in depth in
  [Compile-Time Protection](../type_safety/compile_time_protection.md))

## What You Learned?

✅ Addition/subtraction only work with compatible units  
✅ Adding different units (km + m) produces a common unit result  
✅ Multiplication/division create new composite quantities  
✅ Dimensional analysis happens automatically  
✅ Format quantities with `std::format` for flexible output
