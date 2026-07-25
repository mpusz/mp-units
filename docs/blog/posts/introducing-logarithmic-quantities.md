---
date: 2026-07-24
authors:
 - mpusz
categories:
 - Metrology
comments: true
---

# Introducing Logarithmic Quantities and Units

Decibels are everywhere in engineering. Signal levels in `dBm`, _sound pressure_ in
`dB SPL`, _voltage_ gain in `dB`, filter slopes in `dB/octave`. The neper, pH, and stellar
magnitude share the same structure. Yet, to the best of our knowledge,
no general-purpose units library models logarithmic quantities correctly. Most do not
model them at all. The few that do treat a decibel as a non-linear scale or an offset
unit, and that choice gets the arithmetic wrong in ways that compile silently.

This article is a **request for feedback**. We believe we have a correct design, derived
from the same affine-space model we introduced for
[absolute quantities](introducing-absolute-quantities.md). It also aims to stay consistent
with the ISO/IEC 80000 standards: especially IEC 80000-15:2026, *Logarithmic quantities
and their units*, which consolidates the logarithmic-quantity rules once held in ISO
80000-1:2009 (Annex C); together with ISO 80000-2 (the logarithm functions `lb`, `ln`,
`lg`), ISO 80000-8 (acoustics), and IEC 80000-13 (information theory). We have read those
against this design, cite them throughout, and call out the one place we knowingly diverge
from them ([Open Question 5](#5-reference-in-the-unit-or-on-the-quantity)).

This post describes the design in full, from the quantity spec to the named units, the
arithmetic, the conversions, and every domain we surveyed (audio, RF, acoustics,
chemistry, astronomy, music, information theory). Then it lays out the
[open questions](#open-questions), each with every alternative we
considered and our current preference. Before we implement any of this in **mp-units**, we
want the people who use these quantities daily to tell us where we are wrong.

<!-- more -->

## The problem

Consider a `+6 dB` gain. Ask a naive "decibel type" to convert it back to a linear ratio
and it must pick a formula. The decibel is defined two ways depending on the underlying
quantity:

$$L_P = 10\log_{10}\frac{P}{P_0}\ \mathrm{dB} \qquad L_F = 20\log_{10}\frac{F}{F_0}\ \mathrm{dB}$$

The first is for **power** quantities, the second for **root-power** (field) quantities
such as _voltage_ or _sound pressure_. So `6 dB` is a _power_ ratio of
$10^{6/10} \approx 3.98$ or a _voltage_ ratio of $10^{6/20} \approx 2.0$. The two differ
by a factor of two, which is the power-versus-root-power factor.

A decibel modeled as a plain number cannot tell these apart. It has to default to one of
them, usually the _power_ reading, and that is wrong for every _voltage_, _current_, and
_pressure_ gain. In digital audio, where you apply a gain to a sample stream, this is the
common case, and the bug is invisible:

```cpp
auto half_voltage = (0.5 * V) * (6.0 * dB).linear();  // 0.5 * 3.98 ≈ 1.99 V, WRONG
                                                      // "+6 dB on a voltage" means *2 -> 1.0 V
```

The second failure is more obvious once stated. A plain-number `dBm` lets you write
`10 dBm + 10 dBm` and get `20 dBm`, which is `100 mW` and not any combination of the
inputs. Combining two sources *is* meaningful, but the answer is `13 dBm`
(`10 mW + 10 mW = 20 mW`), a linear-domain sum the number type never performs. A level is
a point on a logarithmic scale, anchored at a reference. You can add a *gain* to a level
and subtract two levels to get a gain, but `level + level` is not one operation: even the
`13 dBm` above assumes the sources are incoherent, and coherent ones combine differently
(see [Arithmetic](#arithmetic)). A number type that permits `+` silently picks one answer.

These are not exotic corner cases. They are the everyday arithmetic of the people who use
decibels for a living. This is why getting the type system right matters here as much as
it does for dimensions.

## Points and deltas

The good news is that we do not need a fourth abstraction. Logarithmic quantities fit the
three-abstraction model (point, absolute, delta) that we introduced in the
[absolute quantities article](introducing-absolute-quantities.md), specialized to the
logarithmic domain.

A **level** carries a fixed reference. `dBm` means "relative to 1 mW", `dB SPL` means
"relative to 20 µPa in air". The log-domain value 0 corresponds to the reference
in the linear domain. That reference *is* an affine origin, so a level is a **point**:

```text
point<log<isq::power>>[dBm]:    0 dBm  <->  P = 1 mW   (the origin)
                               10 dBm  <->  P = 10 mW
                              -10 dBm  <->  P = 0.1 mW
```

A **ratio** or **gain** has no reference. "3 dB of gain", "a perfect fifth of 7
semitones", "2 octaves" are multiplicative ratios in the linear domain, expressed
additively in the log domain. A gain is a **delta**.

The whole arithmetic rule set follows from this one split, and it is affine-space
arithmetic applied to the log domain:

| Operation            | Result     | Meaning                                       |
|:---------------------|:-----------|:----------------------------------------------|
| level $+$ gain       | level      | apply a gain to a level (a linear multiply)   |
| level $-$ level      | gain       | the gain between two levels (a linear divide) |
| gain $+$ gain        | gain       | combine two gains                             |
| level $+$ level      | ill-formed | not one operation, linearize to combine       |
| gain $\times$ scalar | gain       | raise the linear ratio to a power             |

Note one consequence: the log-domain value of a level can be negative. `-10 dBm` is a
perfectly valid _power_ level (100 µW), and it does not imply a negative linear quantity.
The logarithm maps all of the real line onto $(0, +\infty)$, so the non-negativity of the
underlying _power_ is guaranteed by the domain of the log function, not by a runtime
check.

That point/delta split is the foundation for everything that follows. Logarithmic
quantities use two of the linear model's three abstractions, the point and the delta;
there is no logarithmic *absolute*, because an absolute is measured from the natural zero
and $\log(0) = -\infty$. Lifted into the log domain, the mapping is:

| Spec             | Mathematical space        | Zero element                      | Example          |
|:-----------------|:--------------------------|:----------------------------------|:-----------------|
| `log<QS>`        | Vector space (log domain) | Unit log-ratio (linear ratio = 1) | a gain in `dB`   |
| `point<log<QS>>` | Affine space (log domain) | Reference level (linear)          | a level in `dBm` |

`log<QS>` is the natural delta type for logarithmic quantities: like the named vector
deltas `displacement` and `velocity`, it needs no `delta<>` wrapper. So the affine
`point - point = delta` rule reads `point<log<QS>> - point<log<QS>> = log<QS>`, with the
spec itself as the delta, just as subtracting two positions yields `displacement` rather
than `delta<displacement>`. (Only ordinary scalar specs, where the delta must be written
explicitly, give the wrapped `point<X> - point<X> = delta<X>` form.)

## The IEC 80000-15 categories

We anchor the design to IEC 80000-15:2026, which sorts the underlying (linear) quantities
into four classes, each with its own standard logarithmic ratio (clauses 5.2 and 5.3):

| Class             | Example linear quantity                   | Logarithmic ratio (standard) |
|:------------------|:------------------------------------------|:-----------------------------|
| power             | active/apparent power, energy             | $10\log_{10}(P_2/P_1)$ dB    |
| root-power        | RMS `isq::voltage`, `isq::sound_pressure` | $20\log_{10}(F_2/F_1)$ dB    |
| amplitude         | a sinusoid's amplitude                    | $\ln(A_2/A_1)$ Np            |
| transfer function | `isq::voltage / isq::sound_pressure`      | $20\log_{10}(M_2/M_1)$ dB    |

Two things to read off this table. The **decibel** carries the power-versus-root-power
split (factor 10 versus 20), which is the split a naive single-number decibel cannot
represent and which the rest of the design serves. The **neper** is the standard's unit
for *amplitude* ratios (natural log, eq 7). An amplitude is the peak value of a root-power
quantity, and ISO 80000 has no generic `amplitude` quantity, so for our purposes amplitude
*is* root-power.

For the multiplier, then, these four categories collapse to **two**: power (factor 1) and
everything else (factor 2). An amplitude is a root-power quantity, and a transfer function
is root-power by the standard's convention (it is not derived from its operands; see
[transfer functions](#transfer-functions-microphone-sensitivity-transfer-impedance)). That
is why the design carries a single classification, power versus root-power, with no
separate `amplitude` tag, and the neper is domain-typed over a power or root-power
quantity exactly like the decibel, differing only in its base (`e`) and its base factor.

## Prior art

We try to be fair to prior work, so here is what we found, with the receipts. If we have
mischaracterized any of these, please correct us in the comments and we will fix this
section.

**Libraries with no logarithmic support.** Boost.Units, Au, and Dominik Berner's SI model
linear dimensional analysis and offer no decibel or neper type. Audio and DSP frameworks
ship ad-hoc helpers (for example, JUCE provides a `Decibels` utility with `gainToDecibels`
and `decibelsToGain`), but these are conversion functions over `float`, not a type that
participates in dimensional analysis or affine arithmetic.

**The nholthaus units library has decibel support, and it is instructive about why this is
hard.** Nic Holthaus's library provides a `UNIT_ADD_DECIBEL` macro that creates types such
as `dBW_t` and `dBm_t` on a non-linear `decibel_scale`. It gets one thing right:
`operator+` adds the values in the logarithmic domain, so applying a gain to a level and
composing two gains both work. The problem is what the type system does not prevent. There
is no distinction between a level and a gain, so adding two absolute levels compiles and
produces nonsense. This is straight from the library's own test suite
(`UnitContainer.dBAddition` in
[`unitTests/main.cpp`](https://github.com/nholthaus/units/blob/578ac4ff8b0e96af8d87dd6b20357522038ccbb3/unitTests/main.cpp#L1538-L1540)):

```cpp
// adding dBW to dBW is something you probably shouldn't do, but let's see if it works...
auto result_dBW2 = dBW_t(10.0) + dBm_t(40.0);
EXPECT_NEAR(20.0, result_dBW2(), 5.0e-5);
```

Both operands are the same physical power: `10 dBW` is 10 W, and `40 dBm` is also 10 W.
Adding two _power_ levels is meaningless, yet the test passes with the result `20 dBW`,
that is, 100 W. The multiplier is also hardcoded to 10, so there is no root-power
(`20 log`) path: a _voltage_ or _pressure_ decibel would be wrong by the factor-of-two we
showed above. And subtracting two levels does not produce a distinct dimensionless gain
type. The arithmetic is value-based, not category-based. Its scope is the decibel alone:
no neper, and no frequency-interval units (octave, cent).

**Python's pint is the most complete prior art we are aware of.** It converts between
logarithmic and linear units correctly, for example `ureg('20 dBm').to('mW')` returns
`100 mW`, and it defines `dB`, `dBm`, `octave`, and `decade`. Two limitations are
[documented by the project itself](https://pint.readthedocs.io/en/stable/user/log_units.html).
First, the decibel is power-only: the documentation
states that "the dB unit is defined for power quantities (`10*log(p/p0)`) not field
(amplitude) quantities (`20*log(v/v0)`)" and tells the user to "take care that you're only
using it to multiply power levels." The root-power factor is not encoded in the type
system, it is delegated to the user with a warning. Second, pint models these as offset
units: the docs say they "behave much like those described in Temperature conversion."
There is no separate level-versus-gain (point versus delta) type, so the affine rule "you
may not add two levels" is not expressed by the types. Support is also marked Beta.

**C#'s UnitsNet takes a third route, and it is close to what many reach for first.** It
models logarithmic quantities as their own types, `PowerRatio` (`dBW`/`dBm`, factor `10`)
and a separate `AmplitudeRatio` (`dBV`, factor `20`), rather than as units of the linear
`Power` and `ElectricPotential` types. Equality and summation are evaluated in the linear
domain, so combining two `10 dBm` sources correctly gives `13 dBm`, and converting a
voltage ratio to a power ratio takes an explicit impedance
([`AmplitudeRatio.ToPowerRatio`](https://github.com/angularsen/UnitsNet)). Two points
matter for us. UnitsNet already splits power from root-power at the type level, which is
direct support for our claim that the `10`/`20` factor must live in the type. And it stops
there: the quantities are logarithmic scalars whose arithmetic is defined by round-tripping
through linear space, with no affine point/delta layer, so there is no separate *level* and
*gain*, and no origin. That last step is what our design adds.

This is why we open with a novelty claim and a request for review rather than a finished
feature. We are not aware of any library that models logarithmic quantities as affine
point and delta quantities, with domain-correct linearization driven by the underlying
quantity kind, and full quantity-kind safety. If you know one, we want to learn from it
before we build ours.

## The design

### The quantity spec: `log<QS>`

A single new quantity-spec wrapper carries the design:

```cpp
template<QuantitySpec QS>
  requires (!is_log_spec<QS>)        // no log<log<...>>
        && (!is_point_spec<QS>)      // no log<point<...>>
        && (!is_delta_spec<QS>)      // no log<delta<...>>
struct log;
```

`QS` is the underlying linear quantity spec: `isq::power`, `isq::sound_pressure`,
`isq::frequency`, `dimensionless`, and so on. The constraints rule out the nonsensical
nestings. A "log of a log" has no physical meaning, and an already-wrapped point or delta
spec must not be re-wrapped.

`log<QS>` and `QS` are roots of separate quantity-spec trees: they are different quantity
kinds. The ordinary unit-and-kind machinery never crosses between them on its own, so a
`log<isq::power>` quantity is never silently usable where an `isq::power` is expected, or
the reverse. Crossing the boundary re-encodes the same physical state in the other domain
(a `10 dBm` level and `10 mW` describe the same power), so, like any conversion, it
changes the number, here through a logarithm, not the physics. What sets it apart from a
unit conversion such as `mW`→`W` is that the latter stays *within* one kind, whereas this
jumps between the separate `log<isq::power>` and `isq::power` trees, and **mp-units**
never crosses kinds implicitly (the rule that also rejects `length`→`duration`). So the
crossing is a named function, `.linear()` (log to linear) and `.log_in()` (linear to log),
detailed later under
[Conversions across the linear boundary](#conversions-across-the-linear-boundary). The
reference level, the logarithmic base, and the scale multiplier are all encoded in the
named unit, never in the spec.

`log<QS>[unit]` is a valid reference. The library enforces correct usage at each consumer,
based on whether the unit carries a reference level (a level unit) or not (a ratio unit):

- `value * unit` produces a `log<QS>` gain for a ratio unit (`dB`), and is ill-formed for
  a level unit (`dBm`).
- `delta<unit>(value)` constructs the same gain for a ratio unit, as an explicit,
  multiply-free alternative to `value * unit` (useful when `operator*` is undesirable or
  ambiguous with the representation type). It yields the same `log<QS>` gain, not a
  `delta<log<QS>>` double-wrap, and is ill-formed for a level unit.
- `point<unit>(value)` constructs a level point for a level unit, and is ill-formed for a
  ratio unit.

### Ratio units: `dB`, `B`, and `Np`

A ratio (gain) unit is a `named_unit` over `log<get_kind(QS)>`: the unit serves the *kind*
of `QS`, so `QS` may be any quantity and `get_kind` resolves it to its kind
(`get_kind(isq::voltage)` is `isq::electric_potential`, the kind `volt` belongs to).
`dB<QS>` is this unit for a given `QS`, with `QS` defaulting to `dimensionless`, so plain
`dB` is the dimensionless gain. They all print `"dB"`. Its base is 10 and its multiplier
is `10 * log_coefficient_of<QS>`, where `log_coefficient_of<QS>` is the kind's **log
coefficient**: `1` for a power kind and `2` for a root-power one. So the dB multiplier is
10 for power and 20 for root-power. The unit always writes its own factor (`10`), and the
kind's coefficient (`1` or `2`) flips it between 10 and 20, so that difference lives in one
place, the kind (shared by `dB`, `B`, and `Np`), rather than being restated on each unit.
The next section, [Classifying the quantity](#classifying-the-quantity-the-multiplier)
explains `log_coefficient_of` and how a kind declares its coefficient (pH and stellar
magnitude use other values).

```cpp
// log_coefficient_of<QS> reads the log coefficient of QS's kind (see the next section)
template<QuantitySpec auto QS>
struct decibel_ : named_unit<"dB", log<get_kind(QS)>, /*base*/ mag<10>, 10 * log_coefficient_of<QS>> {};

template<QuantitySpec auto QS = dimensionless>
inline constexpr decibel_<get_kind(QS)> decibel{};   // one dB unit per kind

template<QuantitySpec auto QS = dimensionless>
inline constexpr auto dB = decibel<QS>;
```

So `dB` and `dB<isq::power>` carry multiplier 10, while `dB<isq::voltage>` and
`dB<isq::sound_pressure>` carry 20. The unit is one per *kind*: `dB<isq::voltage>` and
`dB<isq::electric_potential>` are the same unit (both `log<electric_potential>`), so
`2 * dB<isq::voltage> + 2 * dB<isq::electric_potential>` adds directly to `4 dB`. `dB<QS>`
is a variable template, as shown above, so one definition serves every kind rather than
minting a named unit per quantity type.

The bel and the neper are ratio units too, reading the *same* classification. They differ
only in their base factor and, for the neper, the logarithm base. The bel drops the
decibel's factor of ten, and the neper switches to the natural-log base `e` and carries
its own half-factor:

```cpp
template<QuantitySpec auto QS>
struct bel_ : named_unit<"B", log<get_kind(QS)>, /*base*/ mag<10>, log_coefficient_of<QS>> {};
template<QuantitySpec auto QS>
struct neper_ : named_unit<"Np", log<get_kind(QS)>, /*base*/ mag_e, log_coefficient_of<QS> / 2> {};

template<QuantitySpec auto QS = dimensionless>
inline constexpr bel_<get_kind(QS)> bel{};
template<QuantitySpec auto QS = dimensionless>
inline constexpr neper_<get_kind(QS)> neper{};

template<QuantitySpec auto QS = dimensionless>
inline constexpr auto B = bel<QS>;
template<QuantitySpec auto QS = dimensionless>
inline constexpr auto Np = neper<QS>;
```

`mag_e` is the natural-log base `e`. The variable templates and symbol aliases mirror
`decibel`, with `QS` defaulting to `dimensionless`. `B` carries factor 1 and `Np` factor ½,
so their multipliers are `log_coefficient_of<QS>` and `log_coefficient_of<QS> / 2`, while
`dB` carries factor 10.

We deliberately do not spell `decibel` as `si::deci<bel>` or as `mag<1, 10> * bel`. The
factor of ten between the bel and the decibel is not a linear magnitude. It lives in the
logarithmic *multiplier* (the bel carries 1, the decibel 10), so `1 B = 10 dB`. The SI
`deci` prefix divides a unit's *linear* magnitude by ten, but a logarithmic unit has no
linear magnitude to divide: its stored value is already a logarithm. Spelling
`si::deci<bel>` forces a linear-domain prefix onto the logarithmic axis, and the
`mag<1, 10>` form buries the factor in a magnitude where the framework can no longer read
it as the multiplier. So the decibel and the bel are co-equal named units, each stating
its own `(base, multiplier)`, with the decibel's multiplier ten times the bel's.

A ratio unit has no reference, so it is the natural delta type and the multiply syntax
produces a gain directly:

```cpp
quantity g = 3.0 * dB<isq::power>; // a 3 dB power gain
quantity i = 7.0 * cent;           // a 7 cent frequency interval
```

### Classifying the quantity: the multiplier

We do not write the literal multiplier on every unit, and there are many: one `dB` per
kind (`dB<isq::power>`, `dB<isq::voltage>`, ...), plus `B` and `Np`. Instead each *kind*
declares one number, its **log coefficient**, and a unit's multiplier is its own factor
times that coefficient (`dB` contributes 10, `B` contributes 1, `Np` contributes ½). The
coefficient is any signed rational. Two values have names, the IEC 80000-15 §5.2 cases: `1`
for a power quantity (the default) and `2` for a root-power one (whose level is taken on
the square, hence $20\log_{10}$ versus $10\log_{10}$). Other domains set their own:
pH uses `-1`, stellar magnitude `-5/2`.

**Why declared, not derived.** It is tempting to compute the coefficient from the
quantity's dimension or defining equation, but you cannot. "Root-power" means the
quantity's *square* is proportional to a power quantity, and that proportionality carries
a dimensioned constant: `voltage² / power = resistance`, and
`sound_pressure² / intensity = ρc`, an acoustic impedance. The impedance does not cancel,
so `voltage²` is not power, dimensionally or otherwise. IEC 80000-15 §5.2 assigns the
classes by hand, and so must we.

The coefficient is a **specifier on the kind**, the same kind of declaration as
`non_negative` from the [absolute quantities design](introducing-absolute-quantities.md)
(though `non_negative` may also sit on a child quantity, whereas the coefficient is
kind-only). It must be kind-level because the multiplier has to be uniform across a kind:
two quantities of one kind add, subtract, and compare through the kind's multiplier, so a
kind cannot mix coefficients. Power is the default `1` and is never written. Every other
kind states its coefficient with `log_coefficient<N>`:

```cpp
// log_coefficient<2> = root-power. electric_potential is the kind root of voltage,
// electric_potential_difference, ..., so all of them inherit coefficient 2:
inline constexpr struct electric_potential :
    quantity_spec<isq::energy / (isq::electric_current * isq::time), log_coefficient<2>> {} electric_potential;

// sound_pressure is root-power but shares the (power) pressure dimension, so is_kind makes it its
// own kind, while it stays kind_of pressure so the unit Pa applies:
inline constexpr struct sound_pressure :
    quantity_spec<isq::pressure, is_kind, log_coefficient<2>> {} sound_pressure;
```

So a root-power kind carries `log_coefficient<2>` on its root (`electric_potential`), and
every quantity of that kind inherits it through `get_kind`. `is_kind` is needed only when
a root-power quantity *shares a dimension with a power one* (`sound_pressure` versus
general `pressure`): its natural kind root is the power `pressure`, so `is_kind` gives it
its own subkind. Because `is_kind` makes `sound_pressure` a distinct kind, converting to or
from plain `pressure` is explicit in **both** directions, the widening up as well as the
narrowing down, so a `sound_pressure` value never silently decays to `pressure`'s
coefficient `1`.

A unit reads the coefficient with `log_coefficient_of<QS>`, the value declared on `get_kind(QS)`
(default `1`): `2` for `isq::voltage` (kind `electric_potential`), `1` for `isq::power`.
It does *not* compose a coefficient for a derived quantity, because there is no sound rule
to compose with (see
[transfer functions](#transfer-functions-microphone-sensitivity-transfer-impedance)).

This buys several things:

- the kind and the multiplier cannot disagree, because the coefficient lives in one place
  and units only scale it;
- `level - level` self-linearizes correctly with no per-unit annotation;
- a custom root-power kind (an audio sample-amplitude type, say) declares
  `log_coefficient<2>` once;
- it can guard: an untagged derived kind like `power²` has no coefficient, a strong
  argument for the `unclassified` default in
  [Open Question 6](#6-should-the-classification-have-a-default), under which `dB<power²>`
  is ill-formed rather than silently coefficient `1`.

There is **no per-unit multiplier override**. The coefficient always lives on the kind,
so a unit can never contradict it. A quantity whose coefficient is neither 1 nor 2 is its
own kind carrying that value: pH uses `-1`, stellar magnitude `-5/2`.

```cpp
// pH is its own kind (a subkind of concentration) with coefficient -1; the pH unit just reads it
inline constexpr struct hydrogen_ion_concentration :
    quantity_spec<isq::concentration, is_kind, log_coefficient<-1>> {} hydrogen_ion_concentration;
```

**The cost, stated plainly.** A kind specifier means classifying the library's kinds: a
one-time audit deciding, for each kind (including subkinds like `sound_pressure`), its
coefficient: power by default, root-power, or a custom value. The coefficient then rides
along on the quantity spec unconditionally, including for the many users who never take a
logarithm. We consider this acceptable. The classification is an intrinsic property of the
quantity (the standard assigns it whether or not anyone takes its logarithm), it costs
nothing at runtime, and because power (coefficient 1) is the default, only non-power kinds
carry a specifier: the root-power ones, plus custom ones like pH and stellar magnitude.
The real hazard is silent misclassification: a root-power quantity left untagged keeps
coefficient 1, so a 6 dB gain
would linearize as a power ratio ($\approx 3.98$) instead of the correct root-power ratio
($2$). Whether to keep that convenient default or require an explicit classification is
[Open Question 6](#6-should-the-classification-have-a-default).

### Gains carry their domain

This is the one place our design departs from what you might write first, and the place we
most want scrutiny.

A gain produced by subtracting two levels keeps the domain of those levels. `point<dBV>`
minus `point<dBV>` is the ordinary affine identity (point minus point equals delta) with
the spec `log<isq::voltage>`, so the result is a **voltage-domain gain**, not a
domain-less dimensionless decibel:

```cpp
quantity g = point<dBV>(0.0) - point<dBV>(-6.0);  // log<isq::voltage> gain, value 6
g.linear();                                       // *2.0, a VOLTAGE ratio, correct by construction
```

The rationale behind this is that `.linear()` of a gain is only correct if the gain knows
its domain. We saw at the top that `6 dB` is a _power_ ratio of about 3.98 or a _voltage_
ratio of about 2.0. A domain-less gain has to guess. A `log<isq::voltage>` gain does not:
its multiplier is 20, so it linearizes to the _voltage_ ratio every time.

It turns out this is not a new feature we are bolting on. It is what our own affine model
already implies. The level type is `point<log<QS>>`, so `point<log<QS>>` minus
`point<log<QS>>` is a `log<QS>` delta by the same rule that makes `point<m>` minus
`point<m>` a `delta<m>`. Collapsing that to a dimensionless decibel would be the mistake.
We use the notation `dB<QS>` throughout to mean a `log<QS>` gain. Every such unit still
prints `"dB"`. The type, not the symbol, carries the domain.

The same reasoning fixes a conformance subtlety. IEC 80000-15:2026 defines the neper
through the *amplitude* ratio $Q_A = \ln(A_2/A_1)$ Np (eq 7). An amplitude is a root-power
quantity, so over a root-power quantity the neper carries factor 1 while the decibel
carries factor 20. Converting between them (the conventional amplitude-to-root-power
conversion) gives $1\ \mathrm{Np} = 20\log_{10}(e) \approx 8.686\ \mathrm{dB}$, which a
domain-carrying neper reproduces by construction. A domain-less neper, treated as a plain
power ratio (factor 10), would instead give $10\log_{10}(e) \approx 4.343\ \mathrm{dB}$,
the wrong answer. The standard itself states no generic `Np`-to-`dB` equivalence
(Table 2). The 8.686 factor is the conventional bridge for the root-power quantities the
neper is normally used with.

### No universal decibel

Domain-typed gains have a cost, and we state it plainly. There is **no neutral,
domain-less decibel** that adds to any level. A `+3 dB` _power_ gain moves a _power_
level. To move a _voltage_ level you need a `+3 dB` _voltage_ gain, and the type system
enforces the match:

```cpp
quantity power_gain = 3.0 * dB<isq::power>;
quantity rf_level   = point<dBm>(10.0);
quantity ok         = rf_level + power_gain;                 // 13 dBm
// quantity bad     = rf_level + 3.0 * dB<isq::voltage>;     // ill-formed: voltage gain on power level
```

The good parts here are: `.linear()` is always correct, the neper matches the standard,
and a _voltage_ gain provably cannot move a _pressure_ level (the same protection linear
quantities have, where a _pressure_ delta cannot move a _voltage_). The drawback is that
the convenient "a 3 dB gain moves anything" mental model is gone. We think the safety is
worth it, because the convenient model is exactly the one that produces the
`3.98`-versus-`2.0` bug. We are aware this is a judgment call, and it is one of the things
we ask you to weigh in on below.

Gains and levels differ across domains. A *gain*'s dB figure is domain-invariant: the same
physical gain reads as the same number of decibels in either domain, which is exactly why
the root-power factor is 20 and not 10. A *level*'s reference is domain-specific: `0 dBV`
(1 V) and `0 dB SPL` (20 µPa) are unrelated physical points, so crossing a level between
domains is never implicit and needs a real physical conversion (a _transducer sensitivity_
or an _impedance_), which is user code. If you ever need
the same gain figure in another domain, you rebuild it there explicitly, as there is no
cross-kind conversion.

### Comparing bases

Within one logarithmic quantity, IEC 80000-15 §4.2.2 allows any base ("any base can be
used without detriment to coherence between SI units"). So a single quantity can have
several units differing only in base: a power ratio in `dB` or `B`, a frequency ratio in
`oct` or `dec`, information content in `Sh`, `nat`, or `Hart`. Whether two such units
combine implicitly falls out of one rule.

Every log unit reduces to a single log-domain scale factor $M / \ln B$. Converting between
two of them is a linear rescale by the ratio of those factors, which is a representable
compile-time rational **if and only if** $\ln B_1 / \ln B_2$ is rational, that is, if the
two bases are rational powers of a common base.

So `dB` and `B` (factor 10) share a common unit, and `oct`, `cent`, `semitone` (factors
1200, 12, 100) share one. But `oct` and `dec` differ by $\log_{10} 2$, which is
irrational, so they have no representable common unit. The rule we adopt:

!!! note "The common-unit rule for logarithmic bases"

    Two log units share a common unit if and only if their conversion factor is a
    representable rational magnitude. When it is not, `+`, `-`, and all comparisons
    between them are ill-formed. A directed `.in()` conversion is still available,
    evaluated at runtime.

```cpp
quantity ok    = 2 * oct + 600 * cent;             // OK: oct and cent share a unit (1 oct = 1200 cent)
// auto wrong  = 2.0 * oct + 0.5 * dec;            // ill-formed: log10(2) is irrational, no common unit
quantity fixed = 2.0 * oct + (0.5 * dec).in(oct);  // OK once you pick the base explicitly
```

The rationale behind this is that there is no mathematically natural common unit for an
irrational factor, and forcing one would let mixed-base arithmetic compile while carrying
a hidden transcendental factor. Making the conversion explicit puts the chosen base on the
page. The two axes are independent: the *domain* axis (`dB<voltage>` versus `dB<power>`)
is governed by quantity kind, and the *base* axis (`oct` versus `dec`, or `Sh` versus
`nat`) by this rule. `dB` and `Np` are both power/root-power units. They sit at different
points on the *base* axis (base 10 versus base `e`), and their ratio is the irrational
$20/\ln 10 \approx 8.686$ from the neper discussion above, so converting between them is a
directed base change like `oct`-to-`dec`, with no shared common unit.

### Level units: `dBm`, `dBW`, and `dB SPL`

With the ratio units defined, a level unit adds a reference, mirroring how
`degree_Celsius` is `kelvin` plus the ice-point origin. A level (point) unit is a
`named_unit` built from a ratio unit plus a reference level. The framework infers the
level's quantity spec from the reference (`mW` gives `kind_of<isq::power>`, `V` gives
`kind_of<isq::voltage>`), and the reference becomes the unit's origin.

```cpp
inline constexpr struct dBm : named_unit<"dBm", dB<isq::power>, mW> {} dBm;
inline constexpr struct dBW : named_unit<"dBW", dB<isq::power>, W>  {} dBW;
inline constexpr struct dBu : named_unit<"dBu", dB<isq::voltage>, mag_power<3,1,2> * mag_power<5,-1,2> * V> {} dBu;
inline constexpr struct dBV : named_unit<"dBV", dB<isq::voltage>, V> {} dBV;
inline constexpr struct dB_SPL : named_unit<"dB SPL", dB<isq::sound_pressure>, mag<20> * uPa> {} dB_SPL;
```

Because the reference is inferred into the spec, `point<dBm> - point<dBV>` is a
compile-time error with no hand-written rules: _power_ and _voltage_ are different kinds.

A conformance note, about *where the reference value lives*. ISO 80000-8 (items 8-14 to
8-16), following ISO 80000-1, states that the unit of a level is `dB` and that information
about the reference "is not permitted" to be attached to the unit symbol: the reference
belongs to the *quantity*. The standard writes a sound pressure level as
`L_p = 40 dB (re 20 µPa)`, with the unit `dB` and the reference in parentheses (or as a
subscript on the quantity symbol). IEC 80000-15 §5.4.3 says the same. By that rule `dBm`,
`dBW`, `dBV`, and `dB SPL` are industry shorthands, not standard units, because each folds
the reference into the unit symbol (the `m` in `dBm` is the milliwatt reference).

Our model agrees on placement: the reference is the point's *origin*, on the quantity side,
never on the `dB` unit. For convenience we still define the named units above (`dBm`,
`dBW`, ...), which bundle `dB` with a standard origin, exactly as `degree_Celsius` bundles
`kelvin` with the ice point. That is handy, but a symbol like `dBm` then *prints* the
reference on the unit, which is what the standard avoids. So the open choice is purely one
of display: the industry `10 dBm`, or the conformant `10 dB (re 1 mW)`. That is
[Open Question 5](#5-reference-in-the-unit-or-on-the-quantity).

A level unit has a reference, so it constructs a point. The multiply syntax is disabled,
exactly as it is for `degree_Celsius`, because `N * unit` never yields a point anywhere in
**mp-units** (a point needs an explicit origin). So `-6 * dBFS` is rejected, and a level is
built with `point<dBm>(...)`:

```cpp
// auto wrong = 3.0 * dBm;         // ill-formed, just like 3.0 * deg_C
quantity p = point<dBm>(3.0);      // a power level, 3 dBm
```

### Origins

Linear quantities can fall back to a `natural_point_origin` (the physical zero).
Logarithmic quantities cannot, because $\log(0) = -\infty$. Every logarithmic point origin
is therefore a reference-level anchor. Three forms cover the design space, in order of how
often you reach for them.

**The auto-generated companion origin** is the common case. Every level unit gets a
framework origin that inherits from `absolute_point_origin<log<QS>>` and is returned by
`default_point_origin(unit)`. You never write it. `point<dBm>(10.0)` is anchored at the 1
mW origin automatically.

**An explicit named absolute origin** is for a custom log scale that no standard level
unit captures, the same way `sea_level` names an _altitude_ origin that is not the
physical zero:

```cpp
inline constexpr struct midi_zero : absolute_point_origin<log<isq::frequency>> {} midi_zero;
// MIDI note 0 = C-1 ≈ 8.175 Hz

quantity<point<log<isq::frequency>[semitone], midi_zero>, int> note = midi_zero + 69 * semitone;
```

**A relative origin** anchors a scale at an application-defined fixed point, for example a
radio system expressing _power_ relative to its nominal transmit level:

```cpp
inline constexpr struct nominal_tx : relative_point_origin<point<dBm>(43.0)> {} nominal_tx;  // 43 dBm = 20 W

quantity backoff = nominal_tx + (-6.0) * dB<isq::power>;  // 37 dBm = 5 W, rooted at nominal_tx
```

### Conversions within the log domain

Converting between two level units of the same kind, such as `dBm` to `dBW`, is both a
unit conversion and an origin change, because each level unit embeds its own
reference-level origin. The offset is non-linear: it is an addition in the log domain
rather than a multiplicative scaling in the linear domain.

```text
P_dBW = P_dBm + 10 * log10(1 mW / 1 W) = P_dBm - 30
```

The correct API is `.in(unit)`, which changes the unit and its embedded origin together,
exactly as `.in(deg_F)` does for a Celsius point:

```cpp
quantity p_dBW = point<dBW>(0.0);  // 0 dBW = 1 W
quantity p_dBm = p_dBW.in(dBm);    // 30 dBm, unit and origin changed together
```

When two levels of the same kind are subtracted, both are first converted to a common
level unit, the one with the smaller reference (preventing truncation, the same rule that
picks `mW` over `W`), and the reference levels cancel to leave a gain.
`point<dBm>(40.0) - point<dBW>(0.0)` is `10 dB`.

`point_for(origin)` and `delta_from(origin)` rebase a level within its origin tree:
`delta_from(nominal_tx)` expresses a _power_ level as a backoff from the nominal transmit
level, which is the natural way to print "minus 33 dB relative to nominal". What
`point_for` should do when the target origin belongs to a *different* level unit is
[Open Question 3](#3-point_for-across-level-units). Converting across kinds (`dBm` to
`dB SPL`) is never implicit, because the quantity specs are incompatible. It requires a
real physical model (a microphone calibration, for instance), which is user code.

### Conversions across the linear boundary

Two named functions are the crossing. They are the canonical API and are not negotiable: a
fully-typed return keeps call sites CTAD-friendly, so
`quantity q = (10 * mW).log_in(dBm);` deduces its type with nothing to spell. Whether to
*also* offer a constructor is
[Open Question 2](#2-should-the-named-conversions-also-have-a-constructor).

Log to linear uses `.linear()` (and `.absolute()` to drop a point's origin):

```cpp
quantity p     = point<dBm>(10.0);
quantity p_lin = p.linear();          // a linear point, 10 mW
quantity p_abs = p_lin.absolute();    // 10 mW as an absolute quantity
quantity ratio = (3.0 * dB<isq::power>).linear();  // the linear ratio, dimensionless
```

Linear to log uses `.log_in(unit)`. The result is always a `quantity`, with the unit fixing
its spec: a ratio unit like `dB` gives a gain (`log<QS>`), a level unit like `dBm` gives a
level (`point<log<QS>>`), mirroring `3 * dB` versus `point<dBm>(...)`:

```cpp
quantity gain = (2.0 * one).log_in(dB);         // linear ratio       -> gain:  10*log10(2) ≈ 3.01 dB
quantity lvl  = (10.0 * mW).log_in(dBm);        // absolute power     -> level: 10 dBm
quantity lvl2 = point<mW>(10.0).log_in(dBm);    // natural-origin pt  -> level: 10 dBm (same result)
// auto  bad  = delta<mW>(10.0).log_in(dBm);    // ill-formed: a delta has no zero to anchor a level
```

Neither direction special-cases the logarithm. Both read the unit's stored base $B$,
multiplier $M$, and reference $x_0$, and apply one generic pair of formulas: `.log_in`
computes $M\log_B(x/x_0)$ and `.linear()` computes $x_0 \cdot B^{\,v/M}$. Since
$\log_B(y) = \ln(y)/\ln(B)$ and $B^k = \exp(k\ln B)$, the framework only needs $\ln$ and
$\exp$. The familiar function for each unit falls out of its base: `Np` has $B = e$, so
$\ln B = 1$ and it reduces to plain $\ln$ / $\exp$; `dB` and `B` have $B = 10$, giving
$\log_{10}$ / $10^{\,\cdot}$; `Sh` has $B = 2$, giving $\log_2$. This is the
floating-point boundary, which is why an integer representation cannot cross it. That is
the only restriction: integers stay first-class *within* the log domain, as
[Representation types](#representation-types) explains.

The linear-to-log direction is only well-formed when the library knows the input is
measured from the unit's zero. An absolute quantity (`10.0 * mW`) or a point at the
natural origin works. A point at a custom absolute origin, or any relative-origin point,
is rejected, because the stored value is a displacement from that custom origin rather
than from the unit's zero, and silently using it would give a wrong result. In those cases
the user computes the absolute value explicitly with domain knowledge.

```cpp
inline constexpr struct noise_floor : absolute_point_origin<isq::power> {} noise_floor;

quantity above = noise_floor + delta<mW>(3.0);  // a point 3 mW above the noise floor
// auto  bad   = above.log_in(dBm);             // ill-formed: measured from noise_floor, not the linear zero
```

A `noise_floor`-anchored point cannot even be rescued automatically: a bare custom origin
has no library-known offset to the linear zero, so only the user knows the floor's absolute
power. That is exactly why the crossing refuses to guess.

For non-positive input, the result follows IEEE 754 floating-point semantics by default:
$\log(0) = -\infty$ and $\log(x < 0) = \mathrm{NaN}$. We impose no library-side
precondition check. Whether to offer an opt-in floor for the engineering pipelines that
need a finite sentinel is
[Open Question 4](#4-what-to-do-at-the-bottom-of-the-scale-log0).

### Arithmetic

Addition and subtraction follow the point/delta table from above. Adding or subtracting a
raw scalar is ill-formed for every logarithmic quantity (`3 dB + 2` has no physical
meaning). The only scalar interaction is multiplying a gain. Five more operations are
worth spelling out.

!!! note "Why is `10 dBm + 10 dBm` ill-formed, and not `13 dBm`?"

    Combining two `10 dBm` sources is a real operation, and the answer is `13 dBm`
    (`10 mW + 10 mW = 20 mW`), not the `20 dBm` a plain-number type would give. But that is
    a *linear-domain* sum, and it is not the only combination. It depends on the kind and
    the physics: two incoherent `60 dB SPL` sources add as intensities and give
    `63 dB SPL`, while two coherent ones add as pressures and give `66 dB SPL`. The phase
    that decides between them lives in the [representation](#representation-types), not the
    unit. A `+` operator cannot pick the right one, so `level + level` stays ill-formed and
    you write the combination explicitly:

    ```cpp
    quantity total = (a.absolute() + b.absolute()).log_in(dBm);   // 20 mW ≈ 13 dBm
    ```

    This is also why `dBm` is a *level* rather than a unit of power. In a model where `dBm`
    is a unit of `Power`, `dBm(40) - dBm(10)` is a power difference in watts, whereas what
    you usually want is the `30 dB` gain between the two levels. The level/gain split keeps
    both operations available and distinct.

**Negation.** Negating a gain produces another gain (the reciprocal linear ratio).
Negating a *level* is also meaningful and permitted, because the log-domain value is
signed: `-point<dBm>(10.0)` is `-10 dBm`, which is `0.1 mW`, a perfectly ordinary RF
value. It does not produce a negative linear quantity.

```cpp
quantity attenuation = -(3.0 * dB<isq::power>);   // a 3 dB attenuation
quantity low_power   = -point<dBm>(10.0);         // -10 dBm, 0.1 mW
```

**Scaling, powers, and roots.** Multiplying a gain by a scalar raises the linear ratio to
a power, so it scales the log-domain value. Because a logarithm turns exponentiation into
multiplication, `pow`, `sqrt`, and `cbrt` on a gain simply scale its numerical value:

```cpp
quantity g       = 6.0 * dB<isq::power>;  // linear ratio 10^0.6 ≈ 3.98
quantity half_g  = sqrt(g);               // 3 dB, linear ratio ≈ 2.00
quantity third_g = cbrt(g);               // 2 dB, linear ratio ≈ 1.58
quantity twice_g = pow<2>(g);             // 12 dB, linear ratio ≈ 15.8
```

**Multiplying two gains is ill-formed.** Unlike linear quantities, where `delta × delta`
yields a derived quantity, multiplying two log gains would raise one linear ratio to the
power of another, which has no standard physical meaning.

**Rates are not the same as that ban.** The ban is about *multiplying* two gains.
*Dividing* a gain by a linear quantity is well-formed and standardized: attenuation
coefficients such as `Np/m`, `dB/m`, and `Np/s` (IEC 80000-15 §4.2.1 lists the attenuation
coefficient as a related logarithmic quantity; the classic $\alpha = 0.83\ \mathrm{Np/m}$
example is from ISO 80000-1:2009, Annex C). `Np/m` times a _length_ gives back a gain.
Dividing a gain by a *logarithmic* interval (`dB/octave`, the filter-design slope) is the
one case that sits in the ban's territory, but as a division rather than a product. Our
position is that the ban forbids the `log × log` product, and division for rates is
permitted. Whether a typed `dB<QS>/octave` is worth more than treating the slope as a
plain number is something we want industry feedback on, not a settled rule.

**Comparison.** Both levels and gains support the full comparison set (`==`, `!=`, `<`,
`<=`, `>`, `>=`), following the same rules as linear quantities. The same unit compares
values directly. Different units of the same kind convert to a common unit first, so a
_power_ level of `10 dBm` equals `-20 dBW`. Different kinds, and comparing a level against
a gain, are ill-formed. No new operators are needed once the conversion machinery is in
place.

### Representation types

Logarithmic quantities do not force a floating-point representation. Integer log-domain
values are common and must work: musical intervals and MIDI note numbers (`700 * cent`,
`12 * semitone`), integer-dB power-control protocols, integer information content
(`3 * Sh`). The constraint sits on the *operation*, not the type, exactly as `math.h`
already gates `sin` and `floor` on `treat_as_floating_point`. Storage, same-unit
arithmetic, scaling, and comparison work for an integer representation. Only the
operations that cross the linear boundary (`.linear()`, `.log_in()`) or perform an
irrational base conversion require a floating-point representation.

A representation can also be complex. A transfer function or phasor is a complex amplitude,
and its dB level is the magnitude, so the phase rides along on the value while the level
stays real:

```cpp
quantity Vc    = std::complex{3.0, 4.0} * V;   // a voltage phasor: |Vc| = 5 V, phase ≈ 53°
quantity level = abs(Vc).log_in(dBV);          // ≈ 14 dBV from |Vc|; the phase stays in Vc
```

Phase, and signal correlation with it, lives in the representation, not the unit.

A custom representation joins by supplying the math functions through argument-dependent
lookup, the same idiom `math.h` already uses
(`requires { log(v); } || requires { std::log(v); }`). For the boundary-crossing
operations it must additionally behave like IEEE 754: provide an infinity and a quiet NaN
so its own `log(0)` returns minus infinity. This is a documented contract on the
representation, not a guard inside the library, which is why integer representations (no
infinity, no NaN) cannot perform the crossing operations.

### Text output

A gain is an ordinary `quantity`, so the existing formatter prints it without changes: a
`log<isq::power>` gain prints `"3 dB"`, a _frequency_ interval prints `"2 oct"`.

A level is a point whose unit determines a single canonical origin, so it formats
unambiguously: `point<dBm>(10.0)` prints `"10 dBm"`, which means exactly
$10\log_{10}(P / 1\,\mathrm{mW})$. Two equal-value levels in different units print
differently (`"10 dBm"` and `"-20 dBW"`), and that is correct, because the unit name
carries the reference. Whether to keep that bundled spelling or print the standard
`dB (re 1 mW)` form is [Open Question 5](#5-reference-in-the-unit-or-on-the-quantity).

When one quantity has more than one conventional reference (sound pressure level uses 20
µPa in air but 1 µPa in water), each reference gets its own level unit with a distinct
symbol (`dB SPL` and `dB SPL (water)` in the catalog below), so they never format
ambiguously.

### Quantity hierarchy

`log<QS>` inherits the ISQ hierarchy from `QS`. If `isq::active_power` is a kind of
`isq::power`, then `log<isq::active_power>` is a kind of `log<isq::power>`. A
`quantity<point<log<isq::power>>[dBm]>` therefore accepts a value typed as
`log<isq::active_power>` (an implicit upcast), but not the reverse without an explicit
cast. This is the same quantity-kind safety the linear library already provides, carried
into the log domain for free.

## The domains

The design above is general. Here is how it lands on each domain we surveyed. The breadth
is the point: these are the communities whose feedback we are asking for. The whole
catalog at a glance (the `Underlying QS` column gives the `isq::` quantity spec, prefix
omitted for brevity, or the category for the domain-typed neper):

| Quantity                | Symbol           | Underlying QS            | Type  | Reference         | Base       | Mult |
|:------------------------|:-----------------|:-------------------------|:------|:------------------|:-----------|:-----|
| Power level             | `dBm`            | `power`                  | level | 1 mW              | 10         | 10   |
| Power level             | `dBW`            | `power`                  | level | 1 W               | 10         | 10   |
| Sound power level       | `L_W`            | `sound_power`            | level | 1 pW              | 10         | 10   |
| Sound exposure level    | `L_E`            | `sound_exposure`         | level | 400 µPa²·s (air)  | 10         | 10   |
| Sound exposure level    | `L_E (water)`    | `sound_exposure`         | level | 1 µPa²·s          | 10         | 10   |
| Voltage level           | `dBu`            | `voltage`                | level | √(3/5) V          | 10         | 20   |
| Voltage level           | `dBV`            | `voltage`                | level | 1 V               | 10         | 20   |
| Sound pressure level    | `dB SPL`         | `sound_pressure`         | level | 20 µPa (air)      | 10         | 20   |
| Sound pressure level    | `dB SPL (water)` | `sound_pressure`         | level | 1 µPa             | 10         | 20   |
| Transfer function level | `dB(V/Pa)`       | `voltage/sound_pressure` | level | 1 V/Pa            | 10         | 20   |
| Antenna gain            | `dBi`            | `dimensionless`          | level | isotropic         | 10         | 10   |
| Antenna gain            | `dBd`            | `dimensionless`          | level | dipole (2.15 dBi) | 10         | 10   |
| Power ratio (gain)      | `dB`             | `dimensionless`          | gain  | none              | 10         | 10   |
| Amplitude ratio         | `Np`             | root-power               | gain  | none              | e          | 1    |
| Frequency interval      | `oct`            | `frequency`              | gain  | none              | 2          | 1    |
| Frequency interval      | `dec`            | `frequency`              | gain  | none              | 10         | 1    |
| Frequency interval      | `semitone`       | `frequency`              | gain  | none              | 2^(1/12)   | 1    |
| Frequency interval      | `cent`           | `frequency`              | gain  | none              | 2^(1/1200) | 1    |
| Information content     | `Sh`             | `information_content`    | gain  | none              | 2          | 1    |
| Information content     | `nat`            | `information_content`    | gain  | none              | e          | 1    |
| Information content     | `Hart`           | `information_content`    | gain  | none              | 10         | 1    |
| pH                      | `pH`             | `concentration`          | level | 1 mol/L           | 10         | −1   |
| Stellar magnitude       | `mag`            | `irradiance`             | level | defined flux      | 10         | −2.5 |

### Power and root-power levels (RF, audio, acoustics)

The catalog shows which units classify as power (factor 10) and which as root-power
(factor 20). The point worth repeating is the consequence for `.linear()`: the numeric
level value of a root-power quantity equals that of the corresponding _power_ quantity
(because $20\log F = 10\log F^2$), but the type differs, so a `dB<isq::voltage>` gain
linearizes as a _voltage_ ratio while a `dB<isq::power>` gain linearizes as a _power_
ratio.

### Antenna gain (RF link budgets)

Antenna gain is a *power* ratio (factor 10, not 20), referenced to a standard radiator:
`dBi` against an isotropic source, `dBd` against a half-wave dipole. The two differ by the
dipole's own 2.15 dBi, so they are two origins offset by 2.15 dB, and `.in(dBi)` converts
between them like `dBm`↔`dBW`. In a link budget the gain enters as its displacement from
isotropic (a `point<dBi>` minus the isotropic `0 dBi`), which moves the `dBm` level:

```cpp
// antenna gain vs a reference radiator: dBi (isotropic, gain 1), dBd (half-wave dipole)
inline constexpr struct dBi : named_unit<"dBi", dB<dimensionless>, one> {} dBi;
inline constexpr struct dBd : named_unit<"dBd", dBi, mag_power<10, 43, 200>> {} dBd;  // 0 dBd = 2.15 dBi

quantity tx   = point<dBm>(30.0);                    // 30 dBm = 1 W
quantity gain = point<dBi>(12.0) - point<dBi>(0.0);  // 12 dB gain over isotropic
quantity eirp = tx + gain;                           // 42 dBm EIRP
```

### Transfer functions (microphone sensitivity, transfer impedance)

A transfer function such as microphone sensitivity (`V/Pa`) is, by IEC 80000-15 §5.2
convention, a root-power quantity (coefficient 2). The framework cannot *derive* that: a
product or quotient of root-power quantities is not generally root-power
(`voltage * current` is a *power* quantity), so there is no sound composition rule, and a
bare quotient `quantity_spec` has no struct to carry the coefficient. So, like every
classified quantity, it is named as **its own kind** carrying the coefficient, and `dB`
reads it, with no override:

```cpp
// the transfer function is its own root-power kind (coefficient 2); dB reads it
inline constexpr struct sensitivity :
    quantity_spec<isq::voltage / isq::sound_pressure, is_kind, log_coefficient<2>> {} sensitivity;
inline constexpr struct dB_VPa : named_unit<"dB(V/Pa)", dB<sensitivity>, V / Pa> {} dB_VPa;
```

A measured transfer function is complex (magnitude and phase). The dB level reads the
magnitude, and the phase rides on the [representation](#representation-types).

### Frequency intervals and music

Frequency intervals are always gains (a ratio of two _frequencies_, no fixed origin), all
sharing the spec `log<isq::frequency>` and differing only in base (see the catalog).
`oct`, `cent`, and `semitone` inter-convert exactly (factors 12, 100, 1200). `oct` and
`dec` are irrationally related, so the common-unit rule makes mixing them without an
explicit `.in()` ill-formed. IEC 80000-15 musical intervals are fixed ratio *values*, best
modeled as `named_constant`s (`perfect_fifth` is `700 * cent`). Absolute pitch (A4 = 440
Hz) stays in hertz. A MIDI-style pitch scale is the explicit-named-origin form from above,
with the interval as the gain.

### Information theory

The shannon, nat, and hartley are gains with a dimensionless (probability-derived) base,
sharing `log<kind_of<isq::information_content>>` and differing only in base (2, e, 10):

```cpp
inline constexpr struct shannon : named_unit<"Sh",   log<kind_of<isq::information_content>>, mag<2>,  1> {} shannon;
inline constexpr struct nat     : named_unit<"nat",  log<kind_of<isq::information_content>>, mag_e,   1> {} nat;
inline constexpr struct hartley : named_unit<"Hart", log<kind_of<isq::information_content>>, mag<10>, 1> {} hartley;
```

Because their bases are irrationally related, the common-unit rule makes `Sh + nat`
ill-formed until you convert to one base, which matches the standard's "one base per
quantity" discipline.

### pH

pH is $-\log_{10}([\mathrm{H^+}] / c_0)$ with $c_0 = 1\ \mathrm{mol/L}$. It is a **level**
(a point) over the `hydrogen_ion_concentration` kind, its own subkind of _concentration_
carrying coefficient `-1` (defined in
[Classifying the quantity](#classifying-the-quantity-the-multiplier)). The negative
coefficient means lower _concentration_ gives higher pH. The units read that coefficient
like any other, with no override. Subtracting two pH points yields a gain. Adding two is
meaningless. Both follow from the point/delta rules.

```cpp
// the kind carries the -1 coefficient; the ratio unit (factor 1) reads it, then the level adds 1 mol/L
inline constexpr struct pH_interval : named_unit<"pH", log<hydrogen_ion_concentration>, /*base*/ mag<10>, log_coefficient_of<hydrogen_ion_concentration>> {} pH_interval;
inline constexpr struct pH_unit     : named_unit<"pH", pH_interval, mol / L> {} pH_unit;
```

### Stellar magnitude

Apparent magnitude follows the Pogson relation $m_1 - m_2 = -2.5\log_{10}(E_1/E_2)$ over
_irradiance_. It is a **level** over its own kind (a subkind of _irradiance_) carrying
coefficient `-5/2`, with the reference flux (Vega, or the AB system) as the origin. One
magnitude is a flux factor of $10^{0.4} \approx 2.512$. Brighter stars have lower (more
negative) magnitudes. A difference of two magnitudes is a flux ratio on the Pogson scale,
which astronomers use routinely (a 5-magnitude difference is exactly a factor of 100 in
flux).

### Out of scope

Some logarithmic-looking quantities do not fit the fixed-reference level model and we
explicitly defer them:

- **Richter magnitude** uses a reference that depends on epicenter distance, not a
  constant in the unit. It is a computed-reference case.
- **Psychoacoustic loudness** (phon, sone) needs frequency-weighted integration, not just
  a log transform.
- **Decay constants** (`Np/s`) and **optical depth** ($\tau = \int \alpha\,dx$) are
  application space. The `Np/m` rate itself is well-formed, but these composite quantities
  are left to client code.

## Open questions

This is the section we are writing the article for. Each question lists the alternatives
we weighed, with an honest good-parts and drawbacks breakdown, and our current preference.
We will not finalize any of these until we have heard from people who use these quantities
daily.

### 1. The spec template name

This is a live question for **mp-units** V3, not only for eventual standardization. `log`
is already a function (the `std::log` overload set, and the `log` this library calls to
cross domains), so a class template `log<QS>` beside it reads ambiguously under a
`using namespace`: `log(x)` is a call and `log<power>` is a template-id, and asking a
reader or an overload resolver to hold both at once is avoidable friction.

- **Keep `log<QS>` (Option A).** Good parts: concise, and it is what reads most
  naturally in this article. Drawbacks: it clashes with the `log` function under `using`,
  so it needs qualification exactly where units code tends to be most casual.
- **Rename to `logarithmic<QS>` (Option B).** Good parts: it parallels `delta<QS>` and
  `point<QS>`, conflicts with nothing, and is the one spelling that also survives a move
  into `std`. Drawbacks: more verbose at the point of use.
- **Provide both (Option C).** Ship `logarithmic<QS>` as the canonical name with `log<QS>`
  as a short alias. Good parts: brevity where it is unambiguous, safety elsewhere.
  Drawbacks: two public names for one concept dilute the documentation and the teaching.

Author preference is `logarithmic<QS>` as the canonical name (Option B), adopted in V3 so
the eventual standard name never has to change. The namespace placement that would let a
short `log` coexist safely is itself unsettled in [P3045](https://wg21.link/p3045).

### 2. Should the named conversions also have a constructor?

The named functions `.linear()` and `.log_in()` are settled. They are the crossing between
the separate `log<QS>` and `QS` trees, and a fully-typed return keeps construction
CTAD-friendly (`quantity q = (10 * mW).log_in(dBm);` deduces its type, where a constructor
would force you to spell it out). The only open part is whether to add a constructor *on
top of* them.

- **Functions only (Option A).** No constructor. Good parts: the cross-kind step is always
  named at the call site, and it stays out of the unit-conversion machinery where it does
  not belong (it is nonlinear, not a magnitude).
- **Also an explicit constructor (Option B).** Add `quantity<point<dBm>>{10 * mW}`. Good
  parts: it pays off once a user aliases the specialization, where the alias already names
  the type and the brace-init reads cleanly:

    ```cpp
    using rf_power = quantity<point<dBm>>;
    rf_power level{10 * mW};            // explicit, and the alias carries the name
    ```

    Drawbacks: a constructor reads like an ordinary within-kind conversion, which slightly
    hides that this one crosses kinds.

An *implicit* constructor (`quantity<point<dBm>> x = 10 * mW;`) is not on the table. It is
exactly the cross-kind implicit conversion **mp-units** forbids, the same rule that rejects
`length`→`duration`.

Author preference leans to the explicit constructor of Option B for alias-heavy code, which
is why we keep it open rather than settling on functions-only.

### 3. `point_for` across level units

`.in(dBm)` converts a `dBW` level to `dBm` and changes two things together: the display
unit *and* the embedded reference origin (a value measured from 1 W becomes the same power
measured from 1 mW). That is the behavior you want.

The affine helper `point_for(origin)` is different. It rebases a point onto a given origin
while leaving the unit alone. For a linear quantity that is harmless, because the unit
carries no reference (metres stay metres). For a **level unit** it is not, because the unit
*does* carry a reference: `point<dBW>(0.0).point_for(O)`, where `O` is the 1 mW origin,
would leave the value in `dBW` (reference 1 W) while anchoring it at 1 mW, a unit whose
embedded reference contradicts its origin. The question is what that call should do.

- **Ill-formed (Option A).** Reject it and point the user to `.in(dBm)`, which changes unit
  and origin together. Good parts: it can never produce a unit whose reference disagrees
  with its origin. Drawbacks: the user must remember that `.in()`, not `point_for`, is the
  tool for level units.
- **Silently switch the unit (Option B).** Treat `point_for(O)` as the matching `.in()`.
  Good parts: convenient. Drawbacks: a function whose name says "origin" quietly changes the
  *unit*, which is surprising.

Author preference leans to Option A, keeping `point_for` ill-formed for level units and
reserving combined unit-and-origin changes for `.in()`. We raise it because this is the one
affine helper whose linear-domain meaning does not carry over cleanly to a
reference-carrying unit.

### 4. What to do at the bottom of the scale (`log(0)`)

This is the question we most want audio, RF, and acoustics engineers to answer. The
linear-to-log conversion computes $\log(\text{linear} / \text{reference})$. For an input
at or below zero, IEEE 754 gives $\log(0) = -\infty$ and
$\log(\text{negative}) = \mathrm{NaN}$. Our baseline imposes no runtime check and lets
those values flow, which is mathematically honest.

Two real concerns push against the bare baseline:

1. Not every representation has an infinity or a NaN.
2. Engineering pipelines expect to keep computing. Digital audio converts a linear
   amplitude of 0 to dB constantly and must not poison the result with `-inf` or `NaN`. In
   practice the DSP does not use `-std::numeric_limits<T>::infinity()`. It uses a large
   finite sentinel. A mathematically positive value can also land just below zero through
   floating-point round-off, for example after subtracting two nearly equal linear values,
   and turn into a spurious `-inf`/`NaN`.

A real production conversion from a digital-audio codebase looks like this (the `8.6859`
constant is the root-power factor $20 / \ln 10$):

```cpp
template<std::floating_point T>
T SampleLevelToDecibel(T x)
{
  return x < T(1.0e-20) ? T(-400) : T(8.6858896380650365530225783783335) * std::log(x);
}
```

The alternatives we are weighing:

- **Undefined behavior.** Works everywhere, but it is not in the spirit of a safety-first
  library.
- **Throw.** Useful for some domains, but not all platforms or codebases use exceptions.
- **Return an error type** (an `undefined_math` value that poisons downstream results).
- **A customization point** supplying an `(epsilon, sentinel)` pair so the conversion
  saturates to a large finite value, most likely keyed on the unit (a generic `double` has
  no floor, but `dBFS` wants `-400`).

Shipping code already picks finite floors, and they differ: the
[ossia/jamoma](https://github.com/ossia/libossia/blob/4e8e08c05ce614aa1c64fd3c5bf157227b0d4e0a/src/ossia/network/dataspace/gain.hpp#L24)
gain dataspace clamps to `-96` dB to avoid `-inf` when ramping, and the audio helper above
uses `-400`. Different floors for different units is the case for keying the customization
point on the unit.

Author preference leans toward the customization point, keyed on the unit, with the bare
IEEE 754 behavior as the default when no floor is configured. We would like to hear how
your codebase handles this today before we commit.

### 5. Reference in the unit or on the quantity

This is purely a **formatting** choice. In both options the named level units (`dBm`,
`dBW`, `dB SPL`, ...) still exist and the reference is still the point's origin. The only
question is what the formatter writes for a level.

ISO 80000-1 puts the reference on the *quantity*, not the unit: the unit is `dB`, and the
reference is written `L_p (re 1 mW)`, or as a subscript on the quantity symbol. Industry
writes `dBm`, `dBW`, `dBV`, `dB SPL`.

- **Print the industry symbol (Option A).** `point<dBm>(10.0)` formats as `"10 dBm"`. Good
  parts: it matches what RF and acoustics engineers read every day, concise and
  self-describing. Drawbacks: non-conformant with ISO 80000-1, which does not allow the
  reference on the unit symbol.
- **Print the conformant form (Option B).** The same value formats as `"10 dB (re 1 mW)"`.
  Good parts: ISO-conformant, and it states the reference explicitly. Drawbacks: more
  verbose, and it drops the compact symbol everyone recognizes.

Author preference is Option A, the industry symbol, but we especially want to hear from the
RF and acoustics communities who read `dBm` and `dB SPL` daily.

### 6. Should the classification have a default?

The power-versus-root-power tag needs a policy for quantities that carry no tag. This
mirrors `quantity_spec`, whose character defaults to `quantity_character::real_scalar` and
is stated only when it differs.

- **Default to power (Option A).** An untagged quantity is treated as power (factor 10),
  and only root-power quantities carry a tag. Good parts: the common case writes nothing,
  it follows the `real_scalar` precedent, and it matches how other libraries pick a
  sensible default. Drawbacks: a root-power quantity left untagged is silently wrong
  (factor 10 instead of 20).
- **No default; `unclassified` (Option B).** Every quantity used logarithmically must be
  classified explicitly, and `dB<QS>` is ill-formed until it is. Good parts: no silent
  misclassification, and it turns the audit into a compile-time checklist (it is also what
  makes `dB<power²>` ill-formed). Drawbacks: more ceremony, and it breaks the "good
  default you do not restate" convention the rest of the library follows.

Author preference is Option A, a power default, for consistency with how the library
already treats `real_scalar`. We raise Option B because the silent-misclassification
hazard is real, and some users may prefer the library refuse to guess.

## How to give feedback

This article exists to gather as much expert feedback as we can before implementation, so
please do not be shy.

- The **comments section below** (powered by GitHub Discussions) is the best place for
  detailed arguments, corrections to the prior-art survey, and answers to the open
  questions. Please tag your answer with the question number.
- If we have mischaracterized another library, tell us, and we will fix the survey.

!!! question "The six open questions, at a glance"

    Answer by number in the comments. Our current lean is in parentheses.

    1. Spec template name: `log<QS>`, `logarithmic<QS>`, or both. (`logarithmic<QS>`)
    2. A constructor on top of `.linear()` / `.log_in()`? (explicit, for alias-heavy code)
    3. `point_for` onto a level unit's origin: ill-formed or silent unit switch.
       (ill-formed)
    4. `log(0)` at the bottom of the scale: UB, throw, error type, or a unit-keyed
       customization point. (customization point)
    5. Reference in the unit (`dBm`) or on the quantity (`dB (re 1 mW)`). (industry `dBm`)
    6. Classification default: power, or an explicit `unclassified`. (power)

## Conclusion

Logarithmic quantities did not need a new abstraction. They are the affine point/delta
model we already adopted for absolute quantities, specialized to a domain where the origin
is a reference level and the displacement is a gain. Modeling them this way makes
`level - level` a gain, `level + gain` a level, and `level + level` a compile-time error,
and it makes `.linear()` return the right number because the gain carries its domain. That
domain is also what fixes the power-versus-root-power factor other libraries miss: a gain
linearizes with `10 log` or `20 log` according to its kind, where the prior art we surveyed
either hardcodes 10 or leaves the choice to the user. The same machinery covers RF, audio,
acoustics, music, information theory, chemistry, and astronomy, because each is the same
structure over a different quantity kind.

We believe this is correct, and we believe it is novel. That combination is why we are
publishing the design before writing the code. Tell us where we are wrong.

## Acknowledgments

Special thanks to **Roth Michaels**, who helped draft the initial design. He arrived at
the same affine point/delta model independently, pushed for domain-carrying gains, and
contributed the digital-audio perspective behind the `log(0)` question.
