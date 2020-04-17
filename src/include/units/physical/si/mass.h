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

#include <units/physical/dimensions.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::si {

struct gram : named_unit<gram, "g", prefix> {};
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

struct dalton : named_scaled_unit<dalton, "Da", no_prefix, ratio<16'605'390'666'050, 10'000'000'000'000, -27>, kilogram> {};

struct dim_mass : physical::dim_mass<kilogram> {};

template<Unit U, Scalar Rep = double>
using mass = quantity<dim_mass, U, Rep>;

inline namespace literals {

// g
constexpr auto operator"" q_g(unsigned long long l) { return mass<gram, std::int64_t>(l); }
constexpr auto operator"" q_g(long double l) { return mass<gram, long double>(l); }

// yg
constexpr auto operator"" q_yg(unsigned long long l) { return mass<yoctogram, std::int64_t>(l); }
constexpr auto operator"" q_yg(long double l) { return mass<yoctogram, long double>(l); }

// zg
constexpr auto operator"" q_zg(unsigned long long l) { return mass<zeptogram, std::int64_t>(l); }
constexpr auto operator"" q_zg(long double l) { return mass<zeptogram, long double>(l); }

// ag
constexpr auto operator"" q_ag(unsigned long long l) { return mass<attogram, std::int64_t>(l); }
constexpr auto operator"" q_ag(long double l) { return mass<attogram, long double>(l); }

// fg
constexpr auto operator"" q_fg(unsigned long long l) { return mass<femtogram, std::int64_t>(l); }
constexpr auto operator"" q_fg(long double l) { return mass<femtogram, long double>(l); }

// pg
constexpr auto operator"" q_pg(unsigned long long l) { return mass<picogram, std::int64_t>(l); }
constexpr auto operator"" q_pg(long double l) { return mass<picogram, long double>(l); }

// ng
constexpr auto operator"" q_ng(unsigned long long l) { return mass<nanogram, std::int64_t>(l); }
constexpr auto operator"" q_ng(long double l) { return mass<nanogram, long double>(l); }

// ug
constexpr auto operator"" q_ug(unsigned long long l) { return mass<microgram, std::int64_t>(l); }
constexpr auto operator"" q_ug(long double l) { return mass<microgram, long double>(l); }

// mg
constexpr auto operator"" q_mg(unsigned long long l) { return mass<milligram, std::int64_t>(l); }
constexpr auto operator"" q_mg(long double l) { return mass<milligram, long double>(l); }

// cg
constexpr auto operator"" q_cg(unsigned long long l) { return mass<centigram, std::int64_t>(l); }
constexpr auto operator"" q_cg(long double l) { return mass<centigram, long double>(l); }

// dg
constexpr auto operator"" q_dg(unsigned long long l) { return mass<decigram, std::int64_t>(l); }
constexpr auto operator"" q_dg(long double l) { return mass<decigram, long double>(l); }

// dag
constexpr auto operator"" q_dag(unsigned long long l) { return mass<decagram, std::int64_t>(l); }
constexpr auto operator"" q_dag(long double l) { return mass<decagram, long double>(l); }

// hg
constexpr auto operator"" q_hg(unsigned long long l) { return mass<hectogram, std::int64_t>(l); }
constexpr auto operator"" q_hg(long double l) { return mass<hectogram, long double>(l); }

// kg
constexpr auto operator"" q_kg(unsigned long long l) { return mass<kilogram, std::int64_t>(l); }
constexpr auto operator"" q_kg(long double l) { return mass<kilogram, long double>(l); }

// Mg
constexpr auto operator"" q_Mg(unsigned long long l) { return mass<megagram, std::int64_t>(l); }
constexpr auto operator"" q_Mg(long double l) { return mass<megagram, long double>(l); }

// Gg
constexpr auto operator"" q_Gg(unsigned long long l) { return mass<gigagram, std::int64_t>(l); }
constexpr auto operator"" q_Gg(long double l) { return mass<gigagram, long double>(l); }

// Tg
constexpr auto operator"" q_Tg(unsigned long long l) { return mass<teragram, std::int64_t>(l); }
constexpr auto operator"" q_Tg(long double l) { return mass<teragram, long double>(l); }

// Pg
constexpr auto operator"" q_Pg(unsigned long long l) { return mass<petagram, std::int64_t>(l); }
constexpr auto operator"" q_Pg(long double l) { return mass<petagram, long double>(l); }

// Eg
constexpr auto operator"" q_Eg(unsigned long long l) { return mass<exagram, std::int64_t>(l); }
constexpr auto operator"" q_Eg(long double l) { return mass<exagram, long double>(l); }

// Zg
constexpr auto operator"" q_Zg(unsigned long long l) { return mass<zettagram, std::int64_t>(l); }
constexpr auto operator"" q_Zg(long double l) { return mass<zettagram, long double>(l); }

// Yg
constexpr auto operator"" q_Yg(unsigned long long l) { return mass<yottagram, std::int64_t>(l); }
constexpr auto operator"" q_Yg(long double l) { return mass<yottagram, long double>(l); }

// t
constexpr auto operator"" q_t(unsigned long long l) { return mass<tonne, std::int64_t>(l); }
constexpr auto operator"" q_t(long double l) { return mass<tonne, long double>(l); }

// yt
constexpr auto operator"" q_yt(unsigned long long l) { return mass<yoctotonne, std::int64_t>(l); }
constexpr auto operator"" q_yt(long double l) { return mass<yoctotonne, long double>(l); }

// zt
constexpr auto operator"" q_zt(unsigned long long l) { return mass<zeptotonne, std::int64_t>(l); }
constexpr auto operator"" q_zt(long double l) { return mass<zeptotonne, long double>(l); }

// at
constexpr auto operator"" q_at(unsigned long long l) { return mass<attotonne, std::int64_t>(l); }
constexpr auto operator"" q_at(long double l) { return mass<attotonne, long double>(l); }

// ft
/*constexpr auto operator"" q_ft(unsigned long long l) { return mass<femtotonne, std::int64_t>(l); }
constexpr auto operator"" q_ft(long double l) { return mass<femtotonne, long double>(l); }*/

// pt
constexpr auto operator"" q_pt(unsigned long long l) { return mass<picotonne, std::int64_t>(l); }
constexpr auto operator"" q_pt(long double l) { return mass<picotonne, long double>(l); }

// nt
constexpr auto operator"" q_nt(unsigned long long l) { return mass<nanotonne, std::int64_t>(l); }
constexpr auto operator"" q_nt(long double l) { return mass<nanotonne, long double>(l); }

// ut
constexpr auto operator"" q_ut(unsigned long long l) { return mass<microtonne, std::int64_t>(l); }
constexpr auto operator"" q_ut(long double l) { return mass<microtonne, long double>(l); }

// mt
constexpr auto operator"" q_mt(unsigned long long l) { return mass<millitonne, std::int64_t>(l); }
constexpr auto operator"" q_mt(long double l) { return mass<millitonne, long double>(l); }

// ct
constexpr auto operator"" q_ct(unsigned long long l) { return mass<centitonne, std::int64_t>(l); }
constexpr auto operator"" q_ct(long double l) { return mass<centitonne, long double>(l); }

// dt
constexpr auto operator"" q_dt(unsigned long long l) { return mass<decitonne, std::int64_t>(l); }
constexpr auto operator"" q_dt(long double l) { return mass<decitonne, long double>(l); }

// dat
constexpr auto operator"" q_dat(unsigned long long l) { return mass<decatonne, std::int64_t>(l); }
constexpr auto operator"" q_dat(long double l) { return mass<decatonne, long double>(l); }

// ht
constexpr auto operator"" q_ht(unsigned long long l) { return mass<hectotonne, std::int64_t>(l); }
constexpr auto operator"" q_ht(long double l) { return mass<hectotonne, long double>(l); }

// kt
constexpr auto operator"" q_kt(unsigned long long l) { return mass<kilotonne, std::int64_t>(l); }
constexpr auto operator"" q_kt(long double l) { return mass<kilotonne, long double>(l); }

// Mt
constexpr auto operator"" q_Mt(unsigned long long l) { return mass<megatonne, std::int64_t>(l); }
constexpr auto operator"" q_Mt(long double l) { return mass<megatonne, long double>(l); }

// Gt
constexpr auto operator"" q_Gt(unsigned long long l) { return mass<gigatonne, std::int64_t>(l); }
constexpr auto operator"" q_Gt(long double l) { return mass<gigatonne, long double>(l); }

// Tt
constexpr auto operator"" q_Tt(unsigned long long l) { return mass<teratonne, std::int64_t>(l); }
constexpr auto operator"" q_Tt(long double l) { return mass<teratonne, long double>(l); }

// Pt
constexpr auto operator"" q_Pt(unsigned long long l) { return mass<petatonne, std::int64_t>(l); }
constexpr auto operator"" q_Pt(long double l) { return mass<petatonne, long double>(l); }

// Et
constexpr auto operator"" q_Et(unsigned long long l) { return mass<exatonne, std::int64_t>(l); }
constexpr auto operator"" q_Et(long double l) { return mass<exatonne, long double>(l); }

// Zt
constexpr auto operator"" q_Zt(unsigned long long l) { return mass<zettatonne, std::int64_t>(l); }
constexpr auto operator"" q_Zt(long double l) { return mass<zettatonne, long double>(l); }

// Yt
constexpr auto operator"" q_Yt(unsigned long long l) { return mass<yottatonne, std::int64_t>(l); }
constexpr auto operator"" q_Yt(long double l) { return mass<yottatonne, long double>(l); }

// Da
constexpr auto operator"" q_Da(unsigned long long l) { return mass<dalton, std::int64_t>(l); }
constexpr auto operator"" q_Da(long double l) { return mass<dalton, long double>(l); }

}  // namespace literals

}  // namespace units::si
