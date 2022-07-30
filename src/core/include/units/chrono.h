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

#include <units/customization_points.h>
// IWYU pragma: begin_exports
#include <units/isq/si/time.h>
#include <units/point_origin.h>
#include <chrono>
// IWYU pragma: end_exports

namespace units {

template<typename Rep, typename Period>
struct quantity_like_traits<std::chrono::duration<Rep, Period>> {
private:
  static constexpr auto mag = ::units::mag<ratio(Period::num, Period::den)>();
public:
  using dimension = isq::si::dim_time;
  using unit = downcast_unit<dimension, mag>;
  using rep = Rep;
  [[nodiscard]] static constexpr rep number(const std::chrono::duration<Rep, Period>& q) { return q.count(); }
};

template<typename C>
struct clock_origin : point_origin<isq::si::dim_time> {};

template<typename C, typename Rep, typename Period>
struct quantity_point_like_traits<std::chrono::time_point<C, std::chrono::duration<Rep, Period>>> {
private:
  static constexpr auto mag = ::units::mag<ratio(Period::num, Period::den)>();
public:
  using origin = clock_origin<C>;
  using unit = downcast_unit<typename origin::dimension, mag>;
  using rep = Rep;
  [[nodiscard]] static constexpr auto relative(const std::chrono::time_point<C, std::chrono::duration<Rep, Period>>& qp)
  {
    return qp.time_since_epoch();
  }
};

namespace detail {

template<typename C, typename Rep, typename Period>
inline constexpr bool is_quantity_point_like<std::chrono::time_point<C, std::chrono::duration<Rep, Period>>> = true;

constexpr std::intmax_t pow_10(std::intmax_t v)
{
  gsl_Expects(v > 0);
  std::intmax_t res = 1;
  for (std::intmax_t i = 0; i < v; i++) res *= 10;
  return res;
}

template<ratio R>
constexpr auto to_std_ratio_impl()
{
  return std::ratio<R.num, R.den>{};
}

}  // namespace detail

// TODO ICE below on gcc-11 when `ratio` is used instead of `auto`
template<auto R>
using to_std_ratio = decltype(detail::to_std_ratio_impl<R>());

template<typename U, typename Rep>
[[nodiscard]] constexpr auto to_std_duration(const quantity<isq::si::dim_time, U, Rep>& q)
{
  return std::chrono::duration<Rep, to_std_ratio<as_ratio(U::mag)>>(q.number());
}

template<typename C, typename U, typename Rep>
[[nodiscard]] constexpr auto to_std_time_point(const quantity_point<clock_origin<C>, U, Rep>& qp)
{
  using ret_type = std::chrono::time_point<C, std::chrono::duration<Rep, to_std_ratio<as_ratio(U::mag)>>>;
  return ret_type(to_std_duration(qp.relative()));
}

}  // namespace units
