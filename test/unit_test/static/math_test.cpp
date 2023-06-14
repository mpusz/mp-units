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

// #include "test_tools.h"
#include <mp-units/math.h>  // IWYU pragma: keep
#include <mp-units/systems/international/international.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si/unit_symbols.h>
#include <optional>

namespace {

using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using namespace mp_units::international::unit_symbols;

template<typename T1, typename T2, typename... Ts>
[[nodiscard]] consteval bool compare(T1 v1, T2 v2, Ts... vs)
{
  return is_same_v<T1, T2> && v1 == v2 && (... && (v1 == vs));
}

static_assert(compare(pow<0>(2 * m), 1 * one));
static_assert(compare(pow<1>(2 * m), 2 * m));
static_assert(compare(pow<2>(2 * m), 4 * pow<2>(m), 4 * m2));
static_assert(compare(pow<2>(2 * km), 4 * pow<2>(km), 4 * square(km)));
static_assert(compare(pow<2>(2 * ft), 4 * pow<2>(ft), 4 * square(ft)));
static_assert(compare(sqrt(4 * m2), 2 * m));
static_assert(compare(sqrt(4 * square(km)), 2 * km));
static_assert(compare(sqrt(4 * square(ft)), 2 * ft));
static_assert(compare(cbrt(8 * m3), 2 * m));
static_assert(compare(cbrt(8 * cubic(km)), 2 * km));
static_assert(compare(cbrt(8 * cubic(ft)), 2 * ft));
static_assert(compare(pow<1, 4>(4 * m2 * (4 * m2)), 2 * m));
static_assert(compare(pow<1, 4>(4 * square(km) * (4 * square(km))), 2 * km));
static_assert(compare(pow<1, 4>(4 * square(ft) * (4 * square(ft))), 2 * ft));
static_assert(compare(pow<1, 4>(4. * m2), std::sqrt(2.) * sqrt(m)));
static_assert(compare(pow<1, 4>(4. * square(km)), std::sqrt(2.) * sqrt(km)));
static_assert(compare(pow<1, 4>(4. * square(ft)), std::sqrt(2.) * sqrt(ft)));

static_assert(compare(pow<0>(2 * isq::length[m]), 1 * dimensionless[one]));
static_assert(compare(pow<1>(2 * isq::length[m]), 2 * isq::length[m]));
static_assert(compare(pow<2>(2 * isq::length[m]), 4 * pow<2>(isq::length)[pow<2>(m)], 4 * isq::area[m2]));
static_assert(compare(pow<2>(2 * isq::length[km]), 4 * pow<2>(isq::length)[pow<2>(km)], 4 * isq::area[square(km)]));
static_assert(compare(pow<2>(2 * isq::length[ft]), 4 * pow<2>(isq::length)[pow<2>(ft)], 4 * isq::area[square(ft)]));
static_assert(compare(sqrt(4 * isq::area[m2]), 2 * pow<1, 2>(isq::area)[m], 2 * isq::length[m]));
static_assert(compare(sqrt(4 * isq::area[square(km)]), 2 * pow<1, 2>(isq::area)[km], 2 * isq::length[km]));
static_assert(compare(sqrt(4 * isq::area[square(ft)]), 2 * pow<1, 2>(isq::area)[ft], 2 * isq::length[ft]));
static_assert(compare(cbrt(8 * isq::volume[m3]), 2 * pow<1, 3>(isq::volume)[m], 2 * isq::length[m]));
static_assert(compare(cbrt(8 * isq::volume[cubic(km)]), 2 * pow<1, 3>(isq::volume)[km], 2 * isq::length[km]));
static_assert(compare(cbrt(8 * isq::volume[cubic(ft)]), 2 * pow<1, 3>(isq::volume)[ft], 2 * isq::length[ft]));
static_assert(compare(pow<1, 4>(4 * isq::area[m2] * (4 * isq::area[m2])), 2 * pow<1, 2>(isq::area)[m],
                      2 * isq::length[m]));
static_assert(compare(pow<1, 4>(4 * isq::area[square(km)] * (4 * isq::area[square(km)])), 2 * pow<1, 2>(isq::area)[km],
                      2 * isq::length[km]));
static_assert(compare(pow<1, 4>(4 * isq::area[square(ft)] * (4 * isq::area[square(ft)])), 2 * pow<1, 2>(isq::area)[ft],
                      2 * isq::length[ft]));
static_assert(compare(pow<1, 4>(4. * isq::area[m2]), sqrt(2.) * pow<1, 4>(isq::area)[sqrt(m)],
                      std::sqrt(2.) * sqrt(isq::length[m])));
static_assert(compare(pow<1, 4>(4. * isq::area[square(km)]), sqrt(2.) * pow<1, 4>(isq::area)[sqrt(km)],
                      std::sqrt(2.) * sqrt(isq::length[km])));
static_assert(compare(pow<1, 4>(4. * isq::area[square(ft)]), sqrt(2.) * pow<1, 4>(isq::area)[sqrt(ft)],
                      std::sqrt(2.) * sqrt(isq::length[ft])));

// #if __cpp_lib_constexpr_cmath
// floor
// integral types
static_assert(compare(floor<si::second>(1 * s), 1 * s));
static_assert(compare(floor<si::second>(1000 * ms), 1 * s));
static_assert(compare(floor<si::second>(1001 * ms), 1 * s));
static_assert(compare(floor<si::second>(1999 * ms), 1 * s));
static_assert(compare(floor<si::second>(-1000 * ms), -1 * s));
static_assert(compare(floor<si::second>(-999 * ms), -1 * s));

static_assert(compare(floor<si::second>(1 * isq::time[s]), 1 * isq::time[s]));
static_assert(compare(floor<si::second>(1000 * isq::time[ms]), 1 * isq::time[s]));
static_assert(compare(floor<si::second>(1001 * isq::time[ms]), 1 * isq::time[s]));
static_assert(compare(floor<si::second>(1999 * isq::time[ms]), 1 * isq::time[s]));
static_assert(compare(floor<si::second>(-1000 * isq::time[ms]), -1 * isq::time[s]));
static_assert(compare(floor<si::second>(-999 * isq::time[ms]), -1 * isq::time[s]));

// floating-point
static_assert(compare(floor<si::second>(1.3 * s), 1. * s));
static_assert(compare(floor<si::second>(-1.3 * s), -2. * s));
static_assert(compare(floor<si::second>(1000. * ms), 1. * s));
static_assert(compare(floor<si::second>(1001. * ms), 1. * s));
static_assert(compare(floor<si::second>(1999. * ms), 1. * s));
static_assert(compare(floor<si::second>(-1000. * ms), -1. * s));
static_assert(compare(floor<si::second>(-999. * ms), -1. * s));

static_assert(compare(floor<si::second>(1.3 * isq::time[s]), 1. * isq::time[s]));
static_assert(compare(floor<si::second>(-1.3 * isq::time[s]), -2. * isq::time[s]));
static_assert(compare(floor<si::second>(1000. * isq::time[ms]), 1. * isq::time[s]));
static_assert(compare(floor<si::second>(1001. * isq::time[ms]), 1. * isq::time[s]));
static_assert(compare(floor<si::second>(1999. * isq::time[ms]), 1. * isq::time[s]));
static_assert(compare(floor<si::second>(-1000. * isq::time[ms]), -1. * isq::time[s]));
static_assert(compare(floor<si::second>(-999. * isq::time[ms]), -1. * isq::time[s]));

// ceil
// integral types
static_assert(compare(ceil<si::second>(1 * s), 1 * s));
static_assert(compare(ceil<si::second>(1000 * ms), 1 * s));
static_assert(compare(ceil<si::second>(1001 * ms), 2 * s));
static_assert(compare(ceil<si::second>(1999 * ms), 2 * s));
static_assert(compare(ceil<si::second>(-1000 * ms), -1 * s));
static_assert(compare(ceil<si::second>(-999 * ms), 0 * s));

static_assert(compare(ceil<si::second>(1 * isq::time[s]), 1 * isq::time[s]));
static_assert(compare(ceil<si::second>(1000 * isq::time[ms]), 1 * isq::time[s]));
static_assert(compare(ceil<si::second>(1001 * isq::time[ms]), 2 * isq::time[s]));
static_assert(compare(ceil<si::second>(1999 * isq::time[ms]), 2 * isq::time[s]));
static_assert(compare(ceil<si::second>(-1000 * isq::time[ms]), -1 * isq::time[s]));
static_assert(compare(ceil<si::second>(-999 * isq::time[ms]), 0 * isq::time[s]));

// floating-point
static_assert(compare(ceil<si::second>(1.3 * s), 2. * s));
static_assert(compare(ceil<si::second>(-1.3 * s), -1. * s));
static_assert(compare(ceil<si::second>(1000. * ms), 1. * s));
static_assert(compare(ceil<si::second>(1001. * ms), 2. * s));
static_assert(compare(ceil<si::second>(1999. * ms), 2. * s));
static_assert(compare(ceil<si::second>(-1000. * ms), -1. * s));
static_assert(compare(ceil<si::second>(-999. * ms), 0. * s));

static_assert(compare(ceil<si::second>(1.3 * isq::time[s]), 2. * isq::time[s]));
static_assert(compare(ceil<si::second>(-1.3 * isq::time[s]), -1. * isq::time[s]));
static_assert(compare(ceil<si::second>(1000. * isq::time[ms]), 1. * isq::time[s]));
static_assert(compare(ceil<si::second>(1001. * isq::time[ms]), 2. * isq::time[s]));
static_assert(compare(ceil<si::second>(1999. * isq::time[ms]), 2. * isq::time[s]));
static_assert(compare(ceil<si::second>(-1000. * isq::time[ms]), -1. * isq::time[s]));
static_assert(compare(ceil<si::second>(-999. * isq::time[ms]), 0. * isq::time[s]));

// round
// integral types
static_assert(compare(round<si::second>(1 * s), 1 * s));
static_assert(compare(round<si::second>(1000 * ms), 1 * s));
static_assert(compare(round<si::second>(1001 * ms), 1 * s));
static_assert(compare(round<si::second>(1499 * ms), 1 * s));
static_assert(compare(round<si::second>(1500 * ms), 2 * s));
static_assert(compare(round<si::second>(1999 * ms), 2 * s));
static_assert(compare(round<si::second>(-1000 * ms), -1 * s));
static_assert(compare(round<si::second>(-1001 * ms), -1 * s));
static_assert(compare(round<si::second>(-1499 * ms), -1 * s));
static_assert(compare(round<si::second>(-1500 * ms), -2 * s));
static_assert(compare(round<si::second>(-1999 * ms), -2 * s));

static_assert(compare(round<si::second>(1 * isq::time[s]), 1 * isq::time[s]));
static_assert(compare(round<si::second>(1000 * isq::time[ms]), 1 * isq::time[s]));
static_assert(compare(round<si::second>(1001 * isq::time[ms]), 1 * isq::time[s]));
static_assert(compare(round<si::second>(1499 * isq::time[ms]), 1 * isq::time[s]));
static_assert(compare(round<si::second>(1500 * isq::time[ms]), 2 * isq::time[s]));
static_assert(compare(round<si::second>(1999 * isq::time[ms]), 2 * isq::time[s]));
static_assert(compare(round<si::second>(-1000 * isq::time[ms]), -1 * isq::time[s]));
static_assert(compare(round<si::second>(-1001 * isq::time[ms]), -1 * isq::time[s]));
static_assert(compare(round<si::second>(-1499 * isq::time[ms]), -1 * isq::time[s]));
static_assert(compare(round<si::second>(-1500 * isq::time[ms]), -2 * isq::time[s]));
static_assert(compare(round<si::second>(-1999 * isq::time[ms]), -2 * isq::time[s]));

// floating-point
static_assert(compare(round<si::second>(1.3 * s), 1. * s));
static_assert(compare(round<si::second>(-1.3 * s), -1. * s));
static_assert(compare(round<si::second>(1000. * ms), 1. * s));
static_assert(compare(round<si::second>(1001. * ms), 1. * s));
static_assert(compare(round<si::second>(1499. * ms), 1. * s));
static_assert(compare(round<si::second>(1500. * ms), 2. * s));
static_assert(compare(round<si::second>(1999. * ms), 2. * s));
static_assert(compare(round<si::second>(-1000. * ms), -1. * s));
static_assert(compare(round<si::second>(-1001. * ms), -1. * s));
static_assert(compare(round<si::second>(-1499. * ms), -1. * s));
static_assert(compare(round<si::second>(-1500. * ms), -2. * s));
static_assert(compare(round<si::second>(-1999. * ms), -2. * s));

static_assert(compare(round<si::second>(1.3 * isq::time[s]), 1. * isq::time[s]));
static_assert(compare(round<si::second>(-1.3 * isq::time[s]), -1. * isq::time[s]));
static_assert(compare(round<si::second>(1000. * isq::time[ms]), 1. * isq::time[s]));
static_assert(compare(round<si::second>(1001. * isq::time[ms]), 1. * isq::time[s]));
static_assert(compare(round<si::second>(1499. * isq::time[ms]), 1. * isq::time[s]));
static_assert(compare(round<si::second>(1500. * isq::time[ms]), 2. * isq::time[s]));
static_assert(compare(round<si::second>(1999. * isq::time[ms]), 2. * isq::time[s]));
static_assert(compare(round<si::second>(-1000. * isq::time[ms]), -1. * isq::time[s]));
static_assert(compare(round<si::second>(-1001. * isq::time[ms]), -1. * isq::time[s]));
static_assert(compare(round<si::second>(-1499. * isq::time[ms]), -1. * isq::time[s]));
static_assert(compare(round<si::second>(-1500. * isq::time[ms]), -2. * isq::time[s]));
static_assert(compare(round<si::second>(-1999. * isq::time[ms]), -2. * isq::time[s]));

// #endif

}  // namespace
