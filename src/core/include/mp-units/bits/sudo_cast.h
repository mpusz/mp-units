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

#include <mp-units/ext/type_traits.h>
#include <mp-units/framework/quantity_concepts.h>
#include <mp-units/framework/reference_concepts.h>
#include <mp-units/framework/scaling.h>
#include <mp-units/framework/unit.h>
#include <mp-units/framework/unit_magnitude.h>

namespace mp_units::detail {

template<typename... Ts>
constexpr bool has_common_type_v = requires { typename std::common_type_t<Ts...>; };

template<typename T, typename Other>
using maybe_common_type =
  conditional<has_common_type_v<T, Other>, std::common_type<T, Other>, std::type_identity<T>>::type;

/**
 * @brief Magnitude-only details about a unit conversion factor
 *
 * Factored out of `conversion_type_traits` and `conversion_value_traits` so that the
 * magnitude decomposition (which depends only on `M`, not on any representation type)
 * is computed once per unique magnitude and shared across all representation combinations.
 *
 * @note This is a low-level facility.
 *
 * @tparam M common magnitude between the two quantities
 */
template<UnitMagnitude auto M>
struct magnitude_traits {
  using c_mag_type = common_magnitude_type<M>;
  static constexpr UnitMagnitude auto num = numerator(M);
  static constexpr UnitMagnitude auto den = denominator(M);
  static constexpr UnitMagnitude auto irr = M * (den / num);
};

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
  if constexpr (equivalent(From::unit, To::unit)) {
    // no scaling of the number needed
    return {silent_cast<typename To::rep>(std::forward<FwdFrom>(q).numerical_value_is_an_implementation_detail_),
            To::reference};
  } else {
    constexpr UnitMagnitude auto c_mag = get_canonical_unit(From::unit).mag / get_canonical_unit(To::unit).mag;

    typename To::rep res =
      silent_cast<typename To::rep>(scale<typename To::rep>(c_mag, q.numerical_value_is_an_implementation_detail_));
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
  requires(mp_units::castable(FromQP::quantity_spec, ToQP::quantity_spec)) &&
          (detail::same_absolute_point_origins(ToQP::point_origin, FromQP::point_origin)) &&
          ((equivalent(FromQP::unit, ToQP::unit) &&
            std::constructible_from<typename ToQP::rep, typename FromQP::rep>) ||
           (!equivalent(FromQP::unit, ToQP::unit)))
[[nodiscard]] constexpr QuantityPoint auto sudo_cast(FwdFromQP&& qp)
{
  if constexpr (is_same_v<MP_UNITS_NONCONST_TYPE(ToQP::point_origin), MP_UNITS_NONCONST_TYPE(FromQP::point_origin)>) {
    // Same origin: delegate entirely to the quantity sudo_cast — no offset arithmetic needed.
    return quantity_point{
      sudo_cast<typename ToQP::quantity_type>(std::forward<FwdFromQP>(qp).quantity_from(FromQP::point_origin)),
      ToQP::point_origin};
  } else {
    // Different origins: we need to (a) convert the rep/unit, (b) add the origin offset.
    // The order and intermediate unit choice matters for accuracy and overflow avoidance.
    //
    // Strategy: pick an intermediate unit/rep, then compute:
    //   result = sudo_cast<intermediate>(input_quantity) + offset
    // where offset is the static difference between the two origins expressed in the
    // intermediate unit.  Finally, sudo_cast the sum to the target type.
    //
    // Numerically, we'll potentially need to do three things:
    //  (a) cast the representation type
    //  (b) scale the numerical value
    //  (c) add/subtract the origin difference
    // The intermediate unit determines the order of (b) and (c).

    constexpr UnitMagnitude auto c_mag =
      mp_units::get_canonical_unit(FromQP::unit).mag / mp_units::get_canonical_unit(ToQP::unit).mag;
    using type_traits = conversion_type_traits<c_mag, typename FromQP::rep, typename ToQP::rep>;
    using c_rep_type = type_traits::c_rep_type;
    using c_type = type_traits::c_type;

    // Helper: statically compute the origin offset expressed in the given quantity type Q.
    // We go via quantity_point subtraction to handle all origin relationships correctly,
    // including cases where two zeroth_point_origins of different units exist.
    constexpr auto offset_as = [&]<typename Q>(std::type_identity<Q>) {
      constexpr auto zero = typename Q::rep{0} * Q::reference;
      return sudo_cast<Q>(quantity_point{zero, FromQP::point_origin} - quantity_point{zero, ToQP::point_origin});
    };

    constexpr auto output_unit_ref = make_reference(FromQP::quantity_spec, ToQP::unit);

    if constexpr (equivalent(FromQP::unit, ToQP::unit)) {
      // Same unit, different origin: no scaling needed — add the offset directly in the
      // common rep without any unit conversion.  This is always exact.
      using intermediate_type = quantity<FromQP::reference, c_rep_type>;
      constexpr auto offset = offset_as(std::type_identity<intermediate_type>{});
      return quantity_point{
        sudo_cast<typename ToQP::quantity_type>(
          sudo_cast<intermediate_type>(std::forward<FwdFromQP>(qp).quantity_from(FromQP::point_origin)) + offset),
        ToQP::point_origin};
    } else if constexpr (treat_as_floating_point<c_type>) {
      // Floating-point intermediate: prefer the larger unit to minimise the magnitude of scaling
      // applied to the input value, then use point_for to let the library's common-unit quantity
      // arithmetic handle the origin offset (e.g. 2.0 km + 42 m → 2042.0 m, not 2.0 + 0.042 km).
      // This avoids the FP precision loss that would occur if we expressed the offset explicitly
      // in the intermediate unit.
      constexpr auto intermediate_ref = [&]() {
        if constexpr (!is_integral(pow<-1>(c_mag)))
          return FromQP::reference;  // from-unit is larger
        else
          return output_unit_ref;  // to-unit is larger (or equal)
      }();
      using intermediate_type = quantity<intermediate_ref, c_type>;
      return quantity_point{
        sudo_cast<typename ToQP::quantity_type>(
          quantity_point{sudo_cast<intermediate_type>(std::forward<FwdFromQP>(qp).quantity_from(FromQP::point_origin)),
                         FromQP::point_origin}
            .point_for(ToQP::point_origin)
            .quantity_from(ToQP::point_origin)),
        ToQP::point_origin};
    } else {
      // Integer intermediate: use the output unit so the result is already in the right
      // unit before add, giving the most-accurate truncation.  However, if the offset
      // would overflow in the output unit (e.g. because it was defined in a larger one),
      // fall back to the input unit.
      //
      // Detect overflow by computing the offset in long double expressed in the output unit
      // and comparing against the representable range of c_rep_type.
      constexpr long double offset_in_output_ld =
        offset_as(std::type_identity<quantity<output_unit_ref, long double>>{}).numerical_value_in(ToQP::unit);
      constexpr bool offset_fits_in_output =
        offset_in_output_ld >= static_cast<long double>(std::numeric_limits<c_rep_type>::min()) &&
        offset_in_output_ld <= static_cast<long double>(std::numeric_limits<c_rep_type>::max());
      if constexpr (offset_fits_in_output) {
        using intermediate_type = quantity<output_unit_ref, c_rep_type>;
        constexpr auto offset = offset_as(std::type_identity<intermediate_type>{});
        return quantity_point{
          sudo_cast<typename ToQP::quantity_type>(
            sudo_cast<intermediate_type>(std::forward<FwdFromQP>(qp).quantity_from(FromQP::point_origin)) + offset),
          ToQP::point_origin};
      } else {
        // offset overflows in the output unit — use the input unit instead
        using intermediate_type = quantity<FromQP::reference, c_rep_type>;
        constexpr auto offset = offset_as(std::type_identity<intermediate_type>{});
        return quantity_point{
          sudo_cast<typename ToQP::quantity_type>(
            sudo_cast<intermediate_type>(std::forward<FwdFromQP>(qp).quantity_from(FromQP::point_origin)) + offset),
          ToQP::point_origin};
      }
    }
  }
}

}  // namespace mp_units::detail
