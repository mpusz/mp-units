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

// IWYU pragma: begin_exports
#include <mp-units/systems/international.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework.h>
#endif
// IWYU pragma: end_exports

MP_UNITS_EXPORT
namespace mp_units::usc {

using namespace international;

// clang-format off

// https://en.wikipedia.org/wiki/United_States_customary_units#Length
// nautical
constexpr struct fathom final : named_unit<"ftm(us)", mag<2> * yard> {} fathom;
constexpr struct cable final : named_unit<"cb(us)", mag<120> * fathom> {} cable;

// survey
struct us_survey_foot final : named_unit<"ft(us)", mag_ratio<1'200, 3'937> * si::metre> {};
struct us_survey_mile final : named_unit<"mi(us)", mag<5280> * us_survey_foot{}> {};

[[deprecated("In accordance with NIST SP 811, as of January 1, 2023, the use of the U.S. survey foot and U.S. survey mile is deprecated.")]]
constexpr us_survey_foot us_survey_foot;

[[deprecated("In accordance with NIST SP 811, as of January 1, 2023, the use of the U.S. survey foot and U.S. survey mile is deprecated.")]]
constexpr us_survey_mile us_survey_mile;

constexpr struct link final : named_unit<"li", mag_ratio<33, 50> * foot> {} link;
constexpr struct rod final : named_unit<"rd", mag<25> * link> {} rod;
constexpr struct chain final : named_unit<"ch", mag<4> * rod> {} chain;
constexpr struct furlong final : named_unit<"fur", mag<10> * chain> {} furlong;
// clang-format on

namespace survey1893 {

// clang-format off
constexpr struct us_survey_foot final : named_unit<"ft(us)", mag_ratio<1'200, 3'937> * si::metre> {} us_survey_foot;
constexpr struct link final : named_unit<"li", mag_ratio<33, 50> * us_survey_foot> {} link;
constexpr struct rod final : named_unit<"rd", mag<25> * link> {} rod;
constexpr struct chain final : named_unit<"ch", mag<4> * rod> {} chain;
constexpr struct furlong final : named_unit<"fur", mag<10> * chain> {} furlong;
constexpr struct us_survey_mile final : named_unit<"mi(us)", mag<8> * furlong> {} us_survey_mile;
constexpr struct league final : named_unit<"lea", mag<3> * us_survey_mile> {} league;
// clang-format on

}  // namespace survey1893

// clang-format off
// https://en.wikipedia.org/wiki/United_States_customary_units#Area
constexpr struct acre final : named_unit<"acre", mag<10> * square(survey1893::chain)> {} acre;
constexpr struct section final : named_unit<"section", mag<640> * acre> {} section;

// https://en.wikipedia.org/wiki/United_States_customary_units#Fluid_volume
constexpr struct gallon final : named_unit<"gal", mag<231> * cubic(inch)> {} gallon;
constexpr struct pottle final : named_unit<"pot", mag_ratio<1, 2> * gallon> {} pottle;
constexpr struct quart final : named_unit<"qt", mag_ratio<1, 2> * pottle> {} quart;
constexpr struct pint final : named_unit<"pt", mag_ratio<1, 2> * quart> {} pint;
constexpr struct cup final : named_unit<"c", mag_ratio<1, 2> * pint> {} cup;
constexpr struct gill final : named_unit<"gi", mag_ratio<1, 2> * cup> {} gill;
constexpr struct fluid_ounce final : named_unit<"fl oz", mag_ratio<1, 4> * gill> {} fluid_ounce;
constexpr struct tablespoon final : named_unit<"tbsp", mag_ratio<1, 2> * fluid_ounce> {} tablespoon;
constexpr struct shot final : named_unit<"jig", mag<3> * tablespoon> {} shot;
constexpr struct teaspoon final : named_unit<"tsp", mag_ratio<1, 3> * tablespoon> {} teaspoon;
constexpr struct minim final : named_unit<"min", mag_ratio<1, 80> * teaspoon> {} minim;
constexpr struct fluid_dram final : named_unit<"fl dr", mag<60> * minim> {} fluid_dram;
constexpr struct barrel final : named_unit<"bbl", mag_ratio<315, 10> * gallon> {} barrel;
constexpr struct oil_barrel final : named_unit<"bbl", mag_ratio<4, 3> * barrel> {} oil_barrel;
constexpr struct hogshead final : named_unit<"hogshead", mag<63> * gallon> {} hogshead;

// https://en.wikipedia.org/wiki/United_States_customary_units#Dry_volume
constexpr struct dry_barrel final : named_unit<"bbl", mag<7056> * cubic(inch)> {} dry_barrel;
constexpr struct bushel final : named_unit<"bu", mag_ratio<3'523'907'016'688, 100'000'000'000> * si::litre> {} bushel;
constexpr struct peck final : named_unit<"pk", mag_ratio<1, 4> * bushel> {} peck;
constexpr struct dry_gallon final : named_unit<"gal", mag_ratio<1, 2> * peck> {} dry_gallon;
constexpr struct dry_quart final : named_unit<"qt", mag_ratio<1, 4> * dry_gallon> {} dry_quart;
constexpr struct dry_pint final : named_unit<"pt", mag_ratio<1, 2> * dry_quart> {} dry_pint;

// https://en.wikipedia.org/wiki/United_States_customary_units#Mass_and_Weight
// https://en.wikipedia.org/wiki/Avoirdupois_system#American_customary_system
constexpr struct quarter final : named_unit<"qr", mag<25> * pound> {} quarter;
constexpr struct short_hundredweight final : named_unit<"cwt", mag<100> * pound> {} short_hundredweight;
constexpr struct ton final : named_unit<"t", mag<2'000> * pound> {} ton;
constexpr auto short_ton = ton;
constexpr struct pennyweight final : named_unit<"dwt", mag<24> * grain> {} pennyweight;
constexpr struct troy_once final : named_unit<"oz t", mag<20> * pennyweight> {} troy_once;
constexpr struct troy_pound final : named_unit<"lb t", mag<12> * troy_once> {} troy_pound;

// https://en.wikipedia.org/wiki/Inch_of_mercury
#ifdef pascal
#pragma push_macro("pascal")
#undef pascal
#define MP_UNITS_REDEFINE_PASCAL
#endif
constexpr struct inch_of_mercury final : named_unit<"inHg", mag_ratio<3'386'389, 1'000> * si::pascal> {} inch_of_mercury;
#ifdef MP_UNITS_REDEFINE_PASCAL
#pragma pop_macro("pascal")
#undef MP_UNITS_REDEFINE_PASCAL
#endif

// https://en.wikipedia.org/wiki/United_States_customary_units#Temperature
constexpr struct zeroth_degree_Fahrenheit final : relative_point_origin<absolute<mag_ratio<5, 9> * si::degree_Celsius>(-32)> {} zeroth_degree_Fahrenheit;
constexpr struct degree_Fahrenheit final : named_unit<symbol_text{u8"℉", "`F"}, mag_ratio<5, 9> * si::degree_Celsius, zeroth_degree_Fahrenheit> {} degree_Fahrenheit;

// clang-format on

namespace unit_symbols {

using namespace international::unit_symbols;

constexpr auto ftm = fathom;
constexpr auto cb = cable;
[[deprecated(
  "In accordance with NIST SP 811, as of January 1, 2023, the use of the U.S. survey foot and U.S. survey mile is "
  "deprecated.")]] constexpr struct us_survey_foot us_ft;
[[deprecated(
  "In accordance with NIST SP 811, as of January 1, 2023, the use of the U.S. survey foot and U.S. survey mile is "
  "deprecated.")]] constexpr struct us_survey_mile us_mi;
constexpr auto li = link;
constexpr auto rd = rod;
constexpr auto ch = chain;
constexpr auto fur = furlong;
constexpr auto lea = league;

constexpr auto gal = gallon;
constexpr auto pot = pottle;
constexpr auto qt = quart;
constexpr auto pt = pint;
constexpr auto c = cup;
constexpr auto gi = gill;
constexpr auto fl_oz = fluid_ounce;
constexpr auto tbsp = tablespoon;
constexpr auto jig = shot;
constexpr auto tsp = teaspoon;
constexpr auto min = minim;
constexpr auto fl_dr = fluid_dram;
constexpr auto bbl = barrel;

constexpr auto dry_bbl = dry_barrel;
constexpr auto bu = bushel;
constexpr auto pk = peck;
constexpr auto dry_gal = dry_gallon;
constexpr auto dry_qt = dry_quart;
constexpr auto dry_pt = dry_pint;

constexpr auto qr = quarter;
constexpr auto cwt = short_hundredweight;
constexpr auto t = ton;
constexpr auto dwt = pennyweight;
constexpr auto oz_t = troy_once;
constexpr auto lb_t = troy_pound;

constexpr auto inHg = inch_of_mercury;

constexpr auto deg_F = degree_Fahrenheit;

}  // namespace unit_symbols

}  // namespace mp_units::usc
