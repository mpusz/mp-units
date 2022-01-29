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
#include <stdexcept>

namespace units {

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
static constexpr bool is_base_power = false;
template<BaseRep T>
static constexpr bool is_base_power<base_power<T>> = true;
} // namespace detail

/**
 * @brief  Concept to detect whether a _type_ is a valid base power.
 *
 * Note that this is somewhat incomplete.  We must also detect whether a _value_ of that type is valid for use with
 * `magnitude<...>`.  We will defer that second check to the constraints on the `magnitude` template.
 */
template<typename T>
concept BasePower = detail::is_base_power<T>;

namespace detail
{
constexpr auto inverse(BasePower auto bp) {
  bp.power.num *= -1;
  return bp;
}

// `widen_t` gives the widest arithmetic type in the same category, for intermediate computations.
template<typename T> requires std::is_arithmetic_v<T>
using widen_t = std::conditional_t<
  std::is_floating_point_v<T>,
  long double,
  std::conditional_t<std::is_signed_v<T>, std::intmax_t, std::uintmax_t>>;

// Raise an arbitrary arithmetic type to a positive integer power at compile time.
template<typename T> requires std::is_arithmetic_v<T>
constexpr T int_power(T base, std::integral auto exp){
  // As this function should only be called at compile time, the exceptions herein function as
  // "parameter-compatible static_asserts", and should not result in exceptions at runtime.
  if (exp < 0) { throw std::invalid_argument{"int_power only supports positive integer powers"}; }

  // TODO(chogg): Unify this implementation with the one in pow.h.  That one takes its exponent as a
  // template parameter, rather than a function parameter.

  if (exp == 0) {
    return 1;
  }

  if (exp % 2 == 1) {
    return base * int_power(base, exp - 1);
  }

  const auto square_root = int_power(base, exp / 2);
  const auto result = square_root * square_root;

  if constexpr(std::is_unsigned_v<T>) {
    if (result / square_root != square_root) { throw std::overflow_error{"Unsigned wraparound"}; }
  }

  return result;
}


template<typename T> requires std::is_arithmetic_v<T>
constexpr widen_t<T> compute_base_power(BasePower auto bp)
{
  // This utility can only handle integer powers.  To compute rational powers at compile time, we'll
  // need to write a custom function.
  //
  // Note that since this function should only be called at compile time, the point of these
  // exceptions is to act as "static_assert substitutes", not to throw actual exceptions at runtime.
  if (bp.power.den != 1) { throw std::invalid_argument{"Rational powers not yet supported"}; }
  if (bp.power.exp < 0) { throw std::invalid_argument{"Unsupported exp value"}; }

  if (bp.power.num < 0) {
    if constexpr (std::is_integral_v<T>) {
      throw std::invalid_argument{"Cannot represent reciprocal as integer"};
    } else {
      return 1 / compute_base_power<T>(inverse(bp));
    }
  }

  auto power = bp.power.num * int_power(10, bp.power.exp);
  return int_power(static_cast<widen_t<T>>(bp.get_base()), power);
}

// A converter for the value member variable of magnitude (below).
//
// The input is the desired result, but in a (wider) intermediate type.  The point of this function
// is to cast to the desired type, but avoid overflow in doing so.
template<typename To, typename From>
  requires std::is_arithmetic_v<To>
    && std::is_arithmetic_v<From>
    && (std::is_integral_v<To> == std::is_integral_v<From>)
constexpr To checked_static_cast(From x) {
  // This function should only ever be called at compile time.  The purpose of these exceptions is
  // to produce compiler errors, because we cannot `static_assert` on function arguments.
  if constexpr (std::is_integral_v<To>) {
    if (!std::in_range<To>(x)) {
      throw std::invalid_argument{"Cannot represent magnitude in this type"};
    }
  } else {
    if (x < std::numeric_limits<To>::min() || x > std::numeric_limits<To>::max()) {
      throw std::invalid_argument{"Cannot represent magnitude in this type"};
    }
  }

  return static_cast<To>(x);
}
} // namespace detail

/**
 * @brief  Equality detection for two base powers.
 */
template<BasePower T, BasePower U>
constexpr bool operator==(T t, U u) {
  return std::is_same_v<T, U> && (t.get_base() == u.get_base()) && (t.power == u.power);
}

/**
 * @brief  A BasePower, raised to a rational power E.
 */
constexpr auto pow(BasePower auto bp, ratio p) {
  bp.power = bp.power * p;
  return bp;
}

// A variety of implementation detail helpers.
namespace detail {

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
  requires ((std::is_signed_v<Ts> && ...))
constexpr bool strictly_increasing(Ts&&... ts) {
  return pairwise_all{std::less{}}(std::forward<Ts>(ts)...);
}

template<BasePower auto... BPs>
static constexpr bool all_base_powers_valid = (is_valid_base_power(BPs) && ...);

template<BasePower auto... BPs>
static constexpr bool all_bases_in_order = strictly_increasing(BPs.get_base()...);

template<BasePower auto... BPs>
static constexpr bool is_base_power_pack_valid = all_base_powers_valid<BPs...> && all_bases_in_order<BPs...>;

constexpr bool is_rational(BasePower auto bp) {
  return std::is_integral_v<decltype(bp.get_base())> && (bp.power.den == 1) && (bp.power.exp >= 0);
}

constexpr bool is_integral(BasePower auto bp) {
  return is_rational(bp) && bp.power.num > 0;
}
} // namespace detail

/**
 * @brief  A representation for positive real numbers which optimizes taking products and rational powers.
 *
 * Magnitudes can be treated as values.  Each type encodes exactly one value.  Users can multiply, divide, raise to
 * rational powers, and compare for equality.
 */
template<BasePower auto... BPs>
  requires (detail::is_base_power_pack_valid<BPs...>)
struct magnitude {
  // Whether this magnitude represents an integer.
  static constexpr bool is_magnitude_integral = (detail::is_integral(BPs) && ...);
  friend constexpr bool is_integral(magnitude) { return is_magnitude_integral; }

  // Whether this magnitude represents a rational number.
  static constexpr bool is_magnitude_rational = (detail::is_rational(BPs) && ...);
  friend constexpr bool is_rational(magnitude) { return is_magnitude_rational; }

  // The value of this magnitude, expressed in a given type.
  template<typename T>
    requires (
        std::is_floating_point_v<T>
        || (std::is_integral_v<T> && (detail::is_integral(BPs) && ...)))
  static constexpr T value = detail::checked_static_cast<T>(
      (detail::compute_base_power<T>(BPs) * ...));
};

// Implementation for Magnitude concept (below).
namespace detail {
template<typename T>
static constexpr bool is_magnitude = false;
template<BasePower auto... BPs>
static constexpr bool is_magnitude<magnitude<BPs...>> = true;
} // namespace detail

/**
 * @brief  Concept to detect whether T is a valid Magnitude.
 */
template<typename T>
concept Magnitude = detail::is_magnitude<T>;

/**
 * @brief  Free-function access to the value of a Magnitude in a desired type.
 *
 * Can avoid the need for an unsightly `.template` keyword.
 */
template<typename T>
T get_value(Magnitude auto m) { return decltype(m)::template value<T>; }

/**
 * @brief  A base to represent pi.
 */
struct pi_base {
  static constexpr long double value = std::numbers::pi_v<long double>;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Magnitude equality implementation.

template<BasePower auto... LeftBPs, BasePower auto... RightBPs>
constexpr bool operator==(magnitude<LeftBPs...>, magnitude<RightBPs...>) {
  if constexpr (sizeof...(LeftBPs) == sizeof...(RightBPs)) { return ((LeftBPs == RightBPs) && ...); }
  else { return false; }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Magnitude rational powers implementation.

template<ratio E, BasePower auto... BPs>
constexpr auto pow(magnitude<BPs...>) {
  if constexpr (E == 0) { return magnitude<>{}; }
  else { return magnitude<pow(BPs, E)...>{}; }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Magnitude product implementation.

// Base cases, for when either (or both) inputs are the identity.
constexpr auto operator*(magnitude<>, magnitude<>) { return magnitude<>{}; }
constexpr auto operator*(magnitude<>, Magnitude auto m) { return m; }
constexpr auto operator*(Magnitude auto m, magnitude<>) { return m; }

// Recursive case for the product of any two non-identity Magnitudes.
template<BasePower auto H1, BasePower auto... T1, BasePower auto H2, BasePower auto... T2>
constexpr auto operator*(magnitude<H1, T1...>, magnitude<H2, T2...>) {
  // Case for when H1 has the smaller base.
  if constexpr(H1.get_base() < H2.get_base()){
    if constexpr (sizeof...(T1) == 0) {
      // Shortcut for the "pure prepend" case, which makes it easier to implement some of the other cases.
      return magnitude<H1, H2, T2...>{};
    } else {
      return magnitude<H1>{} * (magnitude<T1...>{} * magnitude<H2, T2...>{});
    }
  }

  // Case for when H2 has the smaller base.
  if constexpr(H1.get_base() > H2.get_base()){
    return magnitude<H2>{} * (magnitude<H1, T1...>{} * magnitude<T2...>{});
  }

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
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Magnitude quotient implementation.

constexpr auto operator/(Magnitude auto l, Magnitude auto r) { return l * pow<-1>(r); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// `as_magnitude()` implementation.

namespace detail {
// Helper to perform prime factorization at compile time.
template<std::intmax_t N>
  requires (N > 0)
struct prime_factorization {
  static constexpr int first_base = find_first_factor(N);
  static constexpr std::intmax_t first_power = multiplicity(first_base, N);
  static constexpr std::intmax_t remainder = remove_power(first_base, first_power, N);

  static constexpr auto value = magnitude<base_power{first_base, first_power}>{}
      * prime_factorization<remainder>::value;
};

// Specialization for the prime factorization of 1 (base case).
template<>
struct prime_factorization<1> { static constexpr magnitude<> value{}; };

template<std::intmax_t N>
static constexpr auto prime_factorization_v = prime_factorization<N>::value;
} // namespace detail

template<ratio R>
  requires (R.num > 0)
constexpr Magnitude auto as_magnitude() {
  return pow<R.exp>(detail::prime_factorization_v<10>)
    * detail::prime_factorization_v<R.num>
    / detail::prime_factorization_v<R.den>;
}

} // namespace units
