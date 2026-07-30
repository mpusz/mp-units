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
#endif  // MP_UNITS_IMPORT_STD
#endif  // MP_UNITS_IN_MODULE_INTERFACE

namespace mp_units {

MP_UNITS_EXPORT_BEGIN

/**
 * @brief Rounding policies for potentially lossy value conversions
 *
 * A conversion without a policy (e.g. `q.in(u)`) is only available when the framework
 * considers it value-preserving (following the `std::chrono` convention, a floating-point
 * destination is always considered such). For the conversions considered truncating (e.g.
 * an integral representation scaled by a non-integral factor, or a narrowing of the
 * representation type), the user has to explicitly state what should happen to the
 * information that does not fit by passing a rounding policy as the last function argument:
 *
 * @code{.cpp}
 * quantity q1 = (1234 * m).in(km, truncated);          // 1 km
 * quantity q2 = (1567 * m).in(km, rounded);            // 2 km
 * quantity q3 = (-1500 * m).in(km, rounded_down);      // -2 km
 * quantity q4 = (1.23 * s).in<int>(ms, rounded_up);    // 1231 ms
 * @endcode
 *
 * The policy specifies how to round the exact conversion result to a value representable
 * in the destination type:
 * - `truncated`    - the semantics of `static_cast` and `std::chrono::duration_cast`:
 *                    rounds towards zero for an integral destination; a floating-point
 *                    destination converts to the nearest representable value,
 * - `rounded`      - rounds to the nearest representable value, to even in halfway cases
 *                    (the semantics of `std::chrono::round`),
 * - `rounded_down` - rounds towards negative infinity (the semantics of `std::chrono::floor`),
 * - `rounded_up`   - rounds towards positive infinity (the semantics of `std::chrono::ceil`).
 *
 * A floating-point destination represents every conversion result with a rounding error of
 * at most half ULP (`rounded` semantics), and the directed modes cannot be delivered for it,
 * which is why it accepts only the `truncated` and `rounded` policies.
 *
 * Note that `mp_units::floor/ceil/round<U>(q)` perform a different operation: they round to
 * an integral multiple of the target unit even when the representation could store the exact
 * result (e.g. for a floating-point representation). The policies never adjust a value that
 * the destination represents exactly. Both coincide for integral representations.
 */
struct truncated_t {
  explicit truncated_t() = default;
};

struct rounded_t {
  explicit rounded_t() = default;
};

struct rounded_down_t {
  explicit rounded_down_t() = default;
};

struct rounded_up_t {
  explicit rounded_up_t() = default;
};

inline constexpr truncated_t truncated{};
inline constexpr rounded_t rounded{};
inline constexpr rounded_down_t rounded_down{};
inline constexpr rounded_up_t rounded_up{};

/**
 * @brief A concept matching all rounding policies
 *
 * Satisfied only by the types of `truncated`, `rounded`, `rounded_down`, and `rounded_up`.
 */
template<typename T>
concept RoundingPolicy = contains<T, truncated_t, rounded_t, rounded_down_t, rounded_up_t>();

MP_UNITS_EXPORT_END

namespace detail {

// Internal representation of the rounding policies used to parameterize the conversion
// engine (`sudo_cast`, `scale`) without instantiating it for every policy tag type.
enum class rounding_mode : std::int8_t { truncated, rounded, rounded_down, rounded_up };

template<RoundingPolicy Policy>
constexpr rounding_mode rounding_mode_of = is_same_v<Policy, truncated_t>      ? rounding_mode::truncated
                                           : is_same_v<Policy, rounded_t>      ? rounding_mode::rounded
                                           : is_same_v<Policy, rounded_down_t> ? rounding_mode::rounded_down
                                                                               : rounding_mode::rounded_up;

}  // namespace detail

}  // namespace mp_units
