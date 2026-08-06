# Working with Measurement Uncertainty

Every measured value carries an uncertainty, and every result derived from it inherits one.
A program that stores measurements in a plain `double` silently claims they are exact, and
happily prints ten significant digits of a result that the input data justifies to three.

`mp_units::utility::uncertain<T>` fixes that at the representation level. It pairs a central
value with its standard uncertainty and propagates the uncertainty automatically through
arithmetic. Because it satisfies the `RepresentationOf` concept, it works with every quantity,
unit conversion, and math function that its underlying type supports:

```cpp
#include <mp-units/systems/si.h>
#include <mp-units/utility/uncertain.h>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using mp_units::utility::uncertain;

quantity length = uncertain{10.0, 0.1} * m;   // 10.0 ± 0.1 m
quantity width = uncertain{5.0, 0.05} * m;    // 5.0 ± 0.05 m
quantity area = length * width;               // 50.0 ± 0.71 m²
quantity in_mm = length.in(mm);               // 10 000 ± 100 mm
```

## The `uncertain<T>` class

The class stores the central value and the absolute standard uncertainty (σ) and exposes
them through observers:

| Observer                 | Meaning                           |
|--------------------------|-----------------------------------|
| `value()`                | the central value                 |
| `uncertainty()`          | the absolute standard uncertainty |
| `relative_uncertainty()` | `uncertainty() / abs(value())`    |
| `lower_bound()`          | `value() - uncertainty()`         |
| `upper_bound()`          | `value() + uncertainty()`         |

The relative standard uncertainty divides by the *absolute* value, following GUM and the VIM:
it is a magnitude and is never negative, while the central value may well be (CODATA publishes
negative magnetic moments and g-factors).

An exact value converts to `uncertain` implicitly as `value ± 0`. This is intentional: the
conversion is lossless, and it lets exact quantities join the propagation in
mixed-representation arithmetic (e.g., `uncertain_length + 5.0 * m`). Providing a non-zero
uncertainty requires explicit construction.

The element type `T` must be a **real scalar** (`mp_units::utility::RealScalar`) that the
library also allows as a quantity representation. That is deliberately narrower than what
a `quantity` accepts in general: a quantity representation may be complex, a vector, or a
tensor, while `uncertain<T>` needs a real field, because every propagation formula here is
real-valued, and a total order, because the constructor checks that the uncertainty is not
negative.

In practice `double`, `float`, and the integral types qualify. Two kinds of types do not:

- **complex types**, because the propagation formulas are real-valued and the uncertainty
  of a complex value is naturally per-component rather than a single number,
- **`bool`**, which the library refuses to store in a `quantity` at all, so wrapping it could
  not produce a usable representation either.

Constraining the element type means an unsupported one is rejected where `uncertain<T>` is
named, rather than deep inside a propagation formula.

Comparison is total. The order is primarily by the central value, with the uncertainty as
a tie-breaker, which keeps it consistent with equality. A real scalar representation is
required to be `std::totally_ordered`, so a type without `<` could not be used for
a real scalar quantity at all, and features such as the `clamp_to_range` and
`clamp_non_negative` overflow policies apply `<` directly to the representation.

The order is a structural one, not a statistical statement. Two values whose uncertainty
intervals overlap still compare as unequal. For interval-aware comparisons, use the
`lower_bound()`/`upper_bound()` observers explicitly.

## Uncertainty propagation

`uncertain<T>` implements standard first-order uncertainty propagation for statistically
independent values:

<!-- markdownlint-disable MD056 -->
| Operation                    | Formula                                     |
|------------------------------|---------------------------------------------|
| Addition: `z = x + y`        | `σ_z² = σ_x² + σ_y²`                        |
| Subtraction: `z = x - y`     | `σ_z² = σ_x² + σ_y²`                        |
| Multiplication: `z = x × y`  | `σ_z² = (y·σ_x)² + (x·σ_y)²`                |
| Division: `z = x / y`        | `σ_z² = (σ_x/y)² + (x·σ_y/y²)²`             |
| Scalar multiply: `z = k × x` | `σ_z = |k| × σ_x`                           |
| Power: `z = x^n`             | `σ_z = |n × z/x × σ_x|`                     |
| Square root: `z = √x`        | `σ_z = σ_x / (2√x)`                         |
| Cube root: `z = ∛x`          | `σ_z = σ_x / (3·z²)`                        |
| Exponential: `z = exp(x)`    | `σ_z = |z × σ_x|`                           |
| Logarithm: `z = ln(x)`       | `σ_z = |σ_x / x|`                           |
| Base-10 log: `z = log10(x)`  | `σ_z = |σ_x / (x·ln 10)|`                   |
| Base-2 log: `z = log2(x)`    | `σ_z = |σ_x / (x·ln 2)|`                    |
| Sine: `z = sin(x)`           | `σ_z = |cos(x)| × σ_x`                      |
| Cosine: `z = cos(x)`         | `σ_z = |sin(x)| × σ_x`                      |
| Tangent: `z = tan(x)`        | `σ_z = σ_x / cos²(x)`                       |
| Arc sine: `z = asin(x)`      | `σ_z = σ_x / √(1 - x²)`                     |
| Arc cosine: `z = acos(x)`    | `σ_z = σ_x / √(1 - x²)`                     |
| Arc tangent: `z = atan(x)`   | `σ_z = σ_x / (1 + x²)`                      |
| `z = atan2(y, x)`            | `σ_z² = ((x·σ_y)² + (y·σ_x)²) / (x² + y²)²` |
<!-- markdownlint-enable MD056 -->

The trigonometric functions take and return radians, matching the standard library. A useful
property falls out of the derivative form: at an extremum the sensitivity vanishes, so
`sin(π/2 ± 0.01)` reports an uncertainty of zero to first order. That is a real property of
the linear approximation rather than a claim of infinite precision, and it is one of the
places where the second-order term is the one that matters.

Most unit conversions multiply the representation by an exact factor, so they scale the
central value and the uncertainty together and the relative uncertainty stays intact.
Symbolic constants like `π` are exact and contribute zero uncertainty, so `sqrt(area / π)`
recovers `5 ± 0.1 m` from `25 ± 1 π m²` without any accumulated error. See the
[Weighing the Earth](../../examples/weighing_the_earth.md) example for a complete
walk-through.

A conversion factor built from *measured* constants is the exception: it is itself a
measured value, so it contributes uncertainty of its own. That case is covered in
[Conversions materialize the uncertainty](#conversions-materialize-the-uncertainty) below.

!!! warning "Assumptions and limitations"

    - **Independent values.** The class does not track correlations, so `x - x` reports a
      non-zero uncertainty instead of zero. Full rigor for correlated values requires
      covariance matrices and is out of scope. If your problem needs correlation tracking,
      use a dedicated uncertainty library.
    - **First-order approximation.** The formulas use the linear term of the Taylor
      expansion. They are accurate for small relative uncertainties (below roughly 10%) and
      degrade for highly non-linear functions.
    - **Gaussian statistics.** The uncertainty is interpreted as one standard deviation of
      a normally distributed error. Systematic errors need a different treatment.

## Measured constants

Physical constants in **mp-units** are units with exact symbolic magnitudes (see
[Faster-than-lightspeed constants](../../users_guide/framework_basics/faster_than_lightspeed_constants.md)).
For constants that are exact by definition (the SI defining constants, the IAU nominal
values), that is the whole story. Constants that are *measured* additionally declare their
uncertainty in one of two forms, whichever their source publishes: the absolute
`standard_uncertainty` or the unit-invariant `relative_standard_uncertainty`. The absolute
form is the common one, transcribed from the source metrology table:

```cpp
// CODATA 2018: G = 6.674 30(15) × 10⁻¹¹ m³ kg⁻¹ s⁻²
//   value:                 6.674 30 × 10⁻¹¹   -> the unit magnitude
//   standard uncertainty:  0.000 15 × 10⁻¹¹   -> the metadata below
inline constexpr struct newtonian_constant_of_gravitation final :
    named_constant<"G", mag_ratio<667'430, 100'000> * mag_power<10, -11> * cubic(si::metre) / si::kilogram / square(si::second),
                   standard_uncertainty{mag_ratio<15, 10> * mag_power<10, -15> * cubic(si::metre) / si::kilogram / square(si::second)}> {}
newtonian_constant_of_gravitation;
```

The `(15)` in the concise ISO 80000-6 notation is the *absolute* standard uncertainty
(`0.000 15 × 10⁻¹¹ m³ kg⁻¹ s⁻²`), and `standard_uncertainty` stores exactly that, spelled
as a magnitude times a unit of the constant's own dimension. Transcribing the published
pair verbatim matters: NIST rounds the value and its uncertainty consistently, both to two
significant digits of the uncertainty, so a definition that stored only the *relative*
form could not reproduce the published absolute one (deriving it can be off by several
percent in the last digit, 6.1% for the fine-structure constant). The absolute form is
what `uncertain<T>` carries, prints, and propagates, so it is the number to get right.

A second wrapper, `relative_standard_uncertainty{mag_ratio<22, 10> * mag_power<10, -5>}`,
remains available for the cases where the source publishes only the relative form, or
where the constant's defining unit has no tabulated absolute uncertainty. A measured
constant declares exactly one of the two. Declaring both is not allowed, because the
published pair is mutually rounded and carrying both would embed a contradiction.

The uncertainty is stored as an exact symbolic expression, so no representation type is
imposed at the definition point. It is metadata only and never participates in unit
equality, conversion factors, or symbolic simplification. Constants defined this way satisfy
the [`MeasuredConstant`](../../users_guide/framework_basics/concepts.md#MeasuredConstant)
concept. `get_standard_uncertainty(constant)` and
`get_relative_standard_uncertainty(constant)` each return the declared form or derive the
other one on demand. The derivation is an exact ratio of canonical magnitudes
(`u_r = u(x)/|x|`), so it involves no floating-point rounding, and it never carries a sign
even for a negative-valued constant.

## Conversions materialize the uncertainty

The uncertainty of a measured constant is *relational*. In its own unit the constant is
exactly `1`, and "two solar masses" is an exact statement even though `iau::solar_mass` is
defined through `G`. What is uncertain is the *conversion factor* between such a unit and
others: how many kilograms a solar mass really is depends on how well `G` is known.

This is why the conversion engine, not the value, owns the uncertainty. Whenever a
conversion's factor is built from measured constants and the representation type can store
an uncertainty, the factor's relative standard uncertainty is folded into the result in
quadrature. For every other representation type the conversion stays exact, so nothing
changes for regular code:

```cpp
const quantity two_suns = 2.0 * iau::unit_symbols::M_SUN;

std::cout << two_suns << "\n";                                     // 2 M_☉
std::cout << two_suns.in(kg) << "\n";                              // 3.97682e+30 kg
std::cout << two_suns.in<uncertain<double>>(kg) << "\n";           // 3.97682e+30 ± 8.93761e+25 kg
std::cout << value_cast<kg, uncertain<double>>(two_suns) << "\n";  // 3.97682e+30 ± 8.93761e+25 kg
```

The first two lines are exact: `two_suns` in its own unit, and the sanctioned central
value in kilograms. The third opts into the uncertainty of `G` at the conversion,
and the fourth spells the same opt-in as a cast.

The derivation walks both units' definitions and accumulates the net exponent of every
measured constant, so a constant contributing to both sides cancels symbolically, exactly
as its central value cancels in the magnitude ratio. Converting a solar _mass_ to Earth
_masses_ reports zero uncertainty, because both units are defined as `(GM)ᴺ/G` and their
ratio is exact by definition. Here are the same two suns again, with the
uncertainty-capable representation as the only difference:

```cpp
const quantity two_suns = uncertain<double>{2.0} * iau::unit_symbols::M_SUN;

std::cout << two_suns << "\n";                                 // 2 ± 0 M_☉
std::cout << two_suns.in(iau::unit_symbols::M_EARTH) << "\n";  // 665892 ± 0 M_⊕
std::cout << two_suns.in(kg) << "\n";                          // 3.97682e+30 ± 8.93761e+25 kg
```

`two_suns` is exactly two suns even in an uncertainty-capable representation. `G` cancels
in the conversion to Earth masses, so the reported uncertainty stays exactly zero there
too. Only in kilograms does it survive, and since the representation type can hold it, no
explicit opt-in is needed anymore.

The surviving contributions combine in quadrature under first-order propagation:
`u_r = √(Σ (nᵢ · u_r,ᵢ)²)`. Cross-constant independence is assumed. The covariances CODATA
publishes between its values are not modeled, so when several correlated constants survive
into one factor the result can err in either direction, depending on the signs of their
exponents and of the correlation. The fully correlated case that matters most, the same
constant appearing on both sides, is not approximated at all: it cancels before the formula
is applied.

## `measurement_of`

`measurement_of` is a factory function that turns a measured constant into a quantity with
an uncertainty-capable representation. Its argument is constrained to the `MeasuredConstant`
concept, so it accepts exactly the constants that declare a standard uncertainty.
The representation's underlying numeric type defaults to `double`, consistent with the rest
of the library, and can be chosen explicitly.

It returns the quantity `1` of the constant, and, following the rule above, the uncertainty
appears the moment the quantity leaves units in which the constant cancels:

```cpp
#include <mp-units/systems/iau.h>
#include <mp-units/utility/uncertain.h>

using mp_units::utility::measurement_of;

quantity G1 = measurement_of(iau::newtonian_constant_of_gravitation);  // exactly 1 G
quantity G2 = G1.in(m3 / kg / s2);              // 6.6743e-11 ± 1.5e-15 m³ kg⁻¹ s⁻²
quantity G3 = measurement_of<float>(iau::newtonian_constant_of_gravitation);  // uncertain<float>
```

Seeding the value with `1 ± u_r` up front would be wrong: a later conversion would fold the
factor's `u_r` on top of the value's identical, fully correlated one and double-count it.
The relational model avoids this by construction, and derived values compose correctly at
the quantity level:

```cpp
const quantity G_measured = measurement_of(iau::newtonian_constant_of_gravitation);
const quantity solar_mass = (1. * iau::nominal_solar_mass_parameter) / G_measured;
std::cout << solar_mass.in(kg) << "\n";  // 1.98841e+30 ± 4.46881e+25 kg
```

The nominal mass parameter is exact by definition, so the result carries exactly `G`'s
relative uncertainty (`1.5 × 10⁻¹⁵ / 6.6743 × 10⁻¹¹ ≈ 2.25 × 10⁻⁵`).

This is where the `MeasuredConstant` constraint earns its keep. Calling `measurement_of` (or
either uncertainty accessor) on a constant that is exact by definition does not
compile. "Exact by definition" and "measured infinitely precisely" are different
metrological statements, so the library refuses to return `±0` for the former.

## Text output

Both stream insertion and `std::format` print the `value ± σ` form, and a quantity appends
its unit symbol:

```cpp
std::cout << std::format("{}", uncertain{2.5, 0.25} * m) << "\n";  // 2.5 ± 0.25 m
```

### The concise ISO 80000 notation

ISO 80000-1:2022, 7.2.4 specifies a second notation, in which the value is quoted to the
last significant digit of the uncertainty and the uncertainty follows in parentheses,
counted in units of that digit. The standard's own example reads:

!!! quote "ISO 80000-1:2022, 7.2.4"

    In the expression $l = 23{,}478\,2(32)\ \mathrm{m}$, $23{,}478\,2$ is the numerical
    value and $32$ represents a standard uncertainty equal to $0{,}003\,2$.

Appending `~` to the `format-spec` selects it. Where the default form prints both
components losslessly (the view of what the object holds), this one quotes the value to
the precision the uncertainty justifies (the reporting view of GUM 7.2.6). For a constant
transcribed from a metrology table the output reproduces the published notation verbatim.

```cpp
std::cout << std::format("{:~}", uncertain{23.4782, 0.0032}) << "\n";  // 23.4782(32)
```

This is the notation CODATA and the SI Brochure use to publish measured constants, so the
output reproduces the source that the definitions were transcribed from:

```cpp
quantity G = measurement_of(iau::newtonian_constant_of_gravitation).in(m3 / kg / s2);
std::cout << std::format("{::N[~]}", G) << "\n";  // 6.67430(15)e-11 m³ kg⁻¹ s⁻²
```

In the concise form `precision` states the number of significant digits of the uncertainty
rather than a digit count for the value. It defaults to two, which is what CODATA publishes
and what GUM 7.2.6 recommends:

```cpp
std::cout << std::format("{:.1~}", uncertain{6.67430e-11, 1.8e-15}) << "\n";  // 6.6743(2)e-11
```

`type` selects between the scientific (`e`) and the fixed (`f`) form. Left out, the library
picks the one that leaves no shown digit of the value ambiguous:

```cpp
std::cout << std::format("{:~}", uncertain{1234., 15.}) << "\n";   // 1234(15)
std::cout << std::format("{:~}", uncertain{1234., 150.}) << "\n";  // 1.23(15)e+03
```

The second case shows why. The uncertainty reaches the tens digit, so a fixed `1230(15)`
would wrongly read as 15 units of the last digit shown. Quoting the value as a significand
keeps the count honest. Two cases fall outside the notation and print the `±` form instead:
a non-finite value or uncertainty. A zero uncertainty prints as `(0)`, because there is no
rounding rule to apply and every digit of the value is kept.

### Which form to use

The library keeps `value ± σ` as the default, because the concise form necessarily rounds
the value to the precision of the uncertainty, and a default that silently discards digits
is the wrong default. Where the output is meant for publication rather than for debugging,
prefer the concise form, and note that the standard is not neutral between the two:

!!! quote "ISO 80000-1:2022, 7.2.4"

    Uncertainties are often expressed in the following manner: $(23{,}478\,2 \pm
    0{,}003\,2)\ \mathrm{m}$. This is, however, wrong from a mathematical point of view.
    [...] According to ISO/IEC Guide 98-3:2008, 7.2.2, "The ± format should be avoided
    whenever possible because it has traditionally been used to indicate an interval
    corresponding to a high level of confidence and thus may be confused with expanded
    uncertainty".

Both forms in this library print one standard uncertainty, never an expanded one.
