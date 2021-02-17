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

#include <units/physical/dimensions/catalytic_activity.h>
#include <units/physical/si/base/amount_of_substance.h>
#include <units/physical/si/base/time.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::physical::si {

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

struct dim_catalytic_activity : physical::dim_catalytic_activity<dim_catalytic_activity, katal, dim_time, dim_amount_of_substance> {};

template<UnitOf<dim_catalytic_activity> U, QuantityValue Rep = double>
using catalytic_activity = quantity<dim_catalytic_activity, U, Rep>;

inline namespace literals {

// kat
consteval auto operator"" _q_kat(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return catalytic_activity<katal, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_kat(long double l) { return catalytic_activity<katal, long double>(l); }

// ykat
consteval auto operator"" _q_ykat(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return catalytic_activity<yoctokatal, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_ykat(long double l) { return catalytic_activity<yoctokatal, long double>(l); }

// zkat
consteval auto operator"" _q_zkat(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return catalytic_activity<zeptokatal, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_zkat(long double l) { return catalytic_activity<zeptokatal, long double>(l); }

// akat
consteval auto operator"" _q_akat(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return catalytic_activity<attokatal, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_akat(long double l) { return catalytic_activity<attokatal, long double>(l); }

// fkat
consteval auto operator"" _q_fkat(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return catalytic_activity<femtokatal, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_fkat(long double l) { return catalytic_activity<femtokatal, long double>(l); }

// pkat
consteval auto operator"" _q_pkat(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return catalytic_activity<picokatal, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_pkat(long double l) { return catalytic_activity<picokatal, long double>(l); }

// nkat
consteval auto operator"" _q_nkat(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return catalytic_activity<nanokatal, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_nkat(long double l) { return catalytic_activity<nanokatal, long double>(l); }

// ukat
consteval auto operator"" _q_ukat(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return catalytic_activity<microkatal, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_ukat(long double l) { return catalytic_activity<microkatal, long double>(l); }

// mkat
consteval auto operator"" _q_mkat(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return catalytic_activity<millikatal, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_mkat(long double l) { return catalytic_activity<millikatal, long double>(l); }

// ckat
consteval auto operator"" _q_ckat(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return catalytic_activity<centikatal, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_ckat(long double l) { return catalytic_activity<centikatal, long double>(l); }

// dkat
consteval auto operator"" _q_dkat(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return catalytic_activity<decikatal, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_dkat(long double l) { return catalytic_activity<decikatal, long double>(l); }

// dakat
consteval auto operator"" _q_dakat(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return catalytic_activity<decakatal, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_dakat(long double l) { return catalytic_activity<decakatal, long double>(l); }

// hkat
consteval auto operator"" _q_hkat(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return catalytic_activity<hectokatal, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_hkat(long double l) { return catalytic_activity<hectokatal, long double>(l); }

// kkat
consteval auto operator"" _q_kkat(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return catalytic_activity<kilokatal, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_kkat(long double l) { return catalytic_activity<kilokatal, long double>(l); }

// Mkat
consteval auto operator"" _q_Mkat(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return catalytic_activity<megakatal, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_Mkat(long double l) { return catalytic_activity<megakatal, long double>(l); }

// Gkat
consteval auto operator"" _q_Gkat(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return catalytic_activity<gigakatal, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_Gkat(long double l) { return catalytic_activity<gigakatal, long double>(l); }

// Tkat
consteval auto operator"" _q_Tkat(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return catalytic_activity<terakatal, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_Tkat(long double l) { return catalytic_activity<terakatal, long double>(l); }

// Pkat
consteval auto operator"" _q_Pkat(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return catalytic_activity<petakatal, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_Pkat(long double l) { return catalytic_activity<petakatal, long double>(l); }

// Ekat
consteval auto operator"" _q_Ekat(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return catalytic_activity<exakatal, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_Ekat(long double l) { return catalytic_activity<exakatal, long double>(l); }

// Zkat
consteval auto operator"" _q_Zkat(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return catalytic_activity<zettakatal, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_Zkat(long double l) { return catalytic_activity<zettakatal, long double>(l); }

// Ykat
consteval auto operator"" _q_Ykat(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return catalytic_activity<yottakatal, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_Ykat(long double l) { return catalytic_activity<yottakatal, long double>(l); }

// U
consteval auto operator"" _q_U(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return catalytic_activity<enzyme_unit, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_U(long double l) { return catalytic_activity<enzyme_unit, long double>(l); }

}  // namespace literals

namespace unit_constants {

inline constexpr auto kat = catalytic_activity<katal, one_rep>{};
inline constexpr auto ykat = catalytic_activity<yoctokatal, one_rep>{};
inline constexpr auto zkat = catalytic_activity<zeptokatal, one_rep>{};
inline constexpr auto akat = catalytic_activity<attokatal, one_rep>{};
inline constexpr auto fkat = catalytic_activity<femtokatal, one_rep>{};
inline constexpr auto pkat = catalytic_activity<picokatal, one_rep>{};
inline constexpr auto nkat = catalytic_activity<nanokatal, one_rep>{};
inline constexpr auto ukat = catalytic_activity<microkatal, one_rep>{};
inline constexpr auto mkat = catalytic_activity<millikatal, one_rep>{};
inline constexpr auto ckat = catalytic_activity<centikatal, one_rep>{};
inline constexpr auto dkat = catalytic_activity<decikatal, one_rep>{};
inline constexpr auto dakat = catalytic_activity<decakatal, one_rep>{};
inline constexpr auto hkat = catalytic_activity<hectokatal, one_rep>{};
inline constexpr auto kkat = catalytic_activity<kilokatal, one_rep>{};
inline constexpr auto Mkat = catalytic_activity<megakatal, one_rep>{};
inline constexpr auto Gkat = catalytic_activity<gigakatal, one_rep>{};
inline constexpr auto Tkat = catalytic_activity<terakatal, one_rep>{};
inline constexpr auto Pkat = catalytic_activity<petakatal, one_rep>{};
inline constexpr auto Ekat = catalytic_activity<exakatal, one_rep>{};
inline constexpr auto Zkat = catalytic_activity<zettakatal, one_rep>{};
inline constexpr auto Ykat = catalytic_activity<yottakatal, one_rep>{};
inline constexpr auto U = catalytic_activity<enzyme_unit, one_rep>{};

}  // namespace unit_constants

}  // namespace units::physical::si

