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
#include <units/isq/dimensions/thermal_conductivity.h>
#include <units/quantity.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/power.h>
#include <units/isq/si/thermodynamic_temperature.h>
#include <units/unit.h>

namespace units::isq::si {

struct watt_per_metre_kelvin : derived_unit<watt_per_metre_kelvin> {};

struct dim_thermal_conductivity :
    isq::dim_thermal_conductivity<dim_thermal_conductivity, watt_per_metre_kelvin, dim_power, dim_length,
                                  dim_thermodynamic_temperature> {};

template<UnitOf<dim_thermal_conductivity> U, Representation Rep = double>
using thermal_conductivity = quantity<dim_thermal_conductivity, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// W/(m K)
constexpr auto operator"" _q_W_per_m_K(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return thermal_conductivity<watt_per_metre_kelvin, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_W_per_m_K(long double l)
{
  return thermal_conductivity<watt_per_metre_kelvin, long double>(l);
}

}  // namespace literals

#endif  // UNITS_NO_LITERALS

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline thermal_conductivity {

template<Representation Rep = double>
using W_per_m_K = units::isq::si::thermal_conductivity<units::isq::si::watt_per_metre_kelvin, Rep>;

}  // namespace units::aliases::isq::si::inline thermal_conductivity

#endif  // UNITS_NO_ALIASES
