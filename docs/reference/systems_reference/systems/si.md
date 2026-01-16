<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# SI System

**Namespace:** `mp_units::si`

**Module:** `mp_units.systems`

**Header:** `<mp-units/systems/si.h>`

**Secondary Headers:**

- `<mp-units/systems/si/chrono.h>`
- `<mp-units/systems/si/constants.h>`
- `<mp-units/systems/si/math.h>`
- `<mp-units/systems/si/prefixes.h>`
- `<mp-units/systems/si/unit_symbols.h>`
- `<mp-units/systems/si/units.h>`

## Units

| Unit Name | Symbol | unit_symbol | Definition |
|-----------|:------:|:-----------:|------------|
| <span id="ampere"></span>`ampere` | A | `A` | kind: `isq::electric_current` |
| <span id="avogadro_constant"></span>`avogadro_constant` | N_A | — | `mag_ratio<602'214'076, 100'000'000> * mag_power<10, 23> / mole` |
| <span id="becquerel"></span>`becquerel` | Bq | `Bq` | `one / second`<br>kind: `isq::activity` |
| <span id="boltzmann_constant"></span>`boltzmann_constant` | k | — | `mag_ratio<1'380'649, 1'000'000> * mag_power<10, -23> * joule / kelvin` |
| <span id="candela"></span>`candela` | cd | `cd` | kind: `isq::luminous_intensity` |
| <span id="coulomb"></span>`coulomb` | C | `C` | `ampere * second` |
| <span id="degree_Celsius"></span>`degree_Celsius` | ℃ (\`C) | `deg_C` | `kelvin`<br>origin: `zeroth_degree_Celsius` |
| <span id="elementary_charge"></span>`elementary_charge` | e | — | `mag_ratio<1'602'176'634, 1'000'000'000> * mag_power<10, -19> * coulomb` |
| <span id="farad"></span>`farad` | F | `F` | `coulomb / volt` |
| <span id="gram"></span>`gram` | g | `g` | kind: `isq::mass` |
| <span id="gray"></span>`gray` | Gy | `Gy` | `joule / kilogram`<br>kind: `isq::absorbed_dose` |
| <span id="henry"></span>`henry` | H | `H` | `weber / ampere` |
| <span id="hertz"></span>`hertz` | Hz | `Hz` | `one / second`<br>kind: `isq::frequency` |
| <span id="hyperfine_structure_transition_frequency_of_cs"></span>`hyperfine_structure_transition_frequency_of_cs` | Δν_Cs (dv_Cs) | — | `mag<9'192'631'770> * hertz` |
| <span id="joule"></span>`joule` | J | `J` | `newton * metre` |
| <span id="katal"></span>`katal` | kat | `kat` | `mole / second` |
| <span id="kelvin"></span>`kelvin` | K | `K` | kind: `isq::thermodynamic_temperature`<br>origin: `zeroth_kelvin` |
| <span id="kilogram"></span>`kilogram` | — | `kg` | `kilo<gram>` |
| <span id="lumen"></span>`lumen` | lm | `lm` | `candela * steradian` |
| <span id="luminous_efficacy"></span>`luminous_efficacy` | K_cd | — | `mag<683> * lumen / watt` |
| <span id="lux"></span>`lux` | lx | `lx` | `lumen / square(metre)` |
| <span id="magnetic_constant"></span>`magnetic_constant` | μ₀ (u_0) | — | `mag<4> * mag_power<10, -7> * π * henry / metre` |
| <span id="metre"></span>`metre` | m | `m` | kind: `isq::length` |
| <span id="mole"></span>`mole` | mol | `mol` | kind: `isq::amount_of_substance` |
| <span id="newton"></span>`newton` | N | `N` | `kilogram * metre / square(second)` |
| <span id="ohm"></span>`ohm` | Ω (ohm) | `Ω`, `ohm` | `volt / ampere` |
| <span id="pascal"></span>`pascal` | Pa | `Pa` | `newton / square(metre)` |
| <span id="planck_constant"></span>`planck_constant` | h | — | `mag_ratio<662'607'015, 100'000'000> * mag_power<10, -34> * joule * second` |
| <span id="radian"></span>`radian` | rad | `rad` | `metre / metre`<br>kind: `isq::angular_measure` |
| <span id="second"></span>`second` | s | `s` | kind: `isq::duration` |
| <span id="siemens"></span>`siemens` | S | `S` | `one / ohm` |
| <span id="sievert"></span>`sievert` | Sv | `Sv` | `joule / kilogram`<br>kind: `isq::dose_equivalent` |
| <span id="speed_of_light_in_vacuum"></span>`speed_of_light_in_vacuum` | c | — | `mag<299'792'458> * metre / second` |
| <span id="standard_gravity"></span>`standard_gravity` | g₀ (g_0) | — | `mag_ratio<980'665, 100'000> * metre / square(second)` |
| <span id="steradian"></span>`steradian` | sr | `sr` | `square(metre) / square(metre)`<br>kind: `isq::solid_angular_measure` |
| <span id="tesla"></span>`tesla` | T | `T` | `weber / square(metre)` |
| <span id="volt"></span>`volt` | V | `V` | `watt / ampere` |
| <span id="watt"></span>`watt` | W | `W` | `joule / second` |
| <span id="weber"></span>`weber` | Wb | `Wb` | `volt * second` |

## Non-SI units accepted for use with the SI

| Unit Name | Symbol | unit_symbol | Definition |
|-----------|:------:|:-----------:|------------|
| <span id="arcminute"></span>`arcminute` | ′ (') | `arcmin` | `mag_ratio<1, 60> * degree` |
| <span id="arcsecond"></span>`arcsecond` | ″ ('') | `arcsec` | `mag_ratio<1, 60> * arcminute` |
| <span id="are"></span>`are` | a | `a` | `square(si::deca<si::metre>)` |
| <span id="astronomical_unit"></span>`astronomical_unit` | au | `au` | `mag<149'597'870'700> * si::metre` |
| <span id="dalton"></span>`dalton` | Da | `Da` | `mag_ratio<16'605'390'666'050, 10'000'000'000'000> * mag_power<10, -27> * si::kilogram` |
| <span id="day"></span>`day` | d | `d` | `mag<24> * hour` |
| <span id="degree"></span>`degree` | ° (deg) | `deg` | `mag_ratio<1, 180> * π * si::radian` |
| <span id="electronvolt"></span>`electronvolt` | eV | `eV` | `mag_ratio<1'602'176'634, 1'000'000'000> * mag_power<10, -19> * si::joule` |
| <span id="hectare"></span>`hectare` | — | `ha` | `si::hecto<are>` |
| <span id="hour"></span>`hour` | h | `h` | `mag<60> * minute` |
| <span id="litre"></span>`litre` | L | `l`, `L` | `cubic(si::deci<si::metre>)` |
| <span id="minute"></span>`minute` | min | `min` | `mag<60> * si::second` |
| <span id="tonne"></span>`tonne` | t | `t` | `mag<1000> * si::kilogram` |

## Prefixes

| Name | Symbol | Definition |
|------|:------:|------------|
| <span id="quecto"></span>`quecto` | q | `mag_power<10, -30>` |
| <span id="ronto"></span>`ronto` | r | `mag_power<10, -27>` |
| <span id="yocto"></span>`yocto` | y | `mag_power<10, -24>` |
| <span id="zepto"></span>`zepto` | z | `mag_power<10, -21>` |
| <span id="atto"></span>`atto` | a | `mag_power<10, -18>` |
| <span id="femto"></span>`femto` | f | `mag_power<10, -15>` |
| <span id="pico"></span>`pico` | p | `mag_power<10, -12>` |
| <span id="nano"></span>`nano` | n | `mag_power<10, -9>` |
| <span id="micro"></span>`micro` | µ (u) | `mag_power<10, -6>` |
| <span id="milli"></span>`milli` | m | `mag_power<10, -3>` |
| <span id="centi"></span>`centi` | c | `mag_power<10, -2>` |
| <span id="deci"></span>`deci` | d | `mag_power<10, -1>` |
| <span id="deca"></span>`deca` | da | `mag_power<10, 1>` |
| <span id="hecto"></span>`hecto` | h | `mag_power<10, 2>` |
| <span id="kilo"></span>`kilo` | k | `mag_power<10, 3>` |
| <span id="mega"></span>`mega` | M | `mag_power<10, 6>` |
| <span id="giga"></span>`giga` | G | `mag_power<10, 9>` |
| <span id="tera"></span>`tera` | T | `mag_power<10, 12>` |
| <span id="peta"></span>`peta` | P | `mag_power<10, 15>` |
| <span id="exa"></span>`exa` | E | `mag_power<10, 18>` |
| <span id="zetta"></span>`zetta` | Z | `mag_power<10, 21>` |
| <span id="yotta"></span>`yotta` | Y | `mag_power<10, 24>` |
| <span id="ronna"></span>`ronna` | R | `mag_power<10, 27>` |
| <span id="quetta"></span>`quetta` | Q | `mag_power<10, 30>` |

## Point Origins

| Name | Type | Definition |
|------|:----:|------------|
| <span id="absolute_zero"></span>`absolute_zero` | absolute | `isq::thermodynamic_temperature` |
| <span id="chrono_point_origin"></span>`chrono_point_origin` | absolute | `isq::time` |
| <span id="ice_point"></span>`ice_point` | relative | `::mp_units::point<milli<kelvin>>(273'150)` |
| <span id="zeroth_degree_Celsius"></span>`zeroth_degree_Celsius` | — | alias to `ice_point` |
| <span id="zeroth_kelvin"></span>`zeroth_kelvin` | — | alias to `absolute_zero` |
