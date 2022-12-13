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
#include <units/isq/dimensions/magnetic_flux.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/area.h>
#include <units/isq/si/magnetic_induction.h>
#include <units/unit.h>

namespace units::isq::si {

struct weber : named_unit<weber, "Wb"> {};

struct yoctoweber : prefixed_unit<yoctoweber, yocto, weber> {};
struct zeptoweber : prefixed_unit<zeptoweber, zepto, weber> {};
struct attoweber : prefixed_unit<attoweber, atto, weber> {};
struct femtoweber : prefixed_unit<femtoweber, femto, weber> {};
struct picoweber : prefixed_unit<picoweber, pico, weber> {};
struct nanoweber : prefixed_unit<nanoweber, nano, weber> {};
struct microweber : prefixed_unit<microweber, micro, weber> {};
struct milliweber : prefixed_unit<milliweber, milli, weber> {};
struct kiloweber : prefixed_unit<kiloweber, kilo, weber> {};
struct megaweber : prefixed_unit<megaweber, mega, weber> {};
struct gigaweber : prefixed_unit<gigaweber, giga, weber> {};
struct teraweber : prefixed_unit<teraweber, tera, weber> {};
struct petaweber : prefixed_unit<petaweber, peta, weber> {};
struct exaweber : prefixed_unit<exaweber, exa, weber> {};
struct zettaweber : prefixed_unit<zettaweber, zetta, weber> {};
struct yottaweber : prefixed_unit<yottaweber, yotta, weber> {};

struct dim_magnetic_flux : isq::dim_magnetic_flux<dim_magnetic_flux, weber, dim_magnetic_induction, dim_area> {};

template<UnitOf<dim_magnetic_flux> U, Representation Rep = double>
using magnetic_flux = quantity<dim_magnetic_flux, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// Wb
constexpr auto operator"" _q_Wb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_flux<weber, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Wb(long double l) { return magnetic_flux<weber, long double>(l); }

// yWb
constexpr auto operator"" _q_yWb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_flux<yoctoweber, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_yWb(long double l) { return magnetic_flux<yoctoweber, long double>(l); }

// zWb
constexpr auto operator"" _q_zWb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_flux<zeptoweber, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_zWb(long double l) { return magnetic_flux<zeptoweber, long double>(l); }

// aWb
constexpr auto operator"" _q_aWb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_flux<attoweber, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_aWb(long double l) { return magnetic_flux<attoweber, long double>(l); }

// fWb
constexpr auto operator"" _q_fWb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_flux<femtoweber, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_fWb(long double l) { return magnetic_flux<femtoweber, long double>(l); }

// pWb
constexpr auto operator"" _q_pWb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_flux<picoweber, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_pWb(long double l) { return magnetic_flux<picoweber, long double>(l); }

// nWb
constexpr auto operator"" _q_nWb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_flux<nanoweber, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_nWb(long double l) { return magnetic_flux<nanoweber, long double>(l); }

// µWb
constexpr auto operator"" _q_uWb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_flux<microweber, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_uWb(long double l) { return magnetic_flux<microweber, long double>(l); }

// mWb
constexpr auto operator"" _q_mWb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_flux<milliweber, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_mWb(long double l) { return magnetic_flux<milliweber, long double>(l); }

// kWb
constexpr auto operator"" _q_kWb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_flux<kiloweber, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_kWb(long double l) { return magnetic_flux<kiloweber, long double>(l); }

// MWb
constexpr auto operator"" _q_MWb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_flux<megaweber, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_MWb(long double l) { return magnetic_flux<megaweber, long double>(l); }

// GWb
constexpr auto operator"" _q_GWb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_flux<gigaweber, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_GWb(long double l) { return magnetic_flux<gigaweber, long double>(l); }

// TWb
constexpr auto operator"" _q_TWb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_flux<teraweber, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_TWb(long double l) { return magnetic_flux<teraweber, long double>(l); }

// PWb
constexpr auto operator"" _q_PWb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_flux<petaweber, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_PWb(long double l) { return magnetic_flux<petaweber, long double>(l); }

// EWb
constexpr auto operator"" _q_EWb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_flux<exaweber, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_EWb(long double l) { return magnetic_flux<exaweber, long double>(l); }

// ZWb
constexpr auto operator"" _q_ZWb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_flux<zettaweber, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ZWb(long double l) { return magnetic_flux<zettaweber, long double>(l); }

// YWb
constexpr auto operator"" _q_YWb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_flux<yottaweber, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_YWb(long double l) { return magnetic_flux<yottaweber, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace magnetic_flux_references {

inline constexpr auto Wb = reference<dim_magnetic_flux, weber>{};
inline constexpr auto yWb = reference<dim_magnetic_flux, yoctoweber>{};
inline constexpr auto zWb = reference<dim_magnetic_flux, zeptoweber>{};
inline constexpr auto aWb = reference<dim_magnetic_flux, attoweber>{};
inline constexpr auto fWb = reference<dim_magnetic_flux, femtoweber>{};
inline constexpr auto pWb = reference<dim_magnetic_flux, picoweber>{};
inline constexpr auto nWb = reference<dim_magnetic_flux, nanoweber>{};
inline constexpr auto uWb = reference<dim_magnetic_flux, microweber>{};
inline constexpr auto mWb = reference<dim_magnetic_flux, milliweber>{};
inline constexpr auto kWb = reference<dim_magnetic_flux, kiloweber>{};
inline constexpr auto MWb = reference<dim_magnetic_flux, megaweber>{};
inline constexpr auto GWb = reference<dim_magnetic_flux, gigaweber>{};
inline constexpr auto TWb = reference<dim_magnetic_flux, teraweber>{};
inline constexpr auto PWb = reference<dim_magnetic_flux, petaweber>{};
inline constexpr auto EWb = reference<dim_magnetic_flux, exaweber>{};
inline constexpr auto ZWb = reference<dim_magnetic_flux, zettaweber>{};
inline constexpr auto YWb = reference<dim_magnetic_flux, yottaweber>{};

}  // namespace magnetic_flux_references

namespace references {

using namespace magnetic_flux_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline magnetic_flux {

template<Representation Rep = double>
using Wb = units::isq::si::magnetic_flux<units::isq::si::weber, Rep>;
template<Representation Rep = double>
using yWb = units::isq::si::magnetic_flux<units::isq::si::yoctoweber, Rep>;
template<Representation Rep = double>
using zWb = units::isq::si::magnetic_flux<units::isq::si::zeptoweber, Rep>;
template<Representation Rep = double>
using aWb = units::isq::si::magnetic_flux<units::isq::si::attoweber, Rep>;
template<Representation Rep = double>
using fWb = units::isq::si::magnetic_flux<units::isq::si::femtoweber, Rep>;
template<Representation Rep = double>
using pWb = units::isq::si::magnetic_flux<units::isq::si::picoweber, Rep>;
template<Representation Rep = double>
using nWb = units::isq::si::magnetic_flux<units::isq::si::nanoweber, Rep>;
template<Representation Rep = double>
using uWb = units::isq::si::magnetic_flux<units::isq::si::microweber, Rep>;
template<Representation Rep = double>
using mWb = units::isq::si::magnetic_flux<units::isq::si::milliweber, Rep>;
template<Representation Rep = double>
using kWb = units::isq::si::magnetic_flux<units::isq::si::kiloweber, Rep>;
template<Representation Rep = double>
using MWb = units::isq::si::magnetic_flux<units::isq::si::megaweber, Rep>;
template<Representation Rep = double>
using GWb = units::isq::si::magnetic_flux<units::isq::si::gigaweber, Rep>;
template<Representation Rep = double>
using TWb = units::isq::si::magnetic_flux<units::isq::si::teraweber, Rep>;
template<Representation Rep = double>
using PWb = units::isq::si::magnetic_flux<units::isq::si::petaweber, Rep>;
template<Representation Rep = double>
using EWb = units::isq::si::magnetic_flux<units::isq::si::exaweber, Rep>;
template<Representation Rep = double>
using ZWb = units::isq::si::magnetic_flux<units::isq::si::zettaweber, Rep>;
template<Representation Rep = double>
using YWb = units::isq::si::magnetic_flux<units::isq::si::yottaweber, Rep>;

}  // namespace units::aliases::isq::si::inline magnetic_flux

#endif  // UNITS_NO_ALIASES
