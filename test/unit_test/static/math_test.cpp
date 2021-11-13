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

#include "test_tools.h"
#include <units/math.h> // IWYU pragma: keep
#include <units/isq/si/length.h>
#include <units/isq/si/area.h>
#include <units/isq/si/volume.h>
#include <units/isq/si/time.h>
#include <units/isq/si/international/length.h>
#include <units/isq/si/international/area.h>
#include <units/isq/si/international/volume.h>

namespace {

using namespace units;
using namespace units::isq;
using namespace units::isq::si::literals;
using namespace units::isq::si::international::literals;

static_assert(compare<decltype(pow<0>(2_q_m)), std::int64_t>);
static_assert(compare<decltype(pow<1>(2_q_m)), decltype(2_q_m)>);
static_assert(compare<decltype(pow<2>(2_q_m)), decltype(4_q_m2)>);
static_assert(compare<decltype(pow<2>(2_q_km)), decltype(4_q_km2)>);
static_assert(compare<decltype(pow<2>(2_q_ft)), decltype(4_q_ft2)>);
static_assert(compare<decltype(sqrt(4_q_m2)), decltype(2_q_m)>);
static_assert(compare<decltype(sqrt(4_q_km2)), decltype(2_q_km)>);
static_assert(compare<decltype(sqrt(4_q_ft2)), decltype(2_q_ft)>);
static_assert(compare<decltype(cbrt(8_q_m3)), decltype(2_q_m)>);
static_assert(compare<decltype(cbrt(8_q_km3)), decltype(2_q_km)>);
static_assert(compare<decltype(cbrt(8_q_ft3)), decltype(2_q_ft)>);
static_assert(compare<decltype(pow<1, 4>(4_q_m2 * 4_q_m2)), decltype(2_q_m)>);
static_assert(compare<decltype(pow<1, 4>(4_q_km2 * 4_q_km2)), decltype(2_q_km)>);
static_assert(compare<decltype(pow<1, 4>(4_q_ft2 * 4_q_ft2)), decltype(2_q_ft)>);

// rational dimensions
static_assert(compare<decltype(pow<1, 4>(4_q_m2)), decltype(sqrt(2_q_m))>);
static_assert(compare<decltype(pow<1, 4>(4_q_km2)), decltype(sqrt(2_q_km))>);
static_assert(compare<decltype(pow<1, 4>(4_q_ft2)), decltype(sqrt(2_q_ft))>);

#if __cpp_lib_constexpr_cmath  // TODO remove once std::floor, std::ceil, and std::round is constexpr for all compilers
// floor
// integral types
static_assert(compare<decltype(floor<si::second>(1_q_s)), decltype(1_q_s)>);

static_assert(compare<decltype(floor<si::second>(1000_q_ms)), decltype(1_q_s)>);
static_assert(compare<decltype(floor<si::second>(1001_q_ms)), decltype(1_q_s)>);
static_assert(compare<decltype(floor<si::second>(1999_q_ms)), decltype(1_q_s)>);
static_assert(compare<decltype(floor<si::second>(-1000_q_ms)), decltype(-1_q_s)>);
static_assert(compare<decltype(floor<si::second>(-999_q_ms)), decltype(-1_q_s)>);

// floating-point
static_assert(floor<si::second>(1.3_q_s) == 1_q_s);
static_assert(floor<si::second>(-1.3_q_s) == -2_q_s);

// static_assert(floor<si::second>(1000._q_ms) == 1_q_s);  // does not work due to a bug in fpow10() see #311
static_assert(floor<si::second>(1001._q_ms) == 1_q_s);
static_assert(floor<si::second>(1999._q_ms) == 1_q_s);
static_assert(floor<si::second>(-1000._q_ms) == -1_q_s);
static_assert(floor<si::second>(-999._q_ms) == -1_q_s);

// floor with quantity
static_assert(compare<decltype(floor<si::time<si::second>>(1_q_s)), decltype(1_q_s)>);

// ceil
// integral types
static_assert(compare<decltype(ceil<si::second>(1_q_s)), decltype(1_q_s)>);

static_assert(compare<decltype(ceil<si::second>(1000_q_ms)), decltype(1_q_s)>);
static_assert(compare<decltype(ceil<si::second>(1001_q_ms)), decltype(2_q_s)>);
static_assert(compare<decltype(ceil<si::second>(1999_q_ms)), decltype(2_q_s)>);
static_assert(compare<decltype(ceil<si::second>(-1000_q_ms)), decltype(-1_q_s)>);
static_assert(compare<decltype(ceil<si::second>(-999_q_ms)), decltype(0_q_s)>);

// floating-point
static_assert(ceil<si::second>(1.3_q_s) == 2_q_s);
static_assert(ceil<si::second>(-1.3_q_s) == -1_q_s);

static_assert(ceil<si::second>(1000._q_ms) == 1_q_s);
static_assert(ceil<si::second>(1001._q_ms) == 2_q_s);
static_assert(ceil<si::second>(1999._q_ms) == 2_q_s);
static_assert(ceil<si::second>(-1000._q_ms) == -1_q_s);
static_assert(ceil<si::second>(-999._q_ms) == 0_q_s);

// ceil with quantity
static_assert(compare<decltype(ceil<si::time<si::second>>(1_q_s)), decltype(1_q_s)>);

// round
// integral types
static_assert(compare<decltype(round<si::second>(1_q_s)), decltype(1_q_s)>);

static_assert(compare<decltype(round<si::second>(1000_q_ms)), decltype(1_q_s)>);
static_assert(compare<decltype(round<si::second>(1001_q_ms)), decltype(1_q_s)>);
static_assert(compare<decltype(round<si::second>(1499_q_ms)), decltype(1_q_s)>);
static_assert(compare<decltype(round<si::second>(1500_q_ms)), decltype(2_q_s)>);
static_assert(compare<decltype(round<si::second>(1999_q_ms)), decltype(2_q_s)>);

static_assert(compare<decltype(round<si::second>(-1000_q_ms)), decltype(-1_q_s)>);
static_assert(compare<decltype(round<si::second>(-1001_q_ms)), decltype(-1_q_s)>);
static_assert(compare<decltype(round<si::second>(-1499_q_ms)), decltype(-1_q_s)>);
static_assert(compare<decltype(round<si::second>(-1500_q_ms)), decltype(-2_q_s)>);
static_assert(compare<decltype(round<si::second>(-1999_q_ms)), decltype(-2_q_s)>);

// floating-point
static_assert(round<si::second>(1.3_q_s) == 1_q_s);
static_assert(round<si::second>(-1.3_q_s) == -1_q_s);

static_assert(compare<decltype(round<si::second>(1000._q_ms)), decltype(1_q_s)>);
static_assert(compare<decltype(round<si::second>(1001._q_ms)), decltype(1_q_s)>);
static_assert(compare<decltype(round<si::second>(1499._q_ms)), decltype(1_q_s)>);
static_assert(compare<decltype(round<si::second>(1500._q_ms)), decltype(2_q_s)>);
static_assert(compare<decltype(round<si::second>(1999._q_ms)), decltype(2_q_s)>);

static_assert(compare<decltype(round<si::second>(-1000._q_ms)), decltype(-1_q_s)>);
static_assert(compare<decltype(round<si::second>(-1001._q_ms)), decltype(-1_q_s)>);
static_assert(compare<decltype(round<si::second>(-1499._q_ms)), decltype(-1_q_s)>);
static_assert(compare<decltype(round<si::second>(-1500._q_ms)), decltype(-2_q_s)>);
static_assert(compare<decltype(round<si::second>(-1999._q_ms)), decltype(-2_q_s)>);
#endif

}  // namespace
