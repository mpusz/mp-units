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

#include <units/dimensions/length.h>

namespace std::experimental::units {

  struct volume : make_dimension_t<exp<base_dim_length, 3>> {};
  template<> struct downcasting_traits<downcast_from<volume>> : downcast_to<volume> {};

  template<typename T>
  concept bool Volume = Quantity<T> && std::same_as<typename T::dimension, volume>;

  struct cubic_millimetre : derived_unit<volume, millimetre> {};
  template<> struct downcasting_traits<downcast_from<cubic_millimetre>> : downcast_to<cubic_millimetre> {};

  struct cubic_centimetre : derived_unit<volume, centimetre> {};
  template<> struct downcasting_traits<downcast_from<cubic_centimetre>> : downcast_to<cubic_centimetre> {};

  struct cubic_metre : derived_unit<volume, metre> {};
  template<> struct downcasting_traits<downcast_from<cubic_metre>> : downcast_to<cubic_metre> {};

  struct cubic_kilometre : derived_unit<volume, kilometre, metre> {};
  template<> struct downcasting_traits<downcast_from<cubic_kilometre>> : downcast_to<cubic_kilometre> {};

  struct cubic_foot : derived_unit<volume, foot> {};
  template<> struct downcasting_traits<downcast_from<cubic_foot>> : downcast_to<cubic_foot> {};

  inline namespace literals {

    // cub_mm
    constexpr auto operator""cub_mm(unsigned long long l) { return quantity<cubic_millimetre, std::int64_t>(l); }
    constexpr auto operator""cub_mm(long double l) { return quantity<cubic_millimetre, long double>(l); }

    // cub_cm
    constexpr auto operator""cub_cm(unsigned long long l) { return quantity<cubic_centimetre, std::int64_t>(l); }
    constexpr auto operator""cub_cm(long double l) { return quantity<cubic_centimetre, long double>(l); }

    // cub_m
    constexpr auto operator""cub_m(unsigned long long l) { return quantity<cubic_metre, std::int64_t>(l); }
    constexpr auto operator""cub_m(long double l) { return quantity<cubic_metre, long double>(l); }

    // cub_km
    constexpr auto operator""cub_km(unsigned long long l) { return quantity<cubic_kilometre, std::int64_t>(l); }
    constexpr auto operator""cub_km(long double l) { return quantity<cubic_kilometre, long double>(l); }

  }  // namespace literals

}  // namespace std::experimental::units
