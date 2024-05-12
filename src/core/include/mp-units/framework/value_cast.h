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
template<Unit auto ToU, typename Q>
  requires Quantity<std::remove_cvref_t<Q>> && (convertible(std::remove_reference_t<Q>::reference, ToU))
[[nodiscard]] constexpr Quantity auto value_cast(Q&& q)
{
  using q_type = std::remove_reference_t<Q>;
  return detail::sudo_cast<quantity<detail::make_reference(q_type::quantity_spec, ToU), typename q_type::rep>>(
    std::forward<Q>(q));
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
template<Representation ToRep, typename Q>
  requires Quantity<std::remove_cvref_t<Q>> &&
           RepresentationOf<ToRep, std::remove_reference_t<Q>::quantity_spec.character> &&
           std::constructible_from<ToRep, typename std::remove_reference_t<Q>::rep>
[[nodiscard]] constexpr quantity<std::remove_reference_t<Q>::reference, ToRep> value_cast(Q&& q)
{
  return detail::sudo_cast<quantity<std::remove_reference_t<Q>::reference, ToRep>>(std::forward<Q>(q));
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
template<Unit auto ToU, Representation ToRep, typename Q>
  requires Quantity<std::remove_cvref_t<Q>> && (convertible(std::remove_reference_t<Q>::reference, ToU)) &&
           RepresentationOf<ToRep, std::remove_reference_t<Q>::quantity_spec.character> &&
           std::constructible_from<ToRep, typename std::remove_reference_t<Q>::rep>
[[nodiscard]] constexpr Quantity auto value_cast(Q&& q)
{
  using q_type = std::remove_reference_t<Q>;
  return detail::sudo_cast<quantity<detail::make_reference(q_type::quantity_spec, ToU), ToRep>>(std::forward<Q>(q));
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
template<Quantity ToQ, typename Q>
  requires Quantity<std::remove_cvref_t<Q>> && (convertible(std::remove_reference_t<Q>::reference, ToQ::unit)) &&
           (ToQ::quantity_spec == std::remove_reference_t<Q>::quantity_spec) &&
           std::constructible_from<typename ToQ::rep, typename std::remove_reference_t<Q>::rep>
[[nodiscard]] constexpr Quantity auto value_cast(Q&& q)
{
  return detail::sudo_cast<ToQ>(std::forward<Q>(q));
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
template<Unit auto ToU, typename QP>
  requires QuantityPoint<std::remove_cvref_t<QP>> && (convertible(std::remove_reference_t<QP>::reference, ToU))
[[nodiscard]] constexpr QuantityPoint auto value_cast(QP&& qp)
{
  return quantity_point{value_cast<ToU>(std::forward<QP>(qp).quantity_from_origin_is_an_implementation_detail_),
                        std::remove_reference_t<QP>::point_origin};
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
template<Representation ToRep, typename QP>
  requires QuantityPoint<std::remove_cvref_t<QP>> &&
           RepresentationOf<ToRep, std::remove_reference_t<QP>::quantity_spec.character> &&
           std::constructible_from<ToRep, typename std::remove_reference_t<QP>::rep>
[[nodiscard]] constexpr quantity_point<std::remove_reference_t<QP>::reference,
                                       std::remove_reference_t<QP>::point_origin, ToRep>
value_cast(QP&& qp)
{
  return {value_cast<ToRep>(std::forward<QP>(qp).quantity_from_origin_is_an_implementation_detail_),
          std::remove_reference_t<QP>::point_origin};
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
template<Unit auto ToU, Representation ToRep, typename QP>
  requires QuantityPoint<std::remove_cvref_t<QP>> && (convertible(std::remove_reference_t<QP>::reference, ToU)) &&
           RepresentationOf<ToRep, std::remove_reference_t<QP>::quantity_spec.character> &&
           std::constructible_from<ToRep, typename std::remove_reference_t<QP>::rep>
[[nodiscard]] constexpr QuantityPoint auto value_cast(QP&& qp)
{
  return quantity_point{value_cast<ToU, ToRep>(std::forward<QP>(qp).quantity_from_origin_is_an_implementation_detail_),
                        std::remove_reference_t<QP>::point_origin};
}

/**
 * @brief Explicit cast of a quantity point's representation
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
 * Note that value_cast only changes the "representation aspects" (unit and representation
 * type), but not the "meaning" (quantity type or the actual point that is being described).
 *
 * @tparam ToQ a target quantity type to which to cast the representation of the point
 */
template<Quantity ToQ, typename QP>
  requires QuantityPoint<std::remove_cvref_t<QP>> && (convertible(std::remove_reference_t<QP>::reference, ToQ::unit)) &&
           (ToQ::quantity_spec == std::remove_reference_t<QP>::quantity_spec) &&
           std::constructible_from<typename ToQ::rep, typename std::remove_reference_t<QP>::rep>
[[nodiscard]] constexpr QuantityPoint auto value_cast(QP&& qp)
{
  return quantity_point{value_cast<ToQ>(std::forward<QP>(qp).quantity_from_origin_is_an_implementation_detail_),
                        std::remove_reference_t<QP>::point_origin};
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
 * @tparam ToQP a target quantity point type to which to cast the representation of the point
 */
template<QuantityPoint ToQP, typename QP>
  requires QuantityPoint<std::remove_cvref_t<QP>> &&
           (convertible(std::remove_reference_t<QP>::reference, ToQP::unit)) &&
           (ToQP::quantity_spec == std::remove_reference_t<QP>::quantity_spec) &&
           (detail::same_absolute_point_origins(ToQP::point_origin, std::remove_reference_t<QP>::point_origin)) &&
           std::constructible_from<typename ToQP::rep, typename std::remove_reference_t<QP>::rep>
[[nodiscard]] constexpr QuantityPoint auto value_cast(QP&& qp)
{
  using qp_type = std::remove_reference_t<QP>;
  if constexpr (is_same_v<std::remove_const_t<decltype(ToQP::point_origin)>,
                          std::remove_const_t<decltype(qp_type::point_origin)>>) {
    return quantity_point{
      value_cast<typename ToQP::quantity_type>(std::forward<QP>(qp).quantity_from(qp_type::point_origin)),
      qp_type::point_origin};
  } else {
    // it's unclear how hard we should try to avoid truncation here. For now, the only corner case we cater for,
    // is when the range of the quantity type of at most one of QP or ToQP doesn't cover the offset between the
    // point origins. In that case, we need to be careful to ensure we use the quantity type with the larger range
    // of the two to perform the point_origin conversion.
    // Numerically, we'll potentially need to do three things:
    //  (a) cast the representation type
    //  (b) scale the numerical value
    //  (c) add/subtract the origin difference
    // In the following, we carefully select the order of these three operations: each of (a) and (b) is scheduled
    // either before or after (c), such that (c) acts on the largest range possible among all combination of source
    // and target unit and represenation.
    constexpr Magnitude auto c_mag = get_canonical_unit(qp_type::unit).mag / get_canonical_unit(ToQP::unit).mag;
    constexpr Magnitude auto num = detail::numerator(c_mag);
    constexpr Magnitude auto den = detail::denominator(c_mag);
    constexpr Magnitude auto irr = c_mag * (den / num);
    using c_rep_type = detail::maybe_common_type<typename ToQP::rep, typename qp_type::rep>;
    using c_mag_type = detail::common_magnitude_type<c_mag>;
    using multiplier_type = conditional<
      treat_as_floating_point<c_rep_type>,
      // ensure that the multiplier is also floating-point
      conditional<std::is_arithmetic_v<value_type_t<c_rep_type>>,
                  // reuse user's type if possible
                  std::common_type_t<c_mag_type, value_type_t<c_rep_type>>, std::common_type_t<c_mag_type, double>>,
      c_mag_type>;
    constexpr auto val = [](Magnitude auto m) { return get_value<multiplier_type>(m); };
    if constexpr (val(num) * val(irr) > val(den)) {
      // original unit had a larger unit magnitude; if we first convert to the common representation but retain the
      // unit, we obtain the largest possible range while not causing truncation of fractional values. This is optimal
      // for the offset computation.
      return value_cast<typename ToQP::quantity_type>(
        value_cast<c_rep_type>(std::forward<QP>(qp)).point_for(ToQP::point_origin));
    } else {
      // new unit may have a larger unit magnitude; we first need to convert to the new unit (potentially causing
      // truncation, but no more than if we did the conversion later), but make sure we keep the larger of the two
      // representation types. Then, we can perform the offset computation.
      return value_cast<typename ToQP::quantity_type>(
        value_cast<ToQP::unit, c_rep_type>(std::forward<QP>(qp)).point_for(ToQP::point_origin));
    }
  }
}


}  // namespace mp_units
