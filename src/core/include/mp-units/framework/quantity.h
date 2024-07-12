
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
#include <mp-units/compat_macros.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/dimension_concepts.h>
#include <mp-units/framework/quantity_concepts.h>
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/reference.h>
#include <mp-units/framework/reference_concepts.h>
#include <mp-units/framework/representation_concepts.h>
#include <mp-units/framework/unit_concepts.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/ext/contracts.h>
#include <compare>  // IWYU pragma: export
#include <utility>
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

template<typename T>
concept IsFloatingPoint = treat_as_floating_point<T>;

template<typename QFrom, typename QTo>
concept QuantityConvertibleTo =
  Quantity<QFrom> && Quantity<QTo> && detail::QuantitySpecConvertibleTo<QFrom::quantity_spec, QTo::quantity_spec> &&
  detail::UnitConvertibleTo<QFrom::unit, QTo::unit> &&
  (IsFloatingPoint<typename QTo::rep> ||
   (!IsFloatingPoint<typename QFrom::rep> && (integral_conversion_factor(QFrom::unit, QTo::unit)))) &&
  // TODO consider providing constraints of sudo_cast here rather than testing if it can be called (its return type is
  // deduced thus the function is evaluated here and may emit truncating conversion or other warnings)
  requires(QFrom q) { detail::sudo_cast<QTo>(q); };

template<quantity_character Ch, typename Func, typename T, typename U>
concept InvokeResultOf = std::regular_invocable<Func, T, U> && RepresentationOf<std::invoke_result_t<Func, T, U>, Ch>;

template<typename Func, typename Q1, typename Q2,
         quantity_character Ch = std::invoke_result_t<Func, std::remove_const_t<decltype(Q1::quantity_spec)>,
                                                      std::remove_const_t<decltype(Q2::quantity_spec)>>::character>
concept InvocableQuantities =
  Quantity<Q1> && Quantity<Q2> && InvokeResultOf<Ch, Func, typename Q1::rep, typename Q2::rep>;

// TODO remove the following when clang diagnostics improve
// https://github.com/llvm/llvm-project/issues/96660
template<auto R1, auto R2>
concept HaveCommonReferenceImpl = requires { common_reference(R1, R2); };

template<auto R1, auto R2>
concept HaveCommonReference = HaveCommonReferenceImpl<R1, R2>;

template<typename Func, typename Q1, typename Q2>
concept CommonlyInvocableQuantities =
  Quantity<Q1> && Quantity<Q2> && HaveCommonReference<Q1::reference, Q2::reference> &&
  InvocableQuantities<Func, Q1, Q2, common_quantity_spec(Q1::quantity_spec, Q2::quantity_spec).character>;

template<typename Func, Quantity Q1, Quantity Q2>
  requires detail::CommonlyInvocableQuantities<Func, Q1, Q2>
using common_quantity_for = quantity<common_reference(Q1::reference, Q2::reference),
                                     std::invoke_result_t<Func, typename Q1::rep, typename Q2::rep>>;

template<auto R1, auto R2, typename Rep1, typename Rep2>
concept SameValueAs = detail::SameReference<R1, R2> && std::same_as<Rep1, Rep2>;

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
template<Reference auto R, RepresentationOf<get_quantity_spec(R).character> Rep = double>
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
    requires requires { quantity_values<rep>::zero(); }
  {
    return {quantity_values<rep>::zero(), R};
  }

  [[nodiscard]] static constexpr quantity one() noexcept
    requires requires { quantity_values<rep>::one(); }
  {
    return {quantity_values<rep>::one(), R};
  }

  [[nodiscard]] static constexpr quantity min() noexcept
    requires requires { quantity_values<rep>::min(); }
  {
    return {quantity_values<rep>::min(), R};
  }

  [[nodiscard]] static constexpr quantity max() noexcept
    requires requires { quantity_values<rep>::max(); }
  {
    return {quantity_values<rep>::max(), R};
  }

  // construction, assignment, destruction
  quantity() = default;
  quantity(const quantity&) = default;
  quantity(quantity&&) = default;
  ~quantity() = default;

  template<typename Value, Reference R2>
    requires detail::SameValueAs<R2{}, R, std::remove_cvref_t<Value>, Rep>
  constexpr quantity(Value&& v, R2) : numerical_value_is_an_implementation_detail_(std::forward<Value>(v))
  {
  }

  template<typename Value, Reference R2>
    requires(!detail::SameValueAs<R2{}, R, std::remove_cvref_t<Value>, Rep>) &&
            detail::QuantityConvertibleTo<quantity<R2{}, std::remove_cvref_t<Value>>, quantity>
  constexpr quantity(Value&& v, R2) : quantity(quantity<R2{}, std::remove_cvref_t<Value>>{std::forward<Value>(v), R2{}})
  {
  }

  template<detail::QuantityConvertibleTo<quantity> Q>
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr explicit(!std::convertible_to<typename Q::rep, Rep>) quantity(const Q& q) :
      numerical_value_is_an_implementation_detail_(
        detail::sudo_cast<quantity>(q).numerical_value_is_an_implementation_detail_)
  {
  }

  template<QuantityLike Q>
    requires detail::QuantityConvertibleTo<detail::quantity_like_type<Q>, quantity>
  constexpr explicit(is_specialization_of<decltype(quantity_like_traits<Q>::to_numerical_value(std::declval<Q>())),
                                          convert_explicitly> ||
                     // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
                     !std::convertible_to<typename quantity_like_traits<Q>::rep, Rep>) quantity(const Q& q) :
      quantity(
        ::mp_units::quantity{quantity_like_traits<Q>::to_numerical_value(q).value, quantity_like_traits<Q>::reference})
  {
  }

  quantity& operator=(const quantity&) = default;
  quantity& operator=(quantity&&) = default;

  // unit conversions
  template<detail::UnitCompatibleWith<unit, quantity_spec> ToU>
    requires detail::QuantityConvertibleTo<quantity, quantity<detail::make_reference(quantity_spec, ToU{}), Rep>>
  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto in(ToU) const
  {
    return quantity<detail::make_reference(quantity_spec, ToU{}), Rep>{*this};
  }

  template<RepresentationOf<quantity_spec.character> ToRep>
    requires detail::QuantityConvertibleTo<quantity, quantity<reference, ToRep>>
  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto in() const
  {
    return quantity<reference, ToRep>{*this};
  }

  template<RepresentationOf<quantity_spec.character> ToRep, detail::UnitCompatibleWith<unit, quantity_spec> ToU>
    requires detail::QuantityConvertibleTo<quantity, quantity<detail::make_reference(quantity_spec, ToU{}), ToRep>>
  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto in(ToU) const
  {
    return quantity<detail::make_reference(quantity_spec, ToU{}), ToRep>{*this};
  }

  template<detail::UnitCompatibleWith<unit, quantity_spec> ToU>
    requires requires(quantity q) { value_cast<ToU{}>(q); }
  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto force_in(ToU) const
  {
    return value_cast<ToU{}>(*this);
  }

  template<RepresentationOf<quantity_spec.character> ToRep>
    requires requires(quantity q) { value_cast<ToRep>(q); }
  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto force_in() const
  {
    return value_cast<ToRep>(*this);
  }

  template<RepresentationOf<quantity_spec.character> ToRep, detail::UnitCompatibleWith<unit, quantity_spec> ToU>
    requires requires(quantity q) { value_cast<ToU{}, ToRep>(q); }
  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto force_in(ToU) const
  {
    return value_cast<ToU{}, ToRep>(*this);
  }

  // data access
  template<Unit U>
    requires(U{} == unit)
  [[nodiscard]] constexpr rep& numerical_value_ref_in(U) & noexcept
  {
    return numerical_value_is_an_implementation_detail_;
  }

  template<Unit U>
    requires(U{} == unit)
  [[nodiscard]] constexpr const rep& numerical_value_ref_in(U) const& noexcept
  {
    return numerical_value_is_an_implementation_detail_;
  }

  template<Unit U>
    requires(U{} == unit)
  constexpr const rep&& numerical_value_ref_in(U) const&& noexcept
#if __cpp_deleted_function
    = delete("Can't form a reference to a temporary");
#else
    = delete;
#endif

  template<detail::UnitCompatibleWith<unit, quantity_spec> U>
    requires detail::QuantityConvertibleTo<quantity, quantity<detail::make_reference(quantity_spec, U{}), Rep>>
  [[nodiscard]] constexpr rep numerical_value_in(U) const noexcept
  {
    return (*this).in(U{}).numerical_value_is_an_implementation_detail_;
  }

  template<detail::UnitCompatibleWith<unit, quantity_spec> U>
    requires requires(quantity q) { value_cast<U{}>(q); }
  [[nodiscard]] constexpr rep force_numerical_value_in(U) const noexcept
  {
    return (*this).force_in(U{}).numerical_value_is_an_implementation_detail_;
  }

  // conversion operators
  template<typename Q_, QuantityLike Q = std::remove_cvref_t<Q_>>
    requires detail::QuantityConvertibleTo<quantity, detail::quantity_like_type<Q>>
  [[nodiscard]] explicit(is_specialization_of<decltype(quantity_like_traits<Q>::from_numerical_value(
                                                numerical_value_is_an_implementation_detail_)),
                                              convert_explicitly> ||
                         !std::convertible_to<Rep, typename quantity_like_traits<Q>::rep>) constexpr
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  operator Q_() const& noexcept(
    noexcept(quantity_like_traits<Q>::from_numerical_value(numerical_value_is_an_implementation_detail_)) &&
    std::is_nothrow_copy_constructible_v<rep>)
  {
    return quantity_like_traits<Q>::from_numerical_value(numerical_value_is_an_implementation_detail_).value;
  }

  template<typename Q_, QuantityLike Q = std::remove_cvref_t<Q_>>
    requires detail::QuantityConvertibleTo<quantity, detail::quantity_like_type<Q>>
  [[nodiscard]] explicit(is_specialization_of<decltype(quantity_like_traits<Q>::from_numerical_value(
                                                numerical_value_is_an_implementation_detail_)),
                                              convert_explicitly> ||
                         !std::convertible_to<Rep, typename quantity_like_traits<Q>::rep>) constexpr
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  operator Q_() && noexcept(
    noexcept(quantity_like_traits<Q>::from_numerical_value(numerical_value_is_an_implementation_detail_)) &&
    std::is_nothrow_move_constructible_v<rep>)
  {
    return quantity_like_traits<Q>::from_numerical_value(std::move(numerical_value_is_an_implementation_detail_)).value;
  }

  // member unary operators
  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto operator+() const
    requires requires(rep v) {
      {
        +v
      } -> std::common_with<rep>;
    }
  {
    return ::mp_units::quantity{+numerical_value_is_an_implementation_detail_, reference};
  }

  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto operator-() const
    requires requires(rep v) {
      {
        -v
      } -> std::common_with<rep>;
    }
  {
    return ::mp_units::quantity{-numerical_value_is_an_implementation_detail_, reference};
  }

  template<typename Q>
  friend constexpr decltype(auto) operator++(Q&& q)
    requires std::derived_from<std::remove_cvref_t<Q>, quantity> && requires(rep v) {
      {
        ++v
      } -> std::same_as<rep&>;
    }
  {
    ++q.numerical_value_is_an_implementation_detail_;
    return std::forward<Q>(q);
  }

  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto operator++(int)
    requires requires(rep v) {
      {
        v++
      } -> std::common_with<rep>;
    }
  {
    return ::mp_units::quantity{numerical_value_is_an_implementation_detail_++, reference};
  }

  template<typename Q>
  friend constexpr decltype(auto) operator--(Q&& q)
    requires std::derived_from<std::remove_cvref_t<Q>, quantity> && requires(rep v) {
      {
        --v
      } -> std::same_as<rep&>;
    }
  {
    --q.numerical_value_is_an_implementation_detail_;
    return std::forward<Q>(q);
  }

  [[nodiscard]] constexpr QuantityOf<quantity_spec> auto operator--(int)
    requires requires(rep v) {
      {
        v--
      } -> std::common_with<rep>;
    }
  {
    return ::mp_units::quantity{numerical_value_is_an_implementation_detail_--, reference};
  }

  // compound assignment operators
  template<typename Q>
    requires std::derived_from<std::remove_cvref_t<Q>, quantity> && requires(rep a, rep b) {
      {
        a += b
      } -> std::same_as<rep&>;
    }
  friend constexpr decltype(auto) operator+=(Q&& lhs, const quantity& rhs)
  {
    lhs.numerical_value_is_an_implementation_detail_ += rhs.numerical_value_is_an_implementation_detail_;
    return std::forward<Q>(lhs);
  }

  template<typename Q>
    requires std::derived_from<std::remove_cvref_t<Q>, quantity> && requires(rep a, rep b) {
      {
        a -= b
      } -> std::same_as<rep&>;
    }
  friend constexpr decltype(auto) operator-=(Q&& lhs, const quantity& rhs)
  {
    lhs.numerical_value_is_an_implementation_detail_ -= rhs.numerical_value_is_an_implementation_detail_;
    return std::forward<Q>(lhs);
  }

  template<typename Q>
    requires std::derived_from<std::remove_cvref_t<Q>, quantity> && (!treat_as_floating_point<rep>) &&
             requires(rep a, rep b) {
               {
                 a %= b
               } -> std::same_as<rep&>;
             }
  friend constexpr decltype(auto) operator%=(Q&& lhs, const quantity& rhs)

  {
    MP_UNITS_EXPECTS_DEBUG(rhs != zero());
    lhs.numerical_value_is_an_implementation_detail_ %= rhs.numerical_value_is_an_implementation_detail_;
    return std::forward<Q>(lhs);
  }

  template<typename Q, typename Value>
    requires std::derived_from<std::remove_cvref_t<Q>, quantity> && (!Quantity<Value>) &&
             requires(rep a, const Value b) {
               {
                 a *= b
               } -> std::same_as<rep&>;
             }
  friend constexpr decltype(auto) operator*=(Q&& lhs, const Value& v)
  {
    lhs.numerical_value_is_an_implementation_detail_ *= v;
    return std::forward<Q>(lhs);
  }

  template<typename Q1, QuantityOf<dimensionless> Q2>
    requires std::derived_from<std::remove_cvref_t<Q1>, quantity> && (Q2::unit == ::mp_units::one) &&
             requires(rep a, const typename Q2::rep b) {
               {
                 a *= b
               } -> std::same_as<rep&>;
             }
  friend constexpr decltype(auto) operator*=(Q1&& lhs, const Q2& rhs)
  {
    lhs.numerical_value_is_an_implementation_detail_ *= rhs.numerical_value_is_an_implementation_detail_;
    return std::forward<Q1>(lhs);
  }

  template<typename Q, typename Value>
    requires std::derived_from<std::remove_cvref_t<Q>, quantity> && (!Quantity<Value>) &&
             requires(rep a, const Value b) {
               {
                 a /= b
               } -> std::same_as<rep&>;
             }
  friend constexpr decltype(auto) operator/=(Q&& lhs, const Value& v)
  {
    MP_UNITS_EXPECTS_DEBUG(v != quantity_values<Value>::zero());
    lhs.numerical_value_is_an_implementation_detail_ /= v;
    return std::forward<Q>(lhs);
  }

  template<typename Q1, QuantityOf<dimensionless> Q2>
    requires std::derived_from<std::remove_cvref_t<Q1>, quantity> && (Q2::unit == ::mp_units::one) &&
             requires(rep a, const typename Q2::rep b) {
               {
                 a /= b
               } -> std::same_as<rep&>;
             }
  friend constexpr decltype(auto) operator/=(Q1&& lhs, const Q2& rhs)
  {
    MP_UNITS_EXPECTS_DEBUG(rhs != rhs.zero());
    lhs.numerical_value_is_an_implementation_detail_ /= rhs.numerical_value_is_an_implementation_detail_;
    return std::forward<Q1>(lhs);
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

  template<std::derived_from<quantity> Q, auto R2, typename Rep2>
    requires(!treat_as_floating_point<Rep>) && (!treat_as_floating_point<Rep2>) &&
            detail::CommonlyInvocableQuantities<std::modulus<>, quantity, quantity<R2, Rep2>>
  [[nodiscard]] friend constexpr Quantity auto operator%(const Q& lhs, const quantity<R2, Rep2>& rhs)
  {
    MP_UNITS_EXPECTS_DEBUG(rhs != rhs.zero());
    using ret = detail::common_quantity_for<std::modulus<>, quantity, quantity<R2, Rep2>>;
    const ret ret_lhs(lhs);
    const ret ret_rhs(rhs);
    return ::mp_units::quantity{ret_lhs.numerical_value_ref_in(ret::unit) % ret_rhs.numerical_value_ref_in(ret::unit),
                                ret::reference};
  }

  template<std::derived_from<quantity> Q, auto R2, typename Rep2>
    requires detail::InvocableQuantities<std::multiplies<>, quantity, quantity<R2, Rep2>>
  [[nodiscard]] friend constexpr Quantity auto operator*(const Q& lhs, const quantity<R2, Rep2>& rhs)
  {
    return ::mp_units::quantity{lhs.numerical_value_ref_in(unit) * rhs.numerical_value_ref_in(rhs.unit), R * R2};
  }

  template<std::derived_from<quantity> Q, typename Value>
    requires(!Quantity<Value>) &&
            (!Reference<Value>) && detail::InvokeResultOf<quantity_spec.character, std::multiplies<>, Rep, const Value&>
  [[nodiscard]] friend constexpr QuantityOf<quantity_spec> auto operator*(const Q& q, const Value& v)
  {
    return ::mp_units::quantity{q.numerical_value_ref_in(unit) * v, R};
  }

  template<typename Value, std::derived_from<quantity> Q>
    requires(!Quantity<Value>) &&
            (!Reference<Value>) && detail::InvokeResultOf<quantity_spec.character, std::multiplies<>, const Value&, Rep>
  [[nodiscard]] friend constexpr QuantityOf<quantity_spec> auto operator*(const Value& v, const Q& q)
  {
    return ::mp_units::quantity{v * q.numerical_value_ref_in(unit), R};
  }

  template<std::derived_from<quantity> Q, auto R2, typename Rep2>
    requires detail::InvocableQuantities<std::divides<>, quantity, quantity<R2, Rep2>>
  [[nodiscard]] friend constexpr Quantity auto operator/(const Q& lhs, const quantity<R2, Rep2>& rhs)
  {
    MP_UNITS_EXPECTS_DEBUG(rhs != rhs.zero());
    return ::mp_units::quantity{lhs.numerical_value_ref_in(unit) / rhs.numerical_value_ref_in(rhs.unit), R / R2};
  }

  template<std::derived_from<quantity> Q, typename Value>
    requires(!Quantity<Value>) &&
            (!Reference<Value>) && detail::InvokeResultOf<quantity_spec.character, std::divides<>, Rep, const Value&>
  [[nodiscard]] friend constexpr QuantityOf<quantity_spec> auto operator/(const Q& q, const Value& v)
  {
    MP_UNITS_EXPECTS_DEBUG(v != quantity_values<Value>::zero());
    return ::mp_units::quantity{q.numerical_value_ref_in(unit) / v, R};
  }

  template<typename Value, std::derived_from<quantity> Q>
    requires(!Quantity<Value>) &&
            (!Reference<Value>) && detail::InvokeResultOf<quantity_spec.character, std::divides<>, const Value&, Rep>
  [[nodiscard]] friend constexpr QuantityOf<inverse(quantity_spec)> auto operator/(const Value& v, const Q& q)
  {
    return ::mp_units::quantity{v / q.numerical_value_ref_in(unit), ::mp_units::one / R};
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
};

// CTAD
template<typename Value, Reference R>
  requires RepresentationOf<Value, get_quantity_spec(R{}).character>
quantity(Value v, R) -> quantity<R{}, Value>;

template<QuantityLike Q>
explicit(
  is_specialization_of<decltype(quantity_like_traits<Q>::to_numerical_value(std::declval<Q>())), convert_explicitly>)
  quantity(Q) -> quantity<quantity_like_traits<Q>::reference, typename quantity_like_traits<Q>::rep>;

MP_UNITS_EXPORT_END

}  // namespace mp_units

template<mp_units::Quantity Q1, mp_units::Quantity Q2>
  requires requires {
    {
      mp_units::common_reference(Q1::reference, Q2::reference)
    } -> mp_units::Reference;
    typename std::common_type_t<typename Q1::rep, typename Q2::rep>;
  }
struct std::common_type<Q1, Q2> {
public:
  using type = mp_units::quantity<mp_units::common_reference(Q1::reference, Q2::reference),
                                  std::common_type_t<typename Q1::rep, typename Q2::rep>>;
};
