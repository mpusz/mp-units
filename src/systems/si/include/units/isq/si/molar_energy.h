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
#include <units/isq/dimensions/molar_energy.h>
#include <units/quantity.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/amount_of_substance.h>
#include <units/isq/si/energy.h>
#include <units/isq/si/prefixes.h>
#include <units/unit.h>

namespace units::isq::si {

struct joule_per_mole : derived_unit<joule_per_mole> {};

struct dim_molar_energy :
    isq::dim_molar_energy<dim_molar_energy, joule_per_mole, dim_energy, dim_amount_of_substance> {};

template<UnitOf<dim_molar_energy> U, Representation Rep = double>
using molar_energy = quantity<dim_molar_energy, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// J/mol
constexpr auto operator"" _q_J_per_mol(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return molar_energy<joule_per_mole, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_J_per_mol(long double l) { return molar_energy<joule_per_mole, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline molar_energy {

template<Representation Rep = double>
using J_per_mol = units::isq::si::molar_energy<units::isq::si::joule_per_mole, Rep>;

}  // namespace units::aliases::isq::si::inline molar_energy

#endif  // UNITS_NO_ALIASES
