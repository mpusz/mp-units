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

#include <units/physical/dimensions/magnetic_induction.h>
#include <units/physical/si/derived/voltage.h>
#include <units/physical/si/base/time.h>
#include <units/physical/si/base/length.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::physical::si {

struct tesla : named_unit<tesla, "T", prefix> {};

struct yoctotesla : prefixed_unit<yoctotesla, yocto, tesla> {};
struct zeptotesla : prefixed_unit<zeptotesla, zepto, tesla> {};
struct attotesla : prefixed_unit<attotesla, atto, tesla> {};
struct femtotesla : prefixed_unit<femtotesla, femto, tesla> {};
struct picotesla : prefixed_unit<picotesla, pico, tesla> {};
struct nanotesla : prefixed_unit<nanotesla, nano, tesla> {};
struct microtesla : prefixed_unit<microtesla, micro, tesla> {};
struct millitesla : prefixed_unit<millitesla, milli, tesla> {};
struct kilotesla : prefixed_unit<kilotesla, kilo, tesla> {};
struct megatesla : prefixed_unit<megatesla, mega, tesla> {};
struct gigatesla : prefixed_unit<gigatesla, giga, tesla> {};
struct teratesla : prefixed_unit<teratesla, tera, tesla> {};
struct petatesla : prefixed_unit<petatesla, peta, tesla> {};
struct exatesla : prefixed_unit<exatesla, exa, tesla> {};
struct zettatesla : prefixed_unit<zettatesla, zetta, tesla> {};
struct yottatesla : prefixed_unit<yottatesla, yotta, tesla> {};

struct gauss : named_scaled_unit<gauss, "G", prefix, ratio(1, 10'000), tesla> {};

struct dim_magnetic_induction : physical::dim_magnetic_induction<dim_magnetic_induction, tesla, dim_voltage, dim_time, dim_length> {};

template<UnitOf<dim_magnetic_induction> U, QuantityValue Rep = double>
using magnetic_induction = quantity<dim_magnetic_induction, U, Rep>;

inline namespace literals {

// T
constexpr auto operator"" _q_T(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return magnetic_induction<tesla, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_T(long double l) { return magnetic_induction<tesla, long double>(l); }

// yT
constexpr auto operator"" _q_yT(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return magnetic_induction<yoctotesla, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_yT(long double l) { return magnetic_induction<yoctotesla, long double>(l); }

// zT
constexpr auto operator"" _q_zT(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return magnetic_induction<zeptotesla, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_zT(long double l) { return magnetic_induction<zeptotesla, long double>(l); }

// aT
constexpr auto operator"" _q_aT(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return magnetic_induction<attotesla, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_aT(long double l) { return magnetic_induction<attotesla, long double>(l); }

// fT
constexpr auto operator"" _q_fT(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return magnetic_induction<femtotesla, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_fT(long double l) { return magnetic_induction<femtotesla, long double>(l); }

// pT
constexpr auto operator"" _q_pT(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return magnetic_induction<picotesla, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_pT(long double l) { return magnetic_induction<picotesla, long double>(l); }

// nT
constexpr auto operator"" _q_nT(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return magnetic_induction<nanotesla, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_nT(long double l) { return magnetic_induction<nanotesla, long double>(l); }

// µT
constexpr auto operator"" _q_uT(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return magnetic_induction<microtesla, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_uT(long double l) { return magnetic_induction<microtesla, long double>(l); }

// mT
constexpr auto operator"" _q_mT(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return magnetic_induction<millitesla, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_mT(long double l) { return magnetic_induction<millitesla, long double>(l); }

// kT
constexpr auto operator"" _q_kT(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return magnetic_induction<kilotesla, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_kT(long double l) { return magnetic_induction<kilotesla, long double>(l); }

// MT
constexpr auto operator"" _q_MT(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return magnetic_induction<megatesla, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_MT(long double l) { return magnetic_induction<megatesla, long double>(l); }

// GT
constexpr auto operator"" _q_GT(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return magnetic_induction<gigatesla, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_GT(long double l) { return magnetic_induction<gigatesla, long double>(l); }

// TT
constexpr auto operator"" _q_TT(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return magnetic_induction<teratesla, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_TT(long double l) { return magnetic_induction<teratesla, long double>(l); }

// PT
constexpr auto operator"" _q_PT(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return magnetic_induction<petatesla, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_PT(long double l) { return magnetic_induction<petatesla, long double>(l); }

// ET
constexpr auto operator"" _q_ET(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return magnetic_induction<exatesla, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_ET(long double l) { return magnetic_induction<exatesla, long double>(l); }

// ZT
constexpr auto operator"" _q_ZT(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return magnetic_induction<zettatesla, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_ZT(long double l) { return magnetic_induction<zettatesla, long double>(l); }

// YT
constexpr auto operator"" _q_YT(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return magnetic_induction<yottatesla, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_YT(long double l) { return magnetic_induction<yottatesla, long double>(l); }

// G
constexpr auto operator"" _q_G(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return magnetic_induction<gauss, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_G(long double l) { return magnetic_induction<gauss, long double>(l); }

}  // namespace literals

namespace unit_constants {

inline constexpr auto T = magnetic_induction<tesla, one_rep>{};
inline constexpr auto yT = magnetic_induction<yoctotesla, one_rep>{};
inline constexpr auto zT = magnetic_induction<zeptotesla, one_rep>{};
inline constexpr auto aT = magnetic_induction<attotesla, one_rep>{};
inline constexpr auto fT = magnetic_induction<femtotesla, one_rep>{};
inline constexpr auto pT = magnetic_induction<picotesla, one_rep>{};
inline constexpr auto nT = magnetic_induction<nanotesla, one_rep>{};
inline constexpr auto uT = magnetic_induction<microtesla, one_rep>{};
inline constexpr auto mT = magnetic_induction<millitesla, one_rep>{};
inline constexpr auto kT = magnetic_induction<kilotesla, one_rep>{};
inline constexpr auto MT = magnetic_induction<megatesla, one_rep>{};
inline constexpr auto GT = magnetic_induction<gigatesla, one_rep>{};
inline constexpr auto TT = magnetic_induction<teratesla, one_rep>{};
inline constexpr auto PT = magnetic_induction<petatesla, one_rep>{};
inline constexpr auto ET = magnetic_induction<exatesla, one_rep>{};
inline constexpr auto ZT = magnetic_induction<zettatesla, one_rep>{};
inline constexpr auto YT = magnetic_induction<yottatesla, one_rep>{};
inline constexpr auto G = magnetic_induction<gauss, one_rep>{};

}  // namespace unit_constants

} // namespace units
