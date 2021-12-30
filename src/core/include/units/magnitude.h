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

#include <units/ratio.h>
#include <cstdint>
#include <numbers>

namespace units::mag
{

template <typename base_, ratio power_ = ratio{1}>
struct base_power {
  using base = base_;
  static inline constexpr ratio power = power_;
};

template <typename T>
struct is_base_power;
template <typename T>
inline constexpr bool is_base_power_v = is_base_power<T>::value;
template <typename T>
concept BasePower = is_base_power_v<T>;

// A `magnitude` represents a positive real number in a format which optimizes taking products and
// rational powers.
template <BasePower... base_powers>
struct magnitude;

template <typename T>
struct is_magnitude;
template <typename T>
inline constexpr bool is_magnitude_v = is_magnitude<T>::value;
template <typename T>
concept Magnitude = is_magnitude_v<T>;

template <std::intmax_t N>
struct int_base : std::integral_constant<std::intmax_t, N> {};
template <std::intmax_t N, std::intmax_t num = 1, std::intmax_t den = 1>
using int_base_power = base_power<int_base<N>, ratio{num, den}>;

template <Magnitude M>
struct inverse;
template <Magnitude M>
using inverse_t = typename inverse<M>::type;

template <Magnitude... Mags>
struct product;
template <Magnitude... Mags>
using product_t = typename product<Mags...>::type;

template <Magnitude T, Magnitude U>
using quotient_t = product_t<T, inverse_t<U>>;

namespace detail
{
template <std::intmax_t N>
struct prime_factorization;
template <std::intmax_t N>
using prime_factorization_t = typename prime_factorization<N>::type;
} // namespace detail

template <std::intmax_t N, std::intmax_t D = 1>
constexpr auto make_ratio() {
  return quotient_t<detail::prime_factorization_t<N>, detail::prime_factorization_t<D>>{};
}

struct pi {
  static inline constexpr long double value = std::numbers::pi_v<long double>;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation details below.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <BasePower... Bs>
struct magnitude {
  template <Magnitude M>
  constexpr bool operator==(M) const { return std::is_same_v<magnitude, M>; }

  template <Magnitude M>
  constexpr friend auto operator*(magnitude, M) { return product_t<magnitude, M>{}; }

  template <Magnitude M>
  constexpr friend auto operator/(magnitude, M) { return quotient_t<magnitude, M>{}; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// BasePower concept implementation.

template <typename T>
struct is_base_power: std::false_type {};

template <typename B, ratio E>
  requires requires() { B::value; }
struct is_base_power<base_power<B, E>>
  : std::bool_constant<(B::value > 0)> {};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Magnitude concept implementation.

template <typename T>
struct is_magnitude: std::false_type {};

// Check whether a tuple of (possibly heterogeneously typed) values are strictly increasing.
template <typename... Ts>
constexpr bool strictly_increasing(const std::tuple<Ts...> &ts) {
  // Carefully handle different sizes, avoiding unsigned integer underflow.
  constexpr auto num_comparisons = [](auto num_elements) {
    return (num_elements > 1) ? (num_elements - 1) : 0;
  }(sizeof...(Ts));

  // Compare zero or more pairs of neighbours as needed.
  return [&ts]<std::size_t... Is>(std::integer_sequence<std::size_t, Is...>) {
    return ((std::get<Is>(ts) < std::get<Is + 1>(ts)) && ...);
  }(std::make_index_sequence<num_comparisons>());
}

template <BasePower... Bs>
struct is_magnitude<magnitude<Bs...>>
  : std::bool_constant<(
      strictly_increasing(std::make_tuple(Bs::base::value...))
      && ((Bs::power.num != 0) && ...))> {};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Inverse implementation.

template<BasePower B>
using base_power_inverse = base_power<typename B::base, -B::power>;

template <BasePower... Bs>
struct inverse<magnitude<Bs...>> {
  using type = magnitude<base_power_inverse<Bs>...>;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Product implementation.

// Convenience utility to prepend a base_power to a magnitude.
//
// Assumes that the prepended power has a smaller base than every base power in the magnitude.
template <BasePower B, Magnitude M>
struct prepend_base;
template <BasePower B, Magnitude M>
using prepend_base_t = typename prepend_base<B, M>::type;
template <BasePower B, BasePower... Bs>
struct prepend_base<B, magnitude<Bs...>> {
  using type = magnitude<B, Bs...>;
};

// Nullary case.
template <>
struct product<> { using type = magnitude<>; };

// Unary case.
template <Magnitude M>
struct product<M> { using type = M; };

// Binary case, where right argument is null magnitude.
template <Magnitude M>
struct product<M, magnitude<>> { using type = M; };

// Binary case, where left argument is null magnitude, and right is non-null.
template <BasePower B, BasePower... Bs>
struct product<magnitude<>, magnitude<B, Bs...>> { using type = magnitude<B, Bs...>; };

// Binary case, with distinct and non-null heads.
template <BasePower Head1, BasePower... Tail1, BasePower Head2, BasePower... Tail2>
struct product<magnitude<Head1, Tail1...>, magnitude<Head2, Tail2...>>
{
  using type = std::conditional_t<
    (Head1::base::value < Head2::base::value),
    prepend_base_t<Head1, product_t<magnitude<Tail1...>, magnitude<Head2, Tail2...>>>,
    prepend_base_t<Head2, product_t<magnitude<Head1, Tail1...>, magnitude<Tail2...>>>>;
};

// Binary case, same head.
template <typename Base, ratio Pow1, ratio Pow2, BasePower... Tail1, BasePower... Tail2>
struct product<magnitude<base_power<Base, Pow1>, Tail1...>,
               magnitude<base_power<Base, Pow2>, Tail2...>>
{
  using tail_product = product_t<magnitude<Tail1...>, magnitude<Tail2...>>;
  using type = std::conditional_t<
    ((Pow1 + Pow2).num == 0),
    tail_product,
    prepend_base_t<base_power<Base, Pow1 + Pow2>, tail_product>>;
};

// N-ary case (N > 2).
template <Magnitude T, Magnitude U, Magnitude... Tail>
struct product<T, U, Tail...>
{
  using type = product_t<product_t<T, U>, Tail...>;
};

namespace detail
{

////////////////////////////////////////////////////////////////////////////////////////////////////
// Prime factorization implementation.

// Find the smallest prime factor of `n`.
constexpr std::intmax_t find_first_factor(std::intmax_t n)
{
  for (std::intmax_t f = 2; f * f <= n; f += 1 + (f % 2))
  {
    if (n % f == 0) { return f; }
  }
  return n;
}

// The exponent of `factor` in the prime factorization of `n`.
constexpr std::intmax_t multiplicity(std::intmax_t factor, std::intmax_t n)
{
  std::intmax_t m = 0;
  while (n % factor == 0)
  {
    n /= factor;
    ++m;
  }
  return m;
}

// Divide a number by a given base raised to some power.
//
// Undefined unless base > 1, pow >= 0, and (base ^ pow) evenly divides n.
constexpr std::intmax_t remove_power(std::intmax_t base, std::intmax_t pow, std::intmax_t n)
{
  while (pow-- > 0)
  {
    n /= base;
  }
  return n;
}

// Specialization for the prime factorization of 1 (base case).
template <>
struct prime_factorization<1> { using type = magnitude<>; };

// Specialization for the prime factorization of larger numbers (recursive case).
template <std::intmax_t N>
struct prime_factorization {
  static_assert(N > 1, "Can only factor positive integers.");

  static inline constexpr std::intmax_t first_base = find_first_factor(N);
  static inline constexpr std::intmax_t first_power = multiplicity(first_base, N);
  static inline constexpr std::intmax_t remainder = remove_power(first_base, first_power, N);

  using type = product_t<
    magnitude<int_base_power<first_base, first_power>>, prime_factorization_t<remainder>>;
};

} // namespace detail
} // namespace units::mag
