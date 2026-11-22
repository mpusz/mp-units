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

#include <mp-units/bits/module_macros.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#include <type_traits>
#endif
#endif

namespace mp_units::utility {

/**
 * The type of @ref unspecified
 */
MP_UNITS_EXPORT struct unspecified_t {};

/**
 * The default of a customization point that has no usable default
 *
 * A variable template can't be deleted the way a function can, so a customization point that has
 * to be specialized before use starts as `unspecified`, and a specialization replaces it. Code
 * then accepts only the types it was specialized for:
 *
 * @code{.cpp}
 * // snippet-template: none
 * // the unit a sensor reports in, known only for the sensors registered for it
 * template<typename Sensor>
 * constexpr auto native_unit = utility::unspecified;
 *
 * template<>
 * inline constexpr auto native_unit<lidar> = mm;
 *
 * template<typename Sensor>
 *   requires utility::specified<decltype(native_unit<Sensor>)>
 * quantity<native_unit<Sensor>, int> read(const Sensor& sensor);
 * @endcode
 *
 * The library's own customization points, such as @ref tensor_order and @ref radian_of, start
 * this way.
 *
 * @see @ref specified for testing whether a customization point has a value
 */
MP_UNITS_EXPORT inline constexpr unspecified_t unspecified{};

/// Satisfied when a customization point has been given a value (i.e. is not `unspecified`).
MP_UNITS_EXPORT template<typename T>
concept specified = !std::same_as<std::remove_cvref_t<T>, unspecified_t>;

}  // namespace mp_units::utility
