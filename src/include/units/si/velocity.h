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

#include "base_dimensions.h"
#include "length.h"
#include "time.h"

namespace units {

  using dimension_velocity = make_dimension_t<exp<base_dim_length, 1>, exp<base_dim_time, -1>>;

  using meter_per_second = unit<dimension_velocity, std::ratio<1>>;
  using kilometer_per_hour = unit<dimension_velocity, std::ratio<1000, 3600>>;
  using mile_per_hour = unit<dimension_velocity, std::ratio<44'704, 100'000>>;

  template<Unit U = meter_per_second, Number Rep = std::intmax_t>
  using velocity = quantity<dimension_velocity, U, Rep>;

  template<typename T>
  concept bool Velocity = Quantity<T> && Same<typename T::dimension, dimension_velocity>;

  // ...

  namespace literals {

    // mps
    constexpr auto operator""_mps(unsigned long long l) { return velocity<meter_per_second, std::int64_t>(l); }
    constexpr auto operator""_mps(long double l) { return velocity<meter_per_second, long double>(l); }

    // kmph
    constexpr auto operator""_kmph(unsigned long long l) { return velocity<kilometer_per_hour, std::int64_t>(l); }
    constexpr auto operator""_kmph(long double l) { return velocity<kilometer_per_hour, long double>(l); }

    // mph
    constexpr auto operator""_mph(unsigned long long l) { return velocity<mile_per_hour, std::int64_t>(l); }
    constexpr auto operator""_mph(long double l) { return velocity<mile_per_hour, long double>(l); }

  }  // namespace literals

}  // namespace units
