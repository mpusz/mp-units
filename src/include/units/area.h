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

namespace units {

  struct dimension_area : make_dimension_t<exp<base_dim_length, 2>> {};
  template<> struct upcasting_traits<upcast_from<dimension_area>> : upcast_to<dimension_area> {};

  template<typename T>
  concept bool Area = Quantity<T> && std::Same<typename T::dimension, dimension_area>;

  template<Unit U = struct square_meter, Number Rep = double>
  using area = quantity<dimension_area, U, Rep>;

  struct square_millimeter : derived_unit<dimension_area, millimeter> {};
  template<> struct upcasting_traits<upcast_from<square_millimeter>> : upcast_to<square_millimeter> {};

  struct square_centimeter : derived_unit<dimension_area, centimeter> {};
  template<> struct upcasting_traits<upcast_from<square_centimeter>> : upcast_to<square_centimeter> {};

  struct square_meter : derived_unit<dimension_area, meter> {};
  template<> struct upcasting_traits<upcast_from<square_meter>> : upcast_to<square_meter> {};

  struct square_kilometer : derived_unit<dimension_area, kilometer, meter> {};
  template<> struct upcasting_traits<upcast_from<square_kilometer>> : upcast_to<square_kilometer> {};

  struct square_foot : derived_unit<dimension_area, foot> {};
  template<> struct upcasting_traits<upcast_from<square_foot>> : upcast_to<square_foot> {};

  inline namespace literals {

    // sq_mm
    constexpr auto operator""_sq_mm(unsigned long long l) { return area<square_millimeter, std::int64_t>(l); }
    constexpr auto operator""_sq_mm(long double l) { return area<square_millimeter, long double>(l); }

    // sq_cm
    constexpr auto operator""_sq_cm(unsigned long long l) { return area<square_centimeter, std::int64_t>(l); }
    constexpr auto operator""_sq_cm(long double l) { return area<square_centimeter, long double>(l); }

    // sq_m
    constexpr auto operator""_sq_m(unsigned long long l) { return area<square_meter, std::int64_t>(l); }
    constexpr auto operator""_sq_m(long double l) { return area<square_meter, long double>(l); }

    // sq_km
    constexpr auto operator""_sq_km(unsigned long long l) { return area<square_kilometer, std::int64_t>(l); }
    constexpr auto operator""_sq_km(long double l) { return area<square_kilometer, long double>(l); }

  }  // namespace literals

}  // namespace units
