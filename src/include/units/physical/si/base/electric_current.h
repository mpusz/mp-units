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

#include <units/one_rep.h>
#include <units/physical/dimensions/electric_current.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::physical::si {

struct ampere : named_unit<ampere, "A", prefix> {};
struct yoctoampere : prefixed_unit<yoctoampere, yocto, ampere> {};
struct zeptoampere : prefixed_unit<zeptoampere, zepto, ampere> {};
struct attoampere : prefixed_unit<attoampere, atto, ampere> {};
struct femtoampere : prefixed_unit<femtoampere, femto, ampere> {};
struct picoampere : prefixed_unit<picoampere, pico, ampere> {};
struct nanoampere : prefixed_unit<nanoampere, nano, ampere> {};
struct microampere : prefixed_unit<microampere, micro, ampere> {};
struct milliampere : prefixed_unit<milliampere, milli, ampere> {};
struct centiampere : prefixed_unit<centiampere, centi, ampere> {};
struct deciampere : prefixed_unit<deciampere, deci, ampere> {};
struct decaampere : prefixed_unit<decaampere, deca, ampere> {};
struct hectoampere : prefixed_unit<hectoampere, hecto, ampere> {};
struct kiloampere : prefixed_unit<kiloampere, kilo, ampere> {};
struct megaampere : prefixed_unit<megaampere, mega, ampere> {};
struct gigaampere : prefixed_unit<gigaampere, giga, ampere> {};
struct teraampere : prefixed_unit<teraampere, tera, ampere> {};
struct petaampere : prefixed_unit<petaampere, peta, ampere> {};
struct exaampere : prefixed_unit<exaampere, exa, ampere> {};
struct zettaampere : prefixed_unit<zettaampere, zetta, ampere> {};
struct yottaampere : prefixed_unit<yottaampere, yotta, ampere> {};

struct dim_electric_current : physical::dim_electric_current<ampere> {};

template<UnitOf<dim_electric_current> U, QuantityValue Rep = double>
using electric_current = quantity<dim_electric_current, U, Rep>;

inline namespace literals {

// A
consteval auto operator"" _q_A(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return electric_current<ampere, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_A(long double l) { return electric_current<ampere, long double>(l); }

// yA
consteval auto operator"" _q_yA(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return electric_current<yoctoampere, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_yA(long double l) { return electric_current<yoctoampere, long double>(l); }

// zA
consteval auto operator"" _q_zA(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return electric_current<zeptoampere, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_zA(long double l) { return electric_current<zeptoampere, long double>(l); }

// aA
consteval auto operator"" _q_aA(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return electric_current<attoampere, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_aA(long double l) { return electric_current<attoampere, long double>(l); }

// fA
consteval auto operator"" _q_fA(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return electric_current<femtoampere, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_fA(long double l) { return electric_current<femtoampere, long double>(l); }

// pA
consteval auto operator"" _q_pA(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return electric_current<picoampere, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_pA(long double l) { return electric_current<picoampere, long double>(l); }

// nA
consteval auto operator"" _q_nA(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return electric_current<nanoampere, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_nA(long double l) { return electric_current<nanoampere, long double>(l); }

// uA
consteval auto operator"" _q_uA(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return electric_current<microampere, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_uA(long double l) { return electric_current<microampere, long double>(l); }

// mA
consteval auto operator"" _q_mA(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return electric_current<milliampere, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_mA(long double l) { return electric_current<milliampere, long double>(l); }

// cA
consteval auto operator"" _q_cA(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return electric_current<centiampere, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_cA(long double l) { return electric_current<centiampere, long double>(l); }

// dA
consteval auto operator"" _q_dA(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return electric_current<deciampere, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_dA(long double l) { return electric_current<deciampere, long double>(l); }

// daA
consteval auto operator"" _q_daA(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return electric_current<decaampere, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_daA(long double l) { return electric_current<decaampere, long double>(l); }

// hA
consteval auto operator"" _q_hA(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return electric_current<hectoampere, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_hA(long double l) { return electric_current<hectoampere, long double>(l); }

// kA
consteval auto operator"" _q_kA(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return electric_current<kiloampere, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_kA(long double l) { return electric_current<kiloampere, long double>(l); }

// MA
consteval auto operator"" _q_MA(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return electric_current<megaampere, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_MA(long double l) { return electric_current<megaampere, long double>(l); }

// GA
consteval auto operator"" _q_GA(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return electric_current<gigaampere, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_GA(long double l) { return electric_current<gigaampere, long double>(l); }

// TA
consteval auto operator"" _q_TA(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return electric_current<teraampere, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_TA(long double l) { return electric_current<teraampere, long double>(l); }

// PA
consteval auto operator"" _q_PA(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return electric_current<petaampere, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_PA(long double l) { return electric_current<petaampere, long double>(l); }

// EA
consteval auto operator"" _q_EA(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return electric_current<exaampere, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_EA(long double l) { return electric_current<exaampere, long double>(l); }

// ZA
consteval auto operator"" _q_ZA(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return electric_current<zettaampere, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_ZA(long double l) { return electric_current<zettaampere, long double>(l); }

// YA
consteval auto operator"" _q_YA(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return electric_current<yottaampere, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_YA(long double l) { return electric_current<yottaampere, long double>(l); }

} // namespace literals

namespace unit_constants {

inline constexpr auto A = electric_current<ampere, one_rep>{};
inline constexpr auto yA = electric_current<yoctoampere, one_rep>{};
inline constexpr auto zA = electric_current<zeptoampere, one_rep>{};
inline constexpr auto aA = electric_current<attoampere, one_rep>{};
inline constexpr auto fA = electric_current<femtoampere, one_rep>{};
inline constexpr auto pA = electric_current<picoampere, one_rep>{};
inline constexpr auto nA = electric_current<nanoampere, one_rep>{};
inline constexpr auto uA = electric_current<microampere, one_rep>{};
inline constexpr auto mA = electric_current<milliampere, one_rep>{};
inline constexpr auto cA = electric_current<centiampere, one_rep>{};
inline constexpr auto dA = electric_current<deciampere, one_rep>{};
inline constexpr auto daA = electric_current<decaampere, one_rep>{};
inline constexpr auto hA = electric_current<hectoampere, one_rep>{};
inline constexpr auto kA = electric_current<kiloampere, one_rep>{};
inline constexpr auto MA = electric_current<megaampere, one_rep>{};
inline constexpr auto GA = electric_current<gigaampere, one_rep>{};
inline constexpr auto TA = electric_current<teraampere, one_rep>{};
inline constexpr auto PA = electric_current<petaampere, one_rep>{};
inline constexpr auto EA = electric_current<exaampere, one_rep>{};
inline constexpr auto ZA = electric_current<zettaampere, one_rep>{};
inline constexpr auto YA = electric_current<yottaampere, one_rep>{};

}  // namespace unit_constants

}  // namespace units::physical::si
