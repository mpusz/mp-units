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
#include <units/isq/dimensions/surface_tension.h>
#include <units/quantity.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/force.h>
#include <units/unit.h>

namespace units::isq::si {

struct newton_per_metre : derived_unit<newton_per_metre> {};

struct dim_surface_tension : isq::dim_surface_tension<dim_surface_tension, newton_per_metre, dim_force, dim_length> {};

template<UnitOf<dim_surface_tension> U, Representation Rep = double>
using surface_tension = quantity<dim_surface_tension, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// N/m
constexpr auto operator"" _q_N_per_m(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return surface_tension<newton_per_metre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_N_per_m(long double l) { return surface_tension<newton_per_metre, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline surface_tension {

template<Representation Rep = double>
using N_per_m = units::isq::si::surface_tension<units::isq::si::newton_per_metre, Rep>;

}  // namespace units::aliases::isq::si::inline surface_tension

#endif  // UNITS_NO_ALIASES
