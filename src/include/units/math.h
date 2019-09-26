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

#include <units/quantity.h>
#include <cmath>

namespace units {

  template<std::size_t N, typename U, typename Rep>
  inline Quantity pow(const quantity<U, Rep>& q) noexcept
  {
    using dim = dimension_pow_t<typename U::dimension, N>;
    using r = ratio_pow<typename U::ratio, N>;
    return quantity<downcast_target<unit<dim, r>>, Rep>(static_cast<Rep>(std::pow(q.count(), N)));
  }

  template<typename U, typename Rep>
  inline Quantity sqrt(const quantity<U, Rep>& q) noexcept
  {
    using dim = dimension_sqrt_t<typename U::dimension>;
    using r = ratio_sqrt<typename U::ratio>;
    return quantity<downcast_target<unit<dim, r>>, Rep>(static_cast<Rep>(std::sqrt(q.count())));
  }

}  // namespace units
