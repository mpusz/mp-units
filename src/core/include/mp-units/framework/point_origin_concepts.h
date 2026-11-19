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
#include <mp-units/compat_macros.h>
#include <mp-units/framework/symbolic_expression.h>

namespace mp_units {

namespace detail {

struct point_origin_interface;

template<typename T>
constexpr bool is_specialization_of_absolute_point_origin = false;

template<typename T>
constexpr bool is_specialization_of_relative_point_origin = false;

// Every point origin. Exposition-only: an interface needs `PointOriginFor`, which pins the quantity,
// or the library's own checks that two origins share an absolute one, never this concept alone.
//
// `is_base_of_v` rather than `derived_from`: one constraint atom instead of two, evaluated per type; the base is
// always public and unambiguous, so the convertibility atom `derived_from` adds can never fail here.
template<typename T>
concept PointOrigin = std::is_base_of_v<point_origin_interface, T> && SymbolicConstant<T> &&
                      !is_specialization_of_absolute_point_origin<std::remove_const_t<T>> &&
                      !is_specialization_of_relative_point_origin<std::remove_const_t<T>>;

}  // namespace detail

/**
 * A concept matching every point origin
 */
MP_UNITS_EXPORT template<typename T>
concept PointOrigin MP_UNITS_DEPRECATED_CONCEPT("2.6.0: Use `PointOriginFor` or pass the origin as a value") =
  detail::PointOrigin<T>;

}  // namespace mp_units
