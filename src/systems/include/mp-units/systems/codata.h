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

// The CODATA recommended values of the fundamental physical constants, expressed in SI units,
// with one namespace per adjustment.
//
// This is a system of its own rather than a part of `si`. The SI fixes exactly seven constants
// (see `si2019` in `si/constants.h`); everything CODATA publishes beyond those is *measured*, so
// its value carries a relative standard uncertainty and shifts with each adjustment. Keeping them
// out of `si` means `mp-units/systems/si.h`, which is what most translation units reach for, does
// not pay for constants that most of them never name. Include this header when you want them.
//
// The most recent adjustment is `inline`, so `codata::magnetic_constant` is the current value and
// `codata::codata2018::magnetic_constant` pins a specific one. Moving that `inline` to a newer
// adjustment is a deliberate, documented change of the default.
//
// NOTE: these are *not* interchangeable with the constants of the same name in `mp_units::hep`.
// HEP declares its own system of quantities (base dimensions L, T, Q, E, ...; energy is a base
// quantity and there is no mass dimension), so its constants are unrelated types. The published
// uncertainties differ too: before the 2019 redefinition the elementary charge was itself
// measured, so a mass expressed in kg and the same mass expressed in MeV/c² are separate CODATA
// table entries with different `u_r`.

#include <mp-units/bits/module_macros.h>
#include <mp-units/systems/si/units.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework/unit.h>
#endif

MP_UNITS_EXPORT
namespace mp_units::codata {

namespace codata2014 {

// clang-format off
// Exact by definition: until the 2019 redefinition the ampere was defined through this constant.
inline constexpr struct magnetic_constant final :
  named_constant<symbol_text{u8"μ₀", "u_0"}, mag<4> * mag_power<10, -7> * π * si::henry / si::metre> {} magnetic_constant;
// clang-format on

}  // namespace codata2014

namespace codata2018 {

// clang-format off
// 1.256 637 062 12(19) × 10⁻⁶ N/A²; measured since the ampere was redefined via the elementary charge
inline constexpr struct magnetic_constant final :
  named_constant<symbol_text{u8"μ₀", "u_0"}, mag_ratio<125'663'706'212, 100'000'000'000> * mag_power<10, -6> * si::henry / si::metre, relative_standard_uncertainty{mag_ratio<15, 10> * mag_power<10, -10>}> {} magnetic_constant;
// clang-format on

}  // namespace codata2018

inline namespace codata2022 {

// clang-format off
// 1.256 637 061 27(20) × 10⁻⁶ N/A²
inline constexpr struct magnetic_constant final :
  named_constant<symbol_text{u8"μ₀", "u_0"}, mag_ratio<125'663'706'127, 100'000'000'000> * mag_power<10, -6> * si::henry / si::metre, relative_standard_uncertainty{mag_ratio<16, 10> * mag_power<10, -10>}> {} magnetic_constant;
// clang-format on

}  // namespace codata2022

}  // namespace mp_units::codata
