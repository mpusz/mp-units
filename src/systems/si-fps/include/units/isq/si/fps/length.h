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

namespace units::isq::si::fps {

// https://en.wikipedia.org/wiki/Foot_(unit)
using si::international::fathom;
using si::international::foot;
using si::international::inch;
using si::international::mil;
using si::international::mile;
using si::international::thou;
using si::international::yard;

// thousandth of an inch
struct thousandth : alias_unit<thou, "thou"> {};

struct kiloyard : prefixed_unit<kiloyard, si::kilo, yard> {};

struct nautical_mile : named_scaled_unit<nautical_mile, "nmi", mag<2'000>(), yard> {};

struct dim_length : isq::dim_length<foot> {};

template<UnitOf<dim_length> U, Representation Rep = double>
using length = quantity<dim_length, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// Thousandth
constexpr auto operator"" _q_thou(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<thousandth, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_thou(long double l) { return length<thousandth, long double>(l); }
constexpr auto operator"" _q_mil(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<thousandth, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_mil(long double l) { return length<thousandth, long double>(l); }


// Inch
constexpr auto operator"" _q_in(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<inch, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_in(long double l) { return length<inch, long double>(l); }

// Foot
constexpr auto operator"" _q_ft(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<foot, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ft(long double l) { return length<foot, long double>(l); }

// Yard
constexpr auto operator"" _q_yd(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<yard, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_yd(long double l) { return length<yard, long double>(l); }

// Fathom
constexpr auto operator"" _q_ftm(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<fathom, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ftm(long double l) { return length<fathom, long double>(l); }

// Kiloyard
constexpr auto operator"" _q_kyd(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<kiloyard, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_kyd(long double l) { return length<kiloyard, long double>(l); }

// Mile
constexpr auto operator"" _q_mile(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<mile, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_mile(long double l) { return length<mile, long double>(l); }

// Nautical mile
constexpr auto operator"" _q_naut_mi(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<nautical_mile, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_naut_mi(long double l) { return length<nautical_mile, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace length_references {

inline constexpr auto thou = reference<dim_length, thousandth>{};
inline constexpr auto mil = thou;

inline constexpr auto in = reference<dim_length, inch>{};
inline constexpr auto ft = reference<dim_length, foot>{};
inline constexpr auto yd = reference<dim_length, yard>{};
inline constexpr auto ftm = reference<dim_length, fathom>{};
inline constexpr auto kyd = reference<dim_length, kiloyard>{};
inline constexpr auto mile = reference<dim_length, fps::mile>{};
inline constexpr auto naut_mi = reference<dim_length, nautical_mile>{};

}  // namespace length_references

namespace references {

using namespace length_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si::fps

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::fps::inline length {

template<Representation Rep = double>
using thou = units::isq::si::fps::length<units::isq::si::fps::thousandth, Rep>;
template<Representation Rep = double>
using mil = thou<Rep>;

template<Representation Rep = double>
using in = units::isq::si::fps::length<units::isq::si::fps::inch, Rep>;
template<Representation Rep = double>
using ft = units::isq::si::fps::length<units::isq::si::fps::foot, Rep>;
template<Representation Rep = double>
using yd = units::isq::si::fps::length<units::isq::si::fps::yard, Rep>;
template<Representation Rep = double>
using ftm = units::isq::si::fps::length<units::isq::si::fps::fathom, Rep>;
template<Representation Rep = double>
using kyd = units::isq::si::fps::length<units::isq::si::fps::kiloyard, Rep>;
template<Representation Rep = double>
using mile = units::isq::si::fps::length<units::isq::si::fps::mile, Rep>;
template<Representation Rep = double>
using naut_mi = units::isq::si::fps::length<units::isq::si::fps::nautical_mile, Rep>;

}  // namespace units::aliases::isq::si::fps::inline length

#endif  // UNITS_NO_ALIASES
