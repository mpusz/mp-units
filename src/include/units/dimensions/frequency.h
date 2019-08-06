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
#include <units/dimensions/time.h>

namespace std::experimental::units {

  struct dimension_frequency : make_dimension_t<exp<base_dim_time, -1>> {};
  template<> struct downcasting_traits<downcast_from<dimension_frequency>> : downcast_to<dimension_frequency> {};

  template<typename T>
  concept bool Frequency =  Quantity<T> && std::Same<typename T::dimension, dimension_frequency>;

  struct hertz : derived_unit<dimension_frequency, second> {};
  template<> struct downcasting_traits<downcast_from<hertz>> : downcast_to<hertz> {};

  struct millihertz : milli<hertz> {};
  template<> struct downcasting_traits<downcast_from<millihertz>> : downcast_to<millihertz> {};

  struct kilohertz : kilo<hertz> {};
  template<> struct downcasting_traits<downcast_from<kilohertz>> : downcast_to<kilohertz> {};

  struct megahertz : mega<hertz> {};
  template<> struct downcasting_traits<downcast_from<megahertz>> : downcast_to<megahertz> {};

  struct gigahertz : giga<hertz> {};
  template<> struct downcasting_traits<downcast_from<gigahertz>> : downcast_to<gigahertz> {};

  struct terahertz : tera<hertz> {};
  template<> struct downcasting_traits<downcast_from<terahertz>> : downcast_to<terahertz> {};

  inline namespace literals {

    // mHz
    constexpr auto operator""mHz(unsigned long long l) { return quantity<millihertz, std::int64_t>(l); }
    constexpr auto operator""mHz(long double l) { return quantity<millihertz, long double>(l); }

    // Hz
    constexpr auto operator""Hz(unsigned long long l) { return quantity<hertz, std::int64_t>(l); }
    constexpr auto operator""Hz(long double l) { return quantity<hertz, long double>(l); }

    // kHz
    constexpr auto operator""kHz(unsigned long long l) { return quantity<kilohertz, std::int64_t>(l); }
    constexpr auto operator""kHz(long double l) { return quantity<kilohertz, long double>(l); }

    // MHz
    constexpr auto operator""MHz(unsigned long long l) { return quantity<megahertz, std::int64_t>(l); }
    constexpr auto operator""MHz(long double l) { return quantity<megahertz, long double>(l); }

    // GHz
    constexpr auto operator""GHz(unsigned long long l) { return quantity<gigahertz, std::int64_t>(l); }
    constexpr auto operator""GHz(long double l) { return quantity<gigahertz, long double>(l); }

    // THz
    constexpr auto operator""THz(unsigned long long l) { return quantity<terahertz, std::int64_t>(l); }
    constexpr auto operator""THz(long double l) { return quantity<terahertz, long double>(l); }

  }  // namespace literals

}  // namespace std::experimental::units
