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
template<Magnitude auto M, typename Rep1, typename Rep2>
struct conversion_type_traits {
  using c_rep_type = maybe_common_type<Rep1, Rep2>;
  using c_mag_type = common_magnitude_type<M>;
  using multiplier_type = conditional<
    treat_as_floating_point<c_rep_type>,
    // ensure that the multiplier is also floating-point
    conditional<std::is_arithmetic_v<value_type_t<c_rep_type>>,
                // reuse user's type if possible
                std::common_type_t<c_mag_type, value_type_t<c_rep_type>>, std::common_type_t<c_mag_type, double>>,
    c_mag_type>;
  using c_type = maybe_common_type<c_rep_type, multiplier_type>;
};

/**
 * @brief Value-related details about the conversion from one quantity to another
 *
 * This trait provide ingredients to calculate the conversion factor that needs to be applied
 * to a number, in order to convert from one quantity to another.
 *
 * @note This is a low-level facility.
 *
 * @tparam M common magnitude between the two quantities
 * @tparam T common multiplier representation type
 */
template<Magnitude auto M, typename T>
struct conversion_value_traits {
  static constexpr Magnitude auto num = numerator(M);
  static constexpr Magnitude auto den = denominator(M);
  static constexpr Magnitude auto irr = M * (den / num);
  static constexpr T num_mult = get_value<T>(num);
  static constexpr T den_mult = get_value<T>(den);
  static constexpr T irr_mult = get_value<T>(irr);
  static constexpr T ratio = num_mult / den_mult * irr_mult;
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
          ((From::unit == To::unit && std::constructible_from<typename To::rep, typename From::rep>) ||
           (From::unit != To::unit))  // && scalable_with_<typename To::rep>))
// TODO how to constrain the second part here?
[[nodiscard]] constexpr To sudo_cast(FwdFrom&& q)
{
  constexpr auto q_unit = From::unit;
  if constexpr (q_unit == To::unit) {
    // no scaling of the number needed
    return {static_cast<To::rep>(std::forward<FwdFrom>(q).numerical_value_is_an_implementation_detail_),
            To::reference};  // this is the only (and recommended) way to do a truncating conversion on a number, so we
                             // are using static_cast to suppress all the compiler warnings on conversions
  } else {
    constexpr Magnitude auto c_mag = get_canonical_unit(From::unit).mag / get_canonical_unit(To::unit).mag;
    using type_traits = conversion_type_traits<c_mag, typename From::rep, typename To::rep>;
    using multiplier_type = typename type_traits::multiplier_type;
    // TODO the below crashed nearly every compiler I tried it on
    // auto scale = [&](std::invocable<typename type_traits::c_type> auto func) {
    auto scale = [&](auto func) {
      auto res =
        static_cast<To::rep>(func(static_cast<type_traits::c_type>(q.numerical_value_is_an_implementation_detail_)));
      return To{res, To::reference};
    };

    // scale the number
    if constexpr (is_integral(c_mag))
      return scale([&](auto value) { return value * get_value<multiplier_type>(numerator(c_mag)); });
    else if constexpr (is_integral(pow<-1>(c_mag)))
      return scale([&](auto value) { return value / get_value<multiplier_type>(denominator(c_mag)); });
    else {
      using value_traits = conversion_value_traits<c_mag, multiplier_type>;

      // TODO: This may not be the right choice here; see #580 and #599
      if constexpr (std::is_floating_point_v<multiplier_type>)
        // this results in great assembly
        return scale([](auto value) { return value * value_traits::ratio; });
      else
        // this is slower but allows conversions like 2000 m -> 2 km without loosing data
        return scale(
          [](auto value) { return value * value_traits::num_mult / value_traits::den_mult * value_traits::irr_mult; });
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
template<QuantityPoint ToQP, typename FwdFromQP, QuantityPoint FromQP = std::remove_cvref_t<FwdFromQP>>
  requires(castable(FromQP::quantity_spec, ToQP::quantity_spec)) &&
          (detail::same_absolute_point_origins(ToQP::point_origin, FromQP::point_origin)) &&
          ((FromQP::unit == ToQP::unit && std::constructible_from<typename ToQP::rep, typename FromQP::rep>) ||
           (FromQP::unit != ToQP::unit))
[[nodiscard]] constexpr QuantityPoint auto sudo_cast(FwdFromQP&& qp)
{
  constexpr Magnitude auto c_mag = get_canonical_unit(FromQP::unit).mag / get_canonical_unit(ToQP::unit).mag;
  using type_traits = conversion_type_traits<c_mag, typename FromQP::rep, typename ToQP::rep>;
  using c_rep_type = type_traits::c_rep_type;
  using multiplier_type = typename type_traits::multiplier_type;
  using value_traits = conversion_value_traits<c_mag, multiplier_type>;

  constexpr auto output_unit_ref = make_reference(FromQP::quantity_spec, ToQP::unit);
  constexpr auto offset_represented_as = [&](auto quantity) {
    using Q = decltype(quantity);
    // in the following, we take the detour through `quantity_point` to determine the offset between the two
    // point_origins; there seem to be cases where we have two `zeroeth_point_origins` of different units (i.e. m vs.
    // km), and the subtraction operator between such two origin points seems to be missing.
    static constexpr auto zero = typename Q::rep{0} * Q::reference;
    // TODO: should we attempt to round values here, if needed?
    return sudo_cast<Q>(quantity_point{zero, FromQP::point_origin} - quantity_point{zero, ToQP::point_origin});
  };
  if constexpr (is_same_v<std::remove_const_t<decltype(ToQP::point_origin)>,
                          std::remove_const_t<decltype(FromQP::point_origin)>>) {
    // no change of offset needed; delegate to the pure sudo_cast<Q> implementation
    return quantity_point{
      sudo_cast<typename ToQP::quantity_type>(std::forward<FwdFromQP>(qp).quantity_from(FromQP::point_origin)),
      ToQP::point_origin};
  } else if constexpr (FromQP::quantity_type::unit == ToQP::quantity_type::unit) {
    // no scaling of the unit is needed; thus we can perform all computation in a single unit without any runtime
    // scaling anywhere
    // we statically convert the offset to unit of the quantities, to avoid runtime rescaling.
    constexpr auto offset = offset_represented_as(quantity<FromQP::reference, c_rep_type>{});
    return quantity_point{
      sudo_cast<typename ToQP::quantity_type>(std::forward<FromQP>(qp).quantity_from(FromQP::point_origin) + offset),
      ToQP::point_origin};
  } else if constexpr (std::is_floating_point_v<multiplier_type>) {
    // for the provided set of representation types, we will use floating-point intermediate representations
    // (typically, when at least one of input or output representations is floating-point).
    // with those, the choice of unit has almost no impact on the conversion accuracy, and thus we can choose
    // the larger unit of the two (input/output) to ensure there is no risk of overflow.
    constexpr auto intermediate_reference = [&]() {
      if constexpr (value_traits::num_mult * value_traits::irr_mult >= value_traits::den_mult) {
        // the input unit is larger
        return FromQP::reference;
      } else {
        return output_unit_ref;
      }
    }();
    using intermediate_rep_type = typename type_traits::c_type;
    using intermediate_quantity_type = quantity<intermediate_reference, intermediate_rep_type>;
    constexpr auto offset = offset_represented_as(intermediate_quantity_type{});
    return quantity_point{
      sudo_cast<typename ToQP::quantity_type>(
        sudo_cast<intermediate_quantity_type>(std::forward<FromQP>(qp).quantity_from(FromQP::point_origin)) + offset),
      ToQP::point_origin};
  } else {
    // with integral representations, we expect the conversion result to be accurate to the resolution of the output
    // representation. So, in general, we should perform the offset computation using the output units. However,
    // if the offset is large compared to the output unit, there is a risk of overflow. Usually, this requires
    // that the offset is specified in terms of a larger unit (because otherwise, the offset would overflow too).
    // Therefore, we use the offset's unit as intermediate unit if the offset falls outside of the range of the
    // smaller unit.
    constexpr auto intermediate_reference = [&]() {
      if constexpr (value_traits::num_mult * value_traits::irr_mult >= value_traits::den_mult) {
        // the output unit is smaller; check if we can represent the offset faithfully in the output unit without
        // overflow
        using candidate_offset_type = quantity<output_unit_ref, c_rep_type>;
        constexpr auto min_representable_offset = value_cast<long double>(candidate_offset_type::min());
        constexpr auto max_representable_offset = value_cast<long double>(candidate_offset_type::max());
        constexpr auto offset_value = offset_represented_as(quantity<ToQP::reference, long double>{});
        if constexpr ((min_representable_offset <= offset_value) && (offset_value <= max_representable_offset)) {
          // the offset can reasonably be represented by the output unit, so we use that one
          return output_unit_ref;
        } else {
          // the offset would overflow if expressed in the output unit; use the input unit instead
          return FromQP::reference;
        }
      } else {
        // the output units is larger; we always use that one
        return output_unit_ref;
      }
    }();
    using intermediate_rep_type = typename type_traits::c_type;
    using intermediate_quantity_type = quantity<intermediate_reference, intermediate_rep_type>;
    constexpr auto offset = offset_represented_as(intermediate_quantity_type{});
    return quantity_point{
      sudo_cast<typename ToQP::quantity_type>(
        sudo_cast<intermediate_quantity_type>(std::forward<FromQP>(qp).quantity_from(FromQP::point_origin)) + offset),
      ToQP::point_origin};
  }
}

}  // namespace mp_units::detail
