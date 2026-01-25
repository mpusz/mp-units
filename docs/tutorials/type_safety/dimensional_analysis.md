# Automatic Dimensional Analysis

Watch **mp-units** automatically derive correct units through calculations.

**Goal**: Understand how dimensions propagate automatically  
**Time**: ~15 minutes

## Automatic Unit Derivation

When you multiply or divide quantities, **mp-units** automatically creates the
correct derived unit:

```cpp
// ce-embed height=650 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/core.h>
#include <mp-units/systems/si.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  // Start with base quantities
  quantity distance = 100.0 * m;
  quantity time = 10.0 * s;

  // Derived quantities are automatic!
  quantity speed = distance / time;                  // Automatically: m/s
  quantity acceleration = speed / time;              // Automatically: m/s²
  quantity distance_sq = distance * distance;        // Automatically: m²
  quantity acceleration2 = distance / pow<2>(time);  // Also m/s²

  std::cout << "Distance: " << distance << "\n";
  std::cout << "Time: " << time << "\n";
  std::cout << "Speed: " << speed << "\n";
  std::cout << "Acceleration: " << acceleration << "\n";
  std::cout << "Distance squared: " << distance_sq << "\n";
  std::cout << "Another acceleration: " << acceleration2 << "\n";
}
```

**Key insight**: You don't need to specify units for derived quantities - they're
computed automatically!

## Complex Derivations

The library handles arbitrarily complex unit combinations:

```cpp
// ce-embed height=750 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/core.h>
#include <mp-units/systems/si.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  // Basic quantities
  quantity mass = 70.0 * kg;
  quantity distance = 100.0 * m;
  quantity time = 9.58 * s;

  // Derived: speed
  quantity<m / s> speed = distance / time;
  std::cout << "Speed: " << speed << "\n";

  // Derived: acceleration
  quantity<m / s2> acceleration = speed / time;
  std::cout << "Acceleration: " << acceleration << "\n";

  // Derived: force = mass × acceleration (Newton's F = ma)
  quantity<N> force = mass * acceleration;
  std::cout << "Force: " << force << "\n";

  // Derived: kinetic energy (KE = ½mv²)
  quantity<J> kinetic_energy = 0.5 * mass * pow<2>(speed);
  std::cout << "Kinetic energy: " << kinetic_energy << "\n";

  // Derived: power (P = E/t)
  quantity<W> power = kinetic_energy / time;
  std::cout << "Power: " << power << "\n";
}
```

**How it works**: Notice how derived units cascade from base quantities:

- `m/s²` (_acceleration_) from `speed / time`
- `N` (_force_) from `mass * acceleration` - equivalent to `kg·m/s²` ($F = ma$)
- `J` (_energy_) from `mass * speed²` - equivalent to `kg·m²/s²` ($KE = \frac{1}{2}mv^2$)
- `W` (_power_) from `energy / time` - equivalent to `kg·m²/s³` ($P = \frac{E}{t}$)

The library automatically recognizes when derived dimensions match named units
(N, J, W, etc.), but you can also use the raw dimensional forms when needed.

!!! warning "Watch out for operator precedence!"

    A common mistake is forgetting that multiplication and division have the same
    precedence and are evaluated left-to-right:

    ```cpp
    // ❌ WRONG - This does NOT create km/h!
    quantity<km/h> q = 10 * km / 2 * h;  // Won't compile!
    // Evaluates as: ((10 * km) / 2) * h = km·h (distance-time, not speed!)

    // ✅ CORRECT - Use parentheses
    quantity<km/h> speed = 10 * km / (2 * h);  // km/h = 5 km/h
    ```

    **With CTAD, it's even more dangerous:**

    ```cpp
    // ❌ WRONG - Compiles but creates the wrong type!
    quantity q = 10 * km / 2 * h;  // Type is km·h, not km/h!
    // Later code expecting speed will fail or produce wrong results
    ```

    **Best practices:**

    - Always use parentheses when creating quantities in equation's numerator
    - Be especially careful with CTAD - it won't catch this error
    - When in doubt, specify the expected type explicitly

**Key insight**: Each calculation automatically produces the correct derived unit, and the
library recognizes when dimensions match standard named units like Newton (N), Joule (J),
or Watt (W). You can explicitly specify these in your code, but the dimensional analysis
always happens automatically.

## Physics Formula Example

```cpp
// ce-embed height=700 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/core.h>
#include <mp-units/systems/si.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  // Projectile motion: h = v₀t - ½gt²
  quantity initial_velocity = 50.0 * m / s;
  quantity gravity = 1.0 * si::standard_gravity; // unit constant (implicitly m/s2)
  quantity time = 3.0 * s;

  // Upward distance from initial velocity
  quantity upward_distance = initial_velocity * time;
  std::cout << "v₀t = " << upward_distance << "\n";

  // Downward distance from gravity
  quantity downward_distance = 0.5 * gravity * pow<2>(time);
  std::cout << "½gt² = " << downward_distance << "\n";

  // Final height
  quantity height = upward_distance - downward_distance;
  std::cout << "Height (common unit): " << height << "\n";
  std::cout << "Height in meters: " << height.in(m) << "\n";

  // Notice:
  // - (m/s) × s = m (automatically simplified!)
  // - When subtracting quantities with different unit representations,
  //   the library finds a "common unit" that may look complex
  // - Use .in(unit) to convert to a specific readable unit

  std::cout << "\nAt t=" << time << ", projectile is at " << height.in(m) << "\n";
}
```

**Key insight**: When subtracting quantities with different unit representations,
the library automatically finds a common unit (which may look complex). Use `.in(unit)`
to convert to your preferred readable unit. The dimensional analysis ensures the result
is correct regardless of intermediate unit representations.

## Challenges

1. **_Density_**: Calculate _density_ from _mass_ 500 g, _volume_ 250 cm³. What unit?
2. **_Gravitational potential energy_**: Calculate $PE = mgh$ for _mass_ 10 kg,
  _height_ 5 m, using `si::standard_gravity`. What unit? (Hint: Joule)
3. **_Work_**: _Force_ 10 N over _distance_ 5 m. What unit? (Hint: Joule again)

## What You Learned?

✅ Derived units are computed automatically from operations  
✅ Multiplication and division create new dimensions  
✅ Complex unit expressions are simplified automatically  
✅ You never need to manually specify derived units  
✅ Some derived units have standard names (N, J, W, etc.)  
✅ The library recognizes derived named units but doesn't return them
  automatically from calculations - you specify which form to use  
✅ Always use parentheses when creating quantities in the equation's numerator
