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
#include <mp-units/framework/magnitude.h>
#include <mp-units/framework/quantity_concepts.h>
#include <mp-units/framework/reference_concepts.h>
#include <mp-units/framework/unit.h>

namespace mp_units::detail {

template<typename T, typename Other>
struct get_common_type : std::common_type<T, Other> {};

template<typename T, typename Other>
using maybe_common_type = MP_UNITS_TYPENAME std::conditional_t<requires { typename std::common_type_t<T, Other>; },
                                                               get_common_type<T, Other>, std::type_identity<T>>::type;

/**
 * @brief Details about the conversion from one quantity to another.
 *
 * This struct calculates the conversion factor that needs to be applied to a number,
 * in order to convert from one quantity to another. In addition to that, it also
 * helps to determine what representations to use at which step in the conversion process,
 * in order to avoid overflow and underflow while not causing excessive computations.
 *
 * @note This is a low-level facility.
 *
 * @tparam To a target quantity type to cast to
 * @tparam From a source quantity type to cast from
 */
template<Quantity To, Quantity From>
  requires(castable(From::quantity_spec, To::quantity_spec))
struct magnitude_conversion_traits {
  // scale the number
  static constexpr Magnitude auto c_mag = get_canonical_unit(From::unit).mag / get_canonical_unit(To::unit).mag;
  static constexpr Magnitude auto num = numerator(c_mag);
  static constexpr Magnitude auto den = denominator(c_mag);
  static constexpr Magnitude auto irr = c_mag * (den / num);
  using c_rep_type = maybe_common_type<typename std::remove_reference_t<From>::rep, typename To::rep>;
  using c_mag_type = common_magnitude_type<c_mag>;
  using multiplier_type = conditional<
    treat_as_floating_point<c_rep_type>,
    // ensure that the multiplier is also floating-point
    conditional<std::is_arithmetic_v<value_type_t<c_rep_type>>,
                // reuse user's type if possible
                std::common_type_t<c_mag_type, value_type_t<c_rep_type>>, std::common_type_t<c_mag_type, double>>,
    c_mag_type>;
  using c_type = maybe_common_type<c_rep_type, multiplier_type>;
  static constexpr auto val(Magnitude auto m) { return get_value<multiplier_type>(m); };
  static constexpr multiplier_type num_mult = val(num);
  static constexpr multiplier_type den_mult = val(den);
  static constexpr multiplier_type irr_mult = val(irr);
  static constexpr multiplier_type ratio = num_mult / den_mult * irr_mult;
};


/**
 * @brief Explicit cast between different quantity types
 *
 * @note This is a low-level facility and is too powerful to be used by the users directly. They should either use
 * `value_cast` or `quantity_cast`.
 *
 * @tparam To a target quantity type to cast to
 */
template<Quantity To, typename From>
  requires Quantity<std::remove_cvref_t<From>> &&
           (castable(std::remove_reference_t<From>::quantity_spec, To::quantity_spec)) &&
           ((std::remove_reference_t<From>::unit == To::unit &&
             std::constructible_from<typename To::rep, typename std::remove_reference_t<From>::rep>) ||
            (std::remove_reference_t<From>::unit != To::unit))  // && scalable_with_<typename To::rep>))
// TODO how to constrain the second part here?
[[nodiscard]] constexpr To sudo_cast(From&& q)
{
  constexpr auto q_unit = std::remove_reference_t<From>::unit;
  if constexpr (q_unit == To::unit) {
    // no scaling of the number needed
    return {static_cast<MP_UNITS_TYPENAME To::rep>(std::forward<From>(q).numerical_value_is_an_implementation_detail_),
            To::reference};  // this is the only (and recommended) way to do a truncating conversion on a number, so we
                             // are using static_cast to suppress all the compiler warnings on conversions
  } else {
    // scale the number
    using traits = magnitude_conversion_traits<To, std::remove_reference_t<From>>;
    if constexpr (std::is_floating_point_v<typename traits::multiplier_type>) {
      // this results in great assembly
      auto res = static_cast<MP_UNITS_TYPENAME To::rep>(
        static_cast<traits::c_type>(q.numerical_value_is_an_implementation_detail_) * traits::ratio);
      return {res, To::reference};
    } else {
      // this is slower but allows conversions like 2000 m -> 2 km without loosing data
      auto res = static_cast<MP_UNITS_TYPENAME To::rep>(
        static_cast<traits::c_type>(q.numerical_value_is_an_implementation_detail_) * traits::num_mult /
        traits::den_mult * traits::irr_mult);
      return {res, To::reference};
    }
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
template<QuantityPoint ToQP, typename FromQP>
  requires QuantityPoint<std::remove_cvref_t<FromQP>> &&
           (castable(std::remove_reference_t<FromQP>::quantity_spec, ToQP::quantity_spec)) &&
           (detail::same_absolute_point_origins(ToQP::point_origin, std::remove_reference_t<FromQP>::point_origin)) &&
           ((std::remove_reference_t<FromQP>::unit == ToQP::unit &&
             std::constructible_from<typename ToQP::rep, typename std::remove_reference_t<FromQP>::rep>) ||
            (std::remove_reference_t<FromQP>::unit != ToQP::unit))
[[nodiscard]] constexpr QuantityPoint auto sudo_cast(FromQP&& qp)
{
  using qp_type = std::remove_reference_t<FromQP>;
  using traits = magnitude_conversion_traits<typename ToQP::quantity_type, typename qp_type::quantity_type>;
  if constexpr (ToQP::point_origin == qp_type::point_origin) {
    // no change of offset needed
    return quantity_point{
      sudo_cast<typename ToQP::quantity_type>(std::forward<FromQP>(qp).quantity_from(qp_type::point_origin)),
      ToQP::point_origin};
  } else if constexpr (qp_type::quantity_type::unit == ToQP::quantity_type::unit) {
    // no scaling of the unit is needed; thus we can perform all computation in a single unit without any runtime
    // scaling anywhere
    using offset_rep_type = typename traits::c_rep_type;
    // in the following, we take the detour through `quantity_point` to determine the offset between the two
    // point_origins; there seem to be cases where we have two `zeroeth_point_origins` of different units (i.e. m vs.
    // km), where the subtraction for the latter is not defined.
    static constexpr auto zero = offset_rep_type{0} * qp_type::reference;
    // we statically convert the offset to unit of the quantities, to avoid runtime rescaling.
    // TODO: should we implement do a rounding instead here?
    static constexpr auto offset = sudo_cast<quantity<qp_type::reference, offset_rep_type>>(
      quantity_point{zero, qp_type::point_origin} - quantity_point{zero, ToQP::point_origin});
    return quantity_point{
      sudo_cast<typename ToQP::quantity_type>(std::forward<FromQP>(qp).quantity_from(qp_type::point_origin) + offset),
      ToQP::point_origin};
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
    //
    // This implementation handles operations (a) and (b) by delegating to the `sudo_cast<quantity>`,
    // while opertaion (c) is delegated to quantity::operator+. We ensure that no hidden conversions happen in the
    // latter, by proving both operands in the same representation, the `intermediate_quantity_type`. The
    // `intermediate_quantity_type` in turn is chosen such that operations (a) and (b) individually happen either during
    // the inital conversion from the input, or during the final conversion to the output, whichever is optimal given
    // the input and output types.
    static constexpr auto intermediate_reference = make_reference(qp_type::quantity_spec, ToQP::unit);
    // We always work in the larger of the two (input/output) representations.
    using intermediate_rep_type = typename traits::c_type;
    using intermediate_quantity_type = quantity<intermediate_reference, intermediate_rep_type>;
    // in the following, we take the detour through `quantity_point` to determine the offset between the two
    // point_origins; there seem to be cases where we have two `zeroeth_point_origins` of different units (i.e. m vs.
    // km), where the subtraction for the latter is not defined.
    static constexpr auto zero = intermediate_rep_type{0} * intermediate_reference;
    static constexpr intermediate_quantity_type offset =
      quantity_point{zero, qp_type::point_origin} - quantity_point{zero, ToQP::point_origin};
    return quantity_point{
      sudo_cast<typename ToQP::quantity_type>(
        sudo_cast<intermediate_quantity_type>(std::forward<FromQP>(qp).quantity_from(qp_type::point_origin)) + offset),
      ToQP::point_origin};
  }
}

}  // namespace mp_units::detail
