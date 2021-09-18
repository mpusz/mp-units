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

// IWYU pragma: begin_exports
#include <units/isq/dimensions/thermodynamic_temperature.h>
#include <units/quantity.h>
#include <units/quantity_point.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports


namespace units::isq::si::imperial {

struct degree_fahrenheit : named_scaled_unit<degree_fahrenheit, basic_symbol_text{"\u00b0F", "`F"}, no_prefix, ratio(5, 9), kelvin> {};

namespace detail {

struct zero_fahrenheit_abs_temperature_ : scaled_unit<ratio(27315 * 9 - 5 * 3200, 900), kelvin> {};

}  // namespace detail

struct fahrenheit_temperature_origin : point_origin<dim_thermodynamic_temperature> {
  using reference_point_origin = kelvin_temperature_origin;
  static constexpr auto offset_to_reference =
      si::thermodynamic_temperature<imperial::detail::zero_fahrenheit_abs_temperature_, int>(1);
};

template<UnitOf<dim_thermodynamic_temperature> U = degree_fahrenheit, Representation Rep = double>
using fahrenheit_temperature_point = quantity_point<fahrenheit_temperature_origin, U, Rep>;

} // units::isq::si::imperial

namespace units::detail {

template <> struct customary_origin_spec_for_unit<units::isq::si::imperial::degree_fahrenheit> {
  using type = units::isq::si::imperial::fahrenheit_temperature_origin;
};

} // namespace detail

namespace units::isq::si::imperial {

#ifndef UNITS_NO_LITERALS

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

#endif // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace thermodynamic_temperature_references {

inline constexpr auto deg_F =  reference<dim_thermodynamic_temperature, degree_fahrenheit>{};

}  // namespace thermodynamic_temperature_references

namespace references {

using namespace thermodynamic_temperature_references;

}  // namespace references

#endif // UNITS_NO_REFERENCES

}  // namespace units::physical::si::imperial

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::imperial::inline thermodynamic_temperature {

template<Representation Rep = double> using deg_F = units::isq::si::thermodynamic_temperature<units::isq::si::imperial::degree_fahrenheit, Rep>;

}  // namespace units::aliases::isq::si::inline thermodynamic_temperature

#endif // UNITS_NO_ALIASES

namespace units {

template <Representation Rep>
inline constexpr auto interpret_as_temperature_point(const units::isq::si::thermodynamic_temperature<units::isq::si::imperial::degree_fahrenheit, Rep> &t) {
  return units::isq::si::imperial::fahrenheit_temperature_point<units::isq::si::imperial::degree_fahrenheit, Rep>(t);
}


}  // namespace units
