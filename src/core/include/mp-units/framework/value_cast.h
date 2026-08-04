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
#include <mp-units/compat_macros.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/quantity_concepts.h>
#include <mp-units/framework/quantity_point_concepts.h>
#include <mp-units/framework/reference.h>
#include <mp-units/framework/representation_concepts.h>
#include <mp-units/framework/rounding.h>
#include <mp-units/framework/unit_concepts.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <type_traits>
#endif
#endif

namespace mp_units {

namespace detail {

template<typename Rep, Unit UFrom, Unit UTo>
[[nodiscard]] consteval bool scaling_overflows_non_zero_values(UFrom, UTo)
{
  if constexpr (std::is_same_v<UFrom, UTo> || treat_as_floating_point<Rep>)
    return false;
  else if constexpr (std::totally_ordered_with<Rep, std::uintmax_t> &&
                     requires(Rep v) { representation_values<Rep>::max(); }) {
    const auto factor =
      try_get_value<std::uintmax_t>(numerator(get_canonical_unit(UFrom{}).mag / get_canonical_unit(UTo{}).mag));
    if (!factor.has_value())
      return true;  // factor overflows uintmax_t => certainly overflows Rep too
    else if constexpr (std::is_integral_v<Rep>)
      return !std::in_range<Rep>(*factor);
    else
      return *factor > representation_values<Rep>::max();
  } else
    // if the representation is not totally ordered with std::uintmax_t or does not have max() defined
    // then we assume that it might store any value
    return false;
}

template<auto FromU, auto ToU, typename Rep>
concept ExplicitlyCastable = UnitConvertibleTo<MP_UNITS_REMOVE_CONST(decltype(FromU)), ToU> &&
                             (!scaling_overflows_non_zero_values<Rep>(FromU, ToU) ||
                              unsatisfied<"The result of scaling '{}' to '{}' overflows the '{}' representation type">(
                                FromU, ToU, type_name<Rep>()));

// The `truncated` policy is the exact replacement for the pre-2.6.0 forcing conversions, so it keeps
// supporting every representation type they did.
//
// Rounding is only ever performed on the way to an integral destination, so the two requirements below
// apply exactly there: an adjusting policy has to decide which side of a rounding boundary the exact
// result falls on, which needs a real scalar (a vector, a tensor, or a complex value offers no single
// value to compare against a boundary), and rounding a floating-point value works on its integral
// part, which the library performs for standard floating-point types only. A floating-point
// destination instead represents every result to within half ULP on its own (the semantics of
// `rounded`), so it rounds nothing and only rejects the directed policies as undeliverable.
//
// Each requirement is stated as a plain predicate naming the case that violates it, so that the
// library can ask whether a policy applies without emitting a diagnostic: `unsatisfied` throws where
// the implementation supports it, which would turn a query inside an `if constexpr` into a hard error
// instead of `false`.

// An adjusting policy is one that may move the result to a neighboring representable value, which is
// every policy but `truncated`.
template<typename T>
concept AdjustingRoundingPolicy = RoundingPolicy<T> && !std::is_same_v<T, truncated_t>;

template<typename Policy, typename FromRep, typename ToRep>
constexpr bool rounding_lacks_real_scalars =
  AdjustingRoundingPolicy<Policy> && treat_as_integral<ToRep> && (!RealScalar<FromRep> || !RealScalar<ToRep>);

// a representation type treated as floating-point without being one of the standard floating-point
// types (e.g. a fixed-point wrapper used as a real number) cannot have its integral part taken
template<typename Policy, typename FromRep, typename ToRep>
constexpr bool rounding_lacks_standard_floating_point =
  AdjustingRoundingPolicy<Policy> && treat_as_integral<ToRep> && treat_as_floating_point<FromRep> &&
  !std::floating_point<FromRep>;

template<typename Policy, typename ToRep>
constexpr bool rounding_direction_undeliverable =
  treat_as_floating_point<ToRep> && (std::is_same_v<Policy, rounded_down_t> || std::is_same_v<Policy, rounded_up_t>);

template<typename Policy, typename FromRep, typename ToRep>
constexpr bool rounding_policy_applies =
  RoundingPolicy<Policy> && !rounding_lacks_real_scalars<Policy, FromRep, ToRep> &&
  !rounding_lacks_standard_floating_point<Policy, FromRep, ToRep> && !rounding_direction_undeliverable<Policy, ToRep>;

template<typename Policy, typename FromRep, typename ToRep>
concept ValidRoundingPolicyFor =
  RoundingPolicy<Policy> &&
  (!rounding_lacks_real_scalars<Policy, FromRep, ToRep> ||
   unsatisfied<"Rounding policies other than 'truncated' need to compare against a rounding boundary, "
               "which requires real scalar representation types (here: '{}' to '{}')">(type_name<FromRep>(),
                                                                                       type_name<ToRep>())) &&
  (!rounding_lacks_standard_floating_point<Policy, FromRep, ToRep> ||
   unsatisfied<"Rounding a floating-point '{}' to an integral '{}' requires a standard floating-point "
               "source type">(type_name<FromRep>(), type_name<ToRep>())) &&
  (!rounding_direction_undeliverable<Policy, ToRep> ||
   unsatisfied<"A floating-point representation type '{}' rounds every conversion result to nearest; "
               "only the 'truncated' and 'rounded' policies are valid for it">(type_name<ToRep>()));

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

/**
 * @brief Returns `true` if the scaling factor from @p from to @p to is an exact positive integer.
 *
 * This is the key predicate used by the default `implicitly_scalable` to decide whether
 * conversions between integer-like representation types are non-truncating (and therefore
 * may be implicit).  For example, `m → mm` has an integral factor (×1000), so integer
 * conversions in that direction are implicit; `mm → m` has a fractional factor (÷1000),
 * so they are explicit.
 *
 * @param from source unit
 * @param to   target unit
 */
[[nodiscard]] consteval bool is_integral_scaling(Unit auto from, Unit auto to)
{
  if constexpr (std::is_same_v<decltype(from), decltype(to)>)
    return true;
  else
    return is_integral(get_canonical_unit(from).mag / get_canonical_unit(to).mag);
}

/**
 * @brief Controls whether conversion from `quantity<FromUnit, FromRep>` to
 *        `quantity<ToUnit, ToRep>` is implicit or explicit.
 *
 * The default is `true` iff the scaling is non-truncating: either `ToRep` is floating-point,
 * or both reps are non-floating-point and the unit magnitude ratio is an integral factor.
 *
 * Specialize this variable template to customize the implicit/explicit decision for
 * your own representation types.
 *
 * @tparam FromUnit the source unit value (NTTP)
 * @tparam FromRep  the source representation type
 * @tparam ToUnit   the target unit value (NTTP)
 * @tparam ToRep    the target representation type
 */
template<Unit auto FromUnit, typename FromRep, Unit auto ToUnit, typename ToRep>
constexpr bool implicitly_scalable =
  treat_as_floating_point<ToRep> || (!treat_as_floating_point<FromRep> && is_integral_scaling(FromUnit, ToUnit));

/**
 * @brief Explicit cast of a quantity's unit
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (e.g. non-truncating) conversion. In truncating cases an explicit cast have to be used.
 *
 * auto d = value_cast<si::second>(1234 * ms);
 * auto e = value_cast<si::second>(1567 * ms, rounded);
 *
 * @tparam ToU a unit to use for a target quantity
 * @param policy an optional rounding policy (`truncated` when not provided)
 */
template<auto ToU, typename FwdQ, RoundingPolicy Policy = truncated_t, Quantity Q = std::remove_cvref_t<FwdQ>>
  requires UnitOf<MP_UNITS_REMOVE_CONST(decltype(ToU)), Q::quantity_spec> &&
           detail::ExplicitlyCastable<Q::unit, ToU, typename Q::rep> &&
           detail::ValidRoundingPolicyFor<Policy, typename Q::rep, typename Q::rep>
[[nodiscard]] constexpr Quantity auto value_cast(FwdQ&& q, Policy = Policy{})
{
  return detail::sudo_cast<quantity<detail::make_reference(Q::quantity_spec, ToU), typename Q::rep>,
                           detail::rounding_mode_of<Policy>>(std::forward<FwdQ>(q));
}

/**
 * @brief Explicit cast of a quantity's representation type
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (e.g. non-truncating) conversion. In truncating cases an explicit cast have to be used.
 *
 * auto q = value_cast<int>(1.23 * ms);
 * auto r = value_cast<int>(1.55 * ms, rounded);
 *
 * @tparam ToRep a representation type to use for a target quantity
 * @param policy an optional rounding policy (`truncated` when not provided)
 */
template<typename ToRep, typename FwdQ, RoundingPolicy Policy = truncated_t, Quantity Q = std::remove_cvref_t<FwdQ>>
  requires RepresentationOf<ToRep, Q::quantity_spec> && std::constructible_from<ToRep, typename Q::rep> &&
           detail::ValidRoundingPolicyFor<Policy, typename Q::rep, ToRep>
[[nodiscard]] constexpr quantity<Q::reference, ToRep> value_cast(FwdQ&& q, Policy = Policy{})
{
  return detail::sudo_cast<quantity<Q::reference, ToRep>, detail::rounding_mode_of<Policy>>(std::forward<FwdQ>(q));
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
 * @param policy an optional rounding policy (`truncated` when not provided)
 */
template<Unit auto ToU, typename ToRep, typename FwdQ, RoundingPolicy Policy = truncated_t,
         Quantity Q = std::remove_cvref_t<FwdQ>>
  requires UnitOf<MP_UNITS_REMOVE_CONST(decltype(ToU)), Q::quantity_spec> &&
           RepresentationOf<ToRep, Q::quantity_spec> && std::constructible_from<ToRep, typename Q::rep> &&
           detail::ExplicitlyCastable<Q::unit, ToU, ToRep> &&
           detail::ValidRoundingPolicyFor<Policy, typename Q::rep, ToRep>
[[nodiscard]] constexpr Quantity auto value_cast(FwdQ&& q, Policy = Policy{})
{
  return detail::sudo_cast<quantity<detail::make_reference(Q::quantity_spec, ToU), ToRep>,
                           detail::rounding_mode_of<Policy>>(std::forward<FwdQ>(q));
}

template<typename ToRep, Unit auto ToU, typename FwdQ, RoundingPolicy Policy = truncated_t,
         Quantity Q = std::remove_cvref_t<FwdQ>>
  requires UnitOf<MP_UNITS_REMOVE_CONST(decltype(ToU)), Q::quantity_spec> &&
           RepresentationOf<ToRep, Q::quantity_spec> && std::constructible_from<ToRep, typename Q::rep> &&
           detail::ExplicitlyCastable<Q::unit, ToU, ToRep> &&
           detail::ValidRoundingPolicyFor<Policy, typename Q::rep, ToRep>
[[nodiscard]] constexpr Quantity auto value_cast(FwdQ&& q, Policy policy = Policy{})
{
  return value_cast<ToU, ToRep>(std::forward<FwdQ>(q), policy);
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
 * @param policy an optional rounding policy (`truncated` when not provided)
 */
template<Quantity ToQ, typename FwdQ, RoundingPolicy Policy = truncated_t, Quantity Q = std::remove_cvref_t<FwdQ>>
  requires(ToQ::quantity_spec == Q::quantity_spec) && UnitOf<MP_UNITS_NONCONST_TYPE(ToQ::unit), Q::quantity_spec> &&
          std::constructible_from<typename ToQ::rep, typename Q::rep> &&
          detail::ExplicitlyCastable<Q::unit, ToQ::unit, typename ToQ::rep> &&
          detail::ValidRoundingPolicyFor<Policy, typename Q::rep, typename ToQ::rep>
[[nodiscard]] constexpr Quantity auto value_cast(FwdQ&& q, Policy = Policy{})
{
  return detail::sudo_cast<ToQ, detail::rounding_mode_of<Policy>>(std::forward<FwdQ>(q));
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
 * @param policy an optional rounding policy (`truncated` when not provided)
 */
template<Unit auto ToU, typename FwdQP, RoundingPolicy Policy = truncated_t,
         QuantityPoint QP = std::remove_cvref_t<FwdQP>>
  requires UnitOf<MP_UNITS_REMOVE_CONST(decltype(ToU)), QP::quantity_spec> &&
           detail::ExplicitlyCastable<QP::unit, ToU, typename QP::rep> &&
           detail::ValidRoundingPolicyFor<Policy, typename QP::rep, typename QP::rep>
[[nodiscard]] constexpr QuantityPoint auto value_cast(FwdQP&& qp, Policy policy = Policy{})
{
  return quantity_point{
    value_cast<ToU>(std::forward<FwdQP>(qp).quantity_from_origin_is_an_implementation_detail_, policy),
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
 * @param policy an optional rounding policy (`truncated` when not provided)
 */
template<typename ToRep, typename FwdQP, RoundingPolicy Policy = truncated_t,
         QuantityPoint QP = std::remove_cvref_t<FwdQP>>
  requires RepresentationOf<ToRep, QP::quantity_spec> && std::constructible_from<ToRep, typename QP::rep> &&
           detail::ValidRoundingPolicyFor<Policy, typename QP::rep, ToRep>
[[nodiscard]] constexpr quantity_point<QP::reference, QP::point_origin, ToRep> value_cast(FwdQP&& qp,
                                                                                          Policy policy = Policy{})
{
  return {value_cast<ToRep>(std::forward<FwdQP>(qp).quantity_from_origin_is_an_implementation_detail_, policy),
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
 * @param policy an optional rounding policy (`truncated` when not provided)
 */
template<Unit auto ToU, typename ToRep, typename FwdQP, RoundingPolicy Policy = truncated_t,
         QuantityPoint QP = std::remove_cvref_t<FwdQP>>
  requires UnitOf<MP_UNITS_REMOVE_CONST(decltype(ToU)), QP::quantity_spec> &&
           RepresentationOf<ToRep, QP::quantity_spec> && std::constructible_from<ToRep, typename QP::rep> &&
           detail::ExplicitlyCastable<QP::unit, ToU, ToRep> &&
           detail::ValidRoundingPolicyFor<Policy, typename QP::rep, ToRep>
[[nodiscard]] constexpr QuantityPoint auto value_cast(FwdQP&& qp, Policy policy = Policy{})
{
  return quantity_point{
    value_cast<ToU, ToRep>(std::forward<FwdQP>(qp).quantity_from_origin_is_an_implementation_detail_, policy),
    QP::point_origin};
}

template<typename ToRep, Unit auto ToU, typename FwdQP, RoundingPolicy Policy = truncated_t,
         QuantityPoint QP = std::remove_cvref_t<FwdQP>>
  requires UnitOf<MP_UNITS_REMOVE_CONST(decltype(ToU)), QP::quantity_spec> &&
           RepresentationOf<ToRep, QP::quantity_spec> && std::constructible_from<ToRep, typename QP::rep> &&
           detail::ExplicitlyCastable<QP::unit, ToU, ToRep> &&
           detail::ValidRoundingPolicyFor<Policy, typename QP::rep, ToRep>
[[nodiscard]] constexpr QuantityPoint auto value_cast(FwdQP&& qp, Policy policy = Policy{})
{
  return value_cast<ToU, ToRep>(std::forward<FwdQP>(qp), policy);
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
 * @param policy an optional rounding policy (`truncated` when not provided)
 */
template<Quantity ToQ, typename FwdQP, RoundingPolicy Policy = truncated_t,
         QuantityPoint QP = std::remove_cvref_t<FwdQP>>
  requires(ToQ::quantity_spec == QP::quantity_spec) && UnitOf<MP_UNITS_NONCONST_TYPE(ToQ::unit), QP::quantity_spec> &&
          std::constructible_from<typename ToQ::rep, typename QP::rep> &&
          detail::ExplicitlyCastable<QP::unit, ToQ::unit, typename ToQ::rep> &&
          detail::ValidRoundingPolicyFor<Policy, typename QP::rep, typename ToQ::rep>
[[nodiscard]] constexpr QuantityPoint auto value_cast(FwdQP&& qp, Policy policy = Policy{})
{
  return quantity_point{
    value_cast<ToQ>(std::forward<FwdQP>(qp).quantity_from_origin_is_an_implementation_detail_, policy),
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
 * plus the resolution of `qp` as represented in `ToQP`.
 *
 * @tparam ToQP a target quantity point type to which to cast the representation of the point
 * @param policy an optional rounding policy (`truncated` when not provided)
 */
template<QuantityPoint ToQP, typename FwdQP, RoundingPolicy Policy = truncated_t,
         QuantityPoint QP = std::remove_cvref_t<FwdQP>>
  requires(ToQP::quantity_spec == QP::quantity_spec) && UnitOf<MP_UNITS_NONCONST_TYPE(ToQP::unit), QP::quantity_spec> &&
          (detail::same_absolute_point_origins(ToQP::point_origin, QP::point_origin)) &&
          std::constructible_from<typename ToQP::rep, typename QP::rep> &&
          detail::ExplicitlyCastable<QP::unit, ToQP::unit, typename ToQP::rep> &&
          detail::ValidRoundingPolicyFor<Policy, typename QP::rep, typename ToQP::rep>
[[nodiscard]] constexpr QuantityPoint auto value_cast(FwdQP&& qp, Policy = Policy{})
{
  return detail::sudo_cast<ToQP, detail::rounding_mode_of<Policy>>(std::forward<FwdQP>(qp));
}

MP_UNITS_EXPORT_END

}  // namespace mp_units
