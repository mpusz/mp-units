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

template<Unit U, ScalableNumber Rep = double>
using current = quantity<dim_electric_current, U, Rep>;

inline namespace literals {

// A
constexpr auto operator"" q_A(unsigned long long l) { return current<ampere, std::int64_t>(l); }
constexpr auto operator"" q_A(long double l) { return current<ampere, long double>(l); }

// yA
constexpr auto operator"" q_yA(unsigned long long l) { return current<yoctoampere, std::int64_t>(l); }
constexpr auto operator"" q_yA(long double l) { return current<yoctoampere, long double>(l); }

// zA
constexpr auto operator"" q_zA(unsigned long long l) { return current<zeptoampere, std::int64_t>(l); }
constexpr auto operator"" q_zA(long double l) { return current<zeptoampere, long double>(l); }

// aA
constexpr auto operator"" q_aA(unsigned long long l) { return current<attoampere, std::int64_t>(l); }
constexpr auto operator"" q_aA(long double l) { return current<attoampere, long double>(l); }

// fA
constexpr auto operator"" q_fA(unsigned long long l) { return current<femtoampere, std::int64_t>(l); }
constexpr auto operator"" q_fA(long double l) { return current<femtoampere, long double>(l); }

// pA
constexpr auto operator"" q_pA(unsigned long long l) { return current<picoampere, std::int64_t>(l); }
constexpr auto operator"" q_pA(long double l) { return current<picoampere, long double>(l); }

// nA
constexpr auto operator"" q_nA(unsigned long long l) { return current<nanoampere, std::int64_t>(l); }
constexpr auto operator"" q_nA(long double l) { return current<nanoampere, long double>(l); }

// uA
constexpr auto operator"" q_uA(unsigned long long l) { return current<microampere, std::int64_t>(l); }
constexpr auto operator"" q_uA(long double l) { return current<microampere, long double>(l); }

// mA
constexpr auto operator"" q_mA(unsigned long long l) { return current<milliampere, std::int64_t>(l); }
constexpr auto operator"" q_mA(long double l) { return current<milliampere, long double>(l); }

// cA
constexpr auto operator"" q_cA(unsigned long long l) { return current<centiampere, std::int64_t>(l); }
constexpr auto operator"" q_cA(long double l) { return current<centiampere, long double>(l); }

// dA
constexpr auto operator"" q_dA(unsigned long long l) { return current<deciampere, std::int64_t>(l); }
constexpr auto operator"" q_dA(long double l) { return current<deciampere, long double>(l); }

// daA
constexpr auto operator"" q_daA(unsigned long long l) { return current<decaampere, std::int64_t>(l); }
constexpr auto operator"" q_daA(long double l) { return current<decaampere, long double>(l); }

// hA
constexpr auto operator"" q_hA(unsigned long long l) { return current<hectoampere, std::int64_t>(l); }
constexpr auto operator"" q_hA(long double l) { return current<hectoampere, long double>(l); }

// kA
constexpr auto operator"" q_kA(unsigned long long l) { return current<kiloampere, std::int64_t>(l); }
constexpr auto operator"" q_kA(long double l) { return current<kiloampere, long double>(l); }

// MA
constexpr auto operator"" q_MA(unsigned long long l) { return current<megaampere, std::int64_t>(l); }
constexpr auto operator"" q_MA(long double l) { return current<megaampere, long double>(l); }

// GA
constexpr auto operator"" q_GA(unsigned long long l) { return current<gigaampere, std::int64_t>(l); }
constexpr auto operator"" q_GA(long double l) { return current<gigaampere, long double>(l); }

// TA
constexpr auto operator"" q_TA(unsigned long long l) { return current<teraampere, std::int64_t>(l); }
constexpr auto operator"" q_TA(long double l) { return current<teraampere, long double>(l); }

// PA
constexpr auto operator"" q_PA(unsigned long long l) { return current<petaampere, std::int64_t>(l); }
constexpr auto operator"" q_PA(long double l) { return current<petaampere, long double>(l); }

// EA
constexpr auto operator"" q_EA(unsigned long long l) { return current<exaampere, std::int64_t>(l); }
constexpr auto operator"" q_EA(long double l) { return current<exaampere, long double>(l); }

// ZA
constexpr auto operator"" q_ZA(unsigned long long l) { return current<zettaampere, std::int64_t>(l); }
constexpr auto operator"" q_ZA(long double l) { return current<zettaampere, long double>(l); }

// YA
constexpr auto operator"" q_YA(unsigned long long l) { return current<yottaampere, std::int64_t>(l); }
constexpr auto operator"" q_YA(long double l) { return current<yottaampere, long double>(l); }

} // namespace literals

}  // namespace units::physical::si
