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

namespace units {

  struct area : make_dimension_t<exp<base_dim_length, 2>> {};
  template<> struct downcasting_traits<downcast_from<area>> : downcast_to<area> {};

  template<typename T>
  concept bool Area = QuantityOf<T, area>;

  struct square_millimetre : derived_unit<area, millimetre> {};
  template<> struct downcasting_traits<downcast_from<square_millimetre>> : downcast_to<square_millimetre> {};

  struct square_centimetre : derived_unit<area, centimetre> {};
  template<> struct downcasting_traits<downcast_from<square_centimetre>> : downcast_to<square_centimetre> {};

  struct square_metre : derived_unit<area, metre> {};
  template<> struct downcasting_traits<downcast_from<square_metre>> : downcast_to<square_metre> {};

  struct square_kilometre : derived_unit<area, kilometre, metre> {};
  template<> struct downcasting_traits<downcast_from<square_kilometre>> : downcast_to<square_kilometre> {};

  struct square_foot : derived_unit<area, foot> {};
  template<> struct downcasting_traits<downcast_from<square_foot>> : downcast_to<square_foot> {};

  inline namespace literals {

    // sq_mm
    constexpr auto operator""sq_mm(unsigned long long l) { return quantity<square_millimetre, std::int64_t>(l); }
    constexpr auto operator""sq_mm(long double l) { return quantity<square_millimetre, long double>(l); }

    // sq_cm
    constexpr auto operator""sq_cm(unsigned long long l) { return quantity<square_centimetre, std::int64_t>(l); }
    constexpr auto operator""sq_cm(long double l) { return quantity<square_centimetre, long double>(l); }

    // sq_m
    constexpr auto operator""sq_m(unsigned long long l) { return quantity<square_metre, std::int64_t>(l); }
    constexpr auto operator""sq_m(long double l) { return quantity<square_metre, long double>(l); }

    // sq_km
    constexpr auto operator""sq_km(unsigned long long l) { return quantity<square_kilometre, std::int64_t>(l); }
    constexpr auto operator""sq_km(long double l) { return quantity<square_kilometre, long double>(l); }

  }  // namespace literals

}  // namespace units
