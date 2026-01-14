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
#include <mp-units/systems/astronomy.h>
#include <mp-units/systems/si.h>

namespace {

using namespace mp_units;
using namespace mp_units::astronomy::unit_symbols;
using namespace mp_units::si::unit_symbols;
using mp_units::astronomy::unit_symbols::a;

// time units
static_assert(1. * D == 86'400. * s);
static_assert(approx_equal(1. * D_sid, (86'164'0905. / 1'000'000.) * s));
static_assert(approx_equal(1. * a, 365.25 * D));
static_assert(approx_equal(1. * a, 31'557'600. * s));
static_assert(approx_equal(1. * a_trop, 365.242189 * D));
static_assert(1. * c == 100. * a);
static_assert(1. * ka == 1000. * a);

// length units
static_assert(1. * LD == 384'399. * km);
static_assert(1. * ly == 9'460'730'472'580'800. * m);

// spectral flux density
static_assert(approx_equal(1. * Jy, 1e-26 * W / m2 / Hz));

// legacy constants
static_assert(approx_equal(1. * k, 1'720'209'895. / 100'000'000'000. * pow<3, 2>(iau::astronomical_unit) /
                                     pow<1, 2>(iau::solar_mass) / D));

}  // namespace
