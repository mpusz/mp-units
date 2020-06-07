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
#include <units/physical/si/magnetic_flux.h>
#include <units/physical/si/current.h>
#include <units/quantity.h>

namespace units::physical::si {

struct henry : named_unit<henry, "H", prefix> {};

struct yoctohenry : prefixed_unit<yoctohenry, yocto, henry> {};
struct zeptohenry : prefixed_unit<zeptohenry, zepto, henry> {};
struct attohenry : prefixed_unit<attohenry, atto, henry> {};
struct femtohenry : prefixed_unit<femtohenry, femto, henry> {};
struct picohenry : prefixed_unit<picohenry, pico, henry> {};
struct nanohenry : prefixed_unit<nanohenry, nano, henry> {};
struct microhenry : prefixed_unit<microhenry, micro, henry> {};
struct millihenry : prefixed_unit<millihenry, milli, henry> {};
struct kilohenry : prefixed_unit<kilohenry, kilo, henry> {};
struct megahenry : prefixed_unit<megahenry, mega, henry> {};
struct gigahenry : prefixed_unit<gigahenry, giga, henry> {};
struct terahenry : prefixed_unit<terahenry, tera, henry> {};
struct petahenry : prefixed_unit<petahenry, peta, henry> {};
struct exahenry : prefixed_unit<exahenry, exa, henry> {};
struct zettahenry : prefixed_unit<zettahenry, zetta, henry> {};
struct yottahenry : prefixed_unit<yottahenry, yotta, henry> {};

struct dim_inductance : physical::dim_inductance<dim_inductance, henry, dim_magnetic_flux, dim_electric_current> {};

template<in_unit U, in_numeric_value Rep = double>
using inductance = quantity<dim_inductance, U, Rep>;

inline namespace literals {

// H
constexpr auto operator"" q_H(unsigned long long l) { return inductance<henry, std::int64_t>(l); }
constexpr auto operator"" q_H(long double l) { return inductance<henry, long double>(l); }

// yH
constexpr auto operator"" q_yH(unsigned long long l) { return inductance<yoctohenry, std::int64_t>(l); }
constexpr auto operator"" q_yH(long double l) { return inductance<yoctohenry, long double>(l); }

// zH
constexpr auto operator"" q_zH(unsigned long long l) { return inductance<zeptohenry, std::int64_t>(l); }
constexpr auto operator"" q_zH(long double l) { return inductance<zeptohenry, long double>(l); }

// aH
constexpr auto operator"" q_aH(unsigned long long l) { return inductance<attohenry, std::int64_t>(l); }
constexpr auto operator"" q_aH(long double l) { return inductance<attohenry, long double>(l); }

// fH
constexpr auto operator"" q_fH(unsigned long long l) { return inductance<femtohenry, std::int64_t>(l); }
constexpr auto operator"" q_fH(long double l) { return inductance<femtohenry, long double>(l); }

// pH
constexpr auto operator"" q_pH(unsigned long long l) { return inductance<picohenry, std::int64_t>(l); }
constexpr auto operator"" q_pH(long double l) { return inductance<picohenry, long double>(l); }

// nH
constexpr auto operator"" q_nH(unsigned long long l) { return inductance<nanohenry, std::int64_t>(l); }
constexpr auto operator"" q_nH(long double l) { return inductance<nanohenry, long double>(l); }

// µH
constexpr auto operator"" q_uH(unsigned long long l) { return inductance<microhenry, std::int64_t>(l); }
constexpr auto operator"" q_uH(long double l) { return inductance<microhenry, long double>(l); }

// mH
constexpr auto operator"" q_mH(unsigned long long l) { return inductance<millihenry, std::int64_t>(l); }
constexpr auto operator"" q_mH(long double l) { return inductance<millihenry, long double>(l); }

// kH
constexpr auto operator"" q_kH(unsigned long long l) { return inductance<kilohenry, std::int64_t>(l); }
constexpr auto operator"" q_kH(long double l) { return inductance<kilohenry, long double>(l); }

// MH
constexpr auto operator"" q_MH(unsigned long long l) { return inductance<megahenry, std::int64_t>(l); }
constexpr auto operator"" q_MH(long double l) { return inductance<megahenry, long double>(l); }

// GH
constexpr auto operator"" q_GH(unsigned long long l) { return inductance<gigahenry, std::int64_t>(l); }
constexpr auto operator"" q_GH(long double l) { return inductance<gigahenry, long double>(l); }

// TH
constexpr auto operator"" q_TH(unsigned long long l) { return inductance<terahenry, std::int64_t>(l); }
constexpr auto operator"" q_TH(long double l) { return inductance<terahenry, long double>(l); }

// PH
constexpr auto operator"" q_PH(unsigned long long l) { return inductance<petahenry, std::int64_t>(l); }
constexpr auto operator"" q_PH(long double l) { return inductance<petahenry, long double>(l); }

// EH
constexpr auto operator"" q_EH(unsigned long long l) { return inductance<exahenry, std::int64_t>(l); }
constexpr auto operator"" q_EH(long double l) { return inductance<exahenry, long double>(l); }

// ZH
constexpr auto operator"" q_ZH(unsigned long long l) { return inductance<zettahenry, std::int64_t>(l); }
constexpr auto operator"" q_ZH(long double l) { return inductance<zettahenry, long double>(l); }

// YH
constexpr auto operator"" q_YH(unsigned long long l) { return inductance<yottahenry, std::int64_t>(l); }
constexpr auto operator"" q_YH(long double l) { return inductance<yottahenry, long double>(l); }

}  // namespace literals

}  // namespace units::physical::si
