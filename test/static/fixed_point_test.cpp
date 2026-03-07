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

static_assert(std::is_same_v<detail::min_width_uint_t<1>, std::uint8_t>);
static_assert(std::is_same_v<detail::min_width_uint_t<7>, std::uint8_t>);
static_assert(std::is_same_v<detail::min_width_uint_t<8>, std::uint8_t>);
static_assert(std::is_same_v<detail::min_width_uint_t<9>, std::uint16_t>);
static_assert(std::is_same_v<detail::min_width_uint_t<31>, std::uint32_t>);
static_assert(std::is_same_v<detail::min_width_uint_t<32>, std::uint32_t>);
static_assert(std::is_same_v<detail::min_width_uint_t<33>, std::uint64_t>);

using i128 = detail::double_width_int<std::int64_t>;
using u128 = detail::double_width_int<std::uint64_t>;

static_assert((((83 * 79 * 73) * (i128{97} << 64u) / 89) >> 64u) == (83 * 79 * 73 * 97) / 89);

// scale<To>(M{}, value) — integer-to-integer path (exact arithmetic, no floating point)
// scale(M{}, value)     — floating-point same-type shorthand (To = From, uses value_type_t<From>)

// integral factor: exact integer multiply
static_assert(scale<int>(mag<1000>, 5) == 5000);
static_assert(scale<long>(mag<60>, 2l) == 120l);

// integral inverse: exact integer divide
static_assert(scale<int>(mag_ratio<1, 1000>, 5000) == 5);
static_assert(scale<int>(mag_ratio<1, 60>, 120) == 2);

// rational M (3/2 * 4 == 6): exact double-width integer arithmetic
static_assert(scale<int>(mag_ratio<3, 2>, 4) == 6);
// (1/3 * 9 == 3)
static_assert(scale<int>(mag_ratio<1, 3>, 9) == 3);

// identity
static_assert(scale<int>(mag<1>, 42) == 42);

// floating-point path
static_assert(scale<double>(mag_ratio<1, 2>, 1.0) == 0.5);
static_assert(scale<float>(mag<3>, 1.0f) == 3.0f);

// MagnitudeScalable concept
static_assert(detail::MagnitudeScalable<int>);
static_assert(detail::MagnitudeScalable<long>);
static_assert(detail::MagnitudeScalable<double>);
static_assert(detail::MagnitudeScalable<float>);

// Irrational magnitude conversions with integer representation require explicit value_cast.
// deg = (π/180) rad — the conversion factor is irrational, so every integer result is approximate.
//
// Positive: value_cast compiles and produces the expected truncated integer result.
static_assert(value_cast<angular::degree>(1 * angular::radian).numerical_value_in(angular::degree) == 57);
static_assert(value_cast<angular::radian>(180 * angular::degree).numerical_value_in(angular::radian) == 3);

// Negative: implicit conversion is blocked at compile time to prevent accidental precision loss.
static_assert(!std::is_convertible_v<quantity<angular::radian, int>, quantity<angular::degree, int>>);
static_assert(!std::is_convertible_v<quantity<angular::degree, int>, quantity<angular::radian, int>>);

// Large-value safety: deg -> grad uses factor 10/9.  Being a pure rational, the
// computation uses exact 128-bit integer arithmetic — correct on all platforms,
// including ARM / Apple Silicon where long double == double (64-bit mantissa).
static_assert(value_cast<angular::gradian>(std::int64_t{1'000'000'000'000'000'000} * angular::degree)
                .numerical_value_in(angular::gradian) == std::int64_t{1'111'111'111'111'111'111});

}  // namespace
