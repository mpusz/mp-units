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

namespace units::mag {

/**
 * @brief  Any type which can be used as a basis vector in a BasePower.
 *
 * We have two categories.
 *
 * The first is just an `int`.  This is for prime number bases.  These can always be used directly as NTTPs.
 *
 * The second category is a _custom type_, which has a static member variable of type `long double` that holds its
 * value.  We choose `long double` to get the greatest degree of precision; users who need a different type can convert
 * from this at compile time.  This category is for any irrational base we admit into our representation (on which, more
 * details below).
 *
 * The reason we can't hold the value directly for floating point bases is so that we can support some compilers (e.g.,
 * GCC 10) which don't yet permit floating point NTTPs.
 */
template<typename T>
concept BaseRep = std::is_same_v<T, int> || std::is_same_v<std::remove_cvref_t<decltype(T::value)>, long double>;

/**
 * @brief  A basis vector in our magnitude representation, raised to some rational power.
 *
 * The public API is that there is a `power` member variable (of type `ratio`), and a `get_base()` member function (of
 * type either `int` or `long double`, as appropriate), for any specialization.
 *
 * These types exist to be used as NTTPs for the variadic `magnitude<...>` template.  We represent a magnitude (which is
 * a positive real number) as the product of rational powers of "basis vectors", where each "basis vector" is a positive
 * real number.  "Addition" in this vector space corresponds to _multiplying_ two real numbers.  "Scalar multiplication"
 * corresponds to _raising_ a real number to a _rational power_.  Thus, this representation of positive real numbers
 * maps them onto a vector space over the rationals, supporting the operations of products and rational powers.
 *
 * (Note that this is the same representation we already use for dimensions.)
 *
 * As in any vector space, the set of basis vectors must be linearly independent: that is, no product of basis powers
 * can ever give the null vector (which in this case represents the number 1), unless all scalars (again, in this case,
 * _powers_) are zero.  To achieve this, we use the following kinds of basis vectors.
 *   - Prime numbers.  (These are the only allowable values for `int` base.)
 *   - Certain selected irrational numbers, such as pi.
 *
 * Before allowing a new irrational base, make sure that it _cannot_ be represented as the product of rational powers of
 * _existing_ bases, including both prime numbers and any other irrational bases.  For example, even though `sqrt(2)` is
 * irrational, we must not ever use it as a base; instead, we would use `base_power{2, ratio{1, 2}}`.
 */
template<BaseRep T>
struct base_power {
  // The rational power to which the base is raised.
  ratio power{1};

  constexpr long double get_base() const { return T::value; }
};

/**
 * @brief  Specialization for prime number bases.
 */
template<>
struct base_power<int> {
  // The value of the basis "vector".  Must be prime to be used with `magnitude` (below).
  int base;

  // The rational power to which the base is raised.
  ratio power{1};

  constexpr int get_base() const { return base; }
};

/**
 * @brief  Deduction guides for base_power: only permit deducing integral bases.
 */
template<std::integral T, std::convertible_to<ratio> U>
base_power(T, U) -> base_power<int>;
template<std::integral T>
base_power(T) -> base_power<int>;

// Implementation for BasePower concept (below).
namespace detail {
template<typename T>
struct is_base_power : std::false_type {};
template<BaseRep T>
struct is_base_power<base_power<T>> : std::true_type {};
} // namespace detail

/**
 * @brief  Concept to detect whether a _type_ is a valid base power.
 *
 * Note that this is somewhat incomplete.  We must also detect whether a _value_ of that type is valid for use with
 * `magnitude<...>`.  We will defer that second check to the constraints on the `magnitude` template.
 */
template<typename T>
concept BasePower = detail::is_base_power<T>::value;

/**
 * @brief  Equality detection for two base powers.
 */
template<BasePower T, BasePower U>
constexpr bool operator==(T t, U u) {
  return std::is_same_v<T, U> && (t.get_base() == u.get_base()) && (t.power == u.power);
}

/**
 * @brief  The (multiplicative) inverse of a BasePower.
 */
template<BasePower BP>
constexpr auto inverse(BP bp) {
  bp.power = -bp.power;
  return bp;
}

// Implementation helpers for `magnitude<...>` constraints (below).
namespace detail {
constexpr bool is_valid_base_power(const BasePower auto &bp);

template<typename... Ts>
constexpr bool strictly_increasing(Ts&&... ts);
} // namespace detail

/**
 * @brief  A representation for positive real numbers which optimizes taking products and rational powers.
 *
 * Magnitudes can be treated as values.  Each type encodes exactly one value.  Users can multiply, divide, and compare
 * for equality.
 */
template<BasePower auto... BPs>
  requires requires {
    (detail::is_valid_base_power(BPs) && ... && detail::strictly_increasing(BPs.get_base()...));
  }
struct magnitude {};

// Implementation for Magnitude concept (below).
namespace detail {
template<typename T>
struct is_magnitude : std::false_type {};
template<BasePower auto... BPs>
struct is_magnitude<magnitude<BPs...>> : std::true_type {};
} // namespace detail

/**
 * @brief  Concept to detect whether T is a valid Magnitude.
 */
template<typename T>
concept Magnitude = detail::is_magnitude<T>::value;

/**
 * @brief  Convert any positive integer to a Magnitude.
 */
template<std::integral auto N>
  requires requires { N > 0; }
constexpr Magnitude auto as_magnitude();

/**
 * @brief  Make a Magnitude that is a rational number.
 *
 * This will be the main way end users create Magnitudes.  They should rarely (if ever) create a magnitude<...> by
 * manually adding base powers.
 */
template<std::intmax_t N, std::intmax_t D = 1>
constexpr auto make_ratio() { return as_magnitude<N>() / as_magnitude<D>(); }

/**
 * @brief  A base to represent pi.
 */
struct pi_base {
  static constexpr long double value = std::numbers::pi_v<long double>;
};

/**
 * @brief  A simple way to create a Magnitude representing a rational power of pi.
 */
template<ratio Power>
constexpr auto pi_to_the() { return magnitude<base_power<pi_base>{Power}>{}; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation details below.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace detail
{

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

// Helpers to perform prime factorization at compile time.
template<std::intmax_t N>
  requires requires { N > 0; }
struct prime_factorization {
  static constexpr int first_base = find_first_factor(N);
  static constexpr std::intmax_t first_power = multiplicity(first_base, N);
  static constexpr std::intmax_t remainder = remove_power(first_base, first_power, N);

  static constexpr auto value = magnitude<base_power{first_base, first_power}>{}
      * prime_factorization<remainder>::value;
};

template<std::intmax_t N>
static constexpr auto prime_factorization_v = prime_factorization<N>::value;

// Specialization for the prime factorization of 1 (base case).
template<>
struct prime_factorization<1> { static constexpr magnitude<> value{}; };

// A way to check whether a number is prime at compile time.
constexpr bool is_prime(std::intmax_t n) { return (n > 1) && (find_first_factor(n) == n); }

constexpr bool is_valid_base_power(const BasePower auto &bp) {
  if (bp.power == 0) { return false; }

  if constexpr (std::is_same_v<decltype(bp.get_base()), int>) { return is_prime(bp.get_base()); }
  else { return bp.get_base() > 0; }
}

// A function object to apply a predicate to all consecutive pairs of values in a sequence.
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

// Deduction guide: permit constructions such as `pairwise_all{std::less{}}`.
template<typename T>
pairwise_all(T) -> pairwise_all<T>;

// Check whether a sequence of (possibly heterogeneously typed) values are strictly increasing.
template<typename... Ts>
constexpr bool strictly_increasing(Ts&&... ts) {
  return pairwise_all{std::less{}}(std::forward<Ts>(ts)...);
}

} // namespace detail

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Magnitude equality implementation.

template<BasePower auto... LeftBPs, BasePower auto... RightBPs>
constexpr bool operator==(magnitude<LeftBPs...>, magnitude<RightBPs...>) {
  if constexpr (sizeof...(LeftBPs) == sizeof...(RightBPs)) { return ((LeftBPs == RightBPs) && ...); }
  else { return false; }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Magnitude inverse implementation.

template<BasePower auto... BPs>
constexpr auto inverse(magnitude<BPs...>) { return magnitude<inverse(BPs)...>{}; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Magnitude product implementation.

// Base cases, for when either (or both) inputs are the identity.
constexpr auto operator*(magnitude<>, magnitude<>) { return magnitude<>{}; }
constexpr auto operator*(magnitude<>, Magnitude auto m) { return m; }
constexpr auto operator*(Magnitude auto m, magnitude<>) { return m; }

// Recursive case for the product of any two non-identity Magnitudes.
template<BasePower auto H1, BasePower auto... T1, BasePower auto H2, BasePower auto... T2>
constexpr auto operator*(magnitude<H1, T1...>, magnitude<H2, T2...>) {
  // Shortcut for the "pure prepend" case, which makes it easier to implement some of the other cases.
  if constexpr ((sizeof...(T1) == 0) && H1.get_base() < H2.get_base()) { return magnitude<H1, H2, T2...>{}; }

  // "Same leading base" case.
  if constexpr (H1.get_base() == H2.get_base()) {
    constexpr auto partial_product = magnitude<T1...>{} * magnitude<T2...>{};

    // Make a new base_power with the common base of H1 and H2, whose power is their powers' sum.
    constexpr auto new_head = [&](auto head) {
      head.power = H1.power + H2.power;
      return head;
    }(H1);

    if constexpr (new_head.power == 0) {
      return partial_product;
    } else {
      return magnitude<new_head>{} * partial_product;
    }
  }
  // Case for when H1 has the smaller base.
  else if constexpr(H1.get_base() < H2.get_base()){
    return magnitude<H1>{} * (magnitude<T1...>{} * magnitude<H2, T2...>{});
  }
  // Case for when H2 has the smaller base.
  else {
    return magnitude<H2>{} * (magnitude<H1, T1...>{} * magnitude<T2...>{});
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Magnitude quotient implementation.

constexpr auto operator/(Magnitude auto l, Magnitude auto r) { return l * inverse(r); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// `as_magnitude()` implementation.

template<std::integral auto N>
  requires requires { N > 0; }
constexpr Magnitude auto as_magnitude() { return detail::prime_factorization_v<N>; }

} // namespace units::mag
