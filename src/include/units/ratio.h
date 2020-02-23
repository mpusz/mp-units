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
#include <units/concepts.h>
#include <units/bits/ratio_maths.h>
#include <cstdint>
#include <numeric>
#include <type_traits>
#include <tuple>
#include <algorithm>

namespace units {

template<std::intmax_t Num, std::intmax_t Den = 1, std::intmax_t Exp = 0>
  requires(Den != 0)
struct ratio {
  static_assert(-INTMAX_MAX <= Num, "numerator too negative");
  static_assert(-INTMAX_MAX <= Den, "denominator too negative");

 private:
  static constexpr auto norm = detail::normalize(Num, Den, Exp);

 public:
  static constexpr std::intmax_t num = norm[0];
  static constexpr std::intmax_t den = norm[1];
  static constexpr std::intmax_t exp = norm[2];

  using type = ratio<num, den, exp>;

  static constexpr bool is_integral() {
    if constexpr (exp < 0) {
      return false;
    } else {
      return detail::gcdpow(num, exp, den) == den;
    }
  }
};

namespace detail {

template<intmax_t Num, intmax_t Den, intmax_t Exp>
inline constexpr bool is_ratio<ratio<Num, Den, Exp>> = true;

// unused, and align exponents process could be subject to overflow in extreme cases

// template<Ratio R1, Ratio R2>
// constexpr auto ratio_add_detail() {
//   std::intmax_t num1 = R1::num;
//   std::intmax_t num2 = R2::num;

//   // align exponents
//   std::intmax_t new_exp = R1::exp;
//   if constexpr (R1::exp > R2::exp) {
//     new_exp = R1::exp;
//     while (new_exp > R2::exp) {
//       num1 *= 10;
//       --new_exp;
//     }
//   } else if constexpr (R1::exp < R2::exp) {
//     new_exp = R2::exp;
//     while (R1::exp < new_exp) {
//       num2 *= 10;
//       --new_exp;
//     }
//   }

//   // common denominator
//   std::intmax_t lcm_den = std::lcm(R1::den, R2::den);
//   num1 = num1 * (lcm_den / R1::den);
//   num2 = num2 * (lcm_den / R2::den);

//   return std::array{num1 + num2, lcm_den, new_exp};
// }


// template<Ratio R1, Ratio R2>
// struct ratio_add_impl {
//   static constexpr auto detail = ratio_add_detail<R1, R2>();
//   using type = ratio<detail[0], detail[1], detail[2]>;
// };
}// namespace detail


// ratio_add : not used
// template<Ratio R1, Ratio R2>
// using ratio_add = detail::ratio_add_impl<R1, R2>::type;

// ratio_subtract : not used
// TODO implement ratio_subtract
// template<Ratio R1, Ratio R2>
// using ratio_subtract = detail::ratio_subtract_impl<R1, R2>::type;

// ratio_multiply

namespace detail {

static constexpr std::intmax_t safe_multiply(std::intmax_t lhs, std::intmax_t rhs)
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

template<typename R1, typename R2>
struct ratio_multiply_impl {
private:
  static constexpr std::intmax_t gcd1 = std::gcd(R1::num, R2::den);
  static constexpr std::intmax_t gcd2 = std::gcd(R2::num, R1::den);

  static constexpr auto norm = detail::normalize(safe_multiply(R1::num / gcd1, R2::num / gcd2),
                                                 safe_multiply(R1::den / gcd2, R2::den / gcd1),
                                                 R1::exp + R2::exp);

  static constexpr std::intmax_t norm_num = norm[0];
  static constexpr std::intmax_t norm_den = norm[1];
  static constexpr std::intmax_t norm_exp = norm[2];

public:
  using type = ratio<norm_num, norm_den, norm_exp>;
  static constexpr std::intmax_t num = type::num;
  static constexpr std::intmax_t den = type::den;
  static constexpr std::intmax_t exp = type::exp;
};

}  // namespace detail

template<Ratio R1, Ratio R2>
using ratio_multiply = detail::ratio_multiply_impl<R1, R2>::type;

// ratio_divide

namespace detail {

template<typename R1, typename R2>
struct ratio_divide_impl {
  static_assert(R2::num != 0, "division by 0");
  using type = ratio_multiply<R1, ratio<R2::den, R2::num, -R2::exp>>;
  static constexpr std::intmax_t num = type::num;
  static constexpr std::intmax_t den = type::den;
  static constexpr std::intmax_t exp = type::exp;
};

}  // namespace detail

template<Ratio R1, Ratio R2>
using ratio_divide = detail::ratio_divide_impl<R1, R2>::type;

// ratio_pow

namespace detail {

template<typename R, std::intmax_t N>
struct ratio_pow_impl {
  using type = ratio_multiply<typename ratio_pow_impl<R, N - 1>::type, R>;
};

template<typename R>
struct ratio_pow_impl<R, 1> {
  using type = R;
};

template<typename R>
struct ratio_pow_impl<R, 0> {
  using type = ratio<1>;
};

}  // namespace detail

template<Ratio R, std::intmax_t N>
using ratio_pow = detail::ratio_pow_impl<R, N>::type;

// ratio_sqrt

namespace detail {

// sqrt_impl avoids overflow and recursion
// from http://www.codecodex.com/wiki/Calculate_an_integer_square_root#C.2B.2B
// if v >= place this will fail  (so we can't quite use the last bit)
static constexpr std::intmax_t sqrt_impl(std::intmax_t v)
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

template<Ratio R>
constexpr auto make_exp_even()
{
  if constexpr (R::exp % 2 == 0)
    return std::array{R::num, R::den, R::exp}; // already even (incl zero)

  // safely make exp even, so it can be divided by 2 for square root
  if constexpr (R::exp > 0)
    return std::array{R::num * 10, R::den, R::exp - 1};
  else
    return std::array{R::num, R::den * 10, R::exp + 1};
}

template<typename R>
struct ratio_sqrt_impl {
  constexpr static auto even = detail::make_exp_even<R>();
  using type = ratio<detail::sqrt_impl(even[0]), detail::sqrt_impl(even[1]), even[2] / 2>;
};

template<std::intmax_t Den>
struct ratio_sqrt_impl<ratio<0, Den>> {
  using type = ratio<0>;
};

}  // namespace detail

template<Ratio R>
using ratio_sqrt = detail::ratio_sqrt_impl<R>::type;

// common_ratio

namespace detail {

// TODO: simplified
template<typename R1, typename R2>
struct common_ratio_impl {
  static constexpr auto res = gcd_frac(R1::num, R1::den, R1::exp, R2::num, R2::den, R2::exp);
  static constexpr std::intmax_t num = res[0];
  static constexpr std::intmax_t den = res[1];
  static constexpr std::intmax_t exp = res[2];
  using type = ratio<num, den, exp>;
};

}  // namespace detail

template<Ratio R1, Ratio R2>
using common_ratio = detail::common_ratio_impl<R1, R2>::type;

}  // namespace units
