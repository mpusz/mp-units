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
#include <units/isq/dimensions/energy.h>
#include <units/isq/si/energy.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports
#include <units/unit.h>

namespace units::isq::si::hep {
using units::isq::si::electronvolt;

struct yeV : prefixed_unit<yeV, yocto, electronvolt> {};  // N.B. very rarely used
struct zeV : prefixed_unit<zeV, zepto, electronvolt> {};  // N.B. very rarely used
struct aeV : prefixed_unit<aeV, atto, electronvolt> {};   // N.B. very rarely used
struct feV : prefixed_unit<feV, femto, electronvolt> {};
struct peV : prefixed_unit<peV, pico, electronvolt> {};
struct neV : prefixed_unit<neV, nano, electronvolt> {};
struct ueV : prefixed_unit<ueV, micro, electronvolt> {};
struct meV : prefixed_unit<meV, milli, electronvolt> {};
using eV = electronvolt;
struct keV : prefixed_unit<keV, kilo, electronvolt> {};
struct MeV : prefixed_unit<MeV, mega, electronvolt> {};
using GeV = gigaelectronvolt;
struct TeV : prefixed_unit<TeV, tera, electronvolt> {};
struct PeV : prefixed_unit<PeV, peta, electronvolt> {};
struct EeV : prefixed_unit<EeV, exa, electronvolt> {};    // N.B. very rarely used
struct ZeV : prefixed_unit<ZeV, zetta, electronvolt> {};  // N.B. very rarely used
struct YeV : prefixed_unit<YeV, yotta, electronvolt> {};  // N.B. very rarely used

template<UnitOf<dim_energy> U, Representation Rep = double>
using energy = quantity<dim_energy, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

constexpr auto operator"" _q_feV(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return energy<feV, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_feV(long double l) { return energy<feV, long double>(l); }
constexpr auto operator"" _q_peV(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return energy<peV, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_peV(long double l) { return energy<peV, long double>(l); }
constexpr auto operator"" _q_neV(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return energy<neV, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_neV(long double l) { return energy<ueV, long double>(l); }
constexpr auto operator"" _q_ueV(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return energy<neV, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ueV(long double l) { return energy<ueV, long double>(l); }
constexpr auto operator"" _q_meV(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return energy<meV, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_meV(long double l) { return energy<meV, long double>(l); }

constexpr auto operator"" _q_eV(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return energy<eV, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_eV(long double l) { return energy<eV, long double>(l); }

constexpr auto operator"" _q_keV(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return energy<keV, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_keV(long double l) { return energy<keV, long double>(l); }
constexpr auto operator"" _q_MeV(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return energy<MeV, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_MeV(long double l) { return energy<MeV, long double>(l); }
constexpr auto operator"" _q_GeV(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return energy<GeV, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_GeV(long double l) { return energy<GeV, long double>(l); }
constexpr auto operator"" _q_TeV(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return energy<TeV, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_TeV(long double l) { return energy<TeV, long double>(l); }
constexpr auto operator"" _q_PeV(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return energy<PeV, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_PeV(long double l) { return energy<PeV, long double>(l); }
constexpr auto operator"" _q_EeV(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return energy<EeV, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_EeV(long double l) { return energy<EeV, long double>(l); }
constexpr auto operator"" _q_ZeV(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return energy<ZeV, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ZeV(long double l) { return energy<ZeV, long double>(l); }
constexpr auto operator"" _q_YeV(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return energy<YeV, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_YeV(long double l) { return energy<YeV, long double>(l); }
}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace energy_references {

inline constexpr auto eV = reference<dim_energy, hep::eV>{};

}  // namespace energy_references

namespace references {

using namespace energy_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si::hep

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::hep::inline energy {

template<Representation Rep = double>
using yeV = units::isq::si::hep::energy<units::isq::si::hep::yeV, Rep>;
template<Representation Rep = double>
using zeV = units::isq::si::hep::energy<units::isq::si::hep::zeV, Rep>;
template<Representation Rep = double>
using aeV = units::isq::si::hep::energy<units::isq::si::hep::aeV, Rep>;
template<Representation Rep = double>
using feV = units::isq::si::hep::energy<units::isq::si::hep::feV, Rep>;
template<Representation Rep = double>
using peV = units::isq::si::hep::energy<units::isq::si::hep::peV, Rep>;
template<Representation Rep = double>
using neV = units::isq::si::hep::energy<units::isq::si::hep::neV, Rep>;
template<Representation Rep = double>
using ueV = units::isq::si::hep::energy<units::isq::si::hep::ueV, Rep>;
template<Representation Rep = double>
using meV = units::isq::si::hep::energy<units::isq::si::hep::meV, Rep>;

template<Representation Rep = double>
using eV = units::isq::si::hep::energy<units::isq::si::hep::eV, Rep>;

template<Representation Rep = double>
using keV = units::isq::si::hep::energy<units::isq::si::hep::keV, Rep>;
template<Representation Rep = double>
using MeV = units::isq::si::hep::energy<units::isq::si::hep::MeV, Rep>;
template<Representation Rep = double>
using GeV = units::isq::si::hep::energy<units::isq::si::hep::GeV, Rep>;
template<Representation Rep = double>
using TeV = units::isq::si::hep::energy<units::isq::si::hep::TeV, Rep>;
template<Representation Rep = double>
using PeV = units::isq::si::hep::energy<units::isq::si::hep::PeV, Rep>;
template<Representation Rep = double>
using EeV = units::isq::si::hep::energy<units::isq::si::hep::EeV, Rep>;
template<Representation Rep = double>
using ZeV = units::isq::si::hep::energy<units::isq::si::hep::ZeV, Rep>;
template<Representation Rep = double>
using YeV = units::isq::si::hep::energy<units::isq::si::hep::YeV, Rep>;

}  // namespace units::aliases::isq::si::hep::inline energy

#endif  // UNITS_NO_ALIASES
