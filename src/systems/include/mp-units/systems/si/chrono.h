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

#include <mp-units/bits/module_macros.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si/prefixes.h>
#include <mp-units/systems/si/units.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/customization_points.h>
#include <mp-units/quantity_point.h>
#include <chrono>
#endif

namespace mp_units {

namespace detail {

template<typename Period>
[[nodiscard]] inline consteval auto time_unit_from_chrono_period()
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
    return mag<ratio{Period::num, Period::den}> * second;
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
struct chrono_point_origin_ : absolute_point_origin<chrono_point_origin_<C>, isq::time> {
  using clock = C;
};
MP_UNITS_EXPORT template<typename C>
inline constexpr chrono_point_origin_<C> chrono_point_origin;

MP_UNITS_EXPORT_BEGIN

template<typename C, typename Rep, typename Period>
struct quantity_point_like_traits<std::chrono::time_point<C, std::chrono::duration<Rep, Period>>> {
  using T = std::chrono::time_point<C, std::chrono::duration<Rep, Period>>;
  static constexpr auto reference = detail::time_unit_from_chrono_period<Period>();
  static constexpr auto point_origin = chrono_point_origin<C>;
  using rep = Rep;

  [[nodiscard]] static constexpr convert_implicitly<quantity<reference, rep>> to_quantity(const T& qp) noexcept(
    std::is_nothrow_copy_constructible_v<rep>)
  {
    return quantity{qp.time_since_epoch()};
  }

  [[nodiscard]] static constexpr convert_implicitly<T> from_quantity(const quantity<reference, rep>& q) noexcept(
    std::is_nothrow_copy_constructible_v<rep>)
  {
    return T(q);
  }
};

template<QuantityOf<isq::time> Q>
[[nodiscard]] constexpr auto to_chrono_duration(const Q& q)
{
  constexpr auto canonical = get_canonical_unit(Q::unit);
  constexpr ratio r = as_ratio(canonical.mag);
  return std::chrono::duration<typename Q::rep, std::ratio<r.num, r.den>>{q};
}

template<QuantityPointOf<isq::time> QP>
  requires is_specialization_of<std::remove_const_t<decltype(QP::absolute_point_origin)>, chrono_point_origin_>
[[nodiscard]] constexpr auto to_chrono_time_point(const QP& qp)
{
  using clock = MP_UNITS_TYPENAME decltype(QP::absolute_point_origin)::clock;
  using rep = MP_UNITS_TYPENAME QP::rep;
  constexpr auto canonical = get_canonical_unit(QP::unit);
  constexpr ratio r = as_ratio(canonical.mag);
  using ret_type = std::chrono::time_point<clock, std::chrono::duration<rep, std::ratio<r.num, r.den>>>;
  return ret_type(to_chrono_duration(qp - qp.absolute_point_origin));
}

MP_UNITS_EXPORT_END

}  // namespace mp_units
