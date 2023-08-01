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

#include <mp-units/bits/quantity_spec_concepts.h>
#include <mp-units/bits/reference_concepts.h>
#include <mp-units/bits/representation_concepts.h>
#include <mp-units/customization_points.h>

namespace mp_units {

template<QuantitySpec auto Q>
struct absolute_point_origin;

namespace detail {

template<typename T>
inline constexpr bool is_quantity_point = false;

template<typename T>
inline constexpr bool is_specialization_of_absolute_point_origin = false;

template<auto Q>
inline constexpr bool is_specialization_of_absolute_point_origin<absolute_point_origin<Q>> = true;

template<auto Q>
void to_base_specialization_of_absolute_point_origin(const volatile absolute_point_origin<Q>*);

template<typename T>
inline constexpr bool is_derived_from_specialization_of_absolute_point_origin =
  requires(T* t) { to_base_specialization_of_absolute_point_origin(t); };

}  // namespace detail

/**
 * @brief A concept matching all quantity points in the library
 *
 * Satisfied by all types being a either specialization or derived from `quantity_point`
 */
template<typename T>
concept QuantityPoint = detail::is_quantity_point<T>;

/**
 * @brief A concept matching all quantity point origins in the library
 *
 * Satisfied by either quantity points or by all types derived from `absolute_point_origin` class template.
 */
template<typename T>
concept PointOrigin = QuantityPoint<T> || detail::is_derived_from_specialization_of_absolute_point_origin<T>;

/**
 * @brief A concept matching all quantity point origins for a specified quantity type in the library
 *
 * Satisfied by all quantity point origins that are defined using a provided quantity specification.
 */
template<typename T, auto Q>
concept PointOriginFor =
  PointOrigin<T> && QuantitySpec<std::remove_const_t<decltype(Q)>> && implicitly_convertible(Q, T::quantity_spec);

template<Reference auto R, PointOriginFor<get_quantity_spec(R)> auto PO,
         RepresentationOf<get_quantity_spec(R).character> Rep>
class quantity_point;

namespace detail {

template<auto R, auto PO, typename Rep>
void to_base_specialization_of_quantity_point(const volatile quantity_point<R, PO, Rep>*);

template<typename T>
inline constexpr bool is_derived_from_specialization_of_quantity_point =
  requires(T* t) { to_base_specialization_of_quantity_point(t); };

template<typename T>
  requires is_derived_from_specialization_of_quantity_point<T>
inline constexpr bool is_quantity_point<T> = true;

}  // namespace detail

/**
 * @brief A concept matching all quantity points with provided dimension or quantity spec
 *
 * Satisfied by all quantity points with a dimension/quantity_spec being the instantiation derived from
 * the provided dimension/quantity_spec type, or quantity points having the origin with the same
 * `absolute_point_origin`.
 */
template<typename QP, auto V>
concept QuantityPointOf =
  QuantityPoint<QP> &&
  (ReferenceOf<std::remove_const_t<decltype(QP::reference)>, V> ||
   (PointOrigin<std::remove_const_t<decltype(V)>> &&
    (std::same_as<std::remove_const_t<decltype(QP::absolute_point_origin)>, std::remove_const_t<decltype(V)>> ||
     (detail::is_specialization_of_absolute_point_origin<std::remove_const_t<decltype(QP::absolute_point_origin)>> &&
      detail::is_specialization_of_absolute_point_origin<std::remove_const_t<decltype(V)>> &&
      implicitly_convertible(QP::absolute_point_origin.quantity_spec, V.quantity_spec) &&
      !detail::DerivedFromQuantityKindSpecOf<QP::absolute_point_origin.quantity_spec, V.quantity_spec>))));

/**
 * @brief A concept matching all external quantity point like types
 *
 * Satisfied by all external types (not-defined in mp-units) that via a `quantity_point_like_traits` provide
 * all quantity_point-specific information.
 */
template<typename T>
concept QuantityPointLike = requires(T q) {
  quantity_point_like_traits<T>::reference;
  quantity_point_like_traits<T>::point_origin;
  typename quantity_point_like_traits<T>::rep;
  requires Reference<std::remove_const_t<decltype(quantity_point_like_traits<T>::reference)>>;
  requires PointOrigin<std::remove_const_t<decltype(quantity_point_like_traits<T>::point_origin)>>;
  requires RepresentationOf<typename quantity_point_like_traits<T>::rep,
                            get_quantity_spec(quantity_point_like_traits<T>::reference).character>;
  requires std::constructible_from<
    typename quantity_point<quantity_point_like_traits<T>::reference, quantity_point_like_traits<T>::point_origin,
                            typename quantity_point_like_traits<T>::rep>::quantity_type,
    decltype(quantity_point_like_traits<T>::relative(q))>;
};

}  // namespace mp_units
