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

#include <units/length.h>
#include <units/time.h>

namespace std::experimental::units {

  struct dimension_velocity : make_dimension_t<exp<base_dim_length, 1>, exp<base_dim_time, -1>> {};
  template<> struct downcasting_traits<downcast_from<dimension_velocity>> : downcast_to<dimension_velocity> {};

  template<typename T>
  concept bool Velocity = Quantity<T> && std::Same<typename T::dimension, dimension_velocity>;

  struct metre_per_second : derived_unit<dimension_velocity, metre, second> {};
  template<> struct downcasting_traits<downcast_from<metre_per_second>> : downcast_to<metre_per_second> {};

  struct kilometre_per_hour : derived_unit<dimension_velocity, kilometre, hour> {};
  template<> struct downcasting_traits<downcast_from<kilometre_per_hour>> : downcast_to<kilometre_per_hour> {};

  struct mile_per_hour : derived_unit<dimension_velocity, mile, hour> {};
  template<> struct downcasting_traits<downcast_from<mile_per_hour>> : downcast_to<mile_per_hour> {};

  inline namespace literals {

    // mps
    constexpr auto operator""mps(unsigned long long l) { return quantity<metre_per_second, std::int64_t>(l); }
    constexpr auto operator""mps(long double l) { return quantity<metre_per_second, long double>(l); }

    // kmph
    constexpr auto operator""kmph(unsigned long long l) { return quantity<kilometre_per_hour, std::int64_t>(l); }
    constexpr auto operator""kmph(long double l) { return quantity<kilometre_per_hour, long double>(l); }

    // mph
    constexpr auto operator""mph(unsigned long long l) { return quantity<mile_per_hour, std::int64_t>(l); }
    constexpr auto operator""mph(long double l) { return quantity<mile_per_hour, long double>(l); }

  }  // namespace literals

}  // namespace std::experimental::units
