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

#include <mp-units/systems/codata/adopted_values.h>
#include <mp-units/systems/isq/mechanics.h>
#include <mp-units/systems/isq/thermodynamics.h>
#include <mp-units/systems/iupac.h>
#include <mp-units/systems/si/unit_symbols.h>
#include <mp-units/systems/si/units.h>

/* ************** IUPAC Green Book, section 7.2 - Conversion tables for units **************** */

namespace {

using namespace mp_units;
using namespace mp_units::iupac::unit_symbols;
using namespace mp_units::si::unit_symbols;

// pressure
// the standard atmosphere is the CODATA adopted value, not a unit of its own
static_assert(atm == codata::standard_atmosphere);
static_assert(equivalent(atm, mag<101'325>* Pa));

static_assert(equivalent(Torr, mag_ratio<101'325, 760>* Pa));
static_assert(isq::pressure(760 * Torr) == isq::pressure(1 * atm));

// the conventional definition gives exactly 133.322 387 415 Pa
static_assert(equivalent(mmHg, mag_ratio<133'322'387'415, 1'000'000'000>* Pa));

// 760 mmHg is 101 325.014 435 4 Pa, not one atmosphere, so mmHg and Torr are different units
static_assert(!equivalent(mmHg, Torr));
static_assert(isq::pressure(760. * mmHg) > isq::pressure(1. * atm));

// energy
static_assert(equivalent(cal_th, mag_ratio<4'184, 1'000>* J));
static_assert(equivalent(cal_IT, mag_ratio<41'868, 10'000>* J));
static_assert(isq::energy(1'000 * cal_th) == isq::energy(4'184 * J));
static_assert(isq::energy(10'000 * cal_IT) == isq::energy(41'868 * J));
static_assert(!equivalent(cal_th, cal_IT));

}  // namespace
