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
#include <mp-units/framework/compare.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/quantity.h>
#include <mp-units/framework/quantity_point_concepts.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <compare>  // IWYU pragma: export
#endif
#endif

namespace mp_units {

namespace detail {

template<PointOrigin PO>
constexpr bool is_specialization_of_zeroth_point_origin = false;

template<PointOrigin PO>
[[nodiscard]] consteval bool is_zeroth_point_origin(PO)
{
  return is_specialization_of_zeroth_point_origin<PO>;
}

struct point_origin_interface {
  template<PointOrigin PO, typename FwdQ, QuantityOf<PO::quantity_spec> Q = std::remove_cvref_t<FwdQ>>
  [[nodiscard]] friend constexpr quantity_point<Q::reference, MP_UNITS_EXPRESSION_WORKAROUND(PO{}), typename Q::rep>
  operator+(PO, FwdQ&& q)
  {
    return quantity_point{std::forward<FwdQ>(q), PO{}};
  }

  template<Quantity FwdQ, PointOrigin PO, QuantityOf<PO::quantity_spec> Q = std::remove_cvref_t<FwdQ>>
  [[nodiscard]] friend constexpr quantity_point<Q::reference, MP_UNITS_EXPRESSION_WORKAROUND(PO{}), typename Q::rep>
  operator+(FwdQ&& q, PO po)
  {
    return po + std::forward<FwdQ>(q);
  }

  template<PointOrigin PO, Quantity Q>
    requires ReferenceOf<std::remove_const_t<decltype(Q::reference)>, PO::quantity_spec>
  [[nodiscard]] friend constexpr QuantityPoint auto operator-(PO po, const Q& q)
    requires requires { -q; }
  {
    return po + (-q);
  }

  template<PointOrigin PO1, detail::SameAbsolutePointOriginAs<PO1{}> PO2>
    requires QuantitySpecOf<std::remove_const_t<decltype(PO1::quantity_spec)>, PO2::quantity_spec> &&
             (detail::is_derived_from_specialization_of_relative_point_origin<PO1> ||
              detail::is_derived_from_specialization_of_relative_point_origin<PO2>)
  [[nodiscard]] friend constexpr Quantity auto operator-(PO1 po1, PO2 po2)
  {
    if constexpr (detail::is_derived_from_specialization_of_absolute_point_origin<PO1>) {
      return -(po2.quantity_point - po2.quantity_point.absolute_point_origin);
    } else if constexpr (detail::is_derived_from_specialization_of_absolute_point_origin<PO2>) {
      return po1.quantity_point - po1.quantity_point.absolute_point_origin;
    } else {
      return po1.quantity_point - po2.quantity_point;
    }
  }

  template<PointOrigin PO1, PointOrigin PO2>
  [[nodiscard]] friend consteval bool operator==(PO1 po1, PO2 po2)
  {
    if constexpr (detail::is_derived_from_specialization_of_absolute_point_origin<PO1> &&
                  detail::is_derived_from_specialization_of_absolute_point_origin<PO2>)
      return is_same_v<PO1, PO2> || (detail::is_zeroth_point_origin(po1) && detail::is_zeroth_point_origin(po2) &&
                                     interconvertible(po1.quantity_spec, po2.quantity_spec));
    else if constexpr (detail::is_derived_from_specialization_of_relative_point_origin<PO1> &&
                       detail::is_derived_from_specialization_of_relative_point_origin<PO2>)
      return PO1::quantity_point == PO2::quantity_point;
    else if constexpr (detail::is_derived_from_specialization_of_relative_point_origin<PO1>)
      return detail::same_absolute_point_origins(po1, po2) && is_eq_zero(PO1::quantity_point.quantity_from_zero());
    else if constexpr (detail::is_derived_from_specialization_of_relative_point_origin<PO2>)
      return detail::same_absolute_point_origins(po1, po2) && is_eq_zero(PO2::quantity_point.quantity_from_zero());
  }
};

}  // namespace detail

MP_UNITS_EXPORT template<QuantitySpec auto QS>
struct absolute_point_origin : detail::point_origin_interface {
  static constexpr QuantitySpec auto quantity_spec = QS;
};

MP_UNITS_EXPORT template<QuantityPoint auto QP>
struct relative_point_origin : detail::point_origin_interface {
  static constexpr QuantityPoint auto quantity_point = QP;
  static constexpr QuantitySpec auto quantity_spec = []() {
    // select the strongest of specs
    if constexpr (detail::QuantityKindSpec<std::remove_const_t<decltype(QP.quantity_spec)>>)
      return QP.point_origin.quantity_spec;
    else
      return QP.quantity_spec;
  }();
  static constexpr PointOrigin auto absolute_point_origin = QP.absolute_point_origin;
};

template<QuantitySpec auto QS>
struct zeroth_point_origin_ final : absolute_point_origin<QS> {};

MP_UNITS_EXPORT template<QuantitySpec auto QS>
constexpr zeroth_point_origin_<QS> zeroth_point_origin;

namespace detail {

template<auto QS>
constexpr bool is_specialization_of_zeroth_point_origin<zeroth_point_origin_<QS>> = true;

}  // namespace detail


MP_UNITS_EXPORT template<Reference R>
[[nodiscard]] consteval PointOriginFor<get_quantity_spec(R{})> auto default_point_origin(R)
{
  if constexpr (requires { get_unit(R{}).point_origin; })
    return get_unit(R{}).point_origin;
  else
    return zeroth_point_origin<get_quantity_spec(R{})>;
}

namespace detail {

template<PointOrigin PO>
[[nodiscard]] consteval PointOrigin auto get_absolute_point_origin(PO po)
{
  if constexpr (is_derived_from_specialization_of_absolute_point_origin<PO>)
    return po;
  else
    return po.quantity_point.absolute_point_origin;
}

}  // namespace detail

/**
 * @brief A quantity point
 *
 * An absolute quantity measured from an origin.
 *
 * @tparam R a reference of the quantity point providing all information about quantity properties
 * @tparam PO a type that represents the origin point from which the quantity point is measured from
 * @tparam Rep a type to be used to represent values of a quantity point
 */
MP_UNITS_EXPORT template<Reference auto R, PointOriginFor<get_quantity_spec(R)> auto PO = default_point_origin(R),
                         RepresentationOf<get_quantity_spec(R).character> Rep = double>
class quantity_point {
public:
  // member types and values
  static constexpr Reference auto reference = R;
  static constexpr QuantitySpec auto quantity_spec = get_quantity_spec(reference);
  static constexpr Dimension auto dimension = quantity_spec.dimension;
  static constexpr Unit auto unit = get_unit(reference);
  static constexpr PointOrigin auto absolute_point_origin = detail::get_absolute_point_origin(PO);
  static constexpr PointOrigin auto point_origin = PO;
  using rep = Rep;
  using quantity_type = quantity<reference, Rep>;

  quantity_type quantity_from_origin_is_an_implementation_detail_;  // needs to be public for a structural type

  // static member functions
  [[nodiscard]] static constexpr quantity_point min() noexcept
    requires requires { quantity_type::min(); }
  {
    return {quantity_type::min(), PO};
  }

  [[nodiscard]] static constexpr quantity_point max() noexcept
    requires requires { quantity_type::max(); }
  {
    return {quantity_type::max(), PO};
  }

  // construction, assignment, destruction
  quantity_point() = default;
  quantity_point(const quantity_point&) = default;
  quantity_point(quantity_point&&) = default;
  ~quantity_point() = default;

  template<typename FwdQ, QuantityOf<quantity_spec> Q = std::remove_cvref_t<FwdQ>>
    requires std::constructible_from<quantity_type, FwdQ> && (point_origin == default_point_origin(R)) &&
             (implicitly_convertible(Q::quantity_spec, quantity_spec))
  constexpr explicit quantity_point(FwdQ&& q) : quantity_from_origin_is_an_implementation_detail_(std::forward<FwdQ>(q))
  {
  }

  template<typename FwdQ, QuantityOf<quantity_spec> Q = std::remove_cvref_t<FwdQ>>
    requires std::constructible_from<quantity_type, FwdQ>
  constexpr quantity_point(FwdQ&& q, decltype(PO)) :
      quantity_from_origin_is_an_implementation_detail_(std::forward<FwdQ>(q))
  {
  }

  template<typename FwdQ, PointOrigin PO2, QuantityOf<PO2::quantity_spec> Q = std::remove_cvref_t<FwdQ>>
    requires std::constructible_from<quantity_type, FwdQ> && detail::SameAbsolutePointOriginAs<PO2, PO>
  constexpr quantity_point(FwdQ&& q, PO2) :
      quantity_point(
        quantity_point<std::remove_reference_t<Q>::reference, PO2{}, typename std::remove_reference_t<Q>::rep>{
          std::forward<FwdQ>(q), PO2{}})
  {
  }

  template<QuantityPointOf<absolute_point_origin> QP>
    requires std::constructible_from<quantity_type, typename QP::quantity_type>
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr explicit(!std::convertible_to<typename QP::quantity_type, quantity_type>) quantity_point(const QP& qp) :
      quantity_from_origin_is_an_implementation_detail_([&] {
        if constexpr (point_origin == QP::point_origin)
          return qp.quantity_ref_from(point_origin);
        else
          return qp - point_origin;
      }())
  {
  }

  template<QuantityPointLike QP>
    requires(quantity_point_like_traits<QP>::point_origin == point_origin) &&
            std::convertible_to<
              quantity<quantity_point_like_traits<QP>::reference, typename quantity_point_like_traits<QP>::rep>,
              quantity_type>
  constexpr explicit(
    is_specialization_of<decltype(quantity_point_like_traits<QP>::to_numerical_value(std::declval<QP>())),
                         convert_explicitly> ||
    !std::convertible_to<
      quantity<quantity_point_like_traits<QP>::reference, typename quantity_point_like_traits<QP>::rep>, quantity_type>)
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    quantity_point(const QP& qp) :
      quantity_from_origin_is_an_implementation_detail_(quantity_point_like_traits<QP>::to_numerical_value(qp).value,
                                                        get_unit(quantity_point_like_traits<QP>::reference))
  {
  }

  quantity_point& operator=(const quantity_point&) = default;
  quantity_point& operator=(quantity_point&&) = default;

  template<detail::SameAbsolutePointOriginAs<absolute_point_origin> NewPO>
  [[nodiscard]] constexpr QuantityPointOf<(NewPO{})> auto point_for(NewPO new_origin) const
  {
    if constexpr (is_same_v<NewPO, decltype(point_origin)>)
      return *this;
    else
      return ::mp_units::quantity_point{*this - new_origin, new_origin};
  }

  // data access
  template<PointOrigin PO2>
    requires(PO2{} == point_origin)
  [[nodiscard]] constexpr quantity_type& quantity_ref_from(PO2) & noexcept
  {
    return quantity_from_origin_is_an_implementation_detail_;
  }

  template<PointOrigin PO2>
    requires(PO2{} == point_origin)
  [[nodiscard]] constexpr const quantity_type& quantity_ref_from(PO2) const& noexcept
  {
    return quantity_from_origin_is_an_implementation_detail_;
  }

  template<PointOrigin PO2>
    requires(PO2{} == point_origin)
  constexpr const quantity_type&& quantity_ref_from(PO2) const&& noexcept
#if __cpp_deleted_function
    = delete("Can't form a reference to a temporary");
#else
    = delete;
#endif

  template<PointOrigin PO2>
    requires requires(quantity_point qp) { qp - PO2{}; }
  [[nodiscard]] constexpr Quantity auto quantity_from(PO2) const
  {
    return *this - PO2{};
  }

  template<QuantityPointOf<absolute_point_origin> QP>
  [[nodiscard]] constexpr Quantity auto quantity_from(const QP& qp) const
  {
    return *this - qp;
  }

  [[nodiscard]] constexpr Quantity auto quantity_from_zero() const
  {
    if constexpr (requires { unit.point_origin; }) {
      // original quantity point unit can be lost in the below operation
      const auto q = quantity_from(unit.point_origin);
      if constexpr (requires { q.in(unit); })
        // restore the unit if possible (non-truncating)
        return q.in(unit);
      else
        return q;
    } else
      return quantity_from(absolute_point_origin);
  }

  // unit conversions
  template<detail::UnitCompatibleWith<unit, quantity_spec> ToU>
    requires detail::QuantityConvertibleTo<quantity_type, quantity<detail::make_reference(quantity_spec, ToU{}), Rep>>
  [[nodiscard]] constexpr QuantityPointOf<quantity_spec> auto in(ToU) const
  {
    return ::mp_units::quantity_point{quantity_ref_from(point_origin).in(ToU{}), point_origin};
  }

  template<RepresentationOf<quantity_spec.character> ToRep>
    requires detail::QuantityConvertibleTo<quantity_type, quantity<reference, ToRep>>
  [[nodiscard]] constexpr QuantityPointOf<quantity_spec> auto in() const
  {
    return ::mp_units::quantity_point{quantity_ref_from(point_origin).template in<ToRep>(), point_origin};
  }

  template<RepresentationOf<quantity_spec.character> ToRep, detail::UnitCompatibleWith<unit, quantity_spec> ToU>
    requires detail::QuantityConvertibleTo<quantity_type, quantity<detail::make_reference(quantity_spec, ToU{}), ToRep>>
  [[nodiscard]] constexpr QuantityPointOf<quantity_spec> auto in(ToU) const
  {
    return ::mp_units::quantity_point{quantity_ref_from(point_origin).template in<ToRep>(ToU{}), point_origin};
  }

  template<detail::UnitCompatibleWith<unit, quantity_spec> ToU>
    requires requires(quantity_type q) { value_cast<ToU{}>(q); }
  [[nodiscard]] constexpr QuantityPointOf<quantity_spec> auto force_in(ToU) const
  {
    return ::mp_units::quantity_point{quantity_ref_from(point_origin).force_in(ToU{}), point_origin};
  }

  template<RepresentationOf<quantity_spec.character> ToRep>
    requires requires(quantity_type q) { value_cast<ToRep>(q); }
  [[nodiscard]] constexpr QuantityPointOf<quantity_spec> auto force_in() const
  {
    return ::mp_units::quantity_point{quantity_ref_from(point_origin).template force_in<ToRep>(), point_origin};
  }

  template<RepresentationOf<quantity_spec.character> ToRep, detail::UnitCompatibleWith<unit, quantity_spec> ToU>
    requires requires(quantity_type q) { value_cast<ToU{}, ToRep>(q); }
  [[nodiscard]] constexpr QuantityPointOf<quantity_spec> auto force_in(ToU) const
  {
    return ::mp_units::quantity_point{quantity_ref_from(point_origin).template force_in<ToRep>(ToU{}), point_origin};
  }

  // conversion operators
  template<typename QP_, QuantityPointLike QP = std::remove_cvref_t<QP_>>
    requires(point_origin == quantity_point_like_traits<QP>::point_origin) &&
            std::convertible_to<quantity_type, quantity<quantity_point_like_traits<QP>::reference,
                                                        typename quantity_point_like_traits<QP>::rep>>
  [[nodiscard]] explicit(
    is_specialization_of<
      decltype(quantity_point_like_traits<QP>::from_numerical_value(
        quantity_from_origin_is_an_implementation_detail_.numerical_value_is_an_implementation_detail_)),
      convert_explicitly> ||
    !std::convertible_to<quantity_type, quantity<quantity_point_like_traits<QP>::reference,
                                                 typename quantity_point_like_traits<QP>::rep>>) constexpr
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  operator QP_() const& noexcept(
    noexcept(quantity_point_like_traits<QP>::from_numerical_value(
      quantity_from_origin_is_an_implementation_detail_.numerical_value_is_an_implementation_detail_)) &&
    std::is_nothrow_copy_constructible_v<rep>)
  {
    return quantity_point_like_traits<QP>::from_numerical_value(
             quantity_from_origin_is_an_implementation_detail_.numerical_value_is_an_implementation_detail_)
      .value;
  }

  template<typename QP_, QuantityPointLike QP = std::remove_cvref_t<QP_>>
    requires(point_origin == quantity_point_like_traits<QP>::point_origin) &&
            std::convertible_to<quantity_type, quantity<quantity_point_like_traits<QP>::reference,
                                                        typename quantity_point_like_traits<QP>::rep>>
  [[nodiscard]] explicit(
    is_specialization_of<
      decltype(quantity_point_like_traits<QP>::from_numerical_value(
        quantity_from_origin_is_an_implementation_detail_.numerical_value_is_an_implementation_detail_)),
      convert_explicitly> ||
    !std::convertible_to<quantity_type, quantity<quantity_point_like_traits<QP>::reference,
                                                 typename quantity_point_like_traits<QP>::rep>>) constexpr
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  operator QP_() && noexcept(
    noexcept(quantity_point_like_traits<QP>::from_numerical_value(
      quantity_from_origin_is_an_implementation_detail_.numerical_value_is_an_implementation_detail_)) &&
    std::is_nothrow_move_constructible_v<rep>)
  {
    return quantity_point_like_traits<QP>::from_numerical_value(
             std::move(quantity_from_origin_is_an_implementation_detail_).numerical_value_is_an_implementation_detail_)
      .value;
  }

  // member unary operators
  template<typename FwdQP, std::derived_from<quantity_point> QP = std::remove_cvref_t<FwdQP>>
  friend constexpr decltype(auto) operator++(FwdQP&& qp)
    requires requires { ++qp.quantity_from_origin_is_an_implementation_detail_; }
  {
    ++qp.quantity_from_origin_is_an_implementation_detail_;
    return std::forward<FwdQP>(qp);
  }

  [[nodiscard]] constexpr quantity_point operator++(int)
    requires requires { quantity_from_origin_is_an_implementation_detail_++; }
  {
    return {quantity_from_origin_is_an_implementation_detail_++, PO};
  }

  template<typename FwdQP, std::derived_from<quantity_point> QP = std::remove_cvref_t<FwdQP>>
  friend constexpr decltype(auto) operator--(FwdQP&& qp)
    requires requires { --qp.quantity_from_origin_is_an_implementation_detail_; }
  {
    --qp.quantity_from_origin_is_an_implementation_detail_;
    return std::forward<FwdQP>(qp);
  }

  [[nodiscard]] constexpr quantity_point operator--(int)
    requires requires { quantity_from_origin_is_an_implementation_detail_--; }
  {
    return {quantity_from_origin_is_an_implementation_detail_--, PO};
  }

  // compound assignment operators
  template<typename FwdQP, std::derived_from<quantity_point> QP = std::remove_cvref_t<FwdQP>>
    requires requires(quantity_type q) { quantity_from_origin_is_an_implementation_detail_ += q; }
  friend constexpr decltype(auto) operator+=(FwdQP&& qp, const quantity_type& q)
  {
    qp.quantity_from_origin_is_an_implementation_detail_ += q;
    return std::forward<FwdQP>(qp);
  }

  template<typename FwdQP, std::derived_from<quantity_point> QP = std::remove_cvref_t<FwdQP>>
    requires requires(quantity_type q) { quantity_from_origin_is_an_implementation_detail_ -= q; }
  friend constexpr decltype(auto) operator-=(FwdQP&& qp, const quantity_type& q)
  {
    qp.quantity_from_origin_is_an_implementation_detail_ -= q;
    return std::forward<FwdQP>(qp);
  }

  // binary operators on quantity points
  template<std::derived_from<quantity_point> QP, auto R2, typename Rep2>
  // TODO simplify when gcc catches up
    requires ReferenceOf<MP_UNITS_REMOVE_CONST(decltype(R2)), PO.quantity_spec>
  [[nodiscard]] friend constexpr QuantityPoint auto operator+(const QP& qp, const quantity<R2, Rep2>& q)
    requires requires { qp.quantity_ref_from(PO) + q; }
  {
    if constexpr (detail::is_zeroth_point_origin(PO))
      return ::mp_units::quantity_point{qp.quantity_ref_from(PO) + q};
    else
      return ::mp_units::quantity_point{qp.quantity_ref_from(PO) + q, PO};
  }

  template<auto R1, typename Rep1, std::derived_from<quantity_point> QP>
  // TODO simplify when gcc catches up
    requires ReferenceOf<MP_UNITS_REMOVE_CONST(decltype(R1)), PO.quantity_spec>
  [[nodiscard]] friend constexpr QuantityPoint auto operator+(const quantity<R1, Rep1>& q, const QP& qp)
    requires requires { q + qp.quantity_ref_from(PO); }
  {
    return qp + q;
  }

  template<std::derived_from<quantity_point> QP, auto R2, typename Rep2>
  // TODO simplify when gcc catches up
    requires ReferenceOf<MP_UNITS_REMOVE_CONST(decltype(R2)), PO.quantity_spec>
  [[nodiscard]] friend constexpr QuantityPoint auto operator-(const QP& qp, const quantity<R2, Rep2>& q)
    requires requires { qp.quantity_ref_from(PO) - q; }
  {
    if constexpr (detail::is_zeroth_point_origin(PO))
      return ::mp_units::quantity_point{qp.quantity_ref_from(PO) - q};
    else
      return ::mp_units::quantity_point{qp.quantity_ref_from(PO) - q, PO};
  }

  template<std::derived_from<quantity_point> QP, QuantityPointOf<absolute_point_origin> QP2>
  [[nodiscard]] friend constexpr Quantity auto operator-(const QP& lhs, const QP2& rhs)
    // TODO consider constraining it for both branches
    requires requires { lhs.quantity_ref_from(point_origin) - rhs.quantity_ref_from(QP2::point_origin); }
  {
    if constexpr (point_origin == QP2::point_origin)
      return lhs.quantity_ref_from(point_origin) - rhs.quantity_ref_from(QP2::point_origin);
    else
      return lhs.quantity_ref_from(point_origin) - rhs.quantity_ref_from(QP2::point_origin) +
             (lhs.point_origin - rhs.point_origin);
  }

  template<std::derived_from<quantity_point> QP, PointOrigin PO2>
    requires QuantityPointOf<quantity_point, PO2{}> &&
             ReferenceOf<std::remove_const_t<decltype(reference)>, PO2::quantity_spec>
  [[nodiscard]] friend constexpr Quantity auto operator-(const QP& qp, PO2 po)
  {
    if constexpr (point_origin == po)
      return qp.quantity_ref_from(point_origin);
    else if constexpr (detail::is_derived_from_specialization_of_absolute_point_origin<PO2>) {
      if constexpr (point_origin == absolute_point_origin)
        return qp.quantity_ref_from(point_origin);
      else
        return qp.quantity_ref_from(point_origin) + (qp.point_origin - qp.absolute_point_origin);
    } else {
      if constexpr (point_origin == po.quantity_point.point_origin)
        return qp.quantity_ref_from(point_origin) - po.quantity_point.quantity_ref_from(po.quantity_point.point_origin);
      else
        return qp.quantity_ref_from(point_origin) -
               po.quantity_point.quantity_ref_from(po.quantity_point.point_origin) +
               (qp.point_origin - po.quantity_point.point_origin);
    }
  }

  template<PointOrigin PO1, std::derived_from<quantity_point> QP>
    requires QuantityPointOf<quantity_point, PO1{}> &&
             ReferenceOf<std::remove_const_t<decltype(reference)>, PO1::quantity_spec>
  [[nodiscard]] friend constexpr Quantity auto operator-(PO1 po, const QP& qp)
  {
    return -(qp - po);
  }

  template<std::derived_from<quantity_point> QP, QuantityPointOf<absolute_point_origin> QP2>
    requires std::equality_comparable_with<quantity_type, typename QP2::quantity_type>
  [[nodiscard]] friend constexpr bool operator==(const QP& lhs, const QP2& rhs)
  {
    if constexpr (point_origin == QP2::point_origin)
      return lhs.quantity_ref_from(point_origin) == rhs.quantity_ref_from(QP2::point_origin);
    else
      return lhs - lhs.absolute_point_origin == rhs - rhs.absolute_point_origin;
  }

  template<std::derived_from<quantity_point> QP, QuantityPointOf<absolute_point_origin> QP2>
    requires std::three_way_comparable_with<quantity_type, typename QP2::quantity_type>
  [[nodiscard]] friend constexpr auto operator<=>(const QP& lhs, const QP2& rhs)
  {
    if constexpr (point_origin == QP2::point_origin)
      return lhs.quantity_ref_from(point_origin) <=> rhs.quantity_ref_from(QP2::point_origin);
    else
      return lhs - lhs.absolute_point_origin <=> rhs - rhs.absolute_point_origin;
  }
};

// CTAD
template<Quantity Q>
quantity_point(Q q) -> quantity_point<Q::reference, default_point_origin(Q::reference), typename Q::rep>;

template<Quantity Q, PointOriginFor<Q::quantity_spec> PO>
quantity_point(Q q, PO) -> quantity_point<Q::reference, PO{}, typename Q::rep>;

template<QuantityPointLike QP>
explicit(is_specialization_of<decltype(quantity_point_like_traits<QP>::to_numerical_value(std::declval<QP>())),
                              convert_explicitly>) quantity_point(QP)
  -> quantity_point<quantity_point_like_traits<QP>::reference, quantity_point_like_traits<QP>::point_origin,
                    typename quantity_point_like_traits<QP>::rep>;

}  // namespace mp_units
