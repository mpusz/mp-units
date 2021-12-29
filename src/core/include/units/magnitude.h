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

// A `magnitude` represents a positive real number in a format which optimizes taking products and
// rational powers.
template <typename... base_powers>
struct magnitude;

template <typename base, ratio power = ratio{1}>
struct base_power;

template <typename T>
struct is_magnitude;
template <typename T>
inline constexpr bool is_magnitude_v = is_magnitude<T>::value;
template <typename T>
concept Magnitude = is_magnitude_v<T>;

template <std::intmax_t n>
struct int_base : std::integral_constant<std::intmax_t, n> {};
template <std::intmax_t n, std::intmax_t num = 1, std::intmax_t den = 1>
using int_base_power = base_power<int_base<n>, ratio{num, den}>;

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
template <std::intmax_t n>
struct prime_factorization;
template <std::intmax_t n>
using prime_factorization_t = typename prime_factorization<n>::type;
} // namespace detail

template <std::intmax_t num = 1, std::intmax_t den = 1>
using ratio_t = quotient_t<detail::prime_factorization_t<num>, detail::prime_factorization_t<den>>;

struct pi {
  static inline constexpr long double value = std::numbers::pi_v<long double>;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation details below.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename... base_powers>
struct magnitude {
  bool operator==(magnitude) const { return true; }

  template <typename... other_base_powers>
  bool operator==(magnitude<other_base_powers...>) const { return false; }
};

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

template <typename... bases, ratio... powers>
struct is_magnitude<magnitude<base_power<bases, powers>...>>
  : std::bool_constant<(
      strictly_increasing(std::make_tuple(bases::value...))
      && ((powers.num != 0) && ...))> {};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Inverse implementation.

template <typename... bases, ratio... powers>
struct inverse<magnitude<base_power<bases, powers>...>> {
  using type = magnitude<base_power<bases, -powers>...>;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Product implementation.

// Convenience utility to prepend a base_power to a magnitude.
//
// Assumes that the prepended power has a smaller base than every base power in the magnitude.
template <typename new_base, Magnitude M>
struct prepend_base;
template <typename new_base, Magnitude M>
using prepend_base_t = typename prepend_base<new_base, M>::type;
template <typename new_base, typename... old_bases>
struct prepend_base<new_base, magnitude<old_bases...>> {
  using type = magnitude<new_base, old_bases...>;
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
template <typename head, typename... tail>
struct product<magnitude<>, magnitude<head, tail...>> { using type = magnitude<head, tail...>; };

// Binary case, with distinct heads.
template <
  typename base1,
  ratio pow1,
  typename... tail1,
  typename base2,
  ratio pow2,
  typename... tail2>
struct product<magnitude<base_power<base1, pow1>, tail1...>,
               magnitude<base_power<base2, pow2>, tail2...>>
{
  using mag1 = magnitude<base_power<base1, pow1>, tail1...>;
  using mag2 = magnitude<base_power<base2, pow2>, tail2...>;

  using type = std::conditional_t<
    (base1::value < base2::value),
    prepend_base_t<base_power<base1, pow1>, product_t<magnitude<tail1...>, mag2>>,
    prepend_base_t<base_power<base2, pow2>, product_t<mag1, magnitude<tail2...>>>>;
};

// Binary case, same head.
template <typename base, ratio pow1, ratio pow2, typename... tail1, typename... tail2>
struct product<magnitude<base_power<base, pow1>, tail1...>,
               magnitude<base_power<base, pow2>, tail2...>>
{
  using tail_product = product_t<magnitude<tail1...>, magnitude<tail2...>>;
  using type = std::conditional_t<
    ((pow1 + pow2).num == 0),
    tail_product,
    prepend_base_t<base_power<base, pow1 + pow2>, tail_product>>;
};

// N-ary case (N > 2).
template <Magnitude T, Magnitude U, typename... tail>
struct product<T, U, tail...>
{
  using type = product_t<product_t<T, U>, tail...>;
};

} // namespace units::mag
