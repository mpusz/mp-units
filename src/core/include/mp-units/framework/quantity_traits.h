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
 * The unit of a quantity type, or of a type that `quantity_like_traits` or
 * `quantity_point_like_traits` describes
 *
 * Generic code uses it to produce a value in whatever type its caller uses, `std::chrono` or
 * mp-units:
 *
 * @code{.cpp}
 * // snippet-template: none
 * template<typename Duration>
 * Duration eta(QuantityOf<isq::distance> auto remaining, QuantityOf<isq::speed> auto speed)
 * {
 *   return Duration(value_cast<rep_for<Duration>, unit_for<Duration>>(remaining / speed));
 * }
 *
 * auto chrono_eta = eta<std::chrono::minutes>(isq::distance(50. * km), isq::speed(100. * km / h));
 * auto quantity_eta = eta<quantity<min, int>>(isq::distance(50. * km), isq::speed(100. * km / h));
 * @endcode
 *
 * It isn't a customization point. A type is described by specializing `quantity_like_traits` or
 * `quantity_point_like_traits`.
 *
 * @tparam T a quantity or quantity point type, or a type that either trait describes
 *
 * @see @ref reference_for for the quantity specification together with the unit
 * @see @ref rep_for for the representation type
 * @see [Converting External Quantity Types](docs:how_to_guides/integration/converting_external_quantity_types)
 */
template<typename T>
constexpr auto unit_for = detail::unit_for_impl<T>::value;

/**
 * The reference of a quantity type, or of a type that `quantity_like_traits` or
 * `quantity_point_like_traits` describes
 *
 * It carries the quantity specification as well as the unit, so a value built with it has the
 * caller's quantity specification, such as `isq::height`, and not only its unit:
 *
 * @code{.cpp}
 * // snippet-template: none
 * // a sensor reading, delivered as a raw number in the units the caller's type uses
 * template<typename Q>
 * Q from_register(std::int32_t raw)
 * {
 *   return Q(static_cast<rep_for<Q>>(raw) * reference_for<Q>);
 * }
 *
 * auto delay = from_register<std::chrono::milliseconds>(1500);         // 1500 ms
 * auto height = from_register<quantity<isq::height[mm], int>>(1500);   // 1500 mm
 * @endcode
 *
 * It isn't a customization point. A type is described by specializing `quantity_like_traits` or
 * `quantity_point_like_traits`.
 *
 * @tparam T a quantity or quantity point type, or a type that either trait describes
 *
 * @see @ref unit_for for the unit alone
 * @see @ref rep_for for the representation type
 * @see [Converting External Quantity Types](docs:how_to_guides/integration/converting_external_quantity_types)
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
