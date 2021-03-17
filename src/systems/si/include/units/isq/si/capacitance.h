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

#include <units/isq/dimensions/capacitance.h>
#include <units/isq/si/electric_charge.h>
#include <units/isq/si/voltage.h>
#include <units/isq/si/prefixes.h>
#include <units/quantity.h>

namespace units::isq::si {

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

struct dim_capacitance : isq::dim_capacitance<dim_capacitance, farad, dim_electric_charge, dim_voltage> {};

template<UnitOf<dim_capacitance> U, QuantityValue Rep = double>
using capacitance = quantity<dim_capacitance, U, Rep>;

inline namespace literals {

// F
constexpr auto operator"" _q_F(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return capacitance<farad, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_F(long double l) { return capacitance<farad, long double>(l); }

// yF
constexpr auto operator"" _q_yF(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return capacitance<yoctofarad, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_yF(long double l) { return capacitance<yoctofarad, long double>(l); }

// zF
constexpr auto operator"" _q_zF(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return capacitance<zeptofarad, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_zF(long double l) { return capacitance<zeptofarad, long double>(l); }

// aF
constexpr auto operator"" _q_aF(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return capacitance<attofarad, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_aF(long double l) { return capacitance<attofarad, long double>(l); }

// fF
constexpr auto operator"" _q_fF(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return capacitance<femtofarad, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_fF(long double l) { return capacitance<femtofarad, long double>(l); }

// pF
constexpr auto operator"" _q_pF(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return capacitance<picofarad, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_pF(long double l) { return capacitance<picofarad, long double>(l); }

// nF
constexpr auto operator"" _q_nF(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return capacitance<nanofarad, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_nF(long double l) { return capacitance<nanofarad, long double>(l); }

// uF
constexpr auto operator"" _q_uF(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return capacitance<microfarad, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_uF(long double l) { return capacitance<microfarad, long double>(l); }

// mF
constexpr auto operator"" _q_mF(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return capacitance<millifarad, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_mF(long double l) { return capacitance<millifarad, long double>(l); }

// cF
constexpr auto operator"" _q_cF(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return capacitance<centifarad, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_cF(long double l) { return capacitance<centifarad, long double>(l); }

// dF
constexpr auto operator"" _q_dF(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return capacitance<decifarad, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_dF(long double l) { return capacitance<decifarad, long double>(l); }

// daF
constexpr auto operator"" _q_daF(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return capacitance<decafarad, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_daF(long double l) { return capacitance<decafarad, long double>(l); }

// hF
constexpr auto operator"" _q_hF(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return capacitance<hectofarad, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_hF(long double l) { return capacitance<hectofarad, long double>(l); }

// kF
constexpr auto operator"" _q_kF(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return capacitance<kilofarad, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_kF(long double l) { return capacitance<kilofarad, long double>(l); }

// MF
constexpr auto operator"" _q_MF(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return capacitance<megafarad, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_MF(long double l) { return capacitance<megafarad, long double>(l); }

// GF
constexpr auto operator"" _q_GF(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return capacitance<gigafarad, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_GF(long double l) { return capacitance<gigafarad, long double>(l); }

// TF
constexpr auto operator"" _q_TF(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return capacitance<terafarad, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_TF(long double l) { return capacitance<terafarad, long double>(l); }

// PF
constexpr auto operator"" _q_PF(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return capacitance<petafarad, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_PF(long double l) { return capacitance<petafarad, long double>(l); }

// EF
constexpr auto operator"" _q_EF(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return capacitance<exafarad, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_EF(long double l) { return capacitance<exafarad, long double>(l); }

// ZF
constexpr auto operator"" _q_ZF(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return capacitance<zettafarad, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_ZF(long double l) { return capacitance<zettafarad, long double>(l); }

// YF
constexpr auto operator"" _q_YF(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return capacitance<yottafarad, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_YF(long double l) { return capacitance<yottafarad, long double>(l); }

}  // namespace literals

namespace unit_constants {

inline constexpr auto F = reference<dim_capacitance, farad>{};
inline constexpr auto yF = reference<dim_capacitance, yoctofarad>{};
inline constexpr auto zF = reference<dim_capacitance, zeptofarad>{};
inline constexpr auto aF = reference<dim_capacitance, attofarad>{};
inline constexpr auto fF = reference<dim_capacitance, femtofarad>{};
inline constexpr auto pF = reference<dim_capacitance, picofarad>{};
inline constexpr auto nF = reference<dim_capacitance, nanofarad>{};
inline constexpr auto uF = reference<dim_capacitance, microfarad>{};
inline constexpr auto mF = reference<dim_capacitance, millifarad>{};
inline constexpr auto cF = reference<dim_capacitance, centifarad>{};
inline constexpr auto dF = reference<dim_capacitance, decifarad>{};
inline constexpr auto daF = reference<dim_capacitance, decafarad>{};
inline constexpr auto hF = reference<dim_capacitance, hectofarad>{};
inline constexpr auto kF = reference<dim_capacitance, kilofarad>{};
inline constexpr auto MF = reference<dim_capacitance, megafarad>{};
inline constexpr auto GF = reference<dim_capacitance, gigafarad>{};
inline constexpr auto TF = reference<dim_capacitance, terafarad>{};
inline constexpr auto PF = reference<dim_capacitance, petafarad>{};
inline constexpr auto EF = reference<dim_capacitance, exafarad>{};
inline constexpr auto ZF = reference<dim_capacitance, zettafarad>{};
inline constexpr auto YF = reference<dim_capacitance, yottafarad>{};

}  // namespace unit_constants

}  // namespace units::isq::si
