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

#include <units/isq/dimensions/voltage.h>
#include <units/isq/si/electric_current.h>
#include <units/isq/si/power.h>
#include <units/isq/si/prefixes.h>
#include <units/quantity.h>

namespace units::isq::si {

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

struct dim_voltage : isq::dim_voltage<dim_voltage, volt, dim_power, dim_electric_current> {};

template<UnitOf<dim_voltage> U, QuantityValue Rep = double>
using voltage = quantity<dim_voltage, U, Rep>;

inline namespace literals {

// V
constexpr auto operator"" _q_V(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return voltage<volt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_V(long double l) { return voltage<volt, long double>(l); }

// yV
constexpr auto operator"" _q_yV(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return voltage<yoctovolt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_yV(long double l) { return voltage<yoctovolt, long double>(l); }

// zV
constexpr auto operator"" _q_zV(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return voltage<zeptovolt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_zV(long double l) { return voltage<zeptovolt, long double>(l); }

// aV
constexpr auto operator"" _q_aV(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return voltage<attovolt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_aV(long double l) { return voltage<attovolt, long double>(l); }

// fV
constexpr auto operator"" _q_fV(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return voltage<femtovolt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_fV(long double l) { return voltage<femtovolt, long double>(l); }

// pV
constexpr auto operator"" _q_pV(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return voltage<picovolt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_pV(long double l) { return voltage<picovolt, long double>(l); }

// nV
constexpr auto operator"" _q_nV(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return voltage<nanovolt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_nV(long double l) { return voltage<nanovolt, long double>(l); }

// uV
constexpr auto operator"" _q_uV(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return voltage<microvolt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_uV(long double l) { return voltage<microvolt, long double>(l); }

// mV
constexpr auto operator"" _q_mV(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return voltage<millivolt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_mV(long double l) { return voltage<millivolt, long double>(l); }

// cV
constexpr auto operator"" _q_cV(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return voltage<centivolt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_cV(long double l) { return voltage<centivolt, long double>(l); }

// dV
constexpr auto operator"" _q_dV(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return voltage<decivolt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_dV(long double l) { return voltage<decivolt, long double>(l); }

// daV
constexpr auto operator"" _q_daV(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return voltage<decavolt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_daV(long double l) { return voltage<decavolt, long double>(l); }

// hV
constexpr auto operator"" _q_hV(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return voltage<hectovolt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_hV(long double l) { return voltage<hectovolt, long double>(l); }

// kV
constexpr auto operator"" _q_kV(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return voltage<kilovolt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_kV(long double l) { return voltage<kilovolt, long double>(l); }

// MV
constexpr auto operator"" _q_MV(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return voltage<megavolt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_MV(long double l) { return voltage<megavolt, long double>(l); }

// GV
constexpr auto operator"" _q_GV(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return voltage<gigavolt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_GV(long double l) { return voltage<gigavolt, long double>(l); }

// TV
constexpr auto operator"" _q_TV(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return voltage<teravolt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_TV(long double l) { return voltage<teravolt, long double>(l); }

// PV
constexpr auto operator"" _q_PV(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return voltage<petavolt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_PV(long double l) { return voltage<petavolt, long double>(l); }

// EV
constexpr auto operator"" _q_EV(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return voltage<exavolt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_EV(long double l) { return voltage<exavolt, long double>(l); }

// ZV
constexpr auto operator"" _q_ZV(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return voltage<zettavolt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_ZV(long double l) { return voltage<zettavolt, long double>(l); }

// YV
constexpr auto operator"" _q_YV(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return voltage<yottavolt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_YV(long double l) { return voltage<yottavolt, long double>(l); }

}  // namespace literals

namespace references {

inline constexpr auto V = reference<dim_voltage, volt>{};
inline constexpr auto yV = reference<dim_voltage, yoctovolt>{};
inline constexpr auto zV = reference<dim_voltage, zeptovolt>{};
inline constexpr auto aV = reference<dim_voltage, attovolt>{};
inline constexpr auto fV = reference<dim_voltage, femtovolt>{};
inline constexpr auto pV = reference<dim_voltage, picovolt>{};
inline constexpr auto nV = reference<dim_voltage, nanovolt>{};
inline constexpr auto uV = reference<dim_voltage, microvolt>{};
inline constexpr auto mV = reference<dim_voltage, millivolt>{};
inline constexpr auto cV = reference<dim_voltage, centivolt>{};
inline constexpr auto dV = reference<dim_voltage, decivolt>{};
inline constexpr auto daV = reference<dim_voltage, decavolt>{};
inline constexpr auto hV = reference<dim_voltage, hectovolt>{};
inline constexpr auto kV = reference<dim_voltage, kilovolt>{};
inline constexpr auto MV = reference<dim_voltage, megavolt>{};
inline constexpr auto GV = reference<dim_voltage, gigavolt>{};
inline constexpr auto TV = reference<dim_voltage, teravolt>{};
inline constexpr auto PV = reference<dim_voltage, petavolt>{};
inline constexpr auto EV = reference<dim_voltage, exavolt>{};
inline constexpr auto ZV = reference<dim_voltage, zettavolt>{};
inline constexpr auto YV = reference<dim_voltage, yottavolt>{};

}  // namespace references

}  // namespace units::isq::si
