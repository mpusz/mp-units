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
#include <mp-units/bits/hacks.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/bits/sudo_cast.h>
#include <mp-units/bits/unsatisfied.h>
#include <mp-units/compat_macros.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/dimension_concepts.h>
#include <mp-units/framework/quantity_concepts.h>
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/reference.h>
#include <mp-units/framework/reference_concepts.h>
#include <mp-units/framework/representation_concepts.h>
#include <mp-units/framework/unit_concepts.h>
#include <mp-units/framework/value_cast.h>
#include <mp-units/framework/vector_components.h>
#if MP_UNITS_HOSTED
#include <mp-units/bits/format.h>
#include <mp-units/bits/ostream.h>
#endif

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/ext/contracts.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <compare>  // IWYU pragma: export
#include <concepts>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>
#if MP_UNITS_HOSTED
#include <locale>
#include <stdexcept>
#endif
#endif
#endif

namespace mp_units {

namespace detail {

struct zero {
  template<typename T>
    requires std::is_arithmetic_v<T> && (!is_same_v<T, bool>)
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
  Quantity<QTo> && Quantity<QFrom> && mp_units::explicitly_convertible(QFrom::quantity_spec, QTo::quantity_spec) &&
  RepConstructibleFrom<typename QTo::rep, typename QFrom::rep> &&
  ImplicitConversion<QFrom::unit, typename QFrom::rep, QTo::unit, typename QTo::rep>;

template<typename T, typename Rep>
concept ScalarRepConvertible =
  !Quantity<T> && Scalar<T> &&
  (treat_as_floating_point<Rep> || !treat_as_floating_point<T> ||
   unsatisfied<"Scaling a non-floating-point '{}' quantity by a floating-point '{}' scalar is truncating">(
     type_name<Rep>(), type_name<T>()));

template<auto R>
concept UnitOne = Reference<MP_UNITS_REMOVE_CONST(decltype(R))> &&
                  (equivalent(get_unit(R), one) && detail::get_associated_quantity(get_unit(R)) == dimensionless);

template<auto R>
concept ExplicitFromNumber = UnitOne<R> && (mp_units::explicitly_convertible(get_quantity_spec(R), dimensionless));

template<auto R>
concept ImplicitFromNumber =
  ExplicitFromNumber<R> && (mp_units::implicitly_convertible(get_quantity_spec(R), dimensionless));

template<typename Q>
concept ImplicitFromNumberQuantity = Quantity<Q> && ImplicitFromNumber<Q::reference>;

// The invocation result is canonicalized (see `representation_canonical_type`) so that
// expression-template results (e.g. Eigen, Blaze) are checked and stored as their evaluated
// concrete type rather than the lazy proxy type.
template<auto QS, typename Func, typename T, typename U>
concept InvokeResultOf = QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(QS))> && std::regular_invocable<Func, T, U> &&
                         RepresentationOf<representation_canonical_type_t<std::invoke_result_t<Func, T, U>>, QS>;

template<typename Func, typename Q1, typename Q2,
         auto QS = std::invoke_result_t<Func, MP_UNITS_NONCONST_TYPE(Q1::quantity_spec),
                                        MP_UNITS_NONCONST_TYPE(Q2::quantity_spec)>{}>
concept InvocableQuantities = QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(QS))> && Quantity<Q1> && Quantity<Q2> &&
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
  QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(QS))> && (get_character(QS).order == quantity_tensor_order::scalar);

template<auto QS, typename Func, typename T, typename U>
concept ScalableResultOf =
  QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(QS))> && std::regular_invocable<Func, T, U> &&
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

template<typename Func, typename Q1, typename Q2>
concept CommonlyInvocableQuantities =
  Quantity<Q1> && Quantity<Q2> && HaveCommonReference<Q1::reference, Q2::reference> &&
  std::convertible_to<Q1, common_quantity_for<Func, Q1, Q2>> &&
  std::convertible_to<Q2, common_quantity_for<Func, Q1, Q2>> &&
  InvocableQuantities<Func, Q1, Q2, mp_units::get_common_quantity_spec(Q1::quantity_spec, Q2::quantity_spec)> &&
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
  Quantity<Q1> && Quantity<Q2> && HaveCommonReference<Q1::reference, Q2::reference> &&
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
  if constexpr (is_same_v<Cmp, std::compare_three_way>)
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
    requires ImplicitFromNumber<R1> && InvokeResultOf<get_quantity_spec(R1), std::plus<>, Rep1, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator+(const quantity<R1, Rep1>& lhs, const Value& rhs)
  {
    return lhs + quantity{rhs};
  }

  template<auto R1, typename Rep1, RepresentationOf<get_quantity_spec(R1)> Value>
    requires ImplicitFromNumber<R1> && InvokeResultOf<get_quantity_spec(R1), std::plus<>, Rep1, const Value&>
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
    requires ImplicitFromNumber<R1> && InvokeResultOf<get_quantity_spec(R1), std::minus<>, Rep1, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator-(const quantity<R1, Rep1>& lhs, const Value& rhs)
  {
    return lhs - quantity{rhs};
  }

  template<auto R1, typename Rep1, RepresentationOf<get_quantity_spec(R1)> Value>
    requires ImplicitFromNumber<R1> && InvokeResultOf<get_quantity_spec(R1), std::minus<>, Rep1, const Value&>
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
    MP_UNITS_EXPECTS_DEBUG(rhs != 0);
    using ret = common_quantity_for<std::modulus<>, quantity<R1, Rep1>, quantity<R2, Rep2>>;
    const ret ret_lhs(lhs);
    const ret ret_rhs(rhs);
    return quantity{ret_lhs.numerical_value_ref_in(ret::unit) % ret_rhs.numerical_value_ref_in(ret::unit),
                    ret::reference};
  }

  template<auto R1, typename Rep1, RepresentationOf<get_quantity_spec(R1)> Value>
    requires ImplicitFromNumber<R1> && InvokeResultOf<get_quantity_spec(R1), std::modulus<>, Rep1, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator%(const quantity<R1, Rep1>& lhs, const Value& rhs)
  {
    return lhs % quantity{rhs};
  }

  template<auto R1, typename Rep1, RepresentationOf<get_quantity_spec(R1)> Value>
    requires ImplicitFromNumber<R1> && InvokeResultOf<get_quantity_spec(R1), std::modulus<>, Rep1, const Value&>
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
    MP_UNITS_EXPECTS_DEBUG(rhs != 0);
    return quantity{lhs.numerical_value_ref_in(get_unit(R1)) / rhs.numerical_value_ref_in(get_unit(R2)), R1 / R2};
  }

  template<auto R1, typename Rep1, typename Value>
    requires(!Quantity<Value>) && (!Reference<Value>) &&
            InvokeResultOf<get_quantity_spec(R1), std::divides<>, Rep1, const Value&>
  [[nodiscard]] friend constexpr QuantityOf<get_quantity_spec(R1)> auto operator/(const quantity<R1, Rep1>& q,
                                                                                  const Value& val)
  {
    MP_UNITS_EXPECTS_DEBUG(val != representation_values<Value>::zero());
    return quantity{q.numerical_value_ref_in(get_unit(R1)) / val, R1};
  }

  template<typename Value, auto R1, typename Rep1>
    requires(!Quantity<Value>) && (!Reference<Value>) &&
            ScalableResultOf<get_quantity_spec(one / R1), std::divides<>, const Value&, Rep1>
  [[nodiscard]] friend constexpr Quantity auto operator/(const Value& val, const quantity<R1, Rep1>& q)
  {
    MP_UNITS_EXPECTS_DEBUG(q != 0);
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

#if MP_UNITS_HOSTED

  template<typename CharT, typename Traits, auto R, typename Rep>
  friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const quantity<R, Rep>& q)
    requires requires { os << q.numerical_value_ref_in(q.unit); }
  {
    return detail::to_stream(os, [&](std::basic_ostream<CharT, Traits>& oss) {
      if constexpr (is_same_v<Rep, std::uint8_t> || is_same_v<Rep, std::int8_t>)
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
  static constexpr Dimension auto dimension = get_dimension(quantity_spec);
  static constexpr quantity_character character = get_character(quantity_spec);
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
#if __cpp_deleted_function
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

  [[nodiscard]] constexpr explicit(!mp_units::implicitly_convertible(quantity_spec, dimensionless)) quantity(rep val)
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

  template<typename Value>
    requires detail::ExplicitFromNumber<reference> && (!detail::RepConstructibleFrom<rep, Value>)
  constexpr explicit(!std::convertible_to<Value, rep> ||
                     !mp_units::implicitly_convertible(quantity_spec, dimensionless)) quantity(Value val)
#if __cpp_deleted_function
    = delete ("Conversion is truncating");
#else
    = delete;
#endif

  template<auto R2, typename Rep2>
    requires detail::QuantityConstructibleFrom<quantity, quantity<R2, Rep2>> && (equivalent(unit, get_unit(R2)))
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  [[nodiscard]] constexpr explicit(!std::convertible_to<Rep2, rep> ||
                                   !mp_units::implicitly_convertible(get_quantity_spec(R2), quantity_spec) ||
                                   !mp_units::implicitly_scalable<get_unit(R2), Rep2, unit, rep>)
    quantity(const quantity<R2, Rep2>& q) :
      numerical_value_is_an_implementation_detail_(q.numerical_value_in(q.unit))
  {
  }

  template<auto R2, typename Rep2>
    requires detail::QuantityConstructibleFrom<quantity, quantity<R2, Rep2>> && (!equivalent(unit, get_unit(R2)))
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  [[nodiscard]] constexpr explicit(!std::convertible_to<Rep2, rep> ||
                                   !mp_units::implicitly_convertible(get_quantity_spec(R2), quantity_spec) ||
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

  template<UnitOf<quantity_spec> ToU>
    requires detail::ExplicitlyCastable<unit, ToU{}, rep>
  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto force_in(ToU) const
  {
    return value_cast<ToU{}>(*this);
  }

  template<RepresentationOf<quantity_spec> ToRep>
    requires std::constructible_from<ToRep, rep>
  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto force_in() const
  {
    return value_cast<ToRep>(*this);
  }

  template<RepresentationOf<quantity_spec> ToRep, UnitOf<quantity_spec> ToU>
    requires std::constructible_from<ToRep, rep> && detail::ExplicitlyCastable<unit, ToU{}, rep>
  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto force_in(ToU) const
  {
    return value_cast<ToU{}, ToRep>(*this);
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
    requires(get_character(quantity_spec).order >= quantity_tensor_order::vector) && detail::HasMagnitude<rep>
  {
    return ::mp_units::magnitude(numerical_value_is_an_implementation_detail_) * unit;
  }

  // tuple-like decomposition of a vector quantity into named 1D-vector components (see
  // framework/vector_components.h).
  template<std::size_t Idx>
#if !(defined MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17)
    requires detail::DecomposableIndex<quantity_spec, Rep, Idx>
#endif
  [[nodiscard]] friend constexpr Quantity auto get(const quantity& q)
  {
    constexpr auto comp = vector_components<quantity_spec>::template axis<Idx>;
    return detail::component_access<Idx>(q.numerical_value_in(unit)) * comp[unit];
  }

#if defined MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17
  template<auto QS>
    requires(!std::integral<decltype(QS)>)
#else
  template<QuantitySpec auto QS>
    requires detail::DecomposableAxis<get_quantity_spec(R), Rep, QS>
#endif
  [[nodiscard]] friend constexpr Quantity auto get(const quantity& q)
  {
    return get<detail::axis_index_of<QS>(vector_components<quantity_spec>{})>(q);
  }

  // data access
  template<Unit U>
    requires(equivalent(U{}, unit))
  [[nodiscard]] constexpr rep& numerical_value_ref_in(U) & noexcept
  {
    return numerical_value_is_an_implementation_detail_;
  }

  template<Unit U>
    requires(equivalent(U{}, unit))
  [[nodiscard]] constexpr const rep& numerical_value_ref_in(U) const& noexcept
  {
    return numerical_value_is_an_implementation_detail_;
  }

  template<Unit U>
    requires(equivalent(U{}, unit))
  constexpr const rep&& numerical_value_ref_in(U) const&& noexcept
#if __cpp_deleted_function
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

  template<UnitOf<quantity_spec> U>
    requires detail::ExplicitlyCastable<unit, U{}, rep>
  [[nodiscard]] constexpr RepresentationOf<quantity_spec> auto force_numerical_value_in(U) const noexcept
  {
    return force_in(U{}).numerical_value_is_an_implementation_detail_;
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
    requires(mp_units::implicitly_convertible(get_quantity_spec(R2), quantity_spec)) &&
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
    requires(mp_units::implicitly_convertible(get_quantity_spec(R2), quantity_spec)) &&
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
            (mp_units::implicitly_convertible(get_quantity_spec(R2), quantity_spec)) &&
            detail::ImplicitConversion<get_unit(R2), Rep2, unit, rep> && requires(rep& a, const Rep2 b) {
              { a %= b } -> std::same_as<rep&>;
            }
  constexpr quantity& operator%=(const quantity<R2, Rep2>& other) &
  {
    MP_UNITS_EXPECTS_DEBUG(other != 0);
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
    MP_UNITS_EXPECTS_DEBUG(val != representation_values<Value>::zero());
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
template<Reference R, typename Value>
  requires RepresentationOf<representation_canonical_type_t<Value>, get_quantity_spec(R{})>
quantity(Value v, R) -> quantity<R{}, representation_canonical_type_t<Value>>;

template<Reference auto R = one, typename Value>
  requires RepresentationOf<representation_canonical_type_t<Value>, get_quantity_spec(R)>
quantity(Value) -> quantity<R, representation_canonical_type_t<Value>>;

template<QuantityLike Q>
quantity(Q) -> quantity<quantity_like_traits<Q>::reference, typename quantity_like_traits<Q>::rep>;

MP_UNITS_EXPORT_END

namespace detail {

template<QuantitySpec auto QS, typename FwdValue>
[[nodiscard]] constexpr Quantity auto make_quantity_of_one(FwdValue&& val)
{
  return quantity{std::forward<FwdValue>(val), QS[one]};
}

}  // namespace detail

}  // namespace mp_units

// Tuple-like protocol for a decomposable vector quantity, enabling structured bindings.
template<auto R, typename Rep>
  requires mp_units::detail::Decomposable<get_quantity_spec(R), Rep>
struct std::tuple_size<mp_units::quantity<R, Rep>> :
    std::integral_constant<std::size_t, mp_units::vector_components<get_quantity_spec(R)>::size> {};

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
struct std::common_type<mp_units::quantity<R1, Rep1>, mp_units::quantity<R2, Rep2>> {
  using type = mp_units::quantity<mp_units::get_common_reference(R1, R2), std::common_type_t<Rep1, Rep2>>;
};

template<auto R, typename Rep, mp_units::RepresentationOf<get_quantity_spec(R)> Value>
  requires(get_unit(R) == mp_units::one) && requires { typename mp_units::quantity<R, std::common_type_t<Rep, Value>>; }
struct std::common_type<mp_units::quantity<R, Rep>, Value> {
  using type = mp_units::quantity<R, std::common_type_t<Rep, Value>>;
};

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
  using format_specs = mp_units::detail::fill_align_width_format_specs<Char>;

  format_specs specs_{};

  std::basic_string_view<Char> modifiers_format_str_;
  std::basic_string<Char> rep_format_str_ = "{}";
  std::basic_string<Char> unit_format_str_ = "{}";
  std::basic_string<Char> dimension_format_str_ = "{}";

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
  // only once for the runtime path, regardless of how many OutputIt types are used.
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

  template<typename OutputIt>
  struct quantity_formatter {
    const formatter& f;
    OutputIt out;
    const quantity_t& q;
    std::locale locale;

    void on_number()
    {
      out = MP_UNITS_STD_FMT::vformat_to(out, locale, f.rep_format_str_,
                                         MP_UNITS_STD_FMT::make_format_args(q.numerical_value_ref_in(q.unit)));
    }
    void on_maybe_space()
    {
      if constexpr (mp_units::space_before_unit_symbol<unit>) *out++ = ' ';
    }
    void on_unit()
    {
      out = MP_UNITS_STD_FMT::vformat_to(out, locale, f.unit_format_str_, MP_UNITS_STD_FMT::make_format_args(q.unit));
    }
    void on_dimension()
    {
      out = MP_UNITS_STD_FMT::vformat_to(out, locale, f.dimension_format_str_,
                                         MP_UNITS_STD_FMT::make_format_args(q.dimension));
    }
    void on_text(const Char* begin, const Char* end) { mp_units::detail::copy(begin, end, out); }

    // Static trampolines for erased_handler — tiny, do not drag in parse_quantity_specs.
    // (Param renamed from `p` so it doesn't shadow `si::unit_symbols::p` (pico) on MSVC C4459.)
    static void on_number_erased(void* self) { static_cast<quantity_formatter*>(self)->on_number(); }
    static void on_maybe_space_erased(void* self) { static_cast<quantity_formatter*>(self)->on_maybe_space(); }
    static void on_unit_erased(void* self) { static_cast<quantity_formatter*>(self)->on_unit(); }
    static void on_dimension_erased(void* self) { static_cast<quantity_formatter*>(self)->on_dimension(); }
    static void on_text_erased(void* self, const Char* b, const Char* e)
    {
      static_cast<quantity_formatter*>(self)->on_text(b, e);
    }

    erased_handler erase()
    {
      return {this, on_number_erased, on_maybe_space_erased, on_unit_erased, on_dimension_erased, on_text_erased};
    }
  };
  template<typename OutputIt, typename... Args>
  quantity_formatter(const formatter&, OutputIt, Args...) -> quantity_formatter<OutputIt>;

  template<std::forward_iterator It, typename Handler>
  constexpr const It parse_quantity_specs(It begin, It end, Handler& handler) const
  {
    if (begin == end || *begin == ':' || *begin == '}') return begin;
    if (*begin != '%')
      throw MP_UNITS_STD_FMT::format_error(
        "`quantity-specs` should start with a `conversion-spec` ('%' characters expected)");
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
      if (ptr == end) throw MP_UNITS_STD_FMT::format_error("invalid `conversion-spec` format");

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
          throw MP_UNITS_STD_FMT::format_error(std::string("unknown `placement-type` token '") + ch + "'");
      }
      begin = ptr;
    }
    if (begin != ptr) handler.on_text(begin, ptr);
    return ptr;
  }

  template<std::forward_iterator It, typename Formatter>
  constexpr It parse_default_spec(It begin, It end, Formatter& f, std::string& format_str)
  {
    if (begin == end || *begin != '[')
      throw MP_UNITS_STD_FMT::format_error("`default-spec` should contain a `[` character");
    auto it = ++begin;
    for (int nested_brackets = 0; it != end && !(*it == ']' && nested_brackets == 0); it++) {
      if (*it == '[') ++nested_brackets;
      if (*it == ']') {
        if (nested_brackets == 0) throw MP_UNITS_STD_FMT::format_error("unmatched ']' in format string");
        --nested_brackets;
      }
    }
    format_str = "{:" + std::string(begin, it) + '}';
    if (it == end) throw MP_UNITS_STD_FMT::format_error("unmatched '[' in format string");
    MP_UNITS_STD_FMT::basic_format_parse_context<Char> ctx(std::string_view(begin, it));
    auto ptr = f.parse(ctx);
    if (ptr != it) throw MP_UNITS_STD_FMT::format_error("invalid subentity format '" + std::string(begin, it) + "'");
    return ++it;  // skip `]`
  }

  template<std::forward_iterator It>
  [[nodiscard]] constexpr It parse_defaults_specs(It begin, It end)
  {
    if (begin == end || *begin == '}') return begin;
    if (*begin++ != ':') throw MP_UNITS_STD_FMT::format_error("`defaults-specs` should start with a `:`");
    do {
      auto ch = *begin++;
      // TODO check if not repeated
      switch (ch) {
        case 'N':
          begin = parse_default_spec(begin, end, rep_formatter_, rep_format_str_);
          break;
        case 'U':
          begin = parse_default_spec(begin, end, unit_formatter_, unit_format_str_);
          break;
        case 'D':
          begin = parse_default_spec(begin, end, dimension_formatter_, dimension_format_str_);
          break;
        default:
          throw MP_UNITS_STD_FMT::format_error(std::string("unknown `subentity-id` token '") + ch + "'");
      }
    } while (begin != end && *begin != '}');
    return begin;
  }

  // Writes the quantity content to `out`. Used for the width-buffering path.
  template<typename OutputIt>
  OutputIt format_to(OutputIt out, const quantity_t& q, const std::locale& locale) const
  {
    if (modifiers_format_str_.empty()) {
      // default layout: rep [space] unit
      out = MP_UNITS_STD_FMT::vformat_to(out, locale, rep_format_str_,
                                         MP_UNITS_STD_FMT::make_format_args(q.numerical_value_ref_in(q.unit)));
      if constexpr (mp_units::space_before_unit_symbol<unit>) *out++ = ' ';
      return MP_UNITS_STD_FMT::vformat_to(out, locale, unit_format_str_, MP_UNITS_STD_FMT::make_format_args(q.unit));
    }
    // user provided format
    quantity_formatter f{*this, out, q, locale};
    // Use erased_handler so parse_quantity_specs is instantiated only once (with erased_handler),
    // not once per OutputIt type.
    auto eh = f.erase();
    parse_quantity_specs(modifiers_format_str_.data(), modifiers_format_str_.data() + modifiers_format_str_.size(), eh);
    return f.out;
  }

public:
  constexpr auto parse(MP_UNITS_STD_FMT::basic_format_parse_context<Char>& ctx) -> decltype(ctx.begin())
  {
    auto begin = ctx.begin(), end = ctx.end();

    begin = parse_fill_align_width(ctx, begin, end, specs_, mp_units::detail::fmt_align::right);
    if (begin == end) return begin;

    const format_checker checker{};
    auto it = parse_quantity_specs(begin, end, checker);
    modifiers_format_str_ = {begin, it};

    return parse_defaults_specs(it, end);
  }

  template<typename FormatContext>
  auto format(const quantity_t& q, FormatContext& ctx) const
  {
    auto specs = specs_;
    mp_units::detail::handle_dynamic_spec<mp_units::detail::width_checker>(specs.width, specs.width_ref, ctx);

    if (specs.width == 0 && modifiers_format_str_.empty()) {
      // Fast path: no modifiers and no width — call pre-parsed sub-formatters directly.
      // No vformat_to, no locale extraction, no allocation. Works correctly even when
      // N[...]/U[...]/D[...] sub-specs were given: the spec is already encoded in the formatters.
      ctx.advance_to(rep_formatter_.format(q.numerical_value_ref_in(q.unit), ctx));
      if constexpr (mp_units::space_before_unit_symbol<unit>) {
        auto it = ctx.out();
        *it++ = ' ';
        ctx.advance_to(it);
      }
      return unit_formatter_.format(q.unit, ctx);
    }

    // Slow path: modifiers or width — must buffer or use vformat_to.
    const std::locale locale = MP_UNITS_FMT_LOCALE(ctx.locale());
    if (specs.width == 0) return format_to(ctx.out(), q, locale);

    std::basic_string<Char> quantity_buffer;
    format_to(std::back_inserter(quantity_buffer), q, locale);
    return mp_units::detail::write_padded<Char>(ctx.out(), std::basic_string_view<Char>{quantity_buffer}, specs.width,
                                                specs.align, specs.fill);
  }
};

#endif  // MP_UNITS_HOSTED
