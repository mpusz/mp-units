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
#include <units/isq/dimensions/force.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/acceleration.h>
#include <units/isq/si/mass.h>
#include <units/isq/si/prefixes.h>
#include <units/unit.h>

namespace units::isq::si {

struct newton : named_unit<newton, "N"> {};
struct yoctonewton : prefixed_unit<yoctonewton, yocto, newton> {};
struct zeptonewton : prefixed_unit<zeptonewton, zepto, newton> {};
struct attonewton : prefixed_unit<attonewton, atto, newton> {};
struct femtonewton : prefixed_unit<femtonewton, femto, newton> {};
struct piconewton : prefixed_unit<piconewton, pico, newton> {};
struct nanonewton : prefixed_unit<nanonewton, nano, newton> {};
struct micronewton : prefixed_unit<micronewton, micro, newton> {};
struct millinewton : prefixed_unit<millinewton, milli, newton> {};
struct centinewton : prefixed_unit<centinewton, centi, newton> {};
struct decinewton : prefixed_unit<decinewton, deci, newton> {};
struct decanewton : prefixed_unit<decanewton, deca, newton> {};
struct hectonewton : prefixed_unit<hectonewton, hecto, newton> {};
struct kilonewton : prefixed_unit<kilonewton, kilo, newton> {};
struct meganewton : prefixed_unit<meganewton, mega, newton> {};
struct giganewton : prefixed_unit<giganewton, giga, newton> {};
struct teranewton : prefixed_unit<teranewton, tera, newton> {};
struct petanewton : prefixed_unit<petanewton, peta, newton> {};
struct exanewton : prefixed_unit<exanewton, exa, newton> {};
struct zettanewton : prefixed_unit<zettanewton, zetta, newton> {};
struct yottanewton : prefixed_unit<yottanewton, yotta, newton> {};

struct dim_force : isq::dim_force<dim_force, newton, dim_mass, dim_acceleration> {};

template<UnitOf<dim_force> U, Representation Rep = double>
using force = quantity<dim_force, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// N
constexpr auto operator"" _q_N(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<newton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_N(long double l) { return force<newton, long double>(l); }

// yN
constexpr auto operator"" _q_yN(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<yoctonewton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_yN(long double l) { return force<yoctonewton, long double>(l); }

// zN
constexpr auto operator"" _q_zN(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<zeptonewton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_zN(long double l) { return force<zeptonewton, long double>(l); }

// aN
constexpr auto operator"" _q_aN(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<attonewton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_aN(long double l) { return force<attonewton, long double>(l); }

// fN
constexpr auto operator"" _q_fN(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<femtonewton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_fN(long double l) { return force<femtonewton, long double>(l); }

// pN
constexpr auto operator"" _q_pN(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<piconewton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_pN(long double l) { return force<piconewton, long double>(l); }

// nN
constexpr auto operator"" _q_nN(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<nanonewton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_nN(long double l) { return force<nanonewton, long double>(l); }

// uN
constexpr auto operator"" _q_uN(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<micronewton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_uN(long double l) { return force<micronewton, long double>(l); }

// mN
constexpr auto operator"" _q_mN(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<millinewton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_mN(long double l) { return force<millinewton, long double>(l); }

// cN
constexpr auto operator"" _q_cN(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<centinewton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_cN(long double l) { return force<centinewton, long double>(l); }

// dN
constexpr auto operator"" _q_dN(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<decinewton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_dN(long double l) { return force<decinewton, long double>(l); }

// daN
constexpr auto operator"" _q_daN(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<decanewton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_daN(long double l) { return force<decanewton, long double>(l); }

// hN
constexpr auto operator"" _q_hN(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<hectonewton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_hN(long double l) { return force<hectonewton, long double>(l); }

// kN
constexpr auto operator"" _q_kN(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<kilonewton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_kN(long double l) { return force<kilonewton, long double>(l); }

// MN
constexpr auto operator"" _q_MN(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<meganewton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_MN(long double l) { return force<meganewton, long double>(l); }

// GN
constexpr auto operator"" _q_GN(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<giganewton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_GN(long double l) { return force<giganewton, long double>(l); }

// TN
constexpr auto operator"" _q_TN(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<teranewton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_TN(long double l) { return force<teranewton, long double>(l); }

// PN
constexpr auto operator"" _q_PN(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<petanewton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_PN(long double l) { return force<petanewton, long double>(l); }

// EN
constexpr auto operator"" _q_EN(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<exanewton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_EN(long double l) { return force<exanewton, long double>(l); }

// ZN
constexpr auto operator"" _q_ZN(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<zettanewton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ZN(long double l) { return force<zettanewton, long double>(l); }

// YN
constexpr auto operator"" _q_YN(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<yottanewton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_YN(long double l) { return force<yottanewton, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace force_references {

inline constexpr auto N = reference<dim_force, newton>{};
inline constexpr auto yN = reference<dim_force, yoctonewton>{};
inline constexpr auto zN = reference<dim_force, zeptonewton>{};
inline constexpr auto aN = reference<dim_force, attonewton>{};
inline constexpr auto fN = reference<dim_force, femtonewton>{};
inline constexpr auto pN = reference<dim_force, piconewton>{};
inline constexpr auto nN = reference<dim_force, nanonewton>{};
inline constexpr auto uN = reference<dim_force, micronewton>{};
inline constexpr auto mN = reference<dim_force, millinewton>{};
inline constexpr auto cN = reference<dim_force, centinewton>{};
inline constexpr auto dN = reference<dim_force, decinewton>{};
inline constexpr auto daN = reference<dim_force, decanewton>{};
inline constexpr auto hN = reference<dim_force, hectonewton>{};
inline constexpr auto kN = reference<dim_force, kilonewton>{};
inline constexpr auto MN = reference<dim_force, meganewton>{};
inline constexpr auto GN = reference<dim_force, giganewton>{};
inline constexpr auto TN = reference<dim_force, teranewton>{};
inline constexpr auto PN = reference<dim_force, petanewton>{};
inline constexpr auto EN = reference<dim_force, exanewton>{};
inline constexpr auto ZN = reference<dim_force, zettanewton>{};
inline constexpr auto YN = reference<dim_force, yottanewton>{};

}  // namespace force_references

namespace references {

using namespace force_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline force {

template<Representation Rep = double>
using N = units::isq::si::force<units::isq::si::newton, Rep>;
template<Representation Rep = double>
using yN = units::isq::si::force<units::isq::si::yoctonewton, Rep>;
template<Representation Rep = double>
using zN = units::isq::si::force<units::isq::si::zeptonewton, Rep>;
template<Representation Rep = double>
using aN = units::isq::si::force<units::isq::si::attonewton, Rep>;
template<Representation Rep = double>
using fN = units::isq::si::force<units::isq::si::femtonewton, Rep>;
template<Representation Rep = double>
using pN = units::isq::si::force<units::isq::si::piconewton, Rep>;
template<Representation Rep = double>
using nN = units::isq::si::force<units::isq::si::nanonewton, Rep>;
template<Representation Rep = double>
using uN = units::isq::si::force<units::isq::si::micronewton, Rep>;
template<Representation Rep = double>
using mN = units::isq::si::force<units::isq::si::millinewton, Rep>;
template<Representation Rep = double>
using cN = units::isq::si::force<units::isq::si::centinewton, Rep>;
template<Representation Rep = double>
using dN = units::isq::si::force<units::isq::si::decinewton, Rep>;
template<Representation Rep = double>
using daN = units::isq::si::force<units::isq::si::decanewton, Rep>;
template<Representation Rep = double>
using hN = units::isq::si::force<units::isq::si::hectonewton, Rep>;
template<Representation Rep = double>
using kN = units::isq::si::force<units::isq::si::kilonewton, Rep>;
template<Representation Rep = double>
using MN = units::isq::si::force<units::isq::si::meganewton, Rep>;
template<Representation Rep = double>
using GN = units::isq::si::force<units::isq::si::giganewton, Rep>;
template<Representation Rep = double>
using TN = units::isq::si::force<units::isq::si::teranewton, Rep>;
template<Representation Rep = double>
using PN = units::isq::si::force<units::isq::si::petanewton, Rep>;
template<Representation Rep = double>
using EN = units::isq::si::force<units::isq::si::exanewton, Rep>;
template<Representation Rep = double>
using ZN = units::isq::si::force<units::isq::si::zettanewton, Rep>;
template<Representation Rep = double>
using YN = units::isq::si::force<units::isq::si::yottanewton, Rep>;

}  // namespace units::aliases::isq::si::inline force

#endif  // UNITS_NO_ALIASES
