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
#include <units/dimensions/power.h>
#include <units/dimensions/current.h>
#include <units/dimensions/energy.h>
#include <units/dimensions/electric_charge.h>

namespace std::experimental::units {

  struct dimension_voltage : make_dimension_t<exp<base_dim_mass, 1>, exp<base_dim_length, 2>, exp<base_dim_time, -3>, exp<base_dim_current, -1>> {};
  template<> struct downcasting_traits<downcast_from<dimension_voltage>> : downcast_to<dimension_voltage> {};

  template<typename T>
  concept bool Voltage =  Quantity<T> && std::Same<typename T::dimension, dimension_voltage>;

  struct volt : derived_unit<dimension_voltage, kilogram, metre, second, ampere> {};
  template<> struct downcasting_traits<downcast_from<volt>> : downcast_to<volt> {};

  inline namespace literals {

    // V
    constexpr auto operator""V(unsigned long long l) { return quantity<volt, std::int64_t>(l); }
    constexpr auto operator""V(long double l) { return quantity<volt, long double>(l); }

  }  // namespace literals

}  // namespace std::experimental::units
