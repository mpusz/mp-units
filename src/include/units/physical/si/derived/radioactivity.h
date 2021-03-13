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

#include <units/physical/dimensions/radioactivity.h>
#include <units/physical/si/base/time.h>
#include <units/quantity.h>

namespace units::physical::si {

struct becquerel : named_unit<becquerel, "Bq", prefix> {};
struct yoctobecquerel : prefixed_unit<yoctobecquerel, yocto, becquerel> {};
struct zeptobecquerel : prefixed_unit<zeptobecquerel, zepto, becquerel> {};
struct attobecquerel : prefixed_unit<attobecquerel, atto, becquerel> {};
struct femtobecquerel : prefixed_unit<femtobecquerel, femto, becquerel> {};
struct picobecquerel : prefixed_unit<picobecquerel, pico, becquerel> {};
struct nanobecquerel : prefixed_unit<nanobecquerel, nano, becquerel> {};
struct microbecquerel : prefixed_unit<microbecquerel, micro, becquerel> {};
struct millibecquerel : prefixed_unit<millibecquerel, milli, becquerel> {};
struct centibecquerel : prefixed_unit<centibecquerel, centi, becquerel> {};
struct decibecquerel : prefixed_unit<decibecquerel, deci, becquerel> {};
struct decabecquerel : prefixed_unit<decabecquerel, deca, becquerel> {};
struct hectobecquerel : prefixed_unit<hectobecquerel, hecto, becquerel> {};
struct kilobecquerel : prefixed_unit<kilobecquerel, kilo, becquerel> {};
struct megabecquerel : prefixed_unit<megabecquerel, mega, becquerel> {};
struct gigabecquerel : prefixed_unit<gigabecquerel, giga, becquerel> {};
struct terabecquerel : prefixed_unit<terabecquerel, tera, becquerel> {};
struct petabecquerel : prefixed_unit<petabecquerel, peta, becquerel> {};
struct exabecquerel : prefixed_unit<exabecquerel, exa, becquerel> {};
struct zettabecquerel : prefixed_unit<zettabecquerel, zetta, becquerel> {};
struct yottabecquerel : prefixed_unit<yottabecquerel, yotta, becquerel> {};

struct dim_radioactivity : physical::dim_radioactivity<dim_radioactivity, becquerel, dim_time> {};

template<UnitOf<dim_radioactivity> U, QuantityValue Rep = double>
using radioactivity = quantity<dim_radioactivity, U, Rep>;

inline namespace literals {

// Bq
constexpr auto operator"" _q_Bq(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return radioactivity<becquerel, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Bq(long double l) { return radioactivity<becquerel, long double>(l); }

// yBq
constexpr auto operator"" _q_yBq(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return radioactivity<yoctobecquerel, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_yBq(long double l) { return radioactivity<yoctobecquerel, long double>(l); }

// zBq
constexpr auto operator"" _q_zBq(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return radioactivity<zeptobecquerel, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_zBq(long double l) { return radioactivity<zeptobecquerel, long double>(l); }

// aBq
constexpr auto operator"" _q_aBq(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return radioactivity<attobecquerel, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_aBq(long double l) { return radioactivity<attobecquerel, long double>(l); }

// fBq
constexpr auto operator"" _q_fBq(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return radioactivity<femtobecquerel, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_fBq(long double l) { return radioactivity<femtobecquerel, long double>(l); }

// pBq
constexpr auto operator"" _q_pBq(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return radioactivity<picobecquerel, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_pBq(long double l) { return radioactivity<picobecquerel, long double>(l); }

// nBq
constexpr auto operator"" _q_nBq(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return radioactivity<nanobecquerel, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_nBq(long double l) { return radioactivity<nanobecquerel, long double>(l); }

// uBq
constexpr auto operator"" _q_uBq(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return radioactivity<microbecquerel, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_uBq(long double l) { return radioactivity<microbecquerel, long double>(l); }

// mBq
constexpr auto operator"" _q_mBq(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return radioactivity<millibecquerel, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_mBq(long double l) { return radioactivity<millibecquerel, long double>(l); }

// cBq
constexpr auto operator"" _q_cBq(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return radioactivity<centibecquerel, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_cBq(long double l) { return radioactivity<centibecquerel, long double>(l); }

// dBq
constexpr auto operator"" _q_dBq(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return radioactivity<decibecquerel, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_dBq(long double l) { return radioactivity<decibecquerel, long double>(l); }

// daBq
constexpr auto operator"" _q_daBq(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return radioactivity<decabecquerel, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_daBq(long double l) { return radioactivity<decabecquerel, long double>(l); }

// hBq
constexpr auto operator"" _q_hBq(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return radioactivity<hectobecquerel, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_hBq(long double l) { return radioactivity<hectobecquerel, long double>(l); }

// kBq
constexpr auto operator"" _q_kBq(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return radioactivity<kilobecquerel, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_kBq(long double l) { return radioactivity<kilobecquerel, long double>(l); }

// MBq
constexpr auto operator"" _q_MBq(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return radioactivity<megabecquerel, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_MBq(long double l) { return radioactivity<megabecquerel, long double>(l); }

// GBq
constexpr auto operator"" _q_GBq(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return radioactivity<gigabecquerel, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_GBq(long double l) { return radioactivity<gigabecquerel, long double>(l); }

// TBq
constexpr auto operator"" _q_TBq(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return radioactivity<terabecquerel, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_TBq(long double l) { return radioactivity<terabecquerel, long double>(l); }

// PBq
constexpr auto operator"" _q_PBq(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return radioactivity<petabecquerel, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_PBq(long double l) { return radioactivity<petabecquerel, long double>(l); }

// EBq
constexpr auto operator"" _q_EBq(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return radioactivity<exabecquerel, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_EBq(long double l) { return radioactivity<exabecquerel, long double>(l); }

// ZBq
constexpr auto operator"" _q_ZBq(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return radioactivity<zettabecquerel, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_ZBq(long double l) { return radioactivity<zettabecquerel, long double>(l); }

// YBq
constexpr auto operator"" _q_YBq(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return radioactivity<yottabecquerel, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_YBq(long double l) { return radioactivity<yottabecquerel, long double>(l); }

}  // namespace literals

namespace unit_constants {

inline constexpr auto Bq = radioactivity<becquerel, one_rep>{};
inline constexpr auto yBq = radioactivity<yoctobecquerel, one_rep>{};
inline constexpr auto zBq = radioactivity<zeptobecquerel, one_rep>{};
inline constexpr auto aBq = radioactivity<attobecquerel, one_rep>{};
inline constexpr auto fBq = radioactivity<femtobecquerel, one_rep>{};
inline constexpr auto pBq = radioactivity<picobecquerel, one_rep>{};
inline constexpr auto nBq = radioactivity<nanobecquerel, one_rep>{};
inline constexpr auto uBq = radioactivity<microbecquerel, one_rep>{};
inline constexpr auto mBq = radioactivity<millibecquerel, one_rep>{};
inline constexpr auto cBq = radioactivity<centibecquerel, one_rep>{};
inline constexpr auto dBq = radioactivity<decibecquerel, one_rep>{};
inline constexpr auto daBq = radioactivity<decabecquerel, one_rep>{};
inline constexpr auto hBq = radioactivity<hectobecquerel, one_rep>{};
inline constexpr auto kBq = radioactivity<kilobecquerel, one_rep>{};
inline constexpr auto MBq = radioactivity<megabecquerel, one_rep>{};
inline constexpr auto GBq = radioactivity<gigabecquerel, one_rep>{};
inline constexpr auto TBq = radioactivity<terabecquerel, one_rep>{};
inline constexpr auto PBq = radioactivity<petabecquerel, one_rep>{};
inline constexpr auto EBq = radioactivity<exabecquerel, one_rep>{};
inline constexpr auto ZBq = radioactivity<zettabecquerel, one_rep>{};
inline constexpr auto YBq = radioactivity<yottabecquerel, one_rep>{};

}  // namespace unit_constants

}  // namespace units::physical::si
