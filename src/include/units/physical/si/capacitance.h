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
#include <units/physical/si/electric_charge.h>
#include <units/physical/si/voltage.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::si {

struct farad : named_unit<farad, "F", prefix> {};
struct yoctofarad : prefixed_unit<yoctofarad, yocto, farad> {};
struct zeptofarad : prefixed_unit<zeptofarad, zepto, farad> {};
struct attofarad : prefixed_unit<attofarad, atto, farad> {};
struct femtofarad : prefixed_unit<femtofarad, femto, farad> {};
struct picofarad : prefixed_unit<picofarad, pico, farad> {};
struct nanofarad : prefixed_unit<nanofarad, nano, farad> {};
struct microfarad : prefixed_unit<microfarad, micro, farad> {};
struct millifarad : prefixed_unit<millifarad, milli, farad> {};
struct centifarad : prefixed_unit<centifarad, centi, farad> {};
struct decifarad : prefixed_unit<decifarad, deci, farad> {};
struct decafarad : prefixed_unit<decafarad, deca, farad> {};
struct hectofarad : prefixed_unit<hectofarad, hecto, farad> {};
struct kilofarad : prefixed_unit<kilofarad, kilo, farad> {};
struct megafarad : prefixed_unit<megafarad, mega, farad> {};
struct gigafarad : prefixed_unit<gigafarad, giga, farad> {};
struct terafarad : prefixed_unit<terafarad, tera, farad> {};
struct petafarad : prefixed_unit<petafarad, peta, farad> {};
struct exafarad : prefixed_unit<exafarad, exa, farad> {};
struct zettafarad : prefixed_unit<zettafarad, zetta, farad> {};
struct yottafarad : prefixed_unit<yottafarad, yotta, farad> {};

struct dim_capacitance : physical::dim_capacitance<dim_capacitance, farad, dim_electric_charge, dim_voltage> {};

template<Unit U, Scalar Rep = double>
using capacitance = quantity<dim_capacitance, U, Rep>;

inline namespace literals {

// F
constexpr auto operator"" q_F(unsigned long long l) { return capacitance<farad, std::int64_t>(l); }
constexpr auto operator"" q_F(long double l) { return capacitance<farad, long double>(l); }

// yF
constexpr auto operator"" q_yF(unsigned long long l) { return capacitance<yoctofarad, std::int64_t>(l); }
constexpr auto operator"" q_yF(long double l) { return capacitance<yoctofarad, long double>(l); }

// zF
constexpr auto operator"" q_zF(unsigned long long l) { return capacitance<zeptofarad, std::int64_t>(l); }
constexpr auto operator"" q_zF(long double l) { return capacitance<zeptofarad, long double>(l); }

// aF
constexpr auto operator"" q_aF(unsigned long long l) { return capacitance<attofarad, std::int64_t>(l); }
constexpr auto operator"" q_aF(long double l) { return capacitance<attofarad, long double>(l); }

// fF
constexpr auto operator"" q_fF(unsigned long long l) { return capacitance<femtofarad, std::int64_t>(l); }
constexpr auto operator"" q_fF(long double l) { return capacitance<femtofarad, long double>(l); }

// pF
constexpr auto operator"" q_pF(unsigned long long l) { return capacitance<picofarad, std::int64_t>(l); }
constexpr auto operator"" q_pF(long double l) { return capacitance<picofarad, long double>(l); }

// nF
constexpr auto operator"" q_nF(unsigned long long l) { return capacitance<nanofarad, std::int64_t>(l); }
constexpr auto operator"" q_nF(long double l) { return capacitance<nanofarad, long double>(l); }

// uF
constexpr auto operator"" q_uF(unsigned long long l) { return capacitance<microfarad, std::int64_t>(l); }
constexpr auto operator"" q_uF(long double l) { return capacitance<microfarad, long double>(l); }

// mF
constexpr auto operator"" q_mF(unsigned long long l) { return capacitance<millifarad, std::int64_t>(l); }
constexpr auto operator"" q_mF(long double l) { return capacitance<millifarad, long double>(l); }

// cF
constexpr auto operator"" q_cF(unsigned long long l) { return capacitance<centifarad, std::int64_t>(l); }
constexpr auto operator"" q_cF(long double l) { return capacitance<centifarad, long double>(l); }

// dF
constexpr auto operator"" q_dF(unsigned long long l) { return capacitance<decifarad, std::int64_t>(l); }
constexpr auto operator"" q_dF(long double l) { return capacitance<decifarad, long double>(l); }

// daF
constexpr auto operator"" q_daF(unsigned long long l) { return capacitance<decafarad, std::int64_t>(l); }
constexpr auto operator"" q_daF(long double l) { return capacitance<decafarad, long double>(l); }

// hF
constexpr auto operator"" q_hF(unsigned long long l) { return capacitance<hectofarad, std::int64_t>(l); }
constexpr auto operator"" q_hF(long double l) { return capacitance<hectofarad, long double>(l); }

// kF
constexpr auto operator"" q_kF(unsigned long long l) { return capacitance<kilofarad, std::int64_t>(l); }
constexpr auto operator"" q_kF(long double l) { return capacitance<kilofarad, long double>(l); }

// MF
constexpr auto operator"" q_MF(unsigned long long l) { return capacitance<megafarad, std::int64_t>(l); }
constexpr auto operator"" q_MF(long double l) { return capacitance<megafarad, long double>(l); }

// GF
constexpr auto operator"" q_GF(unsigned long long l) { return capacitance<gigafarad, std::int64_t>(l); }
constexpr auto operator"" q_GF(long double l) { return capacitance<gigafarad, long double>(l); }

// TF
constexpr auto operator"" q_TF(unsigned long long l) { return capacitance<terafarad, std::int64_t>(l); }
constexpr auto operator"" q_TF(long double l) { return capacitance<terafarad, long double>(l); }

// PF
constexpr auto operator"" q_PF(unsigned long long l) { return capacitance<petafarad, std::int64_t>(l); }
constexpr auto operator"" q_PF(long double l) { return capacitance<petafarad, long double>(l); }

// EF
constexpr auto operator"" q_EF(unsigned long long l) { return capacitance<exafarad, std::int64_t>(l); }
constexpr auto operator"" q_EF(long double l) { return capacitance<exafarad, long double>(l); }

// ZF
constexpr auto operator"" q_ZF(unsigned long long l) { return capacitance<zettafarad, std::int64_t>(l); }
constexpr auto operator"" q_ZF(long double l) { return capacitance<zettafarad, long double>(l); }

// YF
constexpr auto operator"" q_YF(unsigned long long l) { return capacitance<yottafarad, std::int64_t>(l); }
constexpr auto operator"" q_YF(long double l) { return capacitance<yottafarad, long double>(l); }

}  // namespace literals

}  // namespace units::si
