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

#include <units/ratio.h>

namespace {

using namespace units;

static_assert(ratio(2, 4) == ratio(1, 2));

// basic exponents tests
static_assert(ratio(2, 40, 1) == ratio(1, 20, 1));
static_assert(ratio(20, 4, -1) == ratio(10, 2, -1));
static_assert(ratio(200, 5) == ratio(20'000, 50, -1));

static_assert(ratio(1) * ratio(3, 8) == ratio(3, 8));
static_assert(ratio(3, 8) * ratio(1) == ratio(3, 8));
static_assert(ratio(4) * ratio(1, 8) == ratio(1, 2));
static_assert(ratio(4) * ratio(1, 2) == ratio(2));
static_assert(ratio(1, 8) * ratio(2) == ratio(1, 4));
static_assert(ratio(1, 2) * ratio(8) == ratio(4));

// ratio negation
static_assert(-ratio(3, 8) == ratio(-3, 8));

// ratio addition
static_assert(ratio(1, 2) + ratio(1, 3) == ratio(5, 6));
static_assert(ratio(1, 3, 2) + ratio(11, 6) == ratio(211, 6));  // 100/3 + 11/6

// multiply with exponents
static_assert(ratio(1, 8, 2) * ratio(2, 1, 4) == ratio(1, 4, 6));
static_assert(ratio(1, 2, -4) * ratio(8, 1, 3) == ratio(4, 1, -1));

static_assert(ratio(4) / ratio(2) == ratio(2));
static_assert(ratio(2) / ratio(8) == ratio(1, 4));
static_assert(ratio(1, 8) / ratio(2) == ratio(1, 16));
static_assert(ratio(6) / ratio(3) == ratio(2));

// divide with exponents
static_assert(ratio(1, 8, -6) / ratio(2, 1, -8) == ratio(1, 16, 2));
static_assert(ratio(6, 1, 4) / ratio(3) == ratio(2, 1, 4));

static_assert(pow<0>(ratio(2)) == ratio(1));
static_assert(pow<1>(ratio(2)) == ratio(2));
static_assert(pow<2>(ratio(2)) == ratio(4));
static_assert(pow<3>(ratio(2)) == ratio(8));
static_assert(pow<0>(ratio(1, 2)) == ratio(1));
static_assert(pow<1>(ratio(1, 2)) == ratio(1, 2));
static_assert(pow<2>(ratio(1, 2)) == ratio(1, 4));
static_assert(pow<3>(ratio(1, 2)) == ratio(1, 8));

// pow with exponents
static_assert(pow<2>(ratio(1, 2, 3)) == ratio(1, 4, 6));
static_assert(pow<4, 2>(ratio(1, 2, 3)) == ratio(1, 4, 6));
static_assert(pow<3>(ratio(1, 2, -6)) == ratio(1, 8, -18));

static_assert(sqrt(ratio(9)) == ratio(3));
static_assert(cbrt(ratio(27)) == ratio(3));
static_assert(sqrt(ratio(4)) == ratio(2));
static_assert(cbrt(ratio(8)) == ratio(2));
static_assert(sqrt(ratio(1)) == ratio(1));
static_assert(cbrt(ratio(1)) == ratio(1));
static_assert(sqrt(ratio(0)) == ratio(0));
static_assert(cbrt(ratio(0)) == ratio(0));
static_assert(sqrt(ratio(1, 4)) == ratio(1, 2));
static_assert(cbrt(ratio(1, 8)) == ratio(1, 2));

// sqrt with exponents
static_assert(sqrt(ratio(9, 1, 2)) == ratio(3, 1, 1));
static_assert(cbrt(ratio(27, 1, 3)) == ratio(3, 1, 1));
static_assert(cbrt(ratio(27, 1, 2)) == ratio(13, 1, 0));

// common_ratio
static_assert(common_ratio(ratio(1), ratio(1000)) == ratio(1));
static_assert(common_ratio(ratio(1000), ratio(1)) == ratio(1));
static_assert(common_ratio(ratio(1), ratio(1, 1000)) == ratio(1, 1000));
static_assert(common_ratio(ratio(1, 1000), ratio(1)) == ratio(1, 1000));
static_assert(common_ratio(ratio(100, 1), ratio(10, 1)) == ratio(10, 1));
static_assert(common_ratio(ratio(100, 1), ratio(1, 10)) == ratio(1, 10));

// common ratio with exponents
static_assert(common_ratio(ratio(1), ratio(1, 1, 3)) == ratio(1));
static_assert(common_ratio(ratio(10, 1, -1), ratio(1, 1, -3)) == ratio(1, 1, -3));

// numerator and denominator
static_assert(numerator(ratio(3, 4)) == 3);
static_assert(numerator(ratio(3, 7, 2)) == 300);
static_assert(denominator(ratio(3, 4)) == 4);
static_assert(denominator(ratio(3, 7, -2)) == 700);

}  // namespace
