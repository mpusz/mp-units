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

#include <units/physical/dimensions/capacitance.h>
#include <units/physical/si/derived/electric_charge.h>
#include <units/physical/si/derived/voltage.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::physical::si {

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

template<UnitOf<dim_capacitance> U, QuantityValue Rep = double>
using capacitance = quantity<dim_capacitance, U, Rep>;

inline namespace literals {

// F
constexpr auto operator"" _q_F(unsigned long long l) { return capacitance<farad, std::int64_t>(l); }
constexpr auto operator"" _q_F(long double l) { return capacitance<farad, long double>(l); }

// yF
constexpr auto operator"" _q_yF(unsigned long long l) { return capacitance<yoctofarad, std::int64_t>(l); }
constexpr auto operator"" _q_yF(long double l) { return capacitance<yoctofarad, long double>(l); }

// zF
constexpr auto operator"" _q_zF(unsigned long long l) { return capacitance<zeptofarad, std::int64_t>(l); }
constexpr auto operator"" _q_zF(long double l) { return capacitance<zeptofarad, long double>(l); }

// aF
constexpr auto operator"" _q_aF(unsigned long long l) { return capacitance<attofarad, std::int64_t>(l); }
constexpr auto operator"" _q_aF(long double l) { return capacitance<attofarad, long double>(l); }

// fF
constexpr auto operator"" _q_fF(unsigned long long l) { return capacitance<femtofarad, std::int64_t>(l); }
constexpr auto operator"" _q_fF(long double l) { return capacitance<femtofarad, long double>(l); }

// pF
constexpr auto operator"" _q_pF(unsigned long long l) { return capacitance<picofarad, std::int64_t>(l); }
constexpr auto operator"" _q_pF(long double l) { return capacitance<picofarad, long double>(l); }

// nF
constexpr auto operator"" _q_nF(unsigned long long l) { return capacitance<nanofarad, std::int64_t>(l); }
constexpr auto operator"" _q_nF(long double l) { return capacitance<nanofarad, long double>(l); }

// uF
constexpr auto operator"" _q_uF(unsigned long long l) { return capacitance<microfarad, std::int64_t>(l); }
constexpr auto operator"" _q_uF(long double l) { return capacitance<microfarad, long double>(l); }

// mF
constexpr auto operator"" _q_mF(unsigned long long l) { return capacitance<millifarad, std::int64_t>(l); }
constexpr auto operator"" _q_mF(long double l) { return capacitance<millifarad, long double>(l); }

// cF
constexpr auto operator"" _q_cF(unsigned long long l) { return capacitance<centifarad, std::int64_t>(l); }
constexpr auto operator"" _q_cF(long double l) { return capacitance<centifarad, long double>(l); }

// dF
constexpr auto operator"" _q_dF(unsigned long long l) { return capacitance<decifarad, std::int64_t>(l); }
constexpr auto operator"" _q_dF(long double l) { return capacitance<decifarad, long double>(l); }

// daF
constexpr auto operator"" _q_daF(unsigned long long l) { return capacitance<decafarad, std::int64_t>(l); }
constexpr auto operator"" _q_daF(long double l) { return capacitance<decafarad, long double>(l); }

// hF
constexpr auto operator"" _q_hF(unsigned long long l) { return capacitance<hectofarad, std::int64_t>(l); }
constexpr auto operator"" _q_hF(long double l) { return capacitance<hectofarad, long double>(l); }

// kF
constexpr auto operator"" _q_kF(unsigned long long l) { return capacitance<kilofarad, std::int64_t>(l); }
constexpr auto operator"" _q_kF(long double l) { return capacitance<kilofarad, long double>(l); }

// MF
constexpr auto operator"" _q_MF(unsigned long long l) { return capacitance<megafarad, std::int64_t>(l); }
constexpr auto operator"" _q_MF(long double l) { return capacitance<megafarad, long double>(l); }

// GF
constexpr auto operator"" _q_GF(unsigned long long l) { return capacitance<gigafarad, std::int64_t>(l); }
constexpr auto operator"" _q_GF(long double l) { return capacitance<gigafarad, long double>(l); }

// TF
constexpr auto operator"" _q_TF(unsigned long long l) { return capacitance<terafarad, std::int64_t>(l); }
constexpr auto operator"" _q_TF(long double l) { return capacitance<terafarad, long double>(l); }

// PF
constexpr auto operator"" _q_PF(unsigned long long l) { return capacitance<petafarad, std::int64_t>(l); }
constexpr auto operator"" _q_PF(long double l) { return capacitance<petafarad, long double>(l); }

// EF
constexpr auto operator"" _q_EF(unsigned long long l) { return capacitance<exafarad, std::int64_t>(l); }
constexpr auto operator"" _q_EF(long double l) { return capacitance<exafarad, long double>(l); }

// ZF
constexpr auto operator"" _q_ZF(unsigned long long l) { return capacitance<zettafarad, std::int64_t>(l); }
constexpr auto operator"" _q_ZF(long double l) { return capacitance<zettafarad, long double>(l); }

// YF
constexpr auto operator"" _q_YF(unsigned long long l) { return capacitance<yottafarad, std::int64_t>(l); }
constexpr auto operator"" _q_YF(long double l) { return capacitance<yottafarad, long double>(l); }

}  // namespace literals

namespace unit_constants {

inline constexpr auto F = capacitance<farad, detail::one_rep>{};
inline constexpr auto yF = capacitance<yoctofarad, detail::one_rep>{};
inline constexpr auto zF = capacitance<zeptofarad, detail::one_rep>{};
inline constexpr auto aF = capacitance<attofarad, detail::one_rep>{};
inline constexpr auto fF = capacitance<femtofarad, detail::one_rep>{};
inline constexpr auto pF = capacitance<picofarad, detail::one_rep>{};
inline constexpr auto nF = capacitance<nanofarad, detail::one_rep>{};
inline constexpr auto uF = capacitance<microfarad, detail::one_rep>{};
inline constexpr auto mF = capacitance<millifarad, detail::one_rep>{};
inline constexpr auto cF = capacitance<centifarad, detail::one_rep>{};
inline constexpr auto dF = capacitance<decifarad, detail::one_rep>{};
inline constexpr auto daF = capacitance<decafarad, detail::one_rep>{};
inline constexpr auto hF = capacitance<hectofarad, detail::one_rep>{};
inline constexpr auto kF = capacitance<kilofarad, detail::one_rep>{};
inline constexpr auto MF = capacitance<megafarad, detail::one_rep>{};
inline constexpr auto GF = capacitance<gigafarad, detail::one_rep>{};
inline constexpr auto TF = capacitance<terafarad, detail::one_rep>{};
inline constexpr auto PF = capacitance<petafarad, detail::one_rep>{};
inline constexpr auto EF = capacitance<exafarad, detail::one_rep>{};
inline constexpr auto ZF = capacitance<zettafarad, detail::one_rep>{};
inline constexpr auto YF = capacitance<yottafarad, detail::one_rep>{};

}  // namespace unit_constants

}  // namespace units::physical::si
