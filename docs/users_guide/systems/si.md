# The International System of Units (SI)

## Overview

The **International System of Units** (SI) is the modern form of the metric system and
the most widely used system of measurement worldwide. As defined by the SI Brochure:

!!! quote "SI Brochure (9th edition)"

    The SI is a consistent system of units for use in all aspects of life, including
    international trade, manufacturing, security, health and safety, protection of the
    environment, and in the basic science that underpins all of these. The system of
    quantities underlying the SI and the equations relating them are based on the present
    description of nature and are familiar to all scientists, technologists and engineers.

!!! info

    For a general introduction to systems of units and how they relate to systems of
    quantities, see [Systems of Units](../framework_basics/systems_of_units.md) in the
    Framework Basics section.


## Relationship with ISQ

The SI is explicitly based on the [International System of Quantities (ISQ)](isq.md).
In **mp-units**, this relationship is expressed through the namespace structure - all SI
definitions are found within `mp_units::si`, which builds upon the ISQ foundation.


## Base Units

The SI defines seven base units, one for each ISQ base quantity. The SI Brochure states:

!!! quote "SI Brochure (9th edition)"

    Prior to the definitions adopted in 2018, the SI was defined through seven base units
    from which the derived units were constructed as products of powers of the base units.
    Defining the SI by fixing the numerical values of seven defining constants has the
    effect that this distinction is, in principle, not needed, since all units, base as
    well as derived units, may be constructed directly from the defining constants.
    Nevertheless, the concept of base and derived units is maintained because it is useful
    and historically well established.

The seven base units are defined in **mp-units** as:

```cpp
namespace mp_units::si {

inline constexpr struct second final : named_unit<"s", kind_of<isq::duration>> {} second;
inline constexpr struct metre final : named_unit<"m", kind_of<isq::length>> {} metre;
inline constexpr struct kilogram final : named_unit<"kg", kind_of<isq::mass>> {} kilogram;
inline constexpr struct ampere final : named_unit<"A", kind_of<isq::electric_current>> {} ampere;
inline constexpr struct kelvin final : named_unit<"K", kind_of<isq::thermodynamic_temperature>> {} kelvin;
inline constexpr struct mole final : named_unit<"mol", kind_of<isq::amount_of_substance>> {} mole;
inline constexpr struct candela final : named_unit<"cd", kind_of<isq::luminous_intensity>> {} candela;

}
```

| Base Quantity             | SI Unit  | Symbol | **mp-units** Identifier |
|---------------------------|----------|--------|-------------------------|
| time                      | second   | s      | `si::second`            |
| length                    | metre    | m      | `si::metre`             |
| mass                      | kilogram | kg     | `si::kilogram`          |
| electric current          | ampere   | A      | `si::ampere`            |
| thermodynamic temperature | kelvin   | K      | `si::kelvin`            |
| amount of substance       | mole     | mol    | `si::mole`              |
| luminous intensity        | candela  | cd     | `si::candela`           |

### Special Case: kilogram vs gram

!!! note

    Although `kilogram` is the SI base unit of mass, the library defines both `gram` and
    `kilogram`, with `kilogram` being a `prefixed_unit` based on `gram`. This is necessary
    for proper handling of SI prefixes, as prefixes are applied to gram, not kilogram
    (e.g., milligram, not microkilogram).


## Derived Units

The SI Brochure defines:

!!! quote "SI Brochure (9th edition)"

    Derived units are defined as products of powers of the base units. When the numerical
    factor of this product is one, the derived units are called coherent derived units.

The SI provides special names for 22 coherent derived units. Here are some examples:

```cpp
namespace mp_units::si {

// Geometry
inline constexpr struct radian final : named_unit<"rad", metre / metre, kind_of<isq::angular_measure>> {} radian;
inline constexpr struct steradian final : named_unit<"sr", square(metre) / square(metre), kind_of<isq::solid_angular_measure>> {} steradian;

// Mechanics
inline constexpr struct newton final : named_unit<"N", kilogram * metre / square(second)> {} newton;
inline constexpr struct pascal final : named_unit<"Pa", newton / square(metre)> {} pascal;
inline constexpr struct joule final : named_unit<"J", newton * metre> {} joule;
inline constexpr struct watt final : named_unit<"W", joule / second> {} watt;

// Electricity and magnetism
inline constexpr struct coulomb final : named_unit<"C", ampere * second> {} coulomb;
inline constexpr struct volt final : named_unit<"V", watt / ampere> {} volt;
inline constexpr struct farad final : named_unit<"F", coulomb / volt> {} farad;
inline constexpr struct ohm final : named_unit<symbol_text{u8"Ω", "ohm"}, volt / ampere> {} ohm;

// Radioactivity
inline constexpr struct becquerel final : named_unit<"Bq", one / second> {} becquerel;
inline constexpr struct gray final : named_unit<"Gy", joule / kilogram> {} gray;

// And more...
}
```

### Expressing the Same Quantity in Different Units

As discussed in [Systems of Units](../framework_basics/systems_of_units.md#many-shades-of-the-same-unit),
the same physical quantity can be expressed using different but equivalent unit combinations:

```cpp
using namespace mp_units::si::unit_symbols;

quantity power = 42 * W;
std::cout << power << "\n";                   // 42 W
std::cout << power.in(J / s) << "\n";         // 42 J/s
std::cout << power.in(N * m / s) << "\n";     // 42 m N/s
std::cout << power.in(kg * m2 / s3) << "\n";  // 42 kg m²/s³
```

All four expressions represent exactly the same quantity of power.


## SI Prefixes

The SI defines prefixes that can be applied to any unit (except kilogram, where they apply
to gram) to create scaled versions:

```cpp
namespace mp_units::si {

template<PrefixableUnit U> struct quecto_ final : prefixed_unit<"q", mag_power<10, -30>, U{}> {};
template<PrefixableUnit U> struct ronto_  final : prefixed_unit<"r", mag_power<10, -27>, U{}> {};
template<PrefixableUnit U> struct yocto_  final : prefixed_unit<"y", mag_power<10, -24>, U{}> {};
template<PrefixableUnit U> struct zepto_  final : prefixed_unit<"z", mag_power<10, -21>, U{}> {};
template<PrefixableUnit U> struct atto_   final : prefixed_unit<"a", mag_power<10, -18>, U{}> {};
template<PrefixableUnit U> struct femto_  final : prefixed_unit<"f", mag_power<10, -15>, U{}> {};
template<PrefixableUnit U> struct pico_   final : prefixed_unit<"p", mag_power<10, -12>, U{}> {};
template<PrefixableUnit U> struct nano_   final : prefixed_unit<"n", mag_power<10, -9>, U{}> {};
template<PrefixableUnit U> struct micro_  final : prefixed_unit<symbol_text{u8"µ", "u"}, mag_power<10, -6>, U{}> {};
template<PrefixableUnit U> struct milli_  final : prefixed_unit<"m", mag_power<10, -3>, U{}> {};
template<PrefixableUnit U> struct centi_  final : prefixed_unit<"c", mag_power<10, -2>, U{}> {};
template<PrefixableUnit U> struct deci_   final : prefixed_unit<"d", mag_power<10, -1>, U{}> {};
template<PrefixableUnit U> struct deca_   final : prefixed_unit<"da", mag_power<10, 1>, U{}> {};
template<PrefixableUnit U> struct hecto_  final : prefixed_unit<"h", mag_power<10, 2>, U{}> {};
template<PrefixableUnit U> struct kilo_   final : prefixed_unit<"k", mag_power<10, 3>, U{}> {};
template<PrefixableUnit U> struct mega_   final : prefixed_unit<"M", mag_power<10, 6>, U{}> {};
template<PrefixableUnit U> struct giga_   final : prefixed_unit<"G", mag_power<10, 9>, U{}> {};
template<PrefixableUnit U> struct tera_   final : prefixed_unit<"T", mag_power<10, 12>, U{}> {};
template<PrefixableUnit U> struct peta_   final : prefixed_unit<"P", mag_power<10, 15>, U{}> {};
template<PrefixableUnit U> struct exa_    final : prefixed_unit<"E", mag_power<10, 18>, U{}> {};
template<PrefixableUnit U> struct zetta_  final : prefixed_unit<"Z", mag_power<10, 21>, U{}> {};
template<PrefixableUnit U> struct yotta_  final : prefixed_unit<"Y", mag_power<10, 24>, U{}> {};
template<PrefixableUnit U> struct ronna_  final : prefixed_unit<"R", mag_power<10, 27>, U{}> {};
template<PrefixableUnit U> struct quetta_ final : prefixed_unit<"Q", mag_power<10, 30>, U{}> {};

}
```

!!! note

    `std::ratio` can't represent eight edge cases from the above prefixes because of the
    `std::intmax_t` limitations.

### Using Prefixes

Prefixes can be applied using template syntax:

```cpp
quantity length = 5 * si::kilo<si::metre>;  // 5 km
quantity mass = 500 * si::milli<si::gram>;  // 500 mg
quantity time = 3 * si::micro<si::second>;  // 3 μs
```

Additionally, **mp-units** provide symbols not only for the coherent units, but also
for all the prefixed versions of them. This is an opt-in feature:

```cpp
using namespace mp_units::si::unit_symbols;

quantity length = 5 * km;  // 5 km
quantity mass = 500 * mg;  // 500 mg
quantity time = 3 * μs;    // 3 μs
```


## Non-SI Units Accepted for Use with SI

The SI also accepts certain non-SI units for use alongside SI units:

```cpp
namespace mp_units {
namespace non_si {

// Time
inline constexpr struct minute final : named_unit<"min", mag<60> * second> {} minute;
inline constexpr struct hour final : named_unit<"h", mag<60> * minute> {} hour;
inline constexpr struct day final : named_unit<"d", mag<24> * hour> {} day;

// Plane angle
inline constexpr struct degree final : named_unit<"°", mag<ratio{1, 180}> * mag<pi> * radian> {} degree;
inline constexpr struct arcminute final : named_unit<"'", mag_ratio<1, 60> * degree> {} arcminute;
inline constexpr struct arcsecond final : named_unit<"\"", mag_ratio<1, 60> * arcminute> {} arcsecond;

// Volume
inline constexpr struct litre final : named_unit<"l", cubic(decimetre)> {} litre;

// Mass
inline constexpr struct tonne final : named_unit<"t", mag<1000> * kilogram> {} tonne;

// Energy
inline constexpr struct electronvolt final : named_unit<"eV", mag_ratio<1'602'176'634, 10'000'000'000> * mag_power<10, -19> * joule> {} electronvolt;

// And more...
}

namespace si {

// Non-SI units are accepted for use with SI
using namespace non_si;

}
}  // namespace mp-units
```


## SI Defining Constants

Since the 2019 redefinition, the SI is defined in terms of exact values for seven defining
constants. These are provided in **mp-units**:

```cpp
namespace mp_units::si::inline si2019 {

inline constexpr struct hyperfine_structure_transition_frequency_of_cs final :
  named_unit<symbol_text{u8"Δν_Cs", "dv_Cs"}, mag<9'192'631'770> * hertz> {}
  hyperfine_structure_transition_frequency_of_cs;

inline constexpr struct speed_of_light_in_vacuum final :
  named_unit<"c", mag<299'792'458> * metre / second> {}
  speed_of_light_in_vacuum;

inline constexpr struct planck_constant final :
  named_unit<"h", mag_ratio<662'607'015, 1'000'000'000> * mag_power<10, -34> * joule * second> {}
  planck_constant;

inline constexpr struct elementary_charge final :
  named_unit<"e", mag_ratio<1'602'176'634, 1'000'000'000> * mag_power<10, -19> * coulomb> {}
  elementary_charge;

inline constexpr struct boltzmann_constant final :
  named_unit<"k", mag_ratio<1'380'649, 1'000'000> * mag_power<10, -23> * joule / kelvin> {}
  boltzmann_constant;

inline constexpr struct avogadro_constant final :
  named_unit<symbol_text{u8"N_A", "N_A"}, mag_ratio<602'214'076, 100'000'000> * mag_power<10, 23> / mole> {}
  avogadro_constant;

inline constexpr struct luminous_efficacy final :
  named_unit<symbol_text{u8"K_cd", "K_cd"}, mag<683> * lumen / watt> {}
  luminous_efficacy;

}
```

!!! note

    The `si2019` nested namespace indicates these values are from the 2019 SI definition.
    This allows for future updates if the constants are ever redefined.


## Usage Examples

### Basic Calculations

```cpp
using namespace mp_units;
using namespace mp_units::si::unit_symbols;

// Simple calculations
quantity distance = 100 * m;
quantity time = 5 * s;
quantity speed = distance / time;                 // 20 m/s

// Using derived units
quantity force = 10 * N;
quantity displacement = 2 * m;
quantity work = force * displacement;             // 20 J

// Unit conversions
quantity speed_kmph = speed.in<double>(km / h);   // 72 km/h
```

### Strong Typing with ISQ

```cpp
using namespace mp_units;
using namespace mp_units::si::unit_symbols;

// Strong quantity types from ISQ
quantity height = isq::height(1.8 * m);
quantity mass = isq::mass(75 * kg);
quantity walking_speed = isq::speed(1.4 * m / s);

// Dimensional analysis ensures correctness
quantity kinetic_energy = 0.5 * mass * pow<2>(walking_speed);
std::cout << kinetic_energy.in(J) << "\n";        // Energy in joules
```


## References

- [SI Brochure (9th edition)](https://www.bipm.org/en/publications/si-brochure)
- [International System of Quantities (ISQ)](isq.md)
- [Systems of Units](../framework_basics/systems_of_units.md) - Framework basics
