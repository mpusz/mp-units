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
#include <units/isq/dimensions/luminous_flux.h>
#include <units/isq/si/luminous_intensity.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/generic/solid_angle.h>
#include <units/isq/si/prefixes.h>
#include <units/unit.h>

namespace units::isq::si {

struct lumen : named_unit<lumen, "lm"> {};

struct dim_luminous_flux :
    isq::dim_luminous_flux<dim_luminous_flux, lumen, dim_luminous_intensity, dim_solid_angle<>> {};

template<UnitOf<dim_luminous_flux> U, Representation Rep = double>
using luminous_flux = quantity<dim_luminous_flux, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// lm
constexpr auto operator"" _q_lm(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return luminous_flux<lumen, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_lm(long double l) { return luminous_flux<lumen, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace luminous_flux_references {

inline constexpr auto lm = reference<dim_luminous_flux, lumen>{};

}  // namespace luminous_flux_references

namespace references {

using namespace luminous_flux_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline luminous_flux {

template<Representation Rep = double>
using lm = units::isq::si::luminous_flux<units::isq::si::lumen, Rep>;

}  // namespace units::aliases::isq::si::inline luminous_flux

#endif  // UNITS_NO_ALIASES
