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

#if MP_UNITS_HOSTED
#include <mp-units/math.h>  // IWYU pragma: keep
#endif
#include "test_tools.h"
#include <mp-units/systems/iau.h>
#include <mp-units/systems/si.h>

/* ************** IAU 2015 Resolution B3 - Nominal Solar and Planetary Conversion Constants **************** */

namespace {

using namespace mp_units;
using namespace mp_units::iau::unit_symbols;
using namespace mp_units::si::unit_symbols;

// IAU 2012 Resolution B1 - astronomical unit
static_assert(1. * au == 149'597'870'700. * m);

#if MP_UNITS_HOSTED && (__cpp_lib_constexpr_cmath || MP_UNITS_COMP_GCC)
// Derived unit: parsec
static_assert(round<si::metre>(1.L * pc) == 30'856'775'814'913'673. * m);
#endif

// IAU 2015 Resolution B3 - Nominal solar conversion constants
static_assert(approx_equal(1. * R_SUN_N, 6.957e8 * m));
static_assert(1. * S_SUN_N == 1361. * W / m2);
static_assert(approx_equal(1. * L_SUN_N, 3.828e26 * W));
static_assert(point<T_EFF_SUN_N>(1.) == point<K>(5772.));
static_assert(approx_equal(1. * GM_SUN_N, 1.3271244e20 * m3 / s2));

// IAU 2015 Resolution B3 - Nominal planetary conversion constants
static_assert(approx_equal(1. * R_EARTH_E_N, 6.3781e6 * m));
static_assert(approx_equal(1. * R_EARTH_P_N, 6.3568e6 * m));
static_assert(approx_equal(1. * R_JUP_E_N, 7.1492e7 * m));
static_assert(approx_equal(1. * R_JUP_P_N, 6.6854e7 * m));
static_assert(approx_equal(1. * GM_EARTH_N, 3.986004e14 * m3 / s2));
static_assert(approx_equal(1. * GM_JUP_N, 1.2668653e17 * m3 / s2));

// CODATA 2018 - Newtonian constant of gravitation
static_assert(approx_equal(1. * G, 6.6743e-11 * m3 / kg / s2));

// CODATA 2018 - Derived SI masses (using (GM)ᴺ / G)
static_assert(1. * M_SUN == 1. * iau::nominal_solar_mass_parameter / (1. * iau::newtonian_constant_of_gravitation));
static_assert(1. * M_EARTH ==
              1. * iau::nominal_terrestrial_mass_parameter / (1. * iau::newtonian_constant_of_gravitation));
static_assert(1. * M_JUP == 1. * iau::nominal_jovian_mass_parameter / (1. * iau::newtonian_constant_of_gravitation));

}  // namespace
