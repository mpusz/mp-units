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

#include <units/physical/dimensions/power.h>
#include <units/physical/si/derived/energy.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::physical::si {

struct watt : named_unit<watt, "W", prefix> {};
struct yoctowatt : prefixed_unit<yoctowatt, yocto, watt> {};
struct zeptowatt : prefixed_unit<zeptowatt, zepto, watt> {};
struct attowatt : prefixed_unit<attowatt, atto, watt> {};
struct femtowatt : prefixed_unit<femtowatt, femto, watt> {};
struct picowatt : prefixed_unit<picowatt, pico, watt> {};
struct nanowatt : prefixed_unit<nanowatt, nano, watt> {};
struct microwatt : prefixed_unit<microwatt, micro, watt> {};
struct milliwatt : prefixed_unit<milliwatt, milli, watt> {};
struct kilowatt : prefixed_unit<kilowatt, kilo, watt> {};
struct megawatt : prefixed_unit<megawatt, mega, watt> {};
struct gigawatt : prefixed_unit<gigawatt, giga, watt> {};
struct terawatt : prefixed_unit<terawatt, tera, watt> {};
struct petawatt : prefixed_unit<petawatt, peta, watt> {};
struct exawatt : prefixed_unit<exawatt, exa, watt> {};
struct zettawatt : prefixed_unit<zettawatt, zetta, watt> {};
struct yottawatt : prefixed_unit<yottawatt, yotta, watt> {};

struct dim_power : physical::dim_power<dim_power, watt, dim_energy, dim_time> {};

template<UnitOf<dim_power> U, QuantityValue Rep = double>
using power = quantity<dim_power, U, Rep>;

inline namespace literals {

// W
constexpr auto operator"" _q_W(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return power<watt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_W(long double l) { return power<watt, long double>(l); }

// yW
constexpr auto operator"" _q_yW(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return power<yoctowatt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_yW(long double l) { return power<yoctowatt, long double>(l); }

// zW
constexpr auto operator"" _q_zW(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return power<zeptowatt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_zW(long double l) { return power<zeptowatt, long double>(l); }

// aW
constexpr auto operator"" _q_aW(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return power<attowatt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_aW(long double l) { return power<attowatt, long double>(l); }

// fW
constexpr auto operator"" _q_fW(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return power<femtowatt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_fW(long double l) { return power<femtowatt, long double>(l); }

// pW
constexpr auto operator"" _q_pW(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return power<picowatt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_pW(long double l) { return power<picowatt, long double>(l); }

// nW
constexpr auto operator"" _q_nW(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return power<nanowatt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_nW(long double l) { return power<nanowatt, long double>(l); }

// uW
constexpr auto operator"" _q_uW(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return power<microwatt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_uW(long double l) { return power<microwatt, long double>(l); }

// mW
constexpr auto operator"" _q_mW(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return power<milliwatt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_mW(long double l) { return power<milliwatt, long double>(l); }

// kW
constexpr auto operator"" _q_kW(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return power<kilowatt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_kW(long double l) { return power<kilowatt, long double>(l); }

// MW
constexpr auto operator"" _q_MW(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return power<megawatt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_MW(long double l) { return power<megawatt, long double>(l); }

// GW
constexpr auto operator"" _q_GW(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return power<gigawatt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_GW(long double l) { return power<gigawatt, long double>(l); }

// TW
constexpr auto operator"" _q_TW(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return power<terawatt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_TW(long double l) { return power<terawatt, long double>(l); }

// PW
constexpr auto operator"" _q_PW(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return power<petawatt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_PW(long double l) { return power<petawatt, long double>(l); }

// EW
constexpr auto operator"" _q_EW(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return power<exawatt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_EW(long double l) { return power<exawatt, long double>(l); }

// ZW
constexpr auto operator"" _q_ZW(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return power<zettawatt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_ZW(long double l) { return power<zettawatt, long double>(l); }

// YW
constexpr auto operator"" _q_YW(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return power<yottawatt, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_YW(long double l) { return power<yottawatt, long double>(l); }

}  // namespace literals

namespace unit_constants {

inline constexpr auto W = power<watt, one_rep>{};
inline constexpr auto yW = power<yoctowatt, one_rep>{};
inline constexpr auto zW = power<zeptowatt, one_rep>{};
inline constexpr auto aW = power<attowatt, one_rep>{};
inline constexpr auto fW = power<femtowatt, one_rep>{};
inline constexpr auto pW = power<picowatt, one_rep>{};
inline constexpr auto nW = power<nanowatt, one_rep>{};
inline constexpr auto uW = power<microwatt, one_rep>{};
inline constexpr auto mW = power<milliwatt, one_rep>{};
inline constexpr auto kW = power<kilowatt, one_rep>{};
inline constexpr auto MW = power<megawatt, one_rep>{};
inline constexpr auto GW = power<gigawatt, one_rep>{};
inline constexpr auto TW = power<terawatt, one_rep>{};
inline constexpr auto PW = power<petawatt, one_rep>{};
inline constexpr auto EW = power<exawatt, one_rep>{};
inline constexpr auto ZW = power<zettawatt, one_rep>{};
inline constexpr auto YW = power<yottawatt, one_rep>{};

}  // namespace unit_constants

}  // namespace units::physical::si
