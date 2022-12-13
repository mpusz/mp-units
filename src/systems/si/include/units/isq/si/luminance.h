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
#include <units/isq/dimensions/luminance.h>
#include <units/quantity.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/length.h>
#include <units/isq/si/luminous_intensity.h>
#include <units/unit.h>

namespace units::isq::si {

struct candela_per_metre_sq : derived_unit<candela_per_metre_sq> {};
struct dim_luminance : isq::dim_luminance<dim_luminance, candela_per_metre_sq, dim_luminous_intensity, dim_length> {};

template<UnitOf<dim_luminance> U, Representation Rep = double>
using luminance = quantity<dim_luminance, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// cd/m²
constexpr auto operator"" _q_cd_per_m2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return luminance<candela_per_metre_sq, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_cd_per_m2(long double l) { return luminance<candela_per_metre_sq, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline luminance {

template<Representation Rep = double>
using cd_per_m2 = units::isq::si::luminance<units::isq::si::candela_per_metre_sq, Rep>;

}  // namespace units::aliases::isq::si::inline luminance

#endif  // UNITS_NO_ALIASES
