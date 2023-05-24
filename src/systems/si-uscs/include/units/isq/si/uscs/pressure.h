// The MIT License (MIT)
//
// Copyright (c) 2018 Mateusz Pusz
// Copyright (c) 2023 Tobias Lorenz
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
#include <units/isq/dimensions/pressure.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/pressure.h>
#include <units/unit.h>

namespace units::isq::si::uscs {

// https://en.wikipedia.org/wiki/Inch_of_mercury
// https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8
struct inch_of_mercury : named_scaled_unit<inch_of_mercury, "inHg", mag<ratio(3'386'389, 1'000)>(), si::pascal> {};

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// inHg
constexpr auto operator"" _q_inHg(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return si::pressure<units::isq::si::uscs::inch_of_mercury, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_inHg(long double l)
{
  return si::pressure<units::isq::si::uscs::inch_of_mercury, long double>(l);
}

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace pressure_references {

inline constexpr auto inHg = reference<si::dim_pressure, uscs::inch_of_mercury>{};

}  // namespace pressure_references

namespace references {

using namespace pressure_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si::uscs

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::uscs::inline pressure {

template<Representation Rep = double>
using inHg = units::isq::si::pressure<units::isq::si::uscs::inch_of_mercury, Rep>;

}  // namespace units::aliases::isq::si::uscs::inline pressure

#endif  // UNITS_NO_ALIASES
