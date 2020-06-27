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

namespace units::physical::si {

struct metre : named_unit<metre, "m", prefix> {};
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

struct astronomical_unit : named_scaled_unit<astronomical_unit, "au", no_prefix, ratio(149'597'870'700), metre> {};

struct dim_length : physical::dim_length<metre> {};

template<Unit U, Scalar Rep = double>
using length = quantity<dim_length, U, Rep>;

inline namespace literals {

// m
constexpr auto operator"" q_m(unsigned long long l) { return length<metre, std::int64_t>(l); }
constexpr auto operator"" q_m(long double l) { return length<metre, long double>(l); }

// ym
constexpr auto operator"" q_ym(unsigned long long l) { return length<yoctometre, std::int64_t>(l); }
constexpr auto operator"" q_ym(long double l) { return length<yoctometre, long double>(l); }

// zm
constexpr auto operator"" q_zm(unsigned long long l) { return length<zeptometre, std::int64_t>(l); }
constexpr auto operator"" q_zm(long double l) { return length<zeptometre, long double>(l); }

// am
constexpr auto operator"" q_am(unsigned long long l) { return length<attometre, std::int64_t>(l); }
constexpr auto operator"" q_am(long double l) { return length<attometre, long double>(l); }

// fm
constexpr auto operator"" q_fm(unsigned long long l) { return length<femtometre, std::int64_t>(l); }
constexpr auto operator"" q_fm(long double l) { return length<femtometre, long double>(l); }

// pm
constexpr auto operator"" q_pm(unsigned long long l) { return length<picometre, std::int64_t>(l); }
constexpr auto operator"" q_pm(long double l) { return length<picometre, long double>(l); }

// nm
constexpr auto operator"" q_nm(unsigned long long l) { return length<nanometre, std::int64_t>(l); }
constexpr auto operator"" q_nm(long double l) { return length<nanometre, long double>(l); }

// um
constexpr auto operator"" q_um(unsigned long long l) { return length<micrometre, std::int64_t>(l); }
constexpr auto operator"" q_um(long double l) { return length<micrometre, long double>(l); }

// mm
constexpr auto operator"" q_mm(unsigned long long l) { return length<millimetre, std::int64_t>(l); }
constexpr auto operator"" q_mm(long double l) { return length<millimetre, long double>(l); }

// cm
constexpr auto operator"" q_cm(unsigned long long l) { return length<centimetre, std::int64_t>(l); }
constexpr auto operator"" q_cm(long double l) { return length<centimetre, long double>(l); }

// dm
constexpr auto operator"" q_dm(unsigned long long l) { return length<decimetre, std::int64_t>(l); }
constexpr auto operator"" q_dm(long double l) { return length<decimetre, long double>(l); }

// dam
constexpr auto operator"" q_dam(unsigned long long l) { return length<decametre, std::int64_t>(l); }
constexpr auto operator"" q_dam(long double l) { return length<decametre, long double>(l); }

// hm
constexpr auto operator"" q_hm(unsigned long long l) { return length<hectometre, std::int64_t>(l); }
constexpr auto operator"" q_hm(long double l) { return length<hectometre, long double>(l); }

// km
constexpr auto operator"" q_km(unsigned long long l) { return length<kilometre, std::int64_t>(l); }
constexpr auto operator"" q_km(long double l) { return length<kilometre, long double>(l); }

// Mm
constexpr auto operator"" q_Mm(unsigned long long l) { return length<megametre, std::int64_t>(l); }
constexpr auto operator"" q_Mm(long double l) { return length<megametre, long double>(l); }

// Gm
constexpr auto operator"" q_Gm(unsigned long long l) { return length<gigametre, std::int64_t>(l); }
constexpr auto operator"" q_Gm(long double l) { return length<gigametre, long double>(l); }

// Tm
constexpr auto operator"" q_Tm(unsigned long long l) { return length<terametre, std::int64_t>(l); }
constexpr auto operator"" q_Tm(long double l) { return length<terametre, long double>(l); }

// Pm
constexpr auto operator"" q_Pm(unsigned long long l) { return length<petametre, std::int64_t>(l); }
constexpr auto operator"" q_Pm(long double l) { return length<petametre, long double>(l); }

// Em
constexpr auto operator"" q_Em(unsigned long long l) { return length<exametre, std::int64_t>(l); }
constexpr auto operator"" q_Em(long double l) { return length<exametre, long double>(l); }

// Zm
constexpr auto operator"" q_Zm(unsigned long long l) { return length<zettametre, std::int64_t>(l); }
constexpr auto operator"" q_Zm(long double l) { return length<zettametre, long double>(l); }

// Ym
constexpr auto operator"" q_Ym(unsigned long long l) { return length<yottametre, std::int64_t>(l); }
constexpr auto operator"" q_Ym(long double l) { return length<yottametre, long double>(l); }

// au
constexpr auto operator"" q_au(unsigned long long l) { return length<astronomical_unit, std::int64_t>(l); }
constexpr auto operator"" q_au(long double l) { return length<astronomical_unit, long double>(l); }

}  // namespace literals

}  // namespace units::physical::si
