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
#include <units/isq/dimensions/radioactivity.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/time.h>
#include <units/unit.h>

namespace units::isq::si {

struct becquerel : named_unit<becquerel, "Bq"> {};
struct yoctobecquerel : prefixed_unit<yoctobecquerel, yocto, becquerel> {};
struct zeptobecquerel : prefixed_unit<zeptobecquerel, zepto, becquerel> {};
struct attobecquerel : prefixed_unit<attobecquerel, atto, becquerel> {};
struct femtobecquerel : prefixed_unit<femtobecquerel, femto, becquerel> {};
struct picobecquerel : prefixed_unit<picobecquerel, pico, becquerel> {};
struct nanobecquerel : prefixed_unit<nanobecquerel, nano, becquerel> {};
struct microbecquerel : prefixed_unit<microbecquerel, micro, becquerel> {};
struct millibecquerel : prefixed_unit<millibecquerel, milli, becquerel> {};
struct centibecquerel : prefixed_unit<centibecquerel, centi, becquerel> {};
struct decibecquerel : prefixed_unit<decibecquerel, deci, becquerel> {};
struct decabecquerel : prefixed_unit<decabecquerel, deca, becquerel> {};
struct hectobecquerel : prefixed_unit<hectobecquerel, hecto, becquerel> {};
struct kilobecquerel : prefixed_unit<kilobecquerel, kilo, becquerel> {};
struct megabecquerel : prefixed_unit<megabecquerel, mega, becquerel> {};
struct gigabecquerel : prefixed_unit<gigabecquerel, giga, becquerel> {};
struct terabecquerel : prefixed_unit<terabecquerel, tera, becquerel> {};
struct petabecquerel : prefixed_unit<petabecquerel, peta, becquerel> {};
struct exabecquerel : prefixed_unit<exabecquerel, exa, becquerel> {};
struct zettabecquerel : prefixed_unit<zettabecquerel, zetta, becquerel> {};
struct yottabecquerel : prefixed_unit<yottabecquerel, yotta, becquerel> {};

struct dim_radioactivity : isq::dim_radioactivity<dim_radioactivity, becquerel, dim_time> {};

template<UnitOf<dim_radioactivity> U, Representation Rep = double>
using radioactivity = quantity<dim_radioactivity, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// Bq
constexpr auto operator"" _q_Bq(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return radioactivity<becquerel, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Bq(long double l) { return radioactivity<becquerel, long double>(l); }

// yBq
constexpr auto operator"" _q_yBq(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return radioactivity<yoctobecquerel, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_yBq(long double l) { return radioactivity<yoctobecquerel, long double>(l); }

// zBq
constexpr auto operator"" _q_zBq(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return radioactivity<zeptobecquerel, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_zBq(long double l) { return radioactivity<zeptobecquerel, long double>(l); }

// aBq
constexpr auto operator"" _q_aBq(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return radioactivity<attobecquerel, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_aBq(long double l) { return radioactivity<attobecquerel, long double>(l); }

// fBq
constexpr auto operator"" _q_fBq(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return radioactivity<femtobecquerel, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_fBq(long double l) { return radioactivity<femtobecquerel, long double>(l); }

// pBq
constexpr auto operator"" _q_pBq(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return radioactivity<picobecquerel, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_pBq(long double l) { return radioactivity<picobecquerel, long double>(l); }

// nBq
constexpr auto operator"" _q_nBq(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return radioactivity<nanobecquerel, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_nBq(long double l) { return radioactivity<nanobecquerel, long double>(l); }

// uBq
constexpr auto operator"" _q_uBq(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return radioactivity<microbecquerel, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_uBq(long double l) { return radioactivity<microbecquerel, long double>(l); }

// mBq
constexpr auto operator"" _q_mBq(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return radioactivity<millibecquerel, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_mBq(long double l) { return radioactivity<millibecquerel, long double>(l); }

// cBq
constexpr auto operator"" _q_cBq(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return radioactivity<centibecquerel, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_cBq(long double l) { return radioactivity<centibecquerel, long double>(l); }

// dBq
constexpr auto operator"" _q_dBq(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return radioactivity<decibecquerel, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_dBq(long double l) { return radioactivity<decibecquerel, long double>(l); }

// daBq
constexpr auto operator"" _q_daBq(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return radioactivity<decabecquerel, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_daBq(long double l) { return radioactivity<decabecquerel, long double>(l); }

// hBq
constexpr auto operator"" _q_hBq(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return radioactivity<hectobecquerel, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_hBq(long double l) { return radioactivity<hectobecquerel, long double>(l); }

// kBq
constexpr auto operator"" _q_kBq(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return radioactivity<kilobecquerel, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_kBq(long double l) { return radioactivity<kilobecquerel, long double>(l); }

// MBq
constexpr auto operator"" _q_MBq(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return radioactivity<megabecquerel, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_MBq(long double l) { return radioactivity<megabecquerel, long double>(l); }

// GBq
constexpr auto operator"" _q_GBq(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return radioactivity<gigabecquerel, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_GBq(long double l) { return radioactivity<gigabecquerel, long double>(l); }

// TBq
constexpr auto operator"" _q_TBq(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return radioactivity<terabecquerel, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_TBq(long double l) { return radioactivity<terabecquerel, long double>(l); }

// PBq
constexpr auto operator"" _q_PBq(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return radioactivity<petabecquerel, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_PBq(long double l) { return radioactivity<petabecquerel, long double>(l); }

// EBq
constexpr auto operator"" _q_EBq(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return radioactivity<exabecquerel, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_EBq(long double l) { return radioactivity<exabecquerel, long double>(l); }

// ZBq
constexpr auto operator"" _q_ZBq(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return radioactivity<zettabecquerel, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ZBq(long double l) { return radioactivity<zettabecquerel, long double>(l); }

// YBq
constexpr auto operator"" _q_YBq(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return radioactivity<yottabecquerel, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_YBq(long double l) { return radioactivity<yottabecquerel, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace radioactivity_references {

inline constexpr auto Bq = reference<dim_radioactivity, becquerel>{};
inline constexpr auto yBq = reference<dim_radioactivity, yoctobecquerel>{};
inline constexpr auto zBq = reference<dim_radioactivity, zeptobecquerel>{};
inline constexpr auto aBq = reference<dim_radioactivity, attobecquerel>{};
inline constexpr auto fBq = reference<dim_radioactivity, femtobecquerel>{};
inline constexpr auto pBq = reference<dim_radioactivity, picobecquerel>{};
inline constexpr auto nBq = reference<dim_radioactivity, nanobecquerel>{};
inline constexpr auto uBq = reference<dim_radioactivity, microbecquerel>{};
inline constexpr auto mBq = reference<dim_radioactivity, millibecquerel>{};
inline constexpr auto cBq = reference<dim_radioactivity, centibecquerel>{};
inline constexpr auto dBq = reference<dim_radioactivity, decibecquerel>{};
inline constexpr auto daBq = reference<dim_radioactivity, decabecquerel>{};
inline constexpr auto hBq = reference<dim_radioactivity, hectobecquerel>{};
inline constexpr auto kBq = reference<dim_radioactivity, kilobecquerel>{};
inline constexpr auto MBq = reference<dim_radioactivity, megabecquerel>{};
inline constexpr auto GBq = reference<dim_radioactivity, gigabecquerel>{};
inline constexpr auto TBq = reference<dim_radioactivity, terabecquerel>{};
inline constexpr auto PBq = reference<dim_radioactivity, petabecquerel>{};
inline constexpr auto EBq = reference<dim_radioactivity, exabecquerel>{};
inline constexpr auto ZBq = reference<dim_radioactivity, zettabecquerel>{};
inline constexpr auto YBq = reference<dim_radioactivity, yottabecquerel>{};

}  // namespace radioactivity_references

namespace references {

using namespace radioactivity_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline radioactivity {

template<Representation Rep = double>
using Bq = units::isq::si::radioactivity<units::isq::si::becquerel, Rep>;
template<Representation Rep = double>
using yBq = units::isq::si::radioactivity<units::isq::si::yoctobecquerel, Rep>;
template<Representation Rep = double>
using zBq = units::isq::si::radioactivity<units::isq::si::zeptobecquerel, Rep>;
template<Representation Rep = double>
using aBq = units::isq::si::radioactivity<units::isq::si::attobecquerel, Rep>;
template<Representation Rep = double>
using fBq = units::isq::si::radioactivity<units::isq::si::femtobecquerel, Rep>;
template<Representation Rep = double>
using pBq = units::isq::si::radioactivity<units::isq::si::picobecquerel, Rep>;
template<Representation Rep = double>
using nBq = units::isq::si::radioactivity<units::isq::si::nanobecquerel, Rep>;
template<Representation Rep = double>
using uBq = units::isq::si::radioactivity<units::isq::si::microbecquerel, Rep>;
template<Representation Rep = double>
using mBq = units::isq::si::radioactivity<units::isq::si::millibecquerel, Rep>;
template<Representation Rep = double>
using cBq = units::isq::si::radioactivity<units::isq::si::centibecquerel, Rep>;
template<Representation Rep = double>
using dBq = units::isq::si::radioactivity<units::isq::si::decibecquerel, Rep>;
template<Representation Rep = double>
using daBq = units::isq::si::radioactivity<units::isq::si::decabecquerel, Rep>;
template<Representation Rep = double>
using hBq = units::isq::si::radioactivity<units::isq::si::hectobecquerel, Rep>;
template<Representation Rep = double>
using kBq = units::isq::si::radioactivity<units::isq::si::kilobecquerel, Rep>;
template<Representation Rep = double>
using MBq = units::isq::si::radioactivity<units::isq::si::megabecquerel, Rep>;
template<Representation Rep = double>
using GBq = units::isq::si::radioactivity<units::isq::si::gigabecquerel, Rep>;
template<Representation Rep = double>
using TBq = units::isq::si::radioactivity<units::isq::si::terabecquerel, Rep>;
template<Representation Rep = double>
using PBq = units::isq::si::radioactivity<units::isq::si::petabecquerel, Rep>;
template<Representation Rep = double>
using EBq = units::isq::si::radioactivity<units::isq::si::exabecquerel, Rep>;
template<Representation Rep = double>
using ZBq = units::isq::si::radioactivity<units::isq::si::zettabecquerel, Rep>;
template<Representation Rep = double>
using YBq = units::isq::si::radioactivity<units::isq::si::yottabecquerel, Rep>;

}  // namespace units::aliases::isq::si::inline radioactivity

#endif  // UNITS_NO_ALIASES
