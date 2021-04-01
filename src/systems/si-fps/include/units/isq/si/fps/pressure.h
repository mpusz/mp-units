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

// IWYU pragma: begin_exports
#include <units/isq/dimensions/pressure.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/fps/area.h>
#include <units/isq/si/fps/force.h>
#include <units/isq/si/prefixes.h>
#include <units/unit.h>

namespace units::isq::si::fps {

struct poundal_per_foot_sq : unit<poundal_per_foot_sq> {};

struct dim_pressure : isq::dim_pressure<dim_pressure, poundal_per_foot_sq, dim_force, dim_area> {};

template<UnitOf<dim_pressure> U, Representation Rep = double>
using pressure = quantity<dim_pressure, U, Rep>;

struct pound_force_per_foot_sq : named_scaled_unit<pound_force_per_foot_sq, "lbf ft2", si::prefix, ratio(32'174'049, 1'000'000), poundal_per_foot_sq> {};

struct pound_force_per_inch_sq : named_scaled_unit<pound_force_per_inch_sq, "psi", si::prefix, ratio(1, 144), pound_force_per_foot_sq> {};

struct kilopound_force_per_inch_sq : prefixed_unit<kilopound_force_per_inch_sq, si::kilo, pound_force_per_inch_sq> {};

#if UNITS_UDLS

inline namespace literals {

// Poundal per square foot
constexpr auto operator"" _q_pdl_per_ft2(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return pressure<poundal_per_foot_sq, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_pdl_per_ft2(long double l) { return pressure<poundal_per_foot_sq, long double>(l); }

// Pounds per square inch
constexpr auto operator"" _q_psi(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return pressure<pound_force_per_inch_sq, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_psi(long double l) { return pressure<pound_force_per_inch_sq, long double>(l); }

// kilopounds per square inch
constexpr auto operator"" _q_kpsi(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return pressure<kilopound_force_per_inch_sq, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_kpsi(long double l) { return pressure<kilopound_force_per_inch_sq, long double>(l); }

}  // namespace literals

#endif // UNITS_UDLS

namespace references {

inline constexpr auto psi = reference<dim_pressure, pound_force_per_inch_sq>{};
inline constexpr auto kpsi = reference<dim_pressure, kilopound_force_per_inch_sq>{};

}  // namespace references

}  // namespace units::isq::si::fps
