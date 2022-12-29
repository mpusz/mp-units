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

#include <units/systems/isq/space_and_time.h>
#include <units/systems/si/unit_symbols.h>
#include <units/systems/usc/usc.h>

namespace {

using namespace units;
using namespace units::usc;
using namespace units::usc::unit_symbols;

/* ************** BASE DIMENSIONS **************** */

// https://en.wikipedia.org/wiki/United_States_customary_units#Length

// International
static_assert(isq::length[twip](20) == isq::length[p](1));
static_assert(isq::length[mil](1000) == isq::length[in](1));
static_assert(isq::length[p](72) == isq::length[in](1));
static_assert(isq::length[P](1) == isq::length[p](12));
static_assert(isq::length[in](1) == isq::length[P](6));
static_assert(isq::length[ft](1) == isq::length[in](12));
static_assert(isq::length[yd](1) == isq::length[ft](3));
static_assert(isq::length[mi](1) == isq::length[ft](5280));
static_assert(isq::length[le](1) == isq::length[yd](5280));

// International Nautical
static_assert(isq::length[ftm](1) == isq::length[yd](2));
static_assert(isq::length[cb](1) == isq::length[ftm](120));
static_assert(isq::length[nmi](1) == isq::length[si::metre](1852));

// US survey
UNITS_DIAGNOSTIC_PUSH
UNITS_DIAGNOSTIC_IGNORE_DEPRECATED
static_assert(isq::length[us_ft](3937) == isq::length[si::metre](1200));
static_assert(isq::length[us_mi](3937) == isq::length[si::kilo<si::metre>](6336));
UNITS_DIAGNOSTIC_POP

static_assert(isq::length[li](50) == isq::length[ft](33));
static_assert(isq::length[rd](1) == isq::length[li](25));
static_assert(isq::length[ch](1) == isq::length[rd](4));
static_assert(isq::length[fur](1) == isq::length[ch](10));
static_assert(isq::length[lea](1) == isq::length[mi](3));

static_assert(isq::length[survey1893::link](50) == isq::length[survey1893::us_survey_foot](33));
static_assert(isq::length[survey1893::us_survey_foot](3937) == isq::length[si::metre](1200));
static_assert(isq::length[survey1893::rod](1) == isq::length[survey1893::link](25));
static_assert(isq::length[survey1893::chain](1) == isq::length[survey1893::rod](4));
static_assert(isq::length[survey1893::furlong](1) == isq::length[survey1893::chain](10));
static_assert(isq::length[survey1893::us_survey_mile](1) == isq::length[survey1893::furlong](8));
static_assert(isq::length[survey1893::league](1) == isq::length[survey1893::us_survey_mile](3));

// Area
// static_assert(isq::area[square<survey1893::us_survey_foot>](1) == isq::area[square<inch>](144));
static_assert(isq::area[square<survey1893::chain>](1) == isq::area[square<survey1893::us_survey_foot>](4356));
static_assert(isq::area[acre](1) == isq::area[square<survey1893::us_survey_foot>](43560));
static_assert(isq::area[section](1) == isq::area[square<survey1893::us_survey_mile>](1));

// Volume
static_assert(isq::volume[cubic<foot>](1) == isq::volume[cubic<inch>](1'728));
static_assert(isq::volume[cubic<yard>](1) == isq::volume[cubic<foot>](27));
static_assert(isq::volume[acre * survey1893::us_survey_foot](1) ==
              isq::volume[cubic<survey1893::us_survey_foot>](43'560));

// Fluid volume
static_assert(isq::volume[fl_dr](1) == isq::volume[min](60));
static_assert(isq::volume[tsp](1) == isq::volume[min](80));
static_assert(isq::volume[tbsp](1) == isq::volume[tsp](3));
static_assert(isq::volume[fl_oz](1) == isq::volume[tbsp](2));
static_assert(isq::volume[jig](1) == isq::volume[tbsp](3));
static_assert(isq::volume[gi](1) == isq::volume[fl_oz](4));
static_assert(isq::volume[c](1) == isq::volume[gi](2));
static_assert(isq::volume[pt](1) == isq::volume[c](2));
static_assert(isq::volume[qt](1) == isq::volume[pt](2));
static_assert(isq::volume[pot](1) == isq::volume[qt](2));
static_assert(isq::volume[gal](1) == isq::volume[qt](4));
static_assert(isq::volume[bbl](2) == isq::volume[gal](63));
static_assert(isq::volume[oil_barrel](3) == isq::volume[bbl](4));
static_assert(isq::volume[hogshead](2) == isq::volume[oil_barrel](3));

// Dry volume
static_assert(isq::volume[dry_qt](1) == isq::volume[dry_pt](2));
static_assert(isq::volume[dry_gal](1) == isq::volume[dry_qt](4));
static_assert(isq::volume[pk](1) == isq::volume[dry_gal](2));
static_assert(isq::volume[bu](1) == isq::volume[pk](4));
static_assert(isq::volume[dry_bbl](1) == isq::volume[cubic<inch>](7056));

// Mass
static_assert(isq::mass[gr](7'000) == isq::mass[lb](1));
static_assert(isq::mass[dr](32) == isq::mass[gr](875));
static_assert(isq::mass[oz](1) == isq::mass[dr](16));
static_assert(isq::mass[lb](1) == isq::mass[oz](16));
static_assert(isq::mass[qr](1) == isq::mass[lb](25));
static_assert(isq::mass[cwt](1) == isq::mass[qr](4));
static_assert(isq::mass[t](1) == isq::mass[cwt](20));
static_assert(isq::mass[dwt](1) == isq::mass[gr](24));
static_assert(isq::mass[oz_t](1) == isq::mass[dwt](20));
static_assert(isq::mass[lb_t](1) == isq::mass[oz_t](12));

}  // namespace
