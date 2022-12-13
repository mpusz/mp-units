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
#include <units/isq/dimensions/electric_current.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/prefixes.h>
#include <units/unit.h>

namespace units::isq::si {

struct ampere : named_unit<ampere, "A"> {};
struct yoctoampere : prefixed_unit<yoctoampere, yocto, ampere> {};
struct zeptoampere : prefixed_unit<zeptoampere, zepto, ampere> {};
struct attoampere : prefixed_unit<attoampere, atto, ampere> {};
struct femtoampere : prefixed_unit<femtoampere, femto, ampere> {};
struct picoampere : prefixed_unit<picoampere, pico, ampere> {};
struct nanoampere : prefixed_unit<nanoampere, nano, ampere> {};
struct microampere : prefixed_unit<microampere, micro, ampere> {};
struct milliampere : prefixed_unit<milliampere, milli, ampere> {};
struct centiampere : prefixed_unit<centiampere, centi, ampere> {};
struct deciampere : prefixed_unit<deciampere, deci, ampere> {};
struct decaampere : prefixed_unit<decaampere, deca, ampere> {};
struct hectoampere : prefixed_unit<hectoampere, hecto, ampere> {};
struct kiloampere : prefixed_unit<kiloampere, kilo, ampere> {};
struct megaampere : prefixed_unit<megaampere, mega, ampere> {};
struct gigaampere : prefixed_unit<gigaampere, giga, ampere> {};
struct teraampere : prefixed_unit<teraampere, tera, ampere> {};
struct petaampere : prefixed_unit<petaampere, peta, ampere> {};
struct exaampere : prefixed_unit<exaampere, exa, ampere> {};
struct zettaampere : prefixed_unit<zettaampere, zetta, ampere> {};
struct yottaampere : prefixed_unit<yottaampere, yotta, ampere> {};

struct dim_electric_current : isq::dim_electric_current<ampere> {};

template<UnitOf<dim_electric_current> U, Representation Rep = double>
using electric_current = quantity<dim_electric_current, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// A
constexpr auto operator"" _q_A(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return electric_current<ampere, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_A(long double l) { return electric_current<ampere, long double>(l); }

// yA
constexpr auto operator"" _q_yA(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return electric_current<yoctoampere, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_yA(long double l) { return electric_current<yoctoampere, long double>(l); }

// zA
constexpr auto operator"" _q_zA(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return electric_current<zeptoampere, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_zA(long double l) { return electric_current<zeptoampere, long double>(l); }

// aA
constexpr auto operator"" _q_aA(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return electric_current<attoampere, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_aA(long double l) { return electric_current<attoampere, long double>(l); }

// fA
constexpr auto operator"" _q_fA(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return electric_current<femtoampere, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_fA(long double l) { return electric_current<femtoampere, long double>(l); }

// pA
constexpr auto operator"" _q_pA(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return electric_current<picoampere, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_pA(long double l) { return electric_current<picoampere, long double>(l); }

// nA
constexpr auto operator"" _q_nA(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return electric_current<nanoampere, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_nA(long double l) { return electric_current<nanoampere, long double>(l); }

// uA
constexpr auto operator"" _q_uA(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return electric_current<microampere, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_uA(long double l) { return electric_current<microampere, long double>(l); }

// mA
constexpr auto operator"" _q_mA(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return electric_current<milliampere, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_mA(long double l) { return electric_current<milliampere, long double>(l); }

// cA
constexpr auto operator"" _q_cA(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return electric_current<centiampere, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_cA(long double l) { return electric_current<centiampere, long double>(l); }

// dA
constexpr auto operator"" _q_dA(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return electric_current<deciampere, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_dA(long double l) { return electric_current<deciampere, long double>(l); }

// daA
constexpr auto operator"" _q_daA(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return electric_current<decaampere, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_daA(long double l) { return electric_current<decaampere, long double>(l); }

// hA
constexpr auto operator"" _q_hA(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return electric_current<hectoampere, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_hA(long double l) { return electric_current<hectoampere, long double>(l); }

// kA
constexpr auto operator"" _q_kA(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return electric_current<kiloampere, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_kA(long double l) { return electric_current<kiloampere, long double>(l); }

// MA
constexpr auto operator"" _q_MA(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return electric_current<megaampere, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_MA(long double l) { return electric_current<megaampere, long double>(l); }

// GA
constexpr auto operator"" _q_GA(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return electric_current<gigaampere, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_GA(long double l) { return electric_current<gigaampere, long double>(l); }

// TA
constexpr auto operator"" _q_TA(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return electric_current<teraampere, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_TA(long double l) { return electric_current<teraampere, long double>(l); }

// PA
constexpr auto operator"" _q_PA(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return electric_current<petaampere, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_PA(long double l) { return electric_current<petaampere, long double>(l); }

// EA
constexpr auto operator"" _q_EA(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return electric_current<exaampere, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_EA(long double l) { return electric_current<exaampere, long double>(l); }

// ZA
constexpr auto operator"" _q_ZA(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return electric_current<zettaampere, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ZA(long double l) { return electric_current<zettaampere, long double>(l); }

// YA
constexpr auto operator"" _q_YA(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return electric_current<yottaampere, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_YA(long double l) { return electric_current<yottaampere, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace electric_current_references {

inline constexpr auto A = reference<dim_electric_current, ampere>{};
inline constexpr auto yA = reference<dim_electric_current, yoctoampere>{};
inline constexpr auto zA = reference<dim_electric_current, zeptoampere>{};
inline constexpr auto aA = reference<dim_electric_current, attoampere>{};
inline constexpr auto fA = reference<dim_electric_current, femtoampere>{};
inline constexpr auto pA = reference<dim_electric_current, picoampere>{};
inline constexpr auto nA = reference<dim_electric_current, nanoampere>{};
inline constexpr auto uA = reference<dim_electric_current, microampere>{};
inline constexpr auto mA = reference<dim_electric_current, milliampere>{};
inline constexpr auto cA = reference<dim_electric_current, centiampere>{};
inline constexpr auto dA = reference<dim_electric_current, deciampere>{};
inline constexpr auto daA = reference<dim_electric_current, decaampere>{};
inline constexpr auto hA = reference<dim_electric_current, hectoampere>{};
inline constexpr auto kA = reference<dim_electric_current, kiloampere>{};
inline constexpr auto MA = reference<dim_electric_current, megaampere>{};
inline constexpr auto GA = reference<dim_electric_current, gigaampere>{};
inline constexpr auto TA = reference<dim_electric_current, teraampere>{};
inline constexpr auto PA = reference<dim_electric_current, petaampere>{};
inline constexpr auto EA = reference<dim_electric_current, exaampere>{};
inline constexpr auto ZA = reference<dim_electric_current, zettaampere>{};
inline constexpr auto YA = reference<dim_electric_current, yottaampere>{};

}  // namespace electric_current_references

namespace references {

using namespace electric_current_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline electric_current {

template<Representation Rep = double>
using A = units::isq::si::electric_current<units::isq::si::ampere, Rep>;
template<Representation Rep = double>
using yA = units::isq::si::electric_current<units::isq::si::yoctoampere, Rep>;
template<Representation Rep = double>
using zA = units::isq::si::electric_current<units::isq::si::zeptoampere, Rep>;
template<Representation Rep = double>
using aA = units::isq::si::electric_current<units::isq::si::attoampere, Rep>;
template<Representation Rep = double>
using fA = units::isq::si::electric_current<units::isq::si::femtoampere, Rep>;
template<Representation Rep = double>
using pA = units::isq::si::electric_current<units::isq::si::picoampere, Rep>;
template<Representation Rep = double>
using nA = units::isq::si::electric_current<units::isq::si::nanoampere, Rep>;
template<Representation Rep = double>
using uA = units::isq::si::electric_current<units::isq::si::microampere, Rep>;
template<Representation Rep = double>
using mA = units::isq::si::electric_current<units::isq::si::milliampere, Rep>;
template<Representation Rep = double>
using cA = units::isq::si::electric_current<units::isq::si::centiampere, Rep>;
template<Representation Rep = double>
using dA = units::isq::si::electric_current<units::isq::si::deciampere, Rep>;
template<Representation Rep = double>
using daA = units::isq::si::electric_current<units::isq::si::decaampere, Rep>;
template<Representation Rep = double>
using hA = units::isq::si::electric_current<units::isq::si::hectoampere, Rep>;
template<Representation Rep = double>
using kA = units::isq::si::electric_current<units::isq::si::kiloampere, Rep>;
template<Representation Rep = double>
using MA = units::isq::si::electric_current<units::isq::si::megaampere, Rep>;
template<Representation Rep = double>
using GA = units::isq::si::electric_current<units::isq::si::gigaampere, Rep>;
template<Representation Rep = double>
using TA = units::isq::si::electric_current<units::isq::si::teraampere, Rep>;
template<Representation Rep = double>
using PA = units::isq::si::electric_current<units::isq::si::petaampere, Rep>;
template<Representation Rep = double>
using EA = units::isq::si::electric_current<units::isq::si::exaampere, Rep>;
template<Representation Rep = double>
using ZA = units::isq::si::electric_current<units::isq::si::zettaampere, Rep>;
template<Representation Rep = double>
using YA = units::isq::si::electric_current<units::isq::si::yottaampere, Rep>;

}  // namespace units::aliases::isq::si::inline electric_current

#endif  // UNITS_NO_ALIASES
