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

#include <units/isq/si/frequency.h>
#include <units/isq/si/prefixes.h>
#include <units/unit.h>

// IWYU pragma: begin_exports
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

namespace units::isq::iec80000 {

struct baud : alias_unit<si::hertz, "Bd"> {};
struct kilobaud : prefixed_alias_unit<si::kilohertz, si::kilo, baud> {};
struct megabaud : prefixed_alias_unit<si::megahertz, si::mega, baud> {};
struct gigabaud : prefixed_alias_unit<si::gigahertz, si::giga, baud> {};
struct terabaud : prefixed_alias_unit<si::terahertz, si::tera, baud> {};
struct petabaud : prefixed_alias_unit<si::petahertz, si::peta, baud> {};
struct exabaud : prefixed_alias_unit<si::exahertz, si::exa, baud> {};
struct zettabaud : prefixed_alias_unit<si::zettahertz, si::zetta, baud> {};
struct yottabaud : prefixed_alias_unit<si::yottahertz, si::yotta, baud> {};

// TODO alias_dimension
using dim_modulation_rate = si::dim_frequency;

template<UnitOf<dim_modulation_rate> U, Representation Rep = double>
using modulation_rate = quantity<dim_modulation_rate, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

constexpr auto operator"" _q_Bd(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return modulation_rate<baud, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_kBd(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return modulation_rate<kilobaud, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_MBd(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return modulation_rate<megabaud, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_GBd(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return modulation_rate<gigabaud, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_TBd(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return modulation_rate<terabaud, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_PBd(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return modulation_rate<petabaud, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_EBd(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return modulation_rate<exabaud, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ZBd(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return modulation_rate<zettabaud, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_YBd(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return modulation_rate<yottabaud, std::int64_t>(static_cast<std::int64_t>(l));
}

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace modulation_rate_references {

inline constexpr auto Bd = reference<dim_modulation_rate, baud>{};
inline constexpr auto kBd = reference<dim_modulation_rate, kilobaud>{};
inline constexpr auto MBd = reference<dim_modulation_rate, megabaud>{};
inline constexpr auto GBd = reference<dim_modulation_rate, gigabaud>{};
inline constexpr auto TBd = reference<dim_modulation_rate, terabaud>{};
inline constexpr auto PBd = reference<dim_modulation_rate, petabaud>{};
inline constexpr auto EBd = reference<dim_modulation_rate, exabaud>{};
inline constexpr auto ZBd = reference<dim_modulation_rate, zettabaud>{};
inline constexpr auto YBd = reference<dim_modulation_rate, yottabaud>{};

}  // namespace modulation_rate_references

namespace references {

using namespace modulation_rate_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::iec80000

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::iec80000::inline modulation_rate {

template<Representation Rep = double>
using Bd = units::isq::iec80000::modulation_rate<units::isq::iec80000::baud, Rep>;
template<Representation Rep = double>
using kBd = units::isq::iec80000::modulation_rate<units::isq::iec80000::kilobaud, Rep>;
template<Representation Rep = double>
using MBd = units::isq::iec80000::modulation_rate<units::isq::iec80000::megabaud, Rep>;
template<Representation Rep = double>
using GBd = units::isq::iec80000::modulation_rate<units::isq::iec80000::gigabaud, Rep>;
template<Representation Rep = double>
using TBd = units::isq::iec80000::modulation_rate<units::isq::iec80000::terabaud, Rep>;
template<Representation Rep = double>
using PBd = units::isq::iec80000::modulation_rate<units::isq::iec80000::petabaud, Rep>;
template<Representation Rep = double>
using EBd = units::isq::iec80000::modulation_rate<units::isq::iec80000::exabaud, Rep>;
template<Representation Rep = double>
using ZBd = units::isq::iec80000::modulation_rate<units::isq::iec80000::zettabaud, Rep>;
template<Representation Rep = double>
using YBd = units::isq::iec80000::modulation_rate<units::isq::iec80000::yottabaud, Rep>;

}  // namespace units::aliases::isq::iec80000::inline modulation_rate

#endif  // UNITS_NO_ALIASES
