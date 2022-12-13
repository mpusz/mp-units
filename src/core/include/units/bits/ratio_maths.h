// The MIT License (MIT)
//
// Copyright (c) 2018 Mateusz Pusz, Conor Williams, Oliver Schonrock
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

#include <gsl/gsl-lite.hpp>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <tuple>
#include <type_traits>

namespace units::detail {

template<typename T>
[[nodiscard]] constexpr T abs(T v) noexcept
{
  return v < 0 ? -v : v;
}

// Computes the rational gcd of n1/d1 and n2/d2
[[nodiscard]] constexpr auto gcd_frac(std::intmax_t n1, std::intmax_t d1, std::intmax_t n2, std::intmax_t d2) noexcept
{
  // Short cut for equal ratios
  if (n1 == n2 && d1 == d2) {
    return std::array{n1, d1};
  }

  // gcd(a/b,c/d) = gcd(a⋅d, c⋅b) / b⋅d

  assert(std::numeric_limits<std::intmax_t>::max() / n1 > d2);
  assert(std::numeric_limits<std::intmax_t>::max() / n2 > d1);

  std::intmax_t num = std::gcd(n1 * d2, n2 * d1);

  assert(std::numeric_limits<std::intmax_t>::max() / d1 > d2);

  std::intmax_t den = d1 * d2;

  std::intmax_t gcd = std::gcd(num, den);

  return std::array{num / gcd, den / gcd};
}

constexpr void normalize(std::intmax_t& num, std::intmax_t& den)
{
  if (num == 0) {
    den = 1;
    return;
  }

  std::intmax_t gcd = std::gcd(num, den);
  num = num * (den < 0 ? -1 : 1) / gcd;
  den = detail::abs(den) / gcd;
}

[[nodiscard]] constexpr std::intmax_t safe_multiply(std::intmax_t lhs, std::intmax_t rhs)
{
  constexpr std::intmax_t c = std::uintmax_t(1) << (sizeof(std::intmax_t) * 4);

  const std::intmax_t a0 = detail::abs(lhs) % c;
  const std::intmax_t a1 = detail::abs(lhs) / c;
  const std::intmax_t b0 = detail::abs(rhs) % c;
  const std::intmax_t b1 = detail::abs(rhs) / c;

  gsl_Expects(a1 == 0 || b1 == 0);                               // overflow in multiplication
  gsl_Expects(a0 * b1 + b0 * a1 < (c >> 1));                     // overflow in multiplication
  gsl_Expects(b0 * a0 <= INTMAX_MAX);                            // overflow in multiplication
  gsl_Expects((a0 * b1 + b0 * a1) * c <= INTMAX_MAX - b0 * a0);  // overflow in multiplication

  return lhs * rhs;
}

}  // namespace units::detail
