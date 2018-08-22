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

#include "base_dimensions.h"

namespace units {

  using dimension_time = make_dimension_t<exp<base_dim_time, 1>>;

  template<typename Rep, class Ratio = std::ratio<1>>
  using time = quantity<dimension_time, Rep, Ratio>;

  template<typename Rep>
  using nanoseconds = time<Rep, std::nano>;

  template<typename Rep>
  using microseconds = time<Rep, std::micro>;

  template<typename Rep>
  using milliseconds = time<Rep, std::milli>;

  template<typename Rep>
  using seconds = time<Rep>;

  template<typename Rep>
  using minutes = time<Rep, std::ratio<60>>;

  template<typename Rep>
  using hours = time<Rep, std::ratio<3600>>;

  // ...

  namespace literals {

    // ns
    constexpr auto operator""_ns(unsigned long long l) { return nanoseconds<std::int64_t>(l); }
    constexpr auto operator""_ns(long double l) { return nanoseconds<long double>(l); }

    // us
    constexpr auto operator""_us(unsigned long long l) { return microseconds<std::int64_t>(l); }
    constexpr auto operator""_us(long double l) { return microseconds<long double>(l); }

    // ms
    constexpr auto operator""_ms(unsigned long long l) { return milliseconds<std::int64_t>(l); }
    constexpr auto operator""_ms(long double l) { return milliseconds<long double>(l); }

    // s
    constexpr auto operator""_s(unsigned long long l) { return seconds<std::int64_t>(l); }
    constexpr auto operator""_s(long double l) { return seconds<long double>(l); }

    // min
    constexpr auto operator""_min(unsigned long long l) { return minutes<std::int64_t>(l); }
    constexpr auto operator""_min(long double l) { return minutes<long double>(l); }

    // h
    constexpr auto operator""_h(unsigned long long l) { return hours<std::int64_t>(l); }
    constexpr auto operator""_h(long double l) { return hours<long double>(l); }

  }  // namespace literals

}  // namespace units
