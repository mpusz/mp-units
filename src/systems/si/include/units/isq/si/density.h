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
#include <units/isq/dimensions/density.h>
#include <units/quantity.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/length.h>
#include <units/isq/si/mass.h>
#include <units/isq/si/prefixes.h>
#include <units/unit.h>

namespace units::isq::si {

struct kilogram_per_metre_cub : derived_unit<kilogram_per_metre_cub> {};

struct dim_density : isq::dim_density<dim_density, kilogram_per_metre_cub, dim_mass, dim_length> {};

template<UnitOf<dim_density> U, Representation Rep = double>
using density = quantity<dim_density, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// kg / m³
constexpr auto operator"" _q_kg_per_m3(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return density<kilogram_per_metre_cub, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_kg_per_m3(long double l) { return density<kilogram_per_metre_cub, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline density {

template<Representation Rep = double>
using kg_per_m3 = units::isq::si::density<units::isq::si::kilogram_per_metre_cub, Rep>;

}  // namespace units::aliases::isq::si::inline density

#endif  // UNITS_NO_ALIASES
