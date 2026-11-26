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

module;

#include <mp-units/bits/core_gmf.h>
#include <chrono>

export module mp_units.systems;

export import mp_units.core;
#ifdef MP_UNITS_IMPORT_STD
import std;
#endif

#define MP_UNITS_IN_MODULE_INTERFACE

#include <mp-units/systems/angular.h>
#include <mp-units/systems/cgs.h>
#include <mp-units/systems/codata.h>
#include <mp-units/systems/hep.h>
#include <mp-units/systems/iau.h>
#include <mp-units/systems/iec.h>
#include <mp-units/systems/iec80000.h>
#include <mp-units/systems/imperial.h>
#include <mp-units/systems/international.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/isq_angle.h>
#include <mp-units/systems/iupac.h>
#include <mp-units/systems/natural.h>
#include <mp-units/systems/si.h>
#include <mp-units/systems/typographic.h>
#include <mp-units/systems/usc.h>
#include <mp-units/systems/yard_pound.h>

// Unit conversions need the canonical units of the units involved. Computing them here for the SI
// base units, the SI units with special names and a few common derived units puts them in the BMI,
// where an importer finds them instead of instantiating them itself (measured with Clang).
namespace mp_units::detail {

template<Unit auto... Us>
constexpr bool warm_canonical_units = (((void)get_canonical_unit(Us), ...), true);

static_assert(
  warm_canonical_units<si::metre, si::kilogram, si::second, si::ampere, si::kelvin, si::mole, si::candela, si::radian,
                       si::steradian, si::hertz, si::newton, si::pascal, si::joule, si::watt, si::coulomb, si::volt,
                       si::farad, si::ohm, si::siemens, si::weber, si::tesla, si::henry, si::lumen, si::lux,
                       si::becquerel, si::gray, si::sievert, si::katal, square(si::metre), cubic(si::metre),
                       si::metre / si::second, si::metre / square(si::second)>);

}  // namespace mp_units::detail
