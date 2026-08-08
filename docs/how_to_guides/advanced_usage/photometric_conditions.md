# Working with Photometric Conditions

Photometric quantities weighted with different spectral sensitivity functions (photopic,
scotopic, mesopic, or custom ones) share the same units but must never be added, compared,
or converted to each other. **mp-units** keeps each photometric condition in its own quantity
hierarchy, so mixing them does not compile. The rationale and the full description of the
model live in the
[ISQ user's guide](../../users_guide/systems/isq.md#photometric-conditions). This page shows
the recipes.

## Express a measurement under a specific condition

An unspecified condition means photopic vision, so the plain units work as they always did.
For any other condition, spell it with `_of` on the unit:

```cpp
using namespace mp_units;

quantity daylight = 1000. * si::lumen;
quantity night = 250. * si::lumen_of<isq::scotopic_vision>;
```

The condition argument on the unit does not create a different unit of measurement. It only
states which quantity kind the SI lumen is being associated with. This is also why simple
quantities like the ones above are enough to keep the conditions apart: the unit alone
carries the condition.

Photopic vision is the default, so the photopic instantiation of each unit is the SI unit
itself, and the two spellings are interchangeable:

```cpp
static_assert(si::lumen_of<isq::photopic_vision> == si::lumen);
static_assert(si::candela_of<isq::photopic_vision> == si::candela);
static_assert(si::lux_of<isq::photopic_vision> == si::lux);
```

Use whichever reads better. `si::lumen` is the natural choice in code that deals with one
condition, while the explicit `si::lumen_of<photopic_vision>` pays off next to another
condition, where naming both sides documents the intent.

Spelling the quantity spec starts to pay off when a condition's hierarchy holds more than
one quantity, because then the unit no longer identifies the quantity on its own. Both
_maximum luminous efficacy_ and _spectral luminous efficacy_ are lm/W, so only the typed
form tells them apart:

```cpp
void take_maximum_efficacy(QuantityOf<isq::maximum_luminous_efficacy> auto);

quantity k_m = isq::maximum_luminous_efficacy(683. * si::lumen / si::watt);
quantity k_lambda = isq::spectral_luminous_efficacy(500. * si::lumen / si::watt);
take_maximum_efficacy(k_m);                          // OK
// take_maximum_efficacy(k_lambda);                  // Compile-time error
take_maximum_efficacy(683. * si::lumen / si::watt);  // OK, a simple quantity fits any quantity of its kind
```

Derived quantities work within each condition as usual:

```cpp
quantity flux = 250. * si::lumen_of<isq::scotopic_vision>;
quantity area = 100. * square(si::metre);
quantity illum = (flux / area).in(si::lux_of<isq::scotopic_vision>);   // scotopic illuminance
```

## Compute the S/P ratio

Quotients across conditions are well-formed:

```cpp
quantity sp_ratio = night / daylight;  // 0.25 lm/lm
```

Keep this form. The `lm/lm` unit ratio records which two quantities were divided, and the
value already behaves like a plain number in arithmetic, so nothing is gained by reducing
it early. Convert to a bare figure only where a unit one is wanted, typically in text
output:

```cpp
std::cout << sp_ratio.in(one) << "\n";  // 0.25
```

## Convert between conditions with a known S/P ratio

There is no spectrum-independent conversion between conditions, so the library provides no
cast, not even an explicit one. Crossing requires the S/P ratio of the specific light source,
which lamp datasheets and lighting standards publish per lamp type. Model that ratio as a
quantity that keeps its unit ratio, and the conversion becomes a plain multiplication:

```cpp
// the units of the ratio carry the physics of the crossing
quantity sp_ratio = 2.0 * si::lumen_of<isq::scotopic_vision> / si::lumen_of<isq::photopic_vision>;
quantity led = 1000. * si::lumen;
quantity scotopic = sp_ratio * led;  // 2000 lm under scotopic vision
```

Because the ratio preserves its category, the photopic lumens cancel and the result comes
out directly in `si::lumen_of<scotopic_vision>`. Multiplying it by a quantity of any other
condition does not simplify, and the mismatch stays visible in the resulting type and
output.

This also catches a ratio entered in the wrong order. Constrain the destination, and an
inverted S/P value cannot pass, because the multiplication yields `lm²/lm` of the wrong
kinds rather than a scotopic flux:

```cpp
void take_scotopic_flux(QuantityOf<isq::luminous_flux_of<isq::scotopic_vision>> auto flux);

quantity inverted = 2.0 * si::lumen_of<isq::photopic_vision> / si::lumen_of<isq::scotopic_vision>;  // wrong order
take_scotopic_flux(sp_ratio * led);     // OK
// take_scotopic_flux(inverted * led);  // Compile-time error
```

The same pattern applies to the CIE mesopic system, which combines photopic and scotopic
luminances with empirical coefficients that depend on the adaptation level.

## Keep the two luminous efficacies apart

ISO 80000-7 names two different quantities "luminous efficacy": _of radiation_ (item 7-11.1,
$K = \Phi_\mathrm{v} / \Phi_\mathrm{e}$) and _of a source_ (item 7-11.4,
$\eta_\mathrm{v} = \Phi_\mathrm{v} / P$, with $P$ the power the source consumes). Both are
lm/W, and mixing them silently overstates or understates a lamp's performance, so they are
distinct subkinds of a common `isq::luminous_efficacy` parent, in the same way _fluid head_
and _water head_ are distinct subkinds of _height_.

Crossing between them is real physics, $\eta_\mathrm{v} = K \eta_\mathrm{e}$ with
$\eta_\mathrm{e}$ the radiant efficiency of the source, so write it as a function that
states the formula:

```cpp
constexpr QuantityOf<isq::luminous_efficacy_of_source> auto to_source_efficacy(
                          QuantityOf<isq::luminous_efficacy_of_radiation> auto k,
                          QuantityOf<dimensionless> auto radiant_efficiency)
{
  return isq::luminous_efficacy_of_source(isq::luminous_efficacy(k) * radiant_efficiency);
}

quantity k = isq::luminous_efficacy_of_radiation(400. * si::lumen / si::watt);
quantity eta_v = to_source_efficacy(k, 0.3 * one);  // 120 lm/W

// auto sum = k + eta_v;  // Compile-time error: distinct subkinds
```

Note the explicit `isq::luminous_efficacy(k)` step. A subkind never converts to its parent
implicitly, which is what stops the two efficacies from meeting by accident.

## Work at a mesopic adaptation level

Every mesopic adaptation level defines a different weighting function, so the level is a part
of the type. The library does not interpret the value, and any structural type identifying
the level works:

```cpp
quantity twilight = 2. * si::candela_of<isq::mesopic_vision<50>> / square(si::metre);
// luminances at different adaptation levels are different quantities and do not mix
```

## Define a custom condition

The set of conditions is open. To model, for example, the CIE S 026 melanopic weighting used
in circadian lighting design, derive a tag from `isq::photometric_condition_base`. The entire
tree of quantities and units becomes available for it, with the same safety guarantees:

```cpp
inline constexpr struct melanopic final : mp_units::isq::photometric_condition_base {} melanopic;

quantity mel_edi = 250. * si::lux_of<melanopic>;
```

## What does not compile

```cpp
// auto total = daylight + night;                                             // different quantity kinds
// if (daylight > night) { /* ... */ }                                        // different quantity kinds
// auto oops = daylight.in(si::lumen_of<isq::scotopic_vision>);               // no conversion exists
// auto bad = isq::luminous_flux(250. * si::lumen_of<isq::scotopic_vision>);  // wrong kind unit
// auto worse = mel_edi + 250. * si::lux;                                     // custom kinds do not mix either
```
