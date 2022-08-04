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
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/dimensions/time.h>
#include <units/isq/si/prefixes.h>
#include <units/unit.h>

namespace units::isq::si {

struct second : named_unit<second, "s"> {};
struct yoctosecond : prefixed_unit<yoctosecond, yocto, second> {};
struct zeptosecond : prefixed_unit<zeptosecond, zepto, second> {};
struct attosecond : prefixed_unit<attosecond, atto, second> {};
struct femtosecond : prefixed_unit<femtosecond, femto, second> {};
struct picosecond : prefixed_unit<picosecond, pico, second> {};
struct nanosecond : prefixed_unit<nanosecond, nano, second> {};
struct microsecond : prefixed_unit<microsecond, micro, second> {};
struct millisecond : prefixed_unit<millisecond, milli, second> {};
struct minute : named_scaled_unit<minute, "min", mag<60>(), second> {};
struct hour : named_scaled_unit<hour, "h", mag<60>(), minute> {};
struct day : named_scaled_unit<day, "d", mag<24>(), hour> {};

struct dim_time : isq::dim_time<second> {};

template<UnitOf<dim_time> U, Representation Rep = double>
using time = quantity<dim_time, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// ys
constexpr auto operator"" _q_ys(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return time<yoctosecond, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ys(long double l) { return time<yoctosecond, long double>(l); }

// zs
constexpr auto operator"" _q_zs(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return time<zeptosecond, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_zs(long double l) { return time<zeptosecond, long double>(l); }

// as
constexpr auto operator"" _q_as(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return time<attosecond, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_as(long double l) { return time<attosecond, long double>(l); }

// fs
constexpr auto operator"" _q_fs(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return time<femtosecond, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_fs(long double l) { return time<femtosecond, long double>(l); }

// ps
constexpr auto operator"" _q_ps(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return time<picosecond, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ps(long double l) { return time<picosecond, long double>(l); }

// ns
constexpr auto operator"" _q_ns(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return time<nanosecond, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ns(long double l) { return time<nanosecond, long double>(l); }

// us
constexpr auto operator"" _q_us(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return time<microsecond, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_us(long double l) { return time<microsecond, long double>(l); }

// ms
constexpr auto operator"" _q_ms(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return time<millisecond, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ms(long double l) { return time<millisecond, long double>(l); }

// s
constexpr auto operator"" _q_s(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return time<second, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_s(long double l) { return time<second, long double>(l); }

// min
constexpr auto operator"" _q_min(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return time<minute, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_min(long double l) { return time<minute, long double>(l); }

// h
constexpr auto operator"" _q_h(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return time<hour, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_h(long double l) { return time<hour, long double>(l); }

// d
constexpr auto operator"" _q_d(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return time<day, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_d(long double l) { return time<day, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace time_references {

inline constexpr auto ys = reference<dim_time, yoctosecond>{};
inline constexpr auto zs = reference<dim_time, zeptosecond>{};
inline constexpr auto as = reference<dim_time, attosecond>{};
inline constexpr auto fs = reference<dim_time, femtosecond>{};
inline constexpr auto ps = reference<dim_time, picosecond>{};
inline constexpr auto ns = reference<dim_time, nanosecond>{};
inline constexpr auto us = reference<dim_time, microsecond>{};
inline constexpr auto ms = reference<dim_time, millisecond>{};
inline constexpr auto s = reference<dim_time, second>{};
inline constexpr auto min = reference<dim_time, minute>{};
inline constexpr auto h = reference<dim_time, hour>{};
inline constexpr auto d = reference<dim_time, day>{};

}  // namespace time_references

namespace references {

using namespace time_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline time {

template<Representation Rep = double>
using ys = units::isq::si::time<units::isq::si::yoctosecond, Rep>;
template<Representation Rep = double>
using zs = units::isq::si::time<units::isq::si::zeptosecond, Rep>;
template<Representation Rep = double>
using as = units::isq::si::time<units::isq::si::attosecond, Rep>;
template<Representation Rep = double>
using fs = units::isq::si::time<units::isq::si::femtosecond, Rep>;
template<Representation Rep = double>
using ps = units::isq::si::time<units::isq::si::picosecond, Rep>;
template<Representation Rep = double>
using ns = units::isq::si::time<units::isq::si::nanosecond, Rep>;
template<Representation Rep = double>
using us = units::isq::si::time<units::isq::si::microsecond, Rep>;
template<Representation Rep = double>
using ms = units::isq::si::time<units::isq::si::millisecond, Rep>;
template<Representation Rep = double>
using s = units::isq::si::time<units::isq::si::second, Rep>;
template<Representation Rep = double>
using min = units::isq::si::time<units::isq::si::minute, Rep>;
template<Representation Rep = double>
using h = units::isq::si::time<units::isq::si::hour, Rep>;
template<Representation Rep = double>
using d = units::isq::si::time<units::isq::si::day, Rep>;

}  // namespace units::aliases::isq::si::inline time

#endif  // UNITS_NO_ALIASES
