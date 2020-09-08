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
#include <units/physical/si/area.h>
#include <units/physical/si/force.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::physical::si {

struct pascal : named_unit<pascal, "Pa", prefix> {};
struct yoctopascal : prefixed_unit<yoctopascal, yocto, pascal> {};
struct zeptopascal : prefixed_unit<zeptopascal, zepto, pascal> {};
struct attopascal : prefixed_unit<attopascal, atto, pascal> {};
struct femtopascal : prefixed_unit<femtopascal, femto, pascal> {};
struct picopascal : prefixed_unit<picopascal, pico, pascal> {};
struct nanopascal : prefixed_unit<nanopascal, nano, pascal> {};
struct micropascal : prefixed_unit<micropascal, micro, pascal> {};
struct millipascal : prefixed_unit<millipascal, milli, pascal> {};
struct centipascal : prefixed_unit<centipascal, centi, pascal> {};
struct decipascal : prefixed_unit<decipascal, deci, pascal> {};
struct decapascal : prefixed_unit<decapascal, deca, pascal> {};
struct hectopascal : prefixed_unit<hectopascal, hecto, pascal> {};
struct kilopascal : prefixed_unit<kilopascal, kilo, pascal> {};
struct megapascal : prefixed_unit<megapascal, mega, pascal> {};
struct gigapascal : prefixed_unit<gigapascal, giga, pascal> {};
struct terapascal : prefixed_unit<terapascal, tera, pascal> {};
struct petapascal : prefixed_unit<petapascal, peta, pascal> {};
struct exapascal : prefixed_unit<exapascal, exa, pascal> {};
struct zettapascal : prefixed_unit<zettapascal, zetta, pascal> {};
struct yottapascal : prefixed_unit<yottapascal, yotta, pascal> {};

struct dim_pressure : physical::dim_pressure<dim_pressure, pascal, dim_force, dim_area> {};

template<Unit U, ScalableNumber Rep = double>
using pressure = quantity<dim_pressure, U, Rep>;

inline namespace literals {

// Pa
constexpr auto operator"" q_Pa(unsigned long long l) { return pressure<pascal, std::int64_t>(l); }
constexpr auto operator"" q_Pa(long double l) { return pressure<pascal, long double>(l); }

// yPa
constexpr auto operator"" q_yPa(unsigned long long l) { return pressure<yoctopascal, std::int64_t>(l); }
constexpr auto operator"" q_yPa(long double l) { return pressure<yoctopascal, long double>(l); }

// zPa
constexpr auto operator"" q_zPa(unsigned long long l) { return pressure<zeptopascal, std::int64_t>(l); }
constexpr auto operator"" q_zPa(long double l) { return pressure<zeptopascal, long double>(l); }

// aPa
constexpr auto operator"" q_aPa(unsigned long long l) { return pressure<attopascal, std::int64_t>(l); }
constexpr auto operator"" q_aPa(long double l) { return pressure<attopascal, long double>(l); }

// fPa
constexpr auto operator"" q_fPa(unsigned long long l) { return pressure<femtopascal, std::int64_t>(l); }
constexpr auto operator"" q_fPa(long double l) { return pressure<femtopascal, long double>(l); }

// pPa
constexpr auto operator"" q_pPa(unsigned long long l) { return pressure<picopascal, std::int64_t>(l); }
constexpr auto operator"" q_pPa(long double l) { return pressure<picopascal, long double>(l); }

// nPa
constexpr auto operator"" q_nPa(unsigned long long l) { return pressure<nanopascal, std::int64_t>(l); }
constexpr auto operator"" q_nPa(long double l) { return pressure<nanopascal, long double>(l); }

// uPa
constexpr auto operator"" q_uPa(unsigned long long l) { return pressure<micropascal, std::int64_t>(l); }
constexpr auto operator"" q_uPa(long double l) { return pressure<micropascal, long double>(l); }

// mPa
constexpr auto operator"" q_mPa(unsigned long long l) { return pressure<millipascal, std::int64_t>(l); }
constexpr auto operator"" q_mPa(long double l) { return pressure<millipascal, long double>(l); }

// cPa
constexpr auto operator"" q_cPa(unsigned long long l) { return pressure<centipascal, std::int64_t>(l); }
constexpr auto operator"" q_cPa(long double l) { return pressure<centipascal, long double>(l); }

// dPa
constexpr auto operator"" q_dPa(unsigned long long l) { return pressure<decipascal, std::int64_t>(l); }
constexpr auto operator"" q_dPa(long double l) { return pressure<decipascal, long double>(l); }

// daPa
constexpr auto operator"" q_daPa(unsigned long long l) { return pressure<decapascal, std::int64_t>(l); }
constexpr auto operator"" q_daPa(long double l) { return pressure<decapascal, long double>(l); }

// hPa
constexpr auto operator"" q_hPa(unsigned long long l) { return pressure<hectopascal, std::int64_t>(l); }
constexpr auto operator"" q_hPa(long double l) { return pressure<hectopascal, long double>(l); }

// kPa
constexpr auto operator"" q_kPa(unsigned long long l) { return pressure<kilopascal, std::int64_t>(l); }
constexpr auto operator"" q_kPa(long double l) { return pressure<kilopascal, long double>(l); }

// MPa
constexpr auto operator"" q_MPa(unsigned long long l) { return pressure<megapascal, std::int64_t>(l); }
constexpr auto operator"" q_MPa(long double l) { return pressure<megapascal, long double>(l); }

// GPa
constexpr auto operator"" q_GPa(unsigned long long l) { return pressure<gigapascal, std::int64_t>(l); }
constexpr auto operator"" q_GPa(long double l) { return pressure<gigapascal, long double>(l); }

// TPa
constexpr auto operator"" q_TPa(unsigned long long l) { return pressure<terapascal, std::int64_t>(l); }
constexpr auto operator"" q_TPa(long double l) { return pressure<terapascal, long double>(l); }

// PPa
constexpr auto operator"" q_PPa(unsigned long long l) { return pressure<petapascal, std::int64_t>(l); }
constexpr auto operator"" q_PPa(long double l) { return pressure<petapascal, long double>(l); }

// EPa
constexpr auto operator"" q_EPa(unsigned long long l) { return pressure<exapascal, std::int64_t>(l); }
constexpr auto operator"" q_EPa(long double l) { return pressure<exapascal, long double>(l); }

// ZPa
constexpr auto operator"" q_ZPa(unsigned long long l) { return pressure<zettapascal, std::int64_t>(l); }
constexpr auto operator"" q_ZPa(long double l) { return pressure<zettapascal, long double>(l); }

// YPa
constexpr auto operator"" q_YPa(unsigned long long l) { return pressure<yottapascal, std::int64_t>(l); }
constexpr auto operator"" q_YPa(long double l) { return pressure<yottapascal, long double>(l); }

}  // namespace literals

}  // namespace units::physical::si
