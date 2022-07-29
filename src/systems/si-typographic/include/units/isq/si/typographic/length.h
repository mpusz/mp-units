
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

#include <units/isq/si/length.h>
#include <units/unit.h>

namespace units::isq::si::typographic {

// TODO Conflicts with (https://en.wikipedia.org/wiki/Pica_(typography)), verify correctness of below conversion factors
// and provide hyperlinks to definitions
struct pica_comp : named_scaled_unit<pica_comp, "pica(comp)", mag<4'233'333>() * mag_power<10, -9>(), si::metre> {};
struct pica_prn :
    named_scaled_unit<pica_prn, "pica(prn)", mag<ratio(2108759, 500000)>() * mag_power<10, -3>(), si::metre> {};
struct point_comp :
    named_scaled_unit<point_comp, "point(comp)", mag<ratio(1763889, 500000)>() * mag_power<10, -4>(), si::metre> {};
struct point_prn :
    named_scaled_unit<point_prn, "point(prn)", mag<ratio(1757299, 500000)>() * mag_power<10, -4>(), si::metre> {};

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// pica comp
constexpr auto operator"" _q_pica_comp(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return si::length<pica_comp, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_pica_comp(long double l) { return si::length<pica_comp, long double>(l); }

// pica prn
constexpr auto operator"" _q_pica_prn(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return si::length<pica_prn, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_pica_prn(long double l) { return si::length<pica_prn, long double>(l); }

// point comp
constexpr auto operator"" _q_point_comp(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return si::length<point_comp, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_point_comp(long double l) { return si::length<point_comp, long double>(l); }

// point prn
constexpr auto operator"" _q_point_prn(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return si::length<point_prn, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_point_prn(long double l) { return si::length<point_prn, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace length_references {

inline constexpr auto pica_comp = reference<si::dim_length, typographic::pica_comp>{};
inline constexpr auto pica_prn = reference<si::dim_length, typographic::pica_prn>{};
inline constexpr auto point_comp = reference<si::dim_length, typographic::point_comp>{};
inline constexpr auto point_prn = reference<si::dim_length, typographic::point_prn>{};

}  // namespace length_references

namespace references {

using namespace length_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si::typographic

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::typographic::inline length {

template<Representation Rep = double>
using pica_comp = units::isq::si::length<units::isq::si::typographic::pica_comp, Rep>;
template<Representation Rep = double>
using pica_prn = units::isq::si::length<units::isq::si::typographic::pica_prn, Rep>;
template<Representation Rep = double>
using point_comp = units::isq::si::length<units::isq::si::typographic::point_comp, Rep>;
template<Representation Rep = double>
using point_prn = units::isq::si::length<units::isq::si::typographic::point_prn, Rep>;

}  // namespace units::aliases::isq::si::typographic::inline length

#endif  // UNITS_NO_ALIASES
