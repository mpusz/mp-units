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

/**
 * @brief  A basis vector in our magnitude representation, raised to some rational power.
 *
 * The set of basis vectors must be linearly independent: that is, no product of basis powers can ever equal 1, unless
 * all exponents are zero.  To achieve this, we use arbitrarily many prime numbers as basis vectors, and also various
 * irrational numbers such as pi.
 *
 * @tparam Base  A type representing the basis vector.  Must have a numeric static `value` member.
 * @tparam Power  The rational power to which the base is raised.
 */
template<typename Base, ratio Power = ratio{1}>
struct base_power {
  using base = Base;
  static constexpr ratio power = Power;
};

/**
 * @brief  A type trait and concept to detect whether something is a valid "base power".
 */
template<typename T>
struct is_base_power;
template<typename T>
inline constexpr bool is_base_power_v = is_base_power<T>::value;
template<typename T>
concept BasePower = is_base_power_v<T>;

/**
 * @brief  A representation for positive real numbers which optimizes taking products and rational powers.
 *
 * Magnitudes can be treated as values.  Each type encodes exactly one value.  Users can multiply, divide, and compare
 * for equality using this value API.
 */
template<BasePower... BasePowers>
struct magnitude;

/**
 * @brief  A type trait and concept to detect whether something is a valid magnitude.
 *
 * In particular, these traits check for canonicalized forms: the base powers must be sorted by increasing base value,
 * and all exponents must be nonzero.
 */
template<typename T>
struct is_magnitude;
template<typename T>
inline constexpr bool is_magnitude_v = is_magnitude<T>::value;
template<typename T>
concept Magnitude = is_magnitude_v<T>;

/**
 * @brief  Compute the inverse of a Magnitude.
 */
template<Magnitude M>
struct inverse;
template<Magnitude M>
using inverse_t = typename inverse<M>::type;

/**
 * @brief  Compute the product of 0 or more Magnitudes.
 */
template<Magnitude... Mags>
struct product;
template<Magnitude... Mags>
using product_t = typename product<Mags...>::type;

/**
 * @brief  Compute the quotient of 2 Magnitudes.
 */
template<Magnitude T, Magnitude U>
using quotient_t = product_t<T, inverse_t<U>>;

namespace detail
{
// Helpers to perform prime factorization at compile time.
template<std::intmax_t N>
struct prime_factorization;
template<std::intmax_t N>
using prime_factorization_t = typename prime_factorization<N>::type;

// A way to check whether a number is prime at compile time.
constexpr bool is_prime(std::intmax_t n);
} // namespace detail

/**
 * @brief  A template to represent prime number bases.
 */
template<std::intmax_t N>
struct prime_base : std::integral_constant<std::intmax_t, N> {
  static_assert(detail::is_prime(N));
};

/**
 * @brief  Make a Magnitude that is a rational number.
 *
 * This will be the main way end users create Magnitudes.  They should rarely (if ever) create a magnitude<...> by
 * manually adding base powers.
 */
template<std::intmax_t N, std::intmax_t D = 1>
constexpr auto make_ratio() {
  return quotient_t<detail::prime_factorization_t<N>, detail::prime_factorization_t<D>>{};
}

/**
 * @brief  Make a Magnitude from a single base raised to a particular power.
 *
 * This should handle all of the remaining use cases which can't be captured by make_ratio(), i.e., any irrational
 * magnitudes.  For example:
 *   - `make_base_power<pi>()`                  to represent pi
 *   - `make_base_power<prime_base<2>, 1, 2>()` to represent sqrt(2)
 */
template<typename T, std::intmax_t N = 1, std::intmax_t D = 1>
constexpr auto make_base_power() {
  return magnitude<base_power<T, ratio{N, D}>>{};
}

/**
 * @brief  A base to represent pi.
 */
struct pi {
  static constexpr long double value = std::numbers::pi_v<long double>;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation details below.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<BasePower... Bs>
struct magnitude {
  template<Magnitude M>
  constexpr bool operator==(M) const { return std::is_same_v<magnitude, M>; }

  template<Magnitude M>
  constexpr friend auto operator*(magnitude, M) { return product_t<magnitude, M>{}; }

  template<Magnitude M>
  constexpr friend auto operator/(magnitude, M) { return quotient_t<magnitude, M>{}; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BasePower concept implementation.

// Default implementation: most things are not base powers.
template<typename T>
struct is_base_power: std::false_type {};

// To be a valid base power, one must be a base_power<B, E>, where B has a static value member which is positive.
template<typename B, ratio E>
  requires requires() { B::value; }
struct is_base_power<base_power<B, E>>
  : std::bool_constant<(B::value > 0)> {};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Magnitude concept implementation.

// Default implementation: most things are not magnitudes.
template<typename T>
struct is_magnitude: std::false_type {};

// Check whether a tuple of (possibly heterogeneously typed) values are strictly increasing.
template<typename... Ts>
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

// To be a valid magnitude, one must be a magnitude<...> of BasePowers with nonzero exponents, sorted by increasing base
// value.
template<BasePower... Bs>
struct is_magnitude<magnitude<Bs...>>
  : std::bool_constant<(strictly_increasing(std::make_tuple(Bs::base::value...)) && ((Bs::power.num != 0) && ...))> {};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inverse implementation.

// To invert a BasePower, negate all exponents.
template<BasePower B>
using base_power_inverse = base_power<typename B::base, -B::power>;

template<BasePower... Bs>
struct inverse<magnitude<Bs...>> { using type = magnitude<base_power_inverse<Bs>...>; };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Product implementation.

// Convenience utility to prepend a base_power to a magnitude.
//
// Assumes that the prepended power has a smaller base than every base power in the magnitude.
template<BasePower B, Magnitude M>
struct prepend_base;
template<BasePower B, Magnitude M>
using prepend_base_t = typename prepend_base<B, M>::type;
template<BasePower B, BasePower... Bs>
struct prepend_base<B, magnitude<Bs...>> { using type = magnitude<B, Bs...>; };

// Nullary case.
template<>
struct product<> { using type = magnitude<>; };

// Unary case.
template<Magnitude M>
struct product<M> { using type = M; };

// Binary case, where right argument is null magnitude.
template<Magnitude M>
struct product<M, magnitude<>> { using type = M; };

// Binary case, where left argument is null magnitude, and right is non-null.
template<BasePower B, BasePower... Bs>
struct product<magnitude<>, magnitude<B, Bs...>> { using type = magnitude<B, Bs...>; };

// Binary case, with distinct and non-null heads.
template<BasePower Head1, BasePower... Tail1, BasePower Head2, BasePower... Tail2>
struct product<magnitude<Head1, Tail1...>, magnitude<Head2, Tail2...>>
{
  using type = std::conditional_t<
    (Head1::base::value < Head2::base::value),
    prepend_base_t<Head1, product_t<magnitude<Tail1...>, magnitude<Head2, Tail2...>>>,
    prepend_base_t<Head2, product_t<magnitude<Head1, Tail1...>, magnitude<Tail2...>>>>;
};

// Binary case, same head.
template<typename Base, ratio Pow1, ratio Pow2, BasePower... Tail1, BasePower... Tail2>
struct product<magnitude<base_power<Base, Pow1>, Tail1...>,
               magnitude<base_power<Base, Pow2>, Tail2...>>
{
  using tail_product = product_t<magnitude<Tail1...>, magnitude<Tail2...>>;
  static constexpr auto Pow = Pow1 + Pow2;
  using type = std::conditional_t<
    Pow.num == 0,
    tail_product,
    prepend_base_t<base_power<Base, Pow>, tail_product>>;
};

// N-ary case (N > 2).
template<Magnitude T, Magnitude U, Magnitude... Tail>
struct product<T, U, Tail...> { using type = product_t<product_t<T, U>, Tail...>; };

namespace detail
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
  while (pow-- > 0) { n /= base; }
  return n;
}

// Specialization for the prime factorization of 1 (base case).
template<>
struct prime_factorization<1> { using type = magnitude<>; };

// Specialization for the prime factorization of larger numbers (recursive case).
template<std::intmax_t N>
struct prime_factorization {
  static_assert(N > 1, "Can only factor positive integers.");

  static constexpr std::intmax_t first_base = find_first_factor(N);
  static constexpr std::intmax_t first_power = multiplicity(first_base, N);
  static constexpr std::intmax_t remainder = remove_power(first_base, first_power, N);

  using type = product_t<
    magnitude<base_power<prime_base<first_base>, ratio{first_power}>>, prime_factorization_t<remainder>>;
};

constexpr bool is_prime(std::intmax_t n) { return (n > 1) && (find_first_factor(n) == n); }

} // namespace detail
} // namespace units::mag
