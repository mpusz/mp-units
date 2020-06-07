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
#include <units/physical/si/resistance.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::physical::si {

struct siemens : named_unit<siemens, "S", prefix> {};
struct yoctosiemens : prefixed_unit<yoctosiemens, yocto, siemens> {};
struct zeptosiemens : prefixed_unit<zeptosiemens, zepto, siemens> {};
struct attosiemens : prefixed_unit<attosiemens, atto, siemens> {};
struct femtosiemens : prefixed_unit<femtosiemens, femto, siemens> {};
struct picosiemens : prefixed_unit<picosiemens, pico, siemens> {};
struct nanosiemens : prefixed_unit<nanosiemens, nano, siemens> {};
struct microsiemens : prefixed_unit<microsiemens, micro, siemens> {};
struct millisiemens : prefixed_unit<millisiemens, milli, siemens> {};
struct kilosiemens : prefixed_unit<kilosiemens, kilo, siemens> {};
struct megasiemens : prefixed_unit<megasiemens, mega, siemens> {};
struct gigasiemens : prefixed_unit<gigasiemens, giga, siemens> {};
struct terasiemens : prefixed_unit<terasiemens, tera, siemens> {};
struct petasiemens : prefixed_unit<petasiemens, peta, siemens> {};
struct exasiemens : prefixed_unit<exasiemens, exa, siemens> {};
struct zettasiemens : prefixed_unit<zettasiemens, zetta, siemens> {};
struct yottasiemens : prefixed_unit<yottasiemens, yotta, siemens> {};

struct dim_conductance : physical::dim_conductance<dim_conductance, siemens, dim_resistance> {};

template<in_unit U, in_numeric_value Rep = double>
using conductance = quantity<dim_conductance, U, Rep>;

inline namespace literals {

// R
constexpr auto operator"" q_S(unsigned long long l) { return conductance<siemens, std::int64_t>(l); }
constexpr auto operator"" q_S(long double l) { return conductance<siemens, long double>(l); }

// yS
constexpr auto operator"" q_yS(unsigned long long l) { return conductance<yoctosiemens, std::int64_t>(l); }
constexpr auto operator"" q_yS(long double l) { return conductance<yoctosiemens, long double>(l); }

// zS
constexpr auto operator"" q_zS(unsigned long long l) { return conductance<zeptosiemens, std::int64_t>(l); }
constexpr auto operator"" q_zS(long double l) { return conductance<zeptosiemens, long double>(l); }

// aS
constexpr auto operator"" q_aS(unsigned long long l) { return conductance<attosiemens, std::int64_t>(l); }
constexpr auto operator"" q_aS(long double l) { return conductance<attosiemens, long double>(l); }

// fS
constexpr auto operator"" q_fS(unsigned long long l) { return conductance<femtosiemens, std::int64_t>(l); }
constexpr auto operator"" q_fS(long double l) { return conductance<femtosiemens, long double>(l); }

// pS
constexpr auto operator"" q_pS(unsigned long long l) { return conductance<picosiemens, std::int64_t>(l); }
constexpr auto operator"" q_pS(long double l) { return conductance<picosiemens, long double>(l); }

// nS
constexpr auto operator"" q_nS(unsigned long long l) { return conductance<nanosiemens, std::int64_t>(l); }
constexpr auto operator"" q_nS(long double l) { return conductance<nanosiemens, long double>(l); }

// µS
constexpr auto operator"" q_uS(unsigned long long l) { return conductance<microsiemens, std::int64_t>(l); }
constexpr auto operator"" q_uS(long double l) { return conductance<microsiemens, long double>(l); }

// mS
constexpr auto operator"" q_mS(unsigned long long l) { return conductance<millisiemens, std::int64_t>(l); }
constexpr auto operator"" q_mS(long double l) { return conductance<millisiemens, long double>(l); }

// kS
constexpr auto operator"" q_kS(unsigned long long l) { return conductance<kilosiemens, std::int64_t>(l); }
constexpr auto operator"" q_kS(long double l) { return conductance<kilosiemens, long double>(l); }

// MS
constexpr auto operator"" q_MS(unsigned long long l) { return conductance<megasiemens, std::int64_t>(l); }
constexpr auto operator"" q_MS(long double l) { return conductance<megasiemens, long double>(l); }

// GS
constexpr auto operator"" q_GS(unsigned long long l) { return conductance<gigasiemens, std::int64_t>(l); }
constexpr auto operator"" q_GS(long double l) { return conductance<gigasiemens, long double>(l); }

// TS
constexpr auto operator"" q_TS(unsigned long long l) { return conductance<terasiemens, std::int64_t>(l); }
constexpr auto operator"" q_TS(long double l) { return conductance<terasiemens, long double>(l); }

// PS
constexpr auto operator"" q_PS(unsigned long long l) { return conductance<petasiemens, std::int64_t>(l); }
constexpr auto operator"" q_PS(long double l) { return conductance<petasiemens, long double>(l); }

// ES
constexpr auto operator"" q_ES(unsigned long long l) { return conductance<exasiemens, std::int64_t>(l); }
constexpr auto operator"" q_ES(long double l) { return conductance<exasiemens, long double>(l); }

// ZS
constexpr auto operator"" q_ZS(unsigned long long l) { return conductance<zettasiemens, std::int64_t>(l); }
constexpr auto operator"" q_ZS(long double l) { return conductance<zettasiemens, long double>(l); }

// YS
constexpr auto operator"" q_YS(unsigned long long l) { return conductance<yottasiemens, std::int64_t>(l); }
constexpr auto operator"" q_YS(long double l) { return conductance<yottasiemens, long double>(l); }

}  // namespace literals

}  // namespace units::physical::si

