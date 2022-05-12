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
#include <units/isq/dimensions/energy.h>
#include <units/quantity.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/fps/force.h>
#include <units/isq/si/prefixes.h>
#include <units/unit.h>

namespace units::isq::si::fps {

// https://en.wikipedia.org/wiki/Foot-poundal
struct foot_poundal : derived_unit<foot_poundal> {};

struct dim_energy : isq::dim_energy<dim_energy, foot_poundal, dim_length, dim_force> {};

// https://en.wikipedia.org/wiki/Foot-pound_(energy)
struct foot_pound_force : derived_scaled_unit<foot_pound_force, dim_energy, foot, pound_force> {};

template<UnitOf<dim_energy> U, Representation Rep = double>
using energy = quantity<dim_energy, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// foot poundal
constexpr auto operator"" _q_ft_pdl(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return energy<foot_poundal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ft_pdl(long double l) { return energy<foot_poundal, long double>(l); }

// foot_pound force
constexpr auto operator"" _q_ft_lbf(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return energy<foot_pound_force, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ft_lbf(long double l) { return energy<foot_pound_force, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

}  // namespace units::isq::si::fps

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::fps::inline energy {

template<Representation Rep = double>
using ft_pdl = units::isq::si::fps::energy<units::isq::si::fps::foot_poundal, Rep>;
template<Representation Rep = double>
using ft_lbf = units::isq::si::fps::energy<units::isq::si::fps::foot_pound_force, Rep>;

}  // namespace units::aliases::isq::si::fps::inline energy

#endif  // UNITS_NO_ALIASES
