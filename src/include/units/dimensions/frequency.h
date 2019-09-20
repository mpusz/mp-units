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
#include <units/dimensions/time.h>

namespace units {

  struct frequency : make_dimension_t<exp<base_dim_time, -1>> {};
  template<> struct downcast_traits<downcast_base_t<frequency>> : std::type_identity<frequency> {};

  template<typename T>
  concept bool Frequency =  QuantityOf<T, frequency>;

  struct hertz : derived_unit<frequency, second> {};
  template<> struct downcast_traits<downcast_base_t<hertz>> : std::type_identity<hertz> {};

  struct millihertz : milli<hertz> {};
  template<> struct downcast_traits<downcast_base_t<millihertz>> : std::type_identity<millihertz> {};

  struct kilohertz : kilo<hertz> {};
  template<> struct downcast_traits<downcast_base_t<kilohertz>> : std::type_identity<kilohertz> {};

  struct megahertz : mega<hertz> {};
  template<> struct downcast_traits<downcast_base_t<megahertz>> : std::type_identity<megahertz> {};

  struct gigahertz : giga<hertz> {};
  template<> struct downcast_traits<downcast_base_t<gigahertz>> : std::type_identity<gigahertz> {};

  struct terahertz : tera<hertz> {};
  template<> struct downcast_traits<downcast_base_t<terahertz>> : std::type_identity<terahertz> {};

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

}  // namespace units
