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
#include <units/physical/si/acceleration.h>
#include <units/physical/si/mass.h>
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

template<in_unit U, in_numeric_value Rep = double>
using force = quantity<dim_force, U, Rep>;

inline namespace literals {

// N
constexpr auto operator"" q_N(unsigned long long l) { return force<newton, std::int64_t>(l); }
constexpr auto operator"" q_N(long double l) { return force<newton, long double>(l); }

// yN
constexpr auto operator"" q_yN(unsigned long long l) { return force<yoctonewton, std::int64_t>(l); }
constexpr auto operator"" q_yN(long double l) { return force<yoctonewton, long double>(l); }

// zN
constexpr auto operator"" q_zN(unsigned long long l) { return force<zeptonewton, std::int64_t>(l); }
constexpr auto operator"" q_zN(long double l) { return force<zeptonewton, long double>(l); }

// aN
constexpr auto operator"" q_aN(unsigned long long l) { return force<attonewton, std::int64_t>(l); }
constexpr auto operator"" q_aN(long double l) { return force<attonewton, long double>(l); }

// fN
constexpr auto operator"" q_fN(unsigned long long l) { return force<femtonewton, std::int64_t>(l); }
constexpr auto operator"" q_fN(long double l) { return force<femtonewton, long double>(l); }

// pN
constexpr auto operator"" q_pN(unsigned long long l) { return force<piconewton, std::int64_t>(l); }
constexpr auto operator"" q_pN(long double l) { return force<piconewton, long double>(l); }

// nN
constexpr auto operator"" q_nN(unsigned long long l) { return force<nanonewton, std::int64_t>(l); }
constexpr auto operator"" q_nN(long double l) { return force<nanonewton, long double>(l); }

// uN
constexpr auto operator"" q_uN(unsigned long long l) { return force<micronewton, std::int64_t>(l); }
constexpr auto operator"" q_uN(long double l) { return force<micronewton, long double>(l); }

// mN
constexpr auto operator"" q_mN(unsigned long long l) { return force<millinewton, std::int64_t>(l); }
constexpr auto operator"" q_mN(long double l) { return force<millinewton, long double>(l); }

// cN
constexpr auto operator"" q_cN(unsigned long long l) { return force<centinewton, std::int64_t>(l); }
constexpr auto operator"" q_cN(long double l) { return force<centinewton, long double>(l); }

// dN
constexpr auto operator"" q_dN(unsigned long long l) { return force<decinewton, std::int64_t>(l); }
constexpr auto operator"" q_dN(long double l) { return force<decinewton, long double>(l); }

// daN
constexpr auto operator"" q_daN(unsigned long long l) { return force<decanewton, std::int64_t>(l); }
constexpr auto operator"" q_daN(long double l) { return force<decanewton, long double>(l); }

// hN
constexpr auto operator"" q_hN(unsigned long long l) { return force<hectonewton, std::int64_t>(l); }
constexpr auto operator"" q_hN(long double l) { return force<hectonewton, long double>(l); }

// kN
constexpr auto operator"" q_kN(unsigned long long l) { return force<kilonewton, std::int64_t>(l); }
constexpr auto operator"" q_kN(long double l) { return force<kilonewton, long double>(l); }

// MN
constexpr auto operator"" q_MN(unsigned long long l) { return force<meganewton, std::int64_t>(l); }
constexpr auto operator"" q_MN(long double l) { return force<meganewton, long double>(l); }

// GN
constexpr auto operator"" q_GN(unsigned long long l) { return force<giganewton, std::int64_t>(l); }
constexpr auto operator"" q_GN(long double l) { return force<giganewton, long double>(l); }

// TN
constexpr auto operator"" q_TN(unsigned long long l) { return force<teranewton, std::int64_t>(l); }
constexpr auto operator"" q_TN(long double l) { return force<teranewton, long double>(l); }

// PN
constexpr auto operator"" q_PN(unsigned long long l) { return force<petanewton, std::int64_t>(l); }
constexpr auto operator"" q_PN(long double l) { return force<petanewton, long double>(l); }

// EN
constexpr auto operator"" q_EN(unsigned long long l) { return force<exanewton, std::int64_t>(l); }
constexpr auto operator"" q_EN(long double l) { return force<exanewton, long double>(l); }

// ZN
constexpr auto operator"" q_ZN(unsigned long long l) { return force<zettanewton, std::int64_t>(l); }
constexpr auto operator"" q_ZN(long double l) { return force<zettanewton, long double>(l); }

// YN
constexpr auto operator"" q_YN(unsigned long long l) { return force<yottanewton, std::int64_t>(l); }
constexpr auto operator"" q_YN(long double l) { return force<yottanewton, long double>(l); }

}  // namespace literals

}  // namespace units::physical::si
