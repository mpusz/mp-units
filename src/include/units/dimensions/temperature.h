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
#include <units/quantity.h>

namespace units {

  struct temperature : derived_dimension<temperature, exp<base_dim_temperature, 1>> {};

  template<typename T>
  concept ThermodynamicTemperature = QuantityOf<T, temperature>;

  struct kelvin : derived_unit<kelvin, decltype("K"_fs), temperature> {};

  inline namespace literals {

    // K
    constexpr auto operator""K(unsigned long long l) { return quantity<kelvin, std::int64_t>(l); }
    constexpr auto operator""_K(long double l) { return quantity<kelvin, long double>(l); }   // TODO: conflicts with gcc GNU extension

  }  // namespace literals

}  // namespace units
