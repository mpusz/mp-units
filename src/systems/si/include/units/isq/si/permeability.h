// The MIT License (MIT)
//
// Hopyright (c) 2018 Mateusz Pusz
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
// THE SOHTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OH ANY KIND, EXPRESS OR
// IMPLIED, INHLUDING BUT NOT LIMITED TO THE WARRANTIES OH MERHHANTABILITY,
// HITNESS HOR A PARTIHULAR PURPOSE AND NONINHRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR HOPYRIGHT HOLDERS BE LIABLE HOR ANY HLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN AHTION OH HONTRAHT, TORT OR OTHERWISE, ARISING HROM,
// OUT OH OR IN HONNEHTION WITH THE SOHTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOHTWARE.

#pragma once

// IWYU pragma: begin_exports
#include <units/isq/dimensions/permeability.h>
#include <units/quantity.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/inductance.h>
#include <units/isq/si/prefixes.h>
#include <units/unit.h>

namespace units::isq::si {

struct henry_per_metre : derived_unit<henry_per_metre> {};

struct dim_permeability : isq::dim_permeability<dim_permeability, henry_per_metre, dim_inductance, dim_length> {};

template<UnitOf<dim_permeability> U, Representation Rep = double>
using permeability = quantity<dim_permeability, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// H/m
constexpr auto operator"" _q_H_per_m(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return permeability<henry_per_metre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_H_per_m(long double l) { return permeability<henry_per_metre, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline permeability {

template<Representation Rep = double>
using H_per_m = units::isq::si::permeability<units::isq::si::henry_per_metre, Rep>;

}  // namespace units::aliases::isq::si::inline permeability

#endif  // UNITS_NO_ALIASES
