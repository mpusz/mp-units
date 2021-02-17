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

#include <units/physical/dimensions/resistance.h>
#include <units/physical/si/derived/voltage.h>
#include <units/physical/si/base/electric_current.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::physical::si {

struct ohm : named_unit<ohm, basic_symbol_text{"Ω", "ohm"}, prefix> {};
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

template<UnitOf<dim_resistance> U, QuantityValue Rep = double>
using resistance = quantity<dim_resistance, U, Rep>;

inline namespace literals {

// R
consteval auto operator"" _q_R(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return resistance<ohm, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_R(long double l) { return resistance<ohm, long double>(l); }

// yR
consteval auto operator"" _q_yR(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return resistance<yoctoohm, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_yR(long double l) { return resistance<yoctoohm, long double>(l); }

// zR
consteval auto operator"" _q_zR(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return resistance<zeptoohm, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_zR(long double l) { return resistance<zeptoohm, long double>(l); }

// aR
consteval auto operator"" _q_aR(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return resistance<attoohm, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_aR(long double l) { return resistance<attoohm, long double>(l); }

// fR
consteval auto operator"" _q_fR(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return resistance<femtoohm, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_fR(long double l) { return resistance<femtoohm, long double>(l); }

// pR
consteval auto operator"" _q_pR(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return resistance<picoohm, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_pR(long double l) { return resistance<picoohm, long double>(l); }

// nR
consteval auto operator"" _q_nR(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return resistance<nanoohm, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_nR(long double l) { return resistance<nanoohm, long double>(l); }

// uR
consteval auto operator"" _q_uR(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return resistance<microohm, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_uR(long double l) { return resistance<microohm, long double>(l); }

// mR
consteval auto operator"" _q_mR(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return resistance<milliohm, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_mR(long double l) { return resistance<milliohm, long double>(l); }

// kR
consteval auto operator"" _q_kR(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return resistance<kiloohm, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_kR(long double l) { return resistance<kiloohm, long double>(l); }

// MR
consteval auto operator"" _q_MR(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return resistance<megaohm, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_MR(long double l) { return resistance<megaohm, long double>(l); }

// GR
consteval auto operator"" _q_GR(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return resistance<gigaohm, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_GR(long double l) { return resistance<gigaohm, long double>(l); }

// TR
consteval auto operator"" _q_TR(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return resistance<teraohm, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_TR(long double l) { return resistance<teraohm, long double>(l); }

// PR
consteval auto operator"" _q_PR(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return resistance<petaohm, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_PR(long double l) { return resistance<petaohm, long double>(l); }

// ER
consteval auto operator"" _q_ER(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return resistance<exaohm, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_ER(long double l) { return resistance<exaohm, long double>(l); }

// ZR
consteval auto operator"" _q_ZR(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return resistance<zettaohm, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_ZR(long double l) { return resistance<zettaohm, long double>(l); }

// YR
consteval auto operator"" _q_YR(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return resistance<yottaohm, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_YR(long double l) { return resistance<yottaohm, long double>(l); }

}  // namespace literals

namespace unit_constants {

inline constexpr auto R = resistance<ohm, one_rep>{};
inline constexpr auto yR = resistance<yoctoohm, one_rep>{};
inline constexpr auto zR = resistance<zeptoohm, one_rep>{};
inline constexpr auto aR = resistance<attoohm, one_rep>{};
inline constexpr auto fR = resistance<femtoohm, one_rep>{};
inline constexpr auto pR = resistance<picoohm, one_rep>{};
inline constexpr auto nR = resistance<nanoohm, one_rep>{};
inline constexpr auto uR = resistance<microohm, one_rep>{};
inline constexpr auto mR = resistance<milliohm, one_rep>{};
inline constexpr auto kR = resistance<kiloohm, one_rep>{};
inline constexpr auto MR = resistance<megaohm, one_rep>{};
inline constexpr auto GR = resistance<gigaohm, one_rep>{};
inline constexpr auto TR = resistance<teraohm, one_rep>{};
inline constexpr auto PR = resistance<petaohm, one_rep>{};
inline constexpr auto ER = resistance<exaohm, one_rep>{};
inline constexpr auto ZR = resistance<zettaohm, one_rep>{};
inline constexpr auto YR = resistance<yottaohm, one_rep>{};

}  // namespace unit_constants

}  // namespace units::physical::si
