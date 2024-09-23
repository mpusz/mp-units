# Faster-than-lightspeed Constants

In most libraries, physical constants are implemented as constant (possibly `constexpr`)
quantity values. Such an approach has some disadvantages, often affecting the run time
performance and causing a loss of precision.


## Simplifying constants in an equation

When dealing with equations involving physical constants, they often occur more than once
in an expression. Such a constant may appear both in a numerator and denominator of
a quantity equation. As we know from fundamental physics, we can simplify such an expression
by striking a constant out of the equation. Supporting such behavior allows a faster runtime
performance and often a better precision of the resulting value.


## Physical constants as units

The **mp-units** library allows and encourages the implementation of physical constants as
regular units. With that, the constant's value is handled at compile-time, and under
favorable circumstances, it can be simplified in the same way as all other repeated
units do. If it is not simplified, the value is stored in a type, and the expensive
multiplication or division operations can be delayed in time until a user selects
a specific unit to represent/print the data.

Such a feature often also allows using simpler or faster representation types in the equation.
For example, instead of always having to multiply a small integral value with a big
floating-point constant number, we can just use the integral type all the way. Only
in case a constant will not simplify in the equation, and the user will require a specific
unit, such a multiplication will be lazily invoked, and the representation type will
need to be expanded to facilitate that. With that, addition, subtractions, multiplications,
and divisions will always be the fastest - compiled away or done in out-of-order execution.

To benefit from all of the above, in the **mp-units** library, SI defining and other constants
are implemented as units in the following way:

```cpp
namespace si {

namespace si2019 {

inline constexpr struct speed_of_light_in_vacuum final :
  named_unit<"c", mag<299'792'458> * metre / second> {} speed_of_light_in_vacuum;

}  // namespace si2019

inline constexpr struct magnetic_constant final :
  named_unit<{u8"μ₀", "u_0"}, mag<4> * mag<pi> * mag_power<10, -7> * henry / metre> {} magnetic_constant;

}  // namespace mp_units::si
```


## Usage examples

With the above definitions, we can calculate _vacuum permittivity_ as:

```cpp
constexpr auto permeability_of_vacuum = 1. * si::magnetic_constant;
constexpr auto speed_of_light_in_vacuum = 1 * si::si2019::speed_of_light_in_vacuum;

QuantityOf<isq::permittivity_of_vacuum> auto q = 1 / (permeability_of_vacuum * pow<2>(speed_of_light_in_vacuum));

std::println("permittivity of vacuum = {} = {::N[.3e]}", q, q.in(F / m));
```

The above first prints the following:

```text
permittivity of vacuum = 1  μ₀⁻¹ c⁻² = 8.854e-12 F/m
```

As we can clearly see, all the calculations above were just about multiplying and dividing
the number `1` with the rest of the information provided as a compile-time type. Only when
a user wants a specific SI unit as a result, the unit ratios are lazily resolved.

Another similar example can be an equation for _total energy_:

```cpp
QuantityOf<isq::mechanical_energy> auto total_energy(QuantityOf<isq::momentum> auto p,
                                                     QuantityOf<isq::mass> auto m,
                                                     QuantityOf<isq::speed> auto c)
{
  return isq::mechanical_energy(sqrt(pow<2>(p * c) + pow<2>(m * pow<2>(c))));
}
```

```cpp
constexpr auto GeV = si::giga<si::electronvolt>;
constexpr QuantityOf<isq::speed> auto c = 1. * si::si2019::speed_of_light_in_vacuum;
constexpr auto c2 = pow<2>(c);

const auto p1 = isq::momentum(4. * GeV / c);
const QuantityOf<isq::mass> auto m1 = 3. * GeV / c2;
const auto E = total_energy(p1, m1, c);

std::cout << "in `GeV` and `c`:\n"
          << "p = " << p1 << "\n"
          << "m = " << m1 << "\n"
          << "E = " << E << "\n";

const auto p2 = p1.in(GeV / (m / s));
const auto m2 = m1.in(GeV / pow<2>(m / s));
const auto E2 = total_energy(p2, m2, c).in(GeV);

std::cout << "\nin `GeV`:\n"
          << "p = " << p2 << "\n"
          << "m = " << m2 << "\n"
          << "E = " << E2 << "\n";

const auto p3 = p1.in(kg * m / s);
const auto m3 = m1.in(kg);
const auto E3 = total_energy(p3, m3, c).in(J);

std::cout << "\nin SI base units:\n"
          << "p = " << p3 << "\n"
          << "m = " << m3 << "\n"
          << "E = " << E3 << "\n";
```

The above prints the following:

```text
in `GeV` and `c`:
p = 4 GeV/c
m = 3 GeV/c²
E = 5 GeV

in `GeV`:
p = 1.33426e-08 GeV s/m
m = 3.33795e-17 GeV s²/m²
E = 5 GeV

in SI base units:
p = 2.13771e-18 kg m/s
m = 5.34799e-27 kg
E = 8.01088e-10 J
```
