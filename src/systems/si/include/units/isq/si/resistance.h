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
#include <units/isq/dimensions/resistance.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/electric_current.h>
#include <units/isq/si/prefixes.h>
#include <units/isq/si/voltage.h>
#include <units/unit.h>

namespace units::isq::si {

struct ohm : named_unit<ohm, basic_symbol_text{"Ω", "ohm"}> {};
struct yoctoohm : prefixed_unit<yoctoohm, yocto, ohm> {};
struct zeptoohm : prefixed_unit<zeptoohm, zepto, ohm> {};
struct attoohm : prefixed_unit<attoohm, atto, ohm> {};
struct femtoohm : prefixed_unit<femtoohm, femto, ohm> {};
struct picoohm : prefixed_unit<picoohm, pico, ohm> {};
struct nanoohm : prefixed_unit<nanoohm, nano, ohm> {};
struct microohm : prefixed_unit<microohm, micro, ohm> {};
struct milliohm : prefixed_unit<milliohm, milli, ohm> {};
struct kiloohm : prefixed_unit<kiloohm, kilo, ohm> {};
struct megaohm : prefixed_unit<megaohm, mega, ohm> {};
struct gigaohm : prefixed_unit<gigaohm, giga, ohm> {};
struct teraohm : prefixed_unit<teraohm, tera, ohm> {};
struct petaohm : prefixed_unit<petaohm, peta, ohm> {};
struct exaohm : prefixed_unit<exaohm, exa, ohm> {};
struct zettaohm : prefixed_unit<zettaohm, zetta, ohm> {};
struct yottaohm : prefixed_unit<yottaohm, yotta, ohm> {};

struct dim_resistance : isq::dim_resistance<dim_resistance, ohm, dim_voltage, dim_electric_current> {};

template<UnitOf<dim_resistance> U, Representation Rep = double>
using resistance = quantity<dim_resistance, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// R
constexpr auto operator"" _q_R(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return resistance<ohm, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_R(long double l) { return resistance<ohm, long double>(l); }

// yR
constexpr auto operator"" _q_yR(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return resistance<yoctoohm, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_yR(long double l) { return resistance<yoctoohm, long double>(l); }

// zR
constexpr auto operator"" _q_zR(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return resistance<zeptoohm, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_zR(long double l) { return resistance<zeptoohm, long double>(l); }

// aR
constexpr auto operator"" _q_aR(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return resistance<attoohm, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_aR(long double l) { return resistance<attoohm, long double>(l); }

// fR
constexpr auto operator"" _q_fR(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return resistance<femtoohm, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_fR(long double l) { return resistance<femtoohm, long double>(l); }

// pR
constexpr auto operator"" _q_pR(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return resistance<picoohm, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_pR(long double l) { return resistance<picoohm, long double>(l); }

// nR
constexpr auto operator"" _q_nR(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return resistance<nanoohm, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_nR(long double l) { return resistance<nanoohm, long double>(l); }

// uR
constexpr auto operator"" _q_uR(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return resistance<microohm, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_uR(long double l) { return resistance<microohm, long double>(l); }

// mR
constexpr auto operator"" _q_mR(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return resistance<milliohm, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_mR(long double l) { return resistance<milliohm, long double>(l); }

// kR
constexpr auto operator"" _q_kR(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return resistance<kiloohm, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_kR(long double l) { return resistance<kiloohm, long double>(l); }

// MR
constexpr auto operator"" _q_MR(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return resistance<megaohm, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_MR(long double l) { return resistance<megaohm, long double>(l); }

// GR
constexpr auto operator"" _q_GR(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return resistance<gigaohm, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_GR(long double l) { return resistance<gigaohm, long double>(l); }

// TR
constexpr auto operator"" _q_TR(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return resistance<teraohm, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_TR(long double l) { return resistance<teraohm, long double>(l); }

// PR
constexpr auto operator"" _q_PR(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return resistance<petaohm, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_PR(long double l) { return resistance<petaohm, long double>(l); }

// ER
constexpr auto operator"" _q_ER(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return resistance<exaohm, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ER(long double l) { return resistance<exaohm, long double>(l); }

// ZR
constexpr auto operator"" _q_ZR(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return resistance<zettaohm, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ZR(long double l) { return resistance<zettaohm, long double>(l); }

// YR
constexpr auto operator"" _q_YR(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return resistance<yottaohm, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_YR(long double l) { return resistance<yottaohm, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace resistance_references {

inline constexpr auto R = reference<dim_resistance, ohm>{};
inline constexpr auto yR = reference<dim_resistance, yoctoohm>{};
inline constexpr auto zR = reference<dim_resistance, zeptoohm>{};
inline constexpr auto aR = reference<dim_resistance, attoohm>{};
inline constexpr auto fR = reference<dim_resistance, femtoohm>{};
inline constexpr auto pR = reference<dim_resistance, picoohm>{};
inline constexpr auto nR = reference<dim_resistance, nanoohm>{};
inline constexpr auto uR = reference<dim_resistance, microohm>{};
inline constexpr auto mR = reference<dim_resistance, milliohm>{};
inline constexpr auto kR = reference<dim_resistance, kiloohm>{};
inline constexpr auto MR = reference<dim_resistance, megaohm>{};
inline constexpr auto GR = reference<dim_resistance, gigaohm>{};
inline constexpr auto TR = reference<dim_resistance, teraohm>{};
inline constexpr auto PR = reference<dim_resistance, petaohm>{};
inline constexpr auto ER = reference<dim_resistance, exaohm>{};
inline constexpr auto ZR = reference<dim_resistance, zettaohm>{};
inline constexpr auto YR = reference<dim_resistance, yottaohm>{};

}  // namespace resistance_references

namespace references {

using namespace resistance_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline resistance {

template<Representation Rep = double>
using R = units::isq::si::resistance<units::isq::si::ohm, Rep>;
template<Representation Rep = double>
using yR = units::isq::si::resistance<units::isq::si::yoctoohm, Rep>;
template<Representation Rep = double>
using zR = units::isq::si::resistance<units::isq::si::zeptoohm, Rep>;
template<Representation Rep = double>
using aR = units::isq::si::resistance<units::isq::si::attoohm, Rep>;
template<Representation Rep = double>
using fR = units::isq::si::resistance<units::isq::si::femtoohm, Rep>;
template<Representation Rep = double>
using pR = units::isq::si::resistance<units::isq::si::picoohm, Rep>;
template<Representation Rep = double>
using nR = units::isq::si::resistance<units::isq::si::nanoohm, Rep>;
template<Representation Rep = double>
using uR = units::isq::si::resistance<units::isq::si::microohm, Rep>;
template<Representation Rep = double>
using mR = units::isq::si::resistance<units::isq::si::milliohm, Rep>;
template<Representation Rep = double>
using kR = units::isq::si::resistance<units::isq::si::kiloohm, Rep>;
template<Representation Rep = double>
using MR = units::isq::si::resistance<units::isq::si::megaohm, Rep>;
template<Representation Rep = double>
using GR = units::isq::si::resistance<units::isq::si::gigaohm, Rep>;
template<Representation Rep = double>
using TR = units::isq::si::resistance<units::isq::si::teraohm, Rep>;
template<Representation Rep = double>
using PR = units::isq::si::resistance<units::isq::si::petaohm, Rep>;
template<Representation Rep = double>
using ER = units::isq::si::resistance<units::isq::si::exaohm, Rep>;
template<Representation Rep = double>
using ZR = units::isq::si::resistance<units::isq::si::zettaohm, Rep>;
template<Representation Rep = double>
using YR = units::isq::si::resistance<units::isq::si::yottaohm, Rep>;

}  // namespace units::aliases::isq::si::inline resistance

#endif  // UNITS_NO_ALIASES
