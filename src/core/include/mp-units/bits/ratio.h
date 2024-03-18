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

#include <mp-units/bits/external/hacks.h>
#include <mp-units/bits/module_macros.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <gsl/gsl-lite.hpp>
#include <compare>
#include <cstdint>
#include <numeric>
#endif

namespace mp_units {

namespace detail {

template<typename T>
[[nodiscard]] MP_UNITS_CONSTEVAL T abs(T v) noexcept
{
  return v < 0 ? -v : v;
}

[[nodiscard]] consteval std::intmax_t safe_multiply(std::intmax_t lhs, std::intmax_t rhs)
{
  constexpr std::intmax_t c = std::uintmax_t(1) << (sizeof(std::intmax_t) * 4);

  const std::intmax_t a0 = abs(lhs) % c;
  const std::intmax_t a1 = abs(lhs) / c;
  const std::intmax_t b0 = abs(rhs) % c;
  const std::intmax_t b1 = abs(rhs) / c;

  gsl_Assert(a1 == 0 || b1 == 0);                               // overflow in multiplication
  gsl_Assert(a0 * b1 + b0 * a1 < (c >> 1));                     // overflow in multiplication
  gsl_Assert(b0 * a0 <= INTMAX_MAX);                            // overflow in multiplication
  gsl_Assert((a0 * b1 + b0 * a1) * c <= INTMAX_MAX - b0 * a0);  // overflow in multiplication

  return lhs * rhs;
}

}  // namespace detail

/**
 * @brief Provides compile-time rational arithmetic support.
 *
 * This class is really similar to @c std::ratio.  An important difference is the fact that the objects of that class
 * are used as class NTTPs rather then a type template parameter kind.
 */
MP_UNITS_EXPORT struct ratio {
  std::intmax_t num;
  std::intmax_t den;

  MP_UNITS_CONSTEVAL explicit(false) ratio(std::intmax_t n, std::intmax_t d = 1) : num{n}, den{d}
  {
    gsl_Expects(den != 0);
    if (num == 0)
      den = 1;
    else {
      std::intmax_t gcd = std::gcd(num, den);
      num = num * (den < 0 ? -1 : 1) / gcd;
      den = detail::abs(den) / gcd;
    }
  }

  [[nodiscard]] friend MP_UNITS_CONSTEVAL bool operator==(ratio, ratio) = default;
  [[nodiscard]] friend MP_UNITS_CONSTEVAL auto operator<=>(ratio lhs, ratio rhs) { return (lhs - rhs).num <=> 0; }

  [[nodiscard]] friend MP_UNITS_CONSTEVAL ratio operator-(ratio r) { return ratio{-r.num, r.den}; }

  [[nodiscard]] friend MP_UNITS_CONSTEVAL ratio operator+(ratio lhs, ratio rhs)
  {
    return ratio{lhs.num * rhs.den + lhs.den * rhs.num, lhs.den * rhs.den};
  }

  [[nodiscard]] friend MP_UNITS_CONSTEVAL ratio operator-(ratio lhs, ratio rhs) { return lhs + (-rhs); }

  [[nodiscard]] friend consteval ratio operator*(ratio lhs, ratio rhs)
  {
    const std::intmax_t gcd1 = std::gcd(lhs.num, rhs.den);
    const std::intmax_t gcd2 = std::gcd(rhs.num, lhs.den);
    return ratio{detail::safe_multiply(lhs.num / gcd1, rhs.num / gcd2),
                 detail::safe_multiply(lhs.den / gcd2, rhs.den / gcd1)};
  }

  [[nodiscard]] friend consteval ratio operator/(ratio lhs, ratio rhs) { return lhs * ratio{rhs.den, rhs.num}; }
};

[[nodiscard]] consteval bool is_integral(ratio r) { return r.num % r.den == 0; }

[[nodiscard]] consteval ratio common_ratio(ratio r1, ratio r2)
{
  if (r1.num == r2.num && r1.den == r2.den) return ratio{r1.num, r1.den};

  // gcd(a/b,c/d) = gcd(a⋅d, c⋅b) / b⋅d
  gsl_Assert(std::numeric_limits<std::intmax_t>::max() / r1.num > r2.den);
  gsl_Assert(std::numeric_limits<std::intmax_t>::max() / r2.num > r1.den);
  gsl_Assert(std::numeric_limits<std::intmax_t>::max() / r1.den > r2.den);

  std::intmax_t num = std::gcd(r1.num * r2.den, r2.num * r1.den);
  std::intmax_t den = r1.den * r2.den;
  std::intmax_t gcd = std::gcd(num, den);
  return ratio{num / gcd, den / gcd};
}

}  // namespace mp_units
