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
#include <units/isq/dimensions/acceleration.h>
#include <units/quantity.h>
#include <units/symbol_text.h>
#include <units/unit.h>
// IWYU pragma: end_exports

#include <units/isq/si/speed.h>

namespace units::isq::si {

struct metre_per_second_sq : derived_unit<metre_per_second_sq> {};
struct dim_acceleration : isq::dim_acceleration<dim_acceleration, metre_per_second_sq, dim_length, dim_time> {};

template<UnitOf<dim_acceleration> U, Representation Rep = double>
using acceleration = quantity<dim_acceleration, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// m/s2
constexpr auto operator"" _q_m_per_s2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return acceleration<metre_per_second_sq, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_m_per_s2(long double l) { return acceleration<metre_per_second_sq, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline acceleration {

template<Representation Rep = double>
using m_per_s2 = units::isq::si::acceleration<units::isq::si::metre_per_second_sq, Rep>;

}  // namespace units::aliases::isq::si::inline acceleration

#endif  // UNITS_NO_ALIASES
