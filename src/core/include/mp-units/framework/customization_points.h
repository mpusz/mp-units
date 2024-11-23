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
 * that values fo this type should be treated by the library as a floating-point ones
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
 * @brief Specifies a type to have a scalar character
 *
 * A scalar is a physical quantity that has magnitude but no direction.
 */
template<typename Rep>
constexpr bool is_scalar = std::is_floating_point_v<Rep> || (std::is_integral_v<Rep> && !is_same_v<Rep, bool>);

/**
 * @brief Specifies a type to have a complex character
 *
 * A complex is a physical quantity that has a complex representation type.
 */
template<typename Rep>
constexpr bool is_complex = false;

/**
 * @brief Specifies a type to have a vector character
 *
 * Vectors are physical quantities that possess both magnitude and direction
 * and whose operations obey the axioms of a vector space.
 *
 * In specific cases a scalar can represent a vector with the default direction.
 * If that is the intent, a user should provide a partial specialization:
 *
 * @code{.cpp}
 * template<class T>
 *   requires mp_units::is_scalar<T>
 * constexpr bool mp_units::is_vector<T> = true;
 * @endcode
 */
template<typename Rep>
constexpr bool is_vector = false;

/**
 * @brief Specifies a type to have a tensor character
 *
 * Tensors can be used to describe more general physical quantities.
 *
 * A vector is a tensor of the first order and a scalar is a tensor of order zero.
 * Similarly to `is_vector` a partial specialization is needed in such cases.
 */
template<typename Rep>
constexpr bool is_tensor = false;

MP_UNITS_EXPORT_END

namespace detail::norm_impl {

void norm() = delete;  // poison pill

struct norm_t {
  template<typename T>
  [[nodiscard]] constexpr auto operator()(const T& vec) const
  {
    if constexpr (requires { vec.norm(); })
      return vec.norm();
    else if constexpr (requires { norm(vec); })
      return norm(vec);
    else if constexpr (requires { vec.magnitude(); })
      return vec.magnitude();
    else if constexpr (requires { magnitude(vec); })
      return magnitude(vec);
    // TODO Is it a good idea to enable fundamental types to represent vector quantities?
    // else if constexpr (is_scalar<T>)
    //   return std::abs(vec);
  }
};

}  // namespace detail::norm_impl

inline namespace cpo {

MP_UNITS_EXPORT inline constexpr ::mp_units::detail::norm_impl::norm_t norm;

}

namespace detail::real_impl {

void real() = delete;  // poison pill

struct real_t {
  template<typename T>
  [[nodiscard]] constexpr auto operator()(const T& clx) const
  {
    if constexpr (requires { clx.real(); })
      return clx.real();
    else if constexpr (requires { real(clx); })
      return real(clx);
  }
};

}  // namespace detail::real_impl

inline namespace cpo {

MP_UNITS_EXPORT inline constexpr ::mp_units::detail::real_impl::real_t real;

}

namespace detail::imag_impl {

void imag() = delete;  // poison pill

struct imag_t {
  template<typename T>
  [[nodiscard]] constexpr auto operator()(const T& clx) const
  {
    if constexpr (requires { clx.imag(); })
      return clx.imag();
    else if constexpr (requires { imag(clx); })
      return imag(clx);
  }
};

}  // namespace detail::imag_impl

inline namespace cpo {

MP_UNITS_EXPORT inline constexpr ::mp_units::detail::imag_impl::imag_t imag;

}

namespace detail::modulus_impl {

void modulus() = delete;  // poison pill

struct modulus_t {
  template<typename T>
  [[nodiscard]] constexpr auto operator()(const T& clx) const
  {
    if constexpr (requires { clx.modulus(); })
      return clx.modulus();
    else if constexpr (requires { modulus(clx); })
      return modulus(clx);
    // `std` made a precedence of using `abs` for modulo on `std::complex`
    else if constexpr (requires { abs(clx); })
      return abs(clx);
  }
};

}  // namespace detail::modulus_impl

inline namespace cpo {

MP_UNITS_EXPORT inline constexpr ::mp_units::detail::modulus_impl::modulus_t modulus;

}

MP_UNITS_EXPORT_BEGIN

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
struct quantity_values : std::chrono::duration_values<Rep> {
#else
struct quantity_values {
  static constexpr Rep zero() noexcept
    requires std::constructible_from<Rep, int>
  {
    return Rep(0);
  }

  static constexpr Rep min() noexcept
    requires requires {
      { std::numeric_limits<Rep>::lowest() } -> std::same_as<Rep>;
    }
  {
    return std::numeric_limits<Rep>::lowest();
  }

  static constexpr Rep max() noexcept
    requires requires {
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
