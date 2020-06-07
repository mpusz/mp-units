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
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::physical::si {

struct candela : named_unit<candela, "cd", prefix> {};
struct yoctocandela : prefixed_unit<yoctocandela, yocto, candela> {};
struct zeptocandela : prefixed_unit<zeptocandela, zepto, candela> {};
struct attocandela : prefixed_unit<attocandela, atto, candela> {};
struct femtocandela : prefixed_unit<femtocandela, femto, candela> {};
struct picocandela : prefixed_unit<picocandela, pico, candela> {};
struct nanocandela : prefixed_unit<nanocandela, nano, candela> {};
struct microcandela : prefixed_unit<microcandela, micro, candela> {};
struct millicandela : prefixed_unit<millicandela, milli, candela> {};
struct centicandela : prefixed_unit<centicandela, centi, candela> {};
struct decicandela : prefixed_unit<decicandela, deci, candela> {};
struct decacandela : prefixed_unit<decacandela, deca, candela> {};
struct hectocandela : prefixed_unit<hectocandela, hecto, candela> {};
struct kilocandela : prefixed_unit<kilocandela, kilo, candela> {};
struct megacandela : prefixed_unit<megacandela, mega, candela> {};
struct gigacandela : prefixed_unit<gigacandela, giga, candela> {};
struct teracandela : prefixed_unit<teracandela, tera, candela> {};
struct petacandela : prefixed_unit<petacandela, peta, candela> {};
struct exacandela : prefixed_unit<exacandela, exa, candela> {};
struct zettacandela : prefixed_unit<zettacandela, zetta, candela> {};
struct yottacandela : prefixed_unit<yottacandela, yotta, candela> {};

struct dim_luminous_intensity : physical::dim_luminous_intensity<candela> {};

template<in_unit U, in_numeric_value Rep = double>
using luminous_intensity = quantity<dim_luminous_intensity, U, Rep>;

inline namespace literals {

// cd
constexpr auto operator"" q_cd(unsigned long long l) { return luminous_intensity<candela, std::int64_t>(l); }
constexpr auto operator"" q_cd(long double l) { return luminous_intensity<candela, long double>(l); }

// ycd
constexpr auto operator"" q_ycd(unsigned long long l) { return luminous_intensity<yoctocandela, std::int64_t>(l); }
constexpr auto operator"" q_ycd(long double l) { return luminous_intensity<yoctocandela, long double>(l); }

// zcd
constexpr auto operator"" q_zcd(unsigned long long l) { return luminous_intensity<zeptocandela, std::int64_t>(l); }
constexpr auto operator"" q_zcd(long double l) { return luminous_intensity<zeptocandela, long double>(l); }

// acd
constexpr auto operator"" q_acd(unsigned long long l) { return luminous_intensity<attocandela, std::int64_t>(l); }
constexpr auto operator"" q_acd(long double l) { return luminous_intensity<attocandela, long double>(l); }

// fcd
constexpr auto operator"" q_fcd(unsigned long long l) { return luminous_intensity<femtocandela, std::int64_t>(l); }
constexpr auto operator"" q_fcd(long double l) { return luminous_intensity<femtocandela, long double>(l); }

// pcd
constexpr auto operator"" q_pcd(unsigned long long l) { return luminous_intensity<picocandela, std::int64_t>(l); }
constexpr auto operator"" q_pcd(long double l) { return luminous_intensity<picocandela, long double>(l); }

// ncd
constexpr auto operator"" q_ncd(unsigned long long l) { return luminous_intensity<nanocandela, std::int64_t>(l); }
constexpr auto operator"" q_ncd(long double l) { return luminous_intensity<nanocandela, long double>(l); }

// ucd
constexpr auto operator"" q_ucd(unsigned long long l) { return luminous_intensity<microcandela, std::int64_t>(l); }
constexpr auto operator"" q_ucd(long double l) { return luminous_intensity<microcandela, long double>(l); }

// mcd
constexpr auto operator"" q_mcd(unsigned long long l) { return luminous_intensity<millicandela, std::int64_t>(l); }
constexpr auto operator"" q_mcd(long double l) { return luminous_intensity<millicandela, long double>(l); }

// ccd
constexpr auto operator"" q_ccd(unsigned long long l) { return luminous_intensity<centicandela, std::int64_t>(l); }
constexpr auto operator"" q_ccd(long double l) { return luminous_intensity<centicandela, long double>(l); }

// dcd
constexpr auto operator"" q_dcd(unsigned long long l) { return luminous_intensity<decicandela, std::int64_t>(l); }
constexpr auto operator"" q_dcd(long double l) { return luminous_intensity<decicandela, long double>(l); }

// dacd
constexpr auto operator"" q_dacd(unsigned long long l) { return luminous_intensity<decacandela, std::int64_t>(l); }
constexpr auto operator"" q_dacd(long double l) { return luminous_intensity<decacandela, long double>(l); }

// hcd
constexpr auto operator"" q_hcd(unsigned long long l) { return luminous_intensity<hectocandela, std::int64_t>(l); }
constexpr auto operator"" q_hcd(long double l) { return luminous_intensity<hectocandela, long double>(l); }

// kcd
constexpr auto operator"" q_kcd(unsigned long long l) { return luminous_intensity<kilocandela, std::int64_t>(l); }
constexpr auto operator"" q_kcd(long double l) { return luminous_intensity<kilocandela, long double>(l); }

// Mcd
constexpr auto operator"" q_Mcd(unsigned long long l) { return luminous_intensity<megacandela, std::int64_t>(l); }
constexpr auto operator"" q_Mcd(long double l) { return luminous_intensity<megacandela, long double>(l); }

// Gcd
constexpr auto operator"" q_Gcd(unsigned long long l) { return luminous_intensity<gigacandela, std::int64_t>(l); }
constexpr auto operator"" q_Gcd(long double l) { return luminous_intensity<gigacandela, long double>(l); }

// Tcd
constexpr auto operator"" q_Tcd(unsigned long long l) { return luminous_intensity<teracandela, std::int64_t>(l); }
constexpr auto operator"" q_Tcd(long double l) { return luminous_intensity<teracandela, long double>(l); }

// Pcd
constexpr auto operator"" q_Pcd(unsigned long long l) { return luminous_intensity<petacandela, std::int64_t>(l); }
constexpr auto operator"" q_Pcd(long double l) { return luminous_intensity<petacandela, long double>(l); }

// Ecd
constexpr auto operator"" q_Ecd(unsigned long long l) { return luminous_intensity<exacandela, std::int64_t>(l); }
constexpr auto operator"" q_Ecd(long double l) { return luminous_intensity<exacandela, long double>(l); }

// Zcd
constexpr auto operator"" q_Zcd(unsigned long long l) { return luminous_intensity<zettacandela, std::int64_t>(l); }
constexpr auto operator"" q_Zcd(long double l) { return luminous_intensity<zettacandela, long double>(l); }

// Ycd
constexpr auto operator"" q_Ycd(unsigned long long l) { return luminous_intensity<yottacandela, std::int64_t>(l); }
constexpr auto operator"" q_Ycd(long double l) { return luminous_intensity<yottacandela, long double>(l); }

}  // namespace literals

}  // namespace units::physical::si
