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

struct metre : named_unit<metre, "m", prefix> {};
struct millimetre : prefixed_unit<millimetre, milli, metre> {};
struct centimetre : prefixed_unit<centimetre, centi, metre> {};
struct decimetre : prefixed_unit<decimetre, deci, metre> {};
struct hectometre : prefixed_unit<hectometre, hecto, metre> {};
struct kilometre : prefixed_unit<kilometre, kilo, metre> {};
struct femtometre : prefixed_unit<femtometre,femto,metre> {};

struct astronomical_unit : named_scaled_unit<astronomical_unit, "au", no_prefix, ratio<149'597'870'700>, metre> {};
#if 0
struct inch : named_scaled_unit<inch,"in(imp)",no_prefix,ratio<127,50,-2>, metre> {};
struct foot : named_scaled_unit<foot,"ft(imp)",no_prefix,ratio<381,125,-1>, metre> {};
struct yard : named_scaled_unit<yard,"yd(imp)",no_prefix,ratio<1143,125,-1>, metre> {};
#endif
struct foot_us : named_scaled_unit<foot_us,"ft(us)",no_prefix,ratio<1524003,500000,-1>, metre> {};
struct fathom : named_scaled_unit<fathom,"fathom(imp)",no_prefix,ratio<1143,625>, metre> {};
struct fathom_us : named_scaled_unit<fathom_us,"fathom(us)",no_prefix,ratio<457201,250000>,metre> {};
struct chain : named_scaled_unit<chain,"ch(us)",no_prefix,ratio<502921,250000,1>, metre> {};

#if 0
struct thou : named_scaled_unit<thou,"thou",no_prefix,ratio<127,50,-5>,metre> {};
#endif
struct mil : named_scaled_unit<mil,"mil",no_prefix,ratio<127,50,-5>,metre> {};

struct light_year : named_scaled_unit<light_year,"ly(iau)",no_prefix,ratio<946073,100000,15>,metre> {};
struct mile : named_scaled_unit<mile,"mi(imp)",no_prefix,ratio<25146,15625,3>,metre> {};
struct nautical_mile : named_scaled_unit<nautical_mile,"mi(naut)",no_prefix, ratio<463,250,3>,metre> {};
struct parsec : named_scaled_unit<parsec,"pc(iau)",no_prefix,ratio<1542839,500000,16>,metre> {};
struct pica_comp : named_scaled_unit<pica_comp,"pica(comp)",no_prefix,ratio<4233333,1000000,-3>,metre> {};
struct pica_prn : named_scaled_unit<pica_prn,"pica(prn)",no_prefix,ratio<2108759,500000,-3>,metre> {};
struct point_comp : named_scaled_unit<point_comp,"point(comp)",no_prefix,ratio<1763889,500000,-4>,metre> {};
struct point_prn  : named_scaled_unit<point_prn,"point(prn)", no_prefix, ratio<1757299,500000,-4>,metre> {};
struct rod : named_scaled_unit<rod,"rd(us)",no_prefix, ratio<502921,100000>,metre> {};
struct angstrom : named_scaled_unit<angstrom,"angstrom(nru)",no_prefix,ratio<1,1,-10>,metre> {};

struct dim_length : physical::dim_length<metre> {};

template<Unit U, Scalar Rep = double>
using length = quantity<dim_length, U, Rep>;

inline namespace literals {

// m
constexpr auto operator"" m(unsigned long long l) { return length<metre, std::int64_t>(l); }
constexpr auto operator"" m(long double l) { return length<metre, long double>(l); }

// mm
constexpr auto operator"" mm(unsigned long long l) { return length<millimetre, std::int64_t>(l); }
constexpr auto operator"" mm(long double l) { return length<millimetre, long double>(l); }

// fm
constexpr auto operator"" fm(unsigned long long l) { return length<femtometre, std::int64_t>(l); }
constexpr auto operator"" fm(long double l) { return length<femtometre, long double>(l); }

// cm
constexpr auto operator"" cm(unsigned long long l) { return length<centimetre, std::int64_t>(l); }
constexpr auto operator"" cm(long double l) { return length<centimetre, long double>(l); }

// dm
constexpr auto operator"" dm(unsigned long long l) { return length<decimetre, std::int64_t>(l); }
constexpr auto operator"" dm(long double l) { return length<decimetre, long double>(l); }

// hm
constexpr auto operator"" hm(unsigned long long l) { return length<hectometre, std::int64_t>(l); }
constexpr auto operator"" hm(long double l) { return length<hectometre, long double>(l); }

// km
constexpr auto operator"" km(unsigned long long l) { return length<kilometre, std::int64_t>(l); }
constexpr auto operator"" km(long double l) { return length<kilometre, long double>(l); }

// au
constexpr auto operator"" au(unsigned long long l) { return length<astronomical_unit, std::int64_t>(l); }
constexpr auto operator"" au(long double l) { return length<astronomical_unit, long double>(l); }
#if 0
constexpr auto operator"" q_in(unsigned long long l) { return length<inch, std::int64_t>(l); }
constexpr auto operator"" q_in(long double l) { return length<inch, long double>(l); }

constexpr auto operator"" q_ft(unsigned long long l) { return length<foot, std::int64_t>(l); }
constexpr auto operator"" q_ft(long double l) { return length<foot, long double>(l); }

constexpr auto operator"" q_yd(unsigned long long l) { return length<yard, std::int64_t>(l); }
constexpr auto operator"" q_yd(long double l) { return length<yard, long double>(l); }
#endif
constexpr auto operator"" ft_us(unsigned long long l) { return length<foot_us, std::int64_t>(l); }
constexpr auto operator"" ft_us(long double l) { return length<foot_us, long double>(l); }

constexpr auto operator"" fathom(unsigned long long l) { return length<fathom, std::int64_t>(l); }
constexpr auto operator"" fathom(long double l) { return length<fathom, long double>(l); }

constexpr auto operator"" fathom_us(unsigned long long l) { return length<fathom_us, std::int64_t>(l); }
constexpr auto operator"" fathom_us(long double l) { return length<fathom_us, long double>(l); }

constexpr auto operator"" ch(unsigned long long l) { return length<chain, std::int64_t>(l); }
constexpr auto operator"" ch(long double l) { return length<chain, long double>(l); }

#if 0
constexpr auto operator"" thou(unsigned long long l) { return length<thou, std::int64_t>(l); }
constexpr auto operator"" thou(long double l) { return length<thou, long double>(l); }
#endif
constexpr auto operator"" mil(unsigned long long l) { return length<mil, std::int64_t>(l); }
constexpr auto operator"" mil(long double l) { return length<mil, long double>(l); }

constexpr auto operator"" ly(unsigned long long l) { return length<light_year, std::int64_t>(l); }
constexpr auto operator"" ly(long double l) { return length<light_year, long double>(l); }

constexpr auto operator"" q_mi(unsigned long long l) { return length<mile, std::int64_t>(l); }
constexpr auto operator"" q_mi(long double l) { return length<mile, long double>(l); }

constexpr auto operator"" pc(unsigned long long l) { return length<parsec, std::int64_t>(l); }
constexpr auto operator"" pc(long double l) { return length<parsec, long double>(l); }

constexpr auto operator"" mi_naut(unsigned long long l) { return length<nautical_mile, std::int64_t>(l); }
constexpr auto operator"" mi_naut(long double l) { return length<nautical_mile, long double>(l); }

constexpr auto operator"" pica_comp(unsigned long long l) { return length<pica_comp, std::int64_t>(l); }
constexpr auto operator"" pica_comp(long double l) { return length<pica_comp, long double>(l); }

constexpr auto operator"" pica_prn(unsigned long long l) { return length<pica_prn, std::int64_t>(l); }
constexpr auto operator"" pica_prn(long double l) { return length<pica_prn, long double>(l); }

constexpr auto operator"" point_comp(unsigned long long l) { return length<point_comp, std::int64_t>(l); }
constexpr auto operator"" point_comp(long double l) { return length<point_comp, long double>(l); }

constexpr auto operator"" point_prn(unsigned long long l) { return length<point_prn, std::int64_t>(l); }
constexpr auto operator"" point_prn(long double l) { return length<point_prn, long double>(l); }

constexpr auto operator"" rd(unsigned long long l) { return length<rod, std::int64_t>(l); }
constexpr auto operator"" rd(long double l) { return length<rod, long double>(l); }

constexpr auto operator"" angstrom(unsigned long long l) { return length<angstrom, std::int64_t>(l); }
constexpr auto operator"" angstrom(long double l) { return length<angstrom, long double>(l); }

}  // namespace literals

}  // namespace units::si
