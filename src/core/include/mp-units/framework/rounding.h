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
#include <cstdint>
#include <type_traits>
#endif  // MP_UNITS_IMPORT_STD
#endif  // MP_UNITS_IN_MODULE_INTERFACE

namespace mp_units {

MP_UNITS_EXPORT_BEGIN

/**
 * The type of @ref truncated
 */
struct truncated_t {
  explicit truncated_t() = default;
};

/**
 * The type of @ref rounded
 */
struct rounded_t {
  explicit rounded_t() = default;
};

/**
 * The type of @ref rounded_down
 */
struct rounded_down_t {
  explicit rounded_down_t() = default;
};

/**
 * The type of @ref rounded_up
 */
struct rounded_up_t {
  explicit rounded_up_t() = default;
};

/**
 * Rounding policy with the semantics of `static_cast` and `std::chrono::duration_cast`
 *
 * Rounds towards zero for an integral destination. A floating-point destination gets what its
 * own arithmetic produces, which rounds to nearest. A vector representation converting to an
 * integral type accepts only this policy.
 *
 * @code{.cpp}
 * quantity q1 = (1234 * m).in(km, truncated);   // 1 km
 * quantity q2 = (-1234 * m).in(km, truncated);  // -1 km
 * @endcode
 *
 * @see @ref rounded for rounding to the nearest value
 * @see @ref rounded_down for rounding towards negative infinity
 * @see @ref rounded_up for rounding towards positive infinity
 * @see [Value Conversions](docs:users_guide/framework_basics/value_conversions#value-truncating-conversions) for when a
 * policy is needed
 */
inline constexpr truncated_t truncated{};

/**
 * Rounding policy that rounds to the nearest value, and to even in halfway cases
 *
 * It has the semantics of `std::chrono::round`.
 *
 * @code{.cpp}
 * quantity q1 = (1567 * m).in(km, rounded);  // 2 km
 * quantity q2 = (2500 * m).in(km, rounded);  // 2 km
 * @endcode
 *
 * @see @ref truncated for the semantics of `static_cast`
 * @see @ref rounded_down for rounding towards negative infinity
 * @see @ref rounded_up for rounding towards positive infinity
 * @see [Value Conversions](docs:users_guide/framework_basics/value_conversions#value-truncating-conversions) for when a
 * policy is needed
 */
inline constexpr rounded_t rounded{};

/**
 * Rounding policy that rounds towards negative infinity
 *
 * It has the semantics of `std::chrono::floor`. A conversion to a floating-point
 * representation type doesn't accept it.
 *
 * @code{.cpp}
 * quantity distance = (-1234 * m).in(km, rounded_down);  // -2 km
 * @endcode
 *
 * @see @ref truncated for the semantics of `static_cast`
 * @see @ref rounded for rounding to the nearest value
 * @see @ref rounded_up for rounding towards positive infinity
 * @see [Value Conversions](docs:users_guide/framework_basics/value_conversions#value-truncating-conversions) for when a
 * policy is needed
 */
inline constexpr rounded_down_t rounded_down{};

/**
 * Rounding policy that rounds towards positive infinity
 *
 * It has the semantics of `std::chrono::ceil`. A conversion to a floating-point
 * representation type doesn't accept it.
 *
 * @code{.cpp}
 * quantity duration = (1.2345 * s).in<int>(ms, rounded_up);  // 1235 ms
 * @endcode
 *
 * @see @ref truncated for the semantics of `static_cast`
 * @see @ref rounded for rounding to the nearest value
 * @see @ref rounded_down for rounding towards negative infinity
 * @see [Value Conversions](docs:users_guide/framework_basics/value_conversions#value-truncating-conversions) for when a
 * policy is needed
 */
inline constexpr rounded_up_t rounded_up{};

MP_UNITS_EXPORT_END

namespace detail {

// The rounding policies a value conversion accepts. Exposition-only, as in P3045: a user passes one
// of the four policy objects and never needs to name the concept.
template<typename T>
concept RoundingPolicy = contains<T, truncated_t, rounded_t, rounded_down_t, rounded_up_t>();

// Internal representation of the rounding policies used to parameterize the conversion
// engine (`sudo_cast`, `scale_by_magnitude`) without instantiating it for every policy tag type.
enum class rounding_mode : std::int8_t { truncated, rounded, rounded_down, rounded_up };

template<RoundingPolicy Policy>
constexpr rounding_mode rounding_mode_of = std::is_same_v<Policy, truncated_t>      ? rounding_mode::truncated
                                           : std::is_same_v<Policy, rounded_t>      ? rounding_mode::rounded
                                           : std::is_same_v<Policy, rounded_down_t> ? rounding_mode::rounded_down
                                                                                    : rounding_mode::rounded_up;

}  // namespace detail

}  // namespace mp_units
