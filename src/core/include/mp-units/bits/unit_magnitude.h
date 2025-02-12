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
concept MagArg = std::integral<T> || MagConstant<T>;

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
  else if constexpr (MagConstant<T>)
    return element._value_;
  else
    return element;
}

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

[[nodiscard]] consteval bool is_rational_impl(auto element)
{
  return std::is_integral_v<decltype(get_base(element))> && get_exponent(element).den == 1;
}

[[nodiscard]] consteval bool is_integral_impl(auto element)
{
  return is_rational_impl(element) && get_exponent(element).num > 0;
}

[[nodiscard]] consteval bool is_positive_integral_power_impl(auto element)
{
  auto exp = get_exponent(element);
  return exp.den == 1 && exp.num > 0;
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
  requires(get_base_value(Base) > 0)
[[nodiscard]] consteval UnitMagnitude auto mag_power_lazy();

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
        if constexpr (get_exponent(H) + get_exponent(H2) == 0) {
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
      return remove_positive_power(unit_magnitude<H>{}) *
             common_magnitude(unit_magnitude<T...>{}, unit_magnitude<H2, T2...>{});
    } else if constexpr (get_base_value(H2) < get_base_value(H)) {
      // When H2 has the smaller base, prepend to result from recursion.
      return remove_positive_power(unit_magnitude<H2>{}) *
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
constexpr Out print_separator(Out out, const unit_symbol_formatting& fmt)
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
    return copy_symbol_exponent<CharT, abs(r.num), r.den>(fmt.char_set, negative_power, out);
  };
  return (to_symbol(M), ..., (print_separator<CharT>(out, fmt), to_symbol(Rest)));
}

template<typename CharT, UnitMagnitude auto Num, UnitMagnitude auto Den, UnitMagnitude auto NumConstants,
         UnitMagnitude auto DenConstants, std::intmax_t Exp10, std::output_iterator<CharT> Out>
constexpr Out magnitude_symbol_impl(Out out, const unit_symbol_formatting& fmt)
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
      if (den_size > 1) *out++ = '(';
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
   * @brief  The value of a Magnitude in a desired type T.
   */
  template<typename T>
    requires((is_integral_impl(Ms) && ...)) || treat_as_floating_point<T>
  [[nodiscard]] friend consteval T get_value(const unit_magnitude&)
  {
    // Force the expression to be evaluated in a constexpr context, to catch, e.g., overflow.
    constexpr T result = checked_static_cast<T>((compute_base_power<T>(Ms) * ... * T{1}));
    return result;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Magnitude rational powers implementation.
  template<int Num, int Den = 1>
  [[nodiscard]] friend consteval auto pow(unit_magnitude)
  {
    if constexpr (Num == 0) {
      return unit_magnitude<>{};
    } else {
      return unit_magnitude<power_v_or_T<get_base(Ms), get_exponent(Ms) * ratio{Num, Den}>()...>{};
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Magnitude numerator and denominator implementation.
  [[nodiscard]] friend consteval auto numerator(unit_magnitude)
  {
    return (integer_part(unit_magnitude<Ms>{}) * ... * unit_magnitude<>{});
  }

  [[nodiscard]] friend consteval auto denominator(unit_magnitude) { return numerator(pow<-1>(unit_magnitude{})); }

  [[nodiscard]] friend consteval auto remove_positive_powers(unit_magnitude)
  {
    return (unit_magnitude<>{} * ... * remove_positive_power(unit_magnitude<Ms>{}));
  }

  [[nodiscard]] friend consteval auto common_magnitude_type_impl(unit_magnitude)
  {
    return (std::intmax_t{} * ... * decltype(get_base_value(Ms)){});
  }

  [[nodiscard]] friend consteval auto extract_components(unit_magnitude)
  {
    constexpr auto ratio = (unit_magnitude<>{} * ... * remove_mag_constants(unit_magnitude<Ms>{}));
    if constexpr (ratio == unit_magnitude{})
      return std::tuple{ratio, unit_magnitude<>{}, unit_magnitude<>{}};
    else {
      constexpr auto num_constants = (unit_magnitude<>{} * ... * only_positive_mag_constants(unit_magnitude<Ms>{}));
      constexpr auto den_constants = (unit_magnitude<>{} * ... * only_negative_mag_constants(unit_magnitude<Ms>{}));
      return std::tuple{ratio, num_constants, den_constants};
    }
  }

  template<typename T>
  [[nodiscard]] friend consteval ratio get_power([[maybe_unused]] T base, unit_magnitude)
  {
    return ((get_base_value(Ms) == base ? get_exponent(Ms) : ratio{0}) + ... + ratio{0});
  }

  [[nodiscard]] friend consteval std::intmax_t extract_power_of_10(unit_magnitude m)
  {
    const auto power_of_2 = get_power(2, m);
    const auto power_of_5 = get_power(5, m);

    if ((power_of_2 * power_of_5).num <= 0) return 0;

    return integer_part((abs(power_of_2) < abs(power_of_5)) ? power_of_2 : power_of_5);
  }

  template<typename CharT, std::output_iterator<CharT> Out>
  friend constexpr Out magnitude_symbol(Out out, unit_magnitude, const unit_symbol_formatting& fmt)
  {
    if constexpr (unit_magnitude{} == unit_magnitude<1>{}) {
      return out;
    } else {
      constexpr auto extract_res = extract_components(unit_magnitude{});
      constexpr UnitMagnitude auto ratio = std::get<0>(extract_res);
      constexpr UnitMagnitude auto num_constants = std::get<1>(extract_res);
      constexpr UnitMagnitude auto den_constants = std::get<2>(extract_res);
      constexpr std::intmax_t exp10 = extract_power_of_10(ratio);
      if constexpr (abs(exp10) < 3) {
        // print the value as a regular number (without exponent)
        constexpr UnitMagnitude auto num = numerator(unit_magnitude{});
        constexpr UnitMagnitude auto den = denominator(unit_magnitude{});
        // TODO address the below
        static_assert(ratio == num / den, "Printing rational powers not yet supported");
        return magnitude_symbol_impl<CharT, num, den, num_constants, den_constants, 0>(out, fmt);
      } else {
        // print the value as a number with exponent
        // if user wanted a regular number for this magnitude then probably a better scaled unit should be used
        constexpr UnitMagnitude auto base = ratio / mag_power_lazy<10, exp10>();
        constexpr UnitMagnitude auto num = numerator(base);
        constexpr UnitMagnitude auto den = denominator(base);

        // TODO address the below
        static_assert(base == num / den, "Printing rational powers not yet supported");
        return magnitude_symbol_impl<CharT, num, den, num_constants, den_constants, exp10>(out, fmt);
      }
    }
  }
};

[[nodiscard]] consteval auto common_magnitude(unit_magnitude<>, UnitMagnitude auto m)
{
  return remove_positive_powers(m);
}
[[nodiscard]] consteval auto common_magnitude(UnitMagnitude auto m, unit_magnitude<>)
{
  return remove_positive_powers(m);
}
[[nodiscard]] consteval auto common_magnitude(unit_magnitude<> m, unit_magnitude<>) { return m; }


// The largest integer which can be extracted from any magnitude with only a single basis vector.
template<auto M>
[[nodiscard]] consteval auto integer_part(unit_magnitude<M>)
{
  constexpr auto power_num = get_exponent(M).num;
  constexpr auto power_den = get_exponent(M).den;

  if constexpr (std::is_integral_v<decltype(get_base(M))> && (power_num >= power_den)) {
    // largest integer power
    return unit_magnitude<power_v_or_T<get_base(M), power_num / power_den>()>{};  // Note: integer division intended
  } else {
    return unit_magnitude<>{};
  }
}

template<auto M>
[[nodiscard]] consteval auto remove_positive_power(unit_magnitude<M> m)
{
  if constexpr (get_exponent(M).num < 0) {
    return m;
  } else {
    return unit_magnitude<>{};
  }
}

template<auto M>
[[nodiscard]] consteval auto remove_mag_constants(unit_magnitude<M> m)
{
  if constexpr (MagConstant<decltype(get_base(M))>)
    return unit_magnitude<>{};
  else
    return m;
}

template<auto M>
[[nodiscard]] consteval auto only_positive_mag_constants(unit_magnitude<M> m)
{
  if constexpr (MagConstant<decltype(get_base(M))> && get_exponent(M) >= 0)
    return m;
  else
    return unit_magnitude<>{};
}

template<auto M>
[[nodiscard]] consteval auto only_negative_mag_constants(unit_magnitude<M> m)
{
  if constexpr (MagConstant<decltype(get_base(M))> && get_exponent(M) < 0)
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
  if constexpr (MagConstant<MP_UNITS_REMOVE_CONST(decltype(V))>)
    return unit_magnitude<V>{};
  else
    return prime_factorization_v<V>;
}

}  // namespace mp_units::detail
