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
#include <mp-units/bits/hacks.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/bits/ratio.h>
#include <mp-units/bits/text_tools.h>
#include <mp-units/ext/prime.h>
#include <mp-units/ext/type_traits.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/symbol_text.h>
#include <mp-units/framework/symbolic_expression.h>
#include <mp-units/framework/unit_magnitude_concepts.h>
#include <mp-units/framework/unit_symbol_formatting.h>

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

namespace mp_units::detail {

template<typename T>
concept MagArg = std::integral<T> || is_mag_constant<T> || is_same_v<T, ratio>;

/**
 * @brief  A sentinel type representing the factor (-1) in a unit magnitude.
 *
 * Always appears as the first element in a pack when present.  Two occurrences cancel each other out.
 * This enables support for negative magnitudes in named constants.
 */
struct negative_tag {};

template<auto H, auto...>
consteval auto first_mag_arg()
{
  return H;
}

template<typename M>
constexpr bool is_negative_tag = is_same_v<MP_UNITS_REMOVE_CONST(M), negative_tag>;

/**
 * @brief  Any type which can be used as a basis vector in a power_v.
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
// TODO Unify with `power` if UTPs (P1985) are accepted by the Committee
template<auto V, int Num, int... Den>
  requires(valid_ratio<Num, Den...> && !ratio_one<Num, Den...>)
struct power_v {
  static constexpr auto base = V;
  static constexpr ratio exponent{Num, Den...};
};

template<typename T>
[[nodiscard]] consteval auto get_base(T element)
{
  if constexpr (is_specialization_of_v<T, power_v>)
    return T::base;
  else
    return element;
}

template<typename T>
[[nodiscard]] consteval auto get_base_value(T element)
{
  if constexpr (is_specialization_of_v<T, power_v>)
    return get_base_value(T::base);
  else if constexpr (is_mag_constant<T>)
    return element._value_;
  else if constexpr (is_negative_tag<T>)
    return std::intmax_t{-1};  // sorts before all positive prime bases
  else
    return element;
}

template<MagArg auto V>
constexpr bool is_nonzero_mag_arg = get_base_value(V) != 0;

template<MagArg auto V>
constexpr bool is_positive_mag_arg = get_base_value(V) > 0;

template<auto V, ratio R>
[[nodiscard]] consteval auto power_v_or_T()
{
  if constexpr (R.den == 1) {
    if constexpr (R.num == 1)
      return V;
    else
      return power_v<V, R.num>{};
  } else {
    return power_v<V, R.num, R.den>{};
  }
}

template<typename M>
[[nodiscard]] consteval auto mag_inverse(M)
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
[[nodiscard]] consteval widen_t<T> compute_base_power(auto el)
{
  // This utility can only handle integer powers.  To compute rational powers at compile time, we'll
  // need to write a custom function.
  //
  // Note that since this function should only be called at compile time, the point of these
  // terminations is to act as "static_assert substitutes", not to actually terminate at runtime.

  // The negative_tag sentinel represents the factor (-1).
  if constexpr (is_negative_tag<decltype(get_base(el))>) {
    static_assert(!std::is_unsigned_v<T>, "Cannot represent a negative magnitude value in an unsigned type");
    return widen_t<T>{-1};
  }

  const auto exp = get_exponent(el);

  if (exp.num < 0) {
    if constexpr (std::is_integral_v<T>) {
      std::abort();  // Cannot represent reciprocal as integer
    } else {
      return T{1} / compute_base_power<T>(mag_inverse(el));
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

/**
 * @brief  Multiply one magnitude element's contribution into an optional running product.
 *
 * Used by try_get_value.  Taking M as an NTTP (rather than a function argument) makes it a
 * constant expression on all compilers, including GCC 12.
 */
template<typename T, auto M>
consteval void try_accumulate_element(std::optional<T>& result)
{
  if (!result) return;
  if constexpr (is_negative_tag<decltype(get_base(M))>) {
    result = std::nullopt;  // (-1) factor: not representable as unsigned
    return;
  }
  const auto elem =
    checked_int_pow(static_cast<T>(get_base_value(M)), static_cast<std::uintmax_t>(get_exponent(M).num));
  if (!elem) {
    result = std::nullopt;
    return;
  }
  if (*result != 0 && *elem > std::numeric_limits<T>::max() / *result) {
    result = std::nullopt;
    return;
  }
  *result *= *elem;
}

[[nodiscard]] consteval bool is_rational_impl(auto element)
{
  if constexpr (is_negative_tag<decltype(element)>)
    return true;  // (-1) is a rational number
  else
    return std::is_integral_v<decltype(get_base(element))> && get_exponent(element).den == 1;
}

[[nodiscard]] consteval bool is_integral_impl(auto element)
{
  if constexpr (is_negative_tag<decltype(element)>)
    return true;  // (-1) is an integer
  else
    return is_rational_impl(element) && get_exponent(element).num > 0;
}

[[nodiscard]] consteval bool is_positive_integral_power_impl(auto element)
{
  if constexpr (is_negative_tag<decltype(element)>)
    return false;  // (-1) is not a positive factor
  else {
    auto exp = get_exponent(element);
    return exp.den == 1 && exp.num > 0;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Magnitude product implementation.
[[nodiscard]] consteval bool mag_less(auto lhs, auto rhs)
{
  // clang-arm64 raises "error: implicit conversion from 'long' to 'long double' may lose precision" so we need an
  // explicit cast
  using ct = std::common_type_t<decltype(get_base_value(lhs)), decltype(get_base_value(rhs))>;
  return static_cast<ct>(get_base_value(lhs)) < static_cast<ct>(get_base_value(rhs));
}

// The largest integer which can be extracted from any magnitude with only a single basis vector.
template<auto M>
[[nodiscard]] consteval auto integer_part(unit_magnitude<M>);
[[nodiscard]] consteval std::intmax_t integer_part(ratio r) { return r.num / r.den; }

template<auto M>
[[nodiscard]] consteval auto remove_positive_power(unit_magnitude<M> m);
template<auto M>
[[nodiscard]] consteval auto remove_mag_constants(unit_magnitude<M> m);
template<auto M>
[[nodiscard]] consteval auto only_positive_mag_constants(unit_magnitude<M> m);
template<auto M>
[[nodiscard]] consteval auto only_negative_mag_constants(unit_magnitude<M> m);

template<MagArg auto Base, int Num, int Den = 1>
  requires is_positive_mag_arg<Base>
[[nodiscard]] consteval UnitMagnitude auto mag_power_lazy();

// Forward declarations; fully defined after unit_magnitude
template<auto H, auto... Rest>
[[nodiscard]] consteval auto abs_magnitude(unit_magnitude<H, Rest...>);
[[nodiscard]] consteval auto abs_magnitude(unit_magnitude<>);

template<int Num, int Den>
[[nodiscard]] consteval auto pow_magnitude(unit_magnitude<>);
template<int Num, int Den, auto H, auto... Rest>
[[nodiscard]] consteval auto pow_magnitude(unit_magnitude<H, Rest...>);

template<typename T>
struct magnitude_base {};

template<auto H, auto... T>
struct magnitude_base<unit_magnitude<H, T...>> {
  template<auto H2, auto... T2>
  [[nodiscard]] friend consteval UnitMagnitude auto multiply_impl(unit_magnitude<H, T...>, unit_magnitude<H2, T2...>)
  {
    if constexpr (mag_less(H, H2)) {
      if constexpr (sizeof...(T) == 0) {
        // Shortcut for the "pure prepend" case, which makes it easier to implement some of the other cases.
        return unit_magnitude<H, H2, T2...>{};
      } else {
        return unit_magnitude<H>{} * (unit_magnitude<T...>{} * unit_magnitude<H2, T2...>{});
      }
    } else if constexpr (mag_less(H2, H)) {
      return unit_magnitude<H2>{} * (unit_magnitude<H, T...>{} * unit_magnitude<T2...>{});
    } else {
      if constexpr (is_same_v<decltype(get_base(H)), decltype(get_base(H2))>) {
        constexpr auto partial_product = unit_magnitude<T...>{} * unit_magnitude<T2...>{};
        if constexpr (is_negative_tag<decltype(get_base(H))>) {
          // (-1) * (-1) = 1: two negatives cancel each other out
          return partial_product;
        } else if constexpr (get_exponent(H) + get_exponent(H2) == 0) {
          return partial_product;
        } else {
          // Make a new power_v with the common base of H and H2, whose power is their powers' sum.
          constexpr auto new_head = power_v_or_T<get_base(H), get_exponent(H) + get_exponent(H2)>();

          if constexpr (get_exponent(new_head) == 0) {
            return partial_product;
          } else {
            return unit_magnitude<new_head>{} * partial_product;
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
  [[nodiscard]] friend consteval auto common_magnitude(unit_magnitude<H, T...>, unit_magnitude<H2, T2...>)
  {
    if constexpr (get_base_value(H) < get_base_value(H2)) {
      // When H1 has the smaller base, prepend to result from recursion.
      return mp_units::detail::remove_positive_power(unit_magnitude<H>{}) *
             common_magnitude(unit_magnitude<T...>{}, unit_magnitude<H2, T2...>{});
    } else if constexpr (get_base_value(H2) < get_base_value(H)) {
      // When H2 has the smaller base, prepend to result from recursion.
      return mp_units::detail::remove_positive_power(unit_magnitude<H2>{}) *
             common_magnitude(unit_magnitude<H, T...>{}, unit_magnitude<T2...>{});
    } else {
      // When the bases are equal, pick whichever has the lower power.
      constexpr auto common_tail = common_magnitude(unit_magnitude<T...>{}, unit_magnitude<T2...>{});
      if constexpr (get_exponent(H) < get_exponent(H2)) {
        return unit_magnitude<H>{} * common_tail;
      } else {
        return unit_magnitude<H2>{} * common_tail;
      }
    }
  }
};

template<auto... Ms>
[[nodiscard]] consteval std::size_t magnitude_list_size(unit_magnitude<Ms...>)
{
  return sizeof...(Ms);
}

template<typename CharT, std::output_iterator<CharT> Out>
[[nodiscard]] constexpr Out print_separator(Out out, const unit_symbol_formatting& fmt)
{
  if (fmt.separator == unit_symbol_separator::half_high_dot) {
    if (fmt.char_set != character_set::utf8)
      MP_UNITS_THROW(
        std::invalid_argument("'unit_symbol_separator::half_high_dot' can be only used with 'character_set::utf8'"));
    const std::string_view dot = "⋅" /* U+22C5 DOT OPERATOR */;
    out = detail::copy(dot.begin(), dot.end(), out);
  } else {
    *out++ = ' ';
  }
  return out;
}

template<typename CharT, std::output_iterator<CharT> Out, auto... Ms>
  requires(sizeof...(Ms) == 0)
[[nodiscard]] constexpr auto mag_constants_text(Out out, unit_magnitude<Ms...>, const unit_symbol_formatting&, bool)
{
  return out;
}

template<typename CharT, std::output_iterator<CharT> Out, auto M, auto... Rest>
[[nodiscard]] constexpr auto mag_constants_text(Out out, unit_magnitude<M, Rest...>, const unit_symbol_formatting& fmt,
                                                bool negative_power)
{
  auto to_symbol = [&]<typename T>(T v) {
    out = copy_symbol<CharT>(get_base(v)._symbol_, fmt.char_set, negative_power, out);
    constexpr ratio r = get_exponent(T{});
    return copy_symbol_exponent<CharT, detail::abs(r.num), r.den>(fmt.char_set, negative_power, out);
  };
  return (to_symbol(M), ..., (out = print_separator<CharT>(out, fmt), to_symbol(Rest)));
}

template<typename CharT, UnitMagnitude auto Num, UnitMagnitude auto Den, UnitMagnitude auto NumConstants,
         UnitMagnitude auto DenConstants, std::intmax_t Exp10, std::output_iterator<CharT> Out>
[[nodiscard]] constexpr Out magnitude_symbol_impl(Out out, const unit_symbol_formatting& fmt)
{
  bool numerator = false;
  constexpr auto num_value = get_value<std::intmax_t>(Num);
  if constexpr (num_value != 1) {
    constexpr auto num = regular<num_value>();
    out = copy_symbol<CharT>(num, fmt.char_set, false, out);
    numerator = true;
  }

  constexpr auto num_constants_size = magnitude_list_size(NumConstants);
  if constexpr (num_constants_size) {
    if (numerator) out = print_separator<CharT>(out, fmt);
    out = mag_constants_text<CharT>(out, NumConstants, fmt, false);
    numerator = true;
  }

  using enum unit_symbol_solidus;
  bool denominator = false;
  constexpr auto den_value = get_value<std::intmax_t>(Den);
  constexpr auto den_constants_size = magnitude_list_size(DenConstants);
  constexpr auto den_size = (den_value != 1) + den_constants_size;
  auto start_denominator = [&]() {
    if (fmt.solidus == always || (fmt.solidus == one_denominator && den_size == 1)) {
      if (!numerator) *out++ = '1';
      *out++ = '/';
      if constexpr (den_size > 1) *out++ = '(';
    } else if (numerator) {
      out = print_separator<CharT>(out, fmt);
    }
  };
  const bool negative_power = fmt.solidus == never || (fmt.solidus == one_denominator && den_size > 1);
  if constexpr (den_value != 1) {
    constexpr auto den = regular<den_value>();
    start_denominator();
    out = copy_symbol<CharT>(den, fmt.char_set, negative_power, out);
    denominator = true;
  }

  if constexpr (den_constants_size) {
    if (denominator)
      out = print_separator<CharT>(out, fmt);
    else
      start_denominator();
    out = mag_constants_text<CharT>(out, DenConstants, fmt, negative_power);
    if (fmt.solidus == always && den_size > 1) *out++ = ')';
    denominator = true;
  }

  if constexpr (Exp10 != 0) {
    if (numerator || denominator) {
      constexpr auto mag_multiplier = symbol_text(u8" × " /* U+00D7 MULTIPLICATION SIGN */, " x ");
      out = copy_symbol<CharT>(mag_multiplier, fmt.char_set, negative_power, out);
    }
    constexpr auto exp = symbol_text("10") + superscript<Exp10>();
    out = copy_symbol<CharT>(exp, fmt.char_set, negative_power, out);
  }

  return out;
}

/**
 * @brief  A representation for positive real numbers which optimizes taking products and rational powers.
 *
 * Magnitudes can be treated as values.  Each type encodes exactly one value.  Users can multiply, divide, raise to
 * rational powers, and compare for equality.
 */
template<auto... Ms>
struct unit_magnitude : magnitude_base<unit_magnitude<Ms...>> {
  template<UnitMagnitude M>
  [[nodiscard]] friend consteval UnitMagnitude auto operator*(unit_magnitude lhs, M rhs)
  {
    if constexpr (sizeof...(Ms) == 0)
      return rhs;
    else if constexpr (is_same_v<M, unit_magnitude<>>)
      return lhs;
    else
      return multiply_impl(lhs, rhs);
  }

  [[nodiscard]] friend consteval auto operator/(unit_magnitude lhs, UnitMagnitude auto rhs)
  {
    return lhs * pow<-1>(rhs);
  }

  template<UnitMagnitude Rhs>
  [[nodiscard]] friend consteval bool operator==(unit_magnitude, Rhs)
  {
    return is_same_v<unit_magnitude, Rhs>;
  }

private:
  // all below functions should in fact be in a `detail` namespace but are placed here to benefit from the ADL
  [[nodiscard]] friend consteval bool is_integral(const unit_magnitude&) { return (is_integral_impl(Ms) && ...); }
  [[nodiscard]] friend consteval bool is_rational(const unit_magnitude&) { return (is_rational_impl(Ms) && ...); }
  [[nodiscard]] friend consteval bool is_positive_integral_power(const unit_magnitude&)
  {
    return (is_positive_integral_power_impl(Ms) && ...);
  }

  /**
   * @brief  Like get_value<T> but returns std::nullopt instead of aborting on overflow.
   *
   * Useful for overflow-detection in concepts and constraints, where a hard abort would
   * prevent the expression from being softly evaluated in a `requires` clause.
   * Only available for unsigned integral T.
   */
  template<typename T>
    requires std::is_unsigned_v<T> && ((is_integral_impl(Ms) && ...))
  [[nodiscard]] friend consteval std::optional<T> try_get_value(const unit_magnitude&)
  {
    std::optional<T> result{T{1}};
    (try_accumulate_element<T, Ms>(result), ...);
    return result;
  }

  /**
   * @brief  The value of a Magnitude in a desired type T.
   */
  template<typename T>
    requires((is_integral_impl(Ms) && ...)) || treat_as_floating_point<T>
  [[nodiscard]] friend consteval T get_value(const unit_magnitude&)
  {
    if constexpr (std::is_unsigned_v<T>) {
      constexpr auto result = try_get_value<T>(unit_magnitude{});
      static_assert(result.has_value(), "Magnitude value overflows the target unsigned type");
      return *result;
    } else {
      // Signed integer or floating-point: use the widening fold expression via compute_base_power.
      constexpr T result = checked_static_cast<T>((compute_base_power<T>(Ms) * ... * T{1}));
      return result;
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Magnitude rational powers implementation.
  template<int Num, int Den = 1>
  [[nodiscard]] friend consteval auto pow(unit_magnitude)
  {
    if constexpr (Num == 0) {
      return unit_magnitude<>{};
    } else {
      return pow_magnitude<Num, Den>(unit_magnitude{});
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Magnitude numerator and denominator implementation.
  [[nodiscard]] friend consteval auto numerator(unit_magnitude)
  {
    return (mp_units::detail::integer_part(unit_magnitude<Ms>{}) * ... * unit_magnitude<>{});
  }

  [[nodiscard]] friend consteval auto denominator(unit_magnitude)
  {
    return numerator(pow<-1>(mp_units::detail::abs_magnitude(unit_magnitude{})));
  }

  [[nodiscard]] friend consteval auto remove_positive_powers(unit_magnitude)
  {
    return (unit_magnitude<>{} * ... * mp_units::detail::remove_positive_power(unit_magnitude<Ms>{}));
  }

  [[nodiscard]] friend consteval auto common_magnitude_type_impl(unit_magnitude)
  {
    return (std::intmax_t{} * ... * decltype(get_base_value(Ms)){});
  }

  [[nodiscard]] friend consteval auto extract_components(unit_magnitude)
  {
    constexpr auto ratio = (unit_magnitude<>{} * ... * mp_units::detail::remove_mag_constants(unit_magnitude<Ms>{}));
    if constexpr (ratio == unit_magnitude{})
      return std::tuple{ratio, unit_magnitude<>{}, unit_magnitude<>{}};
    else {
      constexpr auto num_constants =
        (unit_magnitude<>{} * ... * mp_units::detail::only_positive_mag_constants(unit_magnitude<Ms>{}));
      constexpr auto den_constants =
        (unit_magnitude<>{} * ... * mp_units::detail::only_negative_mag_constants(unit_magnitude<Ms>{}));
      return std::tuple{ratio, num_constants, den_constants};
    }
  }

  template<typename T>
  [[nodiscard]] friend consteval ratio get_power([[maybe_unused]] T base, unit_magnitude)
  {
    [[maybe_unused]] auto is_base = [&](auto element) consteval {
      if constexpr (is_negative_tag<decltype(element)>)
        return false;  // (-1) is not a base, it's a special sentinel
      else
        return get_base_value(element) == base;
    };
    return ((is_base(Ms) ? get_exponent(Ms) : ratio{0}) + ... + ratio{0});
  }

  [[nodiscard]] friend consteval std::intmax_t extract_power_of_10(unit_magnitude mag)
  {
    const auto power_of_2 = get_power(2, mag);
    const auto power_of_5 = get_power(5, mag);

    if ((power_of_2 * power_of_5).num <= 0) return 0;

    return integer_part((detail::abs(power_of_2) < detail::abs(power_of_5)) ? power_of_2 : power_of_5);
  }

  template<typename CharT, std::output_iterator<CharT> Out>
  [[nodiscard]] friend constexpr Out magnitude_symbol(Out out, unit_magnitude, const unit_symbol_formatting& fmt)
  {
    if constexpr (sizeof...(Ms) == 0) {
      return out;
    } else {
      // If the magnitude is negative (starts with negative_tag), prepend '-' and delegate.
      constexpr bool is_negative = sizeof...(Ms) > 0 && is_negative_tag<decltype(first_mag_arg<Ms...>())>;
      if constexpr (is_negative) {
        *out++ = '-';
        constexpr UnitMagnitude auto abs_mag = mp_units::detail::abs_magnitude(unit_magnitude{});
        return magnitude_symbol<CharT>(out, abs_mag, fmt);
      } else {
        constexpr auto extract_res = extract_components(unit_magnitude{});
        constexpr UnitMagnitude auto ratio = std::get<0>(extract_res);
        constexpr UnitMagnitude auto num_constants = std::get<1>(extract_res);
        constexpr UnitMagnitude auto den_constants = std::get<2>(extract_res);
        constexpr std::intmax_t exp10 = extract_power_of_10(ratio);
        if constexpr (detail::abs(exp10) < 3) {
          // print the value as a regular number (without exponent)
          constexpr UnitMagnitude auto num = numerator(unit_magnitude{});
          constexpr UnitMagnitude auto den = denominator(unit_magnitude{});
          // TODO address the below
          static_assert(ratio == num / den, "Printing rational powers not yet supported");
          return magnitude_symbol_impl<CharT, num, den, num_constants, den_constants, 0>(out, fmt);
        } else {
          // print the value as a number with exponent
          // if user wanted a regular number for this magnitude then probably a better scaled unit should be used
          constexpr UnitMagnitude auto base = ratio / mp_units::detail::mag_power_lazy<10, exp10>();
          constexpr UnitMagnitude auto num = numerator(base);
          constexpr UnitMagnitude auto den = denominator(base);

          // TODO address the below
          static_assert(base == num / den, "Printing rational powers not yet supported");
          return magnitude_symbol_impl<CharT, num, den, num_constants, den_constants, exp10>(out, fmt);
        }
      }
    }
  }
};

template<int Num, int Den>
[[nodiscard]] consteval auto pow_magnitude(unit_magnitude<>)
{
  return unit_magnitude<>{};
}

template<int Num, int Den, auto H, auto... Rest>
[[nodiscard]] consteval auto pow_magnitude(unit_magnitude<H, Rest...>)
{
  if constexpr (is_negative_tag<decltype(H)>) {
    // Raising (-1) to the power Num/Den (in lowest terms):
    //  - even denominator → taking an even root of a negative number → hard error
    //  - even numerator   → result is positive (+1), negative_tag cancels out
    //  - odd numerator    → result is negative (-1), negative_tag is preserved
    static_assert(ratio{Num, Den}.den % 2 == 1, "Cannot take even root of negative magnitude");
    constexpr auto rest_powered = pow_magnitude<Num, Den>(unit_magnitude<Rest...>{});
    if constexpr (ratio{Num, Den}.num % 2 == 0)
      return rest_powered;
    else
      return unit_magnitude<H>{} * rest_powered;
  } else {
    // No negative_tag at front: apply power uniformly to all elements.
    return unit_magnitude<power_v_or_T<get_base(H), get_exponent(H) * ratio{Num, Den}>(),
                          power_v_or_T<get_base(Rest), get_exponent(Rest) * ratio{Num, Den}>()...>{};
  }
}

[[nodiscard]] consteval auto common_magnitude(unit_magnitude<>, UnitMagnitude auto m)
{
  return remove_positive_powers(m);
}
[[nodiscard]] consteval auto common_magnitude(UnitMagnitude auto m, unit_magnitude<>)
{
  return remove_positive_powers(m);
}
[[nodiscard]] consteval auto common_magnitude(unit_magnitude<> m, unit_magnitude<>) { return m; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// `abs_magnitude` — strips the leading negative_tag if present, leaving the absolute value.

template<auto H, auto... Rest>
[[nodiscard]] consteval auto abs_magnitude(unit_magnitude<H, Rest...>)
{
  if constexpr (is_negative_tag<decltype(H)>)
    return unit_magnitude<Rest...>{};
  else
    return unit_magnitude<H, Rest...>{};
}

[[nodiscard]] consteval auto abs_magnitude(unit_magnitude<>) { return unit_magnitude<>{}; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// `magnitude_is_positive` — checks whether a magnitude is positive (i.e., has no negative_tag).

// Uses a consteval function rather than `is_same_v<decltype(M), decltype(abs_magnitude(M))>` because GCC 12
// incorrectly evaluates the latter to `false` for genuinely positive magnitudes in some constexpr contexts.
[[nodiscard]] consteval bool check_magnitude_is_positive(unit_magnitude<>) { return true; }

template<auto H, auto... Rest>
[[nodiscard]] consteval bool check_magnitude_is_positive(unit_magnitude<H, Rest...>)
{
  return !is_negative_tag<decltype(H)>;
}

template<UnitMagnitude auto M>
constexpr bool magnitude_is_positive = check_magnitude_is_positive(M);

// The largest integer which can be extracted from any magnitude with only a single basis vector.
template<auto M>
[[nodiscard]] consteval auto integer_part(unit_magnitude<M>)
{
  // The negative_tag is the integer (-1): include it in the integer part.
  // The else is required so that the rest of the body (which calls get_exponent/get_base on M)
  // is not instantiated for negative_tag, avoiding conflicting return type deductions.
  if constexpr (is_negative_tag<decltype(M)>) {
    return unit_magnitude<M>{};
  } else {
    constexpr auto power_num = get_exponent(M).num;
    constexpr auto power_den = get_exponent(M).den;

    if constexpr (std::is_integral_v<decltype(get_base(M))> && (power_num >= power_den)) {
      // largest integer power
      return unit_magnitude<power_v_or_T<get_base(M), power_num / power_den>()>{};  // Note: integer division intended
    } else {
      return unit_magnitude<>{};
    }
  }
}

template<auto M>
[[nodiscard]] consteval auto remove_positive_power(unit_magnitude<M> m)
{
  if constexpr (is_negative_tag<decltype(M)>)
    return unit_magnitude<>{};  // negative_tag is a sign sentinel, not a basis element; exclude it
  else if constexpr (get_exponent(M).num < 0) {
    return m;
  } else {
    return unit_magnitude<>{};
  }
}

template<auto M>
[[nodiscard]] consteval auto remove_mag_constants(unit_magnitude<M> m)
{
  if constexpr (is_negative_tag<decltype(M)>)
    return m;  // negative_tag is a sign marker, not a mag_constant; keep it in the ratio part
  else if constexpr (is_mag_constant<decltype(get_base(M))>)
    return unit_magnitude<>{};
  else
    return m;
}

template<auto M>
[[nodiscard]] consteval auto only_positive_mag_constants(unit_magnitude<M> m)
{
  if constexpr (is_negative_tag<decltype(M)>)
    return unit_magnitude<>{};
  else if constexpr (is_mag_constant<decltype(get_base(M))> && get_exponent(M) >= 0)
    return m;
  else
    return unit_magnitude<>{};
}

template<auto M>
[[nodiscard]] consteval auto only_negative_mag_constants(unit_magnitude<M> m)
{
  if constexpr (is_negative_tag<decltype(M)>)
    return unit_magnitude<>{};
  else if constexpr (is_mag_constant<decltype(get_base(M))> && get_exponent(M) < 0)
    return m;
  else
    return unit_magnitude<>{};
}

// Returns the most precise type to express the magnitude factor
template<UnitMagnitude auto M>
using common_magnitude_type = decltype(common_magnitude_type_impl(M));

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// `mag()` implementation.

// Helper to perform prime factorization at compile time.
template<std::intmax_t N>
  requires(N > 0)
struct prime_factorization {
  [[nodiscard]] static consteval std::intmax_t get_or_compute_first_factor()
  {
    return static_cast<std::intmax_t>(find_first_factor(N));
  }

  static constexpr std::intmax_t first_base = get_or_compute_first_factor();
  static constexpr std::intmax_t first_power = multiplicity(first_base, N);
  static constexpr std::intmax_t remainder = remove_power(first_base, first_power, N);

  static constexpr auto value =
    unit_magnitude<power_v_or_T<first_base, ratio{first_power}>()>{} * prime_factorization<remainder>::value;
};

// Specialization for the prime factorization of 1 (base case).
template<>
struct prime_factorization<1> {
  static constexpr unit_magnitude<> value{};
};

template<std::intmax_t N>
constexpr auto prime_factorization_v = prime_factorization<N>::value;

template<MagArg auto V>
[[nodiscard]] consteval UnitMagnitude auto make_magnitude()
{
  using mag_arg_type = MP_UNITS_REMOVE_CONST(decltype(V));

  if constexpr (is_mag_constant<mag_arg_type>)
    return unit_magnitude<V>{};
  else if constexpr (is_same_v<mag_arg_type, ratio>) {
    // ratio{num, den}: factor out the sign, then factorize num and den
    constexpr ratio abs_v{V.num < 0 ? -V.num : V.num, V.den};
    constexpr bool negative = V.num < 0;
    constexpr UnitMagnitude auto abs_mag = prime_factorization_v<abs_v.num> / prime_factorization_v<abs_v.den>;
    if constexpr (negative)
      return unit_magnitude<negative_tag{}>{} * abs_mag;
    else
      return abs_mag;
  } else if constexpr (std::integral<mag_arg_type>) {
    // Integer NTTP: factor out the sign, then prime-factorize the absolute value
    constexpr auto abs_v = V < 0 ? -V : V;
    constexpr UnitMagnitude auto abs_mag = prime_factorization_v<abs_v>;
    if constexpr (V < 0)
      return unit_magnitude<negative_tag{}>{} * abs_mag;
    else
      return abs_mag;
  } else
    return prime_factorization_v<V>;
}

}  // namespace mp_units::detail
