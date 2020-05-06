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

#include <units/concepts.h>
#include <units/quantity.h>
#include <cmath>
#include <limits>

namespace units {

template<std::intmax_t N, typename D, typename U, typename Rep>
requires(N == 0) inline Rep pow(const quantity<D, U, Rep>&) noexcept
{
  return 1;
}

template<std::intmax_t N, typename D, typename U, typename Rep>
inline Quantity AUTO pow(const quantity<D, U, Rep>& q) noexcept
{
  using dim = dimension_pow<D, N>;
  using ratio = ratio_pow<typename U::ratio, N>;
  using unit = downcast_unit<dim, ratio>;
  return quantity<dim, unit, Rep>(static_cast<Rep>(std::pow(q.count(), N)));
}

template<typename D, typename U, typename Rep>
inline Quantity AUTO sqrt(const quantity<D, U, Rep>& q) noexcept
{
  using dim = dimension_sqrt<D>;
  using ratio = ratio_sqrt<typename U::ratio>;
  using unit = downcast_unit<dim, ratio>;
  return quantity<dim, unit, Rep>(static_cast<Rep>(std::sqrt(q.count())));
}

template<typename D, typename U, typename Rep>
constexpr Quantity AUTO abs(const quantity<D, U, Rep>& q) noexcept
{
  return quantity<D, U, Rep>(std::abs(q.count()));
}

template<Quantity Q>
constexpr Quantity AUTO epsilon() noexcept
{
  return Q(std::numeric_limits<typename Q::rep>::epsilon());
}

}  // namespace units
