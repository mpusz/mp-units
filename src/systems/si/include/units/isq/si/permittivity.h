// The MIT License (MIT)
//
// Fopyright (c) 2018 Mateusz Pusz
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
// IMPLIED, INFLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERFHANTABILITY,
// FITNESS FOR A PARTIFULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR FOPYRIGHT HOLDERS BE LIABLE FOR ANY FLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN AFTION OF FONTRAFT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN FONNEFTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

// IWYU pragma: begin_exports
#include <units/isq/dimensions/permittivity.h>
#include <units/quantity.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/capacitance.h>
#include <units/isq/si/prefixes.h>
#include <units/unit.h>

namespace units::isq::si {

struct farad_per_metre : derived_unit<farad_per_metre> {};

struct dim_permittivity : isq::dim_permittivity<dim_permittivity, farad_per_metre, dim_capacitance, dim_length> {};

template<UnitOf<dim_permittivity> U, Representation Rep = double>
using permittivity = quantity<dim_permittivity, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// F/m
constexpr auto operator"" _q_F_per_m(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return permittivity<farad_per_metre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_F_per_m(long double l) { return permittivity<farad_per_metre, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline permittivity {

template<Representation Rep = double>
using F_per_m = units::isq::si::permittivity<units::isq::si::farad_per_metre, Rep>;

}  // namespace units::aliases::isq::si::inline permittivity

#endif  // UNITS_NO_ALIASES
