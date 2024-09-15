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

// IWYU pragma: private, include <mp-units/framework.h>
#include <mp-units/bits/hacks.h>
#include <mp-units/bits/math_concepts.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/bits/ratio.h>
#include <mp-units/bits/text_tools.h>
#include <mp-units/ext/prime.h>
#include <mp-units/ext/type_name.h>
#include <mp-units/ext/type_traits.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/expression_template.h>
#include <mp-units/framework/symbol_text.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <numbers>
#include <optional>
#endif
#endif

namespace mp_units {

namespace detail {

// Higher numbers use fewer trial divisions, at the price of more storage space.
using factorizer = wheel_factorizer<4>;

}  // namespace detail

namespace detail {

template<typename T>
constexpr bool is_magnitude = false;

template<typename T>
constexpr bool is_specialization_of_magnitude = false;

}  // namespace detail

/**
 * @brief  Concept to detect whether T is a valid Magnitude.
 */
MP_UNITS_EXPORT template<typename T>
concept Magnitude = detail::is_magnitude<T>;

/**
 * @brief  A type to represent a standalone constant value.
 */
// template<auto V>
// struct constant {
//   static constexpr auto value = V;
// };

// // is_derived_from_specialization_of_constant
// namespace detail {

// template<auto V>
// void to_base_specialization_of_constant(const volatile constant<V>*);

// template<typename T>
// constexpr bool is_derived_from_specialization_of_constant =
//   requires(T * t) { to_base_specialization_of_constant(t); };

// }  // namespace detail


// template<typename T>
// concept Constant = detail::is_derived_from_specialization_of_constant<T>;

// struct pi_v : constant<std::numbers::pi_v<long double>> {};

/**
 * @brief  Any type which can be used as a basis vector in a PowerV.
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

/**
 * @brief  A basis vector in our magnitude representation, raised to some rational power.
 *
 * The public API is that there is a `power` member variable (of type `ratio`), and a `get_base_value()` member function
 * (of type either `std::intmax_t` or `long double`, as appropriate), for any specialization.
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

namespace detail {

template<typename T>
constexpr bool is_named_magnitude = Magnitude<T> && !detail::is_specialization_of_magnitude<T>;

}

#if MP_UNITS_COMP_CLANG

template<typename T>
struct mag_value {
  T value;
};

template<typename T>
mag_value(T) -> mag_value<T>;

template<typename T>
concept PowerVBase =
  one_of<T, int, std::intmax_t> || is_specialization_of<T, mag_value> || detail::is_named_magnitude<T>;

#else

template<typename T>
concept PowerVBase = one_of<T, int, std::intmax_t, long double> || detail::is_named_magnitude<T>;

#endif

// TODO Unify with `power` if UTPs (P1985) are accepted by the Committee
template<PowerVBase auto V, int Num, int... Den>
  requires(detail::valid_ratio<Num, Den...> && !detail::ratio_one<Num, Den...>)
struct power_v {
  static constexpr auto base = V;
  static constexpr detail::ratio exponent{Num, Den...};
};

namespace detail {

template<typename T>
constexpr bool is_specialization_of_power_v = false;

template<auto V, int... Ints>
constexpr bool is_specialization_of_power_v<power_v<V, Ints...>> = true;

}  // namespace detail


template<typename T>
concept MagnitudeSpec = PowerVBase<T> || detail::is_specialization_of_power_v<T>;

template<MagnitudeSpec auto... Ms>
struct magnitude;

namespace detail {

template<MagnitudeSpec Element>
[[nodiscard]] consteval auto get_base(Element element)
{
  if constexpr (detail::is_specialization_of_power_v<Element>)
    return Element::base;
  else
    return element;
}

template<MagnitudeSpec Element>
[[nodiscard]] consteval auto get_base_value(Element element)
{
  if constexpr (detail::is_specialization_of_power_v<Element>) return get_base_value(Element::base);
#if MP_UNITS_COMP_CLANG
  else if constexpr (is_specialization_of<Element, mag_value>)
    return element.value;
#endif
  else
    return element;
}

template<MagnitudeSpec Element>
[[nodiscard]] MP_UNITS_CONSTEVAL ratio get_exponent(Element)
{
  if constexpr (detail::is_specialization_of_power_v<Element>)
    return Element::exponent;
  else
    return ratio{1};
}

}  // namespace detail


/**
 * @brief  Concept to detect whether a _type_ is a valid base power.
 *
 * Note that this is somewhat incomplete.  We must also detect whether a _value_ of that type is valid for use with
 * `magnitude<...>`.  We will defer that second check to the constraints on the `magnitude` template.
 */
namespace detail {

// We do not want magnitude type to have the `l` literal after a value for a small integral number.
// For example this modifies `magnitude<3l>` to be `magnitude<3>`
template<auto V>
[[nodiscard]] consteval auto shorten_T()
{
  if constexpr (std::integral<decltype(V)>) {
    if constexpr (V <= std::numeric_limits<int>::max()) {
      return static_cast<int>(V);
    } else {
      return static_cast<std::intmax_t>(V);
    }
  } else if constexpr (std::floating_point<decltype(V)>) {
    return static_cast<long double>(V);
  } else {
    return V;
  }
}

template<PowerVBase auto V, ratio R>
[[nodiscard]] consteval auto power_v_or_T()
{
  if constexpr (R.den == 1) {
    if constexpr (R.num == 1)
      return shorten_T<V>();
    else
      return power_v<shorten_T<V>(), R.num>{};
  } else {
    return power_v<shorten_T<V>(), R.num, R.den>{};
  }
}

template<MagnitudeSpec M>
[[nodiscard]] consteval auto inverse(M)
{
  return power_v_or_T<get_base(M{}), -1 * get_exponent(M{})>();
}

// `widen_t` gives the widest arithmetic type in the same category, for intermediate computations.
template<typename T>
using widen_t = conditional<std::is_arithmetic_v<T>,
                            conditional<std::is_floating_point_v<T>, long double,
                                        conditional<std::is_signed_v<T>, std::intmax_t, std::uintmax_t>>,
                            T>;

// Raise an arbitrary arithmetic type to a positive integer power at compile time.
template<typename T>
[[nodiscard]] consteval T int_power(T base, std::integral auto exp)
{
  // As this function should only be called at compile time, the terminations herein function as
  // "parameter-compatible static_asserts", and should not result in terminations at runtime.
  if (exp < 0) {
    std::abort();  // int_power only supports positive integer powers
  }

  constexpr auto checked_multiply = [](auto a, auto b) {
    const auto result = a * b;
    MP_UNITS_DIAGNOSTIC_PUSH
    MP_UNITS_DIAGNOSTIC_IGNORE_FLOAT_EQUAL
    if (result / a != b) {
      std::abort();  // Wraparound detected
    }
    MP_UNITS_DIAGNOSTIC_POP
    return result;
  };

  constexpr auto checked_square = [checked_multiply](auto a) { return checked_multiply(a, a); };

  if (exp == 0) return T{1};
  if (exp % 2 == 1) return checked_multiply(base, int_power(base, exp - 1));
  return checked_square(int_power(base, exp / 2));
}

template<typename T>
[[nodiscard]] consteval std::optional<T> checked_int_pow(T base, std::uintmax_t exp)
{
  T result = T{1};
  while (exp > 0u) {
    if (exp % 2u == 1u) {
      if (base > std::numeric_limits<T>::max() / result) {
        return std::nullopt;
      }
      result *= base;
    }

    exp /= 2u;

    if (base > std::numeric_limits<T>::max() / base) {
      return (exp == 0u) ? std::make_optional(result) : std::nullopt;
    }
    base *= base;
  }
  return result;
}

template<typename T>
[[nodiscard]] consteval std::optional<T> root(T x, std::uintmax_t n)
{
  // The "zeroth root" would be mathematically undefined.
  if (n == 0) {
    return std::nullopt;
  }

  // The "first root" is trivial.
  if (n == 1) {
    return x;
  }

  // We only support nontrivial roots of floating point types.
  if (!std::is_floating_point<T>::value) {
    return std::nullopt;
  }

  // Handle negative numbers: only odd roots are allowed.
  if (x < 0) {
    if (n % 2 == 0) {
      return std::nullopt;
    } else {
      const auto negative_result = root(-x, n);
      if (!negative_result.has_value()) {
        return std::nullopt;
      }
      return static_cast<T>(-negative_result.value());
    }
  }

  // Handle special cases of zero and one.
  if (x == 0 || x == 1) {
    return x;
  }

  // Handle numbers bewtween 0 and 1.
  if (x < 1) {
    const auto inverse_result = root(T{1} / x, n);
    if (!inverse_result.has_value()) {
      return std::nullopt;
    }
    return static_cast<T>(T{1} / inverse_result.value());
  }

  //
  // At this point, error conditions are finished, and we can proceed with the "core" algorithm.
  //

  // Always use `long double` for intermediate computations.  We don't ever expect people to be
  // calling this at runtime, so we want maximum accuracy.
  long double xld = static_cast<long double>(x);
  long double lo = 1.0;
  long double hi = xld;

  // Do a binary search to find the closest value such that `checked_int_pow` recovers the input.
  //
  // Because we know `n > 1`, and `x > 1`, and x^n is monotonically increasing, we know that
  // `checked_int_pow(lo, n) < x < checked_int_pow(hi, n)`.  We will preserve this as an
  // invariant.
  while (lo < hi) {
    long double mid = lo + (hi - lo) / 2;

    auto result = checked_int_pow(mid, n);

    if (!result.has_value()) {
      return std::nullopt;
    }

    // Early return if we get lucky with an exact answer.
    if (result.value() == xld) {
      return static_cast<T>(mid);
    }

    // Check for stagnation.
    if (mid == lo || mid == hi) {
      break;
    }

    // Preserve the invariant that `checked_int_pow(lo, n) < x < checked_int_pow(hi, n)`.
    if (result.value() < xld) {
      lo = mid;
    } else {
      hi = mid;
    }
  }

  // Pick whichever one gets closer to the target.
  const auto lo_diff = xld - checked_int_pow(lo, n).value();
  const auto hi_diff = checked_int_pow(hi, n).value() - xld;
  return static_cast<T>(lo_diff < hi_diff ? lo : hi);
}

template<typename T>
[[nodiscard]] consteval widen_t<T> compute_base_power(MagnitudeSpec auto el)
{
  // This utility can only handle integer powers.  To compute rational powers at compile time, we'll
  // need to write a custom function.
  //
  // Note that since this function should only be called at compile time, the point of these
  // terminations is to act as "static_assert substitutes", not to actually terminate at runtime.
  const auto exp = get_exponent(el);

  if (exp.num < 0) {
    if constexpr (std::is_integral_v<T>) {
      std::abort();  // Cannot represent reciprocal as integer
    } else {
      return T{1} / compute_base_power<T>(inverse(el));
    }
  }

  const auto pow_result =
    checked_int_pow(static_cast<widen_t<T>>(get_base_value(el)), static_cast<std::uintmax_t>(exp.num));
  if (pow_result.has_value()) {
    const auto final_result =
      (exp.den > 1) ? root(pow_result.value(), static_cast<std::uintmax_t>(exp.den)) : pow_result;
    if (final_result.has_value()) {
      return final_result.value();
    } else {
      std::abort();  // Root computation failed.
    }
  } else {
    std::abort();  // Power computation failed.
  }
}

// A converter for the value member variable of magnitude (below).
//
// The input is the desired result, but in a (wider) intermediate type.  The point of this function
// is to cast to the desired type, but avoid overflow in doing so.
template<typename To, typename From>
  requires(!std::is_integral_v<To> || std::is_integral_v<From>)
[[nodiscard]] consteval To checked_static_cast(From x)
{
  // This function should only ever be called at compile time.  The purpose of these terminations is
  // to produce compiler errors, because we cannot `static_assert` on function arguments.
  if constexpr (std::is_integral_v<To>) {
    if (!std::in_range<To>(x)) {
      std::abort();  // Cannot represent magnitude in this type
    }
  }

  return static_cast<To>(x);
}

}  // namespace detail


// A variety of implementation detail helpers.
namespace detail {

// The exponent of `factor` in the prime factorization of `n`.
[[nodiscard]] consteval std::intmax_t multiplicity(std::intmax_t factor, std::intmax_t n)
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
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
[[nodiscard]] consteval std::intmax_t remove_power(std::intmax_t base, std::intmax_t pow, std::intmax_t n)
{
  while (pow-- > 0) {
    n /= base;
  }
  return n;
}

// A way to check whether a number is prime at compile time.
// [[nodiscard]] consteval bool is_prime(std::intmax_t n)
// {
//   return (n >= 0) && factorizer::is_prime(static_cast<std::size_t>(n));
// }

// template<MagnitudeSpec Element>
// [[nodiscard]] consteval bool is_valid_element(Element element)
// {
//   if (get_exponent(element) == 0) {
//     return false;
//   }
//   if constexpr (std::integral<decltype(get_base_value(element))>) {
//     // Some prime numbers are so big, that we can't check their primality without exhausting limits on constexpr
//     steps
//     // and/or iterations.  We can still _perform_ the factorization for these by using the `known_first_factor`
//     // workaround.  However, we can't _check_ that they are prime, because this workaround depends on the input being
//     // usable in a constexpr expression.  This is true for `prime_factorization` (below), where the input `N` is a
//     // template parameter, but is not true for our case, where the input `bp.get_base_value()` is a function
//     parameter. (See
//     // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1045r1.html for some background reading on this
//     // distinction.)
//     //
//     // In our case: we simply give up on excluding every possible ill-formed base power, and settle for catching the
//     // most likely and common mistakes.
//     if (const bool too_big_to_check = (get_base_value(element) > 1'000'000'000)) {
//       return true;
//     }

//     return is_prime(get_base_value(element));
//   } else {
//     return get_base_value(element) > 0;
//   }
// }

// A function object to apply a predicate to all consecutive pairs of values in a sequence.
// template<typename Predicate>
// struct pairwise_all {
//   Predicate predicate;

//   template<typename... Ts>
//   [[nodiscard]] consteval bool operator()(Ts&&... ts) const
//   {
//     // Carefully handle different sizes, avoiding unsigned integer underflow.
//     constexpr auto num_comparisons = [](auto num_elements) {
//       return (num_elements > 1) ? (num_elements - 1) : 0;
//     }(sizeof...(Ts));

//     // Compare zero or more pairs of neighbours as needed.
//     return [this]<std::size_t... Is>(std::tuple<Ts...> && t, std::index_sequence<Is...>)
//     {
//       return (predicate(std::get<Is>(t), std::get<Is + 1>(t)) && ...);
//     }
//     (std::make_tuple(std::forward<Ts>(ts)...), std::make_index_sequence<num_comparisons>());
//   }
// };

// Deduction guide: permit constructions such as `pairwise_all{std::less{}}`.
// template<typename T>
// pairwise_all(T) -> pairwise_all<T>;

// Check whether a sequence of (possibly heterogeneously typed) values are strictly increasing.
// template<typename... Ts>
//   requires(std::is_signed_v<Ts> && ...)
// [[nodiscard]] consteval bool strictly_increasing(Ts&&... ts)
// {
//   return pairwise_all{std::less{}}(std::forward<Ts>(ts)...);
// }

// template<MagnitudeSpec auto... Elements>
// constexpr bool all_elements_valid = (is_valid_element(Elements) && ...);

// template<MagnitudeSpec auto... Elements>
// constexpr bool all_elements_in_order = strictly_increasing(get_base_value(Elements)...);

// template<MagnitudeSpec auto... Elements>
// constexpr bool is_element_pack_valid = all_elements_valid<Elements...> && all_elements_in_order<Elements...>;

[[nodiscard]] consteval bool is_rational(MagnitudeSpec auto element)
{
  static_assert(!Magnitude<decltype(element)>);  // magnitudes are handles by another overload
  return std::is_integral_v<decltype(get_base(element))> && (get_exponent(element).den == 1);
}

[[nodiscard]] consteval bool is_integral(MagnitudeSpec auto element)
{
  static_assert(!Magnitude<decltype(element)>);  // magnitudes are handles by another overload
  return is_rational(element) && get_exponent(element).num > 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Magnitude product implementation.
[[nodiscard]] consteval bool less(MagnitudeSpec auto lhs, MagnitudeSpec auto rhs)
{
  using lhs_base_t = decltype(get_base_value(lhs));
  using rhs_base_t = decltype(get_base_value(rhs));

  if constexpr (is_named_magnitude<lhs_base_t> && is_named_magnitude<rhs_base_t>)
    return detail::type_name<lhs_base_t>() < detail::type_name<rhs_base_t>();
  else if constexpr (!is_named_magnitude<lhs_base_t> && !is_named_magnitude<rhs_base_t>)
    return get_base_value(lhs) < get_base_value(rhs);
  else
    return is_named_magnitude<lhs_base_t>;
}

template<auto H1, auto... T1, auto H2, auto... T2>
[[nodiscard]] consteval Magnitude auto multiply_impl(magnitude<H1, T1...>, magnitude<H2, T2...>)
{
  using namespace detail;

  if constexpr (less(H1, H2)) {
    if constexpr (sizeof...(T1) == 0) {
      // Shortcut for the "pure prepend" case, which makes it easier to implement some of the other cases.
      return magnitude<H1, H2, T2...>{};
    } else {
      return magnitude<H1>{} * (magnitude<T1...>{} * magnitude<H2, T2...>{});
    }
  } else if constexpr (less(H2, H1)) {
    return magnitude<H2>{} * (magnitude<H1, T1...>{} * magnitude<T2...>{});
  } else {
    if constexpr (is_same_v<decltype(get_base(H1)), decltype(get_base(H2))>) {
      constexpr auto partial_product = magnitude<T1...>{} * magnitude<T2...>{};
      if constexpr (get_exponent(H1) + get_exponent(H2) == 0) {
        return partial_product;
      } else {
        // Make a new power_v with the common base of H1 and H2, whose power is their powers' sum.
        constexpr auto new_head = power_v_or_T<get_base(H1), get_exponent(H1) + get_exponent(H2)>();

        if constexpr (get_exponent(new_head) == 0) {
          return partial_product;
        } else {
          return magnitude<new_head>{} * partial_product;
        }
      }
    } else if constexpr (is_named_magnitude<decltype(get_base(H1))>) {
      return magnitude<H1>{} * (magnitude<T1...>{} * magnitude<H2, T2...>{});
    } else {
      return magnitude<H2>{} * (magnitude<H1, T1...>{} * magnitude<T2...>{});
    }
  }
}

}  // namespace detail


/**
 * @brief  A representation for positive real numbers which optimizes taking products and rational powers.
 *
 * Magnitudes can be treated as values.  Each type encodes exactly one value.  Users can multiply, divide, raise to
 * rational powers, and compare for equality.
 */
template<MagnitudeSpec auto... Ms>
// requires detail::is_element_pack_valid<Ms...>
struct magnitude {
  [[nodiscard]] friend consteval bool is_integral(const magnitude&)
  {
    using namespace detail;  // needed for recursive case when magnitudes are in the MagnitudeSpec
    return (is_integral(Ms) && ...);
  }

  [[nodiscard]] friend consteval bool is_rational(const magnitude&)
  {
    using namespace detail;  // needed for recursive case when magnitudes are in the MagnitudeSpec
    return (is_rational(Ms) && ...);
  }

  template<Magnitude M>
  [[nodiscard]] friend consteval Magnitude auto operator*(magnitude m1, M m2)
  {
    if constexpr (sizeof...(Ms) == 0)
      return m2;
    else if constexpr (is_same_v<M, magnitude<>>)
      return m1;
    else
      return detail::multiply_impl(m1, m2);
  }

  [[nodiscard]] friend consteval auto operator/(magnitude l, Magnitude auto r) { return l * pow<-1>(r); }

  template<Magnitude M2>
  [[nodiscard]] friend consteval bool operator==(magnitude, M2)
  {
    return std::is_same_v<magnitude, M2>;
  }

  /**
   * @brief  The value of a Magnitude in a desired type T.
   */
  template<typename T>
    requires((detail::is_integral(Ms) && ...)) || treat_as_floating_point<T>
  [[nodiscard]] friend consteval T get_value(const magnitude&)
  {
    // Force the expression to be evaluated in a constexpr context, to catch, e.g., overflow.
    constexpr T result = detail::checked_static_cast<T>((detail::compute_base_power<T>(Ms) * ... * T{1}));
    return result;
  }
};


namespace detail {

template<auto... Ms>
void to_base_specialization_of_magnitude(const volatile magnitude<Ms...>*);

template<typename T>
constexpr bool is_derived_from_specialization_of_magnitude =
  requires(T* type) { to_base_specialization_of_magnitude(type); };

template<typename T>
  requires is_derived_from_specialization_of_magnitude<T>
constexpr bool is_magnitude<T> = true;

template<auto... Ms>
constexpr bool is_specialization_of_magnitude<magnitude<Ms...>> = true;

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

/**
 * @brief  A convenient Magnitude constant for pi, which we can manipulate like a regular number.
 */
#if MP_UNITS_COMP_CLANG

inline constexpr struct mag_pi : magnitude<mag_value{std::numbers::pi_v<long double>}> {
} mag_pi;

#else

inline constexpr struct mag_pi : magnitude<std::numbers::pi_v<long double>> {
} mag_pi;

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Magnitude rational powers implementation.

template<std::intmax_t Num, std::intmax_t Den = 1, auto... Ms>
[[nodiscard]] consteval auto pow(magnitude<Ms...>)
{
  if constexpr (Num == 0) {
    return magnitude<>{};
  } else {
    return magnitude<
      detail::power_v_or_T<detail::get_base(Ms), detail::get_exponent(Ms) * detail::ratio{Num, Den}>()...>{};
  }
}

template<auto... Ms>
[[nodiscard]] consteval auto sqrt(magnitude<Ms...> m)
{
  return pow<1, 2>(m);
}

template<auto... Ms>
[[nodiscard]] consteval auto cbrt(magnitude<Ms...> m)
{
  return pow<1, 3>(m);
}

MP_UNITS_EXPORT_END

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Magnitude numerator and denominator implementation.

namespace detail {

// The largest integer which can be extracted from any magnitude with only a single basis vector.
template<auto M>
[[nodiscard]] consteval auto integer_part(magnitude<M>)
{
  constexpr auto power_num = get_exponent(M).num;
  constexpr auto power_den = get_exponent(M).den;

  if constexpr (std::is_integral_v<decltype(get_base(M))> && (power_num >= power_den)) {
    // largest integer power
    return magnitude<power_v_or_T<get_base(M), power_num / power_den>()>{};  // Note: integer division intended
  } else {
    return magnitude<>{};
  }
}

template<auto... Ms>
[[nodiscard]] consteval auto numerator(magnitude<Ms...>)
{
  return (detail::integer_part(magnitude<Ms>{}) * ... * magnitude<>{});
}

[[nodiscard]] consteval auto denominator(Magnitude auto m) { return numerator(pow<-1>(m)); }

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

template<auto M>
[[nodiscard]] consteval auto remove_positive_power(magnitude<M> m)
{
  if constexpr (get_exponent(M).num < 0) {
    return m;
  } else {
    return magnitude<>{};
  }
}

template<auto... Ms>
[[nodiscard]] consteval auto remove_positive_powers(magnitude<Ms...>)
{
  return (magnitude<>{} * ... * remove_positive_power(magnitude<Ms>{}));
}

// Base cases, for when either (or both) inputs are the identity.
[[nodiscard]] consteval auto common_magnitude(magnitude<>, magnitude<>) { return magnitude<>{}; }
[[nodiscard]] consteval auto common_magnitude(magnitude<>, Magnitude auto m)
{
  return detail::remove_positive_powers(m);
}
[[nodiscard]] consteval auto common_magnitude(Magnitude auto m, magnitude<>)
{
  return detail::remove_positive_powers(m);
}

// Recursive case for the common Magnitude of any two non-identity Magnitudes.
template<auto H1, auto... T1, auto H2, auto... T2>
[[nodiscard]] consteval auto common_magnitude(magnitude<H1, T1...>, magnitude<H2, T2...>)
{
  using detail::remove_positive_power;

  if constexpr (detail::get_base_value(H1) < detail::get_base_value(H2)) {
    // When H1 has the smaller base, prepend to result from recursion.
    return remove_positive_power(magnitude<H1>{}) * common_magnitude(magnitude<T1...>{}, magnitude<H2, T2...>{});
  } else if constexpr (detail::get_base_value(H2) < detail::get_base_value(H1)) {
    // When H2 has the smaller base, prepend to result from recursion.
    return remove_positive_power(magnitude<H2>{}) * common_magnitude(magnitude<H1, T1...>{}, magnitude<T2...>{});
  } else {
    // When the bases are equal, pick whichever has the lower power.
    constexpr auto common_tail = common_magnitude(magnitude<T1...>{}, magnitude<T2...>{});
    if constexpr (detail::get_exponent(H1) < detail::get_exponent(H2)) {
      return magnitude<H1>{} * common_tail;
    } else {
      return magnitude<H2>{} * common_tail;
    }
  }
}

template<auto... Ms>
[[nodiscard]] consteval auto common_magnitude_type_impl(magnitude<Ms...>)
{
  return (std::intmax_t{} * ... * decltype(get_base_value(Ms)){});
}

// Returns the most precise type to express the magnitude factor
template<Magnitude auto M>
using common_magnitude_type = decltype(common_magnitude_type_impl(M));

}  // namespace detail

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// `mag()` implementation.

// Sometimes we need to give the compiler a "shortcut" when factorizing large numbers (specifically, numbers whose
// _first factor_ is very large).  If we don't, we can run into limits on the number of constexpr steps or iterations.
//
// To provide the first factor for a given number, specialize this variable template.
//
// WARNING:  The program behaviour will be undefined if you provide a wrong answer, so check your math!
MP_UNITS_EXPORT template<std::intmax_t N>
constexpr std::optional<std::intmax_t> known_first_factor = std::nullopt;

namespace detail {

// Helper to perform prime factorization at compile time.
template<std::intmax_t N>
  requires gt_zero<N>
struct prime_factorization {
  [[nodiscard]] static consteval std::intmax_t get_or_compute_first_factor()
  {
    constexpr auto opt = known_first_factor<N>;
    if constexpr (opt.has_value()) {
      return opt.value();  // NOLINT(bugprone-unchecked-optional-access)
    } else {
      return static_cast<std::intmax_t>(factorizer::find_first_factor(N));
    }
  }

  static constexpr std::intmax_t first_base = get_or_compute_first_factor();
  static constexpr std::intmax_t first_power = multiplicity(first_base, N);
  static constexpr std::intmax_t remainder = remove_power(first_base, first_power, N);

  static constexpr auto value =
    magnitude<power_v_or_T<first_base, ratio{first_power}>()>{} * prime_factorization<remainder>::value;
};

// Specialization for the prime factorization of 1 (base case).
template<>
struct prime_factorization<1> {
  static constexpr magnitude<> value{};
};

template<std::intmax_t N>
constexpr auto prime_factorization_v = prime_factorization<N>::value;

}  // namespace detail

/**
 * @brief  Convert any positive integer to a Magnitude.
 *
 * This will be the main way end users create Magnitudes.  They should rarely (if ever) create a magnitude<...> by
 * manually adding base powers.
 */
MP_UNITS_EXPORT template<std::intmax_t V>
  requires detail::gt_zero<V>
constexpr Magnitude auto mag = detail::prime_factorization_v<V>;

MP_UNITS_EXPORT template<std::intmax_t N, std::intmax_t D>
  requires detail::gt_zero<N>
constexpr Magnitude auto mag_ratio = detail::prime_factorization_v<N> / detail::prime_factorization_v<D>;

/**
 * @brief  Create a Magnitude which is some rational number raised to a rational power.
 */
MP_UNITS_EXPORT template<std::intmax_t Base, std::intmax_t Pow>
  requires detail::gt_zero<Base>
constexpr Magnitude auto mag_power = pow<Pow>(mag<Base>);

namespace detail {

template<typename T, auto... Ms>
[[nodiscard]] consteval ratio get_power(T base, magnitude<Ms...>)
{
  return ((get_base_value(Ms) == base ? get_exponent(Ms) : ratio{0}) + ... + ratio{0});
}

[[nodiscard]] consteval std::intmax_t integer_part(ratio r) { return r.num / r.den; }

[[nodiscard]] consteval std::intmax_t extract_power_of_10(Magnitude auto m)
{
  const auto power_of_2 = get_power(2, m);
  const auto power_of_5 = get_power(5, m);

  if ((power_of_2 * power_of_5).num <= 0) return 0;

  return integer_part((detail::abs(power_of_2) < detail::abs(power_of_5)) ? power_of_2 : power_of_5);
}

inline constexpr symbol_text base_multiplier(u8"× 10", "x 10");

template<Magnitude auto M>
[[nodiscard]] consteval auto magnitude_text()
{
  constexpr auto exp10 = extract_power_of_10(M);

  constexpr Magnitude auto base = M / mag_power<10, exp10>;
  constexpr Magnitude auto num = numerator(base);
  constexpr Magnitude auto den = denominator(base);
  // TODO address the below
  static_assert(base == num / den, "Printing rational powers, or irrational bases, not yet supported");

  constexpr auto num_value = get_value<std::intmax_t>(num);
  constexpr auto den_value = get_value<std::intmax_t>(den);

  if constexpr (num_value == 1 && den_value == 1 && exp10 != 0) {
    return base_multiplier + superscript<exp10>();
  } else if constexpr (num_value != 1 || den_value != 1 || exp10 != 0) {
    auto txt = symbol_text("[") + regular<num_value>();
    if constexpr (den_value == 1) {
      if constexpr (exp10 == 0) {
        return txt + symbol_text("]");
      } else {
        return txt + symbol_text(" ") + base_multiplier + superscript<exp10>() + symbol_text("]");
      }
    } else {
      if constexpr (exp10 == 0) {
        return txt + symbol_text("/") + regular<den_value>() + symbol_text("]");
      } else {
        return txt + symbol_text("/") + regular<den_value>() + symbol_text(" ") + base_multiplier +
               superscript<exp10>() + symbol_text("]");
      }
    }
  } else {
    return symbol_text("");
  }
}

}  // namespace detail
}  // namespace mp_units
