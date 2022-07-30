// The MIT License (MIT)
//
// Copyright (c) 2021 Mateusz Pusz
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
#include <units/isq/dimensions/momentum.h>
#include <units/quantity.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/mass.h>
#include <units/isq/si/speed.h>
#include <units/unit.h>

// Necessary to factor `5'344'285'992'678`, which appears in the value for eV/c.
template<>
inline constexpr std::optional<std::intmax_t> units::known_first_factor<296'904'777'371> = 157'667;

namespace units::isq::si::hep {

struct kilogram_metre_per_second : derived_unit<kilogram_metre_per_second> {};

struct eV_per_c :
    named_scaled_unit<eV_per_c, "eV/c", mag<ratio(5'344'285'992'678, 1'000'000'000'000)>() * mag_power<10, -35>(),
                      kilogram_metre_per_second> {};
struct feV_per_c : prefixed_unit<feV_per_c, femto, eV_per_c> {};
struct peV_per_c : prefixed_unit<peV_per_c, pico, eV_per_c> {};
struct neV_per_c : prefixed_unit<neV_per_c, nano, eV_per_c> {};
struct ueV_per_c : prefixed_unit<ueV_per_c, micro, eV_per_c> {};
struct meV_per_c : prefixed_unit<meV_per_c, milli, eV_per_c> {};
struct keV_per_c : prefixed_unit<keV_per_c, kilo, eV_per_c> {};
struct MeV_per_c : prefixed_unit<MeV_per_c, mega, eV_per_c> {};
struct GeV_per_c : prefixed_unit<GeV_per_c, giga, eV_per_c> {};
struct TeV_per_c : prefixed_unit<TeV_per_c, tera, eV_per_c> {};
struct PeV_per_c : prefixed_unit<PeV_per_c, peta, eV_per_c> {};
struct EeV_per_c : prefixed_unit<EeV_per_c, exa, eV_per_c> {};
struct YeV_per_c : prefixed_unit<YeV_per_c, yotta, eV_per_c> {};

struct dim_momentum : isq::dim_momentum<dim_momentum, eV_per_c, dim_mass, dim_speed> {};

template<UnitOf<dim_momentum> U, Representation Rep = double>
using momentum = quantity<dim_momentum, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

constexpr auto operator"" _q_feV_per_c(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return momentum<feV_per_c, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_feV_per_c(long double l) { return momentum<feV_per_c, long double>(l); }
constexpr auto operator"" _q_peV_per_c(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return momentum<peV_per_c, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_peV_per_c(long double l) { return momentum<peV_per_c, long double>(l); }
constexpr auto operator"" _q_neV_per_c(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return momentum<neV_per_c, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_neV_per_c(long double l) { return momentum<neV_per_c, long double>(l); }
constexpr auto operator"" _q_ueV_per_c(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return momentum<ueV_per_c, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ueV_per_c(long double l) { return momentum<ueV_per_c, long double>(l); }
constexpr auto operator"" _q_meV_per_c(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return momentum<meV_per_c, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_meV_per_c(long double l) { return momentum<meV_per_c, long double>(l); }

// eV_per_c
constexpr auto operator"" _q_eV_per_c(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return momentum<eV_per_c, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_eV_per_c(long double l) { return momentum<eV_per_c, long double>(l); }

constexpr auto operator"" _q_keV_per_c(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return momentum<keV_per_c, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_keV_per_c(long double l) { return momentum<keV_per_c, long double>(l); }
constexpr auto operator"" _q_MeV_per_c(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return momentum<MeV_per_c, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_MeV_per_c(long double l) { return momentum<MeV_per_c, long double>(l); }
constexpr auto operator"" _q_GeV_per_c(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return momentum<GeV_per_c, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_GeV_per_c(long double l) { return momentum<GeV_per_c, long double>(l); }
constexpr auto operator"" _q_TeV_per_c(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return momentum<TeV_per_c, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_TeV_per_c(long double l) { return momentum<TeV_per_c, long double>(l); }
constexpr auto operator"" _q_PeV_per_c(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return momentum<PeV_per_c, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_PeV_per_c(long double l) { return momentum<PeV_per_c, long double>(l); }
constexpr auto operator"" _q_EeV_per_c(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return momentum<EeV_per_c, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_EeV_per_c(long double l) { return momentum<EeV_per_c, long double>(l); }
constexpr auto operator"" _q_YeV_per_c(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return momentum<YeV_per_c, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_YeV_per_c(long double l) { return momentum<YeV_per_c, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

}  // namespace units::isq::si::hep

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline momentum {

template<Representation Rep = double>
using eV_per_c = units::isq::si::hep::momentum<units::isq::si::hep::eV_per_c, Rep>;

}  // namespace units::aliases::isq::si::inline momentum

#endif  // UNITS_NO_ALIASES
