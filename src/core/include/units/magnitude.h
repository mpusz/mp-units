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
#include <units/bits/prime.h>
#include <units/ratio.h>
#include <concepts>
#include <cstdint>
#include <exception>
#include <numbers>
#include <optional>

namespace units {
namespace detail {
// Higher numbers use fewer trial divisions, at the price of more storage space.
using factorizer = wheel_factorizer<4>;
}  // namespace detail

/**
 * @brief  Any type which can be used as a basis vector in a BasePower.
 *
 * We have two categories.
 *
 * The first is just an `std::intmax_t`.  This is for prime number bases.  These can always be used directly as NTTPs.
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
concept BaseRep =
  std::is_same_v<T, std::intmax_t> || std::is_same_v<std::remove_cvref_t<decltype(T::value)>, long double>;

/**
 * @brief  A basis vector in our magnitude representation, raised to some rational power.
 *
 * The public API is that there is a `power` member variable (of type `ratio`), and a `get_base()` member function (of
 * type either `std::intmax_t` or `long double`, as appropriate), for any specialization.
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
 *   - Prime numbers.  (These are the only allowable values for `std::intmax_t` base.)
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
struct base_power<std::intmax_t> {
  // The value of the basis "vector".  Must be prime to be used with `magnitude` (below).
  std::intmax_t base;

  // The rational power to which the base is raised.
  ratio power{1};

  constexpr std::intmax_t get_base() const { return base; }
};

/**
 * @brief  Deduction guides for base_power: only permit deducing integral bases.
 */
template<std::integral T, std::convertible_to<ratio> U>
base_power(T, U) -> base_power<std::intmax_t>;
template<std::integral T>
base_power(T) -> base_power<std::intmax_t>;

// Implementation for BasePower concept (below).
namespace detail {
template<typename T>
inline constexpr bool is_base_power = false;
template<BaseRep T>
inline constexpr bool is_base_power<base_power<T>> = true;
}  // namespace detail

/**
 * @brief  Concept to detect whether a _type_ is a valid base power.
 *
 * Note that this is somewhat incomplete.  We must also detect whether a _value_ of that type is valid for use with
 * `magnitude<...>`.  We will defer that second check to the constraints on the `magnitude` template.
 */
template<typename T>
concept BasePower = detail::is_base_power<T>;

namespace detail {

constexpr auto inverse(BasePower auto bp)
{
  bp.power.num *= -1;
  return bp;
}

// `widen_t` gives the widest arithmetic type in the same category, for intermediate computations.
template<typename T>
using widen_t =
  std::conditional_t<std::is_arithmetic_v<T>,
                     std::conditional_t<std::is_floating_point_v<T>, long double,
                                        std::conditional_t<std::is_signed_v<T>, std::intmax_t, std::uintmax_t>>,
                     T>;

// Raise an arbitrary arithmetic type to a positive integer power at compile time.
template<typename T>
constexpr T int_power(T base, std::integral auto exp)
{
  // As this function should only be called at compile time, the terminations herein function as
  // "parameter-compatible static_asserts", and should not result in terminations at runtime.
  if (exp < 0) {
    std::terminate();  // int_power only supports positive integer powers
  }

  constexpr auto checked_multiply = [](auto a, auto b) {
    const auto result = a * b;
    UNITS_DIAGNOSTIC_PUSH
    UNITS_DIAGNOSTIC_IGNORE_FLOAT_EQUAL
    if (result / a != b) {
      std::terminate();  // Wraparound detected
    }
    UNITS_DIAGNOSTIC_POP
    return result;
  };

  constexpr auto checked_square = [checked_multiply](auto a) { return checked_multiply(a, a); };

  // TODO(chogg): Unify this implementation with the one in pow.h.  That one takes its exponent as a
  // template parameter, rather than a function parameter.

  if (exp == 0) {
    return T{1};
  }

  if (exp % 2 == 1) {
    return checked_multiply(base, int_power(base, exp - 1));
  }

  return checked_square(int_power(base, exp / 2));
}


template<typename T>
constexpr widen_t<T> compute_base_power(BasePower auto bp)
{
  // This utility can only handle integer powers.  To compute rational powers at compile time, we'll
  // need to write a custom function.
  //
  // Note that since this function should only be called at compile time, the point of these
  // terminations is to act as "static_assert substitutes", not to actually terminate at runtime.
  if (bp.power.den != 1) {
    std::terminate();  // Rational powers not yet supported
  }

  if (bp.power.num < 0) {
    if constexpr (std::is_integral_v<T>) {
      std::terminate();  // Cannot represent reciprocal as integer
    } else {
      return T{1} / compute_base_power<T>(inverse(bp));
    }
  }

  auto power = bp.power.num;
  return int_power(static_cast<widen_t<T>>(bp.get_base()), power);
}

// A converter for the value member variable of magnitude (below).
//
// The input is the desired result, but in a (wider) intermediate type.  The point of this function
// is to cast to the desired type, but avoid overflow in doing so.
template<typename To, typename From>
// TODO(chogg): Migrate this to use `treat_as_floating_point`.
  requires(!std::is_integral_v<To> || std::is_integral_v<From>)
constexpr To checked_static_cast(From x)
{
  // This function should only ever be called at compile time.  The purpose of these terminations is
  // to produce compiler errors, because we cannot `static_assert` on function arguments.
  if constexpr (std::is_integral_v<To>) {
    if (!std::in_range<To>(x)) {
      std::terminate();  // Cannot represent magnitude in this type
    }
  }

  return static_cast<To>(x);
}
}  // namespace detail

/**
 * @brief  Equality detection for two base powers.
 */
template<BasePower T, BasePower U>
constexpr bool operator==(T t, U u)
{
  return std::is_same_v<T, U> && (t.get_base() == u.get_base()) && (t.power == u.power);
}

/**
 * @brief  A BasePower, raised to a rational power E.
 */
constexpr auto pow(BasePower auto bp, ratio p)
{
  bp.power = bp.power * p;
  return bp;
}

// A variety of implementation detail helpers.
namespace detail {

// The exponent of `factor` in the prime factorization of `n`.
constexpr std::intmax_t multiplicity(std::intmax_t factor, std::intmax_t n)
{
  std::intmax_t m = 0;
  while (n % factor == 0) {
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
  while (pow-- > 0) {
    n /= base;
  }
  return n;
}

// A way to check whether a number is prime at compile time.
constexpr bool is_prime(std::intmax_t n) { return (n >= 0) && factorizer::is_prime(static_cast<std::size_t>(n)); }

constexpr bool is_valid_base_power(const BasePower auto& bp)
{
  if (bp.power == 0) {
    return false;
  }

  if constexpr (std::is_same_v<decltype(bp.get_base()), std::intmax_t>) {
    // Some prime numbers are so big, that we can't check their primality without exhausting limits on constexpr steps
    // and/or iterations.  We can still _perform_ the factorization for these by using the `known_first_factor`
    // workaround.  However, we can't _check_ that they are prime, because this workaround depends on the input being
    // usable in a constexpr expression.  This is true for `prime_factorization` (below), where the input `N` is a
    // template parameter, but is not true for our case, where the input `bp.get_base()` is a function parameter.  (See
    // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1045r1.html for some background reading on this
    // distinction.)
    //
    // In our case: we simply give up on excluding every possible ill-formed base power, and settle for catching the
    // most likely and common mistakes.
    if (const bool too_big_to_check = (bp.get_base() > 1'000'000'000)) {
      return true;
    }

    return is_prime(bp.get_base());
  } else {
    return bp.get_base() > 0;
  }
}

// A function object to apply a predicate to all consecutive pairs of values in a sequence.
template<typename Predicate>
struct pairwise_all {
  Predicate predicate;

  template<typename... Ts>
  constexpr bool operator()(Ts&&... ts) const
  {
    // Carefully handle different sizes, avoiding unsigned integer underflow.
    constexpr auto num_comparisons = [](auto num_elements) {
      return (num_elements > 1) ? (num_elements - 1) : 0;
    }(sizeof...(Ts));

    // Compare zero or more pairs of neighbours as needed.
    return [this]<std::size_t... Is>(std::tuple<Ts...>&& t, std::index_sequence<Is...>) {
      return (predicate(std::get<Is>(t), std::get<Is + 1>(t)) && ...);
    }(std::make_tuple(std::forward<Ts>(ts)...), std::make_index_sequence<num_comparisons>());
  }
};

// Deduction guide: permit constructions such as `pairwise_all{std::less{}}`.
template<typename T>
pairwise_all(T) -> pairwise_all<T>;

// Check whether a sequence of (possibly heterogeneously typed) values are strictly increasing.
template<typename... Ts>
  requires(std::is_signed_v<Ts> && ...)
constexpr bool strictly_increasing(Ts&&... ts)
{
  return pairwise_all{std::less{}}(std::forward<Ts>(ts)...);
}

template<BasePower auto... BPs>
inline constexpr bool all_base_powers_valid = (is_valid_base_power(BPs) && ...);

template<BasePower auto... BPs>
inline constexpr bool all_bases_in_order = strictly_increasing(BPs.get_base()...);

template<BasePower auto... BPs>
inline constexpr bool is_base_power_pack_valid = all_base_powers_valid<BPs...> && all_bases_in_order<BPs...>;

constexpr bool is_rational(BasePower auto bp)
{
  return std::is_integral_v<decltype(bp.get_base())> && (bp.power.den == 1);
}

constexpr bool is_integral(BasePower auto bp) { return is_rational(bp) && bp.power.num > 0; }
}  // namespace detail

/**
 * @brief  A representation for positive real numbers which optimizes taking products and rational powers.
 *
 * Magnitudes can be treated as values.  Each type encodes exactly one value.  Users can multiply, divide, raise to
 * rational powers, and compare for equality.
 */
template<BasePower auto... BPs>
  requires detail::is_base_power_pack_valid<BPs...>
struct magnitude {
  // Whether this magnitude represents an integer.
  friend constexpr bool is_integral(const magnitude&) { return (detail::is_integral(BPs) && ...); }

  // Whether this magnitude represents a rational number.
  friend constexpr bool is_rational(const magnitude&) { return (detail::is_rational(BPs) && ...); }
};

// Implementation for Magnitude concept (below).
namespace detail {
template<typename T>
inline constexpr bool is_magnitude = false;
template<auto... BPs>
inline constexpr bool is_magnitude<magnitude<BPs...>> = true;
}  // namespace detail

/**
 * @brief  Concept to detect whether T is a valid Magnitude.
 */
template<typename T>
concept Magnitude = detail::is_magnitude<T>;

/**
 * @brief  The value of a Magnitude in a desired type T.
 */
template<typename T, auto... BPs>
// TODO(chogg): Migrate this to use `treat_as_floating_point`.
  requires(!std::is_integral_v<T> || is_integral(magnitude<BPs...>{}))
constexpr T get_value(const magnitude<BPs...>&)
{
  // Force the expression to be evaluated in a constexpr context, to catch, e.g., overflow.
  constexpr auto result = detail::checked_static_cast<T>((detail::compute_base_power<T>(BPs) * ... * T{1}));

  return result;
}

/**
 * @brief  A base to represent pi.
 */
struct pi_base {
  static constexpr long double value = std::numbers::pi_v<long double>;
};

/**
 * @brief  A convenient Magnitude constant for pi, which we can manipulate like a regular number.
 */
inline constexpr Magnitude auto mag_pi = magnitude<base_power<pi_base>{}>{};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Magnitude equality implementation.

template<auto... LeftBPs, auto... RightBPs>
constexpr bool operator==(magnitude<LeftBPs...>, magnitude<RightBPs...>)
{
  if constexpr (sizeof...(LeftBPs) == sizeof...(RightBPs)) {
    return ((LeftBPs == RightBPs) && ...);
  } else {
    return false;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Magnitude rational powers implementation.

template<ratio E, auto... BPs>
constexpr auto pow(magnitude<BPs...>)
{
  if constexpr (E.num == 0) {
    return magnitude<>{};
  } else {
    return magnitude<pow(BPs, E)...>{};
  }
}

template<auto... BPs>
constexpr auto sqrt(magnitude<BPs...> m)
{
  return pow<ratio{1, 2}>(m);
}

template<auto... BPs>
constexpr auto cbrt(magnitude<BPs...> m)
{
  return pow<ratio{1, 3}>(m);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Magnitude product implementation.

// Base cases, for when either (or both) inputs are the identity.
constexpr Magnitude auto operator*(magnitude<>, magnitude<>) { return magnitude<>{}; }
constexpr Magnitude auto operator*(magnitude<>, Magnitude auto m) { return m; }
constexpr Magnitude auto operator*(Magnitude auto m, magnitude<>) { return m; }

// Recursive case for the product of any two non-identity Magnitudes.
template<auto H1, auto... T1, auto H2, auto... T2>
constexpr Magnitude auto operator*(magnitude<H1, T1...>, magnitude<H2, T2...>)
{
  // Case for when H1 has the smaller base.
  if constexpr (H1.get_base() < H2.get_base()) {
    if constexpr (sizeof...(T1) == 0) {
      // Shortcut for the "pure prepend" case, which makes it easier to implement some of the other cases.
      return magnitude<H1, H2, T2...>{};
    } else {
      return magnitude<H1>{} * (magnitude<T1...>{} * magnitude<H2, T2...>{});
    }
  }

  // Case for when H2 has the smaller base.
  if constexpr (H1.get_base() > H2.get_base()) {
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
// Magnitude numerator and denominator implementation.

namespace detail {

// The largest integer which can be extracted from any magnitude with only a single basis vector.
template<auto BP>
constexpr auto integer_part(magnitude<BP>)
{
  constexpr auto power_num = BP.power.num;
  constexpr auto power_den = BP.power.den;

  if constexpr (std::is_integral_v<decltype(BP.get_base())> && (power_num >= power_den)) {
    constexpr auto largest_integer_power = [=](BasePower auto bp) {
      bp.power = (power_num / power_den);  // Note: integer division intended.
      return bp;
    }(BP);  // Note: lambda is immediately invoked.

    return magnitude<largest_integer_power>{};
  } else {
    return magnitude<>{};
  }
}

}  // namespace detail

template<auto... BPs>
constexpr auto numerator(magnitude<BPs...>)
{
  return (detail::integer_part(magnitude<BPs>{}) * ... * magnitude<>{});
}

constexpr auto denominator(Magnitude auto m) { return numerator(pow<-1>(m)); }

// Implementation of conversion to ratio goes here, because it needs `numerator()` and `denominator()`.
constexpr ratio as_ratio(Magnitude auto m)
  requires(is_rational(decltype(m){}))
{
  return ratio{
    get_value<std::intmax_t>(numerator(m)),
    get_value<std::intmax_t>(denominator(m)),
  };
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Common Magnitude.
//
// The "common Magnitude" C, of two Magnitudes M1 and M2, is the largest Magnitude such that each of its inputs is
// expressible by only positive basis powers relative to C.  That is, both (M1 / C) and (M2 / C) contain only positive
// powers in the expansion on our basis.
//
// For rational Magnitudes (or, more precisely, Magnitudes that are rational _relative to each other_), this reduces to
// the familiar convention from the std::chrono library: it is the largest Magnitude C such that each input Magnitude is
// an _integer multiple_ of C.  The connection can be seen by considering the definition in the above paragraph, and
// recognizing that both the bases and the powers are all integers for rational Magnitudes.
//
// For relatively _irrational_ Magnitudes (whether from irrational bases, or fractional powers of integer bases), the
// notion of a "common type" becomes less important, because there is no way to preserve pure integer multiplication.
// When we go to retrieve our value, we'll be stuck with a floating point approximation no matter what choice we make.
// Thus, we make the _simplest_ choice which reproduces the correct convention in the rational case: namely, taking the
// minimum power for each base (where absent bases implicitly have a power of 0).

namespace detail {
template<auto BP>
constexpr auto remove_positive_power(magnitude<BP> m)
{
  if constexpr (BP.power.num < 0) {
    return m;
  } else {
    return magnitude<>{};
  }
}

template<auto... BPs>
constexpr auto remove_positive_powers(magnitude<BPs...>)
{
  return (magnitude<>{} * ... * remove_positive_power(magnitude<BPs>{}));
}
}  // namespace detail

// Base cases, for when either (or both) inputs are the identity.
constexpr auto common_magnitude(magnitude<>, magnitude<>) { return magnitude<>{}; }
constexpr auto common_magnitude(magnitude<>, Magnitude auto m) { return detail::remove_positive_powers(m); }
constexpr auto common_magnitude(Magnitude auto m, magnitude<>) { return detail::remove_positive_powers(m); }

// Recursive case for the common Magnitude of any two non-identity Magnitudes.
template<auto H1, auto... T1, auto H2, auto... T2>
constexpr auto common_magnitude(magnitude<H1, T1...>, magnitude<H2, T2...>)
{
  using detail::remove_positive_power;

  if constexpr (H1.get_base() < H2.get_base()) {
    // When H1 has the smaller base, prepend to result from recursion.
    return remove_positive_power(magnitude<H1>{}) * common_magnitude(magnitude<T1...>{}, magnitude<H2, T2...>{});
  } else if constexpr (H2.get_base() < H1.get_base()) {
    // When H2 has the smaller base, prepend to result from recursion.
    return remove_positive_power(magnitude<H2>{}) * common_magnitude(magnitude<H1, T1...>{}, magnitude<T2...>{});
  } else {
    // When the bases are equal, pick whichever has the lower power.
    constexpr auto common_tail = common_magnitude(magnitude<T1...>{}, magnitude<T2...>{});
    if constexpr (H1.power < H2.power) {
      return magnitude<H1>{} * common_tail;
    } else {
      return magnitude<H2>{} * common_tail;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// `mag()` implementation.

// Sometimes we need to give the compiler a "shortcut" when factorizing large numbers (specifically, numbers whose
// _first factor_ is very large).  If we don't, we can run into limits on the number of constexpr steps or iterations.
//
// To provide the first factor for a given number, specialize this variable template.
//
// WARNING:  The program behaviour will be undefined if you provide a wrong answer, so check your math!
template<std::intmax_t N>
inline constexpr std::optional<std::intmax_t> known_first_factor = std::nullopt;

namespace detail {
// Helper to perform prime factorization at compile time.
template<std::intmax_t N>
  requires(N > 0)
struct prime_factorization {
  static constexpr std::intmax_t get_or_compute_first_factor()
  {
    if constexpr (known_first_factor<N>.has_value()) {
      return known_first_factor<N>.value();
    } else {
      return static_cast<std::intmax_t>(factorizer::find_first_factor(N));
    }
  }

  static constexpr std::intmax_t first_base = get_or_compute_first_factor();
  static constexpr std::intmax_t first_power = multiplicity(first_base, N);
  static constexpr std::intmax_t remainder = remove_power(first_base, first_power, N);

  static constexpr auto value =
    magnitude<base_power{first_base, first_power}>{} * prime_factorization<remainder>::value;
};

// Specialization for the prime factorization of 1 (base case).
template<>
struct prime_factorization<1> {
  static constexpr magnitude<> value{};
};

template<std::intmax_t N>
inline constexpr auto prime_factorization_v = prime_factorization<N>::value;
}  // namespace detail

/**
 * @brief  Convert any positive integer to a Magnitude.
 *
 * This will be the main way end users create Magnitudes.  They should rarely (if ever) create a magnitude<...> by
 * manually adding base powers.
 */
template<ratio R>
  requires(R.num > 0)
constexpr Magnitude auto mag()
{
  return detail::prime_factorization_v<R.num> / detail::prime_factorization_v<R.den>;
}

/**
 * @brief  Create a Magnitude which is some rational number raised to a rational power.
 */
template<ratio Base, ratio Pow>
  requires(Base.num > 0)
constexpr Magnitude auto mag_power()
{
  return pow<Pow>(mag<Base>());
}

namespace detail {
template<typename T, BasePower auto... BPs>
constexpr ratio get_power(T base, magnitude<BPs...>)
{
  return ((BPs.get_base() == base ? BPs.power : ratio{0}) + ... + ratio{0});
}

constexpr std::intmax_t integer_part(ratio r) { return r.num / r.den; }

constexpr std::intmax_t extract_power_of_10(Magnitude auto m)
{
  const auto power_of_2 = get_power(2, m);
  const auto power_of_5 = get_power(5, m);

  if ((power_of_2 * power_of_5).num <= 0) {
    return 0;
  }

  return integer_part((detail::abs(power_of_2) < detail::abs(power_of_5)) ? power_of_2 : power_of_5);
}
}  // namespace detail

}  // namespace units
