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
#include <units/generic/angle.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/prefixes.h>

namespace units {

struct steradian : named_unit<steradian, "sr"> {};

template<Unit U = steradian>
struct dim_solid_angle : derived_dimension<dim_solid_angle<U>, U, exponent<dim_angle<>, 2>> {};

struct square_degree : derived_scaled_unit<square_degree, dim_solid_angle<>, degree> {};

template<typename T>
concept SolidAngle = QuantityOfT<T, dim_solid_angle>;

template<UnitOf<dim_solid_angle<>> U, Representation Rep = double>
using solid_angle = quantity<dim_solid_angle<>, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// sr
constexpr auto operator"" _q_sr(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return solid_angle<steradian, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_sr(long double l) { return solid_angle<steradian, long double>(l); }

// deg2
constexpr auto operator"" _q_deg2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return solid_angle<square_degree, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_deg2(long double l) { return solid_angle<square_degree, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace solid_angle_references {

inline constexpr auto sr = reference<dim_solid_angle<>, steradian>{};
inline constexpr auto deg2 = reference<dim_solid_angle<>, square_degree>{};

}  // namespace solid_angle_references

namespace references {

using namespace solid_angle_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units

#ifndef UNITS_NO_ALIASES

namespace units::aliases::inline solid_angle {

template<Representation Rep = double>
using sr = units::solid_angle<units::steradian, Rep>;

template<Representation Rep = double>
using deg2 = units::solid_angle<units::square_degree, Rep>;

}  // namespace units::aliases::inline solid_angle

#endif  // UNITS_NO_ALIASES
