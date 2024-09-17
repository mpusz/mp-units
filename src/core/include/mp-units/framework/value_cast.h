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
#include <mp-units/bits/sudo_cast.h>
#include <mp-units/framework/quantity_concepts.h>
#include <mp-units/framework/quantity_point_concepts.h>
#include <mp-units/framework/reference.h>
#include <mp-units/framework/representation_concepts.h>
#include <mp-units/framework/unit_concepts.h>

MP_UNITS_EXPORT
namespace mp_units {

/**
 * @brief Explicit cast of a quantity's unit
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (e.g. non-truncating) conversion. In truncating cases an explicit cast have to be used.
 *
 * auto d = value_cast<si::second>(1234 * ms);
 *
 * @tparam ToU a unit to use for a target quantity
 */
template<Unit auto ToU, typename FwdQ, Quantity Q = std::remove_cvref_t<FwdQ>>
  requires(convertible(Q::reference, ToU))
[[nodiscard]] constexpr Quantity auto value_cast(FwdQ&& q)
{
  return detail::sudo_cast<quantity<detail::make_reference(Q::quantity_spec, ToU), typename Q::rep>>(
    std::forward<FwdQ>(q));
}

/**
 * @brief Explicit cast of a quantity's representation type
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (e.g. non-truncating) conversion. In truncating cases an explicit cast have to be used.
 *
 * auto q = value_cast<int>(1.23 * ms);
 *
 * @tparam ToRep a representation type to use for a target quantity
 */
template<Representation ToRep, typename FwdQ, Quantity Q = std::remove_cvref_t<FwdQ>>
  requires RepresentationOf<ToRep, Q::quantity_spec.character> && std::constructible_from<ToRep, typename Q::rep>
[[nodiscard]] constexpr quantity<Q::reference, ToRep> value_cast(FwdQ&& q)
{
  return detail::sudo_cast<quantity<Q::reference, ToRep>>(std::forward<FwdQ>(q));
}

/**
 * @brief Explicit cast of a quantity's unit and representation type
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (e.g. non-truncating) conversion. In truncating cases an explicit cast have to be used.
 *
 * auto q = value_cast<us, int>(1.23 * ms);
 *
 * @tparam ToU a unit to use for the target quantity
 * @tparam ToRep a representation type to use for the target quantity
 */
template<Unit auto ToU, Representation ToRep, typename FwdQ, Quantity Q = std::remove_cvref_t<FwdQ>>
  requires(convertible(Q::reference, ToU)) && RepresentationOf<ToRep, Q::quantity_spec.character> &&
          std::constructible_from<ToRep, typename Q::rep>
[[nodiscard]] constexpr Quantity auto value_cast(FwdQ&& q)
{
  return detail::sudo_cast<quantity<detail::make_reference(Q::quantity_spec, ToU), ToRep>>(std::forward<FwdQ>(q));
}

template<Representation ToRep, Unit auto ToU, typename FwdQ, Quantity Q = std::remove_cvref_t<FwdQ>>
  requires(convertible(Q::reference, ToU)) && RepresentationOf<ToRep, Q::quantity_spec.character> &&
          std::constructible_from<ToRep, typename Q::rep>
[[nodiscard]] constexpr Quantity auto value_cast(FwdQ&& q)
{
  return value_cast<ToU, ToRep>(std::forward<FwdQ>(q));
}


/**
 * @brief Explicit cast of a quantity's representation
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (e.g. non-truncating) conversion. In truncating cases an explicit cast have to be used.
 *
 * using ToQ = quantity<us, int>;
 * auto q = value_cast<ToQ>(1.23 * ms);
 *
 * Note that value_cast only changes the "representation aspects" (unit and representation
 * type), but not the "meaning" (quantity type).
 *
 * @tparam ToQ a target quantity type to which to cast the representation
 */
template<Quantity ToQ, typename FwdQ, Quantity Q = std::remove_cvref_t<FwdQ>>
  requires(convertible(Q::reference, ToQ::unit)) &&
          (ToQ::quantity_spec == Q::quantity_spec) && std::constructible_from<typename ToQ::rep, typename Q::rep>
[[nodiscard]] constexpr Quantity auto value_cast(FwdQ&& q)
{
  return detail::sudo_cast<ToQ>(std::forward<FwdQ>(q));
}

/**
 * @brief Explicit cast of a quantity point's unit
 *
 * Implicit conversions between quantity points of different types are allowed only for "safe"
 * (e.g. non-truncating) conversion. In truncating cases an explicit cast have to be used.
 *
 * auto qp = value_cast<si::second>(quantity_point{1234 * ms});
 *
 * @tparam ToU a unit to use for a target quantity point
 */
template<Unit auto ToU, typename FwdQP, QuantityPoint QP = std::remove_cvref_t<FwdQP>>
  requires(convertible(QP::reference, ToU))
[[nodiscard]] constexpr QuantityPoint auto value_cast(FwdQP&& qp)
{
  return quantity_point{value_cast<ToU>(std::forward<FwdQP>(qp).quantity_from_origin_is_an_implementation_detail_),
                        QP::point_origin};
}

/**
 * @brief Explicit cast of a quantity point's representation type
 *
 * Implicit conversions between quantity points of different types are allowed only for "safe"
 * (e.g. non-truncating) conversion. In truncating cases an explicit cast have to be used.
 *
 * auto qp = value_cast<int>(quantity_point{1.23 * ms});
 *
 * @tparam ToRep a representation type to use for a target quantity point
 */
template<Representation ToRep, typename FwdQP, QuantityPoint QP = std::remove_cvref_t<FwdQP>>
  requires RepresentationOf<ToRep, QP::quantity_spec.character> && std::constructible_from<ToRep, typename QP::rep>
[[nodiscard]] constexpr quantity_point<QP::reference, QP::point_origin, ToRep> value_cast(FwdQP&& qp)
{
  return {value_cast<ToRep>(std::forward<FwdQP>(qp).quantity_from_origin_is_an_implementation_detail_),
          QP::point_origin};
}

/**
 * @brief Explicit cast of a quantity point's unit and representation type
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (e.g. non-truncating) conversion. In truncating cases an explicit cast have to be used.
 *
 * auto qp = value_cast<us, int>(quantity_point{1.23 * ms});
 *
 * @tparam ToU a unit to use for the target quantity
 * @tparam ToRep a representation type to use for the target quantity
 */
template<Unit auto ToU, Representation ToRep, typename FwdQP, QuantityPoint QP = std::remove_cvref_t<FwdQP>>
  requires(convertible(QP::reference, ToU)) && RepresentationOf<ToRep, QP::quantity_spec.character> &&
          std::constructible_from<ToRep, typename QP::rep>
[[nodiscard]] constexpr QuantityPoint auto value_cast(FwdQP&& qp)
{
  return quantity_point{
    value_cast<ToU, ToRep>(std::forward<FwdQP>(qp).quantity_from_origin_is_an_implementation_detail_),
    QP::point_origin};
}

template<Representation ToRep, Unit auto ToU, typename FwdQP, QuantityPoint QP = std::remove_cvref_t<FwdQP>>
  requires(convertible(QP::reference, ToU)) && RepresentationOf<ToRep, QP::quantity_spec.character> &&
          std::constructible_from<ToRep, typename QP::rep>
[[nodiscard]] constexpr QuantityPoint auto value_cast(FwdQP&& qp)
{
  return value_cast<ToU, ToRep>(std::forward<FwdQP>(qp));
}

/**
 * @brief Explicit cast of a quantity point's representation
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (e.g. non-truncating) conversion. In truncating cases an explicit cast have to be used.
 *
 * inline constexpr struct A : absolute_point_origin<A, isq::distance> A;
 *
 * using ToQ = quantity<mm, int>;
 * auto qp = value_cast<ToQ>(quantity_point{1.23 * m});
 *
 * Note that value_cast only changes the "representation aspects" (unit and representation
 * type), but not the "meaning" (quantity type or the actual point that is being described).
 *
 * @tparam ToQ a target quantity type to which to cast the representation of the point
 */
template<Quantity ToQ, typename FwdQP, QuantityPoint QP = std::remove_cvref_t<FwdQP>>
  requires(convertible(QP::reference, ToQ::unit)) &&
          (ToQ::quantity_spec == QP::quantity_spec) && std::constructible_from<typename ToQ::rep, typename QP::rep>
[[nodiscard]] constexpr QuantityPoint auto value_cast(FwdQP&& qp)
{
  return quantity_point{value_cast<ToQ>(std::forward<FwdQP>(qp).quantity_from_origin_is_an_implementation_detail_),
                        QP::point_origin};
}

/**
 * @brief Explicit cast of a quantity point's representation, including potentially the point origin
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (e.g. non-truncating) conversion. In truncating cases an explicit cast have to be used.
 *
 * inline constexpr struct A : absolute_point_origin<A, isq::distance> A;
 * inline constexpr struct B : relative_point_origin<A + 1*m> B;
 *
 * using ToQP = quantity_point<mm, B, int>;
 * auto qp = value_cast<ToQP>(quantity_point{1.23 * m});
 *
 * Note that value_cast only changes the "representation aspects" (unit, representation
 * type and point origin), but not the "meaning" (quantity type or the actual point that is
 * being described).
 *
 * Note also that changing the point origin bears risks regarding truncation and overflow
 * similar to other casts that change representation (which is why we require a `value_cast`
 * and disallow implicit conversions). This cast is guaranteed not to cause overflow of
 * any intermediate representation type provided that the input quantity point is within
 * the range of `ToQP`. Calling `value_cast<ToQP>(qp)` on a `qp` outside of the range of `ToQP`
 * is potentially undefined behaviour.
 * The implementation further attempts not to cause more than
 * rounding error than approximately the sum of the resolution of `qp` as represented in `FromQP`,
 * plust the resolution of `qp` as represented in `ToQP`.
 *
 * @tparam ToQP a target quantity point type to which to cast the representation of the point
 */
template<QuantityPoint ToQP, typename FwdQP, QuantityPoint QP = std::remove_cvref_t<FwdQP>>
  requires(convertible(QP::reference, ToQP::unit)) && (ToQP::quantity_spec == QP::quantity_spec) &&
          (detail::same_absolute_point_origins(ToQP::point_origin, QP::point_origin)) &&
          std::constructible_from<typename ToQP::rep, typename QP::rep>
[[nodiscard]] constexpr QuantityPoint auto value_cast(FwdQP&& qp)
{
  return detail::sudo_cast<ToQP>(std::forward<FwdQP>(qp));
}

}  // namespace mp_units
