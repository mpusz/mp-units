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
#include <units/time.h>
#include <units/current.h>

namespace std::experimental::units {

  struct dimension_electric_charge : make_dimension_t<exp<base_dim_time, 1>, exp<base_dim_current, 1>> {};
  template<> struct downcasting_traits<downcast_from<dimension_electric_charge>> : downcast_to<dimension_electric_charge> {};

  template<typename T>
  concept bool ElectricCharge =  Quantity<T> && std::Same<typename T::dimension, dimension_electric_charge>;

  struct coulomb : derived_unit<dimension_electric_charge, second, ampere> {};
  template<> struct downcasting_traits<downcast_from<coulomb>> : downcast_to<coulomb> {};

  inline namespace literals {

    // C
    constexpr auto operator""C(unsigned long long l) { return quantity<coulomb, std::int64_t>(l); }
    constexpr auto operator""C(long double l) { return quantity<coulomb, long double>(l); }

  }  // namespace literals

}  // namespace std::experimental::units
