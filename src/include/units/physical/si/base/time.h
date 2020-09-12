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

#include <chrono>

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

template<Unit U, ScalableNumber Rep = double>
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

// std::chrono compatibility
namespace chrono_compat_detail
{
template <units::ratio R>
consteval
auto to_std_ratio_impl()
{
  constexpr
  auto pow_num = [](std::intmax_t num, std::intmax_t exp)
  {
    while (exp-- > 0) { num *= 10; }
    return num;
  };
  constexpr
  auto pow_den = [](std::intmax_t den, std::intmax_t exp)
  {
    while (exp++ < 0) { den *= 10; }
    return den;
  };
  return std::ratio<pow_num(R.num, R.exp), pow_den(R.den, R.exp)>{};
}
} // namespace chrono_compat_detail

template <units::Unit U, units::ScalableNumber Rep>
constexpr auto to_chrono_duration(units::physical::si::time<U, Rep> const & n)
{
  using resulting_ratio = decltype(chrono_compat_detail::to_std_ratio_impl<U::ratio>());
  return std::chrono::duration<Rep, resulting_ratio>{n.count()};
}

/**
 *  @brief Conversion from a Time to a std::chrono::duration
 *
 *  Converts the argument to a target Duration type. If unspecified, Duration is a
 *  `std::chrono::duration<Rep, Ratio>`, where `Ratio` is equivalent to `U::ratio`.
 *  For example:
 *
 *  auto time1 = units::physical::si::to_chrono_duration<std::chrono::milliseconds>(500_q_ms));
 *  auto time2 = units::physical::si::to_chrono_duration(500_q_ms);
 *
 *  @tparam Duration a target std::chrono::duration type to cast to
 */
template <typename Duration, units::Unit U, units::ScalableNumber Rep>
  requires requires{ std::chrono::duration_cast<Duration>(std::chrono::seconds{}); }
constexpr auto to_chrono_duration(units::physical::si::time<U, Rep> const & n)
{
  return std::chrono::duration_cast<Duration>(to_chrono_duration(n));
}

template <units::ScalableNumber Rep, std::intmax_t Num, std::intmax_t Den>
constexpr auto from_chrono_duration(std::chrono::duration<Rep, std::ratio<Num, Den>> const & time)
{
  constexpr auto ratio = units::ratio(Num, Den);
  using dimension = units::physical::si::dim_time;
  using unit = units::scaled_unit<ratio, units::physical::si::second>;

  return units::quantity<dimension, unit, Rep>{time.count()};
}

/**
 *  @brief Conversion from a std::chrono::duration to a Time
 *
 *  Converts the argument to a target Duration type. If unspecified, Duration is a
 *  `units::physical::si::time<units::scaled_unit<ratio, units::physical::si::seconds>>`,
 *  where `ratio` is equivalent to `time::period`
 *  For example:
 *
 *  using std::literals::operator""ms;
 *  auto time1 = units::physical::si::from_chrono_duration<units::physical::si::time<units::physical::si::second>>(500ms);
 *  auto time2 = units::physical::si::from_chrono_duration(500ms);
 *
 *  @tparam Duration a target type satisfying the Time concept to cast to
 */
template <units::physical::Time Duration, units::ScalableNumber Rep, std::intmax_t Num, std::intmax_t Den>
constexpr auto from_chrono_duration(std::chrono::duration<Rep, std::ratio<Num, Den>> const & time)
{
  return units::quantity_cast<Duration>(from_chrono_duration(time));
}

}  // namespace units::physical::si
