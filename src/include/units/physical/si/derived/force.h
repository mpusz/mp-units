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

#include <units/physical/dimensions/force.h>
#include <units/physical/si/derived/acceleration.h>
#include <units/physical/si/base/mass.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::physical::si {

struct newton : named_unit<newton, "N", prefix> {};
struct yoctonewton : prefixed_unit<yoctonewton, yocto, newton> {};
struct zeptonewton : prefixed_unit<zeptonewton, zepto, newton> {};
struct attonewton : prefixed_unit<attonewton, atto, newton> {};
struct femtonewton : prefixed_unit<femtonewton, femto, newton> {};
struct piconewton : prefixed_unit<piconewton, pico, newton> {};
struct nanonewton : prefixed_unit<nanonewton, nano, newton> {};
struct micronewton : prefixed_unit<micronewton, micro, newton> {};
struct millinewton : prefixed_unit<millinewton, milli, newton> {};
struct centinewton : prefixed_unit<centinewton, centi, newton> {};
struct decinewton : prefixed_unit<decinewton, deci, newton> {};
struct decanewton : prefixed_unit<decanewton, deca, newton> {};
struct hectonewton : prefixed_unit<hectonewton, hecto, newton> {};
struct kilonewton : prefixed_unit<kilonewton, kilo, newton> {};
struct meganewton : prefixed_unit<meganewton, mega, newton> {};
struct giganewton : prefixed_unit<giganewton, giga, newton> {};
struct teranewton : prefixed_unit<teranewton, tera, newton> {};
struct petanewton : prefixed_unit<petanewton, peta, newton> {};
struct exanewton : prefixed_unit<exanewton, exa, newton> {};
struct zettanewton : prefixed_unit<zettanewton, zetta, newton> {};
struct yottanewton : prefixed_unit<yottanewton, yotta, newton> {};

struct dim_force : physical::dim_force<dim_force, newton, dim_mass, dim_acceleration> {};

template<UnitOf<dim_force> U, QuantityValue Rep = double>
using force = quantity<dim_force, U, Rep>;

inline namespace literals {

// N
constexpr auto operator"" _q_N(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return force<newton, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_N(long double l) { return force<newton, long double>(l); }

// yN
constexpr auto operator"" _q_yN(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return force<yoctonewton, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_yN(long double l) { return force<yoctonewton, long double>(l); }

// zN
constexpr auto operator"" _q_zN(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return force<zeptonewton, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_zN(long double l) { return force<zeptonewton, long double>(l); }

// aN
constexpr auto operator"" _q_aN(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return force<attonewton, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_aN(long double l) { return force<attonewton, long double>(l); }

// fN
constexpr auto operator"" _q_fN(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return force<femtonewton, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_fN(long double l) { return force<femtonewton, long double>(l); }

// pN
constexpr auto operator"" _q_pN(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return force<piconewton, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_pN(long double l) { return force<piconewton, long double>(l); }

// nN
constexpr auto operator"" _q_nN(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return force<nanonewton, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_nN(long double l) { return force<nanonewton, long double>(l); }

// uN
constexpr auto operator"" _q_uN(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return force<micronewton, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_uN(long double l) { return force<micronewton, long double>(l); }

// mN
constexpr auto operator"" _q_mN(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return force<millinewton, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_mN(long double l) { return force<millinewton, long double>(l); }

// cN
constexpr auto operator"" _q_cN(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return force<centinewton, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_cN(long double l) { return force<centinewton, long double>(l); }

// dN
constexpr auto operator"" _q_dN(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return force<decinewton, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_dN(long double l) { return force<decinewton, long double>(l); }

// daN
constexpr auto operator"" _q_daN(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return force<decanewton, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_daN(long double l) { return force<decanewton, long double>(l); }

// hN
constexpr auto operator"" _q_hN(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return force<hectonewton, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_hN(long double l) { return force<hectonewton, long double>(l); }

// kN
constexpr auto operator"" _q_kN(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return force<kilonewton, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_kN(long double l) { return force<kilonewton, long double>(l); }

// MN
constexpr auto operator"" _q_MN(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return force<meganewton, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_MN(long double l) { return force<meganewton, long double>(l); }

// GN
constexpr auto operator"" _q_GN(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return force<giganewton, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_GN(long double l) { return force<giganewton, long double>(l); }

// TN
constexpr auto operator"" _q_TN(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return force<teranewton, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_TN(long double l) { return force<teranewton, long double>(l); }

// PN
constexpr auto operator"" _q_PN(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return force<petanewton, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_PN(long double l) { return force<petanewton, long double>(l); }

// EN
constexpr auto operator"" _q_EN(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return force<exanewton, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_EN(long double l) { return force<exanewton, long double>(l); }

// ZN
constexpr auto operator"" _q_ZN(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return force<zettanewton, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_ZN(long double l) { return force<zettanewton, long double>(l); }

// YN
constexpr auto operator"" _q_YN(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return force<yottanewton, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_YN(long double l) { return force<yottanewton, long double>(l); }

}  // namespace literals

namespace unit_constants {

inline constexpr auto N = force<newton, one_rep>{};
inline constexpr auto yN = force<yoctonewton, one_rep>{};
inline constexpr auto zN = force<zeptonewton, one_rep>{};
inline constexpr auto aN = force<attonewton, one_rep>{};
inline constexpr auto fN = force<femtonewton, one_rep>{};
inline constexpr auto pN = force<piconewton, one_rep>{};
inline constexpr auto nN = force<nanonewton, one_rep>{};
inline constexpr auto uN = force<micronewton, one_rep>{};
inline constexpr auto mN = force<millinewton, one_rep>{};
inline constexpr auto cN = force<centinewton, one_rep>{};
inline constexpr auto dN = force<decinewton, one_rep>{};
inline constexpr auto daN = force<decanewton, one_rep>{};
inline constexpr auto hN = force<hectonewton, one_rep>{};
inline constexpr auto kN = force<kilonewton, one_rep>{};
inline constexpr auto MN = force<meganewton, one_rep>{};
inline constexpr auto GN = force<giganewton, one_rep>{};
inline constexpr auto TN = force<teranewton, one_rep>{};
inline constexpr auto PN = force<petanewton, one_rep>{};
inline constexpr auto EN = force<exanewton, one_rep>{};
inline constexpr auto ZN = force<zettanewton, one_rep>{};
inline constexpr auto YN = force<yottanewton, one_rep>{};

}  // namespace unit_constants

}  // namespace units::physical::si
