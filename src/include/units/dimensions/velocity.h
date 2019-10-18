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
#include <units/dimensions/time.h>

namespace units {

  struct velocity : derived_dimension<velocity, exp<base_dim_length, 1>, exp<base_dim_time, -1>> {};

  template<typename T>
  concept Velocity = QuantityOf<T, velocity>;

  struct metre_per_second : coherent_derived_unit<metre_per_second, "m/s", velocity> {};
  struct kilometre_per_hour : deduced_derived_unit<kilometre_per_hour, "km/h", velocity, kilometre, hour> {};
  struct mile_per_hour : deduced_derived_unit<mile_per_hour, "mi/h", velocity, mile, hour> {};

  inline namespace literals {

    // mps
    constexpr auto operator""mps(unsigned long long l) { return quantity<metre_per_second, std::int64_t>(l); }
    constexpr auto operator""mps(long double l) { return quantity<metre_per_second, long double>(l); }

    // kmph
    constexpr auto operator""kmph(unsigned long long l) { return quantity<kilometre_per_hour, std::int64_t>(l); }
    constexpr auto operator""kmph(long double l) { return quantity<kilometre_per_hour, long double>(l); }

    // mph
    constexpr auto operator""mph(unsigned long long l) { return quantity<mile_per_hour, std::int64_t>(l); }
    constexpr auto operator""mph(long double l) { return quantity<mile_per_hour, long double>(l); }

  }  // namespace literals

}  // namespace units
