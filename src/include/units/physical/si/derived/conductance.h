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

#include <units/physical/dimensions/conductance.h>
#include <units/physical/si/derived/resistance.h>
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

template<UnitOf<dim_conductance> U, QuantityValue Rep = double>
using conductance = quantity<dim_conductance, U, Rep>;

inline namespace literals {

// R
consteval auto operator"" _q_S(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return conductance<siemens, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_S(long double l) { return conductance<siemens, long double>(l); }

// yS
consteval auto operator"" _q_yS(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return conductance<yoctosiemens, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_yS(long double l) { return conductance<yoctosiemens, long double>(l); }

// zS
consteval auto operator"" _q_zS(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return conductance<zeptosiemens, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_zS(long double l) { return conductance<zeptosiemens, long double>(l); }

// aS
consteval auto operator"" _q_aS(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return conductance<attosiemens, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_aS(long double l) { return conductance<attosiemens, long double>(l); }

// fS
consteval auto operator"" _q_fS(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return conductance<femtosiemens, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_fS(long double l) { return conductance<femtosiemens, long double>(l); }

// pS
consteval auto operator"" _q_pS(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return conductance<picosiemens, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_pS(long double l) { return conductance<picosiemens, long double>(l); }

// nS
consteval auto operator"" _q_nS(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return conductance<nanosiemens, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_nS(long double l) { return conductance<nanosiemens, long double>(l); }

// µS
consteval auto operator"" _q_uS(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return conductance<microsiemens, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_uS(long double l) { return conductance<microsiemens, long double>(l); }

// mS
consteval auto operator"" _q_mS(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return conductance<millisiemens, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_mS(long double l) { return conductance<millisiemens, long double>(l); }

// kS
consteval auto operator"" _q_kS(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return conductance<kilosiemens, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_kS(long double l) { return conductance<kilosiemens, long double>(l); }

// MS
consteval auto operator"" _q_MS(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return conductance<megasiemens, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_MS(long double l) { return conductance<megasiemens, long double>(l); }

// GS
consteval auto operator"" _q_GS(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return conductance<gigasiemens, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_GS(long double l) { return conductance<gigasiemens, long double>(l); }

// TS
consteval auto operator"" _q_TS(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return conductance<terasiemens, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_TS(long double l) { return conductance<terasiemens, long double>(l); }

// PS
consteval auto operator"" _q_PS(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return conductance<petasiemens, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_PS(long double l) { return conductance<petasiemens, long double>(l); }

// ES
consteval auto operator"" _q_ES(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return conductance<exasiemens, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_ES(long double l) { return conductance<exasiemens, long double>(l); }

// ZS
consteval auto operator"" _q_ZS(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return conductance<zettasiemens, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_ZS(long double l) { return conductance<zettasiemens, long double>(l); }

// YS
consteval auto operator"" _q_YS(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return conductance<yottasiemens, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_YS(long double l) { return conductance<yottasiemens, long double>(l); }

}  // namespace literals

namespace unit_constants {

inline constexpr auto S = conductance<siemens, one_rep>{};
inline constexpr auto yS = conductance<yoctosiemens, one_rep>{};
inline constexpr auto zS = conductance<zeptosiemens, one_rep>{};
inline constexpr auto aS = conductance<attosiemens, one_rep>{};
inline constexpr auto fS = conductance<femtosiemens, one_rep>{};
inline constexpr auto pS = conductance<picosiemens, one_rep>{};
inline constexpr auto nS = conductance<nanosiemens, one_rep>{};
inline constexpr auto uS = conductance<microsiemens, one_rep>{};
inline constexpr auto mS = conductance<millisiemens, one_rep>{};
inline constexpr auto kS = conductance<kilosiemens, one_rep>{};
inline constexpr auto MS = conductance<megasiemens, one_rep>{};
inline constexpr auto GS = conductance<gigasiemens, one_rep>{};
inline constexpr auto TS = conductance<terasiemens, one_rep>{};
inline constexpr auto PS = conductance<petasiemens, one_rep>{};
inline constexpr auto ES = conductance<exasiemens, one_rep>{};
inline constexpr auto ZS = conductance<zettasiemens, one_rep>{};
inline constexpr auto YS = conductance<yottasiemens, one_rep>{};

}  // namespace unit_constants

}  // namespace units::physical::si

