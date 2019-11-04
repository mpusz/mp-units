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

#include <units/dimensions/si_base_dimensions.h>
#include <units/dimensions/force.h>

namespace units {

  struct surface_tension : derived_dimension<surface_tension, exp<force, 1>, exp<length, -1>> {};

  template<typename T>
  concept SurfaceTension = QuantityOf<T, surface_tension>;

  struct newton_per_metre : coherent_derived_unit<newton_per_metre, surface_tension> {};

  inline namespace literals {

    // Nm
    constexpr auto operator""Npm(unsigned long long l) { return quantity<newton_per_metre, std::int64_t>(l); }
    constexpr auto operator""Npm(long double l) { return quantity<newton_per_metre, long double>(l); }

  }  // namespace literals

}  // namespace units
