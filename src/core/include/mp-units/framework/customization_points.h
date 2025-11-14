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
#include <mp-units/bits/module_macros.h>
#include <mp-units/ext/type_traits.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#include <limits>
#include <type_traits>
#if MP_UNITS_HOSTED
#include <chrono>
#endif
#endif
#endif

namespace mp_units {

MP_UNITS_EXPORT_BEGIN

/**
 * @brief Specifies if a value of a type should be treated as a floating-point value
 *
 * This type trait should be specialized for a custom representation type to specify
 * that values of this type should be treated by the library as a floating-point ones
 * which will enable implicit conversions between quantities.
 *
 * @tparam Rep a representation type for which a type trait is defined
 */
template<typename Rep>
constexpr bool treat_as_floating_point =
#if MP_UNITS_HOSTED
  std::chrono::treat_as_floating_point_v<value_type_t<Rep>>;
#else
  std::is_floating_point_v<value_type_t<Rep>>;
#endif

/**
 * @brief Specifies if a specific conversion between two types preserves the value
 *
 * This type trait should be specialized for a custom representation types to specify
 * weather the conversion from the source type to the destination type preserves the value
 * or not. Value-truncating conversions should be forced by the user with explicit casts.
 *
 * @tparam From a source representation type
 * @tparam To a destination representation type
 */
template<typename From, typename To>
constexpr bool is_value_preserving = treat_as_floating_point<To> || !treat_as_floating_point<From>;

template<typename Rep>
[[deprecated("2.5.0: `is_scalar` is no longer necessary and can simply be removed")]]
constexpr bool is_scalar = false;

template<typename Rep>
[[deprecated("2.5.0: `is_complex` is no longer necessary and can simply be removed")]]
constexpr bool is_complex = false;

template<typename Rep>
[[deprecated("2.5.0: `is_vector` is no longer necessary and can simply be removed")]]
constexpr bool is_vector = false;

template<typename Rep>
[[deprecated("2.5.0: `is_tensor` is no longer necessary and can simply be removed")]]
constexpr bool is_tensor = false;

/**
 * @brief A type trait that defines zero, one, min, and max for a representation type
 *
 * The zero, one, min, and max member functions in mp_units::quantity forward their work to
 * these methods. This type can be specialized if the representation Rep requires a specific
 * implementation to return these quantity objects.
 *
 * @tparam Rep a representation type for which a type trait is defined
 */
template<typename Rep>
#if MP_UNITS_HOSTED
struct representation_values : std::chrono::duration_values<Rep> {
#else
struct representation_values {
  static constexpr Rep zero() noexcept
    requires std::constructible_from<Rep, int>
  {
    return Rep(0);
  }

  static constexpr Rep min() noexcept
    requires std::numeric_limits<Rep>::is_specialized && requires {
      { std::numeric_limits<Rep>::lowest() } -> std::same_as<Rep>;
    }
  {
    return std::numeric_limits<Rep>::lowest();
  }

  static constexpr Rep max() noexcept
    requires std::numeric_limits<Rep>::is_specialized && requires {
      { std::numeric_limits<Rep>::max() } -> std::same_as<Rep>;
    }
  {
    return std::numeric_limits<Rep>::max();
  }
#endif

  static constexpr Rep one() noexcept
    requires std::constructible_from<Rep, int>
  {
    return Rep(1);
  }
};

template<typename Rep>
using quantity_values [[deprecated("2.5.0: Use `representation_values` instead")]] = representation_values<Rep>;


/**
 * @brief A type used in @c scaling_traits to indicate an unspecified @c To type.
 */
struct unspecified_rep {};

/**
 * @brief A type trait that defines the behavior of scaling a value using a magnitude
 *
 * Whereas C++ numeric types usually represent a (fixed) subset of the real numbers
 * (or another vector-space over the field of the real numbers),
 * the magnitude concept fundamentally can represent any real number.
 * Thus, in general, the result of a scaling operation is not exactly representable,
 * and some form of approximation may be needed. That approximation is not
 * part of the semantics of a physical quantitiy, but of its representation
 * in C++. Therefore, the approximation semantics are dictatet by the
 * representation type, which can be customised for user-types through
 * this type-trait.
 *
 * In the following, $\mathcal{V}$ shall denote the vector-space represented by all representation
 * types involved in the following discussion.
 *
 * A specialisation @c scaling_traits<From,ToSpec> shall provide the following members:
 *  - `template <Magnitude auto M> static constexpr auto scale(const From &value)`:
 *    Given an element of $\mathcal{V}$ represented by @c value and, a real number represented by @c M,
 *    return a value representing `M * value`, another element of $\mathcal{V}$.
 *    Unless @c ToSpec is the type @c unspecified_rep, the result type is required to be convetrible to @c ToSpec.
 *    When @c ToSpec is the type @c unspecified_rep, the implemenation may choose the best
 *    representation availabe.
 *    Because the scaling factor @c M encodes the represented real value in its type,
 *    that representation may even depend on the actual scaling factor.
 *  - `template <Magnitude auto M> static constexpr bool implicitly_scalable = ...`:
 *    When true, the scaling is to be considered "safe", and may be used in implicit conversions.
 *
 * @tparam From a representation type whose value is being scaled
 * @tparam To a representation type in which the result shall be represented, or @c unspecified_rep, indicating
 *    the implementation is free to chose a representation.
 */
template<typename From, typename To>
struct scaling_traits;

/**
 * @brief Provides support for external quantity-like types
 *
 * The type trait should provide the @c reference object, a type alias @c rep,
 * and static member functions @c to_numerical_value(T) that returns the raw value
 * of the quantity and @c from_numerical_value(rep) that returns @c T from @c rep.
 *
 * If the following expression is @c true, the specified conversion will be explicit:
 * - @c explicit_import for the conversion from @c T to a @c quantity type,
 * - @c explicit_export for the conversion from a @c quantity type to @c T.
 *
 * Usage example can be found in @c mp-units/systems/si/chrono.h header file.
 *
 * @tparam T the type to provide support for
 */
template<typename T>
struct quantity_like_traits;

/**
 * @brief Provides support for external quantity point-like types
 *
 * The type trait should provide nested @c reference and @c origin objects,
 * a type alias @c rep, and static member functions @c to_numerical_value(T) that
 * returns the raw value of the the quantity being the offset of the point from the
 * origin and @c from_numerical_value(rep) that returns @c T formed this raw value.
 *
 * If the following expression is @c true, the specified conversion will be explicit:
 * - @c explicit_import for the conversion from @c T to a @c quantity_point type,
 * - @c explicit_export for the conversion from a @c quantity_point type to @c T.
 *
 * Usage example can be found in @c mp-units/systems/si/chrono.h header file.
 *
 * @tparam T the type to provide support for
 */
template<typename T>
struct quantity_point_like_traits;

MP_UNITS_EXPORT_END

}  // namespace mp_units
