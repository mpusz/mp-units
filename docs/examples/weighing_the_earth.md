---
tags:
- Level - Intermediate
- System - SI
- System - ISQ
- System - IAU
- Feature - Custom Representation
- Feature - Faster-Than-Lightspeed Constants
- Feature - Unit Conversions
- Domain - Physics
- Domain - Metrology
---

# Weighing the Earth

## Overview

This example determines the mass of the Earth from two benchtop measurements, the way the
quantity was first obtained after Cavendish measured `G`. A pendulum is timed to get the
local gravitational acceleration, which is then combined with the Earth's radius and the
gravitational constant:

$$g = \frac{4\pi^2 L}{T^2} \qquad M = \frac{g R^2}{G}$$

Every input carries a measurement uncertainty, `G` carries the one CODATA publishes for it,
and the result reports what all of them together imply. The representation type doing the
work is [`uncertain<T>`](../how_to_guides/advanced_usage/working_with_measurement_uncertainty.md),
which propagates uncertainty through arithmetic automatically.

## Measuring the local gravity

A pendulum is timed over many swings so that the reaction-time error is divided by their
count. Solving $T = 2\pi\sqrt{L/g}$ for the acceleration gives the first stage:

```cpp title="weighing_the_earth.cpp"
--8<-- "example/weighing_the_earth.cpp:61:72"
```

The `π` here is a
[symbolic constant](../users_guide/framework_basics/faster_than_lightspeed_constants.md),
so it is exact and contributes nothing to the uncertainty of the result. Only the two
measurements do.

Note the quantity being defined. `isq::acceleration` is a *vector* quantity in ISO 80000-3,
just as `isq::velocity` is, while a pendulum measures the *magnitude* of the local free-fall
acceleration, which is a scalar. ISQ names no scalar counterpart to `acceleration` the way
`speed` is the one for `velocity`, and the magnitude of a vector quantity is not modelled
yet (it is planned for V3), so the example defines what it means. Being honest about the
character is not pedantry here: it is what lets the second stage reduce to `isq::mass` on
its own. Had `g` been left a vector, the framework would have refused to convert the result
to a scalar quantity, and rightly so.

## Weighing the planet

The second stage asks what mass a sphere must have to produce the observed surface gravity.
`G` is a *measured* constant, so it is materialized with
[`measurement_of`](../how_to_guides/advanced_usage/working_with_measurement_uncertainty.md#measurement_of)
rather than used as a bare unit, and its CODATA uncertainty joins the propagation:

```cpp title="weighing_the_earth.cpp"
--8<-- "example/weighing_the_earth.cpp:74:79"
```

## Running the experiment

First the measured inputs, each with the uncertainty its instrument justifies:

```cpp title="weighing_the_earth.cpp"
--8<-- "example/weighing_the_earth.cpp:86:92"
```

The two _lengths_ are given strong quantity types, and that is typing which buys something.
Both are _lengths_, so nothing dimensional would stop them being passed in each other's
place, but `isq::distance` and `isq::radius` sit in different branches of the ISQ
hierarchy (under `path_length` and `width` respectively), so each is rejected where the
other is expected. The _duration_ needs no such help, since nothing else in the call is a
_duration_. A caller who does not want typed quantities is not forced into them either: a
plain `6371.0 * km` carries `kind_of<isq::length>`, which converts down and satisfies
either constraint.

Then the two stages run and the results are printed against their published counterparts:

```cpp title="weighing_the_earth.cpp"
--8<-- "example/weighing_the_earth.cpp:94:106"
```

Finally the uncertainty budget, which is where the interesting part is. Each input
contributes its own relative uncertainty scaled by the exponent it enters the formula with,
and the contributions are combined by hand so that the total can be compared against the one
the library propagated:

```cpp title="weighing_the_earth.cpp"
--8<-- "example/weighing_the_earth.cpp:108:125"
```

```text
pendulum length     = 1 ± 5e-04 m
time for 50 swings  = 100.3 ± 0.06 s
local gravity       = 9.8107 ± 0.0127 m/s²
  standard gravity  = 9.8066 m/s²
mass of the Earth   = 5.9663e+24 ± 7.7399e+21 kg
  accepted value    = 5.9722e+24 ± 1.3422e+20 kg

relative uncertainty contributions
  pendulum length        = 5.0e-04
  period (squared)       = 1.2e-03
  Earth radius (sq.)     = 3.1e-05
  G (CODATA 2018)        = 2.2e-05
  combined in quadrature = 1.297e-03
  reported by the result = 1.297e-03
```

## What the numbers say

**The two values agree.** Everything needed is in the two mass lines printed above:

| From the output                     | Value                            |
|-------------------------------------|----------------------------------|
| this experiment                     | 5.9663 × 10²⁴ ± 7.7399 × 10²¹ kg |
| published (`iau::terrestrial_mass`) | 5.9722 × 10²⁴ ± 1.3422 × 10²⁰ kg |

The gap between the two central values is 0.099% of the published mass:

$$\Delta = 5.9722 \times 10^{24}\,\mathrm{kg} - 5.9663 \times 10^{24}\,\mathrm{kg}
         = 5.9 \times 10^{21}\,\mathrm{kg}$$

Because *both* figures carry an uncertainty, the comparison uses their combination rather
than either alone:

$$u_\text{combined} = \sqrt{\left(7.7399 \times 10^{21}\right)^2
                           + \left(1.3422 \times 10^{20}\right)^2}
                    = 7.74 \times 10^{21}\,\mathrm{kg}$$

The gap is therefore well inside one combined standard uncertainty:

$$\frac{\Delta}{u_\text{combined}} = 0.76$$

so the two results are consistent. The published uncertainty is about sixty times smaller
than the experiment's and barely moves that number, but it is included rather than assumed
away. This is the question an uncertainty exists to answer: not "what number did I get" but
"is the difference from the expected one larger than what my instruments can resolve".

Note where those two uncertainties come from. `iau::terrestrial_mass` is defined as
`(GM)⊕ᴺ/G`, so its value *in kilograms* is known only as well as `G` is, and the example
opts into reporting that with `.in<uncertain<double>>(kg)`. The standard free-fall
acceleration, by contrast, prints without a `±`, because `codata::standard_gravity` is an
exact *conventional* value fixed by the CGPM rather than a measured one. It is a reference point,
not the true local value: real free-fall acceleration ranges from about 9.78 m/s² at the
equator to 9.83 m/s² at the poles, a spread far wider than this experiment's uncertainty.

**The budget reconciles with the result.** The contributions are independent, so they
combine in *quadrature* rather than by addition: adding the four numbers would give
1.75 × 10⁻³, while combining them properly gives 1.297 × 10⁻³, exactly what the propagation
through the two functions reported. The example prints both so the agreement can be checked
rather than taken on trust.

**The stopwatch dominates the budget.** The period enters squared, so its relative
uncertainty counts double, which puts it above the metre stick even though both readings
look similarly careful. That is the practical lesson of an uncertainty budget: improving the
_radius_ figure or waiting for a better value of `G` would change nothing here, while timing
more swings would.

**`G` is negligible, and that is a finding.** The least precisely known constant in physics
contributes 2.2 × 10⁻⁵, some sixty times below the total. Being able to see that, rather than
assume it, is why the constant carries its published uncertainty in its definition.

!!! warning "The agreement is partly luck, and the numbers show it"

    Everything above is *random* uncertainty. The model carries a systematic error too:
    treating the Earth as a uniform sphere with its mean radius ignores oblateness and
    rotation. That bias can be measured. Putting the exact standard gravity and the same mean
    radius through the same formula gives 5.9639 × 10²⁴ kg, which is 0.139% below the
    published mass.

    Compare that with the experiment's own precision. The printed
    `reported by the result = 1.297e-03` is 0.130% relative, so **the modelling bias is
    larger than the random uncertainty**. The result landed inside its error bar only because
    the measured `g` came out slightly above standard, which pulled the answer up to 0.099%
    low instead of 0.139%.

    The consequence is worth sitting with: time more swings and the random uncertainty
    shrinks while the 0.139% bias does not, so a *more precise* experiment would start to
    disagree with the accepted value while looking more confident. No propagation formula
    can warn about this, which is why the
    [assumptions and limitations](../how_to_guides/advanced_usage/working_with_measurement_uncertainty.md#uncertainty-propagation)
    of `uncertain<T>` are worth reading before trusting a narrow interval.
