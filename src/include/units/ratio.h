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

#include <units/bits/external/hacks.h>
#include <units/bits/ratio_maths.h>
#include <cstdint>
#include <numeric>
#include <type_traits>
#include <tuple>
#include <algorithm>
#include <gsl/gsl_assert>

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

  explicit constexpr ratio(std::intmax_t n, std::intmax_t d = 1, std::intmax_t e = 0): num(n), den(d), exp(e)
  {
    Expects(den != 0);
    detail::normalize(num, den, exp);
  }

  [[nodiscard]] friend constexpr bool operator==(const ratio&, const ratio&) = default;

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

template<std::intmax_t N>
[[nodiscard]] constexpr ratio pow(const ratio& r)
{
  if constexpr(N == 0)
    return ratio(1);
  else if constexpr(N == 1)
    return r;
  else
    return pow<N-1>(r) * r;
}

namespace detail {

// sqrt_impl avoids overflow and recursion
// from http://www.codecodex.com/wiki/Calculate_an_integer_square_root#C.2B.2B
// if v >= place this will fail  (so we can't quite use the last bit)
[[nodiscard]] constexpr std::intmax_t sqrt_impl(std::intmax_t v)
{
  // place = 0x4000 0000 for 32bit
  // place = 0x4000 0000 0000 0000 for 64bit
  std::intmax_t place = static_cast<std::intmax_t>(1) << (sizeof(std::intmax_t) * 8 - 2);
  while (place > v) place /= 4;  // optimized by complier as place >>= 2

  std::intmax_t root = 0;
  while (place) {
    if (v >= root + place) {
      v -= root + place;
      root += place * 2;
    }
    root /= 2;
    place /= 4;
  }
  return root;
}

[[nodiscard]] constexpr auto make_exp_even(const ratio& r)
{
  if(r.exp % 2 == 0)
    return std::array{r.num, r.den, r.exp}; // already even (incl zero)

  // safely make exp even, so it can be divided by 2 for square root
  if(r.exp > 0)
    return std::array{r.num * 10, r.den, r.exp - 1};
  else
    return std::array{r.num, r.den * 10, r.exp + 1};
}

}  // namespace detail

[[nodiscard]] constexpr ratio sqrt(const ratio& r)
{
  if(r.num == 0)
    return ratio(0);
  
  const auto even = detail::make_exp_even(r);
  return ratio(detail::sqrt_impl(even[0]), detail::sqrt_impl(even[1]), even[2] / 2);
}

// common_ratio
[[nodiscard]] constexpr ratio common_ratio(const ratio& r1, const ratio& r2)
{
  const auto res = detail::gcd_frac(r1.num, r1.den, r1.exp, r2.num, r2.den, r2.exp);
  return ratio(res[0], res[1], res[2]);
}

}  // namespace units
