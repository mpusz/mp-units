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
#include <limits>
#include <utility>
#if MP_UNITS_HOSTED
#include <locale>
#endif
#endif
#endif

namespace mp_units {

namespace detail {

template<Unit UFrom, Unit UTo>
[[nodiscard]] consteval bool integral_conversion_factor(UFrom from, UTo to)
{
  if constexpr (is_same_v<UFrom, UTo>)
    return true;
  else
    return is_integral(get_canonical_unit(from).mag / get_canonical_unit(to).mag);
}

template<typename T, typename Arg>
concept ValuePreservingConstruction =
  std::constructible_from<T, Arg> && is_value_preserving<std::remove_cvref_t<Arg>, T>;

template<typename T, typename Arg>
concept ValuePreservingAssignment = std::assignable_from<T&, Arg> && is_value_preserving<std::remove_cvref_t<Arg>, T>;

template<auto FromUnit, auto ToUnit, typename Rep>
concept ValuePreservingScaling =
  SaneScaling<FromUnit, ToUnit, Rep> &&
  (treat_as_floating_point<Rep> || integral_conversion_factor(FromUnit, ToUnit) ||
   unsatisfied<"Scaling from '{}' to '{}' is not value-preserving for '{}' representation type">(
     unit_symbol(FromUnit), unit_symbol(ToUnit), type_name<Rep>()));

template<auto FromUnit, typename FromRep, auto ToUnit, typename ToRep>
concept ValuePreservingConversion =
  // TODO consider providing constraints of sudo_cast to check if representation types can be scaled between each other
  //  CastableReps<FromRep, ToRep, FromUnit, ToUnit> &&
  SaneScaling<FromUnit, ToUnit, ToRep> &&
  (treat_as_floating_point<ToRep> ||
   (!treat_as_floating_point<FromRep> && integral_conversion_factor(FromUnit, ToUnit)) ||
   unsatisfied<"Scaling from '{}' as '{}' to '{}' as '{}' is not value-preserving">(
     unit_symbol(FromUnit), type_name<FromRep>(), unit_symbol(ToUnit), type_name<ToRep>()));

template<typename QTo, typename QFrom>
concept QuantityConstructibleFrom =
  Quantity<QTo> && Quantity<QFrom> && explicitly_convertible(QFrom::quantity_spec, QTo::quantity_spec) &&
  ValuePreservingConstruction<typename QTo::rep, typename QFrom::rep> &&
  ValuePreservingConversion<QFrom::unit, typename QFrom::rep, QTo::unit, typename QTo::rep>;

template<typename T, typename Rep>
concept ScalarValuePreservingTo = (!Quantity<T>) && Scalar<T> && is_value_preserving<T, Rep>;

template<auto R>
concept DimensionlessOne =
  Reference<MP_UNITS_REMOVE_CONST(decltype(R))> && (implicitly_convertible(get_quantity_spec(R), dimensionless)) &&
  (equivalent(get_unit(R), one));

template<typename Q>
concept DimensionlessOneQuantity = Quantity<Q> && DimensionlessOne<Q::reference>;

template<auto QS, typename Func, typename T, typename U>
concept InvokeResultOf = QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(QS))> && std::regular_invocable<Func, T, U> &&
                         RepresentationOf<std::invoke_result_t<Func, T, U>, QS>;

template<typename Func, typename Q1, typename Q2,
         auto QS = std::invoke_result_t<Func, MP_UNITS_NONCONST_TYPE(Q1::quantity_spec),
                                        MP_UNITS_NONCONST_TYPE(Q2::quantity_spec)>{}>
concept InvocableQuantities = QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(QS))> && Quantity<Q1> && Quantity<Q2> &&
                              InvokeResultOf<QS, Func, typename Q1::rep, typename Q2::rep>;

template<auto R1, auto R2>
concept HaveCommonReference = requires { get_common_reference(R1, R2); };

template<typename Func, Quantity Q1, Quantity Q2>
using common_quantity_for = quantity<get_common_reference(Q1::reference, Q2::reference),
                                     std::invoke_result_t<Func, typename Q1::rep, typename Q2::rep>>;

template<typename Rep, Unit U1, Unit U2>
[[nodiscard]] consteval bool overflows_non_zero_common_values(U1 u1, U2 u2)
{
  constexpr Unit auto cu = get_common_unit(U1{}, U2{});
  return scaling_overflows_non_zero_values<Rep>(u1, cu) || scaling_overflows_non_zero_values<Rep>(u2, cu);
}

template<typename Func, typename Q1, typename Q2>
concept CommonlyInvocableQuantities =
  Quantity<Q1> && Quantity<Q2> && HaveCommonReference<Q1::reference, Q2::reference> &&
  std::convertible_to<Q1, common_quantity_for<Func, Q1, Q2>> &&
  std::convertible_to<Q2, common_quantity_for<Func, Q1, Q2>> &&
  InvocableQuantities<Func, Q1, Q2, get_common_quantity_spec(Q1::quantity_spec, Q2::quantity_spec)> &&
  (!overflows_non_zero_common_values<std::invoke_result_t<Func, typename Q1::rep, typename Q2::rep>>(Q1::unit,
                                                                                                     Q2::unit));

template<typename T>
using quantity_like_type = quantity<quantity_like_traits<T>::reference, typename quantity_like_traits<T>::rep>;

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
class quantity {
public:
  Rep numerical_value_is_an_implementation_detail_;  ///< needs to be public for a structural type

  // member types and values
  static constexpr Reference auto reference = R;
  static constexpr QuantitySpec auto quantity_spec = get_quantity_spec(reference);
  static constexpr Dimension auto dimension = quantity_spec.dimension;
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

  // construction, assignment, destruction
  quantity() = default;
  quantity(const quantity&) = default;
  quantity(quantity&&) = default;
  ~quantity() = default;

  template<typename FwdValue, Reference R2>
    requires(equivalent(unit, get_unit(R2{}))) && detail::ValuePreservingConstruction<rep, FwdValue>
  constexpr quantity(FwdValue&& val, R2) : numerical_value_is_an_implementation_detail_(std::forward<FwdValue>(val))
  {
  }

  template<typename FwdValue, Reference R2>
    requires(!equivalent(unit, get_unit(R2{}))) &&
            detail::QuantityConstructibleFrom<quantity, quantity<R2{}, std::remove_cvref_t<FwdValue>>>
  constexpr quantity(FwdValue&& val, R2) : quantity(::mp_units::quantity{std::forward<FwdValue>(val), R2{}})
  {
  }

  template<typename FwdValue>
    requires detail::DimensionlessOne<reference> && detail::ValuePreservingConstruction<rep, FwdValue>
  constexpr explicit(!std::convertible_to<FwdValue, rep>) quantity(FwdValue&& val) :
      numerical_value_is_an_implementation_detail_(std::forward<FwdValue>(val))
  {
  }

  template<auto R2, typename Rep2>
    requires detail::QuantityConstructibleFrom<quantity, quantity<R2, Rep2>>
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr explicit(!implicitly_convertible(get_quantity_spec(R2), quantity_spec) || !std::convertible_to<Rep2, rep>)
    quantity(const quantity<R2, Rep2>& q) :
      quantity(detail::sudo_cast<quantity>(q))
  {
  }

  template<QuantityLike Q>
    requires detail::QuantityConstructibleFrom<quantity, detail::quantity_like_type<Q>>
  constexpr explicit(quantity_like_traits<Q>::explicit_import ||
                     // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
                     !std::convertible_to<detail::quantity_like_type<Q>, quantity>) quantity(const Q& q) :
      quantity(::mp_units::quantity{quantity_like_traits<Q>::to_numerical_value(q), quantity_like_traits<Q>::reference})
  {
  }

  quantity& operator=(const quantity&) = default;
  quantity& operator=(quantity&&) = default;

  template<typename FwdValue>
    requires detail::DimensionlessOne<reference> && detail::ValuePreservingAssignment<rep, FwdValue>
  constexpr quantity& operator=(FwdValue&& val)
  {
    numerical_value_is_an_implementation_detail_ = std::forward<FwdValue>(val);
    return *this;
  }

  template<MP_UNITS_WEAK_UNIT_OF(quantity_spec) ToU>
    requires detail::ValuePreservingScaling<unit, ToU{}, rep>
  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto in(ToU) const
  {
    return quantity<detail::make_reference(quantity_spec, ToU{}), Rep>{*this};
  }

  template<RepresentationOf<quantity_spec> ToRep>
    requires detail::ValuePreservingConstruction<ToRep, rep>
  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto in() const
  {
    return quantity<reference, ToRep>{*this};
  }

  template<RepresentationOf<quantity_spec> ToRep, MP_UNITS_WEAK_UNIT_OF(quantity_spec) ToU>
    requires detail::ValuePreservingConstruction<ToRep, rep> &&
             detail::ValuePreservingConversion<unit, rep, ToU{}, ToRep>
  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto in(ToU) const
  {
    return quantity<detail::make_reference(quantity_spec, ToU{}), ToRep>{*this};
  }

  template<MP_UNITS_WEAK_UNIT_OF(quantity_spec) ToU>
    requires detail::SaneScaling<unit, ToU{}, rep>
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

  template<RepresentationOf<quantity_spec> ToRep, MP_UNITS_WEAK_UNIT_OF(quantity_spec) ToU>
    requires std::constructible_from<ToRep, rep> && detail::SaneScaling<unit, ToU{}, rep>
  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto force_in(ToU) const
  {
    return value_cast<ToU{}, ToRep>(*this);
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
    = delete("Can't form a reference to a temporary");
#else
    = delete;
#endif

  template<MP_UNITS_WEAK_UNIT_OF(quantity_spec) U>
    requires detail::ValuePreservingScaling<unit, U{}, rep>
  [[nodiscard]] constexpr rep numerical_value_in(U) const noexcept
  {
    return in(U{}).numerical_value_is_an_implementation_detail_;
  }

  template<MP_UNITS_WEAK_UNIT_OF(quantity_spec) U>
    requires detail::SaneScaling<unit, U{}, rep>
  [[nodiscard]] constexpr rep force_numerical_value_in(U) const noexcept
  {
    return force_in(U{}).numerical_value_is_an_implementation_detail_;
  }

  // conversion operators
  template<typename V_, std::constructible_from<rep> Value = std::remove_cvref_t<V_>>
    requires detail::DimensionlessOne<reference>
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
    requires(implicitly_convertible(get_quantity_spec(R2), quantity_spec)) &&
            detail::ValuePreservingConversion<get_unit(R2), Rep2, unit, rep> && requires(rep& a, const Rep2 b) {
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
    requires(implicitly_convertible(get_quantity_spec(R2), quantity_spec)) &&
            detail::ValuePreservingConversion<get_unit(R2), Rep2, unit, rep> && requires(rep& a, const Rep2 b) {
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
    requires(!treat_as_floating_point<rep>) && (implicitly_convertible(get_quantity_spec(R2), quantity_spec)) &&
            detail::ValuePreservingConversion<get_unit(R2), Rep2, unit, rep> && requires(rep& a, const Rep2 b) {
              { a %= b } -> std::same_as<rep&>;
            }
  constexpr quantity& operator%=(const quantity<R2, Rep2>& other) &
  {
    MP_UNITS_EXPECTS_DEBUG(is_neq_zero(other));
    if constexpr (equivalent(unit, get_unit(R2)))
      numerical_value_is_an_implementation_detail_ %= other.numerical_value_is_an_implementation_detail_;
    else
      numerical_value_is_an_implementation_detail_ %= other.in(unit).numerical_value_is_an_implementation_detail_;
    return *this;
  }

  template<detail::ScalarValuePreservingTo<rep> Value>
    requires requires(rep& a, const Value b) {
      { a *= b } -> std::same_as<rep&>;
    }
  constexpr quantity& operator*=(const Value& val) &
  {
    numerical_value_is_an_implementation_detail_ *= val;
    return *this;
  }

  template<detail::DimensionlessOneQuantity Q2>
    requires detail::ScalarValuePreservingTo<typename Q2::rep, rep> && requires(rep& a, const Q2::rep b) {
      { a *= b } -> std::same_as<rep&>;
    }
  constexpr quantity& operator*=(const Q2& other) &
  {
    return *this *= other.numerical_value_is_an_implementation_detail_;
  }

  template<detail::ScalarValuePreservingTo<rep> Value>
    requires requires(rep& a, const Value b) {
      { a /= b } -> std::same_as<rep&>;
    }
  constexpr quantity& operator/=(const Value& val) &
  {
    MP_UNITS_EXPECTS_DEBUG(val != representation_values<Value>::zero());
    numerical_value_is_an_implementation_detail_ /= val;
    return *this;
  }

  template<detail::DimensionlessOneQuantity Q2>
    requires detail::ScalarValuePreservingTo<typename Q2::rep, rep> && requires(rep& a, const Q2::rep b) {
      { a /= b } -> std::same_as<rep&>;
    }
  constexpr quantity& operator/=(const Q2& rhs) &
  {
    return *this /= rhs.numerical_value_is_an_implementation_detail_;
  }

  // binary operators on quantities
  template<std::derived_from<quantity> Q, auto R2, typename Rep2>
    requires detail::CommonlyInvocableQuantities<std::plus<>, quantity, quantity<R2, Rep2>>
  [[nodiscard]] friend constexpr Quantity auto operator+(const Q& lhs, const quantity<R2, Rep2>& rhs)
  {
    using ret = detail::common_quantity_for<std::plus<>, quantity, quantity<R2, Rep2>>;
    const ret ret_lhs(lhs);
    const ret ret_rhs(rhs);
    return ::mp_units::quantity{ret_lhs.numerical_value_ref_in(ret::unit) + ret_rhs.numerical_value_ref_in(ret::unit),
                                ret::reference};
  }

  template<std::derived_from<quantity> Q, RepresentationOf<quantity_spec> Value>
    requires detail::DimensionlessOne<Q::reference> &&
             detail::InvokeResultOf<quantity_spec, std::plus<>, Rep, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator+(const Q& lhs, const Value& rhs)
  {
    return lhs + ::mp_units::quantity{rhs};
  }

  template<std::derived_from<quantity> Q, RepresentationOf<quantity_spec> Value>
    requires detail::DimensionlessOne<Q::reference> &&
             detail::InvokeResultOf<quantity_spec, std::plus<>, Rep, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator+(const Value& lhs, const Q& rhs)
  {
    return ::mp_units::quantity{lhs} + rhs;
  }

  template<std::derived_from<quantity> Q, auto R2, typename Rep2>
    requires detail::CommonlyInvocableQuantities<std::minus<>, quantity, quantity<R2, Rep2>>
  [[nodiscard]] friend constexpr Quantity auto operator-(const Q& lhs, const quantity<R2, Rep2>& rhs)
  {
    using ret = detail::common_quantity_for<std::minus<>, quantity, quantity<R2, Rep2>>;
    const ret ret_lhs(lhs);
    const ret ret_rhs(rhs);
    return ::mp_units::quantity{ret_lhs.numerical_value_ref_in(ret::unit) - ret_rhs.numerical_value_ref_in(ret::unit),
                                ret::reference};
  }

  template<std::derived_from<quantity> Q, RepresentationOf<quantity_spec> Value>
    requires detail::DimensionlessOne<Q::reference> &&
             detail::InvokeResultOf<quantity_spec, std::minus<>, Rep, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator-(const Q& lhs, const Value& rhs)
  {
    return lhs - ::mp_units::quantity{rhs};
  }

  template<std::derived_from<quantity> Q, RepresentationOf<quantity_spec> Value>
    requires detail::DimensionlessOne<Q::reference> &&
             detail::InvokeResultOf<quantity_spec, std::minus<>, Rep, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator-(const Value& lhs, const Q& rhs)
  {
    return ::mp_units::quantity{lhs} - rhs;
  }

  template<std::derived_from<quantity> Q, auto R2, typename Rep2>
    requires(!treat_as_floating_point<Rep>) && (!treat_as_floating_point<Rep2>) &&
            detail::CommonlyInvocableQuantities<std::modulus<>, quantity, quantity<R2, Rep2>>
  [[nodiscard]] friend constexpr Quantity auto operator%(const Q& lhs, const quantity<R2, Rep2>& rhs)
  {
    MP_UNITS_EXPECTS_DEBUG(is_neq_zero(rhs));
    using ret = detail::common_quantity_for<std::modulus<>, quantity, quantity<R2, Rep2>>;
    const ret ret_lhs(lhs);
    const ret ret_rhs(rhs);
    return ::mp_units::quantity{ret_lhs.numerical_value_ref_in(ret::unit) % ret_rhs.numerical_value_ref_in(ret::unit),
                                ret::reference};
  }

  template<std::derived_from<quantity> Q, RepresentationOf<quantity_spec> Value>
    requires detail::DimensionlessOne<Q::reference> &&
             detail::InvokeResultOf<quantity_spec, std::modulus<>, Rep, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator%(const Q& lhs, const Value& rhs)
  {
    return lhs % ::mp_units::quantity{rhs};
  }

  template<std::derived_from<quantity> Q, RepresentationOf<quantity_spec> Value>
    requires detail::DimensionlessOne<Q::reference> &&
             detail::InvokeResultOf<quantity_spec, std::modulus<>, Rep, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator%(const Value& lhs, const Q& rhs)
  {
    return ::mp_units::quantity{lhs} % rhs;
  }

  template<std::derived_from<quantity> Q, auto R2, typename Rep2>
    requires detail::InvocableQuantities<std::multiplies<>, quantity, quantity<R2, Rep2>>
  [[nodiscard]] friend constexpr Quantity auto operator*(const Q& lhs, const quantity<R2, Rep2>& rhs)
  {
    return ::mp_units::quantity{lhs.numerical_value_ref_in(unit) * rhs.numerical_value_ref_in(rhs.unit), R * R2};
  }

  template<std::derived_from<quantity> Q, typename Value>
    requires(!Quantity<Value>) &&
            (!Reference<Value>) && detail::InvokeResultOf<quantity_spec, std::multiplies<>, rep, const Value&>
  [[nodiscard]] friend constexpr QuantityOf<quantity_spec> auto operator*(const Q& q, const Value& val)
  {
    return ::mp_units::quantity{q.numerical_value_ref_in(unit) * val, R};
  }

  template<typename Value, std::derived_from<quantity> Q>
    requires(!Quantity<Value>) &&
            (!Reference<Value>) && detail::InvokeResultOf<quantity_spec, std::multiplies<>, const Value&, rep>
  [[nodiscard]] friend constexpr QuantityOf<quantity_spec> auto operator*(const Value& val, const Q& q)
  {
    return ::mp_units::quantity{val * q.numerical_value_ref_in(unit), R};
  }

  template<std::derived_from<quantity> Q, auto R2, typename Rep2>
    requires detail::InvocableQuantities<std::divides<>, quantity, quantity<R2, Rep2>>
  [[nodiscard]] friend constexpr Quantity auto operator/(const Q& lhs, const quantity<R2, Rep2>& rhs)
  {
    MP_UNITS_EXPECTS_DEBUG(is_neq_zero(rhs));
    return ::mp_units::quantity{lhs.numerical_value_ref_in(unit) / rhs.numerical_value_ref_in(rhs.unit), R / R2};
  }

  template<std::derived_from<quantity> Q, typename Value>
    requires(!Quantity<Value>) &&
            (!Reference<Value>) && detail::InvokeResultOf<quantity_spec, std::divides<>, rep, const Value&>
  [[nodiscard]] friend constexpr QuantityOf<quantity_spec> auto operator/(const Q& q, const Value& val)
  {
    MP_UNITS_EXPECTS_DEBUG(val != representation_values<Value>::zero());
    return ::mp_units::quantity{q.numerical_value_ref_in(unit) / val, R};
  }

  template<typename Value, std::derived_from<quantity> Q>
    requires(!Quantity<Value>) &&
            (!Reference<Value>) && detail::InvokeResultOf<quantity_spec, std::divides<>, const Value&, rep>
  [[nodiscard]] friend constexpr Quantity auto operator/(const Value& val, const Q& q)
  {
    MP_UNITS_EXPECTS_DEBUG(is_neq_zero(q));
    return ::mp_units::quantity{val / q.numerical_value_ref_in(unit), one / R};
  }

  template<std::derived_from<quantity> Q, auto R2, typename Rep2>
    requires requires { typename std::common_type_t<quantity, quantity<R2, Rep2>>; } &&
             std::equality_comparable<typename std::common_type_t<quantity, quantity<R2, Rep2>>::rep>
  [[nodiscard]] friend constexpr bool operator==(const Q& lhs, const quantity<R2, Rep2>& rhs)
  {
    using ct = std::common_type_t<quantity, quantity<R2, Rep2>>;
    const ct ct_lhs(lhs);
    const ct ct_rhs(rhs);
    return ct_lhs.numerical_value_ref_in(ct::unit) == ct_rhs.numerical_value_ref_in(ct::unit);
  }

  template<std::derived_from<quantity> Q, RepresentationOf<quantity_spec> Value>
    requires detail::DimensionlessOne<Q::reference> && std::equality_comparable_with<rep, Value>
  [[nodiscard]] friend constexpr bool operator==(const Q& lhs, const Value& rhs)
  {
    return lhs.numerical_value_ref_in(unit) == rhs;
  }

  template<std::derived_from<quantity> Q, auto R2, typename Rep2>
    requires requires { typename std::common_type_t<quantity, quantity<R2, Rep2>>; } &&
             std::three_way_comparable<typename std::common_type_t<quantity, quantity<R2, Rep2>>::rep>
  [[nodiscard]] friend constexpr auto operator<=>(const Q& lhs, const quantity<R2, Rep2>& rhs)
  {
    using ct = std::common_type_t<quantity, quantity<R2, Rep2>>;
    const ct ct_lhs(lhs);
    const ct ct_rhs(rhs);
    return ct_lhs.numerical_value_ref_in(ct::unit) <=> ct_rhs.numerical_value_ref_in(ct::unit);
  }

  template<std::derived_from<quantity> Q, RepresentationOf<quantity_spec> Value>
    requires detail::DimensionlessOne<Q::reference> && std::three_way_comparable_with<rep, Value>
  [[nodiscard]] friend constexpr auto operator<=>(const Q& lhs, const Value& rhs)
  {
    return lhs.numerical_value_ref_in(unit) <=> rhs;
  }
};

// CTAD
template<Reference R, RepresentationOf<get_quantity_spec(R{})> Value>
quantity(Value v, R) -> quantity<R{}, Value>;

#if MP_UNITS_COMP_GCC == 14
template<detail::SomeRepresentation Value>
#else
template<RepresentationOf<get_quantity_spec(one)> Value>
#endif
explicit(false) quantity(Value) -> quantity<one, Value>;

template<QuantityLike Q>
explicit(quantity_like_traits<Q>::explicit_import) quantity(Q)
  -> quantity<quantity_like_traits<Q>::reference, typename quantity_like_traits<Q>::rep>;

#if MP_UNITS_HOSTED

template<typename CharT, typename Traits, auto R, typename Rep>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const quantity<R, Rep>& q)
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

MP_UNITS_EXPORT_END

}  // namespace mp_units

template<auto R1, typename Rep1, auto R2, typename Rep2>
  requires requires {
    { mp_units::get_common_reference(R1, R2) } -> mp_units::Reference;
    typename std::common_type_t<Rep1, Rep2>;
    requires(!mp_units::detail::overflows_non_zero_common_values<std::common_type_t<Rep1, Rep2>>(
      mp_units::get_unit(R1), mp_units::get_unit(R2)));
    requires mp_units::RepresentationOf<std::common_type_t<Rep1, Rep2>,
                                        mp_units::get_common_quantity_spec(mp_units::get_quantity_spec(R1),
                                                                           mp_units::get_quantity_spec(R2))>;
  }
struct std::common_type<mp_units::quantity<R1, Rep1>, mp_units::quantity<R2, Rep2>> {
  using type = mp_units::quantity<mp_units::get_common_reference(R1, R2), std::common_type_t<Rep1, Rep2>>;
};

template<auto R, typename Rep, mp_units::RepresentationOf<mp_units::get_quantity_spec(R)> Value>
  requires(get_unit(R) == mp_units::one) && requires { typename mp_units::quantity<R, std::common_type_t<Rep, Value>>; }
struct std::common_type<mp_units::quantity<R, Rep>, Value> {
  using type = mp_units::quantity<R, std::common_type_t<Rep, Value>>;
};

template<auto R, typename Rep, mp_units::RepresentationOf<mp_units::get_quantity_spec(R)> Value>
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
  static constexpr auto dimension = get_quantity_spec(Reference).dimension;

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
    template<std::forward_iterator It>
    void on_text(It begin, It end) const
    {
      mp_units::detail::copy(begin, end, out);
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

  template<typename OutputIt, typename FormatContext>
  OutputIt format_quantity(OutputIt out, const quantity_t& q, FormatContext& ctx) const
  {
    const std::locale locale = MP_UNITS_FMT_LOCALE(ctx.locale());
    if (modifiers_format_str_.empty()) {
      // default
      out = MP_UNITS_STD_FMT::vformat_to(out, locale, rep_format_str_,
                                         MP_UNITS_STD_FMT::make_format_args(q.numerical_value_ref_in(q.unit)));
      if constexpr (mp_units::space_before_unit_symbol<unit>) *out++ = ' ';
      return MP_UNITS_STD_FMT::vformat_to(out, locale, unit_format_str_, MP_UNITS_STD_FMT::make_format_args(q.unit));
    }
    // user provided format
    quantity_formatter f{*this, out, q, locale};
    parse_quantity_specs(modifiers_format_str_.data(), modifiers_format_str_.data() + modifiers_format_str_.size(), f);
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

    if (specs.width == 0) {
      // Avoid extra copying if width is not specified
      format_quantity(ctx.out(), q, ctx);
      return ctx.out();
    }
    std::basic_string<Char> quantity_buffer;
    format_quantity(std::back_inserter(quantity_buffer), q, ctx);

    std::basic_string<Char> fill_align_width_format_str;
    mp_units::detail::format_global_buffer(std::back_inserter(fill_align_width_format_str), specs);
    return MP_UNITS_STD_FMT::vformat_to(ctx.out(), fill_align_width_format_str,
                                        MP_UNITS_STD_FMT::make_format_args(quantity_buffer));
  }
};

#endif  // MP_UNITS_HOSTED
