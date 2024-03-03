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

#include <mp-units/bits/module_macros.h>
#include <mp-units/bits/quantity_point_concepts.h>
#include <mp-units/compare.h>
#include <mp-units/customization_points.h>
#include <mp-units/quantity.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <compare>
#endif

namespace mp_units {

MP_UNITS_EXPORT template<typename Derived, QuantitySpec auto QS>
struct absolute_point_origin {
  static constexpr QuantitySpec auto quantity_spec = QS;
  using _type_ = absolute_point_origin;
};

MP_UNITS_EXPORT template<QuantityPoint auto QP>
struct relative_point_origin {
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
struct zeroth_point_origin_ : absolute_point_origin<zeroth_point_origin_<QS>, QS> {};

MP_UNITS_EXPORT template<QuantitySpec auto QS>
inline constexpr zeroth_point_origin_<QS> zeroth_point_origin;

namespace detail {

template<PointOrigin PO>
inline constexpr bool is_specialization_of_zeroth_point_origin = false;

template<auto QS>
inline constexpr bool is_specialization_of_zeroth_point_origin<zeroth_point_origin_<QS>> = true;

template<PointOrigin PO>
[[nodiscard]] consteval bool is_zeroth_point_origin(PO)
{
  return is_specialization_of_zeroth_point_origin<PO>;
}

}  // namespace detail

MP_UNITS_EXPORT template<PointOrigin PO1, PointOrigin PO2>
[[nodiscard]] consteval bool operator==(PO1 po1, PO2 po2)
{
  if constexpr (detail::AbsolutePointOrigin<PO1> && detail::AbsolutePointOrigin<PO2>)
    return is_same_v<typename PO1::_type_, typename PO2::_type_> ||
           (detail::is_zeroth_point_origin(po1) && detail::is_zeroth_point_origin(po2) &&
            interconvertible(po1.quantity_spec, po2.quantity_spec));
  else if constexpr (detail::RelativePointOrigin<PO1> && detail::RelativePointOrigin<PO2>)
    return PO1::quantity_point == PO2::quantity_point;
  else if constexpr (detail::RelativePointOrigin<PO1>)
    return detail::same_absolute_point_origins(po1, po2) && is_eq_zero(PO1::quantity_point.quantity_from_zero());
  else if constexpr (detail::RelativePointOrigin<PO2>)
    return detail::same_absolute_point_origins(po1, po2) && is_eq_zero(PO2::quantity_point.quantity_from_zero());
}

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
  if constexpr (AbsolutePointOrigin<PO>)
    return po;
  else
    return po.quantity_point.absolute_point_origin;
}

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

/**
 * @brief A quantity point
 *
 * An absolute quantity measured from an origin.
 *
 * @tparam R a reference of the quantity point providing all information about quantity properties
 * @tparam PO a type that represents the origin point from which the quantity point is measured from
 * @tparam Rep a type to be used to represent values of a quantity point
 */
template<Reference auto R, PointOriginFor<get_quantity_spec(R)> auto PO = default_point_origin(R),
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

  template<typename Q>
    requires QuantityOf<std::remove_cvref_t<Q>, get_quantity_spec(R)> && std::constructible_from<quantity_type, Q> &&
             (point_origin == default_point_origin(R)) && (implicitly_convertible(Q::quantity_spec, quantity_spec))
  constexpr explicit quantity_point(Q&& q) : quantity_from_origin_is_an_implementation_detail_(std::forward<Q>(q))
  {
  }

  template<typename Q>
    requires QuantityOf<std::remove_cvref_t<Q>, get_quantity_spec(R)> && std::constructible_from<quantity_type, Q>
  constexpr quantity_point(Q&& q, std::remove_const_t<decltype(PO)>) :
      quantity_from_origin_is_an_implementation_detail_(std::forward<Q>(q))
  {
  }

  template<typename Q, PointOrigin PO2>
    requires Quantity<std::remove_cvref_t<Q>> && std::constructible_from<quantity_type, Q> &&
             ReferenceOf<std::remove_const_t<decltype(std::remove_reference_t<Q>::reference)>, PO2::quantity_spec> &&
             detail::SameAbsolutePointOriginAs<PO2, PO>
  constexpr quantity_point(Q&& q, PO2) :
      quantity_point(
        quantity_point<std::remove_reference_t<Q>::reference, PO2{}, typename std::remove_reference_t<Q>::rep>{
          std::forward<Q>(q), PO2{}})
  {
  }

  template<QuantityPointOf<absolute_point_origin> QP>
    requires std::constructible_from<quantity_type, typename QP::quantity_type>
  // TODO add perfect forwarding
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
    is_specialization_of<decltype(quantity_point_like_traits<QP>::to_quantity(std::declval<QP>())),
                         convert_explicitly> ||
    !std::convertible_to<
      quantity<quantity_point_like_traits<QP>::reference, typename quantity_point_like_traits<QP>::rep>, quantity_type>)
    quantity_point(const QP& qp) :
      quantity_from_origin_is_an_implementation_detail_(quantity_point_like_traits<QP>::to_quantity(qp).value)
  {
  }

  quantity_point& operator=(const quantity_point&) = default;
  quantity_point& operator=(quantity_point&&) = default;

  template<detail::SameAbsolutePointOriginAs<absolute_point_origin> NewPO>
  [[nodiscard]] constexpr MP_UNITS_CONSTRAINED_AUTO_WORKAROUND(QuantityPointOf<NewPO{}>) auto point_for(
    NewPO new_origin) const
  {
    if constexpr (is_same_v<NewPO, std::remove_const_t<decltype(point_origin)>>)
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
  constexpr const quantity_type&& quantity_ref_from(PO2) const&& noexcept = delete;

  template<PointOrigin PO2>
    requires requires { quantity_point{} - PO2{}; }
  [[nodiscard]] constexpr Quantity auto quantity_from(PO2) const
  {
    return *this - PO2{};
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
  template<UnitCompatibleWith<unit, quantity_spec> U>
    requires detail::QuantityConvertibleTo<quantity_type, quantity<detail::make_reference(quantity_spec, U{}), Rep>>
  [[nodiscard]] constexpr QuantityPointOf<quantity_spec> auto in(U) const
  {
    return ::mp_units::quantity_point{quantity_ref_from(PO).in(U{}), PO};
  }

  template<UnitCompatibleWith<unit, quantity_spec> U>
    requires requires(quantity_type q) { value_cast<U{}>(q); }
  [[nodiscard]] constexpr QuantityPointOf<quantity_spec> auto force_in(U) const
  {
    return ::mp_units::quantity_point{quantity_ref_from(PO).force_in(U{}), PO};
  }

  // conversion operators
  template<typename QP_, QuantityPointLike QP = std::remove_cvref_t<QP_>>
    requires(point_origin == quantity_point_like_traits<QP>::point_origin) &&
            std::convertible_to<quantity_type, quantity<quantity_point_like_traits<QP>::reference,
                                                        typename quantity_point_like_traits<QP>::rep>>
  [[nodiscard]] explicit(
    is_specialization_of<
      decltype(quantity_point_like_traits<QP>::from_quantity(quantity_from_origin_is_an_implementation_detail_)),
      convert_explicitly> ||
    !std::convertible_to<quantity_type, quantity<quantity_point_like_traits<QP>::reference,
                                                 typename quantity_point_like_traits<QP>::rep>>) constexpr
  operator QP_() const& noexcept(
    noexcept(quantity_point_like_traits<QP>::from_quantity(quantity_from_origin_is_an_implementation_detail_)) &&
    std::is_nothrow_copy_constructible_v<rep>)
  {
    return quantity_point_like_traits<QP>::from_quantity(quantity_from_origin_is_an_implementation_detail_).value;
  }

  template<typename QP_, QuantityPointLike QP = std::remove_cvref_t<QP_>>
    requires(point_origin == quantity_point_like_traits<QP>::point_origin) &&
            std::convertible_to<quantity_type, quantity<quantity_point_like_traits<QP>::reference,
                                                        typename quantity_point_like_traits<QP>::rep>>
  [[nodiscard]] explicit(
    is_specialization_of<
      decltype(quantity_point_like_traits<QP>::from_quantity(quantity_from_origin_is_an_implementation_detail_)),
      convert_explicitly> ||
    !std::convertible_to<quantity_type, quantity<quantity_point_like_traits<QP>::reference,
                                                 typename quantity_point_like_traits<QP>::rep>>) constexpr
  operator QP_() && noexcept(
    noexcept(quantity_point_like_traits<QP>::from_quantity(quantity_from_origin_is_an_implementation_detail_)) &&
    std::is_nothrow_move_constructible_v<rep>)
  {
    return quantity_point_like_traits<QP>::from_quantity(std::move(quantity_from_origin_is_an_implementation_detail_))
      .value;
  }

  // member unary operators
  template<typename QP>
  friend constexpr decltype(auto) operator++(QP&& qp)
    requires std::derived_from<std::remove_cvref_t<QP>, quantity_point> &&
             requires { ++qp.quantity_from_origin_is_an_implementation_detail_; }
  {
    ++qp.quantity_from_origin_is_an_implementation_detail_;
    return std::forward<QP>(qp);
  }

  [[nodiscard]] constexpr quantity_point operator++(int)
    requires requires { quantity_from_origin_is_an_implementation_detail_++; }
  {
    return {quantity_from_origin_is_an_implementation_detail_++, PO};
  }

  template<typename QP>
  friend constexpr decltype(auto) operator--(QP&& qp)
    requires std::derived_from<std::remove_cvref_t<QP>, quantity_point> &&
             requires { --qp.quantity_from_origin_is_an_implementation_detail_; }
  {
    --qp.quantity_from_origin_is_an_implementation_detail_;
    return std::forward<QP>(qp);
  }

  [[nodiscard]] constexpr quantity_point operator--(int)
    requires requires { quantity_from_origin_is_an_implementation_detail_--; }
  {
    return {quantity_from_origin_is_an_implementation_detail_--, PO};
  }

  // compound assignment operators
  template<typename QP>
    requires std::derived_from<std::remove_cvref_t<QP>, quantity_point> &&
             requires(quantity_type q) { quantity_from_origin_is_an_implementation_detail_ += q; }
  friend constexpr decltype(auto) operator+=(QP&& qp, const quantity_type& q)
  {
    qp.quantity_from_origin_is_an_implementation_detail_ += q;
    return std::forward<QP>(qp);
  }

  template<typename QP>
    requires std::derived_from<std::remove_cvref_t<QP>, quantity_point> &&
             requires(quantity_type q) { quantity_from_origin_is_an_implementation_detail_ -= q; }
  friend constexpr decltype(auto) operator-=(QP&& qp, const quantity_type& q)
  {
    qp.quantity_from_origin_is_an_implementation_detail_ -= q;
    return std::forward<QP>(qp);
  }
};

// CTAD
template<Quantity Q>
quantity_point(Q q) -> quantity_point<Q::reference, default_point_origin(Q::reference), typename Q::rep>;

template<Quantity Q, PointOriginFor<Q::quantity_spec> PO>
quantity_point(Q q, PO) -> quantity_point<Q::reference, PO{}, typename Q::rep>;

template<QuantityPointLike QP>
explicit(
  is_specialization_of<decltype(quantity_point_like_traits<QP>::to_quantity(std::declval<QP>())), convert_explicitly>)
  quantity_point(QP)
    -> quantity_point<quantity_point_like_traits<QP>::reference, quantity_point_like_traits<QP>::point_origin,
                      typename quantity_point_like_traits<QP>::rep>;

template<auto R1, auto PO1, typename Rep1, auto R2, typename Rep2>
// TODO simplify when gcc catches up
  requires ReferenceOf<std::remove_const_t<decltype(R2)>, PO1.quantity_spec>
[[nodiscard]] constexpr QuantityPoint auto operator+(const quantity_point<R1, PO1, Rep1>& qp,
                                                     const quantity<R2, Rep2>& q)
  requires requires { qp.quantity_ref_from(PO1) + q; }
{
  if constexpr (detail::is_zeroth_point_origin(PO1))
    return quantity_point{qp.quantity_ref_from(PO1) + q};
  else
    return quantity_point{qp.quantity_ref_from(PO1) + q, PO1};
}

template<auto R1, typename Rep1, auto R2, auto PO2, typename Rep2>
// TODO simplify when gcc catches up
  requires ReferenceOf<std::remove_const_t<decltype(R1)>, PO2.quantity_spec>
[[nodiscard]] constexpr QuantityPoint auto operator+(const quantity<R1, Rep1>& q,
                                                     const quantity_point<R2, PO2, Rep2>& qp)
  requires requires { q + qp.quantity_ref_from(PO2); }
{
  return qp + q;
}

template<PointOrigin PO, Quantity Q>
  requires ReferenceOf<std::remove_const_t<decltype(Q::reference)>, PO::quantity_spec>
[[nodiscard]] constexpr quantity_point<Q::reference, PO{}, typename Q::rep> operator+(PO, Q&& q)
{
  return quantity_point{std::forward<Q>(q), PO{}};
}

template<Quantity Q, PointOrigin PO>
  requires ReferenceOf<std::remove_const_t<decltype(Q::reference)>, PO::quantity_spec>
[[nodiscard]] constexpr quantity_point<Q::reference, PO{}, typename Q::rep> operator+(Q&& q, PO po)
{
  return po + std::forward<Q>(q);
}

template<auto R1, auto PO1, typename Rep1, auto R2, typename Rep2>
// TODO simplify when gcc catches up
  requires ReferenceOf<std::remove_const_t<decltype(R2)>, PO1.quantity_spec>
[[nodiscard]] constexpr QuantityPoint auto operator-(const quantity_point<R1, PO1, Rep1>& qp,
                                                     const quantity<R2, Rep2>& q)
  requires requires { qp.quantity_ref_from(PO1) - q; }
{
  if constexpr (detail::is_zeroth_point_origin(PO1))
    return quantity_point{qp.quantity_ref_from(PO1) - q};
  else
    return quantity_point{qp.quantity_ref_from(PO1) - q, PO1};
}

template<PointOrigin PO, Quantity Q>
  requires ReferenceOf<std::remove_const_t<decltype(Q::reference)>, PO::quantity_spec>
[[nodiscard]] constexpr QuantityPoint auto operator-(PO po, const Q& q)
  requires requires { -q; }
{
  return po + (-q);
}

template<QuantityPoint QP1, QuantityPointOf<QP1::absolute_point_origin> QP2>
[[nodiscard]] constexpr Quantity auto operator-(const QP1& lhs, const QP2& rhs)
  // TODO consider constraining it for both branches
  requires requires { lhs.quantity_ref_from(QP1::point_origin) - rhs.quantity_ref_from(QP2::point_origin); }
{
  if constexpr (QP1::point_origin == QP2::point_origin)
    return lhs.quantity_ref_from(QP1::point_origin) - rhs.quantity_ref_from(QP2::point_origin);
  else
    return lhs.quantity_ref_from(QP1::point_origin) - rhs.quantity_ref_from(QP2::point_origin) +
           (lhs.point_origin - rhs.point_origin);
}

template<PointOrigin PO, QuantityPointOf<PO{}> QP>
  requires ReferenceOf<std::remove_const_t<decltype(QP::reference)>, PO::quantity_spec>
[[nodiscard]] constexpr Quantity auto operator-(const QP& qp, PO po)
{
  if constexpr (QP::point_origin == po)
    return qp.quantity_ref_from(QP::point_origin);
  else if constexpr (detail::is_derived_from_specialization_of_absolute_point_origin<PO>) {
    if constexpr (QP::point_origin == QP::absolute_point_origin)
      return qp.quantity_ref_from(QP::point_origin);
    else
      return qp.quantity_ref_from(QP::point_origin) + (qp.point_origin - qp.absolute_point_origin);
  } else {
    if constexpr (QP::point_origin == po.quantity_point.point_origin)
      return qp.quantity_ref_from(QP::point_origin) -
             po.quantity_point.quantity_ref_from(po.quantity_point.point_origin);
    else
      return qp.quantity_ref_from(QP::point_origin) -
             po.quantity_point.quantity_ref_from(po.quantity_point.point_origin) +
             (qp.point_origin - po.quantity_point.point_origin);
  }
}

template<PointOrigin PO, QuantityPointOf<PO{}> QP>
  requires ReferenceOf<std::remove_const_t<decltype(QP::reference)>, PO::quantity_spec>
[[nodiscard]] constexpr Quantity auto operator-(PO po, const QP& qp)
{
  return -(qp - po);
}

template<PointOrigin PO1, detail::SameAbsolutePointOriginAs<PO1{}> PO2>
  requires QuantitySpecOf<std::remove_const_t<decltype(PO1::quantity_spec)>, PO2::quantity_spec> &&
           (detail::is_derived_from_specialization_of_relative_point_origin<PO1> ||
            detail::is_derived_from_specialization_of_relative_point_origin<PO2>)
[[nodiscard]] constexpr Quantity auto operator-(PO1 po1, PO2 po2)
{
  if constexpr (detail::is_derived_from_specialization_of_absolute_point_origin<PO1>) {
    return -(po2.quantity_point - po2.quantity_point.absolute_point_origin);
  } else if constexpr (detail::is_derived_from_specialization_of_absolute_point_origin<PO2>) {
    return po1.quantity_point - po1.quantity_point.absolute_point_origin;
  } else {
    return po1.quantity_point - po2.quantity_point;
  }
}

template<QuantityPoint QP1, QuantityPointOf<QP1::absolute_point_origin> QP2>
  requires std::three_way_comparable_with<typename QP1::quantity_type, typename QP2::quantity_type>
[[nodiscard]] constexpr auto operator<=>(const QP1& lhs, const QP2& rhs)
{
  if constexpr (QP1::point_origin == QP2::point_origin)
    return lhs.quantity_ref_from(QP1::point_origin) <=> rhs.quantity_ref_from(QP2::point_origin);
  else
    return lhs - lhs.absolute_point_origin <=> rhs - rhs.absolute_point_origin;
}

template<QuantityPoint QP1, QuantityPointOf<QP1::absolute_point_origin> QP2>
  requires std::equality_comparable_with<typename QP1::quantity_type, typename QP2::quantity_type>
[[nodiscard]] constexpr bool operator==(const QP1& lhs, const QP2& rhs)
{
  if constexpr (QP1::point_origin == QP2::point_origin)
    return lhs.quantity_ref_from(QP1::point_origin) == rhs.quantity_ref_from(QP2::point_origin);
  else
    return lhs - lhs.absolute_point_origin == rhs - rhs.absolute_point_origin;
}

MP_UNITS_EXPORT_END

}  // namespace mp_units
