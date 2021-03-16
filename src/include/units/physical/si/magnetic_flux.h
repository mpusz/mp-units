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

#include <units/physical/dimensions/magnetic_flux.h>
#include <units/physical/si/magnetic_induction.h>
#include <units/physical/si/area.h>
#include <units/quantity.h>

namespace units::physical::si {

struct weber : named_unit<weber, "Wb", prefix> {};

struct yoctoweber : prefixed_unit<yoctoweber, yocto, weber> {};
struct zeptoweber : prefixed_unit<zeptoweber, zepto, weber> {};
struct attoweber : prefixed_unit<attoweber, atto, weber> {};
struct femtoweber : prefixed_unit<femtoweber, femto, weber> {};
struct picoweber : prefixed_unit<picoweber, pico, weber> {};
struct nanoweber : prefixed_unit<nanoweber, nano, weber> {};
struct microweber : prefixed_unit<microweber, micro, weber> {};
struct milliweber : prefixed_unit<milliweber, milli, weber> {};
struct kiloweber : prefixed_unit<kiloweber, kilo, weber> {};
struct megaweber : prefixed_unit<megaweber, mega, weber> {};
struct gigaweber : prefixed_unit<gigaweber, giga, weber> {};
struct teraweber : prefixed_unit<teraweber, tera, weber> {};
struct petaweber : prefixed_unit<petaweber, peta, weber> {};
struct exaweber : prefixed_unit<exaweber, exa, weber> {};
struct zettaweber : prefixed_unit<zettaweber, zetta, weber> {};
struct yottaweber : prefixed_unit<yottaweber, yotta, weber> {};

struct dim_magnetic_flux : physical::dim_magnetic_flux<dim_magnetic_flux, weber, dim_magnetic_induction, dim_area> {};

template<UnitOf<dim_magnetic_flux> U, QuantityValue Rep = double>
using magnetic_flux = quantity<dim_magnetic_flux, U, Rep>;

inline namespace literals {

// Wb
constexpr auto operator"" _q_Wb(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return magnetic_flux<weber, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Wb(long double l) { return magnetic_flux<weber, long double>(l); }

// yWb
constexpr auto operator"" _q_yWb(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return magnetic_flux<yoctoweber, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_yWb(long double l) { return magnetic_flux<yoctoweber, long double>(l); }

// zWb
constexpr auto operator"" _q_zWb(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return magnetic_flux<zeptoweber, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_zWb(long double l) { return magnetic_flux<zeptoweber, long double>(l); }

// aWb
constexpr auto operator"" _q_aWb(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return magnetic_flux<attoweber, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_aWb(long double l) { return magnetic_flux<attoweber, long double>(l); }

// fWb
constexpr auto operator"" _q_fWb(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return magnetic_flux<femtoweber, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_fWb(long double l) { return magnetic_flux<femtoweber, long double>(l); }

// pWb
constexpr auto operator"" _q_pWb(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return magnetic_flux<picoweber, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_pWb(long double l) { return magnetic_flux<picoweber, long double>(l); }

// nWb
constexpr auto operator"" _q_nWb(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return magnetic_flux<nanoweber, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_nWb(long double l) { return magnetic_flux<nanoweber, long double>(l); }

// µWb
constexpr auto operator"" _q_uWb(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return magnetic_flux<microweber, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_uWb(long double l) { return magnetic_flux<microweber, long double>(l); }

// mWb
constexpr auto operator"" _q_mWb(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return magnetic_flux<milliweber, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_mWb(long double l) { return magnetic_flux<milliweber, long double>(l); }

// kWb
constexpr auto operator"" _q_kWb(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return magnetic_flux<kiloweber, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_kWb(long double l) { return magnetic_flux<kiloweber, long double>(l); }

// MWb
constexpr auto operator"" _q_MWb(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return magnetic_flux<megaweber, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_MWb(long double l) { return magnetic_flux<megaweber, long double>(l); }

// GWb
constexpr auto operator"" _q_GWb(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return magnetic_flux<gigaweber, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_GWb(long double l) { return magnetic_flux<gigaweber, long double>(l); }

// TWb
constexpr auto operator"" _q_TWb(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return magnetic_flux<teraweber, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_TWb(long double l) { return magnetic_flux<teraweber, long double>(l); }

// PWb
constexpr auto operator"" _q_PWb(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return magnetic_flux<petaweber, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_PWb(long double l) { return magnetic_flux<petaweber, long double>(l); }

// EWb
constexpr auto operator"" _q_EWb(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return magnetic_flux<exaweber, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_EWb(long double l) { return magnetic_flux<exaweber, long double>(l); }

// ZWb
constexpr auto operator"" _q_ZWb(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return magnetic_flux<zettaweber, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_ZWb(long double l) { return magnetic_flux<zettaweber, long double>(l); }

// YWb
constexpr auto operator"" _q_YWb(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return magnetic_flux<yottaweber, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_YWb(long double l) { return magnetic_flux<yottaweber, long double>(l); }

}  // namespace literals

namespace unit_constants {

inline constexpr auto Wb = magnetic_flux<weber, one_rep>{};
inline constexpr auto yWb = magnetic_flux<yoctoweber, one_rep>{};
inline constexpr auto zWb = magnetic_flux<zeptoweber, one_rep>{};
inline constexpr auto aWb = magnetic_flux<attoweber, one_rep>{};
inline constexpr auto fWb = magnetic_flux<femtoweber, one_rep>{};
inline constexpr auto pWb = magnetic_flux<picoweber, one_rep>{};
inline constexpr auto nWb = magnetic_flux<nanoweber, one_rep>{};
inline constexpr auto uWb = magnetic_flux<microweber, one_rep>{};
inline constexpr auto mWb = magnetic_flux<milliweber, one_rep>{};
inline constexpr auto kWb = magnetic_flux<kiloweber, one_rep>{};
inline constexpr auto MWb = magnetic_flux<megaweber, one_rep>{};
inline constexpr auto GWb = magnetic_flux<gigaweber, one_rep>{};
inline constexpr auto TWb = magnetic_flux<teraweber, one_rep>{};
inline constexpr auto PWb = magnetic_flux<petaweber, one_rep>{};
inline constexpr auto EWb = magnetic_flux<exaweber, one_rep>{};
inline constexpr auto ZWb = magnetic_flux<zettaweber, one_rep>{};
inline constexpr auto YWb = magnetic_flux<yottaweber, one_rep>{};

}  // namespace unit_constants

}  // namespace units::physical::si
