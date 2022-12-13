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
#include <units/isq/dimensions/power.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/energy.h>
#include <units/isq/si/prefixes.h>
#include <units/unit.h>

namespace units::isq::si {

struct watt : named_unit<watt, "W"> {};
struct yoctowatt : prefixed_unit<yoctowatt, yocto, watt> {};
struct zeptowatt : prefixed_unit<zeptowatt, zepto, watt> {};
struct attowatt : prefixed_unit<attowatt, atto, watt> {};
struct femtowatt : prefixed_unit<femtowatt, femto, watt> {};
struct picowatt : prefixed_unit<picowatt, pico, watt> {};
struct nanowatt : prefixed_unit<nanowatt, nano, watt> {};
struct microwatt : prefixed_unit<microwatt, micro, watt> {};
struct milliwatt : prefixed_unit<milliwatt, milli, watt> {};
struct kilowatt : prefixed_unit<kilowatt, kilo, watt> {};
struct megawatt : prefixed_unit<megawatt, mega, watt> {};
struct gigawatt : prefixed_unit<gigawatt, giga, watt> {};
struct terawatt : prefixed_unit<terawatt, tera, watt> {};
struct petawatt : prefixed_unit<petawatt, peta, watt> {};
struct exawatt : prefixed_unit<exawatt, exa, watt> {};
struct zettawatt : prefixed_unit<zettawatt, zetta, watt> {};
struct yottawatt : prefixed_unit<yottawatt, yotta, watt> {};

struct dim_power : isq::dim_power<dim_power, watt, dim_energy, dim_time> {};

template<UnitOf<dim_power> U, Representation Rep = double>
using power = quantity<dim_power, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// W
constexpr auto operator"" _q_W(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return power<watt, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_W(long double l) { return power<watt, long double>(l); }

// yW
constexpr auto operator"" _q_yW(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return power<yoctowatt, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_yW(long double l) { return power<yoctowatt, long double>(l); }

// zW
constexpr auto operator"" _q_zW(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return power<zeptowatt, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_zW(long double l) { return power<zeptowatt, long double>(l); }

// aW
constexpr auto operator"" _q_aW(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return power<attowatt, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_aW(long double l) { return power<attowatt, long double>(l); }

// fW
constexpr auto operator"" _q_fW(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return power<femtowatt, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_fW(long double l) { return power<femtowatt, long double>(l); }

// pW
constexpr auto operator"" _q_pW(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return power<picowatt, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_pW(long double l) { return power<picowatt, long double>(l); }

// nW
constexpr auto operator"" _q_nW(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return power<nanowatt, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_nW(long double l) { return power<nanowatt, long double>(l); }

// uW
constexpr auto operator"" _q_uW(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return power<microwatt, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_uW(long double l) { return power<microwatt, long double>(l); }

// mW
constexpr auto operator"" _q_mW(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return power<milliwatt, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_mW(long double l) { return power<milliwatt, long double>(l); }

// kW
constexpr auto operator"" _q_kW(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return power<kilowatt, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_kW(long double l) { return power<kilowatt, long double>(l); }

// MW
constexpr auto operator"" _q_MW(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return power<megawatt, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_MW(long double l) { return power<megawatt, long double>(l); }

// GW
constexpr auto operator"" _q_GW(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return power<gigawatt, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_GW(long double l) { return power<gigawatt, long double>(l); }

// TW
constexpr auto operator"" _q_TW(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return power<terawatt, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_TW(long double l) { return power<terawatt, long double>(l); }

// PW
constexpr auto operator"" _q_PW(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return power<petawatt, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_PW(long double l) { return power<petawatt, long double>(l); }

// EW
constexpr auto operator"" _q_EW(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return power<exawatt, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_EW(long double l) { return power<exawatt, long double>(l); }

// ZW
constexpr auto operator"" _q_ZW(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return power<zettawatt, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ZW(long double l) { return power<zettawatt, long double>(l); }

// YW
constexpr auto operator"" _q_YW(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return power<yottawatt, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_YW(long double l) { return power<yottawatt, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace power_references {

inline constexpr auto W = reference<dim_power, watt>{};
inline constexpr auto yW = reference<dim_power, yoctowatt>{};
inline constexpr auto zW = reference<dim_power, zeptowatt>{};
inline constexpr auto aW = reference<dim_power, attowatt>{};
inline constexpr auto fW = reference<dim_power, femtowatt>{};
inline constexpr auto pW = reference<dim_power, picowatt>{};
inline constexpr auto nW = reference<dim_power, nanowatt>{};
inline constexpr auto uW = reference<dim_power, microwatt>{};
inline constexpr auto mW = reference<dim_power, milliwatt>{};
inline constexpr auto kW = reference<dim_power, kilowatt>{};
inline constexpr auto MW = reference<dim_power, megawatt>{};
inline constexpr auto GW = reference<dim_power, gigawatt>{};
inline constexpr auto TW = reference<dim_power, terawatt>{};
inline constexpr auto PW = reference<dim_power, petawatt>{};
inline constexpr auto EW = reference<dim_power, exawatt>{};
inline constexpr auto ZW = reference<dim_power, zettawatt>{};
inline constexpr auto YW = reference<dim_power, yottawatt>{};

}  // namespace power_references

namespace references {

using namespace power_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline power {

template<Representation Rep = double>
using W = units::isq::si::power<units::isq::si::watt, Rep>;
template<Representation Rep = double>
using yW = units::isq::si::power<units::isq::si::yoctowatt, Rep>;
template<Representation Rep = double>
using zW = units::isq::si::power<units::isq::si::zeptowatt, Rep>;
template<Representation Rep = double>
using aW = units::isq::si::power<units::isq::si::attowatt, Rep>;
template<Representation Rep = double>
using fW = units::isq::si::power<units::isq::si::femtowatt, Rep>;
template<Representation Rep = double>
using pW = units::isq::si::power<units::isq::si::picowatt, Rep>;
template<Representation Rep = double>
using nW = units::isq::si::power<units::isq::si::nanowatt, Rep>;
template<Representation Rep = double>
using uW = units::isq::si::power<units::isq::si::microwatt, Rep>;
template<Representation Rep = double>
using mW = units::isq::si::power<units::isq::si::milliwatt, Rep>;
template<Representation Rep = double>
using kW = units::isq::si::power<units::isq::si::kilowatt, Rep>;
template<Representation Rep = double>
using MW = units::isq::si::power<units::isq::si::megawatt, Rep>;
template<Representation Rep = double>
using GW = units::isq::si::power<units::isq::si::gigawatt, Rep>;
template<Representation Rep = double>
using TW = units::isq::si::power<units::isq::si::terawatt, Rep>;
template<Representation Rep = double>
using PW = units::isq::si::power<units::isq::si::petawatt, Rep>;
template<Representation Rep = double>
using EW = units::isq::si::power<units::isq::si::exawatt, Rep>;
template<Representation Rep = double>
using ZW = units::isq::si::power<units::isq::si::zettawatt, Rep>;
template<Representation Rep = double>
using YW = units::isq::si::power<units::isq::si::yottawatt, Rep>;

}  // namespace units::aliases::isq::si::inline power

#endif  // UNITS_NO_ALIASES
