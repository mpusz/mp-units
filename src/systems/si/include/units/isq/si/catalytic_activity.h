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

#include <units/isq/dimensions/catalytic_activity.h>
#include <units/isq/si/amount_of_substance.h>
#include <units/isq/si/time.h>
#include <units/isq/si/prefixes.h>
#include <units/quantity.h>

namespace units::isq::si {

struct katal : named_unit<katal, "kat", prefix> {};
struct yoctokatal : prefixed_unit<yoctokatal, yocto, katal> {};
struct zeptokatal : prefixed_unit<zeptokatal, zepto, katal> {};
struct attokatal : prefixed_unit<attokatal, atto, katal> {};
struct femtokatal : prefixed_unit<femtokatal, femto, katal> {};
struct picokatal : prefixed_unit<picokatal, pico, katal> {};
struct nanokatal : prefixed_unit<nanokatal, nano, katal> {};
struct microkatal : prefixed_unit<microkatal, micro, katal> {};
struct millikatal : prefixed_unit<millikatal, milli, katal> {};
struct centikatal : prefixed_unit<centikatal, centi, katal> {};
struct decikatal : prefixed_unit<decikatal, deci, katal> {};
struct decakatal : prefixed_unit<decakatal, deca, katal> {};
struct hectokatal : prefixed_unit<hectokatal, hecto, katal> {};
struct kilokatal : prefixed_unit<kilokatal, kilo, katal> {};
struct megakatal : prefixed_unit<megakatal, mega, katal> {};
struct gigakatal : prefixed_unit<gigakatal, giga, katal> {};
struct terakatal : prefixed_unit<terakatal, tera, katal> {};
struct petakatal : prefixed_unit<petakatal, peta, katal> {};
struct exakatal : prefixed_unit<exakatal, exa, katal> {};
struct zettakatal : prefixed_unit<zettakatal, zetta, katal> {};
struct yottakatal : prefixed_unit<yottakatal, yotta, katal> {};

struct enzyme_unit : named_scaled_unit<enzyme_unit, "U", prefix, ratio(1, 60, -6), katal> {};

struct dim_catalytic_activity : isq::dim_catalytic_activity<dim_catalytic_activity, katal, dim_time, dim_amount_of_substance> {};

template<UnitOf<dim_catalytic_activity> U, QuantityValue Rep = double>
using catalytic_activity = quantity<dim_catalytic_activity, U, Rep>;

inline namespace literals {

// kat
constexpr auto operator"" _q_kat(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return catalytic_activity<katal, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_kat(long double l) { return catalytic_activity<katal, long double>(l); }

// ykat
constexpr auto operator"" _q_ykat(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return catalytic_activity<yoctokatal, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_ykat(long double l) { return catalytic_activity<yoctokatal, long double>(l); }

// zkat
constexpr auto operator"" _q_zkat(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return catalytic_activity<zeptokatal, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_zkat(long double l) { return catalytic_activity<zeptokatal, long double>(l); }

// akat
constexpr auto operator"" _q_akat(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return catalytic_activity<attokatal, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_akat(long double l) { return catalytic_activity<attokatal, long double>(l); }

// fkat
constexpr auto operator"" _q_fkat(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return catalytic_activity<femtokatal, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_fkat(long double l) { return catalytic_activity<femtokatal, long double>(l); }

// pkat
constexpr auto operator"" _q_pkat(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return catalytic_activity<picokatal, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_pkat(long double l) { return catalytic_activity<picokatal, long double>(l); }

// nkat
constexpr auto operator"" _q_nkat(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return catalytic_activity<nanokatal, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_nkat(long double l) { return catalytic_activity<nanokatal, long double>(l); }

// ukat
constexpr auto operator"" _q_ukat(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return catalytic_activity<microkatal, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_ukat(long double l) { return catalytic_activity<microkatal, long double>(l); }

// mkat
constexpr auto operator"" _q_mkat(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return catalytic_activity<millikatal, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_mkat(long double l) { return catalytic_activity<millikatal, long double>(l); }

// ckat
constexpr auto operator"" _q_ckat(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return catalytic_activity<centikatal, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_ckat(long double l) { return catalytic_activity<centikatal, long double>(l); }

// dkat
constexpr auto operator"" _q_dkat(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return catalytic_activity<decikatal, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_dkat(long double l) { return catalytic_activity<decikatal, long double>(l); }

// dakat
constexpr auto operator"" _q_dakat(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return catalytic_activity<decakatal, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_dakat(long double l) { return catalytic_activity<decakatal, long double>(l); }

// hkat
constexpr auto operator"" _q_hkat(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return catalytic_activity<hectokatal, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_hkat(long double l) { return catalytic_activity<hectokatal, long double>(l); }

// kkat
constexpr auto operator"" _q_kkat(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return catalytic_activity<kilokatal, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_kkat(long double l) { return catalytic_activity<kilokatal, long double>(l); }

// Mkat
constexpr auto operator"" _q_Mkat(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return catalytic_activity<megakatal, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Mkat(long double l) { return catalytic_activity<megakatal, long double>(l); }

// Gkat
constexpr auto operator"" _q_Gkat(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return catalytic_activity<gigakatal, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Gkat(long double l) { return catalytic_activity<gigakatal, long double>(l); }

// Tkat
constexpr auto operator"" _q_Tkat(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return catalytic_activity<terakatal, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Tkat(long double l) { return catalytic_activity<terakatal, long double>(l); }

// Pkat
constexpr auto operator"" _q_Pkat(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return catalytic_activity<petakatal, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Pkat(long double l) { return catalytic_activity<petakatal, long double>(l); }

// Ekat
constexpr auto operator"" _q_Ekat(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return catalytic_activity<exakatal, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Ekat(long double l) { return catalytic_activity<exakatal, long double>(l); }

// Zkat
constexpr auto operator"" _q_Zkat(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return catalytic_activity<zettakatal, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Zkat(long double l) { return catalytic_activity<zettakatal, long double>(l); }

// Ykat
constexpr auto operator"" _q_Ykat(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return catalytic_activity<yottakatal, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Ykat(long double l) { return catalytic_activity<yottakatal, long double>(l); }

// U
constexpr auto operator"" _q_U(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return catalytic_activity<enzyme_unit, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_U(long double l) { return catalytic_activity<enzyme_unit, long double>(l); }

}  // namespace literals

namespace unit_constants {

inline constexpr auto kat = reference<dim_catalytic_activity, katal>{};
inline constexpr auto ykat = reference<dim_catalytic_activity, yoctokatal>{};
inline constexpr auto zkat = reference<dim_catalytic_activity, zeptokatal>{};
inline constexpr auto akat = reference<dim_catalytic_activity, attokatal>{};
inline constexpr auto fkat = reference<dim_catalytic_activity, femtokatal>{};
inline constexpr auto pkat = reference<dim_catalytic_activity, picokatal>{};
inline constexpr auto nkat = reference<dim_catalytic_activity, nanokatal>{};
inline constexpr auto ukat = reference<dim_catalytic_activity, microkatal>{};
inline constexpr auto mkat = reference<dim_catalytic_activity, millikatal>{};
inline constexpr auto ckat = reference<dim_catalytic_activity, centikatal>{};
inline constexpr auto dkat = reference<dim_catalytic_activity, decikatal>{};
inline constexpr auto dakat = reference<dim_catalytic_activity, decakatal>{};
inline constexpr auto hkat = reference<dim_catalytic_activity, hectokatal>{};
inline constexpr auto kkat = reference<dim_catalytic_activity, kilokatal>{};
inline constexpr auto Mkat = reference<dim_catalytic_activity, megakatal>{};
inline constexpr auto Gkat = reference<dim_catalytic_activity, gigakatal>{};
inline constexpr auto Tkat = reference<dim_catalytic_activity, terakatal>{};
inline constexpr auto Pkat = reference<dim_catalytic_activity, petakatal>{};
inline constexpr auto Ekat = reference<dim_catalytic_activity, exakatal>{};
inline constexpr auto Zkat = reference<dim_catalytic_activity, zettakatal>{};
inline constexpr auto Ykat = reference<dim_catalytic_activity, yottakatal>{};
inline constexpr auto U = reference<dim_catalytic_activity, enzyme_unit>{};

}  // namespace unit_constants

}  // namespace units::isq::si

