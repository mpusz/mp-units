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

#include <mp-units/bits/quantity_concepts.h>
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

template<typename T>
concept AbsolutePointOrigin =
  is_derived_from_specialization_of_absolute_point_origin<T> && !is_specialization_of_absolute_point_origin<T>;

}  // namespace detail

/**
 * @brief A concept matching all quantity points in the library
 *
 * Satisfied by all types being a either specialization or derived from `quantity_point`
 */
template<typename T>
concept QuantityPoint = detail::is_quantity_point<T>;

template<QuantityPoint auto QP>
struct relative_point_origin;

namespace detail {

template<typename T>
inline constexpr bool is_specialization_of_relative_point_origin = false;

template<auto QP>
inline constexpr bool is_specialization_of_relative_point_origin<relative_point_origin<QP>> = true;

template<auto QP>
void to_base_specialization_of_relative_point_origin(const volatile relative_point_origin<QP>*);

template<typename T>
inline constexpr bool is_derived_from_specialization_of_relative_point_origin =
  requires(T* t) { to_base_specialization_of_relative_point_origin(t); };

template<typename T>
concept RelativePointOrigin =
  is_derived_from_specialization_of_relative_point_origin<T> && !is_specialization_of_relative_point_origin<T>;

}  // namespace detail

/**
 * @brief A concept matching all quantity point origins in the library
 *
 * Satisfied by either quantity points or by all types derived from `absolute_point_origin` class template.
 */
template<typename T>
concept PointOrigin = detail::AbsolutePointOrigin<T> || detail::RelativePointOrigin<T>;

/**
 * @brief A concept matching all quantity point origins for a specified quantity type in the library
 *
 * Satisfied by all quantity point origins that are defined using a provided quantity specification.
 */
template<typename T, auto QS>
concept PointOriginFor = PointOrigin<T> && QuantitySpecOf<std::remove_const_t<decltype(QS)>, T::quantity_spec>;

template<Reference auto R, PointOriginFor<get_quantity_spec(R)> auto PO,
         RepresentationOf<get_quantity_spec(R).character> Rep>
class quantity_point;

#if defined MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17
template<auto PO, typename Q>
#else
template<PointOrigin auto PO, Quantity Q>
  requires ReferenceOf<std::remove_const_t<decltype(Q::reference)>, PO.quantity_spec>
#endif
[[nodiscard]] constexpr quantity_point<Q::reference, PO, typename Q::rep> make_quantity_point(Q&& q);

namespace detail {

template<auto R, auto PO, typename Rep>
void to_base_specialization_of_quantity_point(const volatile quantity_point<R, PO, Rep>*);

template<typename T>
inline constexpr bool is_derived_from_specialization_of_quantity_point =
  requires(T* t) { to_base_specialization_of_quantity_point(t); };

template<typename T>
  requires is_derived_from_specialization_of_quantity_point<T>
inline constexpr bool is_quantity_point<T> = true;

template<PointOrigin PO1, PointOrigin PO2>
[[nodiscard]] consteval bool same_absolute_point_origins(PO1, PO2)
{
  if constexpr (is_same_v<PO1, PO2>)
    return true;
  else if constexpr (is_derived_from_specialization_of_relative_point_origin<PO1> &&
                     is_derived_from_specialization_of_relative_point_origin<PO2>)
    return is_same_v<std::remove_const_t<decltype(PO1::absolute_point_origin)>,
                     std::remove_const_t<decltype(PO2::absolute_point_origin)>>;
  else if constexpr (is_derived_from_specialization_of_relative_point_origin<PO1>)
    return is_same_v<std::remove_const_t<decltype(PO1::absolute_point_origin)>, PO2>;
  else if constexpr (is_derived_from_specialization_of_relative_point_origin<PO2>)
    return is_same_v<PO1, std::remove_const_t<decltype(PO2::absolute_point_origin)>>;
  else
    return false;
}

template<typename T, auto V>
concept SameAbsolutePointOriginAs =
  PointOrigin<T> && PointOrigin<std::remove_const_t<decltype(V)>> && same_absolute_point_origins(T{}, V);

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
  QuantityPoint<QP> && (ReferenceOf<std::remove_const_t<decltype(QP::reference)>, V> ||
                        detail::SameAbsolutePointOriginAs<std::remove_const_t<decltype(QP::absolute_point_origin)>, V>);

/**
 * @brief A concept matching all external quantity point like types
 *
 * Satisfied by all external types (not-defined in mp-units) that via a `quantity_point_like_traits` provide
 * all quantity_point-specific information.
 */
template<typename T>
concept QuantityPointLike = requires(T qp) {
  quantity_point_like_traits<T>::reference;
  requires Reference<std::remove_const_t<decltype(quantity_point_like_traits<T>::reference)>>;
  quantity_point_like_traits<T>::point_origin;
  requires PointOrigin<std::remove_const_t<decltype(quantity_point_like_traits<T>::point_origin)>>;
  typename quantity_point_like_traits<T>::rep;
  requires RepresentationOf<typename quantity_point_like_traits<T>::rep,
                            get_quantity_spec(quantity_point_like_traits<T>::reference).character>;
  requires Quantity<std::remove_cvref_t<decltype(quantity_point_like_traits<T>::quantity_from_origin(qp))>>;
  {
    make_quantity_point<quantity_point_like_traits<T>::point_origin>(
      quantity_point_like_traits<T>::quantity_from_origin(qp))
  }
  -> std::same_as<quantity_point<quantity_point_like_traits<T>::reference, quantity_point_like_traits<T>::point_origin,
                                 typename quantity_point_like_traits<T>::rep>>;
};

}  // namespace mp_units
