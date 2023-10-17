// The MIT License (MIT)
//
// Copyright (c) 2018 Mateusz Pusz
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <mp-units/systems/iau/iau.h>
#include <mp-units/systems/iec80000/iec80000.h>
#include <mp-units/systems/si/si.h>

namespace {

using namespace mp_units;
using namespace mp_units::si;
using namespace mp_units::iec80000;

#if __cpp_lib_constexpr_string && (!defined MP_UNITS_COMP_GCC || MP_UNITS_COMP_GCC > 11)

using enum text_encoding;
using enum unit_symbol_solidus;
using enum unit_symbol_separator;

// named units
static_assert(unit_symbol(metre) == "m");
static_assert(unit_symbol(second) == "s");
static_assert(unit_symbol(joule) == "J");
static_assert(unit_symbol(degree_Celsius) == "\u00B0C");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(degree_Celsius) == "`C");
static_assert(unit_symbol(kilogram) == "kg");
static_assert(unit_symbol(hour) == "h");

// prefixed units
static_assert(unit_symbol(quecto<ohm>) == "qΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(quecto<ohm>) == "qohm");
static_assert(unit_symbol(ronto<ohm>) == "rΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(ronto<ohm>) == "rohm");
static_assert(unit_symbol(yocto<ohm>) == "yΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(yocto<ohm>) == "yohm");
static_assert(unit_symbol(zepto<ohm>) == "zΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(zepto<ohm>) == "zohm");
static_assert(unit_symbol(atto<ohm>) == "aΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(atto<ohm>) == "aohm");
static_assert(unit_symbol(femto<ohm>) == "fΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(femto<ohm>) == "fohm");
static_assert(unit_symbol(pico<ohm>) == "pΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(pico<ohm>) == "pohm");
static_assert(unit_symbol(nano<ohm>) == "nΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(nano<ohm>) == "nohm");
static_assert(unit_symbol(micro<ohm>) == "µΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(micro<ohm>) == "uohm");
static_assert(unit_symbol(milli<ohm>) == "mΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(milli<ohm>) == "mohm");
static_assert(unit_symbol(centi<ohm>) == "cΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(centi<ohm>) == "cohm");
static_assert(unit_symbol(deci<ohm>) == "dΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(deci<ohm>) == "dohm");
static_assert(unit_symbol(deca<ohm>) == "daΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(deca<ohm>) == "daohm");
static_assert(unit_symbol(hecto<ohm>) == "hΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(hecto<ohm>) == "hohm");
static_assert(unit_symbol(kilo<ohm>) == "kΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(kilo<ohm>) == "kohm");
static_assert(unit_symbol(mega<ohm>) == "MΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(mega<ohm>) == "Mohm");
static_assert(unit_symbol(giga<ohm>) == "GΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(giga<ohm>) == "Gohm");
static_assert(unit_symbol(tera<ohm>) == "TΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(tera<ohm>) == "Tohm");
static_assert(unit_symbol(peta<ohm>) == "PΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(peta<ohm>) == "Pohm");
static_assert(unit_symbol(exa<ohm>) == "EΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(exa<ohm>) == "Eohm");
static_assert(unit_symbol(zetta<ohm>) == "ZΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(zetta<ohm>) == "Zohm");
static_assert(unit_symbol(yotta<ohm>) == "YΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(yotta<ohm>) == "Yohm");
static_assert(unit_symbol(ronna<ohm>) == "RΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(ronna<ohm>) == "Rohm");
static_assert(unit_symbol(quetta<ohm>) == "QΩ");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(quetta<ohm>) == "Qohm");

static_assert(unit_symbol(kibi<bit>) == "Kibit");
static_assert(unit_symbol(mebi<bit>) == "Mibit");
static_assert(unit_symbol(gibi<bit>) == "Gibit");
static_assert(unit_symbol(tebi<bit>) == "Tibit");
static_assert(unit_symbol(pebi<bit>) == "Pibit");
static_assert(unit_symbol(exbi<bit>) == "Eibit");
static_assert(unit_symbol(zebi<bit>) == "Zibit");
static_assert(unit_symbol(yobi<bit>) == "Yibit");

// scaled units
static_assert(unit_symbol(mag<100> * metre) == "× 10² m");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(mag<100> * metre) == "x 10^2 m");
static_assert(unit_symbol(mag<60> * second) == "[6 × 10¹] s");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(mag<60> * second) == "[6 x 10^1] s");
static_assert(unit_symbol(mag<ratio{1, 18}> * metre / second) == "[1/18] m/s");

// derived units
static_assert(unit_symbol(one) == "");
static_assert(unit_symbol(percent) == "%");
static_assert(unit_symbol(per_mille) == "‰");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(per_mille) == "%o");
static_assert(unit_symbol(square(metre)) == "m²");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(square(metre)) == "m^2");
static_assert(unit_symbol(cubic(metre)) == "m³");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(cubic(metre)) == "m^3");
static_assert(unit_symbol(kilo<metre> * metre) == "km m");
static_assert(unit_symbol<unit_symbol_formatting{.separator = half_high_dot}>(kilo<metre> * metre) == "km⋅m");
static_assert(unit_symbol(metre / metre) == "");
static_assert(unit_symbol(kilo<metre> / metre) == "km/m");
static_assert(unit_symbol<unit_symbol_formatting{.solidus = never}>(kilo<metre> / metre) == "km m⁻¹");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii, .solidus = never}>(kilo<metre> / metre) ==
              "km m^-1");
static_assert(unit_symbol(metre / second) == "m/s");
static_assert(unit_symbol<unit_symbol_formatting{.solidus = always}>(metre / second) == "m/s");
static_assert(unit_symbol<unit_symbol_formatting{.solidus = never}>(metre / second) == "m s⁻¹");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii, .solidus = never}>(metre / second) == "m s^-1");
static_assert(unit_symbol<unit_symbol_formatting{.solidus = never, .separator = half_high_dot}>(metre / second) ==
              "m⋅s⁻¹");
static_assert(unit_symbol(metre / square(second)) == "m/s²");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(metre / square(second)) == "m/s^2");
static_assert(unit_symbol<unit_symbol_formatting{.solidus = always}>(metre / square(second)) == "m/s²");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii, .solidus = always}>(metre / square(second)) ==
              "m/s^2");
static_assert(unit_symbol<unit_symbol_formatting{.solidus = never}>(metre / square(second)) == "m s⁻²");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii, .solidus = never}>(metre / square(second)) ==
              "m s^-2");
static_assert(unit_symbol<unit_symbol_formatting{.solidus = never, .separator = half_high_dot}>(metre /
                                                                                                square(second)) ==
              "m⋅s⁻²");
static_assert(unit_symbol(kilogram * metre / square(second)) == "kg m/s²");
static_assert(unit_symbol<unit_symbol_formatting{.separator = half_high_dot}>(kilogram * metre / square(second)) ==
              "kg⋅m/s²");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(kilogram * metre / square(second)) == "kg m/s^2");
static_assert(unit_symbol<unit_symbol_formatting{.solidus = always}>(kilogram * metre / square(second)) == "kg m/s²");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii, .solidus = always}>(kilogram * metre /
                                                                                        square(second)) == "kg m/s^2");
static_assert(unit_symbol<unit_symbol_formatting{.solidus = never}>(kilogram * metre / square(second)) == "kg m s⁻²");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii, .solidus = never}>(kilogram * metre /
                                                                                       square(second)) == "kg m s^-2");
static_assert(unit_symbol<unit_symbol_formatting{.solidus = never, .separator = half_high_dot}>(kilogram * metre /
                                                                                                square(second)) ==
              "kg⋅m⋅s⁻²");
static_assert(unit_symbol(kilogram / metre / square(second)) == "kg m⁻¹ s⁻²");
static_assert(unit_symbol<unit_symbol_formatting{.separator = half_high_dot}>(kilogram / metre / square(second)) ==
              "kg⋅m⁻¹⋅s⁻²");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii}>(kilogram / metre / square(second)) ==
              "kg m^-1 s^-2");
static_assert(unit_symbol<unit_symbol_formatting{.solidus = always}>(kilogram / metre / square(second)) == "kg/(m s²)");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii, .solidus = always}>(kilogram / metre /
                                                                                        square(second)) ==
              "kg/(m s^2)");
static_assert(unit_symbol<unit_symbol_formatting{.solidus = never}>(kilogram / metre / square(second)) == "kg m⁻¹ s⁻²");
static_assert(unit_symbol<unit_symbol_formatting{.encoding = ascii, .solidus = never}>(kilogram / metre /
                                                                                       square(second)) ==
              "kg m^-1 s^-2");
static_assert(unit_symbol<unit_symbol_formatting{.solidus = never, .separator = half_high_dot}>(kilogram / metre /
                                                                                                square(second)) ==
              "kg⋅m⁻¹⋅s⁻²");
static_assert(unit_symbol(pow<123>(metre)) == "m¹²³");
static_assert(unit_symbol(pow<1, 2>(metre)) == "m^(1/2)");
static_assert(unit_symbol(pow<3, 5>(metre)) == "m^(3/5)");
static_assert(unit_symbol(pow<1, 2>(metre / second)) == "m^(1/2)/s^(1/2)");

// dimensionless unit
static_assert(unit_symbol(radian) == "rad");

// Physical constants
static_assert(unit_symbol(si2019::speed_of_light_in_vacuum) == "c");
static_assert(unit_symbol(gram * standard_gravity * si2019::speed_of_light_in_vacuum) == "c g g₀");
static_assert(unit_symbol(gram / standard_gravity) == "g/g₀");
static_assert(unit_symbol(kilo<metre> / second / mega<iau::parsec>) == "km Mpc⁻¹ s⁻¹");

#endif  // __cpp_lib_constexpr_string

}  // namespace
