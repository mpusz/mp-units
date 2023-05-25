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

#include <mp-units/bits/external/hacks.h>
#include <mp-units/bits/external/type_traits.h>
#include <limits>
#include <type_traits>

namespace mp_units {

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
inline constexpr bool treat_as_floating_point = std::is_floating_point_v<Rep>;

template<typename Rep>
  requires requires { typename Rep::value_type; }
inline constexpr bool treat_as_floating_point<Rep> = treat_as_floating_point<typename Rep::value_type>;

/**
 * @brief Specifies a type to have a scalar character
 *
 * A scalar is a physical quantity that has magnitude but no direction.
 */
template<typename Rep>
inline constexpr bool is_scalar = std::is_floating_point_v<Rep> || (std::is_integral_v<Rep> && !is_same_v<Rep, bool>);

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
 * inline constexpr bool mp_units::is_vector<T> = true;
 * @endcode
 */
template<typename Rep>
inline constexpr bool is_vector = false;

/**
 * @brief Specifies a type to have a tensor character
 *
 * Tensors can be used to describe more general physical quantities.
 *
 * A vector is a tensor of the first order and a scalar is a tensor of order zero.
 * Similarly to `is_vector` a partial specialization is needed in such cases.
 */
template<typename Rep>
inline constexpr bool is_tensor = false;

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
struct quantity_values {
  static constexpr Rep zero() noexcept
    requires std::constructible_from<Rep, int>
  {
    return Rep(0);
  }

  static constexpr Rep one() noexcept
    requires std::constructible_from<Rep, int>
  {
    return Rep(1);
  }

  static constexpr Rep min() noexcept
    requires requires {
      {
        std::numeric_limits<Rep>::lowest()
      } -> std::same_as<Rep>;
    }
  {
    return std::numeric_limits<Rep>::lowest();
  }

  static constexpr Rep max() noexcept
    requires requires {
      {
        std::numeric_limits<Rep>::max()
      } -> std::same_as<Rep>;
    }
  {
    return std::numeric_limits<Rep>::max();
  }
};

/**
 * @brief Provides support for external quantity-like types
 *
 * The type trait should provide the @c reference object, a type alias @c rep,
 * and a static member function @c number(T) that returns the raw value of the quantity.
 *
 * Usage example can be found in @c units/chrono.h header file.
 *
 * @tparam T the type to provide support for
 */
template<typename T>
struct quantity_like_traits;

/**
 * @brief Provides support for external quantity point-like types
 *
 * The type trait should provide nested @c reference and @c origin objects,
 * a type alias @c rep, and a static member function @c relative(T) that will return
 * the quantity-like value of the quantity point.
 *
 * Usage example can be found in @c units/chrono.h header file.
 *
 * @tparam T the type to provide support for
 */
template<typename T>
struct quantity_point_like_traits;

}  // namespace mp_units
