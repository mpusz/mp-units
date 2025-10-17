# Tutorial 2: Refactor to Generic, Type-Safe Interfaces

In this exercise, you’ll refactor a function that computes a derived quantity from quantities
measured in SI base units, where the arguments may use different prefixes or scaled units.

For this exercise, we’ll use a real-world example from engineering and meteorology:
_surface mass density_ (_mass_ per _area_). This is used for things like snow load on roofs,
coating thickness, or areal density of materials, and results in a derived unit (kg/m², g/cm²,
etc.) that does not have a special SI name.

The goal is to make the function generic and type-safe using C++ concepts, so it works for
any compatible units and prevents scaling errors at compile time.

## Problem statement

Suppose you have a function that computes _surface mass density_ $\sigma = \frac{m}{A}$
(_mass_ divided by _area_), but the current version only accepts _mass_ in kilograms and
_area_ in square meters, returning the result in kg/m².

```cpp
quantity<si::kilogram / square(si::metre)> surface_mass_density(quantity<si::kilogram> mass,
                                                                quantity<square(si::metre)> area)
{
  return mass / area;
}
```

In real-world scenarios, you might have _mass_ in grams and _area_ in square centimeters,
and you want the function to work generically and safely for any compatible units without
unnecessary conversions that may affect your runtime performance or truncate data.

## Your task

Refactor the function so it accepts any quantity representing _mass_ and any quantity
representing _area_, regardless of their specific units or prefixes. Use `QuantityOf`
concepts to constrain the arguments, ensuring only quantities of _mass_ and _area_
are accepted.

The function should return a quantity representing _surface mass density_, with the unit
determined by the division of the input units (e.g., g/cm², kg/m², etc.).

When you are done, try making a few intentional mistakes and observe the compile-time
error messages. For example:

- pass _length_ instead of _mass_, or _volume_ instead of _area_
- change the equation in the function's body to multiply instead of divide the its
  arguments (typical programmer's mistake).

This will help you understand how concepts enforce type safety.

```cpp
// ce-embed height=650 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#include <iostream>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

// TODO: Refactor to a generic, type-safe version
// - Use concepts to accept any mass and area quantities (with any units or prefixes)
// - The return type should be constrained to isq::surface_mass_density quantity
//   and accept any compatible units
// - No unit scaling should be applied; the result should reflect the direct division
constexpr quantity<si::kilogram / square(si::metre)> surface_mass_density(quantity<si::kilogram> mass,
                                                                          quantity<square(si::metre)> area)
{
  return mass / area;
}

int main()
{
  constexpr Unit auto cm2 = cm * cm;
  quantity mass = 350. * g;         // e.g., snow load on a small roof patch
  quantity area = 5000. * cm2;      // 0.5 square meters
  quantity res = surface_mass_density(mass, area);
  // Convert the result to a specific unit if needed (e.g., g/cm²)
  quantity res_in_g_per_cm2 = res.in(g / cm2);
  std::cout << "res = " << res << '\n';
  std::cout << "res in g/cm² = " << res_in_g_per_cm2 << '\n';
}
```

??? "Solution"

    ```cpp
    #include <mp-units/systems/isq.h>
    #include <mp-units/systems/si.h>
    #include <iostream>

    using namespace mp_units;
    using namespace mp_units::si::unit_symbols;

    constexpr QuantityOf<isq::surface_mass_density> auto surface_mass_density(QuantityOf<isq::mass> auto mass,
                                                                              QuantityOf<isq::area> auto area)
    {
      return mass / area;
    }

    int main()
    {
      constexpr Unit auto cm2 = cm * cm;
      quantity mass = 350. * g;         // e.g., snow load on a small roof patch
      quantity area = 5000. * cm2;      // 0.5 square meters
      auto res = surface_mass_density(mass, area);
      std::cout << res << '\n';
    }
    ```

## Takeaways

- Use C++ concepts to enforce that only valid _mass_ and _area_ quantities can be
  passed to the function, ensuring type safety at compile time.
- The refactored function is generic and works with any compatible units or prefixes
  (e.g., g, kg, m², cm²), making it robust and flexible.
- The concept in the return type both advertises the output quantity and verifies that
  the quantity equation inside the function is correct (i.e., the result is a valid
  quantity type for _surface mass density_).
- The return type is automatically deduced by the type system, reflecting the correct
  derived unit (e.g., g/cm², kg/m²).
- Compile-time errors will occur if you attempt to pass incompatible types (e.g.,
  _length_ instead of _mass_, or _volume_ instead of _area_) or perform incorrect
  calculation inside of the function (e.g., multiplication instead of division),
  preventing subtle bugs.
- This approach improves runtime performance and reduces the risk of data truncation
  during unnecessary conversions.
