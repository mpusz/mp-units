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
#include <units/physical/si/current.h>
#include <units/physical/si/power.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::physical::si {

struct volt : named_unit<volt, "V", prefix> {};
struct yoctovolt : prefixed_unit<yoctovolt, yocto, volt> {};
struct zeptovolt : prefixed_unit<zeptovolt, zepto, volt> {};
struct attovolt : prefixed_unit<attovolt, atto, volt> {};
struct femtovolt : prefixed_unit<femtovolt, femto, volt> {};
struct picovolt : prefixed_unit<picovolt, pico, volt> {};
struct nanovolt : prefixed_unit<nanovolt, nano, volt> {};
struct microvolt : prefixed_unit<microvolt, micro, volt> {};
struct millivolt : prefixed_unit<millivolt, milli, volt> {};
struct centivolt : prefixed_unit<centivolt, centi, volt> {};
struct decivolt : prefixed_unit<decivolt, deci, volt> {};
struct decavolt : prefixed_unit<decavolt, deca, volt> {};
struct hectovolt : prefixed_unit<hectovolt, hecto, volt> {};
struct kilovolt : prefixed_unit<kilovolt, kilo, volt> {};
struct megavolt : prefixed_unit<megavolt, mega, volt> {};
struct gigavolt : prefixed_unit<gigavolt, giga, volt> {};
struct teravolt : prefixed_unit<teravolt, tera, volt> {};
struct petavolt : prefixed_unit<petavolt, peta, volt> {};
struct exavolt : prefixed_unit<exavolt, exa, volt> {};
struct zettavolt : prefixed_unit<zettavolt, zetta, volt> {};
struct yottavolt : prefixed_unit<yottavolt, yotta, volt> {};

struct dim_voltage : physical::dim_voltage<dim_voltage, volt, dim_power, dim_electric_current> {};

template<in_unit U, in_numeric_value Rep = double>
using voltage = quantity<dim_voltage, U, Rep>;

inline namespace literals {

// V
constexpr auto operator"" q_V(unsigned long long l) { return voltage<volt, std::int64_t>(l); }
constexpr auto operator"" q_V(long double l) { return voltage<volt, long double>(l); }

// yV
constexpr auto operator"" q_yV(unsigned long long l) { return voltage<yoctovolt, std::int64_t>(l); }
constexpr auto operator"" q_yV(long double l) { return voltage<yoctovolt, long double>(l); }

// zV
constexpr auto operator"" q_zV(unsigned long long l) { return voltage<zeptovolt, std::int64_t>(l); }
constexpr auto operator"" q_zV(long double l) { return voltage<zeptovolt, long double>(l); }

// aV
constexpr auto operator"" q_aV(unsigned long long l) { return voltage<attovolt, std::int64_t>(l); }
constexpr auto operator"" q_aV(long double l) { return voltage<attovolt, long double>(l); }

// fV
constexpr auto operator"" q_fV(unsigned long long l) { return voltage<femtovolt, std::int64_t>(l); }
constexpr auto operator"" q_fV(long double l) { return voltage<femtovolt, long double>(l); }

// pV
constexpr auto operator"" q_pV(unsigned long long l) { return voltage<picovolt, std::int64_t>(l); }
constexpr auto operator"" q_pV(long double l) { return voltage<picovolt, long double>(l); }

// nV
constexpr auto operator"" q_nV(unsigned long long l) { return voltage<nanovolt, std::int64_t>(l); }
constexpr auto operator"" q_nV(long double l) { return voltage<nanovolt, long double>(l); }

// uV
constexpr auto operator"" q_uV(unsigned long long l) { return voltage<microvolt, std::int64_t>(l); }
constexpr auto operator"" q_uV(long double l) { return voltage<microvolt, long double>(l); }

// mV
constexpr auto operator"" q_mV(unsigned long long l) { return voltage<millivolt, std::int64_t>(l); }
constexpr auto operator"" q_mV(long double l) { return voltage<millivolt, long double>(l); }

// cV
constexpr auto operator"" q_cV(unsigned long long l) { return voltage<centivolt, std::int64_t>(l); }
constexpr auto operator"" q_cV(long double l) { return voltage<centivolt, long double>(l); }

// dV
constexpr auto operator"" q_dV(unsigned long long l) { return voltage<decivolt, std::int64_t>(l); }
constexpr auto operator"" q_dV(long double l) { return voltage<decivolt, long double>(l); }

// daV
constexpr auto operator"" q_daV(unsigned long long l) { return voltage<decavolt, std::int64_t>(l); }
constexpr auto operator"" q_daV(long double l) { return voltage<decavolt, long double>(l); }

// hV
constexpr auto operator"" q_hV(unsigned long long l) { return voltage<hectovolt, std::int64_t>(l); }
constexpr auto operator"" q_hV(long double l) { return voltage<hectovolt, long double>(l); }

// kV
constexpr auto operator"" q_kV(unsigned long long l) { return voltage<kilovolt, std::int64_t>(l); }
constexpr auto operator"" q_kV(long double l) { return voltage<kilovolt, long double>(l); }

// MV
constexpr auto operator"" q_MV(unsigned long long l) { return voltage<megavolt, std::int64_t>(l); }
constexpr auto operator"" q_MV(long double l) { return voltage<megavolt, long double>(l); }

// GV
constexpr auto operator"" q_GV(unsigned long long l) { return voltage<gigavolt, std::int64_t>(l); }
constexpr auto operator"" q_GV(long double l) { return voltage<gigavolt, long double>(l); }

// TV
constexpr auto operator"" q_TV(unsigned long long l) { return voltage<teravolt, std::int64_t>(l); }
constexpr auto operator"" q_TV(long double l) { return voltage<teravolt, long double>(l); }

// PV
constexpr auto operator"" q_PV(unsigned long long l) { return voltage<petavolt, std::int64_t>(l); }
constexpr auto operator"" q_PV(long double l) { return voltage<petavolt, long double>(l); }

// EV
constexpr auto operator"" q_EV(unsigned long long l) { return voltage<exavolt, std::int64_t>(l); }
constexpr auto operator"" q_EV(long double l) { return voltage<exavolt, long double>(l); }

// ZV
constexpr auto operator"" q_ZV(unsigned long long l) { return voltage<zettavolt, std::int64_t>(l); }
constexpr auto operator"" q_ZV(long double l) { return voltage<zettavolt, long double>(l); }

// YV
constexpr auto operator"" q_YV(unsigned long long l) { return voltage<yottavolt, std::int64_t>(l); }
constexpr auto operator"" q_YV(long double l) { return voltage<yottavolt, long double>(l); }

}  // namespace literals

}  // namespace units::physical::si
