// The MIT License (MIT)
//
// Copyright (c) 2020 Yves Delley
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

#include <units/physical/si/base/thermodynamic_temperature.h>

namespace units::physical::si::imperial {

struct degree_fahrenheit : named_scaled_unit<degree_fahrenheit, basic_symbol_text{"\u00b0F", "`F"}, no_prefix, ratio(5, 9), kelvin> {};

namespace detail {

struct zero_fahrenheit_abs_temperature_ : scaled_unit<ratio(27315 * 9 + 5 * 3200, 900), kelvin> {};

}  // namespace detail

struct fahrenheit_temperature_origin : point_origin<fahrenheit_temperature_origin, kelvin> {
  using reference_origin = kelvin_temperature_origin;
  static constexpr auto offset_to_reference =
      si::thermodynamic_temperature<imperial::detail::zero_fahrenheit_abs_temperature_, long int>(1);
};

template<UnitOf<si::dim_thermodynamic_temperature> U = degree_fahrenheit, QuantityValue Rep = double>
using fahrenheit_temperature_point =
  quantity_point<dim_thermodynamic_temperature, U, Rep, fahrenheit_temperature_origin>;

inline namespace literals {

// degree F
constexpr auto operator"" _q_deg_F(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return thermodynamic_temperature<degree_fahrenheit, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_deg_F(long double l) { return thermodynamic_temperature<degree_fahrenheit, long double>(l); }
constexpr auto operator"" _qp_deg_F(unsigned long long l) {
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return fahrenheit_temperature_point<degree_fahrenheit, std::int64_t>(thermodynamic_temperature<degree_fahrenheit, std::int64_t>(static_cast<std::int64_t>(l)));
}
constexpr auto operator"" _qp_deg_F(long double l) {
  return fahrenheit_temperature_point<degree_fahrenheit, long double>(thermodynamic_temperature<degree_fahrenheit, long double>(l));
}

}  // namespace literals

namespace unit_constants {

inline constexpr auto deg_F = thermodynamic_temperature<degree_fahrenheit, one_rep>{};

}  // namespace unit_constants

}  // namespace units::physical::si::imperial
