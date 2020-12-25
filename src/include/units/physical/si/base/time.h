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

#include <units/bits/one_rep.h>
#include <units/physical/dimensions/time.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::physical::si {

struct second : named_unit<second, "s", prefix> {};
struct yoctosecond : prefixed_unit<yoctosecond, yocto, second> {};
struct zeptosecond : prefixed_unit<zeptosecond, zepto, second> {};
struct attosecond : prefixed_unit<attosecond, atto, second> {};
struct femtosecond : prefixed_unit<femtosecond, femto, second> {};
struct picosecond : prefixed_unit<picosecond, pico, second> {};
struct nanosecond : prefixed_unit<nanosecond, nano, second> {};
struct microsecond : prefixed_unit<microsecond, micro, second> {};
struct millisecond : prefixed_unit<millisecond, milli, second> {};
struct minute : named_scaled_unit<minute, "min", no_prefix, ratio(60), second> {};
struct hour : named_scaled_unit<hour, "h", no_prefix, ratio(60), minute> {};
struct day : named_scaled_unit<day, "d", no_prefix, ratio(24), hour> {};

struct dim_time : physical::dim_time<second> {};

template<UnitOf<dim_time> U, QuantityValue Rep = double>
using time = quantity<dim_time, U, Rep>;

inline namespace literals {

// ys
constexpr auto operator"" _q_ys(unsigned long long l) { return time<yoctosecond, std::int64_t>(l); }
constexpr auto operator"" _q_ys(long double l) { return time<yoctosecond, long double>(l); }

// zs
constexpr auto operator"" _q_zs(unsigned long long l) { return time<zeptosecond, std::int64_t>(l); }
constexpr auto operator"" _q_zs(long double l) { return time<zeptosecond, long double>(l); }

// as
constexpr auto operator"" _q_as(unsigned long long l) { return time<attosecond, std::int64_t>(l); }
constexpr auto operator"" _q_as(long double l) { return time<attosecond, long double>(l); }

// fs
constexpr auto operator"" _q_fs(unsigned long long l) { return time<femtosecond, std::int64_t>(l); }
constexpr auto operator"" _q_fs(long double l) { return time<femtosecond, long double>(l); }

// ps
constexpr auto operator"" _q_ps(unsigned long long l) { return time<picosecond, std::int64_t>(l); }
constexpr auto operator"" _q_ps(long double l) { return time<picosecond, long double>(l); }

// ns
constexpr auto operator"" _q_ns(unsigned long long l) { return time<nanosecond, std::int64_t>(l); }
constexpr auto operator"" _q_ns(long double l) { return time<nanosecond, long double>(l); }

// us
constexpr auto operator"" _q_us(unsigned long long l) { return time<microsecond, std::int64_t>(l); }
constexpr auto operator"" _q_us(long double l) { return time<microsecond, long double>(l); }

// ms
constexpr auto operator"" _q_ms(unsigned long long l) { return time<millisecond, std::int64_t>(l); }
constexpr auto operator"" _q_ms(long double l) { return time<millisecond, long double>(l); }

// s
constexpr auto operator"" _q_s(unsigned long long l) { return time<second, std::int64_t>(l); }
constexpr auto operator"" _q_s(long double l) { return time<second, long double>(l); }

// min
constexpr auto operator"" _q_min(unsigned long long l) { return time<minute, std::int64_t>(l); }
constexpr auto operator"" _q_min(long double l) { return time<minute, long double>(l); }

// h
constexpr auto operator"" _q_h(unsigned long long l) { return time<hour, std::int64_t>(l); }
constexpr auto operator"" _q_h(long double l) { return time<hour, long double>(l); }

// d
constexpr auto operator"" _q_d(unsigned long long l) { return time<day, std::int64_t>(l); }
constexpr auto operator"" _q_d(long double l) { return time<day, long double>(l); }

}  // namespace literals

namespace unit_constants {

inline constexpr auto ys = time<yoctosecond, detail::one_rep>{};
inline constexpr auto zs = time<zeptosecond, detail::one_rep>{};
inline constexpr auto as = time<attosecond, detail::one_rep>{};
inline constexpr auto fs = time<femtosecond, detail::one_rep>{};
inline constexpr auto ps = time<picosecond, detail::one_rep>{};
inline constexpr auto ns = time<nanosecond, detail::one_rep>{};
inline constexpr auto us = time<microsecond, detail::one_rep>{};
inline constexpr auto ms = time<millisecond, detail::one_rep>{};
inline constexpr auto s = time<second, detail::one_rep>{};
inline constexpr auto min = time<minute, detail::one_rep>{};
inline constexpr auto h = time<hour, detail::one_rep>{};
inline constexpr auto d = time<day, detail::one_rep>{};

}  // namespace unit_constants

}  // namespace units::physical::si
