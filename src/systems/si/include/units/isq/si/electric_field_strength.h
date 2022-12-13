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
#include <units/isq/dimensions/electric_field_strength.h>
#include <units/quantity.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/voltage.h>
#include <units/unit.h>

namespace units::isq::si {

struct volt_per_metre : derived_unit<volt_per_metre> {};
struct dim_electric_field_strength :
    isq::dim_electric_field_strength<dim_electric_field_strength, volt_per_metre, dim_voltage, dim_length> {};

template<UnitOf<dim_electric_field_strength> U, Representation Rep = double>
using electric_field_strength = quantity<dim_electric_field_strength, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// V/m
constexpr auto operator"" _q_V_per_m(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return electric_field_strength<volt_per_metre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_V_per_m(long double l) { return electric_field_strength<volt_per_metre, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline electric_field_strength {

template<Representation Rep = double>
using V_per_m = units::isq::si::electric_field_strength<units::isq::si::volt_per_metre, Rep>;

}  // namespace units::aliases::isq::si::inline electric_field_strength

#endif  // UNITS_NO_ALIASES
