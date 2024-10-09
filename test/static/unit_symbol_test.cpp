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

#include <mp-units/systems/angular.h>
#include <mp-units/systems/iau.h>
#include <mp-units/systems/iec.h>
#include <mp-units/systems/international.h>
#include <mp-units/systems/si.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <string_view>
#endif

namespace {

using namespace mp_units;
using namespace mp_units::si;
using namespace mp_units::iec;
using namespace mp_units::international;

using enum text_encoding;
using enum unit_symbol_solidus;
using enum unit_symbol_separator;
using usf = unit_symbol_formatting;

// named units
static_assert(unit_symbol(metre) == "m");
static_assert(unit_symbol(second) == "s");
static_assert(unit_symbol(joule) == "J");
static_assert(unit_symbol(degree_Celsius) == "\u2103");
static_assert(unit_symbol<usf{.encoding = portable}>(degree_Celsius) == "`C");
static_assert(unit_symbol(kilogram) == "kg");
static_assert(unit_symbol(hour) == "h");

// prefixed units
static_assert(unit_symbol(quecto<ohm>) == "qΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(quecto<ohm>) == "qohm");
static_assert(unit_symbol(ronto<ohm>) == "rΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(ronto<ohm>) == "rohm");
static_assert(unit_symbol(yocto<ohm>) == "yΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(yocto<ohm>) == "yohm");
static_assert(unit_symbol(zepto<ohm>) == "zΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(zepto<ohm>) == "zohm");
static_assert(unit_symbol(atto<ohm>) == "aΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(atto<ohm>) == "aohm");
static_assert(unit_symbol(femto<ohm>) == "fΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(femto<ohm>) == "fohm");
static_assert(unit_symbol(pico<ohm>) == "pΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(pico<ohm>) == "pohm");
static_assert(unit_symbol(nano<ohm>) == "nΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(nano<ohm>) == "nohm");
static_assert(unit_symbol(micro<ohm>) == "µΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(micro<ohm>) == "uohm");
static_assert(unit_symbol(milli<ohm>) == "mΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(milli<ohm>) == "mohm");
static_assert(unit_symbol(centi<ohm>) == "cΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(centi<ohm>) == "cohm");
static_assert(unit_symbol(deci<ohm>) == "dΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(deci<ohm>) == "dohm");
static_assert(unit_symbol(deca<ohm>) == "daΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(deca<ohm>) == "daohm");
static_assert(unit_symbol(hecto<ohm>) == "hΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(hecto<ohm>) == "hohm");
static_assert(unit_symbol(kilo<ohm>) == "kΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(kilo<ohm>) == "kohm");
static_assert(unit_symbol(mega<ohm>) == "MΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(mega<ohm>) == "Mohm");
static_assert(unit_symbol(giga<ohm>) == "GΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(giga<ohm>) == "Gohm");
static_assert(unit_symbol(tera<ohm>) == "TΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(tera<ohm>) == "Tohm");
static_assert(unit_symbol(peta<ohm>) == "PΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(peta<ohm>) == "Pohm");
static_assert(unit_symbol(exa<ohm>) == "EΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(exa<ohm>) == "Eohm");
static_assert(unit_symbol(zetta<ohm>) == "ZΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(zetta<ohm>) == "Zohm");
static_assert(unit_symbol(yotta<ohm>) == "YΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(yotta<ohm>) == "Yohm");
static_assert(unit_symbol(ronna<ohm>) == "RΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(ronna<ohm>) == "Rohm");
static_assert(unit_symbol(quetta<ohm>) == "QΩ");
static_assert(unit_symbol<usf{.encoding = portable}>(quetta<ohm>) == "Qohm");

static_assert(unit_symbol(kibi<bit>) == "Kibit");
static_assert(unit_symbol(mebi<bit>) == "Mibit");
static_assert(unit_symbol(gibi<bit>) == "Gibit");
static_assert(unit_symbol(tebi<bit>) == "Tibit");
static_assert(unit_symbol(pebi<bit>) == "Pibit");
static_assert(unit_symbol(exbi<bit>) == "Eibit");
static_assert(unit_symbol(zebi<bit>) == "Zibit");
static_assert(unit_symbol(yobi<bit>) == "Yibit");

// scaled units
static_assert(unit_symbol(mag<100> * metre) == "[100 m]");
static_assert(unit_symbol<usf{.encoding = portable}>(mag<100> * metre) == "[100 m]");
static_assert(unit_symbol(mag<1000> * metre) == "[10³ m]");
static_assert(unit_symbol(mag_power<10, 3> * metre) == "[10³ m]");
static_assert(unit_symbol<usf{.encoding = portable}>(mag<1000> * metre) == "[10^3 m]");
static_assert(unit_symbol(mag<6000> * metre) == "[6 × 10³ m]");
static_assert(unit_symbol(mag<6> * mag_power<10, 3> * metre) == "[6 × 10³ m]");
static_assert(unit_symbol<usf{.encoding = portable}>(mag<6000> * metre) == "[6 x 10^3 m]");
static_assert(unit_symbol(mag<10600> * metre) == "[10600 m]");
static_assert(unit_symbol(mag<60> * second) == "[60 s]");
static_assert(unit_symbol(mag_ratio<1, 18> * metre / second) == "[1/18 m]/s");
static_assert(unit_symbol(mag_ratio<1, 18> * (metre / second)) == "[1/18 m/s]");
static_assert(unit_symbol(mag_ratio<1, 1800> * metre / second) == "[1/1800 m]/s");
static_assert(unit_symbol(mag_ratio<1, 1800> * (metre / second)) == "[1/1800 m/s]");
static_assert(unit_symbol(mag_ratio<1, 18000> * metre / second) == "[1/18 × 10⁻³ m]/s");
static_assert(unit_symbol(mag_ratio<1, 18000> * (metre / second)) == "[1/18 × 10⁻³ m/s]");
static_assert(unit_symbol<usf{.encoding = portable}>(mag_ratio<1, 18000> * metre / second) == "[1/18 x 10^-3 m]/s");
static_assert(unit_symbol<usf{.encoding = portable}>(mag_ratio<1, 18000> * (metre / second)) == "[1/18 x 10^-3 m/s]");

// TODO implement all the below
// static_assert(unit_symbol(mag_power<2, 1, 2> * one) == "[2^(1/2)]");
// static_assert(unit_symbol<usf{.encoding = portable}>(mag_power<2, 1, 2> * one) == "[2^(1/2)]");
// static_assert(unit_symbol(mag_power<2, 1, 2> * m) == "[2^(1/2) m]");
// static_assert(unit_symbol<usf{.encoding = portable}>(mag_power<2, 1, 2> * m) == "[2^(1/2) m]");
// static_assert(unit_symbol(mag<1> / mag_power<2, 1, 2> * one) == "[1/2^(1/2)]");
// static_assert(unit_symbol<usf{.encoding = portable}>(mag<1> / mag_power<2, 1, 2> * one) == "[1/2^(1/2)]");
// static_assert(unit_symbol(mag<1> / mag_power<2, 1, 2> * m) == "[1/2^(1/2) m]");
// static_assert(unit_symbol<usf{.encoding = portable}>(mag<1> / mag_power<2, 1, 2> * m) == "[1/2^(1/2) m]");

// magnitude constants
#if defined MP_UNITS_COMP_CLANG || MP_UNITS_COMP_CLANG < 18
inline constexpr struct e final : mag_constant<"e"> {
  static constexpr long double _value_ = std::numbers::e_v<long double>;
#else
inline constexpr struct e final : mag_constant<"e", std::numbers::e_v<long double> > {
#endif
} e;

static_assert(unit_symbol(mag<pi> * one) == "[𝜋]");
static_assert(unit_symbol<usf{.encoding = portable}>(mag<pi> * one) == "[pi]");
static_assert(unit_symbol(mag<pi> * metre) == "[𝜋 m]");
static_assert(unit_symbol<usf{.encoding = portable}>(mag<pi> * metre) == "[pi m]");
static_assert(unit_symbol(mag<2> * mag<pi> * metre) == "[2 𝜋 m]");
static_assert(unit_symbol<usf{.encoding = portable}>(mag<2> * mag<pi> * metre) == "[2 pi m]");
static_assert(unit_symbol<usf{.separator = half_high_dot}>(mag<2> * mag<pi> * metre) == "[2⋅𝜋 m]");

static_assert(unit_symbol(mag<1> / mag<pi> * one) == "[1/𝜋]");
static_assert(unit_symbol<usf{.encoding = portable}>(mag<1> / mag<pi> * one) == "[1/pi]");
static_assert(unit_symbol<usf{.solidus = never}>(mag<1> / mag<pi> * one) == "[𝜋⁻¹]");
static_assert(unit_symbol<usf{.encoding = portable, .solidus = never}>(mag<1> / mag<pi> * one) == "[pi^-1]");

static_assert(unit_symbol(mag<1> / mag<pi> * metre) == "[1/𝜋 m]");
static_assert(unit_symbol<usf{.encoding = portable}>(mag<1> / mag<pi> * metre) == "[1/pi m]");
static_assert(unit_symbol<usf{.solidus = never}>(mag<1> / mag<pi> * metre) == "[𝜋⁻¹ m]");
static_assert(unit_symbol<usf{.encoding = portable, .solidus = never}>(mag<1> / mag<pi> * metre) == "[pi^-1 m]");

static_assert(unit_symbol(mag<2> / mag<pi> * metre) == "[2/𝜋 m]");
static_assert(unit_symbol<usf{.encoding = portable}>(mag<2> / mag<pi> * metre) == "[2/pi m]");
static_assert(unit_symbol<usf{.solidus = never}>(mag<2> / mag<pi> * metre) == "[2 𝜋⁻¹ m]");
static_assert(unit_symbol<usf{.encoding = portable, .solidus = never}>(mag<2> / mag<pi> * metre) == "[2 pi^-1 m]");
static_assert(unit_symbol<usf{.solidus = never, .separator = half_high_dot}>(mag<2> / mag<pi> * metre) == "[2⋅𝜋⁻¹ m]");

static_assert(unit_symbol(mag<1> / (mag<2> * mag<pi>)*metre) == "[2⁻¹ 𝜋⁻¹ m]");
static_assert(unit_symbol<usf{.solidus = always}>(mag<1> / (mag<2> * mag<pi>)*metre) == "[1/(2 𝜋) m]");
static_assert(unit_symbol<usf{.encoding = portable, .solidus = always}>(mag<1> / (mag<2> * mag<pi>)*metre) ==
              "[1/(2 pi) m]");
static_assert(unit_symbol(mag_ratio<1, 2> / mag<pi> * metre) == "[2⁻¹ 𝜋⁻¹ m]");
static_assert(unit_symbol<usf{.solidus = always}>(mag_ratio<1, 2> / mag<pi> * metre) == "[1/(2 𝜋) m]");
static_assert(unit_symbol<usf{.encoding = portable, .solidus = always}>(mag_ratio<1, 2> / mag<pi> * metre) ==
              "[1/(2 pi) m]");
static_assert(unit_symbol(mag_ratio<1, 2> * mag<pi> * metre) == "[𝜋/2 m]");

static_assert(unit_symbol(mag_power<pi, 2> * one) == "[𝜋²]");
static_assert(unit_symbol<usf{.encoding = portable}>(mag_power<pi, 2> * one) == "[pi^2]");
static_assert(unit_symbol(mag_power<pi, 1, 2> * metre) == "[𝜋^(1/2) m]");
static_assert(unit_symbol<usf{.encoding = portable}>(mag_power<pi, 1, 2> * metre) == "[pi^(1/2) m]");

static_assert(unit_symbol(mag<pi> * mag<e> * one) == "[e 𝜋]");
static_assert(unit_symbol(mag<e> * mag<pi> * one) == "[e 𝜋]");
static_assert(unit_symbol<usf{.encoding = portable}>(mag<pi> * mag<e> * one) == "[e pi]");
static_assert(unit_symbol(mag<pi> / mag<e> * one) == "[𝜋/e]");
static_assert(unit_symbol(mag<1> / mag<e> * mag<pi> * one) == "[𝜋/e]");
static_assert(unit_symbol<usf{.solidus = never}>(mag<pi> / mag<e> * one) == "[𝜋 e⁻¹]");
static_assert(unit_symbol(mag<e> / mag<pi> * one) == "[e/𝜋]");
static_assert(unit_symbol(mag<1> / mag<pi> * mag<e> * one) == "[e/𝜋]");
static_assert(unit_symbol<usf{.solidus = never}>(mag<e> / mag<pi> * one) == "[e 𝜋⁻¹]");
static_assert(unit_symbol(mag<1> / (mag<pi> * mag<e>)*one) == "[e⁻¹ 𝜋⁻¹]");
static_assert(unit_symbol<usf{.solidus = always}>(mag<1> / (mag<pi> * mag<e>)*one) == "[1/(e 𝜋)]");
static_assert(unit_symbol(mag<2> / (mag<pi> * mag<e>)*one) == "[2 e⁻¹ 𝜋⁻¹]");
static_assert(unit_symbol<usf{.solidus = always}>(mag<2> / (mag<pi> * mag<e>)*one) == "[2/(e 𝜋)]");

// common units
static_assert(unit_symbol(get_common_unit(kilo<metre>, mile)) == "EQUIV{[1/25146 mi], [1/15625 km]}");
static_assert(unit_symbol(get_common_unit(kilo<metre> / hour, metre / second)) == "EQUIV{[1/5 km/h], [1/18 m/s]}");
static_assert(unit_symbol(get_common_unit(kilo<metre> / hour, metre / second) / second) ==
              "EQUIV{[1/5 km/h], [1/18 m/s]}/s");
static_assert(unit_symbol(get_common_unit(kilo<metre> / hour, metre / second) * second) ==
              "EQUIV{[1/5 km/h], [1/18 m/s]} s");
static_assert(unit_symbol(get_common_unit(radian, degree)) == "EQUIV{[1/𝜋°], [1/180 rad]}");

// derived units
static_assert(unit_symbol(one) == "");  // NOLINT(readability-container-size-empty)
static_assert(unit_symbol(percent) == "%");
static_assert(unit_symbol(per_mille) == "‰");
static_assert(unit_symbol<usf{.encoding = portable}>(per_mille) == "%o");
static_assert(unit_symbol(parts_per_million) == "ppm");
static_assert(unit_symbol(square(metre)) == "m²");
static_assert(unit_symbol<usf{.encoding = portable}>(square(metre)) == "m^2");
static_assert(unit_symbol(cubic(metre)) == "m³");
static_assert(unit_symbol<usf{.encoding = portable}>(cubic(metre)) == "m^3");
static_assert(unit_symbol(kilo<metre> * metre) == "km m");
static_assert(unit_symbol<usf{.separator = half_high_dot}>(kilo<metre> * metre) == "km⋅m");
static_assert(unit_symbol(metre / metre) == "");  // NOLINT(readability-container-size-empty)
static_assert(unit_symbol(kilo<metre> / metre) == "km/m");
static_assert(unit_symbol<usf{.solidus = never}>(kilo<metre> / metre) == "km m⁻¹");
static_assert(unit_symbol<usf{.encoding = portable, .solidus = never}>(kilo<metre> / metre) == "km m^-1");
static_assert(unit_symbol(metre / second) == "m/s");
static_assert(unit_symbol<usf{.solidus = always}>(metre / second) == "m/s");
static_assert(unit_symbol<usf{.solidus = never}>(metre / second) == "m s⁻¹");
static_assert(unit_symbol<usf{.encoding = portable, .solidus = never}>(metre / second) == "m s^-1");
static_assert(unit_symbol<usf{.solidus = never, .separator = half_high_dot}>(metre / second) == "m⋅s⁻¹");
static_assert(unit_symbol(metre / square(second)) == "m/s²");
static_assert(unit_symbol<usf{.encoding = portable}>(metre / square(second)) == "m/s^2");
static_assert(unit_symbol<usf{.solidus = always}>(metre / square(second)) == "m/s²");
static_assert(unit_symbol<usf{.encoding = portable, .solidus = always}>(metre / square(second)) == "m/s^2");
static_assert(unit_symbol<usf{.solidus = never}>(metre / square(second)) == "m s⁻²");
static_assert(unit_symbol<usf{.encoding = portable, .solidus = never}>(metre / square(second)) == "m s^-2");
static_assert(unit_symbol<usf{.solidus = never, .separator = half_high_dot}>(metre / square(second)) == "m⋅s⁻²");
static_assert(unit_symbol(kilogram * metre / square(second)) == "kg m/s²");
static_assert(unit_symbol<usf{.separator = half_high_dot}>(kilogram * metre / square(second)) == "kg⋅m/s²");
static_assert(unit_symbol<usf{.encoding = portable}>(kilogram * metre / square(second)) == "kg m/s^2");
static_assert(unit_symbol<usf{.solidus = always}>(kilogram * metre / square(second)) == "kg m/s²");
static_assert(unit_symbol<usf{.encoding = portable, .solidus = always}>(kilogram * metre / square(second)) ==
              "kg m/s^2");
static_assert(unit_symbol<usf{.solidus = never}>(kilogram * metre / square(second)) == "kg m s⁻²");
static_assert(unit_symbol<usf{.encoding = portable, .solidus = never}>(kilogram * metre / square(second)) ==
              "kg m s^-2");
static_assert(unit_symbol<usf{.solidus = never, .separator = half_high_dot}>(kilogram * metre / square(second)) ==
              "kg⋅m⋅s⁻²");
static_assert(unit_symbol(one / metre / square(second)) == "m⁻¹ s⁻²");
static_assert(unit_symbol<usf{.solidus = always}>(one / metre / square(second)) == "1/(m s²)");
static_assert(unit_symbol(kilogram / metre / square(second)) == "kg m⁻¹ s⁻²");
static_assert(unit_symbol<usf{.separator = half_high_dot}>(kilogram / metre / square(second)) == "kg⋅m⁻¹⋅s⁻²");
static_assert(unit_symbol<usf{.encoding = portable}>(kilogram / metre / square(second)) == "kg m^-1 s^-2");
static_assert(unit_symbol<usf{.solidus = always}>(kilogram / metre / square(second)) == "kg/(m s²)");
static_assert(unit_symbol<usf{.encoding = portable, .solidus = always}>(kilogram / metre / square(second)) ==
              "kg/(m s^2)");
static_assert(unit_symbol<usf{.solidus = never}>(kilogram / metre / square(second)) == "kg m⁻¹ s⁻²");
static_assert(unit_symbol<usf{.encoding = portable, .solidus = never}>(kilogram / metre / square(second)) ==
              "kg m^-1 s^-2");
static_assert(unit_symbol<usf{.solidus = never, .separator = half_high_dot}>(kilogram / metre / square(second)) ==
              "kg⋅m⁻¹⋅s⁻²");
static_assert(unit_symbol(pow<123>(metre)) == "m¹²³");
static_assert(unit_symbol(pow<1, 2>(metre)) == "m^(1/2)");
static_assert(unit_symbol(pow<3, 5>(metre)) == "m^(3/5)");
static_assert(unit_symbol(pow<1, 2>(metre / second)) == "m^(1/2)/s^(1/2)");
static_assert(unit_symbol<usf{.solidus = never}>(pow<1, 2>(metre / second)) == "m^(1/2) s^-(1/2)");
static_assert(unit_symbol(litre / (mag<100> * kilo<metre>)) == "L/[100 km]");
static_assert(unit_symbol((mag<10> * metre) / (mag<20> * second)) == "[10 m]/[20 s]");
static_assert(unit_symbol(pow<2>(mag<3600> * second)) == "[3600 s]²");

// dimensionless unit
static_assert(unit_symbol(radian) == "rad");

// Physical constants
static_assert(unit_symbol(si2019::speed_of_light_in_vacuum) == "c");
static_assert(unit_symbol(gram * standard_gravity * si2019::speed_of_light_in_vacuum) == "g c g₀");
static_assert(unit_symbol(gram / standard_gravity) == "g/g₀");
static_assert(unit_symbol(kilo<metre> / second / mega<iau::parsec>) == "km Mpc⁻¹ s⁻¹");

}  // namespace
