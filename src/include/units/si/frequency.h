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
#include "time.h"

namespace units {

  using dimension_frequency = make_dimension_t<exp<base_dim_time, -1>>;

  template<typename Rep, class Ratio = std::ratio<1>>
  using frequency = quantity<dimension_frequency, Rep, Ratio>;

  template<typename Rep>
  using millihertzs = frequency<Rep, std::milli>;

  template<typename Rep>
  using hertzs = frequency<Rep>;

  template<typename Rep>
  using kilohertzs = frequency<Rep, std::kilo>;

  template<typename Rep>
  using megahertzs = frequency<Rep, std::mega>;

  template<typename Rep>
  using gigahertzs = frequency<Rep, std::giga>;

  template<typename Rep>
  using terahertzs = frequency<Rep, std::tera>;

  // ...

  namespace literals {

    // mHz
    constexpr auto operator""_mHz(unsigned long long l) { return millihertzs<std::int64_t>(l); }
    constexpr auto operator""_mHz(long double l) { return millihertzs<long double>(l); }

    // Hz
    constexpr auto operator""_Hz(unsigned long long l) { return hertzs<std::int64_t>(l); }
    constexpr auto operator""_Hz(long double l) { return hertzs<long double>(l); }

    // kHz
    constexpr auto operator""_kHz(unsigned long long l) { return kilohertzs<std::int64_t>(l); }
    constexpr auto operator""_kHz(long double l) { return kilohertzs<long double>(l); }

    // MHz
    constexpr auto operator""_MHz(unsigned long long l) { return megahertzs<std::int64_t>(l); }
    constexpr auto operator""_MHz(long double l) { return megahertzs<long double>(l); }

    // GHz
    constexpr auto operator""_GHz(unsigned long long l) { return gigahertzs<std::int64_t>(l); }
    constexpr auto operator""_GHz(long double l) { return gigahertzs<long double>(l); }

    // THz
    constexpr auto operator""_THz(unsigned long long l) { return terahertzs<std::int64_t>(l); }
    constexpr auto operator""_THz(long double l) { return terahertzs<long double>(l); }

  }  // namespace literals

}  // namespace units
