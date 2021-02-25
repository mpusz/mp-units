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

#include <units/one_rep.h>
#include <units/zero_rep.h>
#include <units/physical/dimensions/thermodynamic_temperature.h>
#include <units/quantity.h>
#include <units/quantity_point.h>
#include <units/origin.h>
#include <units/physical/si/prefixes.h>

namespace units::physical::si {

struct kelvin : named_unit<kelvin, "K", prefix> {};
struct millikelvin : prefixed_unit<millikelvin, milli, kelvin> {};

struct dim_thermodynamic_temperature : physical::dim_thermodynamic_temperature<kelvin> {};

template<UnitOf<dim_thermodynamic_temperature> U, QuantityValue Rep = double>
using thermodynamic_temperature = quantity<dim_thermodynamic_temperature, U, Rep>;

template<UnitOf<dim_thermodynamic_temperature> U, QuantityValue Rep = double,
         PointOrigin Orig = default_point_origin<typename dimension_unit<dim_thermodynamic_temperature>::reference>>
using thermodynamic_temperature_point = quantity_point<dim_thermodynamic_temperature, U, Rep, Orig>;

struct kelvin_temperature_origin : point_origin<kelvin_temperature_origin, kelvin> {};

template<UnitOf<dim_thermodynamic_temperature> U = kelvin, QuantityValue Rep = double>
using kelvin_temperature_point = thermodynamic_temperature_point<U, Rep, kelvin_temperature_origin>;


struct degree_celsius : alias_unit<kelvin, basic_symbol_text{"\u00b0C", "`C"}, no_prefix> {};

struct celsius_temperature_origin : point_origin<celsius_temperature_origin, kelvin> {
  using reference_origin = kelvin_temperature_origin;
  static constexpr auto offset_to_reference = thermodynamic_temperature<millikelvin, long int>(273150);
};

template<UnitOf<dim_thermodynamic_temperature> U = degree_celsius, QuantityValue Rep = double>
using celsius_temperature_point = thermodynamic_temperature_point<U, Rep, celsius_temperature_origin>;

inline namespace literals {

// K
constexpr auto operator"" _q_K(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return thermodynamic_temperature<kelvin, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_K(long double l) { return thermodynamic_temperature<kelvin, long double>(l); }
constexpr auto operator"" _qp_K(unsigned long long l) {
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return kelvin_temperature_point<kelvin, std::int64_t>(thermodynamic_temperature<kelvin, std::int64_t>(static_cast<std::int64_t>(l)));
}
constexpr auto operator"" _qp_K(long double l) {
  return kelvin_temperature_point<kelvin, long double>(thermodynamic_temperature<kelvin, long double>(l));
}
// mK
constexpr auto operator"" _q_mK(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return thermodynamic_temperature<millikelvin, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_mK(long double l) { return thermodynamic_temperature<millikelvin, long double>(l); }
constexpr auto operator"" _qp_mK(unsigned long long l) {
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return kelvin_temperature_point<millikelvin, std::int64_t>(thermodynamic_temperature<millikelvin, std::int64_t>(static_cast<std::int64_t>(l)));
}
constexpr auto operator"" _qp_mK(long double l) {
  return kelvin_temperature_point<millikelvin, long double>(thermodynamic_temperature<millikelvin, long double>(l));
}

// degree C
constexpr auto operator"" _q_deg_C(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return thermodynamic_temperature<degree_celsius, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_deg_C(long double l) { return thermodynamic_temperature<degree_celsius, long double>(l); }
constexpr auto operator"" _qp_deg_C(unsigned long long l) {
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return celsius_temperature_point<degree_celsius, std::int64_t>(thermodynamic_temperature<degree_celsius, std::int64_t>(static_cast<std::int64_t>(l)));
}
constexpr auto operator"" _qp_deg_C(long double l) {
  return celsius_temperature_point<degree_celsius, long double>(thermodynamic_temperature<degree_celsius, long double>(l));
}

}  // namespace literals

namespace unit_constants {

inline constexpr auto K = thermodynamic_temperature<kelvin, one_rep>{};
inline constexpr auto mK = thermodynamic_temperature<millikelvin, one_rep>{};

inline constexpr auto zp_kelvin = thermodynamic_temperature_point<kelvin, zero_rep, kelvin_temperature_origin>{};

inline constexpr auto deg_C = thermodynamic_temperature<degree_celsius, one_rep>{};

inline constexpr auto zp_deg_celsius = thermodynamic_temperature_point<degree_celsius, zero_rep, celsius_temperature_origin>{};

}  // namespace unit_constants

}  // namespace units::physical::si

namespace units {

/** \brief Take a quantity of in a standard conventional temperature unit and re-interpret it as a quantity point
 *
 * A \c quantity represents "distances" between "points" along a "dimension" scale. However, colloquially, temperature
 * units are double-used to also represent a "point" directly, to be interpreted as a distance to an implicit reference point.
 *
 * This function performs this tasks of identifying and adding that implicit reference point.
 *
 * WARNING: The returned value's physical meaning depends on the unit of the input quantity, which has no physical relevance on it's own.
 * That is, 1 Kelvin and 1 degree Celsius are physically equivalent quantities (as differences between thermodynamic temperatures),
 * yet the returned temperature points are physically different. This is in stark contrast to the usual behaviour of
 * this library, where unit conversions. For example, for the following holds:
 * @code{.cpp}
 *   auto f = [](auto x) { return 2 * x; }
 *   assert( quantity_cast<si::degree_celsius>(f(3 * deg_F)) == f( quantity_cast<si::degree_celsius>(3 * deg_F) ) );
 * @endcode
 * Here, that is not the case:
 * @code{.cpp}
 *   auto f = [](auto x) { return interpret_as_temperature_point(x); }
 *   assert( quantity_point_cast<si::degree_celsius>(f(3 * deg_F)) != f( quantity_cast<si::degree_celsius>(3 * deg_F) ) );
 * @endcode
 *
 * Therefore, be extra careful when using this function in generic code.
 *
 * @param t The quantity to be reinterpreted.
 * @return A quantity_point using the same representation and value interpreted as a distance to a reference temperature
 *   which is selected based on the exact unit of the provided quantity.
 */
template <QuantityValue Rep>
inline constexpr auto interpret_as_temperature_point(const units::physical::si::thermodynamic_temperature<units::physical::si::kelvin, Rep> &t) {
  return units::physical::si::kelvin_temperature_point<units::physical::si::kelvin, Rep>(t);
}
template <QuantityValue Rep>
inline constexpr auto interpret_as_temperature_point(const units::physical::si::thermodynamic_temperature<units::physical::si::millikelvin, Rep> &t) {
  return units::physical::si::kelvin_temperature_point<units::physical::si::millikelvin, Rep>(t);
}

template <QuantityValue Rep>
inline constexpr auto interpret_as_temperature_point(const units::physical::si::thermodynamic_temperature<units::physical::si::degree_celsius, Rep> &t) {
  return units::physical::si::celsius_temperature_point<units::physical::si::degree_celsius, Rep>(t);
}

}  // namespace units
