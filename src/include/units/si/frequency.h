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

  using millihertz = unit<dimension_frequency, std::milli>;
  using hertz = unit<dimension_frequency, std::ratio<1>>;
  using kilohertz = unit<dimension_frequency, std::kilo>;
  using megahertz = unit<dimension_frequency, std::mega>;
  using gigahertz = unit<dimension_frequency, std::giga>;
  using terahertz = unit<dimension_frequency, std::tera>;

  template<Unit U = hertz, Number Rep = std::intmax_t>
  using frequency = quantity<dimension_frequency, U, Rep>;

  template<typename T>
  concept bool Frequency =  Quantity<T> && Same<typename T::dimension, dimension_frequency>;

  // ...

  namespace literals {

    // mHz
    constexpr auto operator""_mHz(unsigned long long l) { return frequency<millihertz, std::int64_t>(l); }
    constexpr auto operator""_mHz(long double l) { return frequency<millihertz, long double>(l); }

    // Hz
    constexpr auto operator""_Hz(unsigned long long l) { return frequency<hertz, std::int64_t>(l); }
    constexpr auto operator""_Hz(long double l) { return frequency<hertz, long double>(l); }

    // kHz
    constexpr auto operator""_kHz(unsigned long long l) { return frequency<kilohertz, std::int64_t>(l); }
    constexpr auto operator""_kHz(long double l) { return frequency<kilohertz, long double>(l); }

    // MHz
    constexpr auto operator""_MHz(unsigned long long l) { return frequency<megahertz, std::int64_t>(l); }
    constexpr auto operator""_MHz(long double l) { return frequency<megahertz, long double>(l); }

    // GHz
    constexpr auto operator""_GHz(unsigned long long l) { return frequency<gigahertz, std::int64_t>(l); }
    constexpr auto operator""_GHz(long double l) { return frequency<gigahertz, long double>(l); }

    // THz
    constexpr auto operator""_THz(unsigned long long l) { return frequency<terahertz, std::int64_t>(l); }
    constexpr auto operator""_THz(long double l) { return frequency<terahertz, long double>(l); }

  }  // namespace literals

}  // namespace units
