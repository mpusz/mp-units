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
#include <mp-units/systems/international/international.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/unit.h>
#endif

MP_UNITS_EXPORT
namespace mp_units::usc {

using namespace international;

// clang-format off

// https://en.wikipedia.org/wiki/United_States_customary_units#Length
// nautical
inline constexpr struct fathom : named_unit<"ftm(us)", mag<2> * yard> {} fathom;
inline constexpr struct cable : named_unit<"cb(us)", mag<120> * fathom> {} cable;

// survey
struct us_survey_foot : named_unit<"ft(us)", mag<ratio{1'200, 3'937}> * si::metre> {};
struct us_survey_mile : named_unit<"mi(us)", mag<5280> * us_survey_foot{}> {};

[[deprecated("In accordance with NIST SP 811, as of January 1, 2023, the use of the U.S. survey foot and U.S. survey mile is deprecated.")]]
inline constexpr us_survey_foot us_survey_foot;

[[deprecated("In accordance with NIST SP 811, as of January 1, 2023, the use of the U.S. survey foot and U.S. survey mile is deprecated.")]]
inline constexpr us_survey_mile us_survey_mile;

inline constexpr struct link : named_unit<"li", mag<ratio{33, 50}> * foot> {} link;
inline constexpr struct rod : named_unit<"rd", mag<25> * link> {} rod;
inline constexpr struct chain : named_unit<"ch", mag<4> * rod> {} chain;
inline constexpr struct furlong : named_unit<"fur", mag<10> * chain> {} furlong;
// clang-format on

namespace survey1893 {

// clang-format off
inline constexpr struct us_survey_foot : named_unit<"ft(us)", mag<ratio{1'200, 3'937}> * si::metre> {} us_survey_foot;
inline constexpr struct link : named_unit<"li", mag<ratio{33, 50}> * us_survey_foot> {} link;
inline constexpr struct rod : named_unit<"rd", mag<25> * link> {} rod;
inline constexpr struct chain : named_unit<"ch", mag<4> * rod> {} chain;
inline constexpr struct furlong : named_unit<"fur", mag<10> * chain> {} furlong;
inline constexpr struct us_survey_mile : named_unit<"mi(us)", mag<8> * furlong> {} us_survey_mile;
inline constexpr struct league : named_unit<"lea", mag<3> * us_survey_mile> {} league;
// clang-format on

}  // namespace survey1893

// clang-format off
// https://en.wikipedia.org/wiki/United_States_customary_units#Area
inline constexpr struct acre : named_unit<"acre", mag<10> * square(survey1893::chain)> {} acre;
inline constexpr struct section : named_unit<"section", mag<640> * acre> {} section;

// https://en.wikipedia.org/wiki/United_States_customary_units#Fluid_volume
inline constexpr struct gallon : named_unit<"gal", mag<231> * cubic(inch)> {} gallon;
inline constexpr struct pottle : named_unit<"pot", mag<ratio{1, 2}> * gallon> {} pottle;
inline constexpr struct quart : named_unit<"qt", mag<ratio{1, 2}> * pottle> {} quart;
inline constexpr struct pint : named_unit<"pt", mag<ratio{1, 2}> * quart> {} pint;
inline constexpr struct cup : named_unit<"c", mag<ratio{1, 2}> * pint> {} cup;
inline constexpr struct gill : named_unit<"gi", mag<ratio{1, 2}> * cup> {} gill;
inline constexpr struct fluid_ounce : named_unit<"fl oz", mag<ratio{1, 4}> * gill> {} fluid_ounce;
inline constexpr struct tablespoon : named_unit<"tbsp", mag<ratio{1, 2}> * fluid_ounce> {} tablespoon;
inline constexpr struct shot : named_unit<"jig", mag<3> * tablespoon> {} shot;
inline constexpr struct teaspoon : named_unit<"tsp", mag<ratio{1, 3}> * tablespoon> {} teaspoon;
inline constexpr struct minim : named_unit<"min", mag<ratio{1, 80}> * teaspoon> {} minim;
inline constexpr struct fluid_dram : named_unit<"fl dr", mag<60> * minim> {} fluid_dram;
inline constexpr struct barrel : named_unit<"bbl", mag<ratio{315, 10}> * gallon> {} barrel;
inline constexpr struct oil_barrel : named_unit<"bbl", mag<ratio{4, 3}> * barrel> {} oil_barrel;
inline constexpr struct hogshead : decltype(mag<63> * gallon) {} hogshead;

// https://en.wikipedia.org/wiki/United_States_customary_units#Dry_volume
inline constexpr struct dry_barrel : named_unit<"bbl", mag<7056> * cubic(inch)> {} dry_barrel;
inline constexpr struct bushel : named_unit<"bu", mag<ratio{3'523'907'016'688, 100'000'000'000}> * si::litre> {} bushel;
inline constexpr struct peck : named_unit<"pk", mag<ratio{1, 4}> * bushel> {} peck;
inline constexpr struct dry_gallon : named_unit<"gal", mag<ratio{1, 2}> * peck> {} dry_gallon;
inline constexpr struct dry_quart : named_unit<"qt", mag<ratio{1, 4}> * dry_gallon> {} dry_quart;
inline constexpr struct dry_pint : named_unit<"pt", mag<ratio{1, 2}> * dry_quart> {} dry_pint;

// https://en.wikipedia.org/wiki/United_States_customary_units#Mass_and_Weight
// https://en.wikipedia.org/wiki/Avoirdupois_system#American_customary_system
inline constexpr struct quarter : named_unit<"qr", mag<25> * pound> {} quarter;
inline constexpr struct short_hundredweight : named_unit<"cwt", mag<100> * pound> {} short_hundredweight;
inline constexpr struct ton : named_unit<"t", mag<2'000> * pound> {} ton;
inline constexpr auto short_ton = ton;
inline constexpr struct pennyweight : named_unit<"dwt", mag<24> * grain> {} pennyweight;
inline constexpr struct troy_once : named_unit<"oz t", mag<20> * pennyweight> {} troy_once;
inline constexpr struct troy_pound : named_unit<"lb t", mag<12> * troy_once> {} troy_pound;

// https://en.wikipedia.org/wiki/Inch_of_mercury
inline constexpr struct inch_of_mercury : named_unit<"inHg", mag<ratio(3'386'389, 1'000)> * si::pascal> {} inch_of_mercury;

// https://en.wikipedia.org/wiki/United_States_customary_units#Temperature
inline constexpr struct zeroth_degree_Fahrenheit : relative_point_origin<si::zeroth_degree_Celsius - 32 * (mag<ratio{5, 9}> * si::degree_Celsius)> {} zeroth_degree_Fahrenheit;
inline constexpr struct degree_Fahrenheit : named_unit<symbol_text{u8"°F", "`F"}, mag<ratio{5, 9}> * si::degree_Celsius, zeroth_degree_Fahrenheit> {} degree_Fahrenheit;

// clang-format on

namespace unit_symbols {

using namespace international::unit_symbols;

inline constexpr auto ftm = fathom;
inline constexpr auto cb = cable;
[[deprecated(
  "In accordance with NIST SP 811, as of January 1, 2023, the use of the U.S. survey foot and U.S. survey mile is "
  "deprecated.")]] inline constexpr struct us_survey_foot us_ft;
[[deprecated(
  "In accordance with NIST SP 811, as of January 1, 2023, the use of the U.S. survey foot and U.S. survey mile is "
  "deprecated.")]] inline constexpr struct us_survey_mile us_mi;
inline constexpr auto li = link;
inline constexpr auto rd = rod;
inline constexpr auto ch = chain;
inline constexpr auto fur = furlong;
inline constexpr auto lea = league;

inline constexpr auto gal = gallon;
inline constexpr auto pot = pottle;
inline constexpr auto qt = quart;
inline constexpr auto pt = pint;
inline constexpr auto c = cup;
inline constexpr auto gi = gill;
inline constexpr auto fl_oz = fluid_ounce;
inline constexpr auto tbsp = tablespoon;
inline constexpr auto jig = shot;
inline constexpr auto tsp = teaspoon;
inline constexpr auto min = minim;
inline constexpr auto fl_dr = fluid_dram;
inline constexpr auto bbl = barrel;

inline constexpr auto dry_bbl = dry_barrel;
inline constexpr auto bu = bushel;
inline constexpr auto pk = peck;
inline constexpr auto dry_gal = dry_gallon;
inline constexpr auto dry_qt = dry_quart;
inline constexpr auto dry_pt = dry_pint;

inline constexpr auto qr = quarter;
inline constexpr auto cwt = short_hundredweight;
inline constexpr auto t = ton;
inline constexpr auto dwt = pennyweight;
inline constexpr auto oz_t = troy_once;
inline constexpr auto lb_t = troy_pound;

inline constexpr auto inHg = inch_of_mercury;

inline constexpr auto deg_F = degree_Fahrenheit;

}  // namespace unit_symbols

}  // namespace mp_units::usc
