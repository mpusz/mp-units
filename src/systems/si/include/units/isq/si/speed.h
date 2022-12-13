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
#include <units/isq/dimensions/speed.h>
#include <units/quantity.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/length.h>
#include <units/isq/si/time.h>
#include <units/unit.h>

namespace units::isq::si {

struct metre_per_second : derived_unit<metre_per_second> {};
struct dim_speed : isq::dim_speed<dim_speed, metre_per_second, dim_length, dim_time> {};

struct kilometre_per_hour : derived_scaled_unit<kilometre_per_hour, dim_speed, kilometre, hour> {};

template<UnitOf<dim_speed> U, Representation Rep = double>
using speed = quantity<dim_speed, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// m/s
constexpr auto operator"" _q_m_per_s(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return speed<metre_per_second, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_m_per_s(long double l) { return speed<metre_per_second, long double>(l); }

// km/h
constexpr auto operator"" _q_km_per_h(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return speed<kilometre_per_hour, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_km_per_h(long double l) { return speed<kilometre_per_hour, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline speed {

template<Representation Rep = double>
using m_per_s = units::isq::si::speed<units::isq::si::metre_per_second, Rep>;
template<Representation Rep = double>
using km_per_h = units::isq::si::speed<units::isq::si::kilometre_per_hour, Rep>;

}  // namespace units::aliases::isq::si::inline speed

#endif  // UNITS_NO_ALIASES
