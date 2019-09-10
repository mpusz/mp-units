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
#include <units/dimensions/electric_charge.h>
#include <units/dimensions/voltage.h>

namespace std::experimental::units {

  struct capacitance : make_dimension_t<exp<base_dim_mass, -1>, exp<base_dim_length, -2>, exp<base_dim_time, 4>, exp<base_dim_current, 2>> {};
  template<> struct downcasting_traits<downcast_from<capacitance>> : downcast_to<capacitance> {};

  template<typename T>
  concept bool Capacitance =  QuantityOf<T, capacitance>;

  struct farad : derived_unit<capacitance, kilogram, metre, second, ampere> {};
  template<> struct downcasting_traits<downcast_from<farad>> : downcast_to<farad> {};

  inline namespace literals {

    // F
    constexpr auto operator""F(unsigned long long l) { return quantity<farad, std::int64_t>(l); }
    constexpr auto operator""_F(long double l) { return quantity<farad, long double>(l); }

  }  // namespace literals

}  // namespace std::experimental::units
