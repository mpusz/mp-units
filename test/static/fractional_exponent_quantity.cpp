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

#include <mp-units/compat_macros.h>
#include <mp-units/framework.h>
#include <mp-units/math.h>  // IWYU pragma: keep
#include <mp-units/systems/isq/electromagnetism.h>
#include <mp-units/systems/isq/mechanics.h>
#include <mp-units/systems/isq/si_quantities.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si/prefixes.h>
#include <mp-units/systems/si/units.h>

namespace {

using namespace mp_units;

// ************************ exponent with denominator 2 (square root) ************************
// amplitude/power spectral density (e.g. nV/√Hz): dimension carries T^(1/2)

QUANTITY_SPEC(power_spectral_density, pow<2>(isq::voltage) / isq::frequency);
QUANTITY_SPEC(amplitude_spectral_density, sqrt(power_spectral_density));

static_assert(implicitly_convertible(sqrt(power_spectral_density), amplitude_spectral_density));
static_assert(implicitly_convertible(power_spectral_density, pow<2>(amplitude_spectral_density)));

static_assert(sqrt(power_spectral_density).dimension == amplitude_spectral_density.dimension);
static_assert(power_spectral_density.dimension == pow<2>(amplitude_spectral_density.dimension));

static_assert(sqrt(square(si::volt) / si::hertz) == si::volt / sqrt(si::hertz));
static_assert(square(si::volt) / si::hertz == pow<2>(si::volt / sqrt(si::hertz)));

constexpr auto sqrt_res = sqrt(power_spectral_density[square(si::volt) / si::hertz]);
static_assert(implicitly_convertible(get_quantity_spec(sqrt_res), amplitude_spectral_density));
static_assert(get_unit(sqrt_res) == si::volt / sqrt(si::hertz));

constexpr auto pow_res = pow<2>(amplitude_spectral_density[si::volt / sqrt(si::hertz)]);
static_assert(implicitly_convertible(power_spectral_density, get_quantity_spec(pow_res)));
static_assert(square(si::volt) / si::hertz == get_unit(pow_res));

#if __cpp_lib_constexpr_cmath || MP_UNITS_COMP_GCC
static_assert(sqrt(16 * power_spectral_density[square(si::volt) / si::hertz]) ==
              4 * amplitude_spectral_density[si::volt / sqrt(si::hertz)]);
static_assert(16 * power_spectral_density[square(si::volt) / si::hertz] ==
              pow<2>(4 * amplitude_spectral_density[si::volt / sqrt(si::hertz)]));
#endif

// fracture toughness K = stress * sqrt(length) (e.g. MPa·√m): dimension M L^(-1/2) T^-2
QUANTITY_SPEC(fracture_toughness, isq::pressure* sqrt(isq::length));

static_assert(fracture_toughness.dimension == isq::pressure.dimension * sqrt(isq::length.dimension));
static_assert(pow<2>(fracture_toughness).dimension == pow<2>(isq::pressure.dimension) * isq::length.dimension);
static_assert(implicitly_convertible(sqrt(pow<2>(fracture_toughness)), fracture_toughness));
static_assert(implicitly_convertible(pow<2>(fracture_toughness), pow<2>(isq::pressure) * isq::length));

static_assert(dimension_symbol(fracture_toughness.dimension) == "ML^-(1/2)T⁻²");
static_assert(pow<2>(si::mega<si::pascal> * sqrt(si::metre)) == pow<2>(si::mega<si::pascal>) * si::metre);
static_assert(unit_symbol(si::mega<si::pascal> * sqrt(si::metre)) == "MPa m^(1/2)");

// ************************ exponent with denominator 3 (cube root) ************************
// Manning's roughness coefficient n = length^(2/3) / speed (e.g. s·m^(-1/3)): dimension T L^(-1/3).
// The denominator is 3 (not 2), so this exercises code paths that must not assume square roots.

QUANTITY_SPEC(manning_roughness_coefficient, pow<2, 3>(isq::length) / isq::speed);

static_assert(manning_roughness_coefficient.dimension == pow<2, 3>(isq::length.dimension) / isq::speed.dimension);
static_assert(pow<3>(manning_roughness_coefficient).dimension ==
              pow<2>(isq::length.dimension) / pow<3>(isq::speed.dimension));
static_assert(implicitly_convertible(cbrt(pow<3>(manning_roughness_coefficient)), manning_roughness_coefficient));

static_assert(dimension_symbol(manning_roughness_coefficient.dimension) == "TL^-(1/3)");
static_assert(pow<3>(si::second / cbrt(si::metre)) == pow<3>(si::second) / si::metre);
static_assert(unit_symbol(si::second / cbrt(si::metre)) == "s/m^(1/3)");

}  // namespace
