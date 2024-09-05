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
constexpr struct pound final : named_unit<"lb", mag_ratio<45'359'237, 100'000'000> * si::kilogram> {} pound;
constexpr struct ounce final : named_unit<"oz", mag_ratio<1, 16> * pound> {} ounce;
constexpr struct dram final : named_unit<"dr", mag_ratio<1, 16> * ounce> {} dram;
constexpr struct grain final : named_unit<"gr", mag_ratio<1, 7'000> * pound> {} grain;

// length
// https://en.wikipedia.org/wiki/United_States_customary_units#Length
constexpr struct yard final : named_unit<"yd", mag_ratio<9'144, 10'000> * si::metre> {} yard;
constexpr struct foot final : named_unit<"ft", mag_ratio<1, 3> * yard> {} foot;
constexpr struct inch final : named_unit<"in", mag_ratio<1, 12> * foot> {} inch;
constexpr struct pica final : named_unit<"P", mag_ratio<1, 6> * inch> {} pica;
constexpr struct point final : named_unit<"p", mag_ratio<1, 12> * pica> {} point;
constexpr struct mil final : named_unit<"mil", mag_ratio<1, 1'000> * inch> {} mil;
constexpr struct twip final : named_unit<"twip", mag_ratio<1, 20> * point> {} twip;
constexpr struct mile final : named_unit<"mi", mag<1760> * yard> {} mile;
constexpr struct league final : named_unit<"le", mag<3> * mile> {} league;

constexpr struct nautical_mile final : named_unit<"nmi", mag<1852> * si::metre> {} nautical_mile;

// speed
constexpr struct knot final : named_unit<"kn", nautical_mile / si::hour> {} knot;

// force
// https://en.wikipedia.org/wiki/Poundal
constexpr struct poundal final : named_unit<"pdl", pound * foot / square(si::second)> {} poundal;

// https://en.wikipedia.org/wiki/Pound_(force)
constexpr struct pound_force final : named_unit<"lbf", pound * si::standard_gravity> {} pound_force;

// https://en.wikipedia.org/wiki/Kip_(unit),
constexpr auto kip = si::kilo<pound_force>;

// pressure
constexpr struct psi final : named_unit<"psi", pound_force / square(inch)> {} psi;

// power
// https://en.wikipedia.org/wiki/Horsepower#Definitions
constexpr struct mechanical_horsepower final : named_unit<"hp(I)", mag<33'000> * foot * pound_force / si::minute> {} mechanical_horsepower;
// clang-format on


namespace unit_symbols {

constexpr auto lb = pound;
constexpr auto oz = ounce;
constexpr auto dr = dram;
constexpr auto gr = grain;

constexpr auto yd = yard;
constexpr auto ft = foot;
constexpr auto in = inch;
constexpr auto P = pica;
constexpr auto p = point;
constexpr auto mi = mile;
constexpr auto le = league;

constexpr auto nmi = nautical_mile;

constexpr auto kn = knot;
constexpr auto kt = knot;
constexpr auto mph = mile / si::hour;

constexpr auto pdl = poundal;
constexpr auto lbf = pound_force;

constexpr auto hp = mechanical_horsepower;

}  // namespace unit_symbols

}  // namespace mp_units::international
