# Temperature Applications

Advanced _temperature_ operations including averaging, interpolation, and real-world calculations.

**Goal**: Master advanced _temperature_ operations and understand their practical applications  
**Time**: ~10 minutes

**Prerequisites**: Complete the [Temperature Handling](temperature_handling.md) tutorial first


## Temperature Averaging and Interpolation

For averaging or interpolating between _temperature_ points, use `midpoint` and `lerp` functions:

```cpp
// ce-embed height=750 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/core.h>
#include <mp-units/systems/si.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  // Two temperature measurements
  quantity_point temp1 = point<deg_C>(10);  // 283.15 K
  quantity_point temp2 = point<deg_C>(20);  // 293.15 K

  std::cout << "Temperature 1: " << temp1.quantity_from_zero() << "\n";
  std::cout << "Temperature 2: " << temp2.quantity_from_zero() << "\n\n";

  // Average using midpoint
  quantity_point avg_temp = midpoint(temp1, temp2);
  std::cout << "Midpoint: " << avg_temp.quantity_from_zero() << "\n";

  // General interpolation with lerp
  quantity_point quarter = lerp(temp1, temp2, 0.25);        // 25% of the way
  quantity_point half = lerp(temp1, temp2, 0.5);            // Same as midpoint
  quantity_point three_quarter = lerp(temp1, temp2, 0.75);  // 75% of the way

  std::cout << "25% interpolation: " << quarter.quantity_from_zero() << "\n";
  std::cout << "50% interpolation: " << half.quantity_from_zero() << "\n";
  std::cout << "75% interpolation: " << three_quarter.quantity_from_zero() << "\n\n";

  // Extrapolate beyond the range (ratio > 1.0 or < 0.0)
  quantity_point extrapolated = lerp(temp1, temp2, 1.5);
  std::cout << "150% extrapolation: " << extrapolated.quantity_from_zero() << "\n";

  std::cout << "\n✅ midpoint and lerp handle temperature point arithmetic correctly!\n";
}
```

**Functions**:

- `midpoint(a, b)` finds the exact middle between two points
- `lerp(a, b, ratio)` performs linear interpolation: $a + \text{ratio} \times (b - a)$
  for any ratio

**Key insight**: `midpoint` and `lerp` correctly handle affine space arithmetic for temperature
points. They compute results using the underlying absolute scale (working with differences
between points), which gives the correct answer of 15 °C when interpolating between 10 °C
and 20 °C. Without these functions, you'd need to manually compute differences and ensure
proper handling of the point vs quantity distinction.

## Real-World: Ideal Gas Law

The ideal gas law ($PV = nRT$) requires **absolute temperature**:

```cpp
// ce-embed height=650 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  // Gas law: PV = nRT
  constexpr quantity R = 8.314 * J / (mol * K);  // gas constant
  quantity n = 1.0 * mol;                        // amount of substance
  quantity V = 0.024 * m3;                       // volume
  quantity_point T_point{27 * deg_C};            // 27°C

  // Temperature MUST be absolute (Kelvin)
  quantity T = T_point.in(K).quantity_from_zero();

  // Calculate pressure
  auto P = (n * R * T) / V;

  std::cout << "Temperature: " << T << " (" << T.in(deg_C) << " above absolute zero)\n";
  std::cout << "Pressure: " << P << "\n";
  std::cout << "Pressure in kPa: " << P.in(kPa) << "\n";

  // Using Celsius directly would give WRONG results!
}
```

**Key insight**: In mp-units' absolute _temperatures_ are represented as affine space
points, and points cannot be multiplied or divided with other quantities. Use
`.quantity_from_zero()` to convert the point to an equivalent quantity (delta from absolute
zero) that supports multiplication and division. This is a limitation of the affine space
model—if absolute quantities were modeled differently, they could be used
directly in formulas like $PV = nRT$.

## Challenges

1. **Interpolation**: Find the temperature at 30% between 10 °C and 30 °C using `lerp`
2. **Heat capacity**: Implement the formula $\Delta Q = mc\Delta T$ where a 2 kg object with
   _specific heat capacity_ $c = 4.18 \, \text{kJ/(kg·K)}$ is heated from 20 °C to 80 °C.
   Calculate the _energy_ required. (Hint: subtract the _temperature_ points to get
   $\Delta T$, which can be directly multiplied—no `quantity_from_zero()` needed
   for differences)

## What You Learned?

✅ `midpoint()` finds the exact middle between two temperature points  
✅ `lerp()` performs linear interpolation and extrapolation  
✅ Celsius and Kelvin differences are equivalent in magnitude  
✅ Scientific calculations require absolute temperature (Kelvin)  

---

!!! important "Congratulations!"

    You've completed all tutorials.

**Next Steps**:

- [User's Guide](../../users_guide/index.md) - In-depth coverage of all
  library concepts
- [Workshops](../../workshops/index.md) - Practice advanced patterns
- [How-to Guides](../../how_to_guides/index.md) - Solve specific problems and tasks
- [Examples](../../examples/index.md) - Analyze complete applications
