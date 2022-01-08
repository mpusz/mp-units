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
#include <units/bits/pow.h>
#include <units/bits/ratio_maths.h>
#include <units/bits/root.h>
#include <cstdint>
// IWYU pragma: end_exports

#include <array>
#include <numeric>
#include <gsl/gsl-lite.hpp>

namespace units {

struct ratio;
constexpr ratio inverse(const ratio& r);

/**
 * @brief Provides compile-time rational arithmetic support.
 *
 * This class is really similar to @c std::ratio but gets an additional `Exp`
 * template parameter that defines the exponent of the ratio. Another important
 * difference is the fact that the objects of that class are used as class NTTPs
 * rather then a type template parameter kind.
 */
struct ratio {
  std::intmax_t num;
  std::intmax_t den;
  std::intmax_t exp;

  constexpr explicit(false) ratio(std::intmax_t n, std::intmax_t d = 1, std::intmax_t e = 0): num(n), den(d), exp(e)
  {
    gsl_Expects(den != 0);
    detail::normalize(num, den, exp);
  }

  [[nodiscard]] friend constexpr bool operator==(const ratio&, const ratio&) = default;

  [[nodiscard]] friend constexpr ratio operator-(const ratio& r)
  {
    return ratio(-r.num, r.den, r.exp);
  }

  [[nodiscard]] friend constexpr ratio operator+(ratio lhs, ratio rhs)
  {
    // First, get the inputs into a common exponent.
    const auto common_exp = std::min(lhs.exp, rhs.exp);
    auto commonify = [common_exp](ratio &r) {
      while (r.exp > common_exp) {
        r.num *= 10;
        --r.exp;
      }
    };
    commonify(lhs);
    commonify(rhs);

    return ratio{lhs.num * rhs.den + lhs.den * rhs.num, lhs.den * rhs.den, common_exp};
  }

  [[nodiscard]] friend constexpr ratio operator*(const ratio& lhs, const ratio& rhs)
  {
    const std::intmax_t gcd1 = std::gcd(lhs.num, rhs.den);
    const std::intmax_t gcd2 = std::gcd(rhs.num, lhs.den);
    return ratio(detail::safe_multiply(lhs.num / gcd1, rhs.num / gcd2),
                 detail::safe_multiply(lhs.den / gcd2, rhs.den / gcd1),
                 lhs.exp + rhs.exp);
  }

  [[nodiscard]] friend constexpr ratio operator/(const ratio& lhs, const ratio& rhs)
  {
    return lhs * inverse(rhs);
  }
};

[[nodiscard]] constexpr ratio inverse(const ratio& r)
{
  return ratio(r.den, r.num, -r.exp);
}

[[nodiscard]] constexpr bool is_integral(const ratio& r)
{
  if(r.exp < 0) {
    return false;
  } else {
    return detail::gcdpow(r.num, r.exp, r.den) == r.den;
  }
}

namespace detail {

[[nodiscard]] constexpr auto make_exp_align(const ratio& r, std::intmax_t alignment)
{
  gsl_Expects(alignment > 0);
  const std::intmax_t rem = r.exp % alignment;

  if (rem == 0) {  // already aligned
    return std::array{r.num, r.den, r.exp};
  }

  if (r.exp > 0) {  // remainder is positive
    return std::array{r.num * ipow10(rem), r.den, r.exp - rem};
  }

  // remainder is negative
  return std::array{r.num, r.den * ipow10(-rem), r.exp - rem};
}

template<std::intmax_t N>
  requires gt_zero<N>
[[nodiscard]] constexpr ratio root(const ratio& r)
{
  if constexpr (N == 1) {
    return r;
  } else {
    if (r.num == 0) {
      return ratio(0);
    }

    const auto aligned = make_exp_align(r, N);
    return ratio(iroot<N>(aligned[0]), iroot<N>(aligned[1]), aligned[2] / N);
  }
}

}  // namespace detail

template<std::intmax_t Num, std::intmax_t Den = 1>
  requires detail::non_zero<Den>
[[nodiscard]] constexpr ratio pow(const ratio& r)
{
  if constexpr (Num == 0) {
    return ratio(1);
  } else if constexpr (Num == Den) {
    return r;
  } else {
    // simplify factors first and compute power for positive exponent
    constexpr std::intmax_t gcd = std::gcd(Num, Den);
    constexpr std::intmax_t num = detail::abs(Num / gcd);
    constexpr std::intmax_t den = detail::abs(Den / gcd);

    // integer root loses precision so do pow first
    const ratio result = detail::root<den>(detail::pow_impl<num>(r));

    if constexpr (Num * Den < 0) {  // account for negative exponent
      return inverse(result);
    } else {
      return result;
    }
  }
}

[[nodiscard]] constexpr ratio sqrt(const ratio& r) { return pow<1, 2>(r); }

[[nodiscard]] constexpr ratio cbrt(const ratio& r) { return pow<1, 3>(r); }

// common_ratio
[[nodiscard]] constexpr ratio common_ratio(const ratio& r1, const ratio& r2)
{
  const auto res = detail::gcd_frac(r1.num, r1.den, r1.exp, r2.num, r2.den, r2.exp);
  return ratio(res[0], res[1], res[2]);
}

}  // namespace units
