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
#include <mp-units/systems/si/constants.h>
#include <mp-units/systems/si/units.h>

// IWYU pragma: begin_exports
#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework.h>
#endif
// IWYU pragma: end_exports

MP_UNITS_EXPORT
namespace mp_units::international {

// clang-format off
// mass
inline constexpr struct pound final : named_unit<"lb", mag_ratio<45'359'237, 100'000'000> * si::kilogram> {} pound;
inline constexpr struct ounce final : named_unit<"oz", mag_ratio<1, 16> * pound> {} ounce;
inline constexpr struct dram final : named_unit<"dr", mag_ratio<1, 16> * ounce> {} dram;
inline constexpr struct grain final : named_unit<"gr", mag_ratio<1, 7'000> * pound> {} grain;

// length
// https://en.wikipedia.org/wiki/United_States_customary_units#Length
inline constexpr struct yard final : named_unit<"yd", mag_ratio<9'144, 10'000> * si::metre> {} yard;
inline constexpr struct foot final : named_unit<"ft", mag_ratio<1, 3> * yard> {} foot;
inline constexpr struct inch final : named_unit<"in", mag_ratio<1, 12> * foot> {} inch;
inline constexpr struct pica final : named_unit<"P", mag_ratio<1, 6> * inch> {} pica;
inline constexpr struct point final : named_unit<"p", mag_ratio<1, 12> * pica> {} point;
inline constexpr struct mil final : named_unit<"mil", mag_ratio<1, 1'000> * inch> {} mil;
inline constexpr struct twip final : named_unit<"twip", mag_ratio<1, 20> * point> {} twip;
inline constexpr struct mile final : named_unit<"mi", mag<1760> * yard> {} mile;
inline constexpr struct league final : named_unit<"le", mag<3> * mile> {} league;

inline constexpr struct nautical_mile final : named_unit<"nmi", mag<1852> * si::metre> {} nautical_mile;

// speed
inline constexpr struct knot final : named_unit<"kn", nautical_mile / si::hour> {} knot;

// force
// https://en.wikipedia.org/wiki/Poundal
inline constexpr struct poundal final : named_unit<"pdl", pound * foot / square(si::second)> {} poundal;

// https://en.wikipedia.org/wiki/Pound_(force)
inline constexpr struct pound_force final : named_unit<"lbf", pound * si::standard_gravity> {} pound_force;

// https://en.wikipedia.org/wiki/Kip_(unit),
inline constexpr auto kip = si::kilo<pound_force>;

// pressure
inline constexpr struct psi final : named_unit<"psi", pound_force / square(inch)> {} psi;

// power
// https://en.wikipedia.org/wiki/Horsepower#Definitions
inline constexpr struct mechanical_horsepower final : named_unit<"hp(I)", mag<33'000> * foot * pound_force / si::minute> {} mechanical_horsepower;
// clang-format on


namespace unit_symbols {

inline constexpr auto lb = pound;
inline constexpr auto oz = ounce;
inline constexpr auto dr = dram;
inline constexpr auto gr = grain;

inline constexpr auto yd = yard;
inline constexpr auto ft = foot;
inline constexpr auto in = inch;
inline constexpr auto P = pica;
inline constexpr auto p = point;
inline constexpr auto mi = mile;
inline constexpr auto le = league;

inline constexpr auto nmi = nautical_mile;

inline constexpr auto kn = knot;
inline constexpr auto kt = knot;
inline constexpr auto mph = mile / si::hour;

inline constexpr auto pdl = poundal;
inline constexpr auto lbf = pound_force;

inline constexpr auto hp = mechanical_horsepower;

}  // namespace unit_symbols

}  // namespace mp_units::international
