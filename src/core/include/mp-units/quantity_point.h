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

#include <mp-units/bits/quantity_point_concepts.h>
#include <mp-units/customization_points.h>
#include <mp-units/quantity.h>
#include <compare>

namespace mp_units {

template<QuantitySpec auto QS>
struct absolute_point_origin {
  static constexpr QuantitySpec auto quantity_spec = QS;
};

template<QuantityPoint auto QP>
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

namespace detail {

[[nodiscard]] consteval PointOrigin auto get_absolute_point_origin(PointOrigin auto po)
{
  if constexpr (requires { po.quantity_point.absolute_point_origin; })
    return po.quantity_point.absolute_point_origin;
  else
    return po;
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
template<Reference auto R, PointOriginFor<get_quantity_spec(R)> auto PO,
         RepresentationOf<get_quantity_spec(R).character> Rep = double>
  requires vector_space<Rep>
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

  quantity_type quantity_from_origin_;  // needs to be public for a structural type

  // static member functions
  [[nodiscard]] static constexpr quantity_point zero() noexcept
    requires requires { quantity_type::zero(); }
  {
    return quantity_point(quantity_type::zero());
  }

  [[nodiscard]] static constexpr quantity_point min() noexcept
    requires requires { quantity_type::min(); }
  {
    return quantity_point{quantity_type::min()};
  }

  [[nodiscard]] static constexpr quantity_point max() noexcept
    requires requires { quantity_type::max(); }
  {
    return quantity_point{quantity_type::max()};
  }

  // construction, assignment, destruction
  quantity_point() = default;
  quantity_point(const quantity_point&) = default;
  quantity_point(quantity_point&&) = default;

  template<QuantityPointOf<absolute_point_origin> QP>
    requires std::constructible_from<quantity_type, typename QP::quantity_type>
  // TODO add perfect forwarding
  constexpr explicit(!std::convertible_to<typename QP::quantity_type, quantity_type>) quantity_point(const QP& qp) :
      quantity_from_origin_([&] {
        if constexpr (is_same_v<std::remove_const_t<decltype(point_origin)>,
                                std::remove_const_t<decltype(QP::point_origin)>>)
          return qp.quantity_ref_from(point_origin);
        else
          return qp - point_origin;
      }())
  {
  }

  template<QuantityPointLike QP>
    requires std::same_as<std::remove_const_t<decltype(quantity_point_like_traits<QP>::point_origin)>,
                          std::remove_const_t<decltype(point_origin)>> &&
             std::convertible_to<
               quantity<quantity_point_like_traits<QP>::reference, typename quantity_point_like_traits<QP>::rep>,
               quantity_type>
  constexpr explicit(
    is_specialization_of<decltype(quantity_point_like_traits<QP>::to_quantity(std::declval<QP>())),
                         convert_explicitly> ||
    !std::convertible_to<
      quantity<quantity_point_like_traits<QP>::reference, typename quantity_point_like_traits<QP>::rep>, quantity_type>)
    quantity_point(const QP& qp) :
      quantity_from_origin_(quantity_point_like_traits<QP>::to_quantity(qp).value)
  {
  }

  quantity_point& operator=(const quantity_point&) = default;
  quantity_point& operator=(quantity_point&&) = default;

  template<PointOriginFor<quantity_spec> NewPO>
  [[nodiscard]] constexpr QuantityPointOf<NewPO{}> auto point_for(NewPO new_origin) const
  {
    if constexpr (is_same_v<NewPO, std::remove_const_t<decltype(point_origin)>>)
      return *this;
    else
      return make_quantity_point<new_origin>(*this - new_origin);
  }

  // data access
  template<std::same_as<std::remove_const_t<decltype(PO)>> PO2>
  [[nodiscard]] constexpr quantity_type& quantity_ref_from(PO2) & noexcept
  {
    return quantity_from_origin_;
  }

  template<std::same_as<std::remove_const_t<decltype(PO)>> PO2>
  [[nodiscard]] constexpr const quantity_type& quantity_ref_from(PO2) const& noexcept
  {
    return quantity_from_origin_;
  }

  template<std::same_as<std::remove_const_t<decltype(PO)>> PO2>
  constexpr const quantity_type&& quantity_ref_from(PO2) const&& noexcept = delete;

  template<PointOrigin PO2>
    requires requires { quantity_point{} - PO2{}; }
  [[nodiscard]] constexpr Quantity auto quantity_from(PO2) const
  {
    return *this - PO2{};
  }

  // unit conversions
  template<UnitCompatibleWith<unit, quantity_spec> U>
    requires detail::QuantityConvertibleTo<quantity_type, quantity<detail::make_reference(quantity_spec, U{}), Rep>>
  [[nodiscard]] constexpr QuantityPoint auto in(U) const
  {
    return make_quantity_point<PO>(quantity_ref_from(PO).in(U{}));
  }

  template<UnitCompatibleWith<unit, quantity_spec> U>
    requires requires(quantity_type q) { value_cast<U{}>(q); }
  [[nodiscard]] constexpr QuantityPoint auto force_in(U) const
  {
    return make_quantity_point<PO>(quantity_ref_from(PO).force_in(U{}));
  }

  // conversion operators
  template<typename QP_, QuantityPointLike QP = std::remove_cvref_t<QP_>>
    requires std::same_as<std::remove_const_t<decltype(point_origin)>,
                          std::remove_const_t<decltype(quantity_point_like_traits<QP>::point_origin)>> &&
             std::convertible_to<quantity_type, quantity<quantity_point_like_traits<QP>::reference,
                                                         typename quantity_point_like_traits<QP>::rep>>
  [[nodiscard]] explicit(
    is_specialization_of<decltype(quantity_point_like_traits<QP>::from_quantity(quantity_from_origin_)),
                         convert_explicitly> ||
    !std::convertible_to<quantity_type, quantity<quantity_point_like_traits<QP>::reference,
                                                 typename quantity_point_like_traits<QP>::rep>>) constexpr
  operator QP_() const& noexcept(noexcept(quantity_point_like_traits<QP>::from_quantity(quantity_from_origin_)) &&
                                 std::is_nothrow_copy_constructible_v<rep>)
  {
    return quantity_point_like_traits<QP>::from_quantity(quantity_from_origin_).value;
  }

  template<typename QP_, QuantityPointLike QP = std::remove_cvref_t<QP_>>
    requires std::same_as<std::remove_const_t<decltype(point_origin)>,
                          std::remove_const_t<decltype(quantity_point_like_traits<QP>::point_origin)>> &&
             std::convertible_to<quantity_type, quantity<quantity_point_like_traits<QP>::reference,
                                                         typename quantity_point_like_traits<QP>::rep>>
  [[nodiscard]] explicit(
    is_specialization_of<decltype(quantity_point_like_traits<QP>::from_quantity(quantity_from_origin_)),
                         convert_explicitly> ||
    !std::convertible_to<quantity_type, quantity<quantity_point_like_traits<QP>::reference,
                                                 typename quantity_point_like_traits<QP>::rep>>) constexpr
  operator QP_() && noexcept(noexcept(quantity_point_like_traits<QP>::from_quantity(quantity_from_origin_)) &&
                             std::is_nothrow_move_constructible_v<rep>)
  {
    return quantity_point_like_traits<QP>::from_quantity(std::move(quantity_from_origin_)).value;
  }

  // member unary operators
  template<typename QP>
  friend constexpr decltype(auto) operator++(QP && qp)
    requires std::derived_from<std::remove_cvref_t<QP>, quantity_point> && number_line<rep>
  {
    ++qp.quantity_from_origin_;
    return std::forward<QP>(qp);
  }

  [[nodiscard]] constexpr quantity_point operator++(int)
    requires number_line<rep>
  {
    return quantity_point(quantity_from_origin_++);
  }

  template<typename QP>
  friend constexpr decltype(auto) operator--(QP && qp)
    requires std::derived_from<std::remove_cvref_t<QP>, quantity_point> && number_line<rep>
  {
    --qp.quantity_from_origin_;
    return std::forward<QP>(qp);
  }

  [[nodiscard]] constexpr quantity_point operator--(int)
    requires number_line<rep>
  {
    return quantity_point(quantity_from_origin_--);
  }

  // compound assignment operators
  template<typename QP>
    requires std::derived_from<std::remove_cvref_t<QP>, quantity_point>
  friend constexpr decltype(auto) operator+=(QP && qp, const quantity_type & q)
  {
    qp.quantity_from_origin_ += q;
    return std::forward<QP>(qp);
  }

  template<typename QP>
    requires std::derived_from<std::remove_cvref_t<QP>, quantity_point>
  friend constexpr decltype(auto) operator-=(QP && qp, const quantity_type & q)
  {
    qp.quantity_from_origin_ -= q;
    return std::forward<QP>(qp);
  }

private:
#if defined MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17
  template<auto PO2, typename Q>
#else
  template<PointOrigin auto PO2, Quantity Q>
    requires ReferenceOf<std::remove_const_t<decltype(Q::reference)>, PO2.quantity_spec>
#endif
  friend constexpr quantity_point<Q::reference, PO2, typename Q::rep> make_quantity_point(Q&&);

  template<Quantity Q>
    requires std::constructible_from<quantity_type, Q> &&
             ReferenceOf<std::remove_const_t<decltype(Q::reference)>, PO.quantity_spec>
  constexpr explicit quantity_point(Q&& q) : quantity_from_origin_(std::forward<Q>(q))
  {
  }
};

// CTAD
template<QuantityPointLike QP>
explicit(
  is_specialization_of<decltype(quantity_point_like_traits<QP>::to_quantity(std::declval<QP>())), convert_explicitly>)
  quantity_point(QP)
    -> quantity_point<quantity_point_like_traits<QP>::reference, quantity_point_like_traits<QP>::point_origin,
                      typename quantity_point_like_traits<QP>::rep>;

template<auto R1, auto PO1, typename Rep1, auto R2, vector_space_for<Rep1> Rep2>
// TODO simplify when gcc catches up
  requires ReferenceOf<std::remove_const_t<decltype(R2)>, PO1.quantity_spec>
[[nodiscard]] constexpr QuantityPoint auto operator+(const quantity_point<R1, PO1, Rep1>& qp,
                                                     const quantity<R2, Rep2>& q)
{
  return make_quantity_point<PO1>(qp.quantity_ref_from(PO1) + q);
}

template<auto R1, typename Rep2, auto R2, auto PO2, vector_space_for<Rep2> Rep1>
// TODO simplify when gcc catches up
  requires ReferenceOf<std::remove_const_t<decltype(R1)>, PO2.quantity_spec>
[[nodiscard]] constexpr QuantityPoint auto operator+(const quantity<R1, Rep1>& q,
                                                     const quantity_point<R2, PO2, Rep2>& qp)
{
  return qp + q;
}

template<PointOrigin PO, Quantity Q>
  requires ReferenceOf<std::remove_const_t<decltype(Q::reference)>, PO::quantity_spec>
[[nodiscard]] constexpr quantity_point<Q::reference, PO{}, typename Q::rep> operator+(PO, Q&& q)
{
  return make_quantity_point<PO{}>(std::forward<Q>(q));
}

template<Quantity Q, PointOrigin PO>
  requires ReferenceOf<std::remove_const_t<decltype(Q::reference)>, PO::quantity_spec>
[[nodiscard]] constexpr quantity_point<Q::reference, PO{}, typename Q::rep> operator+(Q&& q, PO po)
{
  return po + std::forward<Q>(q);
}

template<auto R1, auto PO1, typename Rep1, auto R2, vector_space_for<Rep1> Rep2>
// TODO simplify when gcc catches up
  requires ReferenceOf<std::remove_const_t<decltype(R2)>, PO1.quantity_spec>
[[nodiscard]] constexpr QuantityPoint auto operator-(const quantity_point<R1, PO1, Rep1>& qp,
                                                     const quantity<R2, Rep2>& q)
{
  return make_quantity_point<PO1>(qp.quantity_ref_from(PO1) - q);
}

template<PointOrigin PO, Quantity Q>
  requires ReferenceOf<std::remove_const_t<decltype(Q::reference)>, PO::quantity_spec>
[[nodiscard]] constexpr QuantityPoint auto operator-(PO po, const Q& q)
{
  return po + (-q);
}

template<QuantityPoint QP1, QuantityPointOf<QP1::absolute_point_origin> QP2>
// TODO consider constraining it for both branches
  requires vector_space_for<typename QP1::quantity_type, typename QP2::quantity_type>
[[nodiscard]] constexpr Quantity auto operator-(const QP1& lhs, const QP2& rhs)
{
  if constexpr (is_same_v<std::remove_const_t<decltype(QP1::point_origin)>,
                          std::remove_const_t<decltype(QP2::point_origin)>>)
    return lhs.quantity_ref_from(QP1::point_origin) - rhs.quantity_ref_from(QP2::point_origin);
  else
    return lhs.quantity_ref_from(QP1::point_origin) - rhs.quantity_ref_from(QP2::point_origin) +
           (lhs.point_origin - rhs.point_origin);
}

template<PointOrigin PO, QuantityPointOf<PO{}> QP>
  requires ReferenceOf<std::remove_const_t<decltype(QP::reference)>, PO::quantity_spec>
[[nodiscard]] constexpr Quantity auto operator-(const QP& qp, PO po)
{
  if constexpr (is_same_v<std::remove_const_t<decltype(QP::point_origin)>, std::remove_const_t<PO>>)
    return qp.quantity_ref_from(QP::point_origin);
  else if constexpr (detail::is_derived_from_specialization_of_absolute_point_origin<PO>) {
    if constexpr (is_same_v<std::remove_const_t<decltype(QP::point_origin)>,
                            std::remove_const_t<decltype(QP::absolute_point_origin)>>)
      return qp.quantity_ref_from(QP::point_origin);
    else
      return qp.quantity_ref_from(QP::point_origin) + (qp.point_origin - qp.absolute_point_origin);
  } else {
    if constexpr (is_same_v<std::remove_const_t<decltype(QP::point_origin)>,
                            std::remove_const_t<decltype(po.quantity_point.point_origin)>>)
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
  if constexpr (is_same_v<std::remove_const_t<decltype(QP1::point_origin)>,
                          std::remove_const_t<decltype(QP2::point_origin)>>)
    return lhs.quantity_ref_from(QP1::point_origin) <=> rhs.quantity_ref_from(QP2::point_origin);
  else
    return lhs - lhs.absolute_point_origin <=> rhs - rhs.absolute_point_origin;
}

template<QuantityPoint QP1, QuantityPointOf<QP1::absolute_point_origin> QP2>
  requires std::equality_comparable_with<typename QP1::quantity_type, typename QP2::quantity_type>
[[nodiscard]] constexpr bool operator==(const QP1& lhs, const QP2& rhs)
{
  if constexpr (is_same_v<std::remove_const_t<decltype(QP1::point_origin)>,
                          std::remove_const_t<decltype(QP2::point_origin)>>)
    return lhs.quantity_ref_from(QP1::point_origin) == rhs.quantity_ref_from(QP2::point_origin);
  else
    return lhs - lhs.absolute_point_origin == rhs - rhs.absolute_point_origin;
}

// make_quantity_point
#if defined MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17
template<auto PO, typename Q>
#else
template<PointOrigin auto PO, Quantity Q>
  requires ReferenceOf<std::remove_const_t<decltype(Q::reference)>, PO.quantity_spec>
#endif
[[nodiscard]] constexpr quantity_point<Q::reference, PO, typename Q::rep> make_quantity_point(Q&& q)
{
  return quantity_point<Q::reference, PO, typename Q::rep>(std::forward<Q>(q));
}

template<QuantityPoint QP>
struct enable_number<QP> : std::true_type {};

}  // namespace mp_units
