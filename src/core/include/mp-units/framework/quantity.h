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
#include <mp-units/bits/config.h>
#include <mp-units/bits/diagnostics.h>
#include <mp-units/bits/hacks.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/bits/sudo_cast.h>
#include <mp-units/bits/unsatisfied.h>
#include <mp-units/compat_macros.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/dimension_concepts.h>
#include <mp-units/framework/quantity_concepts.h>
#include <mp-units/framework/quantity_point_concepts.h>
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/reference.h>
#include <mp-units/framework/reference_concepts.h>
#include <mp-units/framework/representation_concepts.h>
#include <mp-units/framework/rounding.h>
#include <mp-units/framework/unit_concepts.h>
#include <mp-units/framework/value_cast.h>
#include <mp-units/framework/vector_components.h>
#if MP_UNITS_HOSTED
#include <mp-units/bits/ostream.h>
#include <mp-units/utility/format.h>
#endif

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/ext/contracts.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <compare>  // IWYU pragma: export
#include <concepts>
#include <functional>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>
#if MP_UNITS_HOSTED
#if !MP_UNITS_USE_FMTLIB
#include <locale>  // for `vformat_to` in the width path of the quantity formatter
#endif
#include <stdexcept>
#endif
#endif
#endif

namespace mp_units {

namespace detail {

struct zero {
  template<typename T>
    requires std::is_arithmetic_v<T> && (!std::is_same_v<T, bool>)
  consteval explicit(false) zero(T val)
  {
    if (val != T{0}) MP_UNITS_THROW(std::logic_error("not zero"));
  }
};


template<typename T, typename Arg>
concept RepConstructibleFrom =
  std::constructible_from<T, Arg> &&
  (treat_as_floating_point<T> || !treat_as_floating_point<std::remove_cvref_t<Arg>> ||
   unsatisfied<"Conversion from floating-point '{}' to non-floating-point '{}' is truncating">(
     type_name<std::remove_cvref_t<Arg>>(), type_name<T>()));

template<typename T, typename Arg>
concept RepAssignableFrom =
  std::assignable_from<T&, Arg> &&
  (treat_as_floating_point<T> || !treat_as_floating_point<std::remove_cvref_t<Arg>> ||
   unsatisfied<"Assignment from floating-point '{}' to non-floating-point '{}' is truncating">(
     type_name<std::remove_cvref_t<Arg>>(), type_name<T>()));

template<auto FromUnit, auto ToUnit, typename Rep>
concept ImplicitScaling = ExplicitlyCastable<FromUnit, ToUnit, Rep> &&
                          (mp_units::implicitly_scalable<FromUnit, Rep, ToUnit, Rep> ||
                           unsatisfied<"Scaling from '{}' to '{}' is truncating for '{}' representation type">(
                             unit_symbol(FromUnit), unit_symbol(ToUnit), type_name<Rep>()));

template<auto FromUnit, typename FromRep, auto ToUnit, typename ToRep>
concept ImplicitConversion = ExplicitlyCastable<FromUnit, ToUnit, ToRep> &&
                             (mp_units::implicitly_scalable<FromUnit, FromRep, ToUnit, ToRep> ||
                              unsatisfied<"Conversion from '{}' as '{}' to '{}' as '{}' is truncating">(
                                unit_symbol(FromUnit), type_name<FromRep>(), unit_symbol(ToUnit), type_name<ToRep>()));

template<typename QTo, typename QFrom>
concept QuantityConstructibleFrom =
  Quantity<QTo> && Quantity<QFrom> &&
  mp_units::utility::explicitly_convertible(QFrom::quantity_spec, QTo::quantity_spec) &&
  RepConstructibleFrom<typename QTo::rep, typename QFrom::rep> &&
  ImplicitConversion<QFrom::unit, typename QFrom::rep, QTo::unit, typename QTo::rep>;

template<typename T, typename Rep>
concept ScalarRepConvertible =
  !Quantity<T> && Scalar<T> &&
  (treat_as_floating_point<Rep> || !treat_as_floating_point<T> ||
   unsatisfied<"Scaling a non-floating-point '{}' quantity by a floating-point '{}' scalar is truncating">(
     type_name<Rep>(), type_name<T>()));

template<auto R>
concept UnitOne = Reference<MP_UNITS_NTTP_HACK(decltype(R))> &&
                  (equivalent(get_unit(R), one) && detail::get_associated_quantity(get_unit(R)) == dimensionless);

template<auto R>
concept ExplicitFromNumber =
  UnitOne<R> && (mp_units::utility::explicitly_convertible(get_quantity_spec(R), dimensionless));

template<auto R>
concept ImplicitFromNumber =
  ExplicitFromNumber<R> && (mp_units::utility::implicitly_convertible(get_quantity_spec(R), dimensionless));

template<typename Q>
concept ImplicitFromNumberQuantity = Quantity<Q> && ImplicitFromNumber<Q::reference>;

// The number operand of `+`, `-` and `%`. The comparisons keep the bare `ImplicitFromNumber`, because
// `q == 0` on a dimensioned quantity is valid and evaluates it to false on the way to the `zero`
// overload, so a sentence there would be a wrong diagnosis.
template<auto R>
concept ImplicitFromNumberOperand =
  ImplicitFromNumber<R> ||
  unsatisfied<"'{}' is not a dimensionless quantity in unit 'one', which a raw number operand requires">(
    type_name(remove_kind(get_quantity_spec(R))));

// The invocation result is canonicalized (see `representation_canonical_type`) so that
// expression-template results (e.g. Eigen, Blaze) are checked and stored as their evaluated
// concrete type rather than the lazy proxy type.
template<auto QS, typename Func, typename T, typename U>
concept InvokeResultOf = QuantitySpec<MP_UNITS_NTTP_HACK(decltype(QS))> && std::regular_invocable<Func, T, U> &&
                         RepresentationOf<representation_canonical_type_t<std::invoke_result_t<Func, T, U>>, QS>;

template<typename Func, typename Q1, typename Q2,
         auto QS = std::invoke_result_t<Func, MP_UNITS_NONCONST_TYPE(Q1::quantity_spec),
                                        MP_UNITS_NONCONST_TYPE(Q2::quantity_spec)>{}>
concept InvocableQuantities = QuantitySpec<MP_UNITS_NTTP_HACK(decltype(QS))> && Quantity<Q1> && Quantity<Q2> &&
                              InvokeResultOf<QS, Func, typename Q1::rep, typename Q2::rep>;

// A representation whose tensor order is that of a vector or tensor (order >= 1). Written as a nested
// requirement so a type for which `tensor_order` is unspecified is simply not satisfied rather than
// a hard error.
template<typename T>
concept OrderRaisingRepresentation = requires { requires tensor_order<T> >= 1; };

// Like `InvokeResultOf`, but also accepts the case where scaling *raises the tensor order*: a
// scalar-character quantity multiplied by a vector or tensor representation. The specific scalar spec
// `QS` can no longer describe such a result and the framework cannot infer the concrete vector/tensor
// sibling (e.g. that `speed`'s vector sibling is `velocity`), so the result is expressed over `QS`'s
// tensor-order-agnostic *kind* and the caller pins the concrete spec. Deliberately limited to a
// *scalar* `QS` scaled up on the tensor-order axis: a vector/tensor `QS` is never demoted this way, and
// a real->complex field mismatch is not bridged either.
template<auto QS>
concept ScalarCharacterSpec =
  QuantitySpec<MP_UNITS_NTTP_HACK(decltype(QS))> && admits(QS._character_.order, quantity_tensor_order::scalar);

template<auto QS, typename Func, typename T, typename U>
concept ScalableResultOf =
  QuantitySpec<MP_UNITS_NTTP_HACK(decltype(QS))> && std::regular_invocable<Func, T, U> &&
  (RepresentationOf<representation_canonical_type_t<std::invoke_result_t<Func, T, U>>, QS> ||
   (ScalarCharacterSpec<QS> &&
    OrderRaisingRepresentation<representation_canonical_type_t<std::invoke_result_t<Func, T, U>>> &&
    RepresentationOf<representation_canonical_type_t<std::invoke_result_t<Func, T, U>>, get_kind(QS)>));

// The reference of a scaled result. When the result representation still matches the reference's spec
// it is returned unchanged; otherwise (the order-raising case above) the spec is replaced by its kind
// over the same unit.
template<Reference auto R, typename Res>
[[nodiscard]] consteval Reference auto scaled_reference()
{
  if constexpr (RepresentationOf<Res, get_quantity_spec(R)>)
    return R;
  else
    return get_kind(get_quantity_spec(R))[get_unit(R)];
}

// Quantities whose specifications have a common one, which is what adding, subtracting and comparing
// them needs. Asked before the common reference is formed, so that `length + time` is reported in these
// words rather than as an invalid call to `get_common_reference`. Only the two-quantity overloads of
// the operators and `hypot` evaluate it, and only with operands that are the mistake, so the sentence
// is not gated.
template<auto QS1, auto QS2>
concept QuantitySpecsOfTheSameKind =
  have_common_quantity_spec(QS1, QS2) ||
  unsatisfied<"'{}' and '{}' are quantities of different kinds and have no common quantity">(
    type_name(remove_kind(QS1)), type_name(remove_kind(QS2)));

template<typename Q1, typename Q2>
concept QuantitiesOfTheSameKind =
  Quantity<Q1> && Quantity<Q2> && QuantitySpecsOfTheSameKind<Q1::quantity_spec, Q2::quantity_spec>;

template<auto R1, auto R2>
concept HaveCommonReference = requires { mp_units::get_common_reference(R1, R2); };

template<typename Func, Quantity Q1, Quantity Q2>
using common_quantity_for =
  quantity<mp_units::get_common_reference(Q1::reference, Q2::reference),
           representation_canonical_type_t<std::invoke_result_t<Func, typename Q1::rep, typename Q2::rep>>>;

template<typename Rep, Unit U1, Unit U2>
[[nodiscard]] consteval bool overflows_non_zero_common_values(U1 u1, U2 u2)
{
  constexpr Unit auto cu = mp_units::get_common_unit(U1{}, U2{});
  return scaling_overflows_non_zero_values<Rep>(u1, cu) || scaling_overflows_non_zero_values<Rep>(u2, cu);
}

// `InvocableQuantities` precedes the conversions to the common quantity because it names the
// representations that have no sum, whereas `common_quantity_for` would report an ill-formed
// `std::invoke_result` for a vector added to a scalar.
template<typename Func, typename Q1, typename Q2>
concept CommonlyInvocableQuantities =
  QuantitiesOfTheSameKind<Q1, Q2> && HaveCommonReference<Q1::reference, Q2::reference> &&
  InvocableQuantities<Func, Q1, Q2, mp_units::get_common_quantity_spec(Q1::quantity_spec, Q2::quantity_spec)> &&
  std::convertible_to<Q1, common_quantity_for<Func, Q1, Q2>> &&
  std::convertible_to<Q2, common_quantity_for<Func, Q1, Q2>> &&
  !overflows_non_zero_common_values<
    representation_canonical_type_t<std::invoke_result_t<Func, typename Q1::rep, typename Q2::rep>>>(Q1::unit,
                                                                                                     Q2::unit);

// Returns true if the unit ratio between Q1 and Q2, when scaled to the common unit, fits within
// the double-width integer type for ct_rep.  For floating-point or already-widest integer
// representations no check is needed (float is always fine; int128 falls back to the direct
// narrowing path which conservative-assumes no overflow for that type).
template<typename Q1, typename Q2>
[[nodiscard]] consteval bool comparable_in_wide_type()
{
  using ct = std::common_type_t<Q1, Q2>;
  using ct_rep = value_type_t<typename ct::rep>;
  if constexpr (!treat_as_floating_point<ct_rep> && sizeof(ct_rep) < sizeof(int128_t))
    return !overflows_non_zero_common_values<double_width_int_for_t<ct_rep>>(Q1::unit, Q2::unit);
  else
    return true;
}

template<typename Q1, typename Q2>
concept CommonlyComparableQuantities =
  QuantitiesOfTheSameKind<Q1, Q2> && HaveCommonReference<Q1::reference, Q2::reference> &&
  requires { typename std::common_type_t<Q1, Q2>; } && comparable_in_wide_type<Q1, Q2>();

// Sign-aware integer comparisons with `std::cmp_less`/`std::cmp_equal` semantics.  Written by
// hand rather than reusing the standard ones so they also work with the extended 128-bit
// integer types, which `std::cmp_*` reject in strict `-std=c++20` mode (`std::is_signed` is not
// specialized for `__int128` there).  Only ever used for two integers of *different* signedness
// (same-signedness comparisons stay on the ordinary path), where comparing through the common
// unsigned type would silently convert a negative value into a large positive one — the trap
// behind both `-1 < 1u` (wrongly ordered) and `-1 == 4294967295u` (wrongly equal).  See GH #703.
template<typename L, typename R>
[[nodiscard]] constexpr bool cmp_less(L lhs, R rhs)
{
  if constexpr (is_signed_v<L> == is_signed_v<R>)
    return lhs < rhs;
  else if constexpr (is_signed_v<L>)
    return lhs < 0 || static_cast<min_width_uint_t<integer_rep_width_v<L>>>(lhs) < rhs;
  else
    return rhs >= 0 && lhs < static_cast<min_width_uint_t<integer_rep_width_v<R>>>(rhs);
}

template<typename L, typename R>
[[nodiscard]] constexpr bool cmp_equal(L lhs, R rhs)
{
  if constexpr (is_signed_v<L> == is_signed_v<R>)
    return lhs == rhs;
  else if constexpr (is_signed_v<L>)
    return lhs >= 0 && static_cast<min_width_uint_t<integer_rep_width_v<L>>>(lhs) == rhs;
  else
    return rhs >= 0 && lhs == static_cast<min_width_uint_t<integer_rep_width_v<R>>>(rhs);
}

template<typename L, typename R>
[[nodiscard]] constexpr std::strong_ordering cmp_three_way(L lhs, R rhs)
{
  if (detail::cmp_less(lhs, rhs)) return std::strong_ordering::less;
  if (detail::cmp_less(rhs, lhs)) return std::strong_ordering::greater;
  return std::strong_ordering::equal;
}

template<typename Q1, typename Q2>
concept DifferentlySignedIntegers =
  Quantity<Q1> && Quantity<Q2> && integral<typename Q1::rep> && integral<typename Q2::rep> &&
  (is_signed_v<typename Q1::rep> != is_signed_v<typename Q2::rep>);

template<typename LhsRep, typename RhsRep, UnitMagnitude M1, typename V1, UnitMagnitude M2, typename V2, typename Cmp>
[[nodiscard]] constexpr auto scaled_sign_aware_compare(M1 lhs_m, const V1& lhs_val, M2 rhs_m, const V2& rhs_val, Cmp)
{
  const auto lhs = scale<LhsRep>(lhs_m, lhs_val);
  const auto rhs = scale<RhsRep>(rhs_m, rhs_val);
  if constexpr (std::is_same_v<Cmp, std::compare_three_way>)
    return detail::cmp_three_way(lhs, rhs);
  else
    return detail::cmp_equal(lhs, rhs);
}

template<typename Q1, typename Q2, typename Cmp>
[[nodiscard]] constexpr auto compare_differently_signed_integers(const Q1& lhs, const Q2& rhs, Cmp cmp)
{
  using ct = std::common_type_t<Q1, Q2>;
  using ct_rep = value_type_t<typename ct::rep>;
  constexpr UnitMagnitude auto lhs_m = get_canonical_unit(Q1::unit).mag / get_canonical_unit(ct::unit).mag;
  constexpr UnitMagnitude auto rhs_m = get_canonical_unit(Q2::unit).mag / get_canonical_unit(ct::unit).mag;
  const auto& lhs_val = lhs.numerical_value_is_an_implementation_detail_;
  const auto& rhs_val = rhs.numerical_value_is_an_implementation_detail_;
  if constexpr (sizeof(ct_rep) < sizeof(int128_t)) {
    constexpr std::size_t w = integer_rep_width_v<double_width_int_for_t<ct_rep>>;
    using lhs_rep = conditional<is_signed_v<typename Q1::rep>, min_width_int_t<w>, min_width_uint_t<w>>;
    using rhs_rep = conditional<is_signed_v<typename Q2::rep>, min_width_int_t<w>, min_width_uint_t<w>>;
    return scaled_sign_aware_compare<lhs_rep, rhs_rep>(lhs_m, lhs_val, rhs_m, rhs_val, cmp);
  } else {
    using lhs_rep = conditional<is_signed_v<typename Q1::rep>, int128_t, uint128_t>;
    using rhs_rep = conditional<is_signed_v<typename Q2::rep>, int128_t, uint128_t>;
    return scaled_sign_aware_compare<lhs_rep, rhs_rep>(lhs_m, lhs_val, rhs_m, rhs_val, cmp);
  }
}

template<typename Q1, typename Q2, typename Cmp>
[[nodiscard]] constexpr auto compare_in_common_type(const Q1& lhs, const Q2& rhs, Cmp cmp)
{
  using ct = std::common_type_t<Q1, Q2>;
  using ct_rep = value_type_t<typename ct::rep>;
  // `fallback_cmp` is a lambda so it can be invoked from either branch below — a bare fallthrough
  // `return` after the `if constexpr` chain would trip MSVC Debug C4702 ("unreachable code") on
  // instantiations where the optimization branch returns.
  auto fallback_cmp = [&] {
    const ct ct_lhs(lhs);
    const ct ct_rhs(rhs);
    MP_UNITS_DIAGNOSTIC_PUSH
    MP_UNITS_DIAGNOSTIC_IGNORE_FLOAT_EQUAL
    return cmp(ct_lhs.numerical_value_ref_in(ct::unit), ct_rhs.numerical_value_ref_in(ct::unit));
    MP_UNITS_DIAGNOSTIC_POP
  };
  if constexpr (!treat_as_floating_point<ct_rep> && sizeof(ct_rep) < sizeof(int128_t)) {
    // widen to a double-width integer so the unit-ratio scaling can't overflow
    using wide_t = double_width_int_for_t<ct_rep>;
    if constexpr (!overflows_non_zero_common_values<wide_t>(Q1::unit, Q2::unit)) {
      constexpr UnitMagnitude auto lhs_m = get_canonical_unit(Q1::unit).mag / get_canonical_unit(ct::unit).mag;
      constexpr UnitMagnitude auto rhs_m = get_canonical_unit(Q2::unit).mag / get_canonical_unit(ct::unit).mag;
      return cmp(scale<wide_t>(lhs_m, lhs.numerical_value_is_an_implementation_detail_),
                 scale<wide_t>(rhs_m, rhs.numerical_value_is_an_implementation_detail_));
    } else {
      return fallback_cmp();
    }
  } else {
    return fallback_cmp();
  }
}

template<typename Q1, typename Q2, typename Cmp>
  requires CommonlyComparableQuantities<Q1, Q2>
[[nodiscard]] constexpr auto compare_quantities(const Q1& lhs, const Q2& rhs, Cmp cmp)
{
  if constexpr (DifferentlySignedIntegers<Q1, Q2>)
    return compare_differently_signed_integers(lhs, rhs, cmp);
  else
    return compare_in_common_type(lhs, rhs, cmp);
}

template<typename T>
using quantity_like_type = quantity<quantity_like_traits<T>::reference, typename quantity_like_traits<T>::rep>;

// Hidden-friend interface for `quantity`. Provides heterogeneous binary and comparison
// operators between any two `quantity<R1, Rep1>` and `quantity<R2, Rep2>` specializations
// (and between a `quantity` and a raw representation value). Made findable only via ADL.
struct quantity_iface {
  // operator+
  template<auto R1, typename Rep1, auto R2, typename Rep2>
    requires CommonlyInvocableQuantities<std::plus<>, quantity<R1, Rep1>, quantity<R2, Rep2>>
  [[nodiscard]] friend constexpr Quantity auto operator+(const quantity<R1, Rep1>& lhs, const quantity<R2, Rep2>& rhs)
  {
    using ret = common_quantity_for<std::plus<>, quantity<R1, Rep1>, quantity<R2, Rep2>>;
    const ret ret_lhs(lhs);
    const ret ret_rhs(rhs);
    return quantity{ret_lhs.numerical_value_ref_in(ret::unit) + ret_rhs.numerical_value_ref_in(ret::unit),
                    ret::reference};
  }

  template<auto R1, typename Rep1, RepresentationOf<get_quantity_spec(R1)> Value>
    requires ImplicitFromNumberOperand<R1> && InvokeResultOf<get_quantity_spec(R1), std::plus<>, Rep1, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator+(const quantity<R1, Rep1>& lhs, const Value& rhs)
  {
    return lhs + quantity{rhs};
  }

  template<auto R1, typename Rep1, RepresentationOf<get_quantity_spec(R1)> Value>
    requires ImplicitFromNumberOperand<R1> && InvokeResultOf<get_quantity_spec(R1), std::plus<>, Rep1, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator+(const Value& lhs, const quantity<R1, Rep1>& rhs)
  {
    return quantity{lhs} + rhs;
  }

  // operator-
  template<auto R1, typename Rep1, auto R2, typename Rep2>
    requires CommonlyInvocableQuantities<std::minus<>, quantity<R1, Rep1>, quantity<R2, Rep2>>
  [[nodiscard]] friend constexpr Quantity auto operator-(const quantity<R1, Rep1>& lhs, const quantity<R2, Rep2>& rhs)
  {
    using ret = common_quantity_for<std::minus<>, quantity<R1, Rep1>, quantity<R2, Rep2>>;
    const ret ret_lhs(lhs);
    const ret ret_rhs(rhs);
    return quantity{ret_lhs.numerical_value_ref_in(ret::unit) - ret_rhs.numerical_value_ref_in(ret::unit),
                    ret::reference};
  }

  template<auto R1, typename Rep1, RepresentationOf<get_quantity_spec(R1)> Value>
    requires ImplicitFromNumberOperand<R1> && InvokeResultOf<get_quantity_spec(R1), std::minus<>, Rep1, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator-(const quantity<R1, Rep1>& lhs, const Value& rhs)
  {
    return lhs - quantity{rhs};
  }

  template<auto R1, typename Rep1, RepresentationOf<get_quantity_spec(R1)> Value>
    requires ImplicitFromNumberOperand<R1> && InvokeResultOf<get_quantity_spec(R1), std::minus<>, Rep1, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator-(const Value& lhs, const quantity<R1, Rep1>& rhs)
  {
    return quantity{lhs} - rhs;
  }

  // operator%
  template<auto R1, typename Rep1, auto R2, typename Rep2>
    requires(!treat_as_floating_point<Rep1>) && (!treat_as_floating_point<Rep2>) &&
            CommonlyInvocableQuantities<std::modulus<>, quantity<R1, Rep1>, quantity<R2, Rep2>>
  [[nodiscard]] friend constexpr Quantity auto operator%(const quantity<R1, Rep1>& lhs, const quantity<R2, Rep2>& rhs)
  {
    MP_UNITS_PRECONDITION_DEBUG(rhs != 0);
    using ret = common_quantity_for<std::modulus<>, quantity<R1, Rep1>, quantity<R2, Rep2>>;
    const ret ret_lhs(lhs);
    const ret ret_rhs(rhs);
    return quantity{ret_lhs.numerical_value_ref_in(ret::unit) % ret_rhs.numerical_value_ref_in(ret::unit),
                    ret::reference};
  }

  template<auto R1, typename Rep1, RepresentationOf<get_quantity_spec(R1)> Value>
    requires ImplicitFromNumberOperand<R1> && InvokeResultOf<get_quantity_spec(R1), std::modulus<>, Rep1, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator%(const quantity<R1, Rep1>& lhs, const Value& rhs)
  {
    return lhs % quantity{rhs};
  }

  template<auto R1, typename Rep1, RepresentationOf<get_quantity_spec(R1)> Value>
    requires ImplicitFromNumberOperand<R1> && InvokeResultOf<get_quantity_spec(R1), std::modulus<>, Rep1, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator%(const Value& lhs, const quantity<R1, Rep1>& rhs)
  {
    return quantity{lhs} % rhs;
  }

  // operator*
  template<auto R1, typename Rep1, auto R2, typename Rep2>
    requires InvocableQuantities<std::multiplies<>, quantity<R1, Rep1>, quantity<R2, Rep2>>
  [[nodiscard]] friend constexpr Quantity auto operator*(const quantity<R1, Rep1>& lhs, const quantity<R2, Rep2>& rhs)
  {
    return quantity{lhs.numerical_value_ref_in(get_unit(R1)) * rhs.numerical_value_ref_in(get_unit(R2)), R1 * R2};
  }

  template<auto R1, typename Rep1, typename Value>
    requires(!Quantity<Value>) && (!Reference<Value>) &&
            ScalableResultOf<get_quantity_spec(R1), std::multiplies<>, Rep1, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator*(const quantity<R1, Rep1>& q, const Value& val)
  {
    return quantity{
      q.numerical_value_ref_in(get_unit(R1)) * val,
      scaled_reference<R1,
                       representation_canonical_type_t<std::invoke_result_t<std::multiplies<>, Rep1, const Value&>>>()};
  }

  template<typename Value, auto R1, typename Rep1>
    requires(!Quantity<Value>) && (!Reference<Value>) &&
            ScalableResultOf<get_quantity_spec(R1), std::multiplies<>, const Value&, Rep1>
  [[nodiscard]] friend constexpr Quantity auto operator*(const Value& val, const quantity<R1, Rep1>& q)
  {
    return quantity{
      val * q.numerical_value_ref_in(get_unit(R1)),
      scaled_reference<R1,
                       representation_canonical_type_t<std::invoke_result_t<std::multiplies<>, const Value&, Rep1>>>()};
  }

  // operator/
  template<auto R1, typename Rep1, auto R2, typename Rep2>
    requires InvocableQuantities<std::divides<>, quantity<R1, Rep1>, quantity<R2, Rep2>>
  [[nodiscard]] friend constexpr Quantity auto operator/(const quantity<R1, Rep1>& lhs, const quantity<R2, Rep2>& rhs)
  {
    MP_UNITS_PRECONDITION_DEBUG(rhs != 0);
    return quantity{lhs.numerical_value_ref_in(get_unit(R1)) / rhs.numerical_value_ref_in(get_unit(R2)), R1 / R2};
  }

  template<auto R1, typename Rep1, typename Value>
    requires(!Quantity<Value>) && (!Reference<Value>) &&
            InvokeResultOf<get_quantity_spec(R1), std::divides<>, Rep1, const Value&>
  [[nodiscard]] friend constexpr QuantityOf<get_quantity_spec(R1)> auto operator/(const quantity<R1, Rep1>& q,
                                                                                  const Value& val)
  {
    MP_UNITS_PRECONDITION_DEBUG(val != representation_values<Value>::zero());
    return quantity{q.numerical_value_ref_in(get_unit(R1)) / val, R1};
  }

  template<typename Value, auto R1, typename Rep1>
    requires(!Quantity<Value>) && (!Reference<Value>) &&
            ScalableResultOf<get_quantity_spec(one / R1), std::divides<>, const Value&, Rep1>
  [[nodiscard]] friend constexpr Quantity auto operator/(const Value& val, const quantity<R1, Rep1>& q)
  {
    MP_UNITS_PRECONDITION_DEBUG(q != 0);
    return quantity{
      val / q.numerical_value_ref_in(get_unit(R1)),
      scaled_reference<one / R1,
                       representation_canonical_type_t<std::invoke_result_t<std::divides<>, const Value&, Rep1>>>()};
  }

  // operator==
  template<auto R1, typename Rep1, auto R2, typename Rep2>
    requires CommonlyComparableQuantities<quantity<R1, Rep1>, quantity<R2, Rep2>> &&
             std::equality_comparable<typename std::common_type_t<quantity<R1, Rep1>, quantity<R2, Rep2>>::rep>
  [[nodiscard]] friend constexpr bool operator==(const quantity<R1, Rep1>& lhs, const quantity<R2, Rep2>& rhs)
  {
    return compare_quantities(lhs, rhs, std::equal_to{});
  }

  template<auto R1, typename Rep1, RepresentationOf<get_quantity_spec(R1)> Value>
    requires ImplicitFromNumber<R1> && std::equality_comparable_with<Rep1, Value>
  [[nodiscard]] friend constexpr bool operator==(const quantity<R1, Rep1>& lhs, const Value& rhs)
  {
    return lhs.numerical_value_ref_in(get_unit(R1)) == rhs;
  }

  template<auto R1, typename Rep1>
    requires std::equality_comparable<Rep1> && requires { representation_values<Rep1>::zero(); }
  [[nodiscard]] friend constexpr bool operator==(const quantity<R1, Rep1>& lhs, zero)
  {
    return lhs.numerical_value_ref_in(get_unit(R1)) == representation_values<Rep1>::zero();
  }

  // operator<=>
  template<auto R1, typename Rep1, auto R2, typename Rep2>
    requires CommonlyComparableQuantities<quantity<R1, Rep1>, quantity<R2, Rep2>> &&
             std::three_way_comparable<typename std::common_type_t<quantity<R1, Rep1>, quantity<R2, Rep2>>::rep>
  [[nodiscard]] friend constexpr auto operator<=>(const quantity<R1, Rep1>& lhs, const quantity<R2, Rep2>& rhs)
  {
    return compare_quantities(lhs, rhs, std::compare_three_way{});
  }

  template<auto R1, typename Rep1, RepresentationOf<get_quantity_spec(R1)> Value>
    requires ImplicitFromNumber<R1> && std::three_way_comparable_with<Rep1, Value>
  [[nodiscard]] friend constexpr auto operator<=>(const quantity<R1, Rep1>& lhs, const Value& rhs)
  {
    return lhs.numerical_value_ref_in(get_unit(R1)) <=> rhs;
  }

  template<auto R1, typename Rep1>
    requires std::three_way_comparable<Rep1> && requires { representation_values<Rep1>::zero(); }
  [[nodiscard]] friend constexpr auto operator<=>(const quantity<R1, Rep1>& lhs, zero)
  {
    return lhs.numerical_value_ref_in(get_unit(R1)) <=> representation_values<Rep1>::zero();
  }

  // tuple-like decomposition of a vector quantity into 1D-vector components (see
  // framework/vector_components.h). A component carries its declared axis spec when the whole
  // declares its axes, and the whole's kind otherwise.
  template<std::size_t Idx, auto R1, typename Rep1>
    requires DecomposableIndex<get_quantity_spec(R1), Rep1, Idx>
  [[nodiscard]] friend constexpr Quantity auto get(const quantity<R1, Rep1>& q)
  {
    constexpr auto comp = component_spec<get_quantity_spec(R1), Rep1, Idx>();
    return component_access<Idx>(q.numerical_value_in(get_unit(R1))) * comp[get_unit(R1)];
  }

  template<QuantitySpec auto QS, auto R1, typename Rep1>
    requires DecomposableAxis<get_quantity_spec(R1), Rep1, QS>
  [[nodiscard]] friend constexpr Quantity auto get(const quantity<R1, Rep1>& q)
  {
    return get<axis_index_of<QS>(declared_axes<get_quantity_spec(R1)>{})>(q);
  }

#if MP_UNITS_HOSTED

  template<typename CharT, typename Traits, auto R, typename Rep>
  friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const quantity<R, Rep>& q)
    requires requires { os << q.numerical_value_ref_in(q.unit); }
  {
    return detail::to_stream(os, [&](std::basic_ostream<CharT, Traits>& oss) {
      if constexpr (std::is_same_v<Rep, std::uint8_t> || std::is_same_v<Rep, std::int8_t>)
        // promote the value to int
        oss << +q.numerical_value_ref_in(q.unit);
      else
        oss << q.numerical_value_ref_in(q.unit);
      if constexpr (space_before_unit_symbol<get_unit(R)>) oss << " ";
      oss << q.unit;
    });
  }

#endif  // MP_UNITS_HOSTED
};

// Component-wise construction of a vector quantity from its component quantities, the inverse of the
// decomposition above and available in both of its modes, so that what `get` takes apart can be put
// back together. Declaring the axes upgrades what is checked, exactly as it does for decomposition:
// each argument is then matched against the spec declared for its position rather than against the
// whole's own spec.

// The quantity type of the `Idx`-th component: what the component-wise constructor accepts in that
// position, in the whole's unit and over the representation's element type.
template<auto R, typename Rep, std::size_t Idx>
using component_quantity = quantity<component_spec<get_quantity_spec(R), Rep, Idx>()[get_unit(R)], value_type_t<Rep>>;

// The element type each component contributes, spelled per index so that the pack can be expanded
// once per component.
template<std::size_t, typename T>
using component_element_type = T;

// Every argument is implicitly convertible to the component quantity of its position. That single
// rule is what makes a swapped pair of axis-typed arguments ill-formed, and it deliberately goes no
// further: a quantity kind converts into every quantity of its hierarchy, so a unit-only argument
// such as `30 * km / h` fits every position and any order of those compiles. Refusing one here would
// make this the only place in the library that second-guesses that conversion, and reaching for a
// kind quantity is the documented way to opt out of kind safety. The argument pack and the index pack
// have equal length (guaranteed by the count check in `ComposableFrom`), so the fold expands them in
// lockstep.
template<auto R, typename Rep, typename... Qs, std::size_t... Idxs>
[[nodiscard]] consteval bool axis_quantities_convertible(std::index_sequence<Idxs...>)
{
  return (... && std::convertible_to<Qs, component_quantity<R, Rep, Idxs>>);
}

// The representation can be built from one element value per component. This asks for *braced*
// initialization, which is what `compose_representation` below performs, rather than
// `std::constructible_from`: that concept spells parenthesized direct-initialization, which does not
// see a vector type whose only element-wise constructor takes an `initializer_list`
// (`blaze::StaticVector` is one). Asking the narrower question would leave such a backend able to
// decompose but not to recompose, and the two directions are meant to close on each other.
template<typename Rep, std::size_t... Idxs>
[[nodiscard]] consteval bool rep_constructible_from_elements(std::index_sequence<Idxs...>)
{
  return requires(component_element_type<Idxs, value_type_t<Rep>>... elems) { Rep{elems...}; };
}

// The two-argument floor keeps this away from the one-argument constructors above, which a
// single-element representation would otherwise collide with. A declared decomposition always has at
// least two axes, so the floor only ever bites an undeclared one.
//
// The order of the conjunction is deliberate. This constructor is a candidate for every construction
// of a quantity, including the `{value, reference}` shape behind `42 * m`, so the two checks that
// depend only on the argument count and the representation come first: they are decided once per
// representation type and argument count, whereas the decomposition checks are decided once per
// quantity type.
template<auto R, typename Rep, typename... Qs>
concept ComposableFrom =
  (sizeof...(Qs) >= 2) && rep_constructible_from_elements<Rep>(std::index_sequence_for<Qs...>{}) &&
  Decomposable<get_quantity_spec(R), Rep> && (sizeof...(Qs) == decomposition_size<get_quantity_spec(R), Rep>()) &&
  (axis_quantities_convertible<R, Rep, Qs...>(std::index_sequence_for<Qs...>{}) ||
   unsatisfied<"Not every argument converts to the component quantity of its position in '{}'">(
     type_name<quantity<R, Rep>>()));

// Materializes the representation from the component quantities. Each argument is converted to its
// position's component quantity type first, which is where the unit conversion and the axis check
// happen, and only then handed to the representation as a plain element value.
template<auto R, typename Rep, std::size_t... Idxs, typename... Qs>
[[nodiscard]] constexpr Rep compose_representation(std::index_sequence<Idxs...>, const Qs&... qs)
{
  return Rep{component_quantity<R, Rep, Idxs>{qs}.numerical_value_in(get_unit(R))...};
}

// Component-wise construction from a single value holding the components, such as a heterogeneous
// vector, so that the tuple protocol closes on itself too: what decomposes into such a value
// composes back from one. The values accepted are those a structured binding decomposes through
// `std::tuple_size`, and each element is read the way such a binding reads it.
// `std::make_from_tuple` cannot serve, because it accepts only the standard library's own
// *tuple-like* types and calls `std::get`.

// The `Idx`-th element converts implicitly to the component quantity of that position, which is the
// check `axis_quantities_convertible` makes for the separate arguments.
template<auto R, typename Rep, typename T, std::size_t Idx>
concept TupleLikeElementConvertible = requires(const T& components) {
  { detail::tuple_like_get<Idx>(components) } -> std::convertible_to<component_quantity<R, Rep, Idx>>;
};

template<auto R, typename Rep, typename T, std::size_t... Idxs>
[[nodiscard]] consteval bool tuple_like_elements_convertible(std::index_sequence<Idxs...>)
{
  return (... && TupleLikeElementConvertible<R, Rep, T, Idxs>);
}

// The rules of `ComposableFrom`, with the count read from `std::tuple_size`. This constructor is a
// candidate for every one-argument construction of a quantity, so the cheap checks come first, and
// the exclusions keep it away from the constructors that already own such an argument:
// - a quantity or a point, because a decomposable quantity is itself tuple-like, and converting it
//   component by component would bypass the quantity-spec conversion rules it is subject to as a whole;
// - a value convertible to the representation, because a vector representation is tuple-like too, and
//   it belongs to the value constructors;
// - a `QuantityLike` type, which converts as a whole through its `quantity_like_traits`. It is asked
//   only once `std::tuple_size` holds, so other arguments never instantiate those traits.
// The deleted catch-all that rejects a truncating number for a quantity of unit `one` would otherwise
// accept a tuple-like value too, so it is constrained on the negation of this concept. It is
// message-free: the only valid one-argument construction it does not exclude up front is a
// representation reached explicitly, and a throwing diagnostic here would turn a tuple-like one into
// a hard error, as it would for that negated use.
template<auto R, typename Rep, typename T>
concept ComposableFromTupleLike =
  (!Quantity<T>) && (!QuantityPoint<T>) && (!std::convertible_to<T, Rep>) && (extent<T>() != dynamic_extent) &&
  (extent<T>() >= 2) && (!QuantityLike<T>) &&
  rep_constructible_from_elements<Rep>(std::make_index_sequence<extent<T>()>{}) &&
  Decomposable<get_quantity_spec(R), Rep> && (extent<T>() == decomposition_size<get_quantity_spec(R), Rep>()) &&
  tuple_like_elements_convertible<R, Rep, T>(std::make_index_sequence<extent<T>()>{});

template<auto R, typename Rep, typename T, std::size_t... Idxs>
[[nodiscard]] constexpr Rep compose_representation_from_tuple_like(std::index_sequence<Idxs...>, const T& components)
{
  return detail::compose_representation<R, Rep>(std::index_sequence<Idxs...>{},
                                                detail::tuple_like_get<Idxs>(components)...);
}

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

/**
 * @brief A quantity
 *
 * Property of a phenomenon, body, or substance, where the property has a magnitude that can be
 * expressed by means of a number and a reference.
 *
 * @tparam R a reference of the quantity providing all information about quantity properties
 * @tparam Rep a type to be used to represent values of a quantity
 */
template<Reference auto R, RepresentationOf<get_quantity_spec(R)> Rep = double>
class quantity : public detail::quantity_iface {
public:
  Rep numerical_value_is_an_implementation_detail_;  ///< needs to be public for a structural type

  // member types and values
  static constexpr Reference auto reference = R;
  static constexpr QuantitySpec auto quantity_spec = get_quantity_spec(reference);
  // The member is read directly rather than through `get_dimension(quantity_spec)`: the same value, without one
  // accessor instantiation per quantity type.
  static constexpr Dimension auto dimension = quantity_spec._dimension_;
#if !MP_UNITS_EAGER_STATIC_MEMBER_INIT_HACK
  // Deprecated in 2.6.0. Ill-formed for a quantity kind, which admits every character. Omitted on MSVC up to
  // 19.51 (see `MP_UNITS_EAGER_STATIC_MEMBER_INIT_HACK`).
  [[deprecated(
    "2.6.0: a quantity's character is no longer a value to inspect; ask "
    "`QuantityOf<Q, quantity_tensor_order::vector>`, `QuantityOf<Q, quantity_field::complex>`, or "
    "another character instead")]] static constexpr quantity_character character =
    detail::definite_character<quantity_spec._character_>();
#endif
  static constexpr Unit auto unit = get_unit(reference);
  using rep = Rep;

  // static member functions
  [[nodiscard]] static constexpr quantity zero() noexcept
    requires requires { representation_values<rep>::zero(); }
  {
    return {representation_values<rep>::zero(), reference};
  }

  [[nodiscard]] static constexpr quantity min() noexcept
    requires requires { representation_values<rep>::min(); }
  {
    return {representation_values<rep>::min(), reference};
  }

  [[nodiscard]] static constexpr quantity max() noexcept
    requires requires { representation_values<rep>::max(); }
  {
    return {representation_values<rep>::max(), reference};
  }

  // construction and assignment
  [[nodiscard]] quantity() = default;

  template<Reference R2>
    requires(equivalent(unit, get_unit(R2{})))
  [[nodiscard]] constexpr quantity(rep val, R2) : numerical_value_is_an_implementation_detail_(std::move(val))
  {
  }

  template<typename Value, Reference R2>
    requires(equivalent(unit, get_unit(R2{}))) && (!detail::RepConstructibleFrom<rep, Value>)
  constexpr quantity(Value val, R2)
#if MP_UNITS_HAS_DELETED_FUNCTION_MESSAGE
    = delete ("Conversion is truncating");
#else
    = delete;
#endif

  template<typename FwdValue, Reference R2>
    requires(!equivalent(unit, get_unit(R2{}))) &&
            detail::QuantityConstructibleFrom<quantity, quantity<R2{}, std::remove_cvref_t<FwdValue>>>
  [[nodiscard]] constexpr quantity(FwdValue&& val, R2) :
      quantity(::mp_units::quantity{std::forward<FwdValue>(val), R2{}})
  {
  }

  [[nodiscard]] constexpr explicit(!mp_units::utility::implicitly_convertible(quantity_spec, dimensionless))
    quantity(rep val)
    requires detail::ExplicitFromNumber<reference>
      : numerical_value_is_an_implementation_detail_(std::move(val))
  {
  }

  template<typename Value>
    requires detail::ExplicitFromNumber<reference> && detail::RepConstructibleFrom<rep, Value> &&
             (!std::convertible_to<Value, rep>)
  [[nodiscard]] constexpr explicit quantity(Value val) : numerical_value_is_an_implementation_detail_(std::move(val))
  {
  }

  // a tuple-like value of the components is not a truncated number: it composes the whole (below)
  template<typename Value>
    requires detail::ExplicitFromNumber<reference> && (!detail::RepConstructibleFrom<rep, Value>) &&
               (!detail::ComposableFromTupleLike<reference, rep, Value>)
  constexpr explicit(!std::convertible_to<Value, rep> ||
                     !mp_units::utility::implicitly_convertible(quantity_spec, dimensionless)) quantity(Value val)
#if MP_UNITS_HAS_DELETED_FUNCTION_MESSAGE
    = delete ("Conversion is truncating");
#else
    = delete;
#endif

  // Component-wise construction from the component quantities of a decomposition (see
  // framework/vector_components.h). This type pins the unit, so every argument is converted into it
  // under the ordinary conversion rules. Each argument must convert to the component quantity of its
  // position: the declared axis when the whole declares its axes, which is what makes a swapped pair
  // of arguments a compile-time error, and the whole's kind otherwise. `ComposableFrom` asks for at
  // least two arguments, so this never competes with the one-argument constructors above.
  template<typename... Qs>
    requires detail::ComposableFrom<reference, rep, Qs...>
  [[nodiscard]] constexpr quantity(const Qs&... qs) :
      numerical_value_is_an_implementation_detail_(
        detail::compose_representation<reference, rep>(std::index_sequence_for<Qs...>{}, qs...))
  {
  }

  // The same construction from a single value of the components that a structured binding decomposes
  // through `std::tuple_size` (a `std::tuple`, a `std::array`, or a third-party type with a member or
  // an argument-dependent `get`), under the same rules. Implicit, as the separate arguments are. A
  // quantity, a point, a `QuantityLike` type, and a value convertible to the representation are
  // excluded, and the deleted truncating constructor above steps aside, so this never competes with
  // the other one-argument constructors.
  template<typename T>
    requires detail::ComposableFromTupleLike<reference, rep, T>
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  [[nodiscard]] constexpr quantity(const T& components) :
      numerical_value_is_an_implementation_detail_(detail::compose_representation_from_tuple_like<reference, rep>(
        std::make_index_sequence<std::tuple_size_v<T>>{}, components))
  {
  }

  template<auto R2, typename Rep2>
    requires detail::QuantityConstructibleFrom<quantity, quantity<R2, Rep2>> && (equivalent(unit, get_unit(R2)))
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  [[nodiscard]] constexpr explicit(!std::convertible_to<Rep2, rep> ||
                                   !mp_units::utility::implicitly_convertible(get_quantity_spec(R2), quantity_spec) ||
                                   !mp_units::implicitly_scalable<get_unit(R2), Rep2, unit, rep>)
    quantity(const quantity<R2, Rep2>& q) :
      // the units are equivalent, so the stored value is already the value in `unit`. `q`'s unit
      // need not be a unit of `q`'s own quantity specification (a product like
      // `moment_of_inertia * angular_velocity²` has a unit that carries more angles than its
      // quantity specification), so `q` is not asked to convert to it
      numerical_value_is_an_implementation_detail_(q.numerical_value_is_an_implementation_detail_)
  {
  }

  template<auto R2, typename Rep2>
    requires detail::QuantityConstructibleFrom<quantity, quantity<R2, Rep2>> && (!equivalent(unit, get_unit(R2)))
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  [[nodiscard]] constexpr explicit(!std::convertible_to<Rep2, rep> ||
                                   !mp_units::utility::implicitly_convertible(get_quantity_spec(R2), quantity_spec) ||
                                   !mp_units::implicitly_scalable<get_unit(R2), Rep2, unit, rep>)
    quantity(const quantity<R2, Rep2>& q) :
      quantity(detail::sudo_cast<quantity>(q))
  {
  }

  template<QuantityLike Q>
    requires detail::QuantityConstructibleFrom<quantity, detail::quantity_like_type<Q>>
  [[nodiscard]] constexpr explicit(quantity_like_traits<Q>::explicit_import ||
                                   // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
                                   !std::convertible_to<detail::quantity_like_type<Q>, quantity>) quantity(const Q& q) :
      quantity(::mp_units::quantity{quantity_like_traits<Q>::to_numerical_value(q), quantity_like_traits<Q>::reference})
  {
  }

  template<typename FwdValue>
    requires detail::ImplicitFromNumber<reference> && detail::RepAssignableFrom<rep, FwdValue>
  constexpr quantity& operator=(FwdValue&& val)
  {
    numerical_value_is_an_implementation_detail_ = std::forward<FwdValue>(val);
    return *this;
  }

  template<UnitOf<quantity_spec> ToU>
    requires detail::ImplicitScaling<unit, ToU{}, rep>
  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto in(ToU) const
  {
    return detail::sudo_cast<quantity<detail::make_reference(quantity_spec, ToU{}), Rep>>(*this);
  }

  template<RepresentationOf<quantity_spec> ToRep>
    requires detail::RepConstructibleFrom<ToRep, rep>
  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto in() const
  {
    return quantity<reference, ToRep>{*this};
  }

  template<RepresentationOf<quantity_spec> ToRep, UnitOf<quantity_spec> ToU>
    requires detail::RepConstructibleFrom<ToRep, rep> && detail::ImplicitConversion<unit, rep, ToU{}, ToRep>
  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto in(ToU) const
  {
    return detail::sudo_cast<quantity<detail::make_reference(quantity_spec, ToU{}), ToRep>>(*this);
  }

  template<UnitOf<quantity_spec> ToU, RoundingPolicy Policy>
    requires detail::ExplicitlyCastable<unit, ToU{}, rep> && detail::ValidRoundingPolicyFor<Policy, rep, rep>
  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto in(ToU, Policy policy) const
  {
    return value_cast<ToU{}>(*this, policy);
  }

  template<RepresentationOf<quantity_spec> ToRep, RoundingPolicy Policy>
    requires std::constructible_from<ToRep, rep> && detail::ValidRoundingPolicyFor<Policy, rep, ToRep>
  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto in(Policy policy) const
  {
    return value_cast<ToRep>(*this, policy);
  }

  template<RepresentationOf<quantity_spec> ToRep, UnitOf<quantity_spec> ToU, RoundingPolicy Policy>
    requires std::constructible_from<ToRep, rep> && detail::ExplicitlyCastable<unit, ToU{}, ToRep> &&
             detail::ValidRoundingPolicyFor<Policy, rep, ToRep>
  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto in(ToU, Policy policy) const
  {
    return value_cast<ToU{}, ToRep>(*this, policy);
  }

  template<UnitOf<quantity_spec> ToU>
    requires detail::ExplicitlyCastable<unit, ToU{}, rep>
  [[deprecated(
    "2.6.0: use `in(unit, policy)` with a rounding policy (e.g. `truncated`) "
    "instead")]] [[nodiscard]] constexpr QuantityOf<quantity_spec> auto force_in(ToU) const
  {
    return in(ToU{}, truncated);
  }

  template<RepresentationOf<quantity_spec> ToRep>
    requires std::constructible_from<ToRep, rep>
  [[deprecated("2.6.0: use `in<Rep>(policy)` with a rounding policy (e.g. `truncated`) instead")]] [[nodiscard]]
  constexpr QuantityOf<quantity_spec> auto force_in() const
  {
    return in<ToRep>(truncated);
  }

  template<RepresentationOf<quantity_spec> ToRep, UnitOf<quantity_spec> ToU>
    requires std::constructible_from<ToRep, rep> && detail::ExplicitlyCastable<unit, ToU{}, rep>
  [[deprecated(
    "2.6.0: use `in<Rep>(unit, policy)` with a rounding policy (e.g. `truncated`) "
    "instead")]] [[nodiscard]] constexpr QuantityOf<quantity_spec> auto force_in(ToU) const
  {
    return in<ToRep>(ToU{}, truncated);
  }

  // Euclidean (or, for a complex field, Hermitian) norm of a vector or tensor quantity, as a scalar
  // quantity in the same unit. The two-part constraint is deliberate: `order >= vector` is the
  // character guard (a scalar such as `isq::speed` has no magnitude, though the `magnitude` CPO would
  // gladly run `std::abs` on its `double`), and `HasMagnitude<rep>` ensures the storage can deliver one.
  //
  // Returning `* unit` drops the precise `quantity_spec` to the unit's kind: a V2 convenience, since V2
  // cannot name the right result type (a scalar-magnitude spec like `vec_mag<isq::force>[N]` is a V3
  // feature). This does not fix the character in general - it collapses to `real_scalar` only for units
  // built from scalar base units (e.g. `km/h`); for one tied to a vector spec (e.g. `N`) the result
  // keeps `vector` character, so one could even take the magnitude of a magnitude. A known V2 limitation.
  [[nodiscard]] constexpr Quantity auto magnitude() const
    // Only a quantity that may be a vector or a tensor gets a magnitude (a kind quantity in a `double`
    // qualifies), and its representation has to be able to produce one.
    requires(QuantityOf<quantity, quantity_tensor_order::vector> ||
             QuantityOf<quantity, quantity_tensor_order::tensor>) &&
            detail::HasMagnitude<rep>
  {
    return ::mp_units::magnitude(numerical_value_is_an_implementation_detail_) * unit;
  }

  // data access
  template<UnitOf<quantity_spec> U>
    requires(equivalent(U{}, unit))
  [[nodiscard]] constexpr rep& numerical_value_ref_in(U) & noexcept
  {
    return numerical_value_is_an_implementation_detail_;
  }

  template<UnitOf<quantity_spec> U>
    requires(equivalent(U{}, unit))
  [[nodiscard]] constexpr const rep& numerical_value_ref_in(U) const& noexcept
  {
    return numerical_value_is_an_implementation_detail_;
  }

  template<UnitOf<quantity_spec> U>
    requires(equivalent(U{}, unit))
  constexpr const rep&& numerical_value_ref_in(U) const&& noexcept
#if MP_UNITS_HAS_DELETED_FUNCTION_MESSAGE
    = delete ("Can't form a reference to a temporary");
#else
    = delete;
#endif

  template<UnitOf<quantity_spec> U>
    requires detail::ImplicitScaling<unit, U{}, rep>
  [[nodiscard]] constexpr RepresentationOf<quantity_spec> auto numerical_value_in(U) const noexcept
  {
    return in(U{}).numerical_value_is_an_implementation_detail_;
  }

  template<UnitOf<quantity_spec> U, RoundingPolicy Policy>
    requires detail::ExplicitlyCastable<unit, U{}, rep> && detail::ValidRoundingPolicyFor<Policy, rep, rep>
  [[nodiscard]] constexpr RepresentationOf<quantity_spec> auto numerical_value_in(U, Policy policy) const noexcept
  {
    return in(U{}, policy).numerical_value_is_an_implementation_detail_;
  }

  template<UnitOf<quantity_spec> U>
    requires detail::ExplicitlyCastable<unit, U{}, rep>
  [[deprecated(
    "2.6.0: use `numerical_value_in(unit, policy)` with a rounding policy (e.g. `truncated`) "
    "instead")]] [[nodiscard]] constexpr RepresentationOf<quantity_spec> auto force_numerical_value_in(U) const noexcept
  {
    return numerical_value_in(U{}, truncated);
  }

  // conversion operators
  template<typename V_, std::constructible_from<rep> Value = std::remove_cvref_t<V_>>
    requires detail::ExplicitFromNumber<reference>
  [[nodiscard]] explicit constexpr operator V_() const& noexcept
  {
    return numerical_value_is_an_implementation_detail_;
  }

  template<typename Q_, QuantityLike Q = std::remove_cvref_t<Q_>>
    requires detail::QuantityConstructibleFrom<detail::quantity_like_type<Q>, quantity>
  [[nodiscard]] explicit(quantity_like_traits<Q>::explicit_export ||
                         !std::convertible_to<quantity, detail::quantity_like_type<Q>>) constexpr
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  operator Q_() const
    noexcept(noexcept(quantity_like_traits<Q>::from_numerical_value(numerical_value_is_an_implementation_detail_)) &&
             std::is_nothrow_copy_constructible_v<rep>)
  {
    return quantity_like_traits<Q>::from_numerical_value(
      numerical_value_in(get_unit(quantity_like_traits<Q>::reference)));
  }

  // member unary operators
  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto operator+() const
    requires requires(const rep v) {
      { +v } -> std::common_with<rep>;
    }
  {
    return ::mp_units::quantity{+numerical_value_is_an_implementation_detail_, reference};
  }

  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto operator-() const
    requires requires(const rep v) {
      { -v } -> std::common_with<rep>;
    }
  {
    return ::mp_units::quantity{-numerical_value_is_an_implementation_detail_, reference};
  }

  constexpr quantity& operator++() &
    requires requires(rep& v) {
      { ++v } -> std::same_as<rep&>;
    }
  {
    ++numerical_value_is_an_implementation_detail_;
    return *this;
  }

  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto operator++(int)
    requires requires(rep& v) {
      { v++ } -> std::common_with<rep>;
    }
  {
    return ::mp_units::quantity{numerical_value_is_an_implementation_detail_++, reference};
  }

  constexpr quantity& operator--() &
    requires requires(rep& v) {
      { --v } -> std::same_as<rep&>;
    }
  {
    --numerical_value_is_an_implementation_detail_;
    return *this;
  }

  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto operator--(int)
    requires requires(rep& v) {
      { v-- } -> std::common_with<rep>;
    }
  {
    return ::mp_units::quantity{numerical_value_is_an_implementation_detail_--, reference};
  }

  // compound assignment operators
  template<auto R2, typename Rep2>
    requires(mp_units::utility::implicitly_convertible(get_quantity_spec(R2), quantity_spec)) &&
            detail::ImplicitConversion<get_unit(R2), Rep2, unit, rep> && requires(rep& a, const Rep2 b) {
              { a += b } -> std::same_as<rep&>;
            }
  constexpr quantity& operator+=(const quantity<R2, Rep2>& other) &
  {
    if constexpr (equivalent(unit, get_unit(R2)))
      numerical_value_is_an_implementation_detail_ += other.numerical_value_is_an_implementation_detail_;
    else
      numerical_value_is_an_implementation_detail_ += other.in(unit).numerical_value_is_an_implementation_detail_;
    return *this;
  }

  template<auto R2, typename Rep2>
    requires(mp_units::utility::implicitly_convertible(get_quantity_spec(R2), quantity_spec)) &&
            detail::ImplicitConversion<get_unit(R2), Rep2, unit, rep> && requires(rep& a, const Rep2 b) {
              { a -= b } -> std::same_as<rep&>;
            }
  constexpr quantity& operator-=(const quantity<R2, Rep2>& other) &
  {
    if constexpr (equivalent(unit, get_unit(R2)))
      numerical_value_is_an_implementation_detail_ -= other.numerical_value_is_an_implementation_detail_;
    else
      numerical_value_is_an_implementation_detail_ -= other.in(unit).numerical_value_is_an_implementation_detail_;
    return *this;
  }

  template<auto R2, typename Rep2>
    requires(!treat_as_floating_point<rep>) &&
            (mp_units::utility::implicitly_convertible(get_quantity_spec(R2), quantity_spec)) &&
            detail::ImplicitConversion<get_unit(R2), Rep2, unit, rep> && requires(rep& a, const Rep2 b) {
              { a %= b } -> std::same_as<rep&>;
            }
  constexpr quantity& operator%=(const quantity<R2, Rep2>& other) &
  {
    MP_UNITS_PRECONDITION_DEBUG(other != 0);
    if constexpr (equivalent(unit, get_unit(R2)))
      numerical_value_is_an_implementation_detail_ %= other.numerical_value_is_an_implementation_detail_;
    else
      numerical_value_is_an_implementation_detail_ %= other.in(unit).numerical_value_is_an_implementation_detail_;
    return *this;
  }

  template<detail::ScalarRepConvertible<rep> Value>
    requires requires(rep& a, const Value b) {
      { a *= b } -> std::same_as<rep&>;
    }
  constexpr quantity& operator*=(const Value& val) &
  {
    numerical_value_is_an_implementation_detail_ *= val;
    return *this;
  }

  template<detail::ImplicitFromNumberQuantity Q2>
    requires detail::ScalarRepConvertible<typename Q2::rep, rep> && requires(rep& a, const Q2::rep b) {
      { a *= b } -> std::same_as<rep&>;
    }
  constexpr quantity& operator*=(const Q2& other) &
  {
    return *this *= other.numerical_value_is_an_implementation_detail_;
  }

  template<detail::ScalarRepConvertible<rep> Value>
    requires requires(rep& a, const Value b) {
      { a /= b } -> std::same_as<rep&>;
    }
  constexpr quantity& operator/=(const Value& val) &
  {
    MP_UNITS_PRECONDITION_DEBUG(val != representation_values<Value>::zero());
    numerical_value_is_an_implementation_detail_ /= val;
    return *this;
  }

  template<detail::ImplicitFromNumberQuantity Q2>
    requires detail::ScalarRepConvertible<typename Q2::rep, rep> && requires(rep& a, const Q2::rep b) {
      { a /= b } -> std::same_as<rep&>;
    }
  constexpr quantity& operator/=(const Q2& rhs) &
  {
    return *this /= rhs.numerical_value_is_an_implementation_detail_;
  }
};

// CTAD
// The stored representation is canonicalized (see `representation_canonical_type`) so that an
// expression-template value (e.g. the result of `Eigen::Vector3d * double`) is materialized to
// its evaluated concrete type instead of being stored as a proxy holding dangling references.
// The deduced representation type is a defaulted template parameter instead of being spelled in
// the guide's result. Clang prints a result type as it is spelled, so users would otherwise see
// `quantity<R, representation_canonical_type_t<Value>>` in diagnostics. The defaulted parameter
// reaches them as the plain type, e.g. `quantity<R, double>`. A deduction guide can't be given
// explicit template arguments, so the extra parameter isn't observable.
template<Reference R, typename Value, typename Rep = representation_canonical_type_t<Value>>
  requires RepresentationOf<Rep, get_quantity_spec(R{})>
quantity(Value v, R) -> quantity<R{}, Rep>;

template<Reference auto R = one, typename Value, typename Rep = representation_canonical_type_t<Value>>
  requires RepresentationOf<Rep, get_quantity_spec(R)>
quantity(Value) -> quantity<R, Rep>;

template<QuantityLike Q, typename Rep = quantity_like_traits<Q>::rep>
quantity(Q) -> quantity<quantity_like_traits<Q>::reference, Rep>;

MP_UNITS_EXPORT_END

namespace detail {

template<QuantitySpec auto QS, typename FwdValue>
[[nodiscard]] constexpr Quantity auto make_quantity_of_one(FwdValue&& val)
{
  return quantity{std::forward<FwdValue>(val), QS[one]};
}

// Defines the result of the `std::common_type` specializations below. The common representation is a
// defaulted template parameter so that Clang diagnostics show the plain type (e.g. `int`) rather
// than `std::common_type_t<short, unsigned char>`.
template<auto R, typename Rep1, typename Rep2, typename Rep = std::common_type_t<Rep1, Rep2>>
struct common_quantity_type {
  using type = quantity<R, Rep>;
};

}  // namespace detail

}  // namespace mp_units

// Tuple-like protocol for a decomposable vector quantity, enabling structured bindings.
template<auto R, typename Rep>
  requires mp_units::detail::Decomposable<get_quantity_spec(R), Rep>
struct std::tuple_size<mp_units::quantity<R, Rep>> :
    std::integral_constant<std::size_t, mp_units::detail::decomposition_size<get_quantity_spec(R), Rep>()> {};

template<std::size_t Idx, auto R, typename Rep>
  requires mp_units::detail::DecomposableIndex<get_quantity_spec(R), Rep, Idx>
struct std::tuple_element<Idx, mp_units::quantity<R, Rep>> {
  using type = std::remove_cvref_t<decltype(get<Idx>(std::declval<mp_units::quantity<R, Rep>>()))>;
};

template<auto R1, typename Rep1, auto R2, typename Rep2>
  requires requires {
    { mp_units::get_common_reference(R1, R2) } -> mp_units::Reference;
    typename std::common_type_t<Rep1, Rep2>;
    requires(
      !mp_units::detail::overflows_non_zero_common_values<std::common_type_t<Rep1, Rep2>>(get_unit(R1), get_unit(R2)));
    requires mp_units::RepresentationOf<
      std::common_type_t<Rep1, Rep2>, mp_units::get_common_quantity_spec(get_quantity_spec(R1), get_quantity_spec(R2))>;
  }
struct std::common_type<mp_units::quantity<R1, Rep1>, mp_units::quantity<R2, Rep2>> :
    mp_units::detail::common_quantity_type<mp_units::get_common_reference(R1, R2), Rep1, Rep2> {};

template<auto R, typename Rep, mp_units::RepresentationOf<get_quantity_spec(R)> Value>
  requires(get_unit(R) == mp_units::one) && requires { typename mp_units::quantity<R, std::common_type_t<Rep, Value>>; }
struct std::common_type<mp_units::quantity<R, Rep>, Value> : mp_units::detail::common_quantity_type<R, Rep, Value> {};

template<auto R, typename Rep, mp_units::RepresentationOf<get_quantity_spec(R)> Value>
  requires requires { typename std::common_type<mp_units::quantity<R, Rep>, Value>; }
struct std::common_type<Value, mp_units::quantity<R, Rep>> : std::common_type<mp_units::quantity<R, Rep>, Value> {};

template<auto R, typename Rep>
  requires std::numeric_limits<Rep>::is_specialized
class std::numeric_limits<mp_units::quantity<R, Rep>> : public std::numeric_limits<Rep> {
public:
  static constexpr mp_units::quantity<R, Rep> min() noexcept
    requires requires { mp_units::quantity<R, Rep>::min(); }
  {
    return mp_units::quantity<R, Rep>::min();
  }

  static constexpr mp_units::quantity<R, Rep> max() noexcept
    requires requires { mp_units::quantity<R, Rep>::max(); }
  {
    return mp_units::quantity<R, Rep>::max();
  }

  static constexpr mp_units::quantity<R, Rep> lowest() noexcept
    requires requires { std::numeric_limits<Rep>::lowest(); }
  {
    return {std::numeric_limits<Rep>::lowest(), R};
  }

  static constexpr mp_units::quantity<R, Rep> epsilon() noexcept
    requires requires { std::numeric_limits<Rep>::epsilon(); }
  {
    return {std::numeric_limits<Rep>::epsilon(), R};
  }

  static constexpr mp_units::quantity<R, Rep> round_error() noexcept
    requires requires { std::numeric_limits<Rep>::round_error(); }
  {
    return {std::numeric_limits<Rep>::round_error(), R};
  }

  static constexpr mp_units::quantity<R, Rep> infinity() noexcept
    requires requires { std::numeric_limits<Rep>::infinity(); }
  {
    return {std::numeric_limits<Rep>::infinity(), R};
  }

  static constexpr mp_units::quantity<R, Rep> quiet_NaN() noexcept
    requires requires { std::numeric_limits<Rep>::quiet_NaN(); }
  {
    return {std::numeric_limits<Rep>::quiet_NaN(), R};
  }

  static constexpr mp_units::quantity<R, Rep> signaling_NaN() noexcept
    requires requires { std::numeric_limits<Rep>::signaling_NaN(); }
  {
    return {std::numeric_limits<Rep>::signaling_NaN(), R};
  }

  static constexpr mp_units::quantity<R, Rep> denorm_min() noexcept
    requires requires { std::numeric_limits<Rep>::denorm_min(); }
  {
    return {std::numeric_limits<Rep>::denorm_min(), R};
  }
};

#if MP_UNITS_HOSTED
#if MP_UNITS_USE_FMTLIB
namespace mp_units::detail {

// {fmt} 12.2 and older spell `generic_context` with two template parameters, newer ones add a third
// for the context the arguments are typed on. Taking the template as a parameter makes the
// three-argument form a constraint that is unsatisfied with the older spelling, instead of an error.
template<template<typename...> class GenericContext, typename FormatContext, typename Char>
  requires requires { typename GenericContext<fmt::basic_appender<Char>, Char, FormatContext>; }
[[nodiscard]] auto make_generic_context(FormatContext& ctx, fmt::basic_memory_buffer<Char>& buffer)
{
  return GenericContext<fmt::basic_appender<Char>, Char, FormatContext>(fmt::basic_appender<Char>(buffer), ctx.args(),
                                                                        ctx.locale());
}

// The {fmt} counterpart of the proposed `std::rebind_format_context`: a context that writes to
// `buffer` and takes its arguments and locale from `ctx`.
template<typename FormatContext, typename Char>
[[nodiscard]] auto rebind_format_context(FormatContext& ctx, fmt::basic_memory_buffer<Char>& buffer)
{
  if constexpr (std::is_same_v<decltype(ctx.out()), fmt::basic_appender<Char>>)
    return FormatContext(fmt::basic_appender<Char>(buffer), ctx.args(), ctx.locale());
  else if constexpr (requires { make_generic_context<fmt::generic_context>(ctx, buffer); })
    return make_generic_context<fmt::generic_context>(ctx, buffer);
  else
    // {fmt} 12.2 and older (`FMT_COMPILE` to a non-appender output) cannot type the arguments of
    // `ctx` for another output, so the buffer gets none.
    return fmt::buffered_context<Char>(fmt::basic_appender<Char>(buffer), {}, ctx.locale());
}

}  // namespace mp_units::detail

#endif


//
// Grammar
//
// quantity-format-spec        = [fill-and-align], [width], [quantity-specs], [defaults-specs];
// quantity-specs              = conversion-spec;
//                             | quantity-specs, conversion-spec;
//                             | quantity-specs, literal-char;
// literal-char                = ? any character other than '{', '}', or '%' ?;
// conversion-spec             = '%', placement-type;
// placement-type              = subentity-id | '?' | '%';
// defaults-specs              = ':', default-spec-list;
// default-spec-list           = default-spec;
//                             | default-spec-list, default-spec;
// default-spec                = subentity-id, '[' format-spec ']';
// subentity-id                = 'N' | 'U' | 'D';
// format-spec                 = ? as specified by the formatter for the argument type ?;
//
#if __cpp_lib_format_ranges && !MP_UNITS_USE_FMTLIB
template<auto Reference, typename Char, std::formattable<Char> Rep>
#else
template<auto Reference, typename Rep, typename Char>
#endif
class MP_UNITS_STD_FMT::formatter<mp_units::quantity<Reference, Rep>, Char> {
  static constexpr auto unit = get_unit(Reference);
  static constexpr auto dimension = get_dimension(get_quantity_spec(Reference));

  using quantity_t = mp_units::quantity<Reference, Rep>;
  using unit_t = MP_UNITS_NONCONST_TYPE(unit);
  using dimension_t = MP_UNITS_NONCONST_TYPE(dimension);
  using format_specs = mp_units::utility::fill_align_width_format_specs<Char>;

  format_specs specs_{};

  std::basic_string_view<Char> modifiers_format_str_;
  // The `default-spec` of each subentity, as views into the format string like the one above:
  // `parse()` runs on every formatting call, so it must not allocate.
  std::basic_string_view<Char> rep_spec_;
  std::basic_string_view<Char> unit_spec_;
  std::basic_string_view<Char> dimension_spec_;

  MP_UNITS_STD_FMT::formatter<Rep> rep_formatter_;
  MP_UNITS_STD_FMT::formatter<unit_t> unit_formatter_;
  MP_UNITS_STD_FMT::formatter<dimension_t> dimension_formatter_;

  struct format_checker {
    constexpr void on_number() const {}
    constexpr void on_maybe_space() const {}
    constexpr void on_unit() const {}
    constexpr void on_dimension() const {}
    template<std::forward_iterator It>
    constexpr void on_text(It, It) const
    {
    }
  };

  // Type-erased handler passed to parse_quantity_specs at runtime.
  // Being a concrete (non-template) type it ensures parse_quantity_specs is instantiated
  // only once for the runtime path, regardless of how many context types are used.
  struct erased_handler {
    void* self;
    void (*on_number_fn)(void*);
    void (*on_maybe_space_fn)(void*);
    void (*on_unit_fn)(void*);
    void (*on_dimension_fn)(void*);
    void (*on_text_fn)(void*, const Char*, const Char*);

    void on_number() { on_number_fn(self); }
    void on_maybe_space() { on_maybe_space_fn(self); }
    void on_unit() { on_unit_fn(self); }
    void on_dimension() { on_dimension_fn(self); }
    void on_text(const Char* begin, const Char* end) { on_text_fn(self, begin, end); }
  };

  // The trampolines are tiny and do not drag in parse_quantity_specs.
  // (Param named `self` so it doesn't shadow `si::unit_symbols::p` (pico) on MSVC C4459.)
  template<typename Handler>
  static void on_number_erased(void* self)
  {
    static_cast<Handler*>(self)->on_number();
  }
  template<typename Handler>
  static void on_maybe_space_erased(void* self)
  {
    static_cast<Handler*>(self)->on_maybe_space();
  }
  template<typename Handler>
  static void on_unit_erased(void* self)
  {
    static_cast<Handler*>(self)->on_unit();
  }
  template<typename Handler>
  static void on_dimension_erased(void* self)
  {
    static_cast<Handler*>(self)->on_dimension();
  }
  template<typename Handler>
  static void on_text_erased(void* self, const Char* b, const Char* e)
  {
    static_cast<Handler*>(self)->on_text(b, e);
  }

  template<typename Handler>
  [[nodiscard]] static erased_handler erase(Handler& h)
  {
    return {&h,
            on_number_erased<Handler>,
            on_maybe_space_erased<Handler>,
            on_unit_erased<Handler>,
            on_dimension_erased<Handler>,
            on_text_erased<Handler>};
  }

  // Writes the components through the sub-formatters pre-parsed in `parse()`, to whatever
  // output `ctx` is bound to.
  template<typename FormatContext>
  struct context_writer {
    const formatter& f;
    FormatContext& ctx;
    const quantity_t& q;

    void on_number() { ctx.advance_to(f.rep_formatter_.format(q.numerical_value_ref_in(q.unit), ctx)); }
    void on_maybe_space()
    {
      if constexpr (mp_units::space_before_unit_symbol<unit>) {
        auto it = ctx.out();
        *it++ = ' ';
        ctx.advance_to(it);
      }
    }
    void on_unit() { ctx.advance_to(f.unit_formatter_.format(q.unit, ctx)); }
    void on_dimension() { ctx.advance_to(f.dimension_formatter_.format(q.dimension, ctx)); }
    void on_text(const Char* begin, const Char* end) { ctx.advance_to(mp_units::detail::copy(begin, end, ctx.out())); }
  };

#if !MP_UNITS_USE_FMTLIB
  // Writes the components to a plain output iterator by re-entering `vformat_to` with format
  // strings reconstructed from the `default-spec`s. Used only for the width path, because
  // `std::basic_format_context` cannot be rebound to a buffer.
  template<typename OutputIt>
  struct vformat_writer {
    const formatter& f;
    OutputIt out;
    const quantity_t& q;
    std::locale locale;

    // `{}` for an empty spec, so that nothing is built in the common case, otherwise `{:<spec>}`
    // built in `storage`.
    static std::basic_string_view<Char> format_str(std::basic_string_view<Char> spec, std::basic_string<Char>& storage)
    {
      static constexpr Char default_format_str[] = {'{', '}'};
      if (spec.empty()) return {default_format_str, 2};
      storage.reserve(spec.size() + 3);
      storage += '{';
      storage += ':';
      storage += spec;
      storage += '}';
      return storage;
    }

    void on_number()
    {
      std::basic_string<Char> storage;
      out = MP_UNITS_STD_FMT::vformat_to(out, locale, format_str(f.rep_spec_, storage),
                                         MP_UNITS_STD_FMT::make_format_args(q.numerical_value_ref_in(q.unit)));
    }
    void on_maybe_space()
    {
      if constexpr (mp_units::space_before_unit_symbol<unit>) *out++ = ' ';
    }
    void on_unit()
    {
      std::basic_string<Char> storage;
      out = MP_UNITS_STD_FMT::vformat_to(out, locale, format_str(f.unit_spec_, storage),
                                         MP_UNITS_STD_FMT::make_format_args(q.unit));
    }
    void on_dimension()
    {
      std::basic_string<Char> storage;
      out = MP_UNITS_STD_FMT::vformat_to(out, locale, format_str(f.dimension_spec_, storage),
                                         MP_UNITS_STD_FMT::make_format_args(q.dimension));
    }
    void on_text(const Char* begin, const Char* end) { out = mp_units::detail::copy(begin, end, out); }
  };
#endif

  template<typename Handler>
  void write_components(Handler& h) const
  {
    if (modifiers_format_str_.empty()) {
      // default layout: rep [space] unit
      h.on_number();
      h.on_maybe_space();
      h.on_unit();
      return;
    }
    // Use erased_handler so parse_quantity_specs is instantiated only once (with erased_handler),
    // not once per context type.
    auto eh = erase(h);
    parse_quantity_specs(modifiers_format_str_.data(), modifiers_format_str_.data() + modifiers_format_str_.size(), eh);
  }

  template<std::forward_iterator It, typename Handler>
  constexpr const It parse_quantity_specs(It begin, It end, Handler& handler) const
  {
    if (begin == end || *begin == ':' || *begin == '}') return begin;
    if (*begin != '%')
      MP_UNITS_THROW(MP_UNITS_STD_FMT::format_error(
        "`quantity-specs` should start with a `conversion-spec` ('%' characters expected)"));
    auto ptr = begin;
    while (ptr != end) {
      auto ch = *ptr;
      if (ch == '}') break;
      if (ch == ':') {
        if (ptr + 1 != end && *(ptr + 1) == ':') {
          handler.on_text(begin, ++ptr);  // account for ':'
          ++ptr;                          // consume the second ':'
          continue;
        }
        // default specs started
        break;
      }
      if (ch != '%') {
        ++ptr;
        continue;
      }
      if (begin != ptr) handler.on_text(begin, ptr);
      ++ptr;  // consume '%'
      if (ptr == end) MP_UNITS_THROW(MP_UNITS_STD_FMT::format_error("invalid `conversion-spec` format"));

      ch = *ptr++;
      switch (ch) {
        case 'N':
          handler.on_number();
          break;
        case 'U':
          handler.on_unit();
          break;
        case 'D':
          handler.on_dimension();
          break;
        case '?':
          handler.on_maybe_space();
          break;
        case '%':
          handler.on_text(ptr - 1, ptr);
          break;
        default:
          MP_UNITS_THROW(MP_UNITS_STD_FMT::format_error(std::string("unknown `placement-type` token '") + ch + "'"));
      }
      begin = ptr;
    }
    if (begin != ptr) handler.on_text(begin, ptr);
    return ptr;
  }

  // The iterators are not `const Char*`: MSVC's `basic_format_parse_context::iterator` is a class.
  template<std::forward_iterator It, typename Formatter>
  constexpr It parse_default_spec(MP_UNITS_STD_FMT::basic_format_parse_context<Char>& ctx, It begin, It end,
                                  Formatter& f, std::basic_string_view<Char>& spec)
  {
    if (begin == end || *begin != '[')
      MP_UNITS_THROW(MP_UNITS_STD_FMT::format_error("`default-spec` should contain a `[` character"));
    auto it = ++begin;
    for (int nested_brackets = 0; it != end && !(*it == ']' && nested_brackets == 0); it++) {
      if (*it == '[') ++nested_brackets;
      if (*it == ']') {
        if (nested_brackets == 0) MP_UNITS_THROW(MP_UNITS_STD_FMT::format_error("unmatched ']' in format string"));
        --nested_brackets;
      }
    }
    if (it == end) MP_UNITS_THROW(MP_UNITS_STD_FMT::format_error("unmatched '[' in format string"));
    spec = {begin, it};
    MP_UNITS_STD_FMT::basic_format_parse_context<Char> sub_ctx(std::basic_string_view<Char>(begin, it));
    auto ptr = f.parse(sub_ctx);
    if (ptr != it)
      MP_UNITS_THROW(MP_UNITS_STD_FMT::format_error("invalid subentity format '" + std::string(begin, it) + "'"));
    return ++it;  // skip `]`
  }

  template<std::forward_iterator It>
  [[nodiscard]] constexpr It parse_defaults_specs(MP_UNITS_STD_FMT::basic_format_parse_context<Char>& ctx, It begin,
                                                  It end)
  {
    if (begin == end || *begin == '}') return begin;
    if (*begin++ != ':') MP_UNITS_THROW(MP_UNITS_STD_FMT::format_error("`defaults-specs` should start with a `:`"));
    do {
      auto ch = *begin++;
      // TODO check if not repeated
      switch (ch) {
        case 'N':
          begin = parse_default_spec(ctx, begin, end, rep_formatter_, rep_spec_);
          break;
        case 'U':
          begin = parse_default_spec(ctx, begin, end, unit_formatter_, unit_spec_);
          break;
        case 'D':
          begin = parse_default_spec(ctx, begin, end, dimension_formatter_, dimension_spec_);
          break;
        default:
          MP_UNITS_THROW(MP_UNITS_STD_FMT::format_error(std::string("unknown `subentity-id` token '") + ch + "'"));
      }
    } while (begin != end && *begin != '}');
    return begin;
  }

public:
  constexpr auto parse(MP_UNITS_STD_FMT::basic_format_parse_context<Char>& ctx) -> decltype(ctx.begin())
  {
    auto begin = ctx.begin(), end = ctx.end();

    begin = parse_fill_align_width(ctx, begin, end, specs_, mp_units::utility::fmt_align::right);
    if (begin == end) return begin;

    const format_checker checker{};
    auto it = parse_quantity_specs(begin, end, checker);
    modifiers_format_str_ = {begin, it};

    return parse_defaults_specs(ctx, it, end);
  }

  template<typename FormatContext>
  auto format(const quantity_t& q, FormatContext& ctx) const
  {
    auto specs = specs_;
    mp_units::utility::handle_dynamic_spec<mp_units::utility::width_checker>(specs.width, specs.width_ref, ctx);

    if (specs.width == 0) {
      // No width: the pre-parsed sub-formatters write straight to `ctx`, whatever the layout.
      context_writer<FormatContext> w{*this, ctx, q};
      write_components(w);
      return ctx.out();
    }

#if MP_UNITS_USE_FMTLIB
    // Width: the same sub-formatters write to a buffer through a context rebound to it, which
    // keeps the arguments and locale of `ctx`.
    MP_UNITS_STD_FMT::basic_memory_buffer<Char> buffer;
    auto buffer_ctx = mp_units::detail::rebind_format_context(ctx, buffer);
    context_writer<decltype(buffer_ctx)> w{*this, buffer_ctx, q};
    write_components(w);
    const std::basic_string_view<Char> content{buffer.data(), buffer.size()};
#else
    std::basic_string<Char> buffer;
    vformat_writer<std::back_insert_iterator<std::basic_string<Char>>> w{*this, std::back_inserter(buffer), q,
                                                                         MP_UNITS_FMT_LOCALE(ctx.locale())};
    write_components(w);
    const std::basic_string_view<Char> content{buffer};
#endif
    return mp_units::utility::write_padded<Char>(ctx.out(), content, specs.width, specs.align, specs.fill);
  }
};


#endif  // MP_UNITS_HOSTED
