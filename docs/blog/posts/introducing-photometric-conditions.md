---
date: 2026-08-12
authors:
 - mpusz
categories:
 - Metrology
comments: true
---

# Introducing Photometric Conditions

A "1000 lm" LED lamp and a "1000 lm" low-pressure sodium lamp do not look equally bright at
night. The number on both boxes is a _photopic_ value: it weights the lamp's spectrum with
$V(\lambda)$, the spectral sensitivity of the eye's cone cells, which drive vision in
daylight. At night, rod cells take over, the sensitivity curve shifts towards blue
($V'(\lambda)$), and the same two lamps deliver roughly 2000 and 250 _scotopic_ lumens
respectively. Both figures are luminous fluxes. Both are expressed in lumens. And a program
that adds or compares them computes a result that is wrong by up to a factor of eight:

```cpp
auto led = 1000. * lm;     // photopic value from the datasheet
auto sodium = 250. * lm;   // scotopic value from a night-visibility model
auto total = led + sodium; // compiles everywhere, means nothing
```

To the best of our knowledge, no units library catches this today, because every one of them
(including **mp-units** until now) models a lumen as a lumen. This post introduces
_photometric conditions_: a way to keep photopic, scotopic, mesopic, and custom quantities
in separate quantity hierarchies, so that the mistake above breaks at compile time, while
everything the physics does allow keeps working.

<!-- more -->

!!! note

    The snippet above is not **mp-units** code. It shows units libraries accept today,
    where the type system has no way to state that `sodium` is a scotopic value.
    Written in **mp-units** with the condition spelled out, the sum does not compile, as
    the rest of this post shows.

## What ISO 80000-7 actually says

The International System of Quantities is precise here, and the design follows it closely.
ISO 80000-7:2019 defines the luminous quantities (items 7-10 to 7-18) for _photopic vision_
and states in clauses 0.4 and 0.5 that the _scotopic_ and _mesopic_ counterparts are "defined
in the same manner", with the quantity symbols distinguished by a prime
($\Phi'_\mathrm{v}$) or a subscript ($L_\mathrm{mes;m}$). Three details matter for the type
system:

1. **The quantities differ, the units do not.** Lumens, candelas, and lux serve every
   condition, because the candela's defining constant (683 lm/W at 540 THz) anchors all the
   weighting functions at one common frequency. The standard derives the scotopic maximum
   luminous efficacy from the same definition:
   $K'_\mathrm{m} = 683\,\mathrm{lm\,W^{-1}}/V'(\lambda_\mathrm{cd}) \approx 1700$ lm/W.
2. **There is no conversion factor.** The ratio between a photopic and a scotopic value of
   the same source depends on the source's spectrum. Lighting engineers publish it per lamp
   type as the S/P ratio, roughly 0.25 for low-pressure sodium and 2.0 for a cool-white LED.
3. **The set of conditions is open.** Mesopic weightings $V_\mathrm{mes;m}(\lambda)$ depend
   on a continuous adaptation level, the standard also names the CIE 10° and CIE 1988
   observers, and CIE S 026 adds five more retinal weightings. "Melanopic lux" is now a
   standard figure in circadian lighting design.

If it is not specified, photopic vision is assumed. The library mirrors that rule exactly.

## One definition, one kind tree per condition

Every luminous quantity is defined once, and the photometric condition is a template
argument that propagates through the whole derived tree:

```cpp
inline constexpr struct photopic_vision final : photometric_condition_base {} photopic_vision;
inline constexpr struct scotopic_vision final : photometric_condition_base {} scotopic_vision;
template<auto AdaptationLevel> struct mesopic_vision_ final : photometric_condition_base {};
template<auto AdaptationLevel> constexpr mesopic_vision_<AdaptationLevel> mesopic_vision;
```

Each condition roots its own quantity hierarchy over the same base dimension. The plain names
(`isq::luminous_flux`, `si::lumen`) are the photopic entities, so all existing code works
unchanged, and the other conditions are spelled with `_of`:

```cpp
quantity daylight = 1000. * si::lumen;
quantity night = 250. * si::lumen_of<isq::scotopic_vision>;

// auto total = daylight + night;                             // Compile-time error
// if (daylight > night) { /* ... */ }                        // Compile-time error
// auto x = daylight.in(si::lumen_of<isq::scotopic_vision>);  // Compile-time error
quantity sp_ratio = night / daylight;                         // OK, 0.25 lm/lm
```

The rationale behind this is that:

- quantities weighted with different sensitivity functions are different quantities, so they
  must form different kinds, exactly like the standard's primed symbols suggest,
- no cast can exist, not even an explicit one, because the conversion factor is not a number
  the library could know, it is the spectrum of a concrete light source,
- products and quotients across conditions remain well-formed, because they are physically
  meaningful (the S/P ratio above is the standard figure of merit for outdoor lighting).

A user models a custom weighting by deriving one tag. The entire tree of quantities and units
becomes available for it, with the same guarantees:

```cpp
inline constexpr struct melanopic final : mp_units::isq::photometric_condition_base {} melanopic;

quantity mel_edi = 250. * si::lux_of<melanopic>;
```

## One lumen, many associations

SI defines exactly one candela, lumen, and lux, so what is `si::lumen_of<C>`? It is not a
different unit of measurement. It prints as `lm`, its conversion factor to `si::lumen` is
exactly one, and its photopic instantiation is `si::lumen` itself. The condition argument
only states which quantity kind the unit is associated with.

This association is what preserves quantity-kind safety in simple mode, where the unit is
the only carrier of the quantity semantics. It is why every example in this post can use
plain units without spelling any quantity spec. Without it, a scotopic value created in
simple mode would be indistinguishable from a photopic one, and the safety story would end
at the first `quantity q = 250. * lm;`.

## The unit ratio is information

In a quotient of two conditions the lumens do not cancel symbolically, so the S/P ratio
above comes out as `0.25 lm/lm`. This is not a limitation, it is the form worth keeping. The
unit ratio records that quantities of two different kinds were divided, the same way the SI
Brochure keeps unit ratios like mg/kg on dimensionless quantities to convey information
about the quantities involved. Such a ratio behaves like a plain number in arithmetic
anyway, so reducing it with `.in(one)` belongs where a bare figure is wanted, typically in
text output.

It also makes the ratio a working conversion factor. Model the datasheet S/P value with its
unit ratio, and converting a photopic measurement is a plain multiplication in which the
photopic lumens cancel:

```cpp
quantity sp_ratio = 2.0 * si::lumen_of<isq::scotopic_vision> /
                    si::lumen_of<isq::photopic_vision>;  // cool-white LED

quantity led = 1000. * si::lumen;
quantity scotopic = sp_ratio * led;  // 2000 lm under scotopic vision
```

The result comes out directly in `si::lumen_of<isq::scotopic_vision>`. Multiplied by a
quantity of any other condition, the units do not simplify, and the mismatch stays visible
in the resulting type and output.

The ratio carries its direction as well, which catches a mistake that is easy to make and
hard to see in a review. An S/P value entered in the wrong order is still a plausible-looking
number, but it makes the multiplication produce `lm²/lm` of the wrong kinds instead of a
scotopic flux, so any constrained destination rejects it:

```cpp
void take_scotopic_flux(QuantityOf<isq::luminous_flux_of<isq::scotopic_vision>> auto flux);

quantity inverted = 2.0 * si::lumen_of<isq::photopic_vision> /
                    si::lumen_of<isq::scotopic_vision>;  // wrong order
take_scotopic_flux(sp_ratio * led);     // OK
// take_scotopic_flux(inverted * led);  // Compile-time error
```

The same applies to a typed variable or a function return type. Nothing here needed a
hand-written check: the quantity kinds of the two lumens do the work.

## Subkinds inside one condition

Separating the conditions is only half of the safety. Within a single condition, ISO 80000-7
names two different quantities "luminous efficacy": *of radiation* (item 7-11.1,
$K = \Phi_\mathrm{v} / \Phi_\mathrm{e}$) and *of a source* (item 7-11.4,
$\eta_\mathrm{v} = \Phi_\mathrm{v} / P$, against the power the source consumes). Both are
lm/W. A lamp quoted at 400 lm/W of radiation and 120 lm/W as a device is not contradicting
itself, it is reporting two quantities, and code that adds or compares them is wrong.

We model them the way the library already models *fluid head* and *water head*: distinct
subkinds of a common parent, sharing its unit but isolated from each other, with the physics
of the crossing written out as a function.

```cpp
constexpr QuantityOf<isq::luminous_efficacy_of_source> auto to_source_efficacy(
  QuantityOf<isq::luminous_efficacy_of_radiation> auto k, QuantityOf<dimensionless> auto radiant_efficiency)
{
  return isq::luminous_efficacy_of_source(isq::luminous_efficacy(k) * radiant_efficiency);
}
```

The same reasoning applies elsewhere in part 7. A *photon number* counts photons, so it is
a subkind of *dimensionless* rather than a plain ratio (the treatment
`isq::storage_capacity` already receives) and cannot be added to a *reflectance*.
Conversely, *absorptance*, *reflectance*, and *transmittance* must stay freely mixable,
because $\alpha + \rho + \tau = 1$ is a
physical identity, and a *linear absorption coefficient* is a part of the corresponding
*linear attenuation coefficient* (which covers absorption and scattering), so those two add
and compare as members of one hierarchy.

## What we do not guard

Honesty about the boundaries:

- **Dimensionless luminous ratios.** _Luminous reflectance_, _luminous transmittance_, and
  _luminance factor_ are quotients of two quantities of the same condition, so their
  condition trees collapse into the common dimensionless kind. They stay distinct quantity
  specs with no implicit conversion between conditions, but they add and compare through
  `dimensionless`, like every other efficiency or factor in the library.
- **The mesopic bridge.** The CIE recommended system computes a mesopic luminance from the
  photopic and the scotopic luminances with empirical coefficients and an iteration over the
  adaptation level. That combination is not a sum, so it does not compile as one. It is user
  code, and the coefficients stay visible on the page.
- **Absorbance.** Items 7-32.1 and 7-32.2 are logarithmic quantities and await the
  [logarithmic-quantities framework](introducing-logarithmic-quantities.md).
- **Colorimetry.** Tristimulus values, colour-matching functions, and chromaticity
  coordinates (items 7-26 to 7-28) are triples of tabulated functions rather than scalar
  quantities of a single kind, so they are not modeled as quantity specs.

## Where this lands

The feature ships with the complete ISO 80000-7 coverage: the full luminous block, the
photon quantities, colour temperatures, and the material properties, next to the radiometric
quantities that were already there. The
[ISQ user's guide](../../users_guide/systems/isq.md#photometric-conditions) describes the
model, and the
[how-to guide](../../how_to_guides/advanced_usage/photometric_conditions.md) provides the
recipes.

We are not aware of another units library that models photometric conditions in the type
system, which is precisely why we want scrutiny. If you design lighting, work in photometry
or colorimetry, or maintain code that mixes radiometric and photometric data, we would love
to hear where this design helps you and where it gets in your way. Please share your
feedback in the comments below.
