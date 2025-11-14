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

#include <mp-units/bits/fixed_point.h>
#include <mp-units/ext/type_traits.h>
#include <mp-units/framework/quantity_concepts.h>
#include <mp-units/framework/reference_concepts.h>
#include <mp-units/framework/unit.h>
#include <mp-units/framework/unit_magnitude.h>

namespace mp_units::detail {

template<typename... Ts>
constexpr bool has_common_type_v = requires { typename std::common_type_t<Ts...>; };

template<typename T, typename Other>
using maybe_common_type =
  std::conditional_t<has_common_type_v<T, Other>, std::common_type<T, Other>, std::type_identity<T>>::type;

/**
 * @brief Type-related details about the conversion from one quantity to another
 *
 * This trait helps to determine what representations to use at which step in the conversion process,
 * in order to avoid overflow and underflow while not causing excessive computations.
 *
 * @note This is a low-level facility.
 *
 * @tparam M common magnitude between the two quantities
 * @tparam Rep1 first quantity representation type
 * @tparam Rep2 second quantity representation type
 */
template<UnitMagnitude auto M, typename Rep1, typename Rep2>
struct conversion_type_traits {
  using c_rep_type = maybe_common_type<Rep1, Rep2>;
  using c_type = conditional<std::is_arithmetic_v<value_type_t<c_rep_type>>, value_type_t<c_rep_type>, double>;
};

/**
 * @brief Explicit cast between different quantity types
 *
 * @note This is a low-level facility and is too powerful to be used by the users directly. They should either use
 * `value_cast` or `quantity_cast`.
 *
 * @tparam To a target quantity type to cast to
 */
template<Quantity To, typename FwdFrom, Quantity From = std::remove_cvref_t<FwdFrom>>
  requires(castable(From::quantity_spec, To::quantity_spec)) &&
          ((equivalent(From::unit, To::unit) && std::constructible_from<typename To::rep, typename From::rep>) ||
           (!equivalent(From::unit, To::unit)))  // && scalable_with_<typename To::rep>))
// TODO how to constrain the second part here?
[[nodiscard]] constexpr To sudo_cast(FwdFrom&& q)
{
  constexpr auto q_unit = From::unit;
  if constexpr (equivalent(q_unit, To::unit)) {
    // no scaling of the number needed
    return {static_cast<To::rep>(std::forward<FwdFrom>(q).numerical_value_is_an_implementation_detail_),
            To::reference};  // this is the only (and recommended) way to do a truncating conversion on a number, so we
                             // are using static_cast to suppress all the compiler warnings on conversions
  } else {
    constexpr Magnitude auto c_mag = get_canonical_unit(From::unit).mag / get_canonical_unit(To::unit).mag;

    typename To::rep res =
      scale(std::type_identity<typename To::rep>{}, c_mag, q.numerical_value_is_an_implementation_detail_);
    return To{res, To::reference};
  }
}


/**
 * @brief Explicit cast between different quantity_point types
 *
 * @note This is a low-level facility and is too powerful to be used by the users directly. They should either use
 * `value_cast` or `quantity_cast`.
 *
 * @tparam ToQP a target quantity point type to which to cast to
 */
template<QuantityPoint ToQP, typename FwdFromQP, QuantityPoint FromQP = std::remove_cvref_t<FwdFromQP>>
  requires(castable(FromQP::quantity_spec, ToQP::quantity_spec)) &&
          (detail::same_absolute_point_origins(ToQP::point_origin, FromQP::point_origin)) &&
          ((equivalent(FromQP::unit, ToQP::unit) &&
            std::constructible_from<typename ToQP::rep, typename FromQP::rep>) ||
           (!equivalent(FromQP::unit, ToQP::unit)))
[[nodiscard]] constexpr QuantityPoint auto sudo_cast(FwdFromQP&& qp)
{
  if constexpr (is_same_v<MP_UNITS_NONCONST_TYPE(ToQP::point_origin), MP_UNITS_NONCONST_TYPE(FromQP::point_origin)>) {
    return quantity_point{
      sudo_cast<typename ToQP::quantity_type>(std::forward<FwdFromQP>(qp).quantity_from(FromQP::point_origin)),
      FromQP::point_origin};
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
    // and target unit and representation.
    constexpr UnitMagnitude auto c_mag = get_canonical_unit(FromQP::unit).mag / get_canonical_unit(ToQP::unit).mag;
    using type_traits = conversion_type_traits<c_mag, typename FromQP::rep, typename ToQP::rep>;
    using c_type = type_traits::c_type;
    if constexpr (_get_value<long double>(c_mag) > 1.) {
      // original unit had a larger unit magnitude; if we first convert to the common representation but retain the
      // unit, we obtain the largest possible range while not causing truncation of fractional values. This is optimal
      // for the offset computation.
      return sudo_cast<ToQP>(
        sudo_cast<quantity_point<FromQP::reference, FromQP::point_origin, c_type>>(std::forward<FwdFromQP>(qp))
          .point_for(ToQP::point_origin));
    } else {
      // new unit may have a larger unit magnitude; we first need to convert to the new unit (potentially causing
      // truncation, but no more than if we did the conversion later), but make sure we keep the larger of the two
      // representation types. Then, we can perform the offset computation.
      return sudo_cast<ToQP>(
        sudo_cast<quantity_point<make_reference(FromQP::quantity_spec, ToQP::unit), FromQP::point_origin, c_type>>(
          std::forward<FwdFromQP>(qp))
          .point_for(ToQP::point_origin));
    }
  }
}

}  // namespace mp_units::detail
