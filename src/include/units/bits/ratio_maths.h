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

#include <units/bits/external/hacks.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <tuple>
#include <type_traits>
#include <gsl/assert>

namespace units::detail {

template<typename T>
[[nodiscard]] constexpr T abs(T v) noexcept
{
  return v < 0 ? -v : v;
}

// the following functions enable gcd and related computations on ratios
// with exponents. They avoid overflow. Further information here:
// https://github.com/mpusz/units/issues/62#issuecomment-588152833

// Computes (a * b) mod m relies on unsigned integer arithmetic, should not
// overflow
[[nodiscard]] constexpr std::uint64_t mulmod(std::uint64_t a, std::uint64_t b, std::uint64_t m)
{
  std::uint64_t res = 0;

  if (b >= m) {
    if (m > UINT64_MAX / 2u) {
      b -= m;
    } else {
      b %= m;
    }
  }

  while (a != 0) {
    if (a & 1) {
      if (b >= m - res) {
        res -= m;
      }
      res += b;
    }
    a >>= 1;

    std::uint64_t temp_b = b;
    if (b >= m - b) {
      temp_b -= m;
    }
    b += temp_b;
  }

  return res;
}

// Calculates (a ^ e) mod m , should not overflow.
[[nodiscard]] constexpr std::uint64_t modpow(std::uint64_t a, std::uint64_t e, std::uint64_t m)
{
  a %= m;
  std::uint64_t result = 1;

  while (e > 0) {
    if (e & 1) {
      result = mulmod(result, a, m);
    }
    a = mulmod(a, a, m);
    e >>= 1;
  }
  return result;
}

// gcd(a * 10 ^ e, b), should not overflow
[[nodiscard]] constexpr std::intmax_t gcdpow(std::intmax_t a, std::intmax_t e, std::intmax_t b) noexcept
{
  assert(a > 0);
  assert(e >= 0);
  assert(b > 0);

  // gcd(i, j) = gcd(j, i mod j) for j != 0 Euclid;
  //
  // gcd(a 10^e, b) = gcd(b, a 10^e mod b)
  //
  // (a 10^e) mod b -> [ (a mod b) (10^e mod b) ] mod b

  return std::gcd(
      b, static_cast<std::intmax_t>(mulmod(static_cast<std::uint64_t>(a % b),
                                           modpow(10, static_cast<std::uint64_t>(e), static_cast<std::uint64_t>(b)),
                                           static_cast<std::uint64_t>(b))));
}

constexpr void cwap(std::intmax_t& lhs, std::intmax_t& rhs)
{
  std::intmax_t tmp = lhs;
  lhs = rhs;
  rhs = tmp;
}

// Computes the rational gcd of n1/d1 x 10^e1 and n2/d2 x 10^e2
[[nodiscard]] constexpr auto gcd_frac(std::intmax_t n1, std::intmax_t d1, std::intmax_t e1, std::intmax_t n2, std::intmax_t d2,
                                      std::intmax_t e2) noexcept
{
  // Short cut for equal ratios
  if (n1 == n2 && d1 == d2 && e1 == e2) {
    return std::array{n1, d1, e1};
  }

  if (e2 > e1) {
    detail::cwap(n1, n2);
    detail::cwap(d1, d2);
    detail::cwap(e1, e2);
  }

  std::intmax_t exp = e2;  // minimum

  // gcd(a/b,c/d) = gcd(a⋅d, c⋅b) / b⋅d

  assert(std::numeric_limits<std::intmax_t>::max() / n1 > d2);
  assert(std::numeric_limits<std::intmax_t>::max() / n2 > d1);

  std::intmax_t num = detail::gcdpow(n1 * d2, e1 - e2, n2 * d1);

  assert(std::numeric_limits<std::intmax_t>::max() / d1 > d2);

  std::intmax_t den = d1 * d2;

  std::intmax_t gcd = std::gcd(num, den);

  return std::array{num / gcd, den / gcd, exp};
}

constexpr void normalize(std::intmax_t& num, std::intmax_t& den, std::intmax_t& exp)
{
  if(num == 0) {
    den = 1;
    exp = 0;
    return;
  }

  std::intmax_t gcd = std::gcd(num, den);
  num = num * (den < 0 ? -1 : 1) / gcd;
  den = detail::abs(den) / gcd;

  while (num % 10 == 0) {
    num /= 10;
    ++exp;
  }
  while (den % 10 == 0) {
    den /= 10;
    --exp;
  }
}

[[nodiscard]] static constexpr std::intmax_t safe_multiply(std::intmax_t lhs, std::intmax_t rhs)
{
  constexpr std::intmax_t c = std::uintmax_t(1) << (sizeof(std::intmax_t) * 4);

  const std::intmax_t a0 = detail::abs(lhs) % c;
  const std::intmax_t a1 = detail::abs(lhs) / c;
  const std::intmax_t b0 = detail::abs(rhs) % c;
  const std::intmax_t b1 = detail::abs(rhs) / c;

  Expects(a1 == 0 || b1 == 0);                               // overflow in multiplication
  Expects(a0 * b1 + b0 * a1 < (c >> 1));                     // overflow in multiplication
  Expects(b0 * a0 <= INTMAX_MAX);                            // overflow in multiplication
  Expects((a0 * b1 + b0 * a1) * c <= INTMAX_MAX - b0 * a0);  // overflow in multiplication

  return lhs * rhs;
}

}  // namespace units::detail
