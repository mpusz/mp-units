# The International System of Units (SI)

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
inline constexpr struct newton final : named_unit<"N", kilogram * metre / square(second), kind_of<isq::force>> {} newton;
inline constexpr struct pascal final : named_unit<"Pa", newton / square(metre), kind_of<isq::pressure>> {} pascal;
inline constexpr struct joule final : named_unit<"J", newton * metre, kind_of<isq::energy>> {} joule;
inline constexpr struct watt final : named_unit<"W", joule / second> {} watt;

// Electricity and magnetism
inline constexpr struct coulomb final : named_unit<"C", ampere * second> {} coulomb;
inline constexpr struct volt final : named_unit<"V", watt / ampere, kind_of<isq::electric_potential>> {} volt;
inline constexpr struct farad final : named_unit<"F", coulomb / volt, kind_of<isq::capacitance>> {} farad;
inline constexpr struct ohm final : named_unit<symbol_text{u8"Ω", "ohm"}, volt / ampere, kind_of<isq::impedance>> {} ohm;
inline constexpr struct siemens final : named_unit<"S", one / ohm, kind_of<isq::admittance>> {} siemens;
inline constexpr struct weber final : named_unit<"Wb", volt * second> {} weber;
inline constexpr struct tesla final : named_unit<"T", weber / square(metre), kind_of<isq::magnetic_flux_density>> {} tesla;
inline constexpr struct henry final : named_unit<"H", weber / ampere> {} henry;

// Photometry
inline constexpr struct lumen final : named_unit<"lm", candela * steradian, kind_of<isq::luminous_flux>> {} lumen;
inline constexpr struct lux final : named_unit<"lx", lumen / square(metre), kind_of<isq::illuminance>> {} lux;

// Radioactivity
inline constexpr struct becquerel final : named_unit<"Bq", one / second, kind_of<isq::activity>> {} becquerel;
inline constexpr struct gray final : named_unit<"Gy", joule / kilogram, kind_of<isq::absorbed_dose>> {} gray;
inline constexpr struct sievert final : named_unit<"Sv", joule / kilogram, kind_of<isq::dose_equivalent>> {} sievert;
inline constexpr struct katal final : named_unit<"kat", mole / second, kind_of<isq::catalytic_activity>> {} katal;

// And more...
}
```

!!! note "Units not restricted to a single quantity kind"

    Some SI named units are deliberately defined **without** a `kind_of<>` constraint
    because they are legitimately used by multiple unrelated ISQ quantity hierarchies:

    | Unit | ISQ quantity hierarchies that use it |
    |------|--------------------------------------|
    | `watt (W)` | _power_, _heat flow rate_, _active power_, _radiant flux_, and more — spanning mechanics, thermodynamics, electromagnetism, and radiometry |
    | `coulomb (C)` | _electric charge_ and _electric flux_ — distinct ISQ hierarchies sharing the same dimension |
    | `weber (Wb)` | _magnetic flux_, _protoflux_, _linked magnetic flux_, _total magnetic flux_ — distinct electromagnetic ISQ quantities |
    | `henry (H)` | _inductance_ and _permeance_ — unrelated ISQ quantities with the same dimension |

    Adding `kind_of<>` to any of these units would incorrectly reject legitimate usages
    with one of the quantity hierarchies it is designed to serve.

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
  named_constant<symbol_text{u8"Δν_Cs", "dv_Cs"}, mag<9'192'631'770> * hertz> {}
  hyperfine_structure_transition_frequency_of_cs;

inline constexpr struct speed_of_light_in_vacuum final :
  named_constant<"c", mag<299'792'458> * metre / second> {}
  speed_of_light_in_vacuum;

inline constexpr struct planck_constant final :
  named_constant<"h", mag_ratio<662'607'015, 1'000'000'000> * mag_power<10, -34> * joule * second> {}
  planck_constant;

inline constexpr struct elementary_charge final :
  named_constant<"e", mag_ratio<1'602'176'634, 1'000'000'000> * mag_power<10, -19> * coulomb> {}
  elementary_charge;

inline constexpr struct boltzmann_constant final :
  named_constant<"k", mag_ratio<1'380'649, 1'000'000> * mag_power<10, -23> * joule / kelvin> {}
  boltzmann_constant;

inline constexpr struct avogadro_constant final :
  named_constant<symbol_text{u8"N_A", "N_A"}, mag_ratio<602'214'076, 100'000'000> * mag_power<10, 23> / mole> {}
  avogadro_constant;

inline constexpr struct luminous_efficacy final :
  named_constant<symbol_text{u8"K_cd", "K_cd"}, mag<683> * lumen / watt> {}
  luminous_efficacy;

}
```

!!! note

    The `si2019` nested namespace indicates these values are from the 2019 SI definition.
    This allows for future updates if the constants are ever redefined.


## Header Files

When not using C++ modules, the SI system is exposed through a set of headers under
`<mp-units/systems/si/...>`. Choose the headers that match your use case to keep
compile times as low as possible.

### Umbrella header — `<mp-units/systems/si.h>`

```cpp
#include <mp-units/systems/si.h>
```

The all-in-one header. It pulls in everything the SI system offers:

- all SI units, prefixes and SI-defining constants
- all short-name unit symbols (e.g. `m`, `km`, `Hz`) via `si::unit_symbols`
- `<chrono>` interoperability (hosted only)
- SI `math.h` overloads of `<cmath>` functions for quantities (hosted only)
- `invoke_with_prefixed` and `prefix_range` utilities (hosted only)

**When to use:** application code where SI breadth is more important than
individual include cost — the typical choice for `.cpp` files.

### Lean umbrella — `<mp-units/systems/si/core.h>`

```cpp
#include <mp-units/systems/si/core.h>
```

A lighter umbrella that includes units, prefixes, and constants but deliberately
omits `unit_symbols.h`, `chrono.h`, `math.h`, and `prefix_utils.h`.

**When to use:** library headers (`.h`/`.hpp`) that use SI units internally but
should not force `unit_symbols`, `<chrono>`, or `<cmath>` on their users. Also the
recommended include for other system headers that build on top of SI (e.g. CGS,
imperial) to avoid bringing in the full symbol table.

### Individual headers

Use these when you need only a specific slice of the SI system.

- `<mp-units/systems/si/prefixes.h>`

    Provides only the 24 SI prefix class templates (`quecto_` … `quetta_`) and their
    corresponding variable templates (`quecto` … `quetta`).
    Does **not** bring in any units, constants, or runtime utilities.

    ```cpp
    #include <mp-units/systems/si/prefixes.h>
    ```

    **When to use:** when defining a new unit system that needs SI prefixes but none
    of the SI units themselves (e.g., HEP).

- `<mp-units/systems/si/units.h>`

    Provides all SI named units (base and derived) and the non-SI units accepted for
    use with the SI (`non_si::minute`, `non_si::hour`, `non_si::litre`, …).
    Depends on `prefixes.h` and the ISQ quantity definitions.

    ```cpp
    #include <mp-units/systems/si/units.h>
    ```

- `<mp-units/systems/si/constants.h>`

    Provides the seven SI-defining constants from the 2019 redefinition
    (`si::speed_of_light_in_vacuum`, `si::planck_constant`, etc.) in the inline
    namespace `si::si2019` and a few more directly in the `si` namespace.
    Depends on `units.h`.

    ```cpp
    #include <mp-units/systems/si/constants.h>
    ```

- `<mp-units/systems/si/unit_symbols.h>`

    Opens `namespace mp_units::si::unit_symbols` and defines short-name `inline
    constexpr` variables for every SI unit and all its prefixed variants
    (`m`, `km`, `mm`, …, `Hz`, `kHz`, …).

    !!! warning

        This header instantiates a large number of prefixed units at parse time,
        which is the main compile-time cost of `si.h`.

    ```cpp
    #include <mp-units/systems/si/unit_symbols.h>

    using namespace mp_units::si::unit_symbols;
    quantity length = 5 * km;
    ```

    **When to use:** `.cpp` files or translation units where the concise symbol
    syntax is desired. Avoid in widely-included library headers.

- `<mp-units/systems/si/chrono.h>` *(hosted only)*

    Provides interoperability between `std::chrono::duration` / `std::chrono::time_point`
    and **mp-units** quantities. Includes `<chrono>`.

    ```cpp
    #include <mp-units/systems/si/chrono.h>

    auto d = std::chrono::seconds{5};
    quantity t = d;  // quantity<si::second, std::int64_t>
    ```

- `<mp-units/systems/si/math.h>` *(hosted only)*

    Provides quantity-aware overloads of the `<cmath>` transcendental and rounding
    functions (`sin`, `cos`, `pow`, `sqrt`, `floor`, `ceil`, …) that are aware of
    angular-measure semantics.

    ```cpp
    #include <mp-units/systems/si/math.h>

    quantity angle = 30 * deg;
    auto s = sin(angle);  // converts to radians internally
    ```

- `<mp-units/systems/si/prefix_utils.h>` *(hosted only)*

    Provides the `prefix_range` enum and the `invoke_with_prefixed` function template,
    which automatically selects the most readable SI prefix for a quantity and calls
    a user-supplied functor with the rescaled value.

    ```cpp
    #include <mp-units/systems/si/prefix_utils.h>

    invoke_with_prefixed([](auto q) { std::cout << q << "\n"; }, 0.005 * m, m);
    // prints: 5 mm
    ```

### At a glance

| Header              | Units | Prefixes | Constants | Symbols | chrono | math | prefix_utils |
|---------------------|:-----:|:--------:|:---------:|:-------:|:------:|:----:|:------------:|
| `si.h`              |   ✓   |    ✓     |     ✓     |    ✓    |   ✓    |  ✓   |      ✓       |
| `si/core.h`         |   ✓   |    ✓     |     ✓     |    —    |   —    |  —   |      —       |
| `si/prefixes.h`     |   —   |    ✓     |     —     |    —    |   —    |  —   |      —       |
| `si/units.h`        |   ✓   |    ✓     |     —     |    —    |   —    |  —   |      —       |
| `si/constants.h`    |   ✓   |    ✓     |     ✓     |    —    |   —    |  —   |      —       |
| `si/unit_symbols.h` |   ✓   |    ✓     |     —     |    ✓    |   —    |  —   |      —       |
| `si/chrono.h`       |   ✓   |    ✓     |     —     |    —    |   ✓    |  —   |      —       |
| `si/math.h`         |   ✓   |    ✓     |     —     |    —    |   —    |  ✓   |      —       |
| `si/prefix_utils.h` |   —   |    ✓     |     —     |    —    |   —    |  ✓   |      ✓       |

!!! note "Freestanding environments"

    Headers marked *hosted only* (`chrono.h`, `math.h`, `prefix_utils.h`) require a
    hosted C++ standard library and are silently excluded when `MP_UNITS_HOSTED` is
    not set. The remaining headers are safe to use in freestanding environments.


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

- [SI Systems Reference](../../reference/systems_reference/systems/si.md) - Complete
  list of units, prefixes, and point origins
- [Systems of Units](../framework_basics/systems_of_units.md) - Framework basics
- [SI Brochure (9th edition)](https://www.bipm.org/en/publications/si-brochure)
- [International System of Quantities (ISQ)](isq.md)
