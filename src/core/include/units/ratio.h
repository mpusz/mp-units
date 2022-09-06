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

// IWYU pragma: begin_exports
#include <units/bits/math_concepts.h>
#include <units/bits/ratio_maths.h>
#include <cstdint>
// IWYU pragma: end_exports

#include <gsl/gsl-lite.hpp>
#include <array>
#include <compare>
#include <numeric>

namespace units {

struct ratio;
constexpr ratio inverse(const ratio& r);

/**
 * @brief Provides compile-time rational arithmetic support.
 *
 * This class is really similar to @c std::ratio.  An important difference is the fact that the objects of that class
 * are used as class NTTPs rather then a type template parameter kind.
 */
struct ratio {
  std::intmax_t num;
  std::intmax_t den;

  constexpr explicit(false) ratio(std::intmax_t n, std::intmax_t d = 1) : num(n), den(d)
  {
    gsl_Expects(den != 0);
    detail::normalize(num, den);
  }

  [[nodiscard]] friend constexpr bool operator==(const ratio&, const ratio&) = default;

  [[nodiscard]] friend constexpr auto operator<=>(const ratio& lhs, const ratio& rhs) { return (lhs - rhs).num <=> 0; }

  [[nodiscard]] friend constexpr ratio operator-(const ratio& r) { return ratio(-r.num, r.den); }

  [[nodiscard]] friend constexpr ratio operator+(ratio lhs, ratio rhs)
  {
    return ratio{lhs.num * rhs.den + lhs.den * rhs.num, lhs.den * rhs.den};
  }

  [[nodiscard]] friend constexpr ratio operator-(const ratio& lhs, const ratio& rhs) { return lhs + (-rhs); }

  [[nodiscard]] friend constexpr ratio operator*(const ratio& lhs, const ratio& rhs)
  {
    const std::intmax_t gcd1 = std::gcd(lhs.num, rhs.den);
    const std::intmax_t gcd2 = std::gcd(rhs.num, lhs.den);
    return ratio(detail::safe_multiply(lhs.num / gcd1, rhs.num / gcd2),
                 detail::safe_multiply(lhs.den / gcd2, rhs.den / gcd1));
  }

  [[nodiscard]] friend constexpr ratio operator/(const ratio& lhs, const ratio& rhs) { return lhs * inverse(rhs); }
};

[[nodiscard]] constexpr ratio inverse(const ratio& r) { return ratio(r.den, r.num); }

[[nodiscard]] constexpr bool is_integral(const ratio& r) { return r.num % r.den == 0; }

// common_ratio
[[nodiscard]] constexpr ratio common_ratio(const ratio& r1, const ratio& r2)
{
  const auto res = detail::gcd_frac(r1.num, r1.den, r2.num, r2.den);
  return ratio(res[0], res[1]);
}

}  // namespace units
