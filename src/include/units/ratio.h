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
#include <cstdint>
#include <numeric>
#include <type_traits>
#include <tuple>
#include <algorithm>

namespace units {

namespace detail {

template<typename T>
[[nodiscard]] constexpr T abs(T v) noexcept
{
  return v < 0 ? -v : v;
}

constexpr std::tuple<std::intmax_t, std::intmax_t, std::intmax_t>  normalize(std::intmax_t num, std::intmax_t den, std::intmax_t exp) {
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
  return std::make_tuple(num, den, exp);
}

}  // namespace detail

template<std::intmax_t Num, std::intmax_t Den = 1, std::intmax_t Exp = 0>
  requires(Den != 0)
struct ratio {
  static_assert(-INTMAX_MAX <= Num, "numerator too negative");
  static_assert(-INTMAX_MAX <= Den, "denominator too negative");

 private:
  static constexpr auto norm = detail::normalize(Num, Den, Exp);

 public:
  static constexpr std::intmax_t num = std::get<0>(norm);
  static constexpr std::intmax_t den = std::get<1>(norm);
  static constexpr std::intmax_t exp = std::get<2>(norm);

  using type = ratio<num, den, exp>;
};

namespace detail {

template<intmax_t Num, intmax_t Den, intmax_t Exp>
inline constexpr bool is_ratio<ratio<Num, Den, Exp>> = true;

}  // namespace detail


// ratio_add
// TODO implement ratio_add
// template<Ratio R1, Ratio R2>
// using ratio_add = detail::ratio_add_impl<R1, R2>::type;

// ratio_subtract
// TODO implement ratio_subtract
// template<Ratio R1, Ratio R2>
// using ratio_subtract = detail::ratio_subtract_impl<R1, R2>::type;

// ratio_multiply

namespace detail {

static constexpr std::intmax_t safe_multiply(std::intmax_t lhs, std::intmax_t rhs)
{
  constexpr std::uintmax_t c = std::uintmax_t(1) << (sizeof(std::intmax_t) * 4);

  const std::uintmax_t a0 = detail::abs(lhs) % c;
  const std::uintmax_t a1 = detail::abs(lhs) / c;
  const std::uintmax_t b0 = detail::abs(rhs) % c;
  const std::uintmax_t b1 = detail::abs(rhs) / c;

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

  static constexpr std::intmax_t norm_num = std::get<0>(norm);
  static constexpr std::intmax_t norm_den = std::get<1>(norm);
  static constexpr std::intmax_t norm_exp = std::get<2>(norm);

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

template<typename R, std::size_t N>
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

template<Ratio R, std::size_t N>
using ratio_pow = detail::ratio_pow_impl<R, N>::type;

// ratio_sqrt

namespace detail {

constexpr std::intmax_t sqrt_impl(std::intmax_t v, std::intmax_t l, std::intmax_t r)
{
  if (l == r) return r;

  const auto mid = (r + l) / 2;
  if (mid * mid >= v)
    return sqrt_impl(v, l, mid);
  else
    return sqrt_impl(v, mid + 1, r);
}

static constexpr std::intmax_t sqrt_impl(std::intmax_t v) { return sqrt_impl(v, 1, v); }

template<typename R>
struct ratio_sqrt_impl {
  // TODO  this is broken..need /2 logic on EXP
  using type = ratio<detail::sqrt_impl(R::num), detail::sqrt_impl(R::den)>;
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
  static constexpr std::intmax_t gcd_num = std::gcd(R1::num, R2::num);
  static constexpr std::intmax_t gcd_den = std::gcd(R1::den, R2::den);
  using type = ratio<gcd_num, (R1::den / gcd_den) * R2::den, std::min(R1::exp, R2::exp)>;
};

}  // namespace detail

template<Ratio R1, Ratio R2>
using common_ratio = detail::common_ratio_impl<R1, R2>::type;

}  // namespace units
