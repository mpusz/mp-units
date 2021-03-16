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

#include <units/physical/dimensions/volume.h>
#include <units/physical/si/length.h>
#include <units/quantity.h>

namespace units::physical::si {

struct cubic_metre : unit<cubic_metre> {};
struct dim_volume : physical::dim_volume<dim_volume, cubic_metre, dim_length> {};

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

template<UnitOf<dim_volume> U, QuantityValue Rep = double>
using volume = quantity<dim_volume, U, Rep>;

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

namespace unit_constants {

inline constexpr auto m3 = volume<cubic_metre, one_rep>{};
inline constexpr auto ym3 = volume<cubic_yoctometre, one_rep>{};
inline constexpr auto zm3 = volume<cubic_zeptometre, one_rep>{};
inline constexpr auto am3 = volume<cubic_attometre, one_rep>{};
inline constexpr auto fm3 = volume<cubic_femtometre, one_rep>{};
inline constexpr auto pm3 = volume<cubic_picometre, one_rep>{};
inline constexpr auto nm3 = volume<cubic_nanometre, one_rep>{};
inline constexpr auto um3 = volume<cubic_micrometre, one_rep>{};
inline constexpr auto mm3 = volume<cubic_millimetre, one_rep>{};
inline constexpr auto cm3 = volume<cubic_centimetre, one_rep>{};
inline constexpr auto dm3 = volume<cubic_decimetre, one_rep>{};
inline constexpr auto dam3 = volume<cubic_decametre, one_rep>{};
inline constexpr auto hm3 = volume<cubic_hectometre, one_rep>{};
inline constexpr auto km3 = volume<cubic_kilometre, one_rep>{};
inline constexpr auto Mm3 = volume<cubic_megametre, one_rep>{};
inline constexpr auto Gm3 = volume<cubic_gigametre, one_rep>{};
inline constexpr auto Tm3 = volume<cubic_terametre, one_rep>{};
inline constexpr auto Pm3 = volume<cubic_petametre, one_rep>{};
inline constexpr auto Em3 = volume<cubic_exametre, one_rep>{};
inline constexpr auto Zm3 = volume<cubic_zettametre, one_rep>{};
inline constexpr auto Ym3 = volume<cubic_yottametre, one_rep>{};
inline constexpr auto l = volume<litre, one_rep>{};
inline constexpr auto yl = volume<yoctolitre, one_rep>{};
inline constexpr auto zl = volume<zeptolitre, one_rep>{};
inline constexpr auto al = volume<attolitre, one_rep>{};
inline constexpr auto fl = volume<femtolitre, one_rep>{};
inline constexpr auto pl = volume<picolitre, one_rep>{};
inline constexpr auto nl = volume<nanolitre, one_rep>{};
inline constexpr auto ul = volume<microlitre, one_rep>{};
inline constexpr auto ml = volume<millilitre, one_rep>{};
inline constexpr auto cl = volume<centilitre, one_rep>{};
inline constexpr auto dl = volume<decilitre, one_rep>{};
inline constexpr auto dal = volume<decalitre, one_rep>{};
inline constexpr auto hl = volume<hectolitre, one_rep>{};
inline constexpr auto kl = volume<kilolitre, one_rep>{};
inline constexpr auto Ml = volume<megalitre, one_rep>{};
inline constexpr auto Gl = volume<gigalitre, one_rep>{};
inline constexpr auto Tl = volume<teralitre, one_rep>{};
inline constexpr auto Pl = volume<petalitre, one_rep>{};
inline constexpr auto El = volume<exalitre, one_rep>{};
inline constexpr auto Zl = volume<zettalitre, one_rep>{};
inline constexpr auto Yl = volume<yottalitre, one_rep>{};

}  // namespace unit_constants

}  // namespace units::physical::si
