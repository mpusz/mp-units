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

#include <mp-units/customization_points.h>
#include <mp-units/quantity_point.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si/prefixes.h>
#include <mp-units/systems/si/units.h>
#include <chrono>

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
  else
    return mag<ratio{Period::num, Period::den}> * second;
}

}  // namespace detail

template<typename Rep, typename Period>
struct quantity_like_traits<std::chrono::duration<Rep, Period>> {
  static constexpr auto reference = detail::time_unit_from_chrono_period<Period>();
  using rep = Rep;
  [[nodiscard]] static constexpr rep number(const std::chrono::duration<Rep, Period>& q) { return q.count(); }
};

template<typename C>
struct chrono_point_origin_ : absolute_point_origin<isq::time> {
  using clock = C;
};
template<typename C>
inline constexpr chrono_point_origin_<C> chrono_point_origin;


template<typename C, typename Rep, typename Period>
struct quantity_point_like_traits<std::chrono::time_point<C, std::chrono::duration<Rep, Period>>> {
  static constexpr auto reference = detail::time_unit_from_chrono_period<Period>();
  static constexpr auto point_origin = chrono_point_origin<C>;
  using rep = Rep;
  [[nodiscard]] static constexpr auto relative(const std::chrono::time_point<C, std::chrono::duration<Rep, Period>>& qp)
  {
    return qp.time_since_epoch();
  }
};

template<QuantityOf<isq::time> Q>
[[nodiscard]] constexpr auto to_chrono_duration(const Q& q)
{
  constexpr auto canonical = detail::get_canonical_unit(Q::unit);
  constexpr ratio r = as_ratio(canonical.mag);
  return std::chrono::duration<typename Q::rep, std::ratio<r.num, r.den>>{q.number()};
}

template<QuantityPointOf<isq::time> QP>
  requires is_specialization_of<std::remove_const_t<decltype(QP::absolute_point_origin)>, chrono_point_origin_>
[[nodiscard]] constexpr auto to_chrono_time_point(const QP& qp)
{
  using clock = MP_UNITS_TYPENAME decltype(QP::absolute_point_origin)::clock;
  using rep = MP_UNITS_TYPENAME QP::rep;
  constexpr auto canonical = detail::get_canonical_unit(QP::unit);
  constexpr ratio r = as_ratio(canonical.mag);
  using ret_type = std::chrono::time_point<clock, std::chrono::duration<rep, std::ratio<r.num, r.den>>>;
  return ret_type(to_chrono_duration(qp.absolute()));
}

}  // namespace mp_units
