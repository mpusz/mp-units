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
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/quantity_concepts.h>
#include <mp-units/framework/quantity_point_concepts.h>

namespace mp_units {

namespace detail {

template<typename T>
struct unit_for_impl {};

template<typename T>
  requires Quantity<T> || QuantityPoint<T>
struct unit_for_impl<T> {
  static constexpr auto value = T::unit;
};

template<typename T>
  requires(!Quantity<T> && !QuantityPoint<T>) && requires { quantity_like_traits<T>::reference; }
struct unit_for_impl<T> {
  static constexpr auto value = get_unit(quantity_like_traits<T>::reference);
};

template<typename T>
  requires(!Quantity<T> && !QuantityPoint<T>) && requires { quantity_point_like_traits<T>::reference; }
struct unit_for_impl<T> {
  static constexpr auto value = get_unit(quantity_point_like_traits<T>::reference);
};

template<typename T>
struct reference_for_impl {};

template<typename T>
  requires Quantity<T> || QuantityPoint<T>
struct reference_for_impl<T> {
  static constexpr auto value = T::reference;
};

template<typename T>
  requires(!Quantity<T> && !QuantityPoint<T>) && requires { quantity_like_traits<T>::reference; }
struct reference_for_impl<T> {
  static constexpr auto value = quantity_like_traits<T>::reference;
};

template<typename T>
  requires(!Quantity<T> && !QuantityPoint<T>) && requires { quantity_point_like_traits<T>::reference; }
struct reference_for_impl<T> {
  static constexpr auto value = quantity_point_like_traits<T>::reference;
};

template<typename T>
struct rep_for_impl {};

template<typename T>
  requires Quantity<T> || QuantityPoint<T>
struct rep_for_impl<T> {
  using type = typename T::rep;
};

template<typename T>
  requires(!Quantity<T> && !QuantityPoint<T>) && requires { typename quantity_like_traits<T>::rep; }
struct rep_for_impl<T> {
  using type = typename quantity_like_traits<T>::rep;
};

template<typename T>
  requires(!Quantity<T> && !QuantityPoint<T>) && requires { typename quantity_point_like_traits<T>::rep; }
struct rep_for_impl<T> {
  using type = typename quantity_point_like_traits<T>::rep;
};

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

/**
 * Public helper that yields the unit associated with a quantity-like type.
 *
 * Derived from the type's own `::unit` member (for `quantity` / `quantity_point`) or from
 * `quantity_like_traits<T>` / `quantity_point_like_traits<T>` for external types.
 *
 * This is **not** a customization point. Users teach the library about external types by
 * specializing `quantity_like_traits` / `quantity_point_like_traits`, not this helper.
 *
 * @tparam T a quantity-like or quantity-point-like type
 */
template<typename T>
constexpr auto unit_for = detail::unit_for_impl<T>::value;

/**
 * Public helper that yields the reference (unit + quantity spec) associated with a
 *        quantity-like type.
 *
 * Derived from the type's own `::reference` member (for `quantity` / `quantity_point`) or
 * from `quantity_like_traits<T>` / `quantity_point_like_traits<T>` for external types.
 *
 * This is **not** a customization point. Users teach the library about external types by
 * specializing `quantity_like_traits` / `quantity_point_like_traits`, not this helper.
 *
 * @tparam T a quantity-like or quantity-point-like type
 */
template<typename T>
constexpr auto reference_for = detail::reference_for_impl<T>::value;

/**
 * Public alias that yields the representation type of a quantity-like type.
 *
 * Derived from the type's own `::rep` member (for `quantity` / `quantity_point`) or from
 * `quantity_like_traits<T>::rep` / `quantity_point_like_traits<T>::rep` for external types.
 *
 * This is **not** a customization point. Users teach the library about external types by
 * specializing `quantity_like_traits` / `quantity_point_like_traits`, not this helper.
 *
 * @tparam T a quantity-like or quantity-point-like type
 */
template<typename T>
using rep_for = typename detail::rep_for_impl<T>::type;

MP_UNITS_EXPORT_END

}  // namespace mp_units
