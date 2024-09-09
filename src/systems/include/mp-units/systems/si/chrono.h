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

#include <mp-units/bits/requires_hosted.h>
//
#include <mp-units/bits/module_macros.h>
#include <mp-units/systems/isq/si_quantities.h>
#include <mp-units/systems/si/prefixes.h>
#include <mp-units/systems/si/units.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/quantity_point.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <chrono>
#endif
#endif

namespace mp_units {

namespace detail {

template<typename Period>
[[nodiscard]] consteval auto time_unit_from_chrono_period()
{
  using namespace si;

  if constexpr (is_same_v<Period, std::chrono::nanoseconds::period>)
    return nano<second>;
  else if constexpr (is_same_v<Period, std::chrono::microseconds::period>)
    return micro<second>;
  else if constexpr (is_same_v<Period, std::chrono::milliseconds::period>)
    return milli<second>;
  else if constexpr (is_same_v<Period, std::chrono::seconds::period>)
    return second;
  else if constexpr (is_same_v<Period, std::chrono::minutes::period>)
    return minute;
  else if constexpr (is_same_v<Period, std::chrono::hours::period>)
    return hour;
  else if constexpr (is_same_v<Period, std::chrono::days::period>)
    return day;
  else if constexpr (is_same_v<Period, std::chrono::weeks::period>)
    return mag<7> * day;
  else
    return mag_ratio<Period::num, Period::den> * second;
}

}  // namespace detail

MP_UNITS_EXPORT template<typename Rep, typename Period>
struct quantity_like_traits<std::chrono::duration<Rep, Period>> {
  static constexpr auto reference = detail::time_unit_from_chrono_period<Period>();
  using rep = Rep;

  [[nodiscard]] static constexpr convert_implicitly<rep> to_numerical_value(
    const std::chrono::duration<Rep, Period>& q) noexcept(std::is_nothrow_copy_constructible_v<rep>)
  {
    return q.count();
  }

  [[nodiscard]] static constexpr convert_implicitly<std::chrono::duration<Rep, Period>> from_numerical_value(
    const rep& v) noexcept(std::is_nothrow_copy_constructible_v<rep>)
  {
    return std::chrono::duration<Rep, Period>(v);
  }
};

template<typename C>
struct chrono_point_origin_ final : absolute_point_origin<isq::time> {
  using clock = C;
};
MP_UNITS_EXPORT template<typename C>
constexpr chrono_point_origin_<C> chrono_point_origin;

MP_UNITS_EXPORT_BEGIN

template<typename C, typename Rep, typename Period>
struct quantity_point_like_traits<std::chrono::time_point<C, std::chrono::duration<Rep, Period>>> {
  using T = std::chrono::time_point<C, std::chrono::duration<Rep, Period>>;
  static constexpr auto reference = detail::time_unit_from_chrono_period<Period>();
  static constexpr auto point_origin = chrono_point_origin<C>;
  using rep = Rep;

  [[nodiscard]] static constexpr convert_implicitly<rep> to_numerical_value(const T& tp) noexcept(
    std::is_nothrow_copy_constructible_v<rep>)
  {
    return tp.time_since_epoch().count();
  }

  [[nodiscard]] static constexpr convert_implicitly<T> from_numerical_value(const rep& v) noexcept(
    std::is_nothrow_copy_constructible_v<rep>)
  {
    return T(std::chrono::duration<Rep, Period>(v));
  }
};

namespace detail {

[[nodiscard]] constexpr auto as_ratio(Magnitude auto m)
  requires(is_rational(m))
{
  return std::ratio<get_value<std::intmax_t>(numerator(m)), get_value<std::intmax_t>(denominator(m))>{};
}

}  // namespace detail

template<QuantityOf<MP_UNITS_IS_VALUE_WORKAROUND(isq::time)> Q>
[[nodiscard]] constexpr auto to_chrono_duration(const Q& q)
{
  return std::chrono::duration<typename Q::rep, decltype(detail::as_ratio(get_canonical_unit(Q::unit).mag))>{q};
}

template<QuantityPointOf<MP_UNITS_IS_VALUE_WORKAROUND(isq::time)> QP>
  requires is_specialization_of<std::remove_const_t<decltype(QP::absolute_point_origin)>, chrono_point_origin_>
[[nodiscard]] constexpr auto to_chrono_time_point(const QP& qp)
{
  using clock = decltype(QP::absolute_point_origin)::clock;
  using rep = QP::rep;
  using ret_type =
    std::chrono::time_point<clock,
                            std::chrono::duration<rep, decltype(detail::as_ratio(get_canonical_unit(QP::unit).mag))>>;
  return ret_type(to_chrono_duration(qp - qp.absolute_point_origin));
}

MP_UNITS_EXPORT_END

}  // namespace mp_units
