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
#include <units/isq/dimensions/area.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/length.h>
#include <units/unit.h>

namespace units::isq::si {

struct square_metre : derived_unit<square_metre> {};
struct dim_area : isq::dim_area<dim_area, square_metre, dim_length> {};

struct square_yoctometre : derived_scaled_unit<square_yoctometre, dim_area, yoctometre> {};
struct square_zeptometre : derived_scaled_unit<square_zeptometre, dim_area, zeptometre> {};
struct square_attometre : derived_scaled_unit<square_attometre, dim_area, attometre> {};
struct square_femtometre : derived_scaled_unit<square_femtometre, dim_area, femtometre> {};
struct square_picometre : derived_scaled_unit<square_picometre, dim_area, picometre> {};
struct square_nanometre : derived_scaled_unit<square_nanometre, dim_area, nanometre> {};
struct square_micrometre : derived_scaled_unit<square_micrometre, dim_area, micrometre> {};
struct square_millimetre : derived_scaled_unit<square_millimetre, dim_area, millimetre> {};
struct square_centimetre : derived_scaled_unit<square_centimetre, dim_area, centimetre> {};
struct square_decimetre : derived_scaled_unit<square_decimetre, dim_area, decimetre> {};
struct square_decametre : derived_scaled_unit<square_decametre, dim_area, decametre> {};
struct square_hectometre : derived_scaled_unit<square_hectometre, dim_area, hectometre> {};
struct square_kilometre : derived_scaled_unit<square_kilometre, dim_area, kilometre> {};
struct square_megametre : derived_scaled_unit<square_megametre, dim_area, megametre> {};
struct square_gigametre : derived_scaled_unit<square_gigametre, dim_area, gigametre> {};
struct square_terametre : derived_scaled_unit<square_terametre, dim_area, terametre> {};
struct square_petametre : derived_scaled_unit<square_petametre, dim_area, petametre> {};
struct square_exametre : derived_scaled_unit<square_exametre, dim_area, exametre> {};
struct square_zettametre : derived_scaled_unit<square_zettametre, dim_area, zettametre> {};
struct square_yottametre : derived_scaled_unit<square_yottametre, dim_area, yottametre> {};

struct are : alias_unit<square_decametre, "a"> {};
struct centiare : prefixed_alias_unit<square_metre, centi, are> {};
struct deciare : prefixed_unit<deciare, deci, are> {};
struct decare : prefixed_unit<decare, deca, are> {};
struct hectare : prefixed_alias_unit<square_hectometre, hecto, are> {};

template<UnitOf<dim_area> U, Representation Rep = double>
using area = quantity<dim_area, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// m2
constexpr auto operator"" _q_m2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<square_metre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_m2(long double l) { return area<square_metre, long double>(l); }

// ym2
constexpr auto operator"" _q_ym2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<square_yoctometre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ym2(long double l) { return area<square_yoctometre, long double>(l); }

// zm2
constexpr auto operator"" _q_zm2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<square_zeptometre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_zm2(long double l) { return area<square_zeptometre, long double>(l); }

// am2
constexpr auto operator"" _q_am2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<square_attometre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_am2(long double l) { return area<square_attometre, long double>(l); }

// fm2
constexpr auto operator"" _q_fm2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<square_femtometre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_fm2(long double l) { return area<square_femtometre, long double>(l); }

// pm2
constexpr auto operator"" _q_pm2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<square_picometre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_pm2(long double l) { return area<square_picometre, long double>(l); }

// nm2
constexpr auto operator"" _q_nm2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<square_nanometre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_nm2(long double l) { return area<square_nanometre, long double>(l); }

// um2
constexpr auto operator"" _q_um2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<square_micrometre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_um2(long double l) { return area<square_micrometre, long double>(l); }

// mm2
constexpr auto operator"" _q_mm2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<square_millimetre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_mm2(long double l) { return area<square_millimetre, long double>(l); }

// cm2
constexpr auto operator"" _q_cm2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<square_centimetre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_cm2(long double l) { return area<square_centimetre, long double>(l); }

// dm2
constexpr auto operator"" _q_dm2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<square_decimetre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_dm2(long double l) { return area<square_decimetre, long double>(l); }

// dam2
constexpr auto operator"" _q_dam2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<square_decametre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_dam2(long double l) { return area<square_decametre, long double>(l); }

// hm2
constexpr auto operator"" _q_hm2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<square_hectometre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_hm2(long double l) { return area<square_hectometre, long double>(l); }

// km2
constexpr auto operator"" _q_km2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<square_kilometre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_km2(long double l) { return area<square_kilometre, long double>(l); }

// Mm2
constexpr auto operator"" _q_Mm2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<square_megametre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Mm2(long double l) { return area<square_megametre, long double>(l); }

// Gm2
constexpr auto operator"" _q_Gm2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<square_gigametre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Gm2(long double l) { return area<square_gigametre, long double>(l); }

// Tm2
constexpr auto operator"" _q_Tm2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<square_terametre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Tm2(long double l) { return area<square_terametre, long double>(l); }

// Pm2
constexpr auto operator"" _q_Pm2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<square_petametre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Pm2(long double l) { return area<square_petametre, long double>(l); }

// Em2
constexpr auto operator"" _q_Em2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<square_exametre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Em2(long double l) { return area<square_exametre, long double>(l); }

// Zm2
constexpr auto operator"" _q_Zm2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<square_zettametre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Zm2(long double l) { return area<square_zettametre, long double>(l); }

// Ym2
constexpr auto operator"" _q_Ym2(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<square_yottametre, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Ym2(long double l) { return area<square_yottametre, long double>(l); }

// a
constexpr auto operator"" _q_a(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<are, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_a(long double l) { return area<are, long double>(l); }

// ca
constexpr auto operator"" _q_ca(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<centiare, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ca(long double l) { return area<centiare, long double>(l); }

// da
constexpr auto operator"" _q_da(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<deciare, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_da(long double l) { return area<deciare, long double>(l); }

// daa
constexpr auto operator"" _q_daa(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<decare, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_daa(long double l) { return area<decare, long double>(l); }

// ha
constexpr auto operator"" _q_ha(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<hectare, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ha(long double l) { return area<hectare, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace area_references {

inline constexpr auto m2 = reference<dim_area, square_metre>{};
inline constexpr auto ym2 = reference<dim_area, square_yoctometre>{};
inline constexpr auto zm2 = reference<dim_area, square_zeptometre>{};
inline constexpr auto am2 = reference<dim_area, square_attometre>{};
inline constexpr auto fm2 = reference<dim_area, square_femtometre>{};
inline constexpr auto pm2 = reference<dim_area, square_picometre>{};
inline constexpr auto nm2 = reference<dim_area, square_nanometre>{};
inline constexpr auto um2 = reference<dim_area, square_micrometre>{};
inline constexpr auto mm2 = reference<dim_area, square_millimetre>{};
inline constexpr auto cm2 = reference<dim_area, square_centimetre>{};
inline constexpr auto dm2 = reference<dim_area, square_decimetre>{};
inline constexpr auto dam2 = reference<dim_area, square_decametre>{};
inline constexpr auto hm2 = reference<dim_area, square_hectometre>{};
inline constexpr auto km2 = reference<dim_area, square_kilometre>{};
inline constexpr auto Mm2 = reference<dim_area, square_megametre>{};
inline constexpr auto Gm2 = reference<dim_area, square_gigametre>{};
inline constexpr auto Tm2 = reference<dim_area, square_terametre>{};
inline constexpr auto Pm2 = reference<dim_area, square_petametre>{};
inline constexpr auto Em2 = reference<dim_area, square_exametre>{};
inline constexpr auto Zm2 = reference<dim_area, square_zettametre>{};
inline constexpr auto Ym2 = reference<dim_area, square_yottametre>{};

inline constexpr auto a = reference<dim_area, are>{};
inline constexpr auto ca = reference<dim_area, centiare>{};
inline constexpr auto da = reference<dim_area, deciare>{};
inline constexpr auto daa = reference<dim_area, decare>{};
inline constexpr auto ha = reference<dim_area, hectare>{};

}  // namespace area_references

namespace references {

using namespace area_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline area {

template<Representation Rep = double>
using m2 = units::isq::si::area<units::isq::si::square_metre, Rep>;
template<Representation Rep = double>
using ym2 = units::isq::si::area<units::isq::si::square_yoctometre, Rep>;
template<Representation Rep = double>
using zm2 = units::isq::si::area<units::isq::si::square_zeptometre, Rep>;
template<Representation Rep = double>
using am2 = units::isq::si::area<units::isq::si::square_attometre, Rep>;
template<Representation Rep = double>
using fm2 = units::isq::si::area<units::isq::si::square_femtometre, Rep>;
template<Representation Rep = double>
using pm2 = units::isq::si::area<units::isq::si::square_picometre, Rep>;
template<Representation Rep = double>
using nm2 = units::isq::si::area<units::isq::si::square_nanometre, Rep>;
template<Representation Rep = double>
using um2 = units::isq::si::area<units::isq::si::square_micrometre, Rep>;
template<Representation Rep = double>
using mm2 = units::isq::si::area<units::isq::si::square_millimetre, Rep>;
template<Representation Rep = double>
using cm2 = units::isq::si::area<units::isq::si::square_centimetre, Rep>;
template<Representation Rep = double>
using dm2 = units::isq::si::area<units::isq::si::square_decimetre, Rep>;
template<Representation Rep = double>
using dam2 = units::isq::si::area<units::isq::si::square_decametre, Rep>;
template<Representation Rep = double>
using hm2 = units::isq::si::area<units::isq::si::square_hectometre, Rep>;
template<Representation Rep = double>
using km2 = units::isq::si::area<units::isq::si::square_kilometre, Rep>;
template<Representation Rep = double>
using Mm2 = units::isq::si::area<units::isq::si::square_megametre, Rep>;
template<Representation Rep = double>
using Gm2 = units::isq::si::area<units::isq::si::square_gigametre, Rep>;
template<Representation Rep = double>
using Tm2 = units::isq::si::area<units::isq::si::square_terametre, Rep>;
template<Representation Rep = double>
using Pm2 = units::isq::si::area<units::isq::si::square_petametre, Rep>;
template<Representation Rep = double>
using Em2 = units::isq::si::area<units::isq::si::square_exametre, Rep>;
template<Representation Rep = double>
using Zm2 = units::isq::si::area<units::isq::si::square_zettametre, Rep>;
template<Representation Rep = double>
using Ym2 = units::isq::si::area<units::isq::si::square_yottametre, Rep>;

template<Representation Rep = double>
using ha = units::isq::si::area<units::isq::si::hectare, Rep>;

}  // namespace units::aliases::isq::si::inline area

#endif  // UNITS_NO_ALIASES
