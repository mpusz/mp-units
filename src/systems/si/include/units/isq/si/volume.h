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
#include <units/isq/dimensions/volume.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/length.h>
#include <units/unit.h>

namespace units::isq::si {

struct cubic_metre : unit<cubic_metre> {};
struct dim_volume : isq::dim_volume<dim_volume, cubic_metre, dim_length> {};

struct cubic_yoctometre : deduced_unit<cubic_yoctometre, dim_volume, yoctometre> {};
struct cubic_zeptometre : deduced_unit<cubic_zeptometre, dim_volume, zeptometre> {};
struct cubic_attometre : deduced_unit<cubic_attometre, dim_volume, attometre> {};
struct cubic_femtometre : deduced_unit<cubic_femtometre, dim_volume, femtometre> {};
struct cubic_picometre : deduced_unit<cubic_picometre, dim_volume, picometre> {};
struct cubic_nanometre : deduced_unit<cubic_nanometre, dim_volume, nanometre> {};
struct cubic_micrometre : deduced_unit<cubic_micrometre, dim_volume, micrometre> {};
struct cubic_millimetre : deduced_unit<cubic_millimetre, dim_volume, millimetre> {};
struct cubic_centimetre : deduced_unit<cubic_centimetre, dim_volume, centimetre> {};
struct cubic_decimetre : deduced_unit<cubic_decimetre, dim_volume, decimetre> {};
struct cubic_decametre : deduced_unit<cubic_decametre, dim_volume, decametre> {};
struct cubic_hectometre : deduced_unit<cubic_hectometre, dim_volume, hectometre> {};
struct cubic_kilometre : deduced_unit<cubic_kilometre, dim_volume, kilometre> {};
struct cubic_megametre : deduced_unit<cubic_megametre, dim_volume, megametre> {};
struct cubic_gigametre : deduced_unit<cubic_gigametre, dim_volume, gigametre> {};
struct cubic_terametre : deduced_unit<cubic_terametre, dim_volume, terametre> {};
struct cubic_petametre : deduced_unit<cubic_petametre, dim_volume, petametre> {};
struct cubic_exametre : deduced_unit<cubic_exametre, dim_volume, exametre> {};
struct cubic_zettametre : deduced_unit<cubic_zettametre, dim_volume, zettametre> {};
struct cubic_yottametre : deduced_unit<cubic_yottametre, dim_volume, yottametre> {};

struct litre : alias_unit<cubic_decimetre, "l", prefix> {};
struct yoctolitre : prefixed_alias_unit<cubic_nanometre, yocto, litre> {};
struct zeptolitre : prefixed_unit<zeptolitre, zepto, litre> {};
struct attolitre : prefixed_unit<attolitre, atto, litre> {};
struct femtolitre : prefixed_alias_unit<cubic_micrometre, femto, litre> {};
struct picolitre : prefixed_unit<picolitre, pico, litre> {};
struct nanolitre : prefixed_unit<nanolitre, nano, litre> {};
struct microlitre : prefixed_alias_unit<cubic_millimetre, micro, litre> {};
struct millilitre : prefixed_alias_unit<cubic_centimetre, milli, litre> {};
struct centilitre : prefixed_unit<centilitre, centi, litre> {};
struct decilitre : prefixed_unit<decilitre, deci, litre> {};
struct decalitre : prefixed_unit<decalitre, deca, litre> {};
struct hectolitre : prefixed_alias_unit<cubic_decimetre, hecto, litre> {};
struct kilolitre : prefixed_alias_unit<cubic_metre, kilo, litre> {};
struct megalitre : prefixed_alias_unit<cubic_decametre, mega, litre> {};
struct gigalitre : prefixed_alias_unit<cubic_hectometre, giga, litre> {};
struct teralitre : prefixed_alias_unit<cubic_kilometre, tera, litre> {};
struct petalitre : prefixed_unit<petalitre, peta, litre> {};
struct exalitre : prefixed_unit<exalitre, exa, litre> {};
struct zettalitre : prefixed_alias_unit<cubic_megametre, zetta, litre> {};
struct yottalitre : prefixed_unit<yottalitre, yotta, litre> {};

template<UnitOf<dim_volume> U, Representation Rep = double>
using volume = quantity<dim_volume, U, Rep>;

#ifdef UNITS_LITERALS

inline namespace literals {

// m3
constexpr auto operator"" _q_m3(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<cubic_metre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_m3(long double l) { return volume<cubic_metre, long double>(l); }

// ym3
constexpr auto operator"" _q_ym3(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<cubic_yoctometre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_ym3(long double l) { return volume<cubic_yoctometre, long double>(l); }

// zm3
constexpr auto operator"" _q_zm3(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<cubic_zeptometre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_zm3(long double l) { return volume<cubic_zeptometre, long double>(l); }

// am3
constexpr auto operator"" _q_am3(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<cubic_attometre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_am3(long double l) { return volume<cubic_attometre, long double>(l); }

// fm3
constexpr auto operator"" _q_fm3(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<cubic_femtometre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_fm3(long double l) { return volume<cubic_femtometre, long double>(l); }

// pm3
constexpr auto operator"" _q_pm3(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<cubic_picometre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_pm3(long double l) { return volume<cubic_picometre, long double>(l); }

// nm3
constexpr auto operator"" _q_nm3(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<cubic_nanometre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_nm3(long double l) { return volume<cubic_nanometre, long double>(l); }

// um3
constexpr auto operator"" _q_um3(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<cubic_micrometre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_um3(long double l) { return volume<cubic_micrometre, long double>(l); }

// mm3
constexpr auto operator"" _q_mm3(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<cubic_millimetre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_mm3(long double l) { return volume<cubic_millimetre, long double>(l); }

// cm3
constexpr auto operator"" _q_cm3(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<cubic_centimetre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_cm3(long double l) { return volume<cubic_centimetre, long double>(l); }

// dm3
constexpr auto operator"" _q_dm3(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<cubic_decimetre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_dm3(long double l) { return volume<cubic_decimetre, long double>(l); }

// dam3
constexpr auto operator"" _q_dam3(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<cubic_decametre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_dam3(long double l) { return volume<cubic_decametre, long double>(l); }

// hm3
constexpr auto operator"" _q_hm3(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<cubic_hectometre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_hm3(long double l) { return volume<cubic_hectometre, long double>(l); }

// km3
constexpr auto operator"" _q_km3(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<cubic_kilometre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_km3(long double l) { return volume<cubic_kilometre, long double>(l); }

// Mm3
constexpr auto operator"" _q_Mm3(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<cubic_megametre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Mm3(long double l) { return volume<cubic_megametre, long double>(l); }

// Gm3
constexpr auto operator"" _q_Gm3(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<cubic_gigametre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Gm3(long double l) { return volume<cubic_gigametre, long double>(l); }

// Tm3
constexpr auto operator"" _q_Tm3(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<cubic_terametre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Tm3(long double l) { return volume<cubic_terametre, long double>(l); }

// Pm3
constexpr auto operator"" _q_Pm3(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<cubic_petametre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Pm3(long double l) { return volume<cubic_petametre, long double>(l); }

// Em3
constexpr auto operator"" _q_Em3(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<cubic_exametre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Em3(long double l) { return volume<cubic_exametre, long double>(l); }

// Zm3
constexpr auto operator"" _q_Zm3(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<cubic_zettametre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Zm3(long double l) { return volume<cubic_zettametre, long double>(l); }

// Ym3
constexpr auto operator"" _q_Ym3(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<cubic_yottametre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Ym3(long double l) { return volume<cubic_yottametre, long double>(l); }

// l
constexpr auto operator"" _q_l(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<litre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_l(long double l) { return volume<litre, long double>(l); }

// yl
constexpr auto operator"" _q_yl(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<yoctolitre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_yl(long double l) { return volume<yoctolitre, long double>(l); }

// zl
constexpr auto operator"" _q_zl(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<zeptolitre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_zl(long double l) { return volume<zeptolitre, long double>(l); }

// al
constexpr auto operator"" _q_al(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<attolitre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_al(long double l) { return volume<attolitre, long double>(l); }

// fl
constexpr auto operator"" _q_fl(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<femtolitre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_fl(long double l) { return volume<femtolitre, long double>(l); }

// pl
constexpr auto operator"" _q_pl(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<picolitre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_pl(long double l) { return volume<picolitre, long double>(l); }

// nl
constexpr auto operator"" _q_nl(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<nanolitre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_nl(long double l) { return volume<nanolitre, long double>(l); }

// ul
constexpr auto operator"" _q_ul(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<microlitre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_ul(long double l) { return volume<microlitre, long double>(l); }

// ml
constexpr auto operator"" _q_ml(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<millilitre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_ml(long double l) { return volume<millilitre, long double>(l); }

// cl
constexpr auto operator"" _q_cl(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<centilitre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_cl(long double l) { return volume<centilitre, long double>(l); }

// dl
constexpr auto operator"" _q_dl(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<decilitre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_dl(long double l) { return volume<decilitre, long double>(l); }

// dal
constexpr auto operator"" _q_dal(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<decalitre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_dal(long double l) { return volume<decalitre, long double>(l); }

// hl
constexpr auto operator"" _q_hl(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<hectolitre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_hl(long double l) { return volume<hectolitre, long double>(l); }

// kl
constexpr auto operator"" _q_kl(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<kilolitre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_kl(long double l) { return volume<kilolitre, long double>(l); }

// Ml
constexpr auto operator"" _q_Ml(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<megalitre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Ml(long double l) { return volume<megalitre, long double>(l); }

// Gl
constexpr auto operator"" _q_Gl(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<gigalitre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Gl(long double l) { return volume<gigalitre, long double>(l); }

// Tl
constexpr auto operator"" _q_Tl(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<teralitre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Tl(long double l) { return volume<teralitre, long double>(l); }

// Pl
constexpr auto operator"" _q_Pl(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<petalitre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Pl(long double l) { return volume<petalitre, long double>(l); }

// El
constexpr auto operator"" _q_El(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<exalitre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_El(long double l) { return volume<exalitre, long double>(l); }

// Zl
constexpr auto operator"" _q_Zl(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<zettalitre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Zl(long double l) { return volume<zettalitre, long double>(l); }

// Yl
constexpr auto operator"" _q_Yl(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return volume<yottalitre, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Yl(long double l) { return volume<yottalitre, long double>(l); }

}  // namespace literals

#endif // UNITS_LITERALS

#ifdef UNITS_REFERENCES

namespace volume_references {

inline constexpr auto m3 = reference<dim_volume, cubic_metre>{};
inline constexpr auto ym3 = reference<dim_volume, cubic_yoctometre>{};
inline constexpr auto zm3 = reference<dim_volume, cubic_zeptometre>{};
inline constexpr auto am3 = reference<dim_volume, cubic_attometre>{};
inline constexpr auto fm3 = reference<dim_volume, cubic_femtometre>{};
inline constexpr auto pm3 = reference<dim_volume, cubic_picometre>{};
inline constexpr auto nm3 = reference<dim_volume, cubic_nanometre>{};
inline constexpr auto um3 = reference<dim_volume, cubic_micrometre>{};
inline constexpr auto mm3 = reference<dim_volume, cubic_millimetre>{};
inline constexpr auto cm3 = reference<dim_volume, cubic_centimetre>{};
inline constexpr auto dm3 = reference<dim_volume, cubic_decimetre>{};
inline constexpr auto dam3 = reference<dim_volume, cubic_decametre>{};
inline constexpr auto hm3 = reference<dim_volume, cubic_hectometre>{};
inline constexpr auto km3 = reference<dim_volume, cubic_kilometre>{};
inline constexpr auto Mm3 = reference<dim_volume, cubic_megametre>{};
inline constexpr auto Gm3 = reference<dim_volume, cubic_gigametre>{};
inline constexpr auto Tm3 = reference<dim_volume, cubic_terametre>{};
inline constexpr auto Pm3 = reference<dim_volume, cubic_petametre>{};
inline constexpr auto Em3 = reference<dim_volume, cubic_exametre>{};
inline constexpr auto Zm3 = reference<dim_volume, cubic_zettametre>{};
inline constexpr auto Ym3 = reference<dim_volume, cubic_yottametre>{};
inline constexpr auto l = reference<dim_volume, litre>{};
inline constexpr auto yl = reference<dim_volume, yoctolitre>{};
inline constexpr auto zl = reference<dim_volume, zeptolitre>{};
inline constexpr auto al = reference<dim_volume, attolitre>{};
inline constexpr auto fl = reference<dim_volume, femtolitre>{};
inline constexpr auto pl = reference<dim_volume, picolitre>{};
inline constexpr auto nl = reference<dim_volume, nanolitre>{};
inline constexpr auto ul = reference<dim_volume, microlitre>{};
inline constexpr auto ml = reference<dim_volume, millilitre>{};
inline constexpr auto cl = reference<dim_volume, centilitre>{};
inline constexpr auto dl = reference<dim_volume, decilitre>{};
inline constexpr auto dal = reference<dim_volume, decalitre>{};
inline constexpr auto hl = reference<dim_volume, hectolitre>{};
inline constexpr auto kl = reference<dim_volume, kilolitre>{};
inline constexpr auto Ml = reference<dim_volume, megalitre>{};
inline constexpr auto Gl = reference<dim_volume, gigalitre>{};
inline constexpr auto Tl = reference<dim_volume, teralitre>{};
inline constexpr auto Pl = reference<dim_volume, petalitre>{};
inline constexpr auto El = reference<dim_volume, exalitre>{};
inline constexpr auto Zl = reference<dim_volume, zettalitre>{};
inline constexpr auto Yl = reference<dim_volume, yottalitre>{};

}  // namespace volume_references

namespace references {

using namespace volume_references;

}  // namespace references

#endif // UNITS_REFERENCES

}  // namespace units::isq::si
