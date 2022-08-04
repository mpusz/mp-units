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
#include <units/isq/dimensions/length.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/prefixes.h>
#include <units/unit.h>

namespace units::isq::si {

struct metre : named_unit<metre, "m"> {};
struct yoctometre : prefixed_unit<yoctometre, yocto, metre> {};
struct zeptometre : prefixed_unit<zeptometre, zepto, metre> {};
struct attometre : prefixed_unit<attometre, atto, metre> {};
struct femtometre : prefixed_unit<femtometre, femto, metre> {};
struct picometre : prefixed_unit<picometre, pico, metre> {};
struct nanometre : prefixed_unit<nanometre, nano, metre> {};
struct micrometre : prefixed_unit<micrometre, micro, metre> {};
struct millimetre : prefixed_unit<millimetre, milli, metre> {};
struct centimetre : prefixed_unit<centimetre, centi, metre> {};
struct decimetre : prefixed_unit<decimetre, deci, metre> {};
struct decametre : prefixed_unit<decametre, deca, metre> {};
struct hectometre : prefixed_unit<hectometre, hecto, metre> {};
struct kilometre : prefixed_unit<kilometre, kilo, metre> {};
struct megametre : prefixed_unit<megametre, mega, metre> {};
struct gigametre : prefixed_unit<gigametre, giga, metre> {};
struct terametre : prefixed_unit<terametre, tera, metre> {};
struct petametre : prefixed_unit<petametre, peta, metre> {};
struct exametre : prefixed_unit<exametre, exa, metre> {};
struct zettametre : prefixed_unit<zettametre, zetta, metre> {};
struct yottametre : prefixed_unit<yottametre, yotta, metre> {};

struct astronomical_unit : named_scaled_unit<astronomical_unit, "au", mag<149'597'870'700>(), metre> {};

struct dim_length : isq::dim_length<metre> {};

template<UnitOf<dim_length> U, Representation Rep = double>
using length = quantity<dim_length, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// m
constexpr auto operator"" _q_m(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<metre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_m(long double l) { return length<metre, long double>(l); }

// ym
constexpr auto operator"" _q_ym(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<yoctometre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ym(long double l) { return length<yoctometre, long double>(l); }

// zm
constexpr auto operator"" _q_zm(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<zeptometre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_zm(long double l) { return length<zeptometre, long double>(l); }

// am
constexpr auto operator"" _q_am(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<attometre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_am(long double l) { return length<attometre, long double>(l); }

// fm
constexpr auto operator"" _q_fm(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<femtometre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_fm(long double l) { return length<femtometre, long double>(l); }

// pm
constexpr auto operator"" _q_pm(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<picometre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_pm(long double l) { return length<picometre, long double>(l); }

// nm
constexpr auto operator"" _q_nm(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<nanometre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_nm(long double l) { return length<nanometre, long double>(l); }

// um
constexpr auto operator"" _q_um(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<micrometre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_um(long double l) { return length<micrometre, long double>(l); }

// mm
constexpr auto operator"" _q_mm(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<millimetre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_mm(long double l) { return length<millimetre, long double>(l); }

// cm
constexpr auto operator"" _q_cm(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<centimetre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_cm(long double l) { return length<centimetre, long double>(l); }

// dm
constexpr auto operator"" _q_dm(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<decimetre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_dm(long double l) { return length<decimetre, long double>(l); }

// dam
constexpr auto operator"" _q_dam(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<decametre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_dam(long double l) { return length<decametre, long double>(l); }

// hm
constexpr auto operator"" _q_hm(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<hectometre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_hm(long double l) { return length<hectometre, long double>(l); }

// km
constexpr auto operator"" _q_km(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<kilometre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_km(long double l) { return length<kilometre, long double>(l); }

// Mm
constexpr auto operator"" _q_Mm(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<megametre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Mm(long double l) { return length<megametre, long double>(l); }

// Gm
constexpr auto operator"" _q_Gm(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<gigametre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Gm(long double l) { return length<gigametre, long double>(l); }

// Tm
constexpr auto operator"" _q_Tm(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<terametre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Tm(long double l) { return length<terametre, long double>(l); }

// Pm
constexpr auto operator"" _q_Pm(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<petametre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Pm(long double l) { return length<petametre, long double>(l); }

// Em
constexpr auto operator"" _q_Em(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<exametre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Em(long double l) { return length<exametre, long double>(l); }

// Zm
constexpr auto operator"" _q_Zm(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<zettametre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Zm(long double l) { return length<zettametre, long double>(l); }

// Ym
constexpr auto operator"" _q_Ym(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<yottametre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Ym(long double l) { return length<yottametre, long double>(l); }

// au
constexpr auto operator"" _q_au(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return length<astronomical_unit, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_au(long double l) { return length<astronomical_unit, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace length_references {

inline constexpr auto m = reference<dim_length, metre>{};
inline constexpr auto ym = reference<dim_length, yoctometre>{};
inline constexpr auto zm = reference<dim_length, zeptometre>{};
inline constexpr auto am = reference<dim_length, attometre>{};
inline constexpr auto fm = reference<dim_length, femtometre>{};
inline constexpr auto pm = reference<dim_length, picometre>{};
inline constexpr auto nm = reference<dim_length, nanometre>{};
inline constexpr auto um = reference<dim_length, micrometre>{};
inline constexpr auto mm = reference<dim_length, millimetre>{};
inline constexpr auto cm = reference<dim_length, centimetre>{};
inline constexpr auto dm = reference<dim_length, decimetre>{};
inline constexpr auto dam = reference<dim_length, decametre>{};
inline constexpr auto hm = reference<dim_length, hectometre>{};
inline constexpr auto km = reference<dim_length, kilometre>{};
inline constexpr auto Mm = reference<dim_length, megametre>{};
inline constexpr auto Gm = reference<dim_length, gigametre>{};
inline constexpr auto Tm = reference<dim_length, terametre>{};
inline constexpr auto Pm = reference<dim_length, petametre>{};
inline constexpr auto Em = reference<dim_length, exametre>{};
inline constexpr auto Zm = reference<dim_length, zettametre>{};
inline constexpr auto Ym = reference<dim_length, yottametre>{};
inline constexpr auto au = reference<dim_length, astronomical_unit>{};

}  // namespace length_references

namespace references {

using namespace length_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline length {

template<Representation Rep = double>
using m = units::isq::si::length<units::isq::si::metre, Rep>;
template<Representation Rep = double>
using ym = units::isq::si::length<units::isq::si::yoctometre, Rep>;
template<Representation Rep = double>
using zm = units::isq::si::length<units::isq::si::zeptometre, Rep>;
template<Representation Rep = double>
using am = units::isq::si::length<units::isq::si::attometre, Rep>;
template<Representation Rep = double>
using fm = units::isq::si::length<units::isq::si::femtometre, Rep>;
template<Representation Rep = double>
using pm = units::isq::si::length<units::isq::si::picometre, Rep>;
template<Representation Rep = double>
using nm = units::isq::si::length<units::isq::si::nanometre, Rep>;
template<Representation Rep = double>
using um = units::isq::si::length<units::isq::si::micrometre, Rep>;
template<Representation Rep = double>
using mm = units::isq::si::length<units::isq::si::millimetre, Rep>;
template<Representation Rep = double>
using cm = units::isq::si::length<units::isq::si::centimetre, Rep>;
template<Representation Rep = double>
using dm = units::isq::si::length<units::isq::si::decimetre, Rep>;
template<Representation Rep = double>
using dam = units::isq::si::length<units::isq::si::decametre, Rep>;
template<Representation Rep = double>
using hm = units::isq::si::length<units::isq::si::hectometre, Rep>;
template<Representation Rep = double>
using km = units::isq::si::length<units::isq::si::kilometre, Rep>;
template<Representation Rep = double>
using Mm = units::isq::si::length<units::isq::si::megametre, Rep>;
template<Representation Rep = double>
using Gm = units::isq::si::length<units::isq::si::gigametre, Rep>;
template<Representation Rep = double>
using Tm = units::isq::si::length<units::isq::si::terametre, Rep>;
template<Representation Rep = double>
using Pm = units::isq::si::length<units::isq::si::petametre, Rep>;
template<Representation Rep = double>
using Em = units::isq::si::length<units::isq::si::exametre, Rep>;
template<Representation Rep = double>
using Zm = units::isq::si::length<units::isq::si::zettametre, Rep>;
template<Representation Rep = double>
using Ym = units::isq::si::length<units::isq::si::yottametre, Rep>;
template<Representation Rep = double>
using au = units::isq::si::length<units::isq::si::astronomical_unit, Rep>;

}  // namespace units::aliases::isq::si::inline length

#endif  // UNITS_NO_ALIASES
