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
#include <units/isq/dimensions/mass.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/prefixes.h>
#include <units/unit.h>

namespace units::isq::si {

struct gram : named_unit<gram, "g"> {};
struct yoctogram : prefixed_unit<yoctogram, yocto, gram> {};
struct zeptogram : prefixed_unit<zeptogram, zepto, gram> {};
struct attogram : prefixed_unit<attogram, atto, gram> {};
struct femtogram : prefixed_unit<femtogram, femto, gram> {};
struct picogram : prefixed_unit<picogram, pico, gram> {};
struct nanogram : prefixed_unit<nanogram, nano, gram> {};
struct microgram : prefixed_unit<microgram, micro, gram> {};
struct milligram : prefixed_unit<milligram, milli, gram> {};
struct centigram : prefixed_unit<centigram, centi, gram> {};
struct decigram : prefixed_unit<decigram, deci, gram> {};
struct decagram : prefixed_unit<decagram, deca, gram> {};
struct hectogram : prefixed_unit<hectogram, hecto, gram> {};
struct kilogram : prefixed_unit<kilogram, kilo, gram> {};
struct megagram : prefixed_unit<megagram, mega, gram> {};
struct gigagram : prefixed_unit<gigagram, giga, gram> {};
struct teragram : prefixed_unit<teragram, tera, gram> {};
struct petagram : prefixed_unit<petagram, peta, gram> {};
struct exagram : prefixed_unit<exagram, exa, gram> {};
struct zettagram : prefixed_unit<zettagram, zetta, gram> {};
struct yottagram : prefixed_unit<yottagram, yotta, gram> {};

struct tonne : alias_unit<megagram, "t"> {};
struct yoctotonne : prefixed_alias_unit<attogram, yocto, tonne> {};
struct zeptotonne : prefixed_alias_unit<femtogram, zepto, tonne> {};
struct attotonne : prefixed_alias_unit<picogram, atto, tonne> {};
struct femtotonne : prefixed_alias_unit<nanogram, femto, tonne> {};
struct picotonne : prefixed_alias_unit<microgram, pico, tonne> {};
struct nanotonne : prefixed_alias_unit<milligram, nano, tonne> {};
struct microtonne : prefixed_alias_unit<gram, micro, tonne> {};
struct millitonne : prefixed_alias_unit<kilogram, milli, tonne> {};
struct centitonne : prefixed_unit<centitonne, centi, tonne> {};
struct decitonne : prefixed_unit<decitonne, deci, tonne> {};
struct decatonne : prefixed_unit<decatonne, deca, tonne> {};
struct hectotonne : prefixed_unit<hectotonne, hecto, tonne> {};
struct kilotonne : prefixed_alias_unit<gigagram, kilo, tonne> {};
struct megatonne : prefixed_alias_unit<teragram, mega, tonne> {};
struct gigatonne : prefixed_alias_unit<petagram, giga, tonne> {};
struct teratonne : prefixed_alias_unit<exagram, tera, tonne> {};
struct petatonne : prefixed_alias_unit<zettagram, peta, tonne> {};
struct exatonne : prefixed_alias_unit<yottagram, exa, tonne> {};
struct zettatonne : prefixed_unit<zettatonne, zetta, tonne> {};
struct yottatonne : prefixed_unit<yottatonne, yotta, tonne> {};

struct dalton :
    named_scaled_unit<dalton, "Da", mag<ratio(16'605'390'666'050, 10'000'000'000'000)>() * mag_power<10, -27>(),
                      kilogram> {};

struct dim_mass : isq::dim_mass<kilogram> {};

template<UnitOf<dim_mass> U, Representation Rep = double>
using mass = quantity<dim_mass, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// g
constexpr auto operator"" _q_g(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<gram, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_g(long double l) { return mass<gram, long double>(l); }

// yg
constexpr auto operator"" _q_yg(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<yoctogram, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_yg(long double l) { return mass<yoctogram, long double>(l); }

// zg
constexpr auto operator"" _q_zg(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<zeptogram, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_zg(long double l) { return mass<zeptogram, long double>(l); }

// ag
constexpr auto operator"" _q_ag(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<attogram, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ag(long double l) { return mass<attogram, long double>(l); }

// fg
constexpr auto operator"" _q_fg(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<femtogram, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_fg(long double l) { return mass<femtogram, long double>(l); }

// pg
constexpr auto operator"" _q_pg(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<picogram, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_pg(long double l) { return mass<picogram, long double>(l); }

// ng
constexpr auto operator"" _q_ng(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<nanogram, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ng(long double l) { return mass<nanogram, long double>(l); }

// ug
constexpr auto operator"" _q_ug(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<microgram, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ug(long double l) { return mass<microgram, long double>(l); }

// mg
constexpr auto operator"" _q_mg(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<milligram, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_mg(long double l) { return mass<milligram, long double>(l); }

// cg
constexpr auto operator"" _q_cg(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<centigram, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_cg(long double l) { return mass<centigram, long double>(l); }

// dg
constexpr auto operator"" _q_dg(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<decigram, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_dg(long double l) { return mass<decigram, long double>(l); }

// dag
constexpr auto operator"" _q_dag(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<decagram, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_dag(long double l) { return mass<decagram, long double>(l); }

// hg
constexpr auto operator"" _q_hg(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<hectogram, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_hg(long double l) { return mass<hectogram, long double>(l); }

// kg
constexpr auto operator"" _q_kg(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<kilogram, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_kg(long double l) { return mass<kilogram, long double>(l); }

// Mg
constexpr auto operator"" _q_Mg(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<megagram, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Mg(long double l) { return mass<megagram, long double>(l); }

// Gg
constexpr auto operator"" _q_Gg(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<gigagram, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Gg(long double l) { return mass<gigagram, long double>(l); }

// Tg
constexpr auto operator"" _q_Tg(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<teragram, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Tg(long double l) { return mass<teragram, long double>(l); }

// Pg
constexpr auto operator"" _q_Pg(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<petagram, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Pg(long double l) { return mass<petagram, long double>(l); }

// Eg
constexpr auto operator"" _q_Eg(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<exagram, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Eg(long double l) { return mass<exagram, long double>(l); }

// Zg
constexpr auto operator"" _q_Zg(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<zettagram, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Zg(long double l) { return mass<zettagram, long double>(l); }

// Yg
constexpr auto operator"" _q_Yg(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<yottagram, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Yg(long double l) { return mass<yottagram, long double>(l); }

// t
constexpr auto operator"" _q_t(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<tonne, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_t(long double l) { return mass<tonne, long double>(l); }

// yt
constexpr auto operator"" _q_yt(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<yoctotonne, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_yt(long double l) { return mass<yoctotonne, long double>(l); }

// zt
constexpr auto operator"" _q_zt(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<zeptotonne, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_zt(long double l) { return mass<zeptotonne, long double>(l); }

// at
constexpr auto operator"" _q_at(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<attotonne, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_at(long double l) { return mass<attotonne, long double>(l); }

// TODO Collides with foot
// ft
// constexpr auto operator"" _q_ft(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return
// mass<femtotonne, std::int64_t>(static_cast<std::int64_t>(l)); } constexpr auto operator"" _q_ft(long double l) {
// return mass<femtotonne, long double>(l); }

// pt
constexpr auto operator"" _q_pt(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<picotonne, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_pt(long double l) { return mass<picotonne, long double>(l); }

// nt
constexpr auto operator"" _q_nt(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<nanotonne, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_nt(long double l) { return mass<nanotonne, long double>(l); }

// ut
constexpr auto operator"" _q_ut(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<microtonne, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ut(long double l) { return mass<microtonne, long double>(l); }

// mt
constexpr auto operator"" _q_mt(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<millitonne, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_mt(long double l) { return mass<millitonne, long double>(l); }

// ct
constexpr auto operator"" _q_ct(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<centitonne, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ct(long double l) { return mass<centitonne, long double>(l); }

// dt
constexpr auto operator"" _q_dt(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<decitonne, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_dt(long double l) { return mass<decitonne, long double>(l); }

// dat
constexpr auto operator"" _q_dat(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<decatonne, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_dat(long double l) { return mass<decatonne, long double>(l); }

// ht
constexpr auto operator"" _q_ht(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<hectotonne, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ht(long double l) { return mass<hectotonne, long double>(l); }

// kt
constexpr auto operator"" _q_kt(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<kilotonne, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_kt(long double l) { return mass<kilotonne, long double>(l); }

// Mt
constexpr auto operator"" _q_Mt(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<megatonne, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Mt(long double l) { return mass<megatonne, long double>(l); }

// Gt
constexpr auto operator"" _q_Gt(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<gigatonne, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Gt(long double l) { return mass<gigatonne, long double>(l); }

// Tt
constexpr auto operator"" _q_Tt(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<teratonne, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Tt(long double l) { return mass<teratonne, long double>(l); }

// Pt
constexpr auto operator"" _q_Pt(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<petatonne, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Pt(long double l) { return mass<petatonne, long double>(l); }

// Et
constexpr auto operator"" _q_Et(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<exatonne, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Et(long double l) { return mass<exatonne, long double>(l); }

// Zt
constexpr auto operator"" _q_Zt(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<zettatonne, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Zt(long double l) { return mass<zettatonne, long double>(l); }

// Yt
constexpr auto operator"" _q_Yt(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<yottatonne, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Yt(long double l) { return mass<yottatonne, long double>(l); }

// Da
constexpr auto operator"" _q_Da(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<dalton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Da(long double l) { return mass<dalton, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace mass_references {

inline constexpr auto g = reference<dim_mass, gram>{};
inline constexpr auto yg = reference<dim_mass, yoctogram>{};
inline constexpr auto zg = reference<dim_mass, zeptogram>{};
inline constexpr auto ag = reference<dim_mass, attogram>{};
inline constexpr auto fg = reference<dim_mass, femtogram>{};
inline constexpr auto pg = reference<dim_mass, picogram>{};
inline constexpr auto ng = reference<dim_mass, nanogram>{};
inline constexpr auto ug = reference<dim_mass, microgram>{};
inline constexpr auto mg = reference<dim_mass, milligram>{};
inline constexpr auto cg = reference<dim_mass, centigram>{};
inline constexpr auto dg = reference<dim_mass, decigram>{};
inline constexpr auto dag = reference<dim_mass, decagram>{};
inline constexpr auto hg = reference<dim_mass, hectogram>{};
inline constexpr auto kg = reference<dim_mass, kilogram>{};
inline constexpr auto Mg = reference<dim_mass, megagram>{};
inline constexpr auto Gg = reference<dim_mass, gigagram>{};
inline constexpr auto Tg = reference<dim_mass, teragram>{};
inline constexpr auto Pg = reference<dim_mass, petagram>{};
inline constexpr auto Eg = reference<dim_mass, exagram>{};
inline constexpr auto Zg = reference<dim_mass, zettagram>{};
inline constexpr auto Yg = reference<dim_mass, yottagram>{};

inline constexpr auto t = reference<dim_mass, tonne>{};
inline constexpr auto yt = reference<dim_mass, yoctotonne>{};
inline constexpr auto zt = reference<dim_mass, zeptotonne>{};
inline constexpr auto at = reference<dim_mass, attotonne>{};
inline constexpr auto ft = reference<dim_mass, femtotonne>{};
inline constexpr auto pt = reference<dim_mass, picotonne>{};
inline constexpr auto nt = reference<dim_mass, nanotonne>{};
inline constexpr auto ut = reference<dim_mass, microtonne>{};
inline constexpr auto mt = reference<dim_mass, millitonne>{};
inline constexpr auto ct = reference<dim_mass, centitonne>{};
inline constexpr auto dt = reference<dim_mass, decitonne>{};
inline constexpr auto dat = reference<dim_mass, decatonne>{};
inline constexpr auto ht = reference<dim_mass, hectotonne>{};
inline constexpr auto kt = reference<dim_mass, kilotonne>{};
inline constexpr auto Mt = reference<dim_mass, megatonne>{};
inline constexpr auto Gt = reference<dim_mass, gigatonne>{};
inline constexpr auto Tt = reference<dim_mass, teratonne>{};
inline constexpr auto Pt = reference<dim_mass, petatonne>{};
inline constexpr auto Et = reference<dim_mass, exatonne>{};
inline constexpr auto Zt = reference<dim_mass, zettatonne>{};
inline constexpr auto Yt = reference<dim_mass, yottatonne>{};
inline constexpr auto Da = reference<dim_mass, dalton>{};

}  // namespace mass_references

namespace references {

using namespace mass_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline mass {

template<Representation Rep = double>
using g = units::isq::si::mass<units::isq::si::gram, Rep>;
template<Representation Rep = double>
using yg = units::isq::si::mass<units::isq::si::yoctogram, Rep>;
template<Representation Rep = double>
using zg = units::isq::si::mass<units::isq::si::zeptogram, Rep>;
template<Representation Rep = double>
using ag = units::isq::si::mass<units::isq::si::attogram, Rep>;
template<Representation Rep = double>
using fg = units::isq::si::mass<units::isq::si::femtogram, Rep>;
template<Representation Rep = double>
using pg = units::isq::si::mass<units::isq::si::picogram, Rep>;
template<Representation Rep = double>
using ng = units::isq::si::mass<units::isq::si::nanogram, Rep>;
template<Representation Rep = double>
using ug = units::isq::si::mass<units::isq::si::microgram, Rep>;
template<Representation Rep = double>
using mg = units::isq::si::mass<units::isq::si::milligram, Rep>;
template<Representation Rep = double>
using cg = units::isq::si::mass<units::isq::si::centigram, Rep>;
template<Representation Rep = double>
using dg = units::isq::si::mass<units::isq::si::decigram, Rep>;
template<Representation Rep = double>
using dag = units::isq::si::mass<units::isq::si::decagram, Rep>;
template<Representation Rep = double>
using hg = units::isq::si::mass<units::isq::si::hectogram, Rep>;
template<Representation Rep = double>
using kg = units::isq::si::mass<units::isq::si::kilogram, Rep>;
template<Representation Rep = double>
using Mg = units::isq::si::mass<units::isq::si::megagram, Rep>;
template<Representation Rep = double>
using Gg = units::isq::si::mass<units::isq::si::gigagram, Rep>;
template<Representation Rep = double>
using Tg = units::isq::si::mass<units::isq::si::teragram, Rep>;
template<Representation Rep = double>
using Pg = units::isq::si::mass<units::isq::si::petagram, Rep>;
template<Representation Rep = double>
using Eg = units::isq::si::mass<units::isq::si::exagram, Rep>;
template<Representation Rep = double>
using Zg = units::isq::si::mass<units::isq::si::zettagram, Rep>;
template<Representation Rep = double>
using Yg = units::isq::si::mass<units::isq::si::yottagram, Rep>;

template<Representation Rep = double>
using t = units::isq::si::mass<units::isq::si::tonne, Rep>;
template<Representation Rep = double>
using yt = units::isq::si::mass<units::isq::si::yoctotonne, Rep>;
template<Representation Rep = double>
using zt = units::isq::si::mass<units::isq::si::zeptotonne, Rep>;
template<Representation Rep = double>
using at = units::isq::si::mass<units::isq::si::attotonne, Rep>;
template<Representation Rep = double>
using ft = units::isq::si::mass<units::isq::si::femtotonne, Rep>;
template<Representation Rep = double>
using pt = units::isq::si::mass<units::isq::si::picotonne, Rep>;
template<Representation Rep = double>
using nt = units::isq::si::mass<units::isq::si::nanotonne, Rep>;
template<Representation Rep = double>
using ut = units::isq::si::mass<units::isq::si::microtonne, Rep>;
template<Representation Rep = double>
using mt = units::isq::si::mass<units::isq::si::millitonne, Rep>;
template<Representation Rep = double>
using ct = units::isq::si::mass<units::isq::si::centitonne, Rep>;
template<Representation Rep = double>
using dt = units::isq::si::mass<units::isq::si::decitonne, Rep>;
template<Representation Rep = double>
using dat = units::isq::si::mass<units::isq::si::decatonne, Rep>;
template<Representation Rep = double>
using ht = units::isq::si::mass<units::isq::si::hectotonne, Rep>;
template<Representation Rep = double>
using kt = units::isq::si::mass<units::isq::si::kilotonne, Rep>;
template<Representation Rep = double>
using Mt = units::isq::si::mass<units::isq::si::megatonne, Rep>;
template<Representation Rep = double>
using Gt = units::isq::si::mass<units::isq::si::gigatonne, Rep>;
template<Representation Rep = double>
using Tt = units::isq::si::mass<units::isq::si::teratonne, Rep>;
template<Representation Rep = double>
using Pt = units::isq::si::mass<units::isq::si::petatonne, Rep>;
template<Representation Rep = double>
using Et = units::isq::si::mass<units::isq::si::exatonne, Rep>;
template<Representation Rep = double>
using Zt = units::isq::si::mass<units::isq::si::zettatonne, Rep>;
template<Representation Rep = double>
using Yt = units::isq::si::mass<units::isq::si::yottatonne, Rep>;
template<Representation Rep = double>
using Da = units::isq::si::mass<units::isq::si::dalton, Rep>;

}  // namespace units::aliases::isq::si::inline mass

#endif  // UNITS_NO_ALIASES
