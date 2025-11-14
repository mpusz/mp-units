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

namespace detail {

template<typename To, typename T>
constexpr auto cast_if_integral(const T& value)
{
  if constexpr (std::is_integral_v<value_type_t<T>>) {
    return static_cast<To>(value);
  } else {
    return value;
  }
}

// @brief For a representation type that uses "floating-point scaling", select an appropriate floating-point type as
// scale factor.
template<typename T>
struct floating_point_scaling_factor_type;

template<std::floating_point T>
struct floating_point_scaling_factor_type<T> {
  using type = T;
};

// try to choose the smallest standard floating-point type which can represent the integer exactly (has at least as many
// mantiassa bits as the integer is wide)
template<std::integral T>
struct floating_point_scaling_factor_type<T> {
  using type = min_digit_float_t<std::numeric_limits<T>::digits>;
};

template<typename T>
  requires requires {
    typename T::value_type;
    typename floating_point_scaling_factor_type<typename T::value_Type>::type;
  }
struct floating_point_scaling_factor_type<T> {
  using type = floating_point_scaling_factor_type<typename T::value_Type>::type;
};

template<typename T>
concept UsesFloatingPointScaling =
  treat_as_floating_point<T> && requires(T value, floating_point_scaling_factor_type<value_type_t<T>>::type f) {
    // the result representation does not necessarily have to be the same.
    { value * f } -> std::equality_comparable;
    { value * f } -> std::copyable;
  };

template<typename T>
concept IsIntegerLike = std::is_integral_v<value_type_t<T>> && std::is_convertible_v<T, value_type_t<T>> &&
                        std::is_convertible_v<value_type_t<T>, T>;

template<typename T>
concept UsesFixedPointScaling = IsIntegerLike<T>;

template<typename T>
concept UsesFloatingPointScalingOrIsIntegerLike = UsesFloatingPointScaling<T> || IsIntegerLike<T>;


template<typename From, typename To = unspecified_rep>
concept HasScalingTraits = requires {
  { sizeof(mp_units::scaling_traits<From, To>) } -> std::convertible_to<std::size_t>;
};

}  // namespace detail


/**
 * @brief `scaling_traits` for representations that scale by multiplication with a float
 *
 * This class implements scaling by either multiplying or dividing the value with
 * a floating-point representation of the scaling factor; the floating-point representation
 * is chosen such that it is of comparable precision as the representation type,
 *
 * It is used for all cases where at least one of the two is "floating-point like",
 * and the other one is either "floating-point like" or "integer-like".
 * Here, we call type "X-like" if it either is an "X"-standard type, or it has a
 * a nested type `value_type` which is an "X"-standard type and those two are implicityl interconvertible.
 *
 * @tparam Rep Representation type
 */
template<detail::UsesFloatingPointScalingOrIsIntegerLike From, detail::UsesFloatingPointScalingOrIsIntegerLike To>
  requires((detail::UsesFloatingPointScaling<From> || detail::UsesFloatingPointScaling<To>))
struct scaling_traits<From, To> {
  using _scaling_factor_type = std::common_type_t<value_type_t<From>, value_type_t<To>>;
  static_assert(std::is_floating_point_v<_scaling_factor_type>);

  template<UnitMagnitude auto M>
  static constexpr bool implicitly_scalable =
    std::is_convertible_v<decltype(detail::cast_if_integral<_scaling_factor_type>(std::declval<From>()) *
                                   std::declval<_scaling_factor_type>()),
                          To>;

  template<UnitMagnitude auto M>
  static constexpr To scale(const From& value)
  {
    using U = _scaling_factor_type;
    if constexpr (is_integral(pow<-1>(M)) && !is_integral(M)) {
      constexpr U div = static_cast<U>(get_value<long double>(pow<-1>(M)));
      return static_cast<To>(detail::cast_if_integral<U>(value) / div);
    } else {
      constexpr U ratio = static_cast<U>(get_value<long double>(M));
      return static_cast<To>(detail::cast_if_integral<U>(value) * ratio);
    }
  }
};


template<detail::UsesFloatingPointScaling From>
struct scaling_traits<From, unspecified_rep> : scaling_traits<From, From> {};


template<detail::UsesFixedPointScaling From, detail::UsesFixedPointScaling To>
struct scaling_traits<From, To> {
  using _common_type = std::common_type_t<value_type_t<From>, value_type_t<To>>;
  static_assert(std::is_integral_v<_common_type>);

  // TODO: should we take possible overflow into account here? This would lead to this almost always resulting
  // in explicit conversions, except for small integral factors combined with a widening conversion.
  template<UnitMagnitude auto M>
  static constexpr bool implicitly_scalable = std::is_convertible_v<From, To> && is_integral(M);

  template<UnitMagnitude auto M>
  static constexpr To scale(const From& value)
  {
    if constexpr (is_integral(M)) {
      constexpr auto mul = get_value<_common_type>(M);
      return static_cast<To>(static_cast<value_type_t<From>>(value) * mul);
    } else if constexpr (is_integral(pow<-1>(M))) {
      constexpr auto div = get_value<_common_type>(pow<-1>(M));
      return static_cast<To>(static_cast<value_type_t<From>>(value) / div);
    } else {
      constexpr auto ratio = detail::fixed_point<_common_type>(get_value<long double>(M));
      return static_cast<To>(ratio.scale(static_cast<value_type_t<From>>(value)));
    }
  }
};

template<detail::UsesFixedPointScaling From>
struct scaling_traits<From, unspecified_rep> : scaling_traits<From, From> {};


MP_UNITS_EXPORT_BEGIN

// @brief approximate the result of the symbolic multiplication of @c from by @c scaling_factor, and represent it as an
// instance of @c To (chosen automatically if unspecified)
template<typename To = unspecified_rep, UnitMagnitude M, typename From>
  requires detail::HasScalingTraits<From, To>
constexpr To scale(M scaling_factor [[maybe_unused]], const From& value)
{
  static_assert(std::is_same_v<To, unspecified_rep> ||
                  std::is_convertible_v<decltype(scaling_traits<From, To>::template scale<M{}>(value)), To>,
                "scaling_traits<From,To>::scale must produce a value that is convertible to To");
  return scaling_traits<From, To>::template scale<M{}>(value);
}

MP_UNITS_EXPORT_END

}  // namespace mp_units
