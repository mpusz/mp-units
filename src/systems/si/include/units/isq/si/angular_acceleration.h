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
#include <units/isq/dimensions/angular_acceleration.h>
#include <units/quantity.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/generic/angle.h>
#include <units/isq/si/prefixes.h>
#include <units/isq/si/time.h>
#include <units/unit.h>

namespace units::isq::si {

struct radian_per_second_sq : derived_unit<radian_per_second_sq> {};

struct dim_angular_acceleration :
    isq::dim_angular_acceleration<dim_angular_acceleration, radian_per_second_sq, dim_angle<>, dim_time> {};

template<UnitOf<dim_angular_acceleration> U, Representation Rep = double>
using angular_acceleration = quantity<dim_angular_acceleration, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// rad/s2
constexpr auto operator"" _q_rad_per_s2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return angular_acceleration<radian_per_second_sq, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_rad_per_s2(long double l)
{
  return angular_acceleration<radian_per_second_sq, long double>(l);
}

}  // namespace literals

#endif  // UNITS_NO_LITERALS

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline angular_acceleration {

template<Representation Rep = double>
using rad_per_s2 = units::isq::si::angular_acceleration<units::isq::si::radian_per_second_sq, Rep>;

}  // namespace units::aliases::isq::si::inline angular_acceleration

#endif  // UNITS_NO_ALIASES
