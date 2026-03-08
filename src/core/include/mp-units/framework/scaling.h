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
#include <mp-units/bits/fixed_point.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/unit_magnitude_concepts.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#endif
#endif


namespace mp_units {

/**
 * @brief Intentionally silent narrowing cast with compiler float-conversion diagnostics suppressed.
 *
 * @tparam To    target type
 * @tparam From  source type (deduced)
 */
MP_UNITS_EXPORT template<typename To, typename From>
[[nodiscard]] constexpr To silent_cast(From value) noexcept
{
  MP_UNITS_DIAGNOSTIC_PUSH
  MP_UNITS_DIAGNOSTIC_IGNORE_FLOAT_CONVERSION
  return static_cast<To>(value);
  MP_UNITS_DIAGNOSTIC_POP
}

namespace detail {

template<typename T>
inline constexpr bool treat_as_integral = !treat_as_floating_point<T>;

template<typename T>
concept UsesFloatingPointScaling = treat_as_floating_point<T> && requires(T value, value_type_t<T> f) {
  { value * f } -> WeaklyRegular;
};

template<typename T>
concept UsesFixedPointScaling = treat_as_integral<value_type_t<T>> && std::is_convertible_v<T, value_type_t<T>> &&
                                std::is_convertible_v<value_type_t<T>, T>;

// If T's value_type is integer-like (non-floating-point), extract the raw integer value
// and cast it to To.  For floating-point-like types, return the value unchanged so that
// the caller can multiply/divide it directly.
template<typename To, typename T>
constexpr decltype(auto) cast_if_integral(const T& value)
{
  if constexpr (treat_as_integral<value_type_t<T>>)
    return static_cast<To>(static_cast<value_type_t<T>>(value));
  else
    return value;  // parenthesised to produce a const-ref, preserving the type for *
}

template<typename From, typename To>
struct scaling_traits_impl {
  template<auto M>
    requires(UsesFloatingPointScaling<From> || UsesFloatingPointScaling<To> ||
             (UsesFixedPointScaling<From> && UsesFixedPointScaling<To>))
  [[nodiscard]] static constexpr To scale(const From& value)
  {
    if constexpr (UsesFloatingPointScaling<From> || UsesFloatingPointScaling<To>) {
      // At least one side is floating-point: compute with common_type_t precision.
      using common_t = std::common_type_t<value_type_t<From>, value_type_t<To>>;
      static_assert(treat_as_floating_point<common_t>);
      if constexpr (is_integral(pow<-1>(M)) && !is_integral(M)) {
        // M has an integral inverse (pure divisor).  Prefer division over multiplication
        // to avoid the rounding errors introduced by 1/x in floating-point.
        constexpr common_t div = static_cast<common_t>(get_value<long double>(pow<-1>(M)));
        return silent_cast<To>(cast_if_integral<common_t>(value) / div);
      } else {
        constexpr common_t ratio = static_cast<common_t>(get_value<long double>(M));
        return silent_cast<To>(cast_if_integral<common_t>(value) * ratio);
      }
    } else {
      // Both sides are integer-like: exact integer arithmetic, no floating-point involved.
      using common_t = std::common_type_t<value_type_t<From>, value_type_t<To>>;
      static_assert(treat_as_integral<common_t>);
      if constexpr (is_integral(M)) {
        constexpr common_t mul = get_value<common_t>(M);
        return static_cast<To>(static_cast<value_type_t<From>>(value) * mul);
      } else if constexpr (is_integral(pow<-1>(M))) {
        constexpr common_t div = get_value<common_t>(pow<-1>(M));
        return static_cast<To>(static_cast<value_type_t<From>>(value) / div);
      } else if constexpr (is_integral(M * (denominator(M) / numerator(M)))) {
        // M is a pure rational p/q (no irrational factors such as π).
        // Use exact double-width integer arithmetic: avoids long double precision loss
        // on platforms where long double == double (e.g. ARM / Apple Silicon).
        constexpr common_t num = get_value<common_t>(numerator(M));
        constexpr common_t den = get_value<common_t>(denominator(M));
        if constexpr (sizeof(common_t) < sizeof(detail::int128_t)) {
          // A wider native type exists: use it to avoid intermediate overflow.
          using wide_t = detail::double_width_int_for_t<common_t>;
          return static_cast<To>(
            static_cast<common_t>(static_cast<wide_t>(static_cast<value_type_t<From>>(value)) * num / den));
        } else {
          // common_t is already the widest native integer; compute directly in common_t.
          return static_cast<To>(static_cast<common_t>(static_cast<value_type_t<From>>(value)) * num / den);
        }
      } else {
        // M has irrational factors (e.g. π); use long double fixed-point approximation.
        constexpr auto ratio = detail::fixed_point<common_t>(get_value<long double>(M));
        return static_cast<To>(ratio.scale(static_cast<value_type_t<From>>(value)));
      }
    }
  }
};

}  // namespace detail


MP_UNITS_EXPORT_BEGIN

template<typename From, typename To>
struct scaling_traits : detail::scaling_traits_impl<From, To> {};

/**
 * @brief Scale @p value by the unit magnitude passed as @p m, converting to type @c To.
 */
template<typename To, UnitMagnitude M, typename From>
  requires requires(const From& v) { scaling_traits<From, To>::template scale<M{}>(v); }
[[nodiscard]] constexpr To scale(M, const From& value)
{
  return scaling_traits<From, To>::template scale<M{}>(value);
}

MP_UNITS_EXPORT_END

}  // namespace mp_units
