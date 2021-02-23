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
#include <units/origin.h>
#include <units/physical/si/base/time.h>
#include <chrono>

namespace units {

template<typename Rep, typename Period>
struct quantity_like_traits<std::chrono::duration<Rep, Period>> {
  using dimension = physical::si::dim_time;
  using unit = downcast_unit<dimension, ratio(Period::num, Period::den)>;
  using rep = Rep;
  [[nodiscard]] static constexpr rep count(const std::chrono::duration<Rep, Period>& q) { return q.count(); }
};

template <typename C>
struct chrono_clock_point_origin : point_origin<chrono_clock_point_origin<C>, typename dimension_unit<physical::si::dim_time>::reference> {};

template<typename C, typename Rep, typename Period>
struct quantity_point_like_traits<std::chrono::time_point<C, std::chrono::duration<Rep, Period>>> {
  using dimension = physical::si::dim_time;
  using unit = downcast_unit<dimension, ratio(Period::num, Period::den)>;
  using rep = Rep;
  using origin = chrono_clock_point_origin<typename std::chrono::time_point<C, std::chrono::duration<Rep, Period>>::clock>;
  [[nodiscard]] static constexpr auto relative(
    const std::chrono::time_point<C, std::chrono::duration<Rep, Period>>& qp) {
    return qp.time_since_epoch();
  }
};

} // namespace units
