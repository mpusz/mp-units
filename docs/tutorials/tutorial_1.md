# Tutorial 1: Refactor to Strong Types

In this tutorial, you’ll refactor a small legacy function that uses raw doubles into a
type‑safe version using **mp-units**. The goal is to accept and return strong, unit‑aware
types to prevent dimensional bugs at compile time.

## Problem statement

The legacy function computes _kinetic energy_ ($E = \tfrac{1}{2}\, m\, v^2$) and assumes
kilogram and metre per second, but nothing enforces this:


```cpp
double kinetic_energy_j(double mass_kg, double speed_mps)
{
  return 0.5 * mass_kg * speed_mps * speed_mps; // Joules (if inputs are indeed kg and m/s)
}
```

## Your task

Refactor the legacy function, and arguments passed to it from `main()` to use strongly
typed quantities (e.g., quantity of mass in kilograms, quantity of speed in metre
per second). Print the result in J and kJ.

When you are done, make a few intentional mistakes and check error messages.

```cpp
// ce-embed height=650 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3"
#include <mp-units/systems/si.h>
#include <mp-units/core.h>
#include <iostream>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

// Legacy implementation
constexpr double kinetic_energy_j(double mass_kg, double speed_mps)
{
  return 0.5 * mass_kg * speed_mps * speed_mps;
}

// TODO: Implement a strongly typed version
// ...

int main()
{
  double mass_kg = 80;
  double speed_mps = 12.5;
  auto energy_J = kinetic_energy_j(mass_kg, speed_mps);
  std::cout << energy_J << " J (" << energy_J / 1000 << " kJ)\n";

  // do the same as above with strong types
  // 
}
```

??? "Solution"

    ```cpp
    #include <mp-units/systems/si.h>
    #include <mp-units/core.h>
    #include <iostream>

    using namespace mp_units;
    using namespace mp_units::si::unit_symbols;

    constexpr quantity<si::joule> kinetic_energy(quantity<si::kilogram> mass,
                                                 quantity<si::metre / si::second> speed)
    {
      return 0.5 * mass * pow<2>(speed);
    }

    int main()
    {
      quantity mass = 80 * kg;
      quantity speed = 12.5 * (m / s);
      quantity energy = kinetic_energy(mass, speed);
      std::cout << energy << " (" << energy.in(kJ) << ")\n";
    }
    ```

## Takeaways

- Function contracts are now self‑documenting and enforced by the type system.
- Wrong‑unit calls fail at compile time, preventing latent bugs.
- The return type
    - communicates its physical meaning, not just a raw number,
    - ensures that the object returned from a function is the result of correct quantity arithmetic.
