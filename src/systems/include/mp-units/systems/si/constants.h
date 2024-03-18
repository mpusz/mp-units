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
#include <mp-units/quantity.h>
#endif

MP_UNITS_EXPORT
namespace mp_units::si {

namespace si2019 {

// clang-format off
inline constexpr struct hyperfine_structure_transition_frequency_of_cs :
  named_unit<symbol_text{u8"Δν_Cs", "dv_Cs"}, mag<9'192'631'770> * hertz> {} hyperfine_structure_transition_frequency_of_cs;
inline constexpr struct speed_of_light_in_vacuum :
  named_unit<"c", mag<299'792'458> * metre / second> {} speed_of_light_in_vacuum;
inline constexpr struct planck_constant :
  named_unit<"h", mag<ratio{662'607'015, 100'000'000}> * mag_power<10, -34> * joule * second> {} planck_constant;
inline constexpr struct elementary_charge :
  named_unit<"e", mag<ratio{1'602'176'634, 1'000'000'000}> * mag_power<10, -19> * coulomb> {} elementary_charge;
inline constexpr struct boltzmann_constant :
  named_unit<"k", mag<ratio{1'380'649, 1'000'000}> * mag_power<10, -23> * joule / kelvin> {} boltzmann_constant;
inline constexpr struct avogadro_constant :
  named_unit<"N_A", mag<ratio{602'214'076, 100'000'000}> * mag_power<10, 23> / mole> {} avogadro_constant;
inline constexpr struct luminous_efficacy :
  named_unit<"K_cd", mag<683> * lumen / watt> {} luminous_efficacy;
// clang-format on

}  // namespace si2019

// clang-format off
inline constexpr struct standard_gravity :
  named_unit<symbol_text{u8"g₀", "g_0"}, mag<ratio{980'665, 100'000}> * metre / square(second)> {} standard_gravity;
inline constexpr struct magnetic_constant :
  named_unit<symbol_text{u8"μ₀", "u_0"}, mag<4> * mag_pi * mag_power<10, -7> * henry / metre> {} magnetic_constant;
// clang-format on

}  // namespace mp_units::si
