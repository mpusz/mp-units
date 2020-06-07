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
#include <units/physical/si/voltage.h>
#include <units/physical/si/current.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::physical::si {

struct ohm : named_unit<ohm, {"Ω", "ohm"}, prefix> {};
struct yoctoohm : prefixed_unit<yoctoohm, yocto, ohm> {};
struct zeptoohm : prefixed_unit<zeptoohm, zepto, ohm> {};
struct attoohm : prefixed_unit<attoohm, atto, ohm> {};
struct femtoohm : prefixed_unit<femtoohm, femto, ohm> {};
struct picoohm : prefixed_unit<picoohm, pico, ohm> {};
struct nanoohm : prefixed_unit<nanoohm, nano, ohm> {};
struct microohm : prefixed_unit<microohm, micro, ohm> {};
struct milliohm : prefixed_unit<milliohm, milli, ohm> {};
struct kiloohm : prefixed_unit<kiloohm, kilo, ohm> {};
struct megaohm : prefixed_unit<megaohm, mega, ohm> {};
struct gigaohm : prefixed_unit<gigaohm, giga, ohm> {};
struct teraohm : prefixed_unit<teraohm, tera, ohm> {};
struct petaohm : prefixed_unit<petaohm, peta, ohm> {};
struct exaohm : prefixed_unit<exaohm, exa, ohm> {};
struct zettaohm : prefixed_unit<zettaohm, zetta, ohm> {};
struct yottaohm : prefixed_unit<yottaohm, yotta, ohm> {};

struct dim_resistance : physical::dim_resistance<dim_resistance, ohm, dim_voltage, dim_electric_current> {};

template<in_unit U, in_numeric_value Rep = double>
using resistance = quantity<dim_resistance, U, Rep>;

inline namespace literals {

// R
constexpr auto operator"" q_R(unsigned long long l) { return resistance<ohm, std::int64_t>(l); }
constexpr auto operator"" q_R(long double l) { return resistance<ohm, long double>(l); }

// yR
constexpr auto operator"" q_yR(unsigned long long l) { return resistance<yoctoohm, std::int64_t>(l); }
constexpr auto operator"" q_yR(long double l) { return resistance<yoctoohm, long double>(l); }

// zR
constexpr auto operator"" q_zR(unsigned long long l) { return resistance<zeptoohm, std::int64_t>(l); }
constexpr auto operator"" q_zR(long double l) { return resistance<zeptoohm, long double>(l); }

// aR
constexpr auto operator"" q_aR(unsigned long long l) { return resistance<attoohm, std::int64_t>(l); }
constexpr auto operator"" q_aR(long double l) { return resistance<attoohm, long double>(l); }

// fR
constexpr auto operator"" q_fR(unsigned long long l) { return resistance<femtoohm, std::int64_t>(l); }
constexpr auto operator"" q_fR(long double l) { return resistance<femtoohm, long double>(l); }

// pR
constexpr auto operator"" q_pR(unsigned long long l) { return resistance<picoohm, std::int64_t>(l); }
constexpr auto operator"" q_pR(long double l) { return resistance<picoohm, long double>(l); }

// nR
constexpr auto operator"" q_nR(unsigned long long l) { return resistance<nanoohm, std::int64_t>(l); }
constexpr auto operator"" q_nR(long double l) { return resistance<nanoohm, long double>(l); }

// uR
constexpr auto operator"" q_uR(unsigned long long l) { return resistance<microohm, std::int64_t>(l); }
constexpr auto operator"" q_uR(long double l) { return resistance<microohm, long double>(l); }

// mR
constexpr auto operator"" q_mR(unsigned long long l) { return resistance<milliohm, std::int64_t>(l); }
constexpr auto operator"" q_mR(long double l) { return resistance<milliohm, long double>(l); }

// kR
constexpr auto operator"" q_kR(unsigned long long l) { return resistance<kiloohm, std::int64_t>(l); }
constexpr auto operator"" q_kR(long double l) { return resistance<kiloohm, long double>(l); }

// MR
constexpr auto operator"" q_MR(unsigned long long l) { return resistance<megaohm, std::int64_t>(l); }
constexpr auto operator"" q_MR(long double l) { return resistance<megaohm, long double>(l); }

// GR
constexpr auto operator"" q_GR(unsigned long long l) { return resistance<gigaohm, std::int64_t>(l); }
constexpr auto operator"" q_GR(long double l) { return resistance<gigaohm, long double>(l); }

// TR
constexpr auto operator"" q_TR(unsigned long long l) { return resistance<teraohm, std::int64_t>(l); }
constexpr auto operator"" q_TR(long double l) { return resistance<teraohm, long double>(l); }

// PR
constexpr auto operator"" q_PR(unsigned long long l) { return resistance<petaohm, std::int64_t>(l); }
constexpr auto operator"" q_PR(long double l) { return resistance<petaohm, long double>(l); }

// ER
constexpr auto operator"" q_ER(unsigned long long l) { return resistance<exaohm, std::int64_t>(l); }
constexpr auto operator"" q_ER(long double l) { return resistance<exaohm, long double>(l); }

// ZR
constexpr auto operator"" q_ZR(unsigned long long l) { return resistance<zettaohm, std::int64_t>(l); }
constexpr auto operator"" q_ZR(long double l) { return resistance<zettaohm, long double>(l); }

// YR
constexpr auto operator"" q_YR(unsigned long long l) { return resistance<yottaohm, std::int64_t>(l); }
constexpr auto operator"" q_YR(long double l) { return resistance<yottaohm, long double>(l); }

}  // namespace literals

}  // namespace units::physical::si
