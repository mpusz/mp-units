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
#include <units/isq/dimensions/length.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/international/length.h>
#include <units/unit.h>

namespace units::isq::si::imperial {

// https://en.wikipedia.org/wiki/Chain_(unit)
struct chain : named_scaled_unit<chain, "ch", mag<22>(), si::international::yard> {};

// https://en.wikipedia.org/wiki/Rod_(unit)
struct rod : named_scaled_unit<rod, "rd", mag<ratio(1, 4)>(), chain> {};

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// ch
constexpr auto operator"" _q_ch(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return si::length<chain, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ch(long double l) { return si::length<chain, long double>(l); }

// rd
constexpr auto operator"" _q_rd(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return si::length<rod, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_rd(long double l) { return si::length<rod, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace length_references {

inline constexpr auto ch = reference<si::dim_length, chain>{};
inline constexpr auto rd = reference<si::dim_length, rod>{};

}  // namespace length_references

namespace references {

using namespace length_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si::imperial

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::imperial::inline length {

template<Representation Rep = double>
using ch = units::isq::si::length<units::isq::si::imperial::chain, Rep>;
template<Representation Rep = double>
using rd = units::isq::si::length<units::isq::si::imperial::rod, Rep>;

}  // namespace units::aliases::isq::si::imperial::inline length

#endif  // UNITS_NO_ALIASES
