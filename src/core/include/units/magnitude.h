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

namespace detail
{
// Helpers to perform prime factorization at compile time.
template<std::intmax_t N>
  requires requires { N > 0; }
struct prime_factorization;
template<std::intmax_t N>
static constexpr auto prime_factorization_v = prime_factorization<N>::value;

// A way to check whether a number is prime at compile time.
constexpr bool is_prime(std::intmax_t n);
} // namespace detail

// Integer rep is for prime numbers; long double is for any irrational base we permit.
template<typename T>
concept BaseRep = std::is_same_v<T, int> || std::is_same_v<T, long double>;

/**
 * @brief  A basis vector in our magnitude representation, raised to some rational power.
 *
 * The set of basis vectors must be linearly independent: that is, no product of basis powers can ever equal 1, unless
 * all exponents are zero.  To achieve this, we use the following kinds of basis vectors.
 *   - Prime numbers.  (These are the only allowable values for `int` base.)
 *   - Certain selected irrational numbers, such as pi.
 *
 * Before allowing a new irrational base, make sure that it _cannot_ be represented as the product of rational powers of
 * _existing_ bases, including both prime numbers and any other irrational bases.  For example, even though sqrt(2) is
 * irrational, we must not ever use it as a base
 */
template<BaseRep T>
struct base_power {
  // The value of the basis vector.
  T base;

  // The rational power to which the base is raised.
  ratio power{1};
};

template<BaseRep T, std::convertible_to<ratio> U>
base_power(T, U) -> base_power<T>;

template<BaseRep T>
base_power(T) -> base_power<T>;

template<typename T, typename U>
constexpr bool operator==(base_power<T> t, base_power<U> u) {
  return std::is_same_v<T, U> && (t.base == u.base) && (t.power == u.power);
}

template<BaseRep T>
constexpr auto inverse(base_power<T> bp) {
  bp.power = -bp.power;
  return bp;
}

namespace detail
{
template<BaseRep T>
constexpr bool is_valid_base_power(const base_power<T> &bp) {
  if (bp.power == 0) { return false; }

  if constexpr (std::is_same_v<T, int>) { return is_prime(bp.base); }
  else if constexpr (std::is_same_v<T, long double>) { return bp.base > 0; }
  else { return false; }  // Unreachable.
}
} // namespace detail

template<typename T>
concept BasePower = std::is_same_v<T, base_power<int>> || std::is_same_v<T, base_power<long double>>;

/**
 * @brief  A representation for positive real numbers which optimizes taking products and rational powers.
 *
 * Magnitudes can be treated as values.  Each type encodes exactly one value.  Users can multiply, divide, and compare
 * for equality using this value API.
 */
template<BasePower auto... BasePowers>
  requires requires {
    // (is_valid_base_power(BasePowers) && ... && strictly_increasing(std::make_tuple(BasePowers.base...)));
    (detail::is_valid_base_power(BasePowers) && ...);
  }
struct magnitude {};

template<BasePower auto... LeftBPs, BasePower auto... RightBPs>
constexpr bool operator==(magnitude<LeftBPs...>, magnitude<RightBPs...>) { return ((LeftBPs == RightBPs) && ...); }

template<BasePower auto... BasePowers>
constexpr auto inverse(magnitude<BasePowers...>) { return magnitude<inverse(BasePowers)...>{}; }

constexpr auto operator*(magnitude<>, magnitude<>) { return magnitude<>{}; }

template<BasePower auto... BPs>
constexpr auto operator*(magnitude<>, magnitude<BPs...> m) { return m; }

template<BasePower auto... BPs>
constexpr auto operator*(magnitude<BPs...> m, magnitude<>) { return m; }

template<BasePower auto H1, BasePower auto... T1, BasePower auto H2, BasePower auto... T2>
constexpr auto operator*(magnitude<H1, T1...>, magnitude<H2, T2...>) {
  // Shortcut for prepending, which makes it easier to implement some of the other cases.
  if constexpr ((sizeof...(T1) == 0) && H1.base < H2.base) { return magnitude<H1, H2, T2...>{}; }

  if constexpr (H1.base == H2.base) {
    constexpr auto partial_product = magnitude<T1...>{} * magnitude<T2...>{};
    constexpr base_power new_head{H1.base, (H1.power + H2.power)};

    if constexpr (new_head.power == 0) {
      return partial_product;
    } else {
      return magnitude<new_head>{} * partial_product;
    }
  } else if constexpr(H1.base < H2.base){
    return magnitude<H1>{} * (magnitude<T1...>{} * magnitude<H2, T2...>{});
  } else { // We know H2.base < H1.base
    return magnitude<H2>{} * (magnitude<H1, T1...>{} * magnitude<T2...>{});
  }
}

template<BasePower auto... LeftBPs, BasePower auto... RightBPs>
constexpr auto operator/(magnitude<LeftBPs...> l, magnitude<RightBPs...> r) { return l * inverse(r); }

/**
 * @brief  Make a Magnitude that is a rational number.
 *
 * This will be the main way end users create Magnitudes.  They should rarely (if ever) create a magnitude<...> by
 * manually adding base powers.
 */
template<std::intmax_t N, std::intmax_t D = 1>
constexpr auto make_ratio() { return detail::prime_factorization_v<N> / detail::prime_factorization_v<D>; }

/**
 * @brief  A base to represent pi.
 */
template<ratio Power>
  requires requires { Power != 0; }
constexpr auto pi_power() { return base_power{std::numbers::pi_v<long double>, Power}; }

template<ratio Power>
constexpr auto pi_to_the() { return magnitude<pi_power<Power>()>{}; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation details below.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Magnitude concept implementation.

template<typename Predicate>
struct pairwise_all {
  Predicate predicate;

  template<typename... Ts>
  constexpr bool operator()(Ts&&... ts) const {
    // Carefully handle different sizes, avoiding unsigned integer underflow.
    constexpr auto num_comparisons = [](auto num_elements) {
      return (num_elements > 1) ? (num_elements - 1) : 0;
    }(sizeof...(Ts));

    // Compare zero or more pairs of neighbours as needed.
    return [this]<std::size_t... Is>(std::tuple<Ts...> &&t, std::index_sequence<Is...>) {
      return (predicate(std::get<Is>(t), std::get<Is + 1>(t)) && ...);
    }(std::make_tuple(std::forward<Ts>(ts)...), std::make_index_sequence<num_comparisons>());
  }
};

template<typename T>
pairwise_all(T) -> pairwise_all<T>;

// Check whether a tuple of (possibly heterogeneously typed) values are strictly increasing.
template<typename... Ts>
constexpr bool strictly_increasing(Ts&&... ts) {
  return pairwise_all{std::less{}}(std::forward<Ts>(ts)...);
}

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
struct prime_factorization<1> { static constexpr magnitude<> value{}; };

// Specialization for the prime factorization of larger numbers (recursive case).
template<std::intmax_t N>
  requires requires { N > 0; }
struct prime_factorization {
  static constexpr int first_base = find_first_factor(N);
  static constexpr std::intmax_t first_power = multiplicity(first_base, N);
  static constexpr std::intmax_t remainder = remove_power(first_base, first_power, N);

  static constexpr auto value = magnitude<base_power{first_base, first_power}>{}
      * prime_factorization_v<remainder>;
};

constexpr bool is_prime(std::intmax_t n) { return (n > 1) && (find_first_factor(n) == n); }

} // namespace detail
} // namespace units::mag
