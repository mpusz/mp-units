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
#include <units/isq/dimensions/power.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/fps/energy.h>
#include <units/isq/si/prefixes.h>
#include <units/unit.h>

namespace units::isq::si::fps {

struct foot_poundal_per_second : derived_unit<foot_poundal_per_second> {};

struct dim_power : isq::dim_power<dim_power, foot_poundal_per_second, dim_length, dim_force, dim_time> {};

struct foot_pound_force_per_second :
    derived_scaled_unit<foot_pound_force_per_second, dim_power, foot, pound_force, second> {};

struct horse_power : named_scaled_unit<horse_power, "hp", mag<550>(), foot_pound_force_per_second> {};

template<UnitOf<dim_power> U, Representation Rep = double>
using power = quantity<dim_power, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// foot pound force per second
constexpr auto operator"" _q_ft_pdl_per_s(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return power<foot_poundal_per_second, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ft_pdl_per_s(long double l) { return power<foot_poundal_per_second, long double>(l); }

// foot pound force per second
constexpr auto operator"" _q_ft_lbf_per_s(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return power<foot_pound_force_per_second, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ft_lbf_per_s(long double l) { return power<foot_pound_force_per_second, long double>(l); }

// horse power
constexpr auto operator"" _q_hp(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return power<horse_power, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_hp(long double l) { return power<horse_power, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace power_references {

inline constexpr auto hp = reference<dim_power, horse_power>{};

}  // namespace power_references

namespace references {

using namespace power_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si::fps

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::fps::inline power {

template<Representation Rep = double>
using ft_pdl_per_s = units::isq::si::fps::power<units::isq::si::fps::foot_poundal_per_second, Rep>;
template<Representation Rep = double>
using ft_lbf_per_s = units::isq::si::fps::power<units::isq::si::fps::foot_pound_force_per_second, Rep>;
template<Representation Rep = double>
using hp = units::isq::si::fps::power<units::isq::si::fps::horse_power, Rep>;

}  // namespace units::aliases::isq::si::fps::inline power

#endif  // UNITS_NO_ALIASES
