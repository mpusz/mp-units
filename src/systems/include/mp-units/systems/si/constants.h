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

#pragma once

#include <mp-units/bits/module_macros.h>
#include <mp-units/systems/si/units.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework/unit.h>
#endif

MP_UNITS_EXPORT
namespace mp_units::si {

inline namespace si2019 {

// clang-format off
inline constexpr struct hyperfine_structure_transition_frequency_of_cs final :
  named_constant<symbol_text{u8"Δν_Cs", "dv_Cs"}, mag<9'192'631'770> * hertz> {} hyperfine_structure_transition_frequency_of_cs;
inline constexpr struct speed_of_light_in_vacuum final :
  named_constant<"c", mag<299'792'458> * metre / second> {} speed_of_light_in_vacuum;
inline constexpr struct planck_constant final :
  named_constant<"h", mag_ratio<662'607'015, 100'000'000> * mag_power<10, -34> * joule * second> {} planck_constant;
inline constexpr struct elementary_charge final :
  named_constant<"e", mag_ratio<1'602'176'634, 1'000'000'000> * mag_power<10, -19> * coulomb> {} elementary_charge;
inline constexpr struct boltzmann_constant final :
  named_constant<"k", mag_ratio<1'380'649, 1'000'000> * mag_power<10, -23> * joule / kelvin> {} boltzmann_constant;
inline constexpr struct avogadro_constant final :
  named_constant<"N_A", mag_ratio<602'214'076, 100'000'000> * mag_power<10, 23> / mole> {} avogadro_constant;
inline constexpr struct luminous_efficacy final :
  named_constant<"K_cd", mag<683> * lumen / watt> {} luminous_efficacy;
// clang-format on

}  // namespace si2019

// The standard gravity is not an SI defining constant: it is a conventional value adopted by
// CGPM 3 (1901), which is exactly what NIST's "adopted values" CODATA category holds. Its home
// is `codata::standard_gravity` (a single adjustment-invariant entity from
// <mp-units/systems/codata/adopted_values.h>); this duplicate stays only for compatibility.
[[deprecated("2.6.0: Use `codata::standard_gravity` from <mp-units/systems/codata/adopted_values.h>")]]
// clang-format off
inline constexpr struct standard_gravity final :
  named_constant<symbol_text{u8"g₀", "g_0"}, mag_ratio<980'665, 100'000> * metre / square(second)> {} standard_gravity;
// clang-format on

// Like the standard gravity above, ℏ is not an SI defining constant; it belongs to the `codata`
// system, which defines it as the exact `h / 2π` relation in the post-2019 adjustments and as
// the measured "Planck constant over 2 pi" in `codata2014`.
[[deprecated(
  "2.6.0: Use `codata::reduced_planck_constant` from <mp-units/systems/codata.h> or one of its "
  "per-adjustment headers")]]
// clang-format off
inline constexpr struct reduced_planck_constant final :
  named_constant<symbol_text{u8"ℏ", "hbar"}, si2019::planck_constant / (mag<2> * π)> {} reduced_planck_constant;
// clang-format on

// The value below is exact only under the pre-2019 SI, which defined the ampere through this
// constant. The 2019 redefinition tied the ampere to the elementary charge instead, making this a
// measured quantity that belongs to a CODATA adjustment rather than to the SI.
[[deprecated(
  "2.6.0: Since the 2019 SI redefinition this constant is measured, not exact. Use "
  "`codata::magnetic_constant` from <mp-units/systems/codata.h> for the current value, "
  "or `codata::codata2014::magnetic_constant` for the exact pre-2019 one")]]
// clang-format off
inline constexpr struct magnetic_constant final :
  named_constant<symbol_text{u8"μ₀", "u_0"}, mag<4> * mag_power<10, -7> * π * henry / metre> {} magnetic_constant;
// clang-format on

}  // namespace mp_units::si
