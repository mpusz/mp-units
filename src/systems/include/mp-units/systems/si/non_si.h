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


#pragma once

#include <mp-units/bits/module_macros.h>
#include <mp-units/systems/codata/codata2022_dalton.h>
#include <mp-units/systems/isq/si_quantities.h>
#include <mp-units/systems/si/prefixes.h>
#include <mp-units/systems/si/units.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework/unit.h>
#endif

MP_UNITS_EXPORT
namespace mp_units {

/**
 * The non-SI units of the SI Brochure
 *
 * It holds the units of Table 8 of the SI Brochure, such as the minute, the litre, the
 * nautical mile, and the electronvolt. Only the logarithmic ones are missing. The ones in
 * general use with the SI are also available in `si`.
 *
 * @code{.cpp}
 * quantity duration = 90 * min;
 * quantity in_hours = duration.in<double>(h);  // 1.5 h
 * @endcode
 */
namespace non_si {

// clang-format off
// long-standing units of time and angle
inline constexpr struct minute final : named_unit<"min", mag<60> * si::second> {} minute;
inline constexpr struct hour final : named_unit<"h", mag<60> * minute> {} hour;
inline constexpr struct day final : named_unit<"d", mag<24> * hour> {} day;
inline constexpr struct degree final : named_unit<symbol_text{u8"°", "deg"}, mag_ratio<1, 180> * π * si::radian> {} degree;
inline constexpr struct arcminute final : named_unit<symbol_text{u8"′", "'"}, mag_ratio<1, 60> * degree> {} arcminute;
inline constexpr struct arcsecond final : named_unit<symbol_text{u8"″", "''"}, mag_ratio<1, 60> * arcminute> {} arcsecond;

// historical names for decimal multiples and submultiples of SI units
inline constexpr struct are final : named_unit<"a", square(si::deca<si::metre>)> {} are;
inline constexpr auto hectare = si::hecto<are>;
inline constexpr struct barn final : named_unit<"b", mag_power<10, -28> * square(si::metre)> {} barn;
inline constexpr struct litre final : named_unit<"L", cubic(si::deci<si::metre>)> {} litre;
inline constexpr struct tonne final : named_unit<"t", mag<1000> * si::kilogram> {} tonne;
inline constexpr struct angstrom final : named_unit<symbol_text{u8"Å" /* U+00C5 */, "angstrom"}, mag_power<10, -10> * si::metre> {} angstrom;
inline constexpr struct gal final : named_unit<"Gal", si::centi<si::metre> / square(si::second)> {} gal;
#ifdef pascal
#pragma push_macro("pascal")
#undef pascal
#define MP_UNITS_REDEFINE_PASCAL
#endif
inline constexpr struct bar final : named_unit<"bar", mag_power<10, 5> * si::pascal> {} bar;
#ifdef MP_UNITS_REDEFINE_PASCAL
#pragma pop_macro("pascal")
#undef MP_UNITS_REDEFINE_PASCAL
#endif

// internationally recognised units that are not decimal multiples or submultiples of SI units
// The Brochure takes the value of the dalton from the current CODATA adjustment.
using codata::dalton;
inline constexpr struct astronomical_unit final : named_unit<"au", mag<149'597'870'700> * si::metre> {} astronomical_unit;
// The Brochure gives no symbol for the nautical mile or the knot (Table 8, note (k)).
inline constexpr struct nautical_mile final : named_unit<"nmi", mag<1852> * si::metre> {} nautical_mile;
inline constexpr struct knot final : named_unit<"kn", nautical_mile / hour> {} knot;
inline constexpr struct electronvolt final : named_unit<"eV", mag_ratio<1'602'176'634, 1'000'000'000> * mag_power<10, -19> * si::joule> {} electronvolt;

// units used in specialized technical disciplines
// TODO the logarithmic units (neper, bel, and decibel) - how to support those?
inline constexpr struct var final : named_unit<"var", si::volt * si::ampere, kind_of<isq::reactive_power>> {} var;
// clang-format on

}  // namespace non_si

/**
 * The degree of plane angle is written with no space, as in `90°`
 */
template<>
inline constexpr bool space_before_unit_symbol<non_si::degree> = false;
/**
 * The arcminute is written with no space, as in `30′`
 */
template<>
inline constexpr bool space_before_unit_symbol<non_si::arcminute> = false;
/**
 * The arcsecond is written with no space, as in `15″`
 */
template<>
inline constexpr bool space_before_unit_symbol<non_si::arcsecond> = false;

namespace non_si::unit_symbols {

// no prefixes should be provided for the below units
inline constexpr auto min = minute;
inline constexpr auto h = hour;
inline constexpr auto d = day;

// TODO Should the following non-SI units have prefixed symbols predefined as well?
inline constexpr auto deg = degree;
inline constexpr auto arcmin = arcminute;
inline constexpr auto arcsec = arcsecond;
inline constexpr auto a = are;
inline constexpr auto ha = hectare;
inline constexpr auto b = barn;
inline constexpr auto l = litre;
inline constexpr auto L = litre;  // both versions of litre are allowed
inline constexpr auto t = tonne;
using non_si::angstrom;
inline constexpr auto Å = angstrom;
inline constexpr auto Gal = gal;
using non_si::bar;
inline constexpr auto Da = dalton;
inline constexpr auto au = astronomical_unit;
inline constexpr auto nmi = nautical_mile;
inline constexpr auto NM = nautical_mile;
inline constexpr auto kn = knot;
inline constexpr auto kt = knot;
inline constexpr auto eV = electronvolt;
using non_si::var;

}  // namespace non_si::unit_symbols

// `si` provides the rows of Table 8 in general use with the SI. The others serve particular
// fields or, as the dalton does, have a measured value, so `si` doesn't provide them.
namespace si {

using non_si::arcminute;
using non_si::arcsecond;
using non_si::are;
using non_si::astronomical_unit;
using non_si::bar;
using non_si::day;
using non_si::degree;
using non_si::electronvolt;
using non_si::hectare;
using non_si::hour;
using non_si::litre;
using non_si::minute;
using non_si::tonne;
[[deprecated("2.6.0: Use `non_si::dalton` instead")]]
inline constexpr auto dalton = non_si::dalton;

}  // namespace si

namespace si::unit_symbols {

using non_si::unit_symbols::a;
using non_si::unit_symbols::arcmin;
using non_si::unit_symbols::arcsec;
using non_si::unit_symbols::au;
using non_si::unit_symbols::bar;
using non_si::unit_symbols::d;
using non_si::unit_symbols::deg;
using non_si::unit_symbols::eV;
using non_si::unit_symbols::h;
using non_si::unit_symbols::ha;
using non_si::unit_symbols::l;
using non_si::unit_symbols::L;
using non_si::unit_symbols::min;
using non_si::unit_symbols::t;
[[deprecated("2.6.0: Use `non_si::unit_symbols::Da` instead")]]
inline constexpr auto Da = non_si::dalton;

}  // namespace si::unit_symbols

}  // namespace mp_units
