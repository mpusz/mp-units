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
#include <units/physical/si/energy.h>
#include <units/physical/si/mass.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::physical::si {

struct gray : named_unit<gray, "Gy", prefix> {};
struct yoctogray : prefixed_unit<yoctogray, yocto, gray> {};
struct zeptogray : prefixed_unit<zeptogray, zepto, gray> {};
struct attogray : prefixed_unit<attogray, atto, gray> {};
struct femtogray : prefixed_unit<femtogray, femto, gray> {};
struct picogray : prefixed_unit<picogray, pico, gray> {};
struct nanogray : prefixed_unit<nanogray, nano, gray> {};
struct microgray : prefixed_unit<microgray, micro, gray> {};
struct milligray : prefixed_unit<milligray, milli, gray> {};
struct centigray : prefixed_unit<centigray, centi, gray> {};
struct decigray : prefixed_unit<decigray, deci, gray> {};
struct decagray : prefixed_unit<decagray, deca, gray> {};
struct hectogray : prefixed_unit<hectogray, hecto, gray> {};
struct kilogray : prefixed_unit<kilogray, kilo, gray> {};
struct megagray : prefixed_unit<megagray, mega, gray> {};
struct gigagray : prefixed_unit<gigagray, giga, gray> {};
struct teragray : prefixed_unit<teragray, tera, gray> {};
struct petagray : prefixed_unit<petagray, peta, gray> {};
struct exagray : prefixed_unit<exagray, exa, gray> {};
struct zettagray : prefixed_unit<zettagray, zetta, gray> {};
struct yottagray : prefixed_unit<yottagray, yotta, gray> {};

struct dim_absorbed_dose : physical::dim_absorbed_dose<dim_absorbed_dose, gray, dim_energy, dim_mass> {};

template<Unit U, ScalableNumber Rep = double>
using absorbed_dose = quantity<dim_absorbed_dose, U, Rep>;

inline namespace literals {

// G
constexpr auto operator"" _q_Gy(unsigned long long l) { return absorbed_dose<gray, std::int64_t>(l); }
constexpr auto operator"" _q_Gy(long double l) { return absorbed_dose<gray, long double>(l); }

// yGy
constexpr auto operator"" _q_yGy(unsigned long long l) { return absorbed_dose<yoctogray, std::int64_t>(l); }
constexpr auto operator"" _q_yGy(long double l) { return absorbed_dose<yoctogray, long double>(l); }

// zGy
constexpr auto operator"" _q_zGy(unsigned long long l) { return absorbed_dose<zeptogray, std::int64_t>(l); }
constexpr auto operator"" _q_zGy(long double l) { return absorbed_dose<zeptogray, long double>(l); }

// aGy
constexpr auto operator"" _q_aGy(unsigned long long l) { return absorbed_dose<attogray, std::int64_t>(l); }
constexpr auto operator"" _q_aGy(long double l) { return absorbed_dose<attogray, long double>(l); }

// fGy
constexpr auto operator"" _q_fGy(unsigned long long l) { return absorbed_dose<femtogray, std::int64_t>(l); }
constexpr auto operator"" _q_fGy(long double l) { return absorbed_dose<femtogray, long double>(l); }

// pGy
constexpr auto operator"" _q_pGy(unsigned long long l) { return absorbed_dose<picogray, std::int64_t>(l); }
constexpr auto operator"" _q_pGy(long double l) { return absorbed_dose<picogray, long double>(l); }

// nGy
constexpr auto operator"" _q_nGy(unsigned long long l) { return absorbed_dose<nanogray, std::int64_t>(l); }
constexpr auto operator"" _q_nGy(long double l) { return absorbed_dose<nanogray, long double>(l); }

// uGy
constexpr auto operator"" _q_uGy(unsigned long long l) { return absorbed_dose<microgray, std::int64_t>(l); }
constexpr auto operator"" _q_uGy(long double l) { return absorbed_dose<microgray, long double>(l); }

// mGy
constexpr auto operator"" _q_mGy(unsigned long long l) { return absorbed_dose<milligray, std::int64_t>(l); }
constexpr auto operator"" _q_mGy(long double l) { return absorbed_dose<milligray, long double>(l); }

// cGy
constexpr auto operator"" _q_cGy(unsigned long long l) { return absorbed_dose<centigray, std::int64_t>(l); }
constexpr auto operator"" _q_cGy(long double l) { return absorbed_dose<centigray, long double>(l); }

// dGy
constexpr auto operator"" _q_dGy(unsigned long long l) { return absorbed_dose<decigray, std::int64_t>(l); }
constexpr auto operator"" _q_dGy(long double l) { return absorbed_dose<decigray, long double>(l); }

// daGy
constexpr auto operator"" _q_daGy(unsigned long long l) { return absorbed_dose<decagray, std::int64_t>(l); }
constexpr auto operator"" _q_daGy(long double l) { return absorbed_dose<decagray, long double>(l); }

// hGy
constexpr auto operator"" _q_hGy(unsigned long long l) { return absorbed_dose<hectogray, std::int64_t>(l); }
constexpr auto operator"" _q_hGy(long double l) { return absorbed_dose<hectogray, long double>(l); }

// kGy
constexpr auto operator"" _q_kGy(unsigned long long l) { return absorbed_dose<kilogray, std::int64_t>(l); }
constexpr auto operator"" _q_kGy(long double l) { return absorbed_dose<kilogray, long double>(l); }

// MGy
constexpr auto operator"" _q_MGy(unsigned long long l) { return absorbed_dose<megagray, std::int64_t>(l); }
constexpr auto operator"" _q_MGy(long double l) { return absorbed_dose<megagray, long double>(l); }

// GGy
constexpr auto operator"" _q_GGy(unsigned long long l) { return absorbed_dose<gigagray, std::int64_t>(l); }
constexpr auto operator"" _q_GGy(long double l) { return absorbed_dose<gigagray, long double>(l); }

// TGy
constexpr auto operator"" _q_TGy(unsigned long long l) { return absorbed_dose<teragray, std::int64_t>(l); }
constexpr auto operator"" _q_TGy(long double l) { return absorbed_dose<teragray, long double>(l); }

// PGy
constexpr auto operator"" _q_PGy(unsigned long long l) { return absorbed_dose<petagray, std::int64_t>(l); }
constexpr auto operator"" _q_PGy(long double l) { return absorbed_dose<petagray, long double>(l); }

// EGy
constexpr auto operator"" _q_EGy(unsigned long long l) { return absorbed_dose<exagray, std::int64_t>(l); }
constexpr auto operator"" _q_EGy(long double l) { return absorbed_dose<exagray, long double>(l); }

// ZGy
constexpr auto operator"" _q_ZGy(unsigned long long l) { return absorbed_dose<zettagray, std::int64_t>(l); }
constexpr auto operator"" _q_ZGy(long double l) { return absorbed_dose<zettagray, long double>(l); }

// YGy
constexpr auto operator"" _q_YGy(unsigned long long l) { return absorbed_dose<yottagray, std::int64_t>(l); }
constexpr auto operator"" _q_YGy(long double l) { return absorbed_dose<yottagray, long double>(l); }

}  // namespace literals

}  // namespace units::physical::si
