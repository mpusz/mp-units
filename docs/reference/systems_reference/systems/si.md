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
| <span id="ampere"></span><code>ampere</code> | A | `A` | <code>kind: [isq::electric_<wbr>current](isq.md#electric_current)</code> |
| <span id="avogadro_constant"></span><code>avogadro_<wbr>constant</code> | N_A | — | <code>mag_ratio<602'214'076, 100'000'000> * <wbr>mag_power<10, 23> / <wbr>[mole](#mole)</code> |
| <span id="becquerel"></span><code>becquerel</code> | Bq | `Bq` | <code>[one](core.md#one) / <wbr>[second](#second)<br>kind: [isq::activity](isq.md#activity)</code> |
| <span id="boltzmann_constant"></span><code>boltzmann_<wbr>constant</code> | k | — | <code>mag_ratio<1'380'649, 1'000'000> * <wbr>mag_power<10, -23> * <wbr>[joule](#joule) / <wbr>[kelvin](#kelvin)</code> |
| <span id="candela"></span><code>candela</code> | cd | `cd` | <code>kind: [isq::luminous_<wbr>intensity](isq.md#luminous_intensity)</code> |
| <span id="coulomb"></span><code>coulomb</code> | C | `C` | <code>[ampere](#ampere) * <wbr>[second](#second)</code> |
| <span id="degree_Celsius"></span><code>degree_<wbr>Celsius</code> | ℃ (\`C) | `deg_C` | <code>[kelvin](#kelvin)<br>origin: [zeroth_<wbr>degree_<wbr>Celsius](#zeroth_degree_Celsius)</code> |
| <span id="elementary_charge"></span><code>elementary_<wbr>charge</code> | e | — | <code>mag_ratio<1'602'176'634, 1'000'000'000> * <wbr>mag_power<10, -19> * <wbr>[coulomb](#coulomb)</code> |
| <span id="farad"></span><code>farad</code> | F | `F` | <code>[coulomb](#coulomb) / <wbr>[volt](#volt)</code> |
| <span id="gram"></span><code>gram</code> | g | `g` | <code>kind: [isq::mass](isq.md#mass)</code> |
| <span id="gray"></span><code>gray</code> | Gy | `Gy` | <code>[joule](#joule) / <wbr>[kilogram](#kilogram)<br>kind: [isq::absorbed_<wbr>dose](isq.md#absorbed_dose)</code> |
| <span id="henry"></span><code>henry</code> | H | `H` | <code>[weber](#weber) / <wbr>[ampere](#ampere)</code> |
| <span id="hertz"></span><code>hertz</code> | Hz | `Hz` | <code>[one](core.md#one) / <wbr>[second](#second)<br>kind: [isq::frequency](isq.md#frequency)</code> |
| <span id="hyperfine_structure_transition_frequency_of_cs"></span><code>hyperfine_<wbr>structure_<wbr>transition_<wbr>frequency_<wbr>of_<wbr>cs</code> | Δν_Cs (dv_Cs) | — | <code>mag<9'192'631'770> * <wbr>[hertz](#hertz)</code> |
| <span id="joule"></span><code>joule</code> | J | `J` | <code>[newton](#newton) * <wbr>[metre](#metre)</code> |
| <span id="katal"></span><code>katal</code> | kat | `kat` | <code>[mole](#mole) / <wbr>[second](#second)</code> |
| <span id="kelvin"></span><code>kelvin</code> | K | `K` | <code>kind: [isq::thermodynamic_<wbr>temperature](isq.md#thermodynamic_temperature)<br>origin: [zeroth_<wbr>kelvin](#zeroth_kelvin)</code> |
| <span id="kilogram"></span><code>kilogram</code> | — | `kg` | <code>[kilo](#kilo)<[gram](#gram)></code> |
| <span id="lumen"></span><code>lumen</code> | lm | `lm` | <code>[candela](#candela) * <wbr>[steradian](#steradian)</code> |
| <span id="luminous_efficacy"></span><code>luminous_<wbr>efficacy</code> | K_cd | — | <code>mag<683> * <wbr>[lumen](#lumen) / <wbr>[watt](#watt)</code> |
| <span id="lux"></span><code>lux</code> | lx | `lx` | <code>[lumen](#lumen) / <wbr>square([metre](#metre))</code> |
| <span id="magnetic_constant"></span><code>magnetic_<wbr>constant</code> | μ₀ (u_0) | — | <code>mag<4> * <wbr>mag_power<10, -7> * <wbr>π * <wbr>[henry](#henry) / <wbr>[metre](#metre)</code> |
| <span id="metre"></span><code>metre</code> | m | `m` | <code>kind: [isq::length](isq.md#length)</code> |
| <span id="mole"></span><code>mole</code> | mol | `mol` | <code>kind: [isq::amount_<wbr>of_<wbr>substance](isq.md#amount_of_substance)</code> |
| <span id="newton"></span><code>newton</code> | N | `N` | <code>[kilogram](#kilogram) * <wbr>[metre](#metre) / <wbr>square([second](#second))</code> |
| <span id="ohm"></span><code>ohm</code> | Ω (ohm) | `Ω`, `ohm` | <code>[volt](#volt) / <wbr>[ampere](#ampere)</code> |
| <span id="pascal"></span><code>pascal</code> | Pa | `Pa` | <code>[newton](#newton) / <wbr>square([metre](#metre))</code> |
| <span id="planck_constant"></span><code>planck_<wbr>constant</code> | h | — | <code>mag_ratio<662'607'015, 100'000'000> * <wbr>mag_power<10, -34> * <wbr>[joule](#joule) * <wbr>[second](#second)</code> |
| <span id="radian"></span><code>radian</code> | rad | `rad` | <code>[metre](#metre) / <wbr>[metre](#metre)<br>kind: [isq::angular_<wbr>measure](isq.md#angular_measure)</code> |
| <span id="second"></span><code>second</code> | s | `s` | <code>kind: [isq::duration](isq.md#duration)</code> |
| <span id="siemens"></span><code>siemens</code> | S | `S` | <code>[one](core.md#one) / <wbr>[ohm](#ohm)</code> |
| <span id="sievert"></span><code>sievert</code> | Sv | `Sv` | <code>[joule](#joule) / <wbr>[kilogram](#kilogram)<br>kind: [isq::dose_<wbr>equivalent](isq.md#dose_equivalent)</code> |
| <span id="speed_of_light_in_vacuum"></span><code>speed_<wbr>of_<wbr>light_<wbr>in_<wbr>vacuum</code> | c | — | <code>mag<299'792'458> * <wbr>[metre](#metre) / <wbr>[second](#second)</code> |
| <span id="standard_gravity"></span><code>standard_<wbr>gravity</code> | g₀ (g_0) | — | <code>mag_ratio<980'665, 100'000> * <wbr>[metre](#metre) / <wbr>square([second](#second))</code> |
| <span id="steradian"></span><code>steradian</code> | sr | `sr` | <code>square([metre](#metre)) / <wbr>square([metre](#metre))<br>kind: [isq::solid_<wbr>angular_<wbr>measure](isq.md#solid_angular_measure)</code> |
| <span id="tesla"></span><code>tesla</code> | T | `T` | <code>[weber](#weber) / <wbr>square([metre](#metre))</code> |
| <span id="volt"></span><code>volt</code> | V | `V` | <code>[watt](#watt) / <wbr>[ampere](#ampere)</code> |
| <span id="watt"></span><code>watt</code> | W | `W` | <code>[joule](#joule) / <wbr>[second](#second)</code> |
| <span id="weber"></span><code>weber</code> | Wb | `Wb` | <code>[volt](#volt) * <wbr>[second](#second)</code> |

## Non-SI units accepted for use with the SI

| Unit Name | Symbol | unit_symbol | Definition |
|-----------|:------:|:-----------:|------------|
| <span id="arcminute"></span><code>arcminute</code> | ′ (') | `arcmin` | <code>mag_ratio<1, 60> * <wbr>[degree](#degree)</code> |
| <span id="arcsecond"></span><code>arcsecond</code> | ″ ('') | `arcsec` | <code>mag_ratio<1, 60> * <wbr>[arcminute](#arcminute)</code> |
| <span id="are"></span><code>are</code> | a | `a` | <code>square([deca](#deca)<[metre](#metre)>)</code> |
| <span id="astronomical_unit"></span><code>astronomical_<wbr>unit</code> | au | `au` | <code>mag<149'597'870'700> * <wbr>[metre](#metre)</code> |
| <span id="dalton"></span><code>dalton</code> | Da | `Da` | <code>mag_ratio<16'605'390'666'050, 10'000'000'000'000> * <wbr>mag_power<10, -27> * <wbr>[kilogram](#kilogram)</code> |
| <span id="day"></span><code>day</code> | d | `d` | <code>mag<24> * <wbr>[hour](#hour)</code> |
| <span id="degree"></span><code>degree</code> | ° (deg) | `deg` | <code>mag_ratio<1, 180> * <wbr>π * <wbr>[radian](#radian)</code> |
| <span id="electronvolt"></span><code>electronvolt</code> | eV | `eV` | <code>mag_ratio<1'602'176'634, 1'000'000'000> * <wbr>mag_power<10, -19> * <wbr>[joule](#joule)</code> |
| <span id="hectare"></span><code>hectare</code> | — | `ha` | <code>[hecto](#hecto)<[are](#are)></code> |
| <span id="hour"></span><code>hour</code> | h | `h` | <code>mag<60> * <wbr>[minute](#minute)</code> |
| <span id="litre"></span><code>litre</code> | L | `l`, `L` | <code>cubic([deci](#deci)<[metre](#metre)>)</code> |
| <span id="minute"></span><code>minute</code> | min | `min` | <code>mag<60> * <wbr>[second](#second)</code> |
| <span id="tonne"></span><code>tonne</code> | t | `t` | <code>mag<1000> * <wbr>[kilogram](#kilogram)</code> |

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
| <span id="absolute_zero"></span>`absolute_zero` | absolute | <code>[isq::thermodynamic_<wbr>temperature](isq.md#thermodynamic_temperature)</code> |
| <span id="chrono_point_origin"></span>`chrono_point_origin` | absolute | <code>[isq::time](isq.md#time)</code> |
| <span id="ice_point"></span>`ice_point` | relative | <code>::mp_units::point<[milli](#milli)<[kelvin](#kelvin)>>(273'150)</code> |
| <span id="zeroth_degree_Celsius"></span>`zeroth_degree_Celsius` | — | alias to [ice_<wbr>point](#ice_point) |
| <span id="zeroth_kelvin"></span>`zeroth_kelvin` | — | alias to [absolute_<wbr>zero](#absolute_zero) |
