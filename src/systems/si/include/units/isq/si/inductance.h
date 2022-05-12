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
#include <units/isq/dimensions/inductance.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/electric_current.h>
#include <units/isq/si/magnetic_flux.h>
#include <units/unit.h>

namespace units::isq::si {

struct henry : named_unit<henry, "H"> {};

struct yoctohenry : prefixed_unit<yoctohenry, yocto, henry> {};
struct zeptohenry : prefixed_unit<zeptohenry, zepto, henry> {};
struct attohenry : prefixed_unit<attohenry, atto, henry> {};
struct femtohenry : prefixed_unit<femtohenry, femto, henry> {};
struct picohenry : prefixed_unit<picohenry, pico, henry> {};
struct nanohenry : prefixed_unit<nanohenry, nano, henry> {};
struct microhenry : prefixed_unit<microhenry, micro, henry> {};
struct millihenry : prefixed_unit<millihenry, milli, henry> {};
struct kilohenry : prefixed_unit<kilohenry, kilo, henry> {};
struct megahenry : prefixed_unit<megahenry, mega, henry> {};
struct gigahenry : prefixed_unit<gigahenry, giga, henry> {};
struct terahenry : prefixed_unit<terahenry, tera, henry> {};
struct petahenry : prefixed_unit<petahenry, peta, henry> {};
struct exahenry : prefixed_unit<exahenry, exa, henry> {};
struct zettahenry : prefixed_unit<zettahenry, zetta, henry> {};
struct yottahenry : prefixed_unit<yottahenry, yotta, henry> {};

struct dim_inductance : isq::dim_inductance<dim_inductance, henry, dim_magnetic_flux, dim_electric_current> {};

template<UnitOf<dim_inductance> U, Representation Rep = double>
using inductance = quantity<dim_inductance, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// H
constexpr auto operator"" _q_H(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return inductance<henry, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_H(long double l) { return inductance<henry, long double>(l); }

// yH
constexpr auto operator"" _q_yH(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return inductance<yoctohenry, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_yH(long double l) { return inductance<yoctohenry, long double>(l); }

// zH
constexpr auto operator"" _q_zH(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return inductance<zeptohenry, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_zH(long double l) { return inductance<zeptohenry, long double>(l); }

// aH
constexpr auto operator"" _q_aH(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return inductance<attohenry, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_aH(long double l) { return inductance<attohenry, long double>(l); }

// fH
constexpr auto operator"" _q_fH(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return inductance<femtohenry, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_fH(long double l) { return inductance<femtohenry, long double>(l); }

// pH
constexpr auto operator"" _q_pH(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return inductance<picohenry, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_pH(long double l) { return inductance<picohenry, long double>(l); }

// nH
constexpr auto operator"" _q_nH(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return inductance<nanohenry, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_nH(long double l) { return inductance<nanohenry, long double>(l); }

// µH
constexpr auto operator"" _q_uH(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return inductance<microhenry, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_uH(long double l) { return inductance<microhenry, long double>(l); }

// mH
constexpr auto operator"" _q_mH(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return inductance<millihenry, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_mH(long double l) { return inductance<millihenry, long double>(l); }

// kH
constexpr auto operator"" _q_kH(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return inductance<kilohenry, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_kH(long double l) { return inductance<kilohenry, long double>(l); }

// MH
constexpr auto operator"" _q_MH(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return inductance<megahenry, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_MH(long double l) { return inductance<megahenry, long double>(l); }

// GH
constexpr auto operator"" _q_GH(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return inductance<gigahenry, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_GH(long double l) { return inductance<gigahenry, long double>(l); }

// TH
constexpr auto operator"" _q_TH(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return inductance<terahenry, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_TH(long double l) { return inductance<terahenry, long double>(l); }

// PH
constexpr auto operator"" _q_PH(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return inductance<petahenry, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_PH(long double l) { return inductance<petahenry, long double>(l); }

// EH
constexpr auto operator"" _q_EH(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return inductance<exahenry, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_EH(long double l) { return inductance<exahenry, long double>(l); }

// ZH
constexpr auto operator"" _q_ZH(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return inductance<zettahenry, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ZH(long double l) { return inductance<zettahenry, long double>(l); }

// YH
constexpr auto operator"" _q_YH(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return inductance<yottahenry, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_YH(long double l) { return inductance<yottahenry, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace inductance_references {

inline constexpr auto H = reference<dim_inductance, henry>{};
inline constexpr auto yH = reference<dim_inductance, yoctohenry>{};
inline constexpr auto zH = reference<dim_inductance, zeptohenry>{};
inline constexpr auto aH = reference<dim_inductance, attohenry>{};
inline constexpr auto fH = reference<dim_inductance, femtohenry>{};
inline constexpr auto pH = reference<dim_inductance, picohenry>{};
inline constexpr auto nH = reference<dim_inductance, nanohenry>{};
inline constexpr auto uH = reference<dim_inductance, microhenry>{};
inline constexpr auto mH = reference<dim_inductance, millihenry>{};
inline constexpr auto kH = reference<dim_inductance, kilohenry>{};
inline constexpr auto MH = reference<dim_inductance, megahenry>{};
inline constexpr auto GH = reference<dim_inductance, gigahenry>{};
inline constexpr auto TH = reference<dim_inductance, terahenry>{};
inline constexpr auto PH = reference<dim_inductance, petahenry>{};
inline constexpr auto EH = reference<dim_inductance, exahenry>{};
inline constexpr auto ZH = reference<dim_inductance, zettahenry>{};
inline constexpr auto YH = reference<dim_inductance, yottahenry>{};

}  // namespace inductance_references

namespace references {

using namespace inductance_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline inductance {

template<Representation Rep = double>
using H = units::isq::si::inductance<units::isq::si::henry, Rep>;
template<Representation Rep = double>
using yH = units::isq::si::inductance<units::isq::si::yoctohenry, Rep>;
template<Representation Rep = double>
using zH = units::isq::si::inductance<units::isq::si::zeptohenry, Rep>;
template<Representation Rep = double>
using aH = units::isq::si::inductance<units::isq::si::attohenry, Rep>;
template<Representation Rep = double>
using fH = units::isq::si::inductance<units::isq::si::femtohenry, Rep>;
template<Representation Rep = double>
using pH = units::isq::si::inductance<units::isq::si::picohenry, Rep>;
template<Representation Rep = double>
using nH = units::isq::si::inductance<units::isq::si::nanohenry, Rep>;
template<Representation Rep = double>
using uH = units::isq::si::inductance<units::isq::si::microhenry, Rep>;
template<Representation Rep = double>
using mH = units::isq::si::inductance<units::isq::si::millihenry, Rep>;
template<Representation Rep = double>
using kH = units::isq::si::inductance<units::isq::si::kilohenry, Rep>;
template<Representation Rep = double>
using MH = units::isq::si::inductance<units::isq::si::megahenry, Rep>;
template<Representation Rep = double>
using GH = units::isq::si::inductance<units::isq::si::gigahenry, Rep>;
template<Representation Rep = double>
using TH = units::isq::si::inductance<units::isq::si::terahenry, Rep>;
template<Representation Rep = double>
using PH = units::isq::si::inductance<units::isq::si::petahenry, Rep>;
template<Representation Rep = double>
using EH = units::isq::si::inductance<units::isq::si::exahenry, Rep>;
template<Representation Rep = double>
using ZH = units::isq::si::inductance<units::isq::si::zettahenry, Rep>;
template<Representation Rep = double>
using YH = units::isq::si::inductance<units::isq::si::yottahenry, Rep>;

}  // namespace units::aliases::isq::si::inline inductance

#endif  // UNITS_NO_ALIASES
