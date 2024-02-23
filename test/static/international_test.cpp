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

#include <mp-units/math.h>
#include <mp-units/systems/international/international.h>
#include <mp-units/systems/isq/mechanics.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si/constants.h>
#include <mp-units/systems/si/units.h>

template<class T>
  requires mp_units::is_scalar<T>
inline constexpr bool mp_units::is_vector<T> = true;

namespace {

using namespace mp_units;
using namespace mp_units::international;
using namespace mp_units::international::unit_symbols;

// Mass
static_assert(100'000'000 * isq::mass[lb] == 45'359'237 * isq::mass[si::kilogram]);
static_assert(1 * isq::mass[lb] == 16 * isq::mass[oz]);
static_assert(1 * isq::mass[oz] == 16 * isq::mass[dr]);
static_assert(7'000 * isq::mass[gr] == 1 * isq::mass[lb]);

// Length
// https://en.wikipedia.org/wiki/United_States_customary_units#Length
static_assert(20 * isq::length[twip] == 1 * isq::length[p]);
static_assert(1000 * isq::length[mil] == 1 * isq::length[in]);
static_assert(72 * isq::length[p] == 1 * isq::length[in]);
static_assert(1 * isq::length[P] == 12 * isq::length[p]);
static_assert(1 * isq::length[in] == 6 * isq::length[P]);
static_assert(1 * isq::length[ft] == 12 * isq::length[in]);
static_assert(1 * isq::length[yd] == 3 * isq::length[ft]);
static_assert(10'000 * isq::length[yd] == 9'144 * isq::length[si::metre]);
static_assert(1 * isq::length[mi] == 5280 * isq::length[ft]);
static_assert(1 * isq::length[mi] == 1760 * isq::length[yd]);
static_assert(1 * isq::length[le] == 15'840 * isq::length[ft]);
static_assert(1 * isq::length[le] == 5280 * isq::length[yd]);

// Nautical
static_assert(1 * isq::length[nmi] == 1852 * isq::length[si::metre]);
static_assert(1 * isq::speed[kn] == isq::length(1 * nmi) / isq::time(1 * si::hour));

// Force
static_assert(1 * isq::force[pdl] ==
              isq::mass(1 * lb) * isq::length(1 * ft) / (isq::time(1 * si::second) * isq::time(1 * si::second)));
static_assert(1 * isq::force[lbf] == isq::mass(1 * lb) * (1 * si::standard_gravity));
static_assert(1 * isq::force[kip] == 1000 * isq::force[lbf]);

// Pressure
static_assert(1 * isq::pressure[psi] == isq::force(1 * lbf) / isq::area(1 * square(inch)));

// Power
static_assert(1 * isq::power[hp] == isq::length(33'000 * ft) * isq::force(1 * lbf) / isq::time(1 * si::minute));

}  // namespace
