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

#include <mp-units/bits/fixed_point.h>
#include <mp-units/compat_macros.h>
#include <mp-units/framework.h>
#include <mp-units/systems/angular.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cstdint>
#include <type_traits>
#endif

using namespace mp_units;

namespace {

// min_width_uint_t selects the narrowest standard unsigned type that holds N bits
static_assert(std::is_same_v<detail::min_width_uint_t<1>, std::uint8_t>);
static_assert(std::is_same_v<detail::min_width_uint_t<7>, std::uint8_t>);
static_assert(std::is_same_v<detail::min_width_uint_t<8>, std::uint8_t>);
static_assert(std::is_same_v<detail::min_width_uint_t<9>, std::uint16_t>);
static_assert(std::is_same_v<detail::min_width_uint_t<31>, std::uint32_t>);
static_assert(std::is_same_v<detail::min_width_uint_t<32>, std::uint32_t>);
static_assert(std::is_same_v<detail::min_width_uint_t<33>, std::uint64_t>);

// scale<To>(M{}, value) — integer-to-integer path (exact arithmetic, no floating point)

// integral factor: exact integer multiply
static_assert(scale<int>(mag<1000>, 5) == 5000);
static_assert(scale<long>(mag<60>, 2l) == 120l);

// integral inverse: exact integer divide
static_assert(scale<int>(mag_ratio<1, 1000>, 5000) == 5);
static_assert(scale<int>(mag_ratio<1, 60>, 120) == 2);

// rational M (3/2 * 4 == 6): exact widened integer arithmetic (int64_t for int)
static_assert(scale<int>(mag_ratio<3, 2>, 4) == 6);
// (1/3 * 9 == 3)
static_assert(scale<int>(mag_ratio<1, 3>, 9) == 3);

// identity
static_assert(scale<int>(mag<1>, 42) == 42);

// floating-point path
static_assert(scale<double>(mag_ratio<1, 2>, 1.0) == 0.5);
static_assert(scale<float>(mag<3>, 1.0f) == 3.0f);

// rounding policies on the integral-inverse (pure divisor) path
static_assert(scale<int>(mag_ratio<1, 1000>, 1567, rounded) == 2);
static_assert(scale<int>(mag_ratio<1, 1000>, 1500, rounded) == 2);  // tie rounds to even
static_assert(scale<int>(mag_ratio<1, 1000>, 2500, rounded) == 2);  // tie rounds to even
static_assert(scale<int>(mag_ratio<1, 1000>, -1234, truncated) == -1);
static_assert(scale<int>(mag_ratio<1, 1000>, -1234, rounded_down) == -2);
static_assert(scale<int>(mag_ratio<1, 1000>, -1234, rounded_up) == -1);
static_assert(scale<int>(mag_ratio<1, 1000>, 1234, rounded_up) == 2);

// rounding policies on the rational path (widened integer arithmetic)
static_assert(scale<int>(mag_ratio<3, 2>, 3, truncated) == 4);    // 4.5
static_assert(scale<int>(mag_ratio<3, 2>, 3, rounded) == 4);      // tie rounds to even
static_assert(scale<int>(mag_ratio<3, 2>, 5, rounded) == 8);      // 7.5, tie rounds to even
static_assert(scale<int>(mag_ratio<3, 2>, 3, rounded_up) == 5);   // 4.5
static_assert(scale<int>(mag_ratio<3, 2>, -3, truncated) == -4);  // -4.5
static_assert(scale<int>(mag_ratio<3, 2>, -3, rounded_down) == -5);
static_assert(scale<int>(mag_ratio<3, 2>, -3, rounded) == -4);  // tie rounds to even

// UnitMagnitudeScalable concept
static_assert(detail::UnitMagnitudeScalable<int>);
static_assert(detail::UnitMagnitudeScalable<long>);
static_assert(detail::UnitMagnitudeScalable<double>);
static_assert(detail::UnitMagnitudeScalable<float>);

// Irrational magnitude conversions with integer representation require explicit value_cast.
// deg = (π/180) rad — the conversion factor is irrational, so every integer result is approximate.
//
// Positive: value_cast compiles and produces the expected truncated integer result.
static_assert(value_cast<angular::degree>(1 * angular::radian).numerical_value_in(angular::degree) == 57);
static_assert(value_cast<angular::radian>(180 * angular::degree).numerical_value_in(angular::radian) == 3);

// Negative: implicit conversion is blocked at compile time to prevent accidental precision loss.
static_assert(!std::is_convertible_v<quantity<angular::radian, int>, quantity<angular::degree, int>>);
static_assert(!std::is_convertible_v<quantity<angular::degree, int>, quantity<angular::radian, int>>);

// Rounding policies on the irrational-factor path (deg = (π/180) rad, fixed-point multiply
// followed by a right shift). The shift floors, so every policy but `rounded_down` has to
// adjust the quotient, which makes this the most delicate of the three scaling paths.
static_assert((50 * angular::degree).in(angular::radian, truncated).numerical_value_in(angular::radian) == 0);
static_assert((50 * angular::degree).in(angular::radian, rounded).numerical_value_in(angular::radian) == 1);
static_assert((50 * angular::degree).in(angular::radian, rounded_down).numerical_value_in(angular::radian) == 0);
static_assert((50 * angular::degree).in(angular::radian, rounded_up).numerical_value_in(angular::radian) == 1);

// 120 deg is 2.0944 rad: only `rounded_up` crosses to the next integer
static_assert((120 * angular::degree).in(angular::radian, truncated).numerical_value_in(angular::radian) == 2);
static_assert((120 * angular::degree).in(angular::radian, rounded).numerical_value_in(angular::radian) == 2);
static_assert((120 * angular::degree).in(angular::radian, rounded_down).numerical_value_in(angular::radian) == 2);
static_assert((120 * angular::degree).in(angular::radian, rounded_up).numerical_value_in(angular::radian) == 3);

// negative values: `truncated` rounds towards zero, `rounded_down` towards negative infinity
static_assert((-120 * angular::degree).in(angular::radian, truncated).numerical_value_in(angular::radian) == -2);
static_assert((-120 * angular::degree).in(angular::radian, rounded).numerical_value_in(angular::radian) == -2);
static_assert((-120 * angular::degree).in(angular::radian, rounded_down).numerical_value_in(angular::radian) == -3);
static_assert((-120 * angular::degree).in(angular::radian, rounded_up).numerical_value_in(angular::radian) == -2);

// -30 deg is -0.5236 rad, so the nearest integer is -1 while truncation yields 0
static_assert((-30 * angular::degree).in(angular::radian, truncated).numerical_value_in(angular::radian) == 0);
static_assert((-30 * angular::degree).in(angular::radian, rounded).numerical_value_in(angular::radian) == -1);
static_assert((-30 * angular::degree).in(angular::radian, rounded_down).numerical_value_in(angular::radian) == -1);
static_assert((-30 * angular::degree).in(angular::radian, rounded_up).numerical_value_in(angular::radian) == 0);

// the reverse direction (rad -> deg, factor 180/π): 100 rad is 5729.578 deg
static_assert((100 * angular::radian).in(angular::degree, truncated).numerical_value_in(angular::degree) == 5729);
static_assert((100 * angular::radian).in(angular::degree, rounded).numerical_value_in(angular::degree) == 5730);
static_assert((-100 * angular::radian).in(angular::degree, truncated).numerical_value_in(angular::degree) == -5729);
static_assert((-100 * angular::radian).in(angular::degree, rounded).numerical_value_in(angular::degree) == -5730);
static_assert((-100 * angular::radian).in(angular::degree, rounded_down).numerical_value_in(angular::degree) == -5730);
static_assert((-100 * angular::radian).in(angular::degree, rounded_up).numerical_value_in(angular::degree) == -5729);

// Large-value safety: deg -> grad uses factor 10/9.  Being a pure rational, the
// computation uses exact 128-bit integer arithmetic — correct on all platforms,
// including ARM / Apple Silicon where long double == double (64-bit mantissa).
static_assert(value_cast<angular::gradian>(std::int64_t{1'000'000'000'000'000'000} * angular::degree)
                .numerical_value_in(angular::gradian) == std::int64_t{1'111'111'111'111'111'111});

}  // namespace
