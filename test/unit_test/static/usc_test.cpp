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

#include <mp-units/systems/isq/mechanics.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si/unit_symbols.h>
#include <mp-units/systems/usc/usc.h>

namespace {

using namespace mp_units;
using namespace mp_units::usc;
using namespace mp_units::usc::unit_symbols;

/* ************** BASE DIMENSIONS **************** */

// https://en.wikipedia.org/wiki/United_States_customary_units#Length

// International
static_assert(isq::length(20 * twip) == isq::length(1 * p));
static_assert(isq::length(1000 * mil) == isq::length(1 * in));
static_assert(isq::length(72 * p) == isq::length(1 * in));
static_assert(isq::length(1 * P) == isq::length(12 * p));
static_assert(isq::length(1 * in) == isq::length(6 * P));
static_assert(isq::length(1 * ft) == isq::length(12 * in));
static_assert(isq::length(1 * yd) == isq::length(3 * ft));
static_assert(isq::length(1 * mi) == isq::length(5280 * ft));
static_assert(isq::length(1 * le) == isq::length(5280 * yd));

// International Nautical
static_assert(isq::length(1 * ftm) == isq::length(2 * yd));
static_assert(isq::length(1 * cb) == isq::length(120 * ftm));
static_assert(isq::length(1 * nmi) == isq::length(1852 * si::metre));

// US survey
MP_UNITS_DIAGNOSTIC_PUSH
MP_UNITS_DIAGNOSTIC_IGNORE_DEPRECATED
static_assert(isq::length(3937 * us_ft) == isq::length(1200 * si::metre));
static_assert(isq::length(3937 * us_mi) == isq::length(6336 * si::kilo<si::metre>));
MP_UNITS_DIAGNOSTIC_POP

static_assert(isq::length(50 * li) == isq::length(33 * ft));
static_assert(isq::length(1 * rd) == isq::length(25 * li));
static_assert(isq::length(1 * ch) == isq::length(4 * rd));
static_assert(isq::length(1 * fur) == isq::length(10 * ch));
static_assert(isq::length(1 * lea) == isq::length(3 * mi));

static_assert(isq::length(50 * survey1893::link) == isq::length(33 * survey1893::us_survey_foot));
static_assert(isq::length(3937 * survey1893::us_survey_foot) == isq::length(1200 * si::metre));
static_assert(isq::length(1 * survey1893::rod) == isq::length(25 * survey1893::link));
static_assert(isq::length(1 * survey1893::chain) == isq::length(4 * survey1893::rod));
static_assert(isq::length(1 * survey1893::furlong) == isq::length(10 * survey1893::chain));
static_assert(isq::length(1 * survey1893::us_survey_mile) == isq::length(8 * survey1893::furlong));
static_assert(isq::length(1 * survey1893::league) == isq::length(3 * survey1893::us_survey_mile));

// Area
// TODO verify the below
// static_assert(isq::area(1 * square(survey1893::us_survey_foot)) == isq::area(144 * square(inch)));
static_assert(isq::area(1 * square(survey1893::chain)) == isq::area(4356 * square(survey1893::us_survey_foot)));
static_assert(isq::area(1 * acre) == isq::area(43560 * square(survey1893::us_survey_foot)));
static_assert(isq::area(1 * section) == isq::area(1 * square(survey1893::us_survey_mile)));

// Volume
static_assert(isq::volume(1 * cubic(foot)) == isq::volume(1'728 * cubic(inch)));
static_assert(isq::volume(1 * cubic(yard)) == isq::volume(27 * cubic(foot)));
static_assert(isq::volume(1 * (acre * survey1893::us_survey_foot)) ==
              isq::volume(43'560 * cubic(survey1893::us_survey_foot)));

// Fluid volume
static_assert(isq::volume(1 * fl_dr) == isq::volume(60 * min));
static_assert(isq::volume(1 * tsp) == isq::volume(80 * min));
static_assert(isq::volume(1 * tbsp) == isq::volume(3 * tsp));
static_assert(isq::volume(1 * fl_oz) == isq::volume(2 * tbsp));
static_assert(isq::volume(1 * jig) == isq::volume(3 * tbsp));
static_assert(isq::volume(1 * gi) == isq::volume(4 * fl_oz));
static_assert(isq::volume(1 * c) == isq::volume(2 * gi));
static_assert(isq::volume(1 * pt) == isq::volume(2 * c));
static_assert(isq::volume(1 * qt) == isq::volume(2 * pt));
static_assert(isq::volume(1 * pot) == isq::volume(2 * qt));
static_assert(isq::volume(1 * gal) == isq::volume(4 * qt));
static_assert(isq::volume(2 * bbl) == isq::volume(63 * gal));
static_assert(isq::volume(3 * oil_barrel) == isq::volume(4 * bbl));
static_assert(isq::volume(2 * hogshead) == isq::volume(3 * oil_barrel));

// Dry volume
static_assert(isq::volume(1 * dry_qt) == isq::volume(2 * dry_pt));
static_assert(isq::volume(1 * dry_gal) == isq::volume(4 * dry_qt));
static_assert(isq::volume(1 * pk) == isq::volume(2 * dry_gal));
static_assert(isq::volume(1 * bu) == isq::volume(4 * pk));
static_assert(isq::volume(1 * dry_bbl) == isq::volume(7056 * cubic(inch)));

// Mass
static_assert(isq::mass(7'000 * gr) == isq::mass(1 * lb));
static_assert(isq::mass(32 * dr) == isq::mass(875 * gr));
static_assert(isq::mass(1 * oz) == isq::mass(16 * dr));
static_assert(isq::mass(1 * lb) == isq::mass(16 * oz));
static_assert(isq::mass(1 * qr) == isq::mass(25 * lb));
static_assert(isq::mass(1 * cwt) == isq::mass(4 * qr));
static_assert(isq::mass(1 * t) == isq::mass(20 * cwt));
static_assert(isq::mass(1 * dwt) == isq::mass(24 * gr));
static_assert(isq::mass(1 * oz_t) == isq::mass(20 * dwt));
static_assert(isq::mass(1 * lb_t) == isq::mass(12 * oz_t));

// Pressure
static_assert(isq::pressure(1'000 * inHg) == isq::pressure(3'386'389 * si::pascal));

// Temperature
static_assert(isq::thermodynamic_temperature(9 * deg_F) == isq::thermodynamic_temperature(5 * si::degree_Celsius));

}  // namespace
