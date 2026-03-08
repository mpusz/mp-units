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
#include <mp-units/systems/si/constants.h>
#include <mp-units/systems/si/unit_symbols.h>
#include <mp-units/systems/si/units.h>
#include <mp-units/systems/yard_pound.h>

namespace {

using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using namespace mp_units::yard_pound;
using namespace mp_units::yard_pound::unit_symbols;

// Mass
static_assert(47 * isq::mass[lb] < 47 * isq::mass[si::kilogram]);
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
static_assert(1 * isq::length[lea] == 15'840 * isq::length[ft]);
static_assert(1 * isq::length[lea] == 5280 * isq::length[yd]);
static_assert(1 * isq::length[ftm] == 2 * isq::length[yd]);

// Nautical
static_assert(1 * isq::length[nmi] == 1852 * isq::length[si::metre]);
static_assert(1 * isq::speed[kn] == isq::length(1 * nmi) / isq::duration(1 * si::hour));

// Force
static_assert(1 * isq::force[pdl] == isq::mass(1 * lb) * isq::length(1 * ft) /
                                       (isq::duration(1 * si::second) * isq::duration(1 * si::second)));
static_assert(1 * isq::force[lbf] == isq::mass(1 * lb) * (1 * si::standard_gravity));
static_assert(1 * isq::force[kip] == 1000 * isq::force[lbf]);

// Pressure
static_assert(1 * isq::pressure[psi] == isq::force(1 * lbf) / isq::area(1 * square(inch)));

// Power
static_assert(1 * isq::power[hp] == isq::length(33'000 * ft) * isq::force(1 * lbf) / isq::duration(1 * si::minute));

// Cross-unit integer comparisons: both operand values fit in int64, but both scaled intermediates
// exceed INT64_MAX and require __int128 arithmetic (double_width_int_for_t<int64_t>) to compare
// correctly — demonstrating the overflow-safe widening introduced for cross-unit comparisons.

// lb vs kg: common unit is their magnitude-GCD (≈ 1.7×10⁻⁷ g), scale factors are huge.
//   100'000'000'000'000 × factor_lb ≈ 4.54×10²¹ > INT64_MAX
static_assert(100'000'000'000'000 * isq::mass[lb] == 45'359'237'000'000 * isq::mass[si::kilogram]);

// mi vs km: common unit is 64 mm (GCD of 1'609'344 mm and 1'000'000 mm);
//   scale factors km→cu = 15'625, mi→cu = 25'146.
//   590'295'810'380'364 × 15'625 ≈ 9.223×10¹⁸ > INT64_MAX
//   366'792'811'468'750 × 25'146 ≈ 9.223×10¹⁸ > INT64_MAX
static_assert(590'295'810'380'364LL * isq::length[km] == 366'792'811'468'750LL * isq::length[mi]);
static_assert(590'295'810'380'364LL * isq::length[km] > 366'792'811'468'749LL * isq::length[mi]);
static_assert(590'295'810'380'364LL * isq::length[km] < 366'792'811'468'751LL * isq::length[mi]);

// Comparisons disabled at compile-time when unit-ratio scaling would overflow even __int128.
// A template wrapper is used so that SFINAE failure inside requires is a soft error, not a
// hard error on GCC.
template<Reference auto R1, Reference auto R2>
constexpr bool no_cross_unit_int_comparison = !requires { 1 * R1 == 1 * R2; } && !requires { 1 * R1 < 1 * R2; } &&
                                              !requires { 1 * R2 == 1 * R1; } && !requires { 1 * R2 < 1 * R1; };

// cubic(mi) vs cubic(pm): factor = (1.609344×10^15)^3 ≈ 4.17×10^45 > 2^127,
// so even value=1 makes integer comparison ill-formed.
static_assert(no_cross_unit_int_comparison<isq::volume[cubic(mi)], isq::volume[cubic(pm)]>);

}  // namespace
