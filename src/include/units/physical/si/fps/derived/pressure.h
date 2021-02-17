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

#include <units/physical/dimensions/pressure.h>
#include <units/physical/si/fps/derived/area.h>
#include <units/physical/si/fps/derived/force.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::physical::si::fps {

struct poundal_per_foot_sq : unit<poundal_per_foot_sq> {};

struct dim_pressure : physical::dim_pressure<dim_pressure, poundal_per_foot_sq, dim_force, dim_area> {};

template<UnitOf<dim_pressure> U, QuantityValue Rep = double>
using pressure = quantity<dim_pressure, U, Rep>;

struct pound_force_per_foot_sq : named_scaled_unit<pound_force_per_foot_sq, "lbf ft2", si::prefix, ratio(32'174'049, 1'000'000), poundal_per_foot_sq> {};

struct pound_force_per_inch_sq : named_scaled_unit<pound_force_per_inch_sq, "psi", si::prefix, ratio(1, 144), pound_force_per_foot_sq> {};

struct kilopound_force_per_inch_sq : prefixed_unit<kilopound_force_per_inch_sq, si::kilo, pound_force_per_inch_sq> {};

inline namespace literals {

// Poundal per square foot
consteval auto operator"" _q_pdl_per_ft2(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return pressure<poundal_per_foot_sq, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_pdl_per_ft2(long double l) { return pressure<poundal_per_foot_sq, long double>(l); }

// Pounds per square inch
consteval auto operator"" _q_psi(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return pressure<pound_force_per_inch_sq, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_psi(long double l) { return pressure<pound_force_per_inch_sq, long double>(l); }

// kilopounds per square inch
consteval auto operator"" _q_kpsi(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return pressure<kilopound_force_per_inch_sq, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_kpsi(long double l) { return pressure<kilopound_force_per_inch_sq, long double>(l); }

}  // namespace literals

namespace unit_constants {

inline constexpr auto psi = pressure<pound_force_per_inch_sq, one_rep>{};
inline constexpr auto kpsi = pressure<kilopound_force_per_inch_sq, one_rep>{};

}  // namespace unit_constants

}  // namespace units::physical::si::fps
