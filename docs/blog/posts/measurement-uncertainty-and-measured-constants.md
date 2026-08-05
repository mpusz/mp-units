---
date: 2026-08-05
authors:
 - mpusz
categories:
 - Metrology
comments: true
draft: true
---

# Measurement uncertainty and measured constants

When you write `constexpr double G = 6.674e-11;` in your code, the type system makes a claim
that is not true. It claims the value is exact. It is not. The Newtonian constant of
gravitation is one of the least precisely known constants in physics. CODATA 2018 lists it
as `6.674 30(15) × 10⁻¹¹ m³ kg⁻¹ s⁻²`, and the `(15)` means the fifth significant digit is
already uncertain. Only four digits of `G` are actually known. Every result derived from
that `double` inherits an uncertainty that the program neither tracks nor reports. The code
happily prints ten significant digits of a solar mass computed from a constant that
guarantees four.

**mp-units** now models this honestly. This post introduces the `uncertain<T>` representation
type, the `relative_standard_uncertainty` metadata for measured constants, and the
`measurement_of` helper that connects the two.

<!-- more -->

## Constants are units, and that is not going to change

Since the beginning of the V2 design, physical constants in **mp-units** are units. This is
the [Faster-than-lightspeed constants](../../users_guide/framework_basics/faster_than_lightspeed_constants.md)
feature. The central value of a constant lives in the unit's exact symbolic magnitude, so:

- constants cancel symbolically with zero runtime cost and zero precision loss,
- no representation type is forced on the user (`1 * G` works with `int`, `double`, or
  anything else),
- the conversion cost is paid only when (and if) the user asks for a specific unit.

It turns out this design also matches metrological practice. The IAU 2015 Resolution B3
nominal solar and planetary constants are *exact by definition* precisely so that they can
serve as conversion units. The seven defining constants of the 2019 SI are exact as well.
For all of those, an exact unit is not an approximation of the truth. It is the truth.

The gap was elsewhere. Some constants are *measured*, not defined. `G`, the particle masses
in the HEP system, the fine-structure constant. For those, the CODATA table publishes two
numbers: the central value and the relative standard uncertainty. The unit magnitude stores
the first one. The second one had nowhere to live. This gap was first reported in
[#464](https://github.com/mpusz/mp-units/issues/464), and
[Ralph Steinhagen](https://github.com/RalphSteinhagen) explored a full uncertainty-propagation
type in that discussion back in 2023.

## Where should uncertainty live?

We considered three options.

**Inside unit magnitudes?** No. Unit equality compares canonical magnitudes, and a
conversion factor is a ratio of them, so both are arithmetic on magnitudes and both depend
on the operands being exact numbers. A "unit of G ± σ" could not produce a well-defined
conversion factor or a decidable equality. This is not a limitation to lift someday.
Exactness is what makes constants-as-units work at all.

**Constants as quantities?** Also no. A quantity constant like
`inline constexpr quantity G = 6.674e-11 * si::metre * ...;` forces a representation type
on every user. A `float` codebase would silently promote to `double`. It also gives up exact
cancellation.

**Declared in the constant, tracked by the representation.** This is the one, and it is not
a single place. A measured constant's definition carries both numbers CODATA publishes: the
central value becomes the unit's exact symbolic magnitude, and the relative standard
uncertainty sits right next to it as metadata. The *magnitude* stays exact, which is what
keeps unit equality and conversion-factor arithmetic working. The *constant* does not claim
to be exact anymore: its definition now states openly that the value is measured, and how
well. Tracking is a separate job with a separate home. A dedicated representation type carries
uncertainty through arithmetic, and the conversion engine materializes the declared one
whenever a conversion factor depends on the constant. Each number lands in the layer that
can handle it correctly.

## `uncertain<T>`

The `mp_units::utility` component now provides `uncertain<T>`, a representation type that
pairs a central value with its standard uncertainty and propagates uncertainty through
arithmetic using the standard first-order formulas:

```cpp
#include <mp-units/systems/si.h>
#include <mp-units/utility/uncertain.h>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using mp_units::utility::uncertain;

quantity length = uncertain{10.0, 0.1} * m;   // 10.0 ± 0.1 m
quantity width = uncertain{5.0, 0.05} * m;    // 5.0 ± 0.05 m
quantity area = length * width;               // 50.0 ± 0.71 m²
```

No customization is needed. `uncertain<T>` simply satisfies the `RepresentationOf` concept,
so it works with every quantity, every unit conversion, and every math function that its
underlying type supports. An exact quantity joins the propagation implicitly as `value ± 0`,
so mixing `uncertain` and plain representations in one expression works as expected.

The name follows the VIM (International Vocabulary of Metrology) terminology on purpose.
A *measurement* is the process, and a measurement result is a value with an uncertainty
*and a unit*. A bare `{value, σ}` pair with no unit attached is neither. It is just an
uncertain number, so the type is called `uncertain`, and it follows the same
adjective-wrapper naming pattern as `std::optional<T>` and `std::expected<T>`.

!!! important "Scope"

    `uncertain<T>` implements first-order propagation for statistically independent values.
    It does not track correlations, so `x - x` reports a non-zero uncertainty. Full rigor
    for correlated values requires covariance matrices, and Ralph's prototype in
    [#464](https://github.com/mpusz/mp-units/issues/464) shows how quickly that grows.
    We chose the 80/20 boundary and documented it. If your problem needs correlation
    tracking, a dedicated uncertainty library is the right tool.

## Measured constants carry their uncertainty

A measured constant now declares its relative standard uncertainty right in its definition,
as an exact symbolic magnitude, using the exact term the CODATA table uses:

```cpp
inline constexpr struct newtonian_constant_of_gravitation final :
    named_constant<"G", mag_ratio<667'430, 100'000> * mag_power<10, -11> * cubic(si::metre) / si::kilogram / square(si::second),
                   relative_standard_uncertainty{mag_ratio<22, 10> * mag_power<10, -5>}> {}
newtonian_constant_of_gravitation;
```

The definition reads like the CODATA entry it transcribes: the value `6.674 30 × 10⁻¹¹`, and
the relative standard uncertainty `2.2 × 10⁻⁵`. No representation type appears anywhere. The
metadata is an annotation only. It never participates in unit equality, conversion factors,
or symbolic simplification, so nothing about the existing constant behavior changes.

Exact constants simply do not have this parameter. This makes exact-vs-measured a
distinction the type system can see, expressed by the new `MeasuredConstant` concept.
Previously that distinction lived only in comments.

## Conversions materialize the uncertainty

Working through the design surfaced a fact that shaped everything else: the uncertainty of
a measured constant is *relational*. In its own unit the constant is exactly `1`. "Two
solar masses" is an exact statement even though `iau::solar_mass` is defined through `G`,
and it stays exact in an uncertainty-capable representation too: `uncertain<double>{2.0} *
iau::unit_symbols::M_SUN` prints `2 ± 0 M_☉`. What is uncertain is the *conversion factor*
between
such a unit and others: how many kilograms a solar mass really is depends on how well `G`
is known.

This is why the conversion engine is the place where a constant's declared uncertainty
becomes part of a value. When a conversion's factor is built from measured constants and
the representation type can store an uncertainty, the factor's relative standard
uncertainty is folded into the result in quadrature. For every other representation type
nothing changes:

```cpp
const quantity two_suns = 2.0 * iau::unit_symbols::M_SUN;

std::cout << two_suns << "\n";                                     // 2 M_☉
std::cout << two_suns.in(kg) << "\n";                              // 3.97682e+30 kg
std::cout << two_suns.in<uncertain<double>>(kg) << "\n";           // 3.97682e+30 ± 8.749e+25 kg
std::cout << value_cast<kg, uncertain<double>>(two_suns) << "\n";  // 3.97682e+30 ± 8.749e+25 kg
```

The first two lines are exact: `two_suns` in its own unit, and the sanctioned central
value in kilograms. The third opts into the uncertainty at the conversion, and the
fourth spells the same opt-in as a cast.

The derivation walks both units' definitions and accumulates the net exponent of every
measured constant, so a constant contributing to both sides cancels symbolically, exactly
as its central value cancels in the magnitude ratio. Here are the same two suns again,
with the uncertainty-capable representation as the only difference:

```cpp
const quantity two_suns = uncertain<double>{2.0} * iau::unit_symbols::M_SUN;

std::cout << two_suns << "\n";                                 // 2 ± 0 M_☉
std::cout << two_suns.in(iau::unit_symbols::M_EARTH) << "\n";  // 665892 ± 0 M_⊕
std::cout << two_suns.in(kg) << "\n";                          // 3.97682e+30 ± 8.749e+25 kg
```

`two_suns` is exactly two suns even in an uncertainty-capable representation. Both masses
are defined as `(GM)ᴺ/G`, so `G` cancels in the conversion to Earth masses and the reported
uncertainty stays exactly zero there too. Only in kilograms does it survive, and since the
representation type can hold it, no explicit opt-in is needed anymore.

That cancellation is something no value-level scheme can get right. Once an uncertainty is
baked into a number, propagating it through a solar-to-Earth-mass conversion double-counts
what the symbolic form knows how to cancel.

## `measurement_of`

`measurement_of` is a small factory function built on the same rule. Its argument is
constrained to the `MeasuredConstant` concept, so it accepts exactly the constants that
declare a relative standard uncertainty and rejects everything else at compile time. For
such a constant it returns the quantity `1` in an uncertainty-capable representation, and
the uncertainty appears the moment the quantity leaves units in which the constant
cancels. The underlying numeric type defaults to `double`, consistent with the rest of the
library, and any other supported one can be selected explicitly, so a `float` codebase
never touches a `double`:

```cpp
const quantity G_measured = measurement_of(iau::newtonian_constant_of_gravitation);
const quantity G_single = measurement_of<float>(iau::newtonian_constant_of_gravitation);
static_assert(std::is_same_v<decltype(G_measured)::rep, uncertain<double>>);
static_assert(std::is_same_v<decltype(G_single)::rep, uncertain<float>>);

std::cout << "G = " << G_measured.in(m3 / kg / s2) << "\n";

const quantity solar_mass = (1. * iau::nominal_solar_mass_parameter) / G_measured;
std::cout << "M_sun = " << solar_mass.in(kg) << "\n";
```

```text
G = 6.6743e-11 ± 1.46835e-15 m³ kg⁻¹ s⁻²
M_sun = 1.98841e+30 ± 4.3745e+25 kg
```

The solar mass example is the payoff. The IAU defines the nominal solar mass parameter
`(GM)☉ᴺ` as exact, so the entire uncertainty of the derived solar mass comes from `G`, and
the result correctly reports it: `1.98841 × 10³⁰ kg`, known precisely as well as `G` itself
and no better, which is what the IAU intended when it switched to nominal values in 2015.

This is where that `MeasuredConstant` constraint earns its keep. Asking for the measurement
of a constant that is exact by definition does not compile:

```cpp
quantity c = measurement_of(si::si2019::speed_of_light_in_vacuum);  // constraint not satisfied
```

The rationale behind this is that:

- "exact by definition" and "measured infinitely precisely" are different metrological
  statements, and returning `±0` would conflate them,
- asking for the *measurement* of a defined constant is a category error, and we prefer to
  break at compile time, not at runtime.

## Limits

**Cross-constant independence is assumed.** The quadrature sum `u_r = √(Σ (nᵢ · u_r,ᵢ)²)`
treats the measured constants surviving in a conversion factor as independent. CODATA
publishes covariances between its values, and modeling them is deliberately out of scope.
It is the same boundary `uncertain<T>` itself draws, for the same reason: doing correlation
properly means carrying a covariance matrix, and that is a different library. When several
correlated constants do survive into one factor, the reported uncertainty can err in either
direction, depending on the signs of their exponents and of the correlation between them.

The case where correlation matters most is not approximated at all. A constant appearing in
both units is fully correlated with itself, and it cancels symbolically before the formula
is ever applied, which is exactly why the solar-to-Earth-mass conversion reports zero rather
than a small non-zero number.

**Coverage.** Every measured constant in the three HEP CODATA namespaces carries the
uncertainty published by its own release, as do the SI-expressed ones in the new
[`codata`](../../users_guide/systems/codata.md) system, which `iau::G` now imports rather
than duplicating. Constants that are exact by definition carry nothing at all, which is
what makes the distinction visible to the type system in the first place. Filling out the
rest of the CODATA table is tracked in
[#820](https://github.com/mpusz/mp-units/issues/820).

## Printing what the standard prints

The default text output is the `value ± σ` form, the notation engineers read every day:

```cpp
std::cout << G.in(m3 / kg / s2) << "\n";  // 6.6743e-11 ± 1.46835e-15 m³ kg⁻¹ s⁻²
```

ISO 80000-1:2022, 7.2.4 specifies a different one. The value is quoted to the last
significant digit of the uncertainty, and the uncertainty follows in parentheses, counted
in units of that digit. Appending `~` to the format spec selects it. The tilde marks the
output as an approximation, because unlike the default form this one rounds the value:

```cpp
std::println("{::N[~]}", G.in(m3 / kg / s2));  // 6.67430(15)e-11 m³ kg⁻¹ s⁻²
```

That is character for character how CODATA publishes the value, which is the point. A
constant transcribed from a published table now prints back in the notation of the table it
came from.

The standard is also not neutral between the two forms:

!!! quote "ISO 80000-1:2022, 7.2.4"

    Uncertainties are often expressed in the following manner: $(23{,}478\,2 \pm
    0{,}003\,2)\ \mathrm{m}$. This is, however, wrong from a mathematical point of view.
    [...] "The ± format should be avoided whenever possible because it has traditionally
    been used to indicate an interval corresponding to a high level of confidence and thus
    may be confused with expanded uncertainty".

The `±` form nevertheless remains the default, because the concise form has to round the
value to the precision of the uncertainty, and a default that silently discards digits is
the wrong default. The recommendation applies to published results, and that is exactly
where reaching for `~` costs one character.

## Further Reading

This post covers the design and the reasoning behind it. The working details live in the
documentation:

- [Working with Measurement Uncertainty](../../how_to_guides/advanced_usage/working_with_measurement_uncertainty.md)
  is the how-to guide: the observers `uncertain<T>` exposes, the full table of propagation
  formulas including the trigonometric ones, what its ordering does and does not mean,
  which element types it accepts, and the measured-constant workflow step by step
- [Weighing the Earth](../../examples/weighing_the_earth.md) is a complete program using
  it: the mass of the planet from a pendulum, a radius, and `G`
- [Faster-than-lightspeed constants](../../users_guide/framework_basics/faster_than_lightspeed_constants.md)
  explains why constants are units in the first place
- [CODATA fundamental physical constants](https://physics.nist.gov/cuu/Constants/) are the
  source of the values and uncertainties transcribed into the systems
- [JCGM 100 (GUM)](https://www.bipm.org/en/committees/jc/jcgm/publications) is the
  authority on how uncertainty is expressed and propagated

## Acknowledgments

Credits go to [Ralph Steinhagen](https://github.com/RalphSteinhagen) for the prototype and
the discussion that shaped the scope, and to the ISO/IEC 80000 and CODATA tables for having
the answer ready every time we wondered what the right behavior should be.

Please share your feedback in the comments below.
