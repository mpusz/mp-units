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

#include <units/base_dimensions.h>
#include <units/quantity.h>

namespace std::experimental::units {

  struct dimension_length : make_dimension_t<exp<base_dim_length, 1>> {};
  template<> struct downcasting_traits<downcast_from<dimension_length>> : downcast_to<dimension_length> {};

  template<typename T>
  concept bool Length = Quantity<T> && std::Same<typename T::dimension, dimension_length>;

  // SI units
  struct meter : unit<dimension_length> {};
  template<> struct downcasting_traits<downcast_from<meter>> : downcast_to<meter> {};

  struct millimeter : milli<meter> {};
  template<> struct downcasting_traits<downcast_from<millimeter>> : downcast_to<millimeter> {};

  struct centimeter : centi<meter> {};
  template<> struct downcasting_traits<downcast_from<centimeter>> : downcast_to<centimeter> {};

  struct kilometer : kilo<meter> {};
  template<> struct downcasting_traits<downcast_from<kilometer>> : downcast_to<kilometer> {};

  inline namespace literals {

    // mm
    constexpr auto operator""_mm(unsigned long long l) { return quantity<millimeter, std::int64_t>(l); }
    constexpr auto operator""_mm(long double l) { return quantity<millimeter, long double>(l); }

    // cm
    constexpr auto operator""_cm(unsigned long long l) { return quantity<centimeter, std::int64_t>(l); }
    constexpr auto operator""_cm(long double l) { return quantity<centimeter, long double>(l); }

    // m
    constexpr auto operator""_m(unsigned long long l) { return quantity<meter, std::int64_t>(l); }
    constexpr auto operator""_m(long double l) { return quantity<meter, long double>(l); }

    // km
    constexpr auto operator""_km(unsigned long long l) { return quantity<kilometer, std::int64_t>(l); }
    constexpr auto operator""_km(long double l) { return quantity<kilometer, long double>(l); }

  } // namespace literals

  // US customary units
  struct yard : unit<dimension_length, ratio<9'144, 10'000>> {};
  template<> struct downcasting_traits<downcast_from<yard>> : downcast_to<yard> {};

  struct foot : unit<dimension_length, ratio_multiply<ratio<1, 3>, yard::ratio>> {};
  template<> struct downcasting_traits<downcast_from<foot>> : downcast_to<foot> {};

  struct inch : unit<dimension_length, ratio_multiply<ratio<1, 12>, foot::ratio>> {};
  template<> struct downcasting_traits<downcast_from<inch>> : downcast_to<inch> {};

  struct mile : unit<dimension_length, ratio_multiply<ratio<1'760>, yard::ratio>> {};
  template<> struct downcasting_traits<downcast_from<mile>> : downcast_to<mile> {};

  inline namespace literals {

    // yd
    constexpr auto operator""_yd(unsigned long long l) { return quantity<yard, std::int64_t>(l); }
    constexpr auto operator""_yd(long double l) { return quantity<yard, long double>(l); }

    // ft
    constexpr auto operator""_ft(unsigned long long l) { return quantity<foot, std::int64_t>(l); }
    constexpr auto operator""_ft(long double l) { return quantity<foot, long double>(l); }

    // in
    constexpr auto operator""_in(unsigned long long l) { return quantity<inch, std::int64_t>(l); }
    constexpr auto operator""_in(long double l) { return quantity<inch, long double>(l); }

    // mi
    constexpr auto operator""_mi(unsigned long long l) { return quantity<mile, std::int64_t>(l); }
    constexpr auto operator""_mi(long double l) { return quantity<mile, long double>(l); }

  }  // namespace literals

}  // namespace std::experimental::units
