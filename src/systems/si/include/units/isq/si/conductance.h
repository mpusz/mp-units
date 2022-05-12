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
#include <units/isq/dimensions/conductance.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/prefixes.h>
#include <units/isq/si/resistance.h>
#include <units/unit.h>

namespace units::isq::si {

struct siemens : named_unit<siemens, "S"> {};
struct yoctosiemens : prefixed_unit<yoctosiemens, yocto, siemens> {};
struct zeptosiemens : prefixed_unit<zeptosiemens, zepto, siemens> {};
struct attosiemens : prefixed_unit<attosiemens, atto, siemens> {};
struct femtosiemens : prefixed_unit<femtosiemens, femto, siemens> {};
struct picosiemens : prefixed_unit<picosiemens, pico, siemens> {};
struct nanosiemens : prefixed_unit<nanosiemens, nano, siemens> {};
struct microsiemens : prefixed_unit<microsiemens, micro, siemens> {};
struct millisiemens : prefixed_unit<millisiemens, milli, siemens> {};
struct kilosiemens : prefixed_unit<kilosiemens, kilo, siemens> {};
struct megasiemens : prefixed_unit<megasiemens, mega, siemens> {};
struct gigasiemens : prefixed_unit<gigasiemens, giga, siemens> {};
struct terasiemens : prefixed_unit<terasiemens, tera, siemens> {};
struct petasiemens : prefixed_unit<petasiemens, peta, siemens> {};
struct exasiemens : prefixed_unit<exasiemens, exa, siemens> {};
struct zettasiemens : prefixed_unit<zettasiemens, zetta, siemens> {};
struct yottasiemens : prefixed_unit<yottasiemens, yotta, siemens> {};

struct dim_conductance : isq::dim_conductance<dim_conductance, siemens, dim_resistance> {};

template<UnitOf<dim_conductance> U, Representation Rep = double>
using conductance = quantity<dim_conductance, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// R
constexpr auto operator"" _q_S(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return conductance<siemens, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_S(long double l) { return conductance<siemens, long double>(l); }

// yS
constexpr auto operator"" _q_yS(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return conductance<yoctosiemens, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_yS(long double l) { return conductance<yoctosiemens, long double>(l); }

// zS
constexpr auto operator"" _q_zS(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return conductance<zeptosiemens, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_zS(long double l) { return conductance<zeptosiemens, long double>(l); }

// aS
constexpr auto operator"" _q_aS(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return conductance<attosiemens, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_aS(long double l) { return conductance<attosiemens, long double>(l); }

// fS
constexpr auto operator"" _q_fS(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return conductance<femtosiemens, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_fS(long double l) { return conductance<femtosiemens, long double>(l); }

// pS
constexpr auto operator"" _q_pS(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return conductance<picosiemens, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_pS(long double l) { return conductance<picosiemens, long double>(l); }

// nS
constexpr auto operator"" _q_nS(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return conductance<nanosiemens, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_nS(long double l) { return conductance<nanosiemens, long double>(l); }

// µS
constexpr auto operator"" _q_uS(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return conductance<microsiemens, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_uS(long double l) { return conductance<microsiemens, long double>(l); }

// mS
constexpr auto operator"" _q_mS(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return conductance<millisiemens, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_mS(long double l) { return conductance<millisiemens, long double>(l); }

// kS
constexpr auto operator"" _q_kS(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return conductance<kilosiemens, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_kS(long double l) { return conductance<kilosiemens, long double>(l); }

// MS
constexpr auto operator"" _q_MS(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return conductance<megasiemens, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_MS(long double l) { return conductance<megasiemens, long double>(l); }

// GS
constexpr auto operator"" _q_GS(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return conductance<gigasiemens, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_GS(long double l) { return conductance<gigasiemens, long double>(l); }

// TS
constexpr auto operator"" _q_TS(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return conductance<terasiemens, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_TS(long double l) { return conductance<terasiemens, long double>(l); }

// PS
constexpr auto operator"" _q_PS(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return conductance<petasiemens, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_PS(long double l) { return conductance<petasiemens, long double>(l); }

// ES
constexpr auto operator"" _q_ES(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return conductance<exasiemens, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ES(long double l) { return conductance<exasiemens, long double>(l); }

// ZS
constexpr auto operator"" _q_ZS(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return conductance<zettasiemens, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ZS(long double l) { return conductance<zettasiemens, long double>(l); }

// YS
constexpr auto operator"" _q_YS(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return conductance<yottasiemens, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_YS(long double l) { return conductance<yottasiemens, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace conductance_references {

inline constexpr auto S = reference<dim_conductance, siemens>{};
inline constexpr auto yS = reference<dim_conductance, yoctosiemens>{};
inline constexpr auto zS = reference<dim_conductance, zeptosiemens>{};
inline constexpr auto aS = reference<dim_conductance, attosiemens>{};
inline constexpr auto fS = reference<dim_conductance, femtosiemens>{};
inline constexpr auto pS = reference<dim_conductance, picosiemens>{};
inline constexpr auto nS = reference<dim_conductance, nanosiemens>{};
inline constexpr auto uS = reference<dim_conductance, microsiemens>{};
inline constexpr auto mS = reference<dim_conductance, millisiemens>{};
inline constexpr auto kS = reference<dim_conductance, kilosiemens>{};
inline constexpr auto MS = reference<dim_conductance, megasiemens>{};
inline constexpr auto GS = reference<dim_conductance, gigasiemens>{};
inline constexpr auto TS = reference<dim_conductance, terasiemens>{};
inline constexpr auto PS = reference<dim_conductance, petasiemens>{};
inline constexpr auto ES = reference<dim_conductance, exasiemens>{};
inline constexpr auto ZS = reference<dim_conductance, zettasiemens>{};
inline constexpr auto YS = reference<dim_conductance, yottasiemens>{};

}  // namespace conductance_references

namespace references {

using namespace conductance_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline conductance {

template<Representation Rep = double>
using S = units::isq::si::conductance<units::isq::si::siemens, Rep>;
template<Representation Rep = double>
using yS = units::isq::si::conductance<units::isq::si::yoctosiemens, Rep>;
template<Representation Rep = double>
using zS = units::isq::si::conductance<units::isq::si::zeptosiemens, Rep>;
template<Representation Rep = double>
using aS = units::isq::si::conductance<units::isq::si::attosiemens, Rep>;
template<Representation Rep = double>
using fS = units::isq::si::conductance<units::isq::si::femtosiemens, Rep>;
template<Representation Rep = double>
using pS = units::isq::si::conductance<units::isq::si::picosiemens, Rep>;
template<Representation Rep = double>
using nS = units::isq::si::conductance<units::isq::si::nanosiemens, Rep>;
template<Representation Rep = double>
using uS = units::isq::si::conductance<units::isq::si::microsiemens, Rep>;
template<Representation Rep = double>
using mS = units::isq::si::conductance<units::isq::si::millisiemens, Rep>;
template<Representation Rep = double>
using kS = units::isq::si::conductance<units::isq::si::kilosiemens, Rep>;
template<Representation Rep = double>
using MS = units::isq::si::conductance<units::isq::si::megasiemens, Rep>;
template<Representation Rep = double>
using GS = units::isq::si::conductance<units::isq::si::gigasiemens, Rep>;
template<Representation Rep = double>
using TS = units::isq::si::conductance<units::isq::si::terasiemens, Rep>;
template<Representation Rep = double>
using PS = units::isq::si::conductance<units::isq::si::petasiemens, Rep>;
template<Representation Rep = double>
using ES = units::isq::si::conductance<units::isq::si::exasiemens, Rep>;
template<Representation Rep = double>
using ZS = units::isq::si::conductance<units::isq::si::zettasiemens, Rep>;
template<Representation Rep = double>
using YS = units::isq::si::conductance<units::isq::si::yottasiemens, Rep>;

}  // namespace units::aliases::isq::si::inline conductance

#endif  // UNITS_NO_ALIASES
