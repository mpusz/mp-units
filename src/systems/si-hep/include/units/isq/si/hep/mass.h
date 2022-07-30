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
#include <units/isq/dimensions/mass.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/mass.h>
#include <units/unit.h>

// Necessary to factor `1'672'621'923'695`, which appears in the proton mass.
template<>
inline constexpr std::optional<std::intmax_t> units::known_first_factor<334'524'384'739> = 334'524'384'739;

// Necessary to factor `17'826'619'216'279`, which appears in the value for eV/c^2.
template<>
inline constexpr std::optional<std::intmax_t> units::known_first_factor<225'653'407'801> = 225'653'407'801;

namespace units::isq::si::hep {

struct eV_per_c2 :
    named_scaled_unit<eV_per_c2, basic_symbol_text{"eV/c²", "eV/c^2"},
                      mag<ratio(17'826'619'216'279, 1'000'000'000'000)>() * mag_power<10, -35>(), kilogram> {};
struct feV_per_c2 : prefixed_unit<feV_per_c2, femto, eV_per_c2> {};
struct peV_per_c2 : prefixed_unit<peV_per_c2, pico, eV_per_c2> {};
struct neV_per_c2 : prefixed_unit<neV_per_c2, nano, eV_per_c2> {};
struct ueV_per_c2 : prefixed_unit<ueV_per_c2, micro, eV_per_c2> {};
struct meV_per_c2 :
    prefixed_unit<meV_per_c2, milli, eV_per_c2> {};  // approximate mass of an electron/positron (0.511 MeV/c2)
struct keV_per_c2 : prefixed_unit<keV_per_c2, kilo, eV_per_c2> {};
struct MeV_per_c2 : prefixed_unit<MeV_per_c2, mega, eV_per_c2> {};
struct GeV_per_c2 :
    prefixed_unit<GeV_per_c2, giga, eV_per_c2> {};  // approximate mass of a proton (0.938 GeV/c2) or neutron
struct TeV_per_c2 : prefixed_unit<TeV_per_c2, tera, eV_per_c2> {};
struct PeV_per_c2 : prefixed_unit<PeV_per_c2, peta, eV_per_c2> {};
struct EeV_per_c2 : prefixed_unit<EeV_per_c2, exa, eV_per_c2> {};
struct YeV_per_c2 : prefixed_unit<YeV_per_c2, yotta, eV_per_c2> {};
struct electron_mass :
    named_scaled_unit<eV_per_c2, "m_e", mag<ratio(9'109'383'701'528, 1'000'000'000'000)>() * mag_power<10, -31>(),
                      kilogram> {};
struct proton_mass :
    named_scaled_unit<eV_per_c2, "m_p", mag<ratio(1'672'621'923'695, 1'000'000'000'000)>() * mag_power<10, -27>(),
                      kilogram> {};
struct neutron_mass :
    named_scaled_unit<eV_per_c2, "m_n", mag<ratio(1'674'927'498'049, 1'000'000'000'000)>() * mag_power<10, -27>(),
                      kilogram> {};

struct dim_mass : isq::dim_mass<eV_per_c2> {};

template<UnitOf<dim_mass> U, Representation Rep = double>
using mass = quantity<dim_mass, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {
constexpr auto operator"" _q_feV_per_c2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<feV_per_c2, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_feV_per_c2(long double l) { return mass<feV_per_c2, long double>(l); }
constexpr auto operator"" _q_peV_per_c2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<peV_per_c2, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_peV_per_c2(long double l) { return mass<peV_per_c2, long double>(l); }
constexpr auto operator"" _q_neV_per_c2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<neV_per_c2, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_neV_per_c2(long double l) { return mass<neV_per_c2, long double>(l); }
constexpr auto operator"" _q_ueV_per_c2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<ueV_per_c2, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ueV_per_c2(long double l) { return mass<ueV_per_c2, long double>(l); }
constexpr auto operator"" _q_meV_per_c2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<meV_per_c2, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_meV_per_c2(long double l) { return mass<meV_per_c2, long double>(l); }

// eV_per_c2
constexpr auto operator"" _q_eV_per_c2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<eV_per_c2, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_eV_per_c2(long double l) { return mass<eV_per_c2, long double>(l); }

constexpr auto operator"" _q_keV_per_c2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<keV_per_c2, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_keV_per_c2(long double l) { return mass<keV_per_c2, long double>(l); }
constexpr auto operator"" _q_MeV_per_c2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<MeV_per_c2, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_MeV_per_c2(long double l) { return mass<MeV_per_c2, long double>(l); }
constexpr auto operator"" _q_GeV_per_c2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<GeV_per_c2, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_GeV_per_c2(long double l) { return mass<GeV_per_c2, long double>(l); }
constexpr auto operator"" _q_TeV_per_c2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<TeV_per_c2, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_TeV_per_c2(long double l) { return mass<TeV_per_c2, long double>(l); }
constexpr auto operator"" _q_PeV_per_c2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<PeV_per_c2, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_PeV_per_c2(long double l) { return mass<PeV_per_c2, long double>(l); }
constexpr auto operator"" _q_EeV_per_c2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<EeV_per_c2, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_EeV_per_c2(long double l) { return mass<EeV_per_c2, long double>(l); }
constexpr auto operator"" _q_YeV_per_c2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<YeV_per_c2, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_YeV_per_c2(long double l) { return mass<YeV_per_c2, long double>(l); }

// special HEP masses
constexpr auto operator"" _q_electron_mass(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<electron_mass, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_electron_mass(long double l) { return mass<electron_mass, long double>(l); }
constexpr auto operator"" _q_proton_mass(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<proton_mass, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_proton_mass(long double l) { return mass<proton_mass, long double>(l); }
constexpr auto operator"" _q_neutron_mass(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<neutron_mass, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_neutron_mass(long double l) { return mass<neutron_mass, long double>(l); }
}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace mass_references {

inline constexpr auto eV_per_c2 = reference<dim_mass, ::units::isq::si::hep::eV_per_c2>{};

}  // namespace mass_references

namespace references {

using namespace mass_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si::hep

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::hep::inline mass {

template<Representation Rep = double>
using eV_per_c2g = units::isq::si::hep::mass<units::isq::si::hep::eV_per_c2, Rep>;

}  // namespace units::aliases::isq::si::hep::inline mass

#endif  // UNITS_NO_ALIASES
