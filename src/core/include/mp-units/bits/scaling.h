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

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#endif
#endif


namespace mp_units {

namespace detail {

template<std::floating_point A, std::floating_point B>
using minimal_floating_point_type =
  std::conditional_t<(std::numeric_limits<A>::digits <= std::numeric_limits<B>::digits), A, B>;

template<typename To, typename T>
constexpr auto cast_integral(const T& value)
{
  if constexpr (std::is_integral_v<value_type_t<T>>) {
    return static_cast<To>(value);
  } else {
    return value;
  }
}

template<typename T>
struct floating_point_scaling_factor_type {
  // fallback implementation for types with a `value_type` nested type
  using type =
    std::enable_if_t<!std::is_same_v<value_type_t<T>, T>, floating_point_scaling_factor_type<value_type_t<T>>>::type;
};

template<std::floating_point T>
struct floating_point_scaling_factor_type<T> : std::type_identity<T> {};

// try to choose the smallest standard floating-point type which can represent the integer exactly (has at least as many
// mantiassa bits as the integer is wide)
template<std::integral T>
struct floating_point_scaling_factor_type<T> : std::type_identity<min_digit_float_t<std::numeric_limits<T>::digits>> {};

template<typename T>
  requires requires { typename scaling_traits<T>::floating_point_scaling_factor_type; }
struct floating_point_scaling_factor_type<T> :
    std::type_identity<typename scaling_traits<T>::floating_point_scaling_factor_type> {};


template<Magnitude auto M>
struct floating_point_scaling_impl {
  template<typename T>
  static constexpr T ratio = [] {
    using U = long double;
    return static_cast<T>(_get_value<U>(M));
  }();

  template<typename To, typename From>
  static constexpr To scale(const From& value)
  {
    using U = minimal_floating_point_type<typename floating_point_scaling_factor_type<To>::type,
                                          typename floating_point_scaling_factor_type<From>::type>;
    if constexpr (_is_integral(M)) {
      return static_cast<To>(cast_integral<U>(value) * _get_value<U>(M));
    } else if constexpr (_is_integral(_pow<-1>(M))) {
      return static_cast<To>(cast_integral<U>(value) / _get_value<U>(_pow<-1>(M)));
    } else {
      return static_cast<To>(cast_integral<U>(value) * ratio<U>);
    }
  }
};

/**
 * @brief Default implementation of `scaling_traits` for "floating-point like" types
 *
 * This class implements scaling by either multiplying or dividing the value with
 * a floating-point representation of the scaling factor; the floating-point representation
 * is chosen such that it is of comparable precision as the representation type,
 *
 * @note This is a low-level facility. Neither the `From`, nor the `To` types of the scaling
 * operation are actually constrained to be floating-point or even "floating-point like" types.
 * All it represents is the scaling operation as implemented by multiplication with a floating-point
 * representation of the scaling factor. This is also used whenever simultaneously scaling and
 * converting between integer and floating-point types.
 *
 * @tparam Rep Representation type
 */
template<typename Rep>
struct floating_point_scaling_traits {
  template<Magnitude M, typename From>
  static constexpr Rep scale_from(M, const From& value)
  {
    return floating_point_scaling_impl<M{}>::template scale<Rep>(value);
  }

  template<Magnitude M>
  static constexpr auto scale(M, const Rep& value)
  {
    // for standard floating-point types, the result respresentation is always the same
    return floating_point_scaling_impl<M{}>::template scale<Rep>(value);
  }
};


template<Magnitude auto M>
struct fixed_point_scaling_impl {
  template<std::integral T>
  static constexpr auto ratio = [] {
    using U = long double;
    return detail::fixed_point<T>(_get_value<U>(M));
  }();

  template<typename To, typename From>
    requires std::integral<value_type_t<To>> && std::integral<value_type_t<From>>
  static constexpr To scale(const From& value)
  {
    using U = std::common_type_t<value_type_t<From>, value_type_t<To>>;
    if constexpr (_is_integral(M)) {
      return static_cast<To>(static_cast<value_type_t<From>>(value) * _get_value<U>(M));
    } else if constexpr (_is_integral(_pow<-1>(M))) {
      return static_cast<To>(static_cast<value_type_t<From>>(value) / _get_value<U>(_pow<-1>(M)));
    } else {
      return static_cast<To>(ratio<U>.scale(static_cast<value_type_t<From>>(value)));
    }
  }
};


template<typename Rep>
struct fixed_point_scaling_traits {
  template<Magnitude M, typename From>
  static constexpr Rep scale_from(M, const From& value)
  {
    return fixed_point_scaling_impl<M{}>::template scale<Rep>(value);
  }

  template<Magnitude M>
  static constexpr auto scale(M, const Rep& value)
  {
    // for standard integer types, the result respresentation is always the same
    return fixed_point_scaling_impl<M{}>::template scale<Rep>(value);
  }
};


template<typename T, typename Other = T>
inline constexpr auto select_scaling_traits = [] {
  if constexpr (requires {
                  // we only check if the traits class is complete, not it's members; we do not want to fall-back
                  // depending on the argument types
                  { sizeof(mp_units::scaling_traits<T>) } -> std::integral;
                }) {
    // traits class is defined; use that
    return mp_units::scaling_traits<T>{};
  } else {
    // undefined traits class; fall-back to default handling
    if constexpr (mp_units::treat_as_floating_point<T> || mp_units::treat_as_floating_point<Other>) {
      return floating_point_scaling_traits<T>{};
    } else if constexpr (std::is_integral_v<T> ||
                         (std::is_integral_v<value_type_t<T>> && std::convertible_to<value_type_t<T>, T> &&
                          std::convertible_to<T, value_type_t<T>>)) {
      return fixed_point_scaling_traits<value_type_t<T>>{};
    } else {
      // placeholder to report failure
      return std::false_type{};
    }
  }
}();

template<typename T, typename Other = T>
concept HasScalingTraits = !std::convertible_to<decltype(select_scaling_traits<T, Other>), std::false_type>;

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

template<typename To, Magnitude M, typename From>
  requires detail::HasScalingTraits<To, From> ||
           requires(const From& value) { value.scale(std::type_identity<To>{}, M{}); }
constexpr To scale(std::type_identity<To> to_type, M scaling_factor, const From& value)
{
  if constexpr (requires {
                  { value.scale(to_type, scaling_factor) } -> std::convertible_to<To>;
                }) {
    return value.scale(to_type, scaling_factor);
  } else {
    return detail::select_scaling_traits<To, From>.scale_from(scaling_factor, value);
  }
}

template<Magnitude M, typename From>
  requires detail::HasScalingTraits<From> || requires(const From& value) { value.scale(M{}); }
constexpr auto scale(M scaling_factor, const From& value)
{
  if constexpr (requires { value.scale(scaling_factor); }) {
    return value.scale(scaling_factor);
  } else {
    return detail::select_scaling_traits<From>.scale(scaling_factor, value);
  }
}


MP_UNITS_EXPORT_END

}  // namespace mp_units
