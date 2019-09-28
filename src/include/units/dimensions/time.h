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

#include <units/dimensions/base_dimensions.h>
#include <units/quantity.h>

namespace units {

  struct time : derived_dimension<time, exp<base_dim_time, 1>> {};

  template<typename T>
  concept Time = QuantityOf<T, time>;

  struct second : derived_unit<second, time> {};
  struct nanosecond : derived_unit<nanosecond, nano<second>> {};
  struct microsecond : derived_unit<microsecond, micro<second>> {};
  struct millisecond : derived_unit<millisecond, milli<second>> {};
  struct minute : derived_unit<minute, time, ratio<60>> {};
  struct hour : derived_unit<hour, time, ratio<3600>> {};

  inline namespace literals {

    // ns
    constexpr auto operator""ns(unsigned long long l) { return quantity<nanosecond, std::int64_t>(l); }
    constexpr auto operator""ns(long double l) { return quantity<nanosecond, long double>(l); }

    // us
    constexpr auto operator""us(unsigned long long l) { return quantity<microsecond, std::int64_t>(l); }
    constexpr auto operator""us(long double l) { return quantity<microsecond, long double>(l); }

    // ms
    constexpr auto operator""ms(unsigned long long l) { return quantity<millisecond, std::int64_t>(l); }
    constexpr auto operator""ms(long double l) { return quantity<millisecond, long double>(l); }

    // s
    constexpr auto operator""s(unsigned long long l) { return quantity<second, std::int64_t>(l); }
    constexpr auto operator""s(long double l) { return quantity<second, long double>(l); }

    // min
    constexpr auto operator""min(unsigned long long l) { return quantity<minute, std::int64_t>(l); }
    constexpr auto operator""min(long double l) { return quantity<minute, long double>(l); }

    // h
    constexpr auto operator""h(unsigned long long l) { return quantity<hour, std::int64_t>(l); }
    constexpr auto operator""h(long double l) { return quantity<hour, long double>(l); }

  }  // namespace literals

}  // namespace units
