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
#include <mp-units/bits/constexpr_math.h>
#include <mp-units/bits/math_concepts.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/bits/ratio.h>
#include <mp-units/bits/text_tools.h>
#include <mp-units/ext/prime.h>
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
#include <limits>
#include <numbers>
#include <optional>
#endif
#endif

namespace mp_units {

namespace detail {

// Higher numbers use fewer trial divisions, at the price of more storage space.
using factorizer = wheel_factorizer<4>;

}  // namespace detail

MP_UNITS_EXPORT struct mag_constant {};

template<typename T>
concept MagConstant = std::derived_from<T, mag_constant> && std::is_empty_v<T> && requires {
  { +T::value } -> std::same_as<long double>;
};

/**
 * @brief  Any type which can be used as a basis vector in a PowerV.
 *
 * We have two categories.
 *
 * The first is just an integral type (either `int` or `std::intmax_t`). This is for prime number bases.
 * These can always be used directly as NTTPs.
 *
 * The second category is a _custom tag type_, which inherits from `mag_constant` and has a static member variable
 * `value` of type `long double` that holds its value. We choose `long double` to get the greatest degree of precision;
 * users who need a different type can convert from this at compile time.  This category is for any irrational base
 * we admit into our representation (on which, more details below).
 */
template<typename T>
concept PowerVBase = one_of<T, int, std::intmax_t> || MagConstant<T>;

// TODO Unify with `power` if UTPs (P1985) are accepted by the Committee
template<PowerVBase auto V, int Num, int... Den>
  requires(detail::valid_ratio<Num, Den...> && !detail::ratio_one<Num, Den...>)
struct power_v {
  static constexpr auto base = V;
  static constexpr detail::ratio exponent{Num, Den...};
};

template<typename T>
concept MagnitudeSpec = PowerVBase<T> || is_specialization_of_v<T, power_v>;

// TODO refactor to typenames?
template<MagnitudeSpec auto... Ms>
struct magnitude;

/**
 * @brief  Concept to detect whether T is a valid Magnitude.
 */
MP_UNITS_EXPORT template<typename T>
concept Magnitude = is_specialization_of_v<T, magnitude>;

namespace detail {

template<MagnitudeSpec Element>
[[nodiscard]] consteval auto get_base(Element element)
{
  if constexpr (is_specialization_of_v<Element, power_v>)
    return Element::base;
  else
    return element;
}

template<MagnitudeSpec Element>
[[nodiscard]] consteval auto get_base_value(Element element)
{
  if constexpr (is_specialization_of_v<Element, power_v>)
    return get_base_value(Element::base);
  else if constexpr (MagConstant<Element>)
    return element.value;
  else
    return element;
}

template<MagnitudeSpec Element>
[[nodiscard]] MP_UNITS_CONSTEVAL ratio get_exponent(Element)
{
  if constexpr (is_specialization_of_v<Element, power_v>)
    return Element::exponent;
  else
    return ratio{1};
}

// We do not want magnitude type to have the `l` literal after a value for a small integral number.
// For example this modifies `magnitude<3l>` to be `magnitude<3>`
template<auto V>
[[nodiscard]] consteval auto shorten_T()
{
  if constexpr (std::integral<decltype(V)>) {
    if constexpr (V <= std::numeric_limits<int>::max())
      return static_cast<int>(V);
    else
      return static_cast<std::intmax_t>(V);
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
  return get_base_value(lhs) < get_base_value(rhs);
}

// The largest integer which can be extracted from any magnitude with only a single basis vector.
template<auto M>
[[nodiscard]] consteval auto integer_part(magnitude<M>);
[[nodiscard]] consteval std::intmax_t integer_part(ratio r) { return r.num / r.den; }

template<auto M>
[[nodiscard]] consteval auto remove_positive_power(magnitude<M> m);

inline constexpr symbol_text base_multiplier(u8"× 10", "x 10");

template<std::intmax_t Base, std::intmax_t Pow>
  requires detail::gt_zero<Base>
[[nodiscard]] consteval Magnitude auto mag_power_lazy();

template<typename T>
struct magnitude_base {};

template<auto H, auto... T>
struct magnitude_base<magnitude<H, T...>> {
  template<auto H2, auto... T2>
  [[nodiscard]] friend consteval Magnitude auto _multiply_impl(magnitude<H, T...>, magnitude<H2, T2...>)
  {
    if constexpr (less(H, H2)) {
      if constexpr (sizeof...(T) == 0) {
        // Shortcut for the "pure prepend" case, which makes it easier to implement some of the other cases.
        return magnitude<H, H2, T2...>{};
      } else {
        return magnitude<H>{} * (magnitude<T...>{} * magnitude<H2, T2...>{});
      }
    } else if constexpr (less(H2, H)) {
      return magnitude<H2>{} * (magnitude<H, T...>{} * magnitude<T2...>{});
    } else {
      if constexpr (is_same_v<decltype(get_base(H)), decltype(get_base(H2))>) {
        constexpr auto partial_product = magnitude<T...>{} * magnitude<T2...>{};
        if constexpr (get_exponent(H) + get_exponent(H2) == 0) {
          return partial_product;
        } else {
          // Make a new power_v with the common base of H and H2, whose power is their powers' sum.
          constexpr auto new_head = power_v_or_T<get_base(H), get_exponent(H) + get_exponent(H2)>();

          if constexpr (get_exponent(new_head) == 0) {
            return partial_product;
          } else {
            return magnitude<new_head>{} * partial_product;
          }
        }
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Common Magnitude.
  //
  // The "common Magnitude" C, of two Magnitudes M1 and M2, is the largest Magnitude such that each of its inputs is
  // expressible by only positive basis powers relative to C.  That is, both (M1 / C) and (M2 / C) contain only positive
  // powers in the expansion on our basis.
  //
  // For rational Magnitudes (or, more precisely, Magnitudes that are rational _relative to each other_), this reduces
  // to the familiar convention from the std::chrono library: it is the largest Magnitude C such that each input
  // Magnitude is an _integer multiple_ of C.  The connection can be seen by considering the definition in the above
  // paragraph, and recognizing that both the bases and the powers are all integers for rational Magnitudes.
  //
  // For relatively _irrational_ Magnitudes (whether from irrational bases, or fractional powers of integer bases), the
  // notion of a "common type" becomes less important, because there is no way to preserve pure integer multiplication.
  // When we go to retrieve our value, we'll be stuck with a floating point approximation no matter what choice we make.
  // Thus, we make the _simplest_ choice which reproduces the correct convention in the rational case: namely, taking
  // the minimum power for each base (where absent bases implicitly have a power of 0).
  template<auto H2, auto... T2>
  [[nodiscard]] friend consteval auto _common_magnitude(magnitude<H, T...>, magnitude<H2, T2...>)
  {
    using detail::remove_positive_power;

    if constexpr (detail::get_base_value(H) < detail::get_base_value(H2)) {
      // When H1 has the smaller base, prepend to result from recursion.
      return remove_positive_power(magnitude<H>{}) * _common_magnitude(magnitude<T...>{}, magnitude<H2, T2...>{});
    } else if constexpr (detail::get_base_value(H2) < detail::get_base_value(H)) {
      // When H2 has the smaller base, prepend to result from recursion.
      return remove_positive_power(magnitude<H2>{}) * _common_magnitude(magnitude<H, T...>{}, magnitude<T2...>{});
    } else {
      // When the bases are equal, pick whichever has the lower power.
      constexpr auto common_tail = _common_magnitude(magnitude<T...>{}, magnitude<T2...>{});
      if constexpr (detail::get_exponent(H) < detail::get_exponent(H2)) {
        return magnitude<H>{} * common_tail;
      } else {
        return magnitude<H2>{} * common_tail;
      }
    }
  }
};

}  // namespace detail


/**
 * @brief  A representation for positive real numbers which optimizes taking products and rational powers.
 *
 * Magnitudes can be treated as values.  Each type encodes exactly one value.  Users can multiply, divide, raise to
 * rational powers, and compare for equality.
 */
template<MagnitudeSpec auto... Ms>
struct magnitude : detail::magnitude_base<magnitude<Ms...>> {
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
      return _multiply_impl(m1, m2);
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

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Magnitude rational powers implementation.
  template<std::intmax_t Num, std::intmax_t Den = 1>
  [[nodiscard]] friend consteval auto pow(magnitude)
  {
    if constexpr (Num == 0) {
      return magnitude<>{};
    } else {
      return magnitude<
        detail::power_v_or_T<detail::get_base(Ms), detail::get_exponent(Ms) * detail::ratio{Num, Den}>()...>{};
    }
  }

  [[nodiscard]] friend consteval auto sqrt(magnitude m) { return pow<1, 2>(m); }
  [[nodiscard]] friend consteval auto cbrt(magnitude m) { return pow<1, 3>(m); }

private:
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Magnitude numerator and denominator implementation.
  [[nodiscard]] friend consteval auto _numerator(magnitude)
  {
    return (detail::integer_part(magnitude<Ms>{}) * ... * magnitude<>{});
  }

  [[nodiscard]] friend consteval auto _denominator(magnitude) { return _numerator(pow<-1>(magnitude{})); }


  [[nodiscard]] friend consteval auto _remove_positive_powers(magnitude)
  {
    return (magnitude<>{} * ... * detail::remove_positive_power(magnitude<Ms>{}));
  }

  [[nodiscard]] friend consteval auto _common_magnitude_type_impl(magnitude)
  {
    return (std::intmax_t{} * ... * decltype(detail::get_base_value(Ms)){});
  }

  template<typename T>
  [[nodiscard]] friend consteval detail::ratio _get_power(T base, magnitude)
  {
    return ((detail::get_base_value(Ms) == base ? detail::get_exponent(Ms) : detail::ratio{0}) + ... +
            detail::ratio{0});
  }

  [[nodiscard]] friend consteval std::intmax_t _extract_power_of_10(magnitude m)
  {
    const auto power_of_2 = _get_power(2, m);
    const auto power_of_5 = _get_power(5, m);

    if ((power_of_2 * power_of_5).num <= 0) return 0;

    return detail::integer_part((detail::abs(power_of_2) < detail::abs(power_of_5)) ? power_of_2 : power_of_5);
  }

  [[nodiscard]] friend consteval auto _magnitude_text(magnitude)
  {
    constexpr auto exp10 = _extract_power_of_10(magnitude{});

    constexpr Magnitude auto base = magnitude{} / detail::mag_power_lazy<10, exp10>();
    constexpr Magnitude auto num = _numerator(base);
    constexpr Magnitude auto den = _denominator(base);
    // TODO address the below
    static_assert(base == num / den, "Printing rational powers, or irrational bases, not yet supported");

    constexpr auto num_value = get_value<std::intmax_t>(num);
    constexpr auto den_value = get_value<std::intmax_t>(den);

    if constexpr (num_value == 1 && den_value == 1 && exp10 != 0) {
      return detail::base_multiplier + detail::superscript<exp10>();
    } else if constexpr (num_value != 1 || den_value != 1 || exp10 != 0) {
      auto txt = symbol_text("[") + detail::regular<num_value>();
      if constexpr (den_value == 1) {
        if constexpr (exp10 == 0) {
          return txt + symbol_text("]");
        } else {
          return txt + symbol_text(" ") + detail::base_multiplier + detail::superscript<exp10>() + symbol_text("]");
        }
      } else {
        if constexpr (exp10 == 0) {
          return txt + symbol_text("/") + detail::regular<den_value>() + symbol_text("]");
        } else {
          return txt + symbol_text("/") + detail::regular<den_value>() + symbol_text(" ") + detail::base_multiplier +
                 detail::superscript<exp10>() + symbol_text("]");
        }
      }
    } else {
      return symbol_text("");
    }
  }
};

[[nodiscard]] consteval auto _common_magnitude(magnitude<>, Magnitude auto m) { return _remove_positive_powers(m); }
[[nodiscard]] consteval auto _common_magnitude(Magnitude auto m, magnitude<>) { return _remove_positive_powers(m); }
[[nodiscard]] consteval auto _common_magnitude(magnitude<> m, magnitude<>) { return m; }


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

template<auto M>
[[nodiscard]] consteval auto remove_positive_power(magnitude<M> m)
{
  if constexpr (get_exponent(M).num < 0) {
    return m;
  } else {
    return magnitude<>{};
  }
}

// Returns the most precise type to express the magnitude factor
template<Magnitude auto M>
using common_magnitude_type = decltype(_common_magnitude_type_impl(M));

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

template<typename T>
concept MagArg = std::integral<T> || MagConstant<T>;

namespace detail {

template<MagArg auto V>
[[nodiscard]] consteval Magnitude auto make_magnitude()
{
  if constexpr (MagConstant<decltype(V)>)
    return magnitude<V>{};
  else
    return prime_factorization_v<V>;
}

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

template<MagArg auto V>
  requires detail::gt_zero<detail::get_base_value(V)>
constexpr Magnitude auto mag = detail::make_magnitude<V>();

template<std::intmax_t N, std::intmax_t D>
  requires detail::gt_zero<N>
constexpr Magnitude auto mag_ratio = detail::prime_factorization_v<N> / detail::prime_factorization_v<D>;

/**
 * @brief  Create a Magnitude which is some rational number raised to a rational power.
 */
template<std::intmax_t Base, std::intmax_t Pow>
  requires detail::gt_zero<Base>
constexpr Magnitude auto mag_power = pow<Pow>(mag<Base>);

/**
 * @brief  A convenient Magnitude constant for pi, which we can manipulate like a regular number.
 */
struct pi : mag_constant {
  static constexpr auto value = std::numbers::pi_v<long double>;
};
inline constexpr pi pi;

MP_UNITS_EXPORT_END

namespace detail {

// This is introduced to break the dependency cycle between `magnitude::_magnitude_text` and `prime_factorization`
template<std::intmax_t Base, std::intmax_t Pow>
  requires detail::gt_zero<Base>
[[nodiscard]] consteval Magnitude auto mag_power_lazy()
{
  return pow<Pow>(mag<Base>);
}

}  // namespace detail

}  // namespace mp_units
