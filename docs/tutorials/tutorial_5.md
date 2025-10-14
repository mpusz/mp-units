# Tutorial 5: Affine Spaces and Reference Origins

Many real-world measurements are relative to different reference points (origins).
This tutorial explores how to use **mp-units** to safely handle absolute and relative quantities,
using affine spaces. We'll use a real-world example: computing the _height_ of the
Hochrheinbrücke bridge over the Rhine, given different AMSL (Above Mean Sea Level) reference
levels in Germany and Switzerland.

??? question "Did you know?"

    **The Hochrheinbrücke Mishap**

    When the Hochrheinbrücke was constructed between Germany and Switzerland in 2003-2004,
    engineers had to account for different height reference systems: Germany's NHN
    (Normalhöhennull) and Switzerland's "Meter über Meer" system, which differ by 27 cm.
    This was known and planned for – the Swiss abutment was supposed to be built 27 cm
    higher to compensate. However, due to a sign error in the calculations, the Swiss
    abutment was built 27 cm **lower** instead, creating a total misalignment of 54 cm!
    Fortunately, the error was discovered during a site inspection in December 2003,
    early enough to make corrections before completion. This real-world example highlights
    the critical importance of proper reference system handling in engineering.

## Problem statement

The official AMSL reference in Germany is the Normalhöhennull (NHN).
In Switzerland, it's the "Meter über Meer" system (Pierre du Niton),
which is **27 cm lower** than NHN.

For this tutorial, let's work with a simplified scenario:

- A bridge deck is measured as 285 m above NHN (German reference)
- The river surface is measured as 278 m above the Swiss reference system

!!! Note

    These are hypothetical height measurements used for this tutorial to demonstrate
    affine space concepts - they don't represent the actual elevations of the Hochrheinbrücke.

## Your task

Given:

- Define _absolute height points_ for the bridge deck and river surface, each with their
  respective origins (NHN and Swiss reference system).
- Convert one measurement to the other's reference system, being careful to account for the
  27 cm offset that caused the famous Hochrheinbrücke construction challenge.
- Compute the _height difference_ (clearance) between the bridge and the river in meters
  ($h_\text{clearance} = alt_\text{bridge} - alt_\text{river}$).

!!! Info Note

    In this example, we use both absolute and relative point origins to represent different
    reference systems (NHN and Swiss). This approach makes conversions explicit and safe.
    For more details, see [The Affine Space](../users_guide/framework_basics/the_affine_space.md)
    in the user guide.

```cpp
// ce-embed height=650 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3"
#include <mp-units/systems/si.h>
#include <iostream>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

// TODO Define two point origins for NHN and Swiss reference system

int main()
{
  quantity_point alt_bridge = ;  // 285 m over NHN
  quantity_point alt_river = ;   // 278 m over Swiss reference
  quantity clearance = alt_bridge - alt_river;
  std::cout << "Clearance = " << clearance << "\n";
}
```

??? "Solution"

    ```cpp
    #include <mp-units/systems/si.h>
    #include <iostream>

    using namespace mp_units;
    using namespace mp_units::si::unit_symbols;

    inline constexpr struct nhn_sea_level final : absolute_point_origin<isq::altitude> {} nhn_sea_level;
    inline constexpr struct swiss_sea_level final : relative_point_origin<nhn_sea_level - 27*cm> {} swiss_sea_level;

    int main()
    {
      quantity_point alt_bridge = nhn_sea_level + 285. * m;
      quantity_point alt_river = swiss_sea_level + 278. * m;
      quantity clearance = alt_bridge - alt_river;
      std::cout << "Clearance = " << clearance << "\n";
    }
    ```

## Takeaways

- Affine spaces let you safely handle _absolute_ and _relative_ measurements with
  different origins.
- Always convert to a common reference before comparing or subtracting _points_.
- Using absolute and relative point origins in your code helps make reference system
  conversions explicit and less error-prone.
  See the [user guide](../users_guide/framework_basics/the_affine_space.md) for more on
  this feature.
- The Swiss reference (Pierre du Niton) is about 27 cm **lower** than the German NHN.
- Always check the direction of such offsets!
- [Reference: Wikipedia – Normalhöhennull (NHN)](https://en.wikipedia.org/wiki/Normalh%C3%B6hennull)
