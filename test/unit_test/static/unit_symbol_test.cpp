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
static_assert(unit_symbol(degree_Celsius, {.encoding = ascii}) == "`C");
static_assert(unit_symbol(kilogram) == "kg");
static_assert(unit_symbol(hour) == "h");

// prefixed units
static_assert(unit_symbol(quecto<ohm>) == "qΩ");
static_assert(unit_symbol(quecto<ohm>, {.encoding = ascii}) == "qohm");
static_assert(unit_symbol(ronto<ohm>) == "rΩ");
static_assert(unit_symbol(ronto<ohm>, {.encoding = ascii}) == "rohm");
static_assert(unit_symbol(yocto<ohm>) == "yΩ");
static_assert(unit_symbol(yocto<ohm>, {.encoding = ascii}) == "yohm");
static_assert(unit_symbol(zepto<ohm>) == "zΩ");
static_assert(unit_symbol(zepto<ohm>, {.encoding = ascii}) == "zohm");
static_assert(unit_symbol(atto<ohm>) == "aΩ");
static_assert(unit_symbol(atto<ohm>, {.encoding = ascii}) == "aohm");
static_assert(unit_symbol(femto<ohm>) == "fΩ");
static_assert(unit_symbol(femto<ohm>, {.encoding = ascii}) == "fohm");
static_assert(unit_symbol(pico<ohm>) == "pΩ");
static_assert(unit_symbol(pico<ohm>, {.encoding = ascii}) == "pohm");
static_assert(unit_symbol(nano<ohm>) == "nΩ");
static_assert(unit_symbol(nano<ohm>, {.encoding = ascii}) == "nohm");
static_assert(unit_symbol(micro<ohm>) == "µΩ");
static_assert(unit_symbol(micro<ohm>, {.encoding = ascii}) == "uohm");
static_assert(unit_symbol(milli<ohm>) == "mΩ");
static_assert(unit_symbol(milli<ohm>, {.encoding = ascii}) == "mohm");
static_assert(unit_symbol(centi<ohm>) == "cΩ");
static_assert(unit_symbol(centi<ohm>, {.encoding = ascii}) == "cohm");
static_assert(unit_symbol(deci<ohm>) == "dΩ");
static_assert(unit_symbol(deci<ohm>, {.encoding = ascii}) == "dohm");
static_assert(unit_symbol(deca<ohm>) == "daΩ");
static_assert(unit_symbol(deca<ohm>, {.encoding = ascii}) == "daohm");
static_assert(unit_symbol(hecto<ohm>) == "hΩ");
static_assert(unit_symbol(hecto<ohm>, {.encoding = ascii}) == "hohm");
static_assert(unit_symbol(kilo<ohm>) == "kΩ");
static_assert(unit_symbol(kilo<ohm>, {.encoding = ascii}) == "kohm");
static_assert(unit_symbol(mega<ohm>) == "MΩ");
static_assert(unit_symbol(mega<ohm>, {.encoding = ascii}) == "Mohm");
static_assert(unit_symbol(giga<ohm>) == "GΩ");
static_assert(unit_symbol(giga<ohm>, {.encoding = ascii}) == "Gohm");
static_assert(unit_symbol(tera<ohm>) == "TΩ");
static_assert(unit_symbol(tera<ohm>, {.encoding = ascii}) == "Tohm");
static_assert(unit_symbol(peta<ohm>) == "PΩ");
static_assert(unit_symbol(peta<ohm>, {.encoding = ascii}) == "Pohm");
static_assert(unit_symbol(exa<ohm>) == "EΩ");
static_assert(unit_symbol(exa<ohm>, {.encoding = ascii}) == "Eohm");
static_assert(unit_symbol(zetta<ohm>) == "ZΩ");
static_assert(unit_symbol(zetta<ohm>, {.encoding = ascii}) == "Zohm");
static_assert(unit_symbol(yotta<ohm>) == "YΩ");
static_assert(unit_symbol(yotta<ohm>, {.encoding = ascii}) == "Yohm");
static_assert(unit_symbol(ronna<ohm>) == "RΩ");
static_assert(unit_symbol(ronna<ohm>, {.encoding = ascii}) == "Rohm");
static_assert(unit_symbol(quetta<ohm>) == "QΩ");
static_assert(unit_symbol(quetta<ohm>, {.encoding = ascii}) == "Qohm");

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
static_assert(unit_symbol(mag<100> * metre, {.encoding = ascii}) == "x 10^2 m");
static_assert(unit_symbol(mag<60> * second) == "[6 × 10¹] s");
static_assert(unit_symbol(mag<60> * second, {.encoding = ascii}) == "[6 x 10^1] s");
static_assert(unit_symbol(mag<ratio{1, 18}> * metre / second) == "[1/18] m/s");

// derived units
static_assert(unit_symbol(one) == "");
static_assert(unit_symbol(percent) == "%");
static_assert(unit_symbol(per_mille) == "‰");
static_assert(unit_symbol(per_mille, {.encoding = ascii}) == "%o");
static_assert(unit_symbol(square(metre)) == "m²");
static_assert(unit_symbol(square(metre), {.encoding = ascii}) == "m^2");
static_assert(unit_symbol(cubic(metre)) == "m³");
static_assert(unit_symbol(cubic(metre), {.encoding = ascii}) == "m^3");
static_assert(unit_symbol(kilo<metre> * metre) == "km m");
static_assert(unit_symbol(kilo<metre> * metre, {.separator = half_high_dot}) == "km⋅m");
static_assert(unit_symbol(metre / metre) == "");
static_assert(unit_symbol(kilo<metre> / metre) == "km/m");
static_assert(unit_symbol(kilo<metre> / metre, {.solidus = never}) == "km m⁻¹");
static_assert(unit_symbol(kilo<metre> / metre, {.encoding = ascii, .solidus = never}) == "km m^-1");
static_assert(unit_symbol(metre / second) == "m/s");
static_assert(unit_symbol(metre / second, {.solidus = always}) == "m/s");
static_assert(unit_symbol(metre / second, {.solidus = never}) == "m s⁻¹");
static_assert(unit_symbol(metre / second, {.encoding = ascii, .solidus = never}) == "m s^-1");
static_assert(unit_symbol(metre / second, {.solidus = never, .separator = half_high_dot}) == "m⋅s⁻¹");
static_assert(unit_symbol(metre / square(second)) == "m/s²");
static_assert(unit_symbol(metre / square(second), {.encoding = ascii}) == "m/s^2");
static_assert(unit_symbol(metre / square(second), {.solidus = always}) == "m/s²");
static_assert(unit_symbol(metre / square(second), {.encoding = ascii, .solidus = always}) == "m/s^2");
static_assert(unit_symbol(metre / square(second), {.solidus = never}) == "m s⁻²");
static_assert(unit_symbol(metre / square(second), {.encoding = ascii, .solidus = never}) == "m s^-2");
static_assert(unit_symbol(metre / square(second), {.solidus = never, .separator = half_high_dot}) == "m⋅s⁻²");
static_assert(unit_symbol(kilogram * metre / square(second)) == "kg m/s²");
static_assert(unit_symbol(kilogram * metre / square(second), {.separator = half_high_dot}) == "kg⋅m/s²");
static_assert(unit_symbol(kilogram * metre / square(second), {.encoding = ascii}) == "kg m/s^2");
static_assert(unit_symbol(kilogram * metre / square(second), {.solidus = always}) == "kg m/s²");
static_assert(unit_symbol(kilogram * metre / square(second), {.encoding = ascii, .solidus = always}) == "kg m/s^2");
static_assert(unit_symbol(kilogram * metre / square(second), {.solidus = never}) == "kg m s⁻²");
static_assert(unit_symbol(kilogram * metre / square(second), {.encoding = ascii, .solidus = never}) == "kg m s^-2");
static_assert(unit_symbol(kilogram * metre / square(second), {.solidus = never, .separator = half_high_dot}) ==
              "kg⋅m⋅s⁻²");
static_assert(unit_symbol(kilogram / metre / square(second)) == "kg m⁻¹ s⁻²");
static_assert(unit_symbol(kilogram / metre / square(second), {.separator = half_high_dot}) == "kg⋅m⁻¹⋅s⁻²");
static_assert(unit_symbol(kilogram / metre / square(second), {.encoding = ascii}) == "kg m^-1 s^-2");
static_assert(unit_symbol(kilogram / metre / square(second), {.solidus = always}) == "kg/(m s²)");
static_assert(unit_symbol(kilogram / metre / square(second), {.encoding = ascii, .solidus = always}) == "kg/(m s^2)");
static_assert(unit_symbol(kilogram / metre / square(second), {.solidus = never}) == "kg m⁻¹ s⁻²");
static_assert(unit_symbol(kilogram / metre / square(second), {.encoding = ascii, .solidus = never}) == "kg m^-1 s^-2");
static_assert(unit_symbol(kilogram / metre / square(second), {.solidus = never, .separator = half_high_dot}) ==
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
