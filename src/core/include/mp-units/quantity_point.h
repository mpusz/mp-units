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

template<QuantitySpec auto Q>
struct absolute_point_origin {
  static constexpr QuantitySpec auto quantity_spec = Q;
};

template<QuantityPoint auto QP>
struct relative_point_origin {
  static constexpr QuantityPoint auto quantity_point = QP;
  static constexpr QuantitySpec auto quantity_spec =
    common_quantity_spec(QP.quantity_spec, QP.point_origin.quantity_spec);
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

template<QuantityPointLike QP>
using quantity_point_like_type =
  quantity_point<quantity_point_like_traits<QP>::reference, quantity_point_like_traits<QP>::point_origin,
                 typename quantity_point_like_traits<QP>::rep>;

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
template<Reference auto R, PointOriginFor<get_quantity_spec(R)> auto PO = absolute_point_origin<get_quantity_spec(R)>{},
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

  quantity_type q_;  // needs to be public for a structural type

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

  template<typename T>
    requires std::constructible_from<quantity_type, T>
  constexpr explicit quantity_point(T&& v, decltype(point_origin) = point_origin) : q_(std::forward<T>(v))
  {
  }

  template<QuantityPointOf<absolute_point_origin> QP>
    requires std::constructible_from<quantity_type, typename QP::quantity_type>
  // TODO add perfect forwarding
  constexpr explicit(!std::convertible_to<typename QP::quantity_type, quantity_type>) quantity_point(const QP& qp) :
      q_([&] {
        if constexpr (is_same_v<std::remove_const_t<decltype(point_origin)>,
                                std::remove_const_t<decltype(QP::point_origin)>>) {
          return qp.relative();
        } else if constexpr (detail::is_derived_from_specialization_of_absolute_point_origin<
                               std::remove_const_t<decltype(point_origin)>>) {
          return qp.absolute();
        } else {
          return qp.absolute() - zero().absolute();
        }
      }())
  {
  }

  template<QuantityPointLike QP>
    requires std::same_as<std::remove_const_t<decltype(quantity_point_like_traits<QP>::point_origin)>,
                          std::remove_const_t<decltype(point_origin)>> &&
             std::convertible_to<typename detail::quantity_point_like_type<QP>::quantity_type, quantity_type>
  constexpr explicit quantity_point(const QP& qp) :
      q_(typename detail::quantity_point_like_type<QP>::quantity_type{quantity_point_like_traits<QP>::relative(qp)})
  {
  }

  quantity_point& operator=(const quantity_point&) = default;
  quantity_point& operator=(quantity_point&&) = default;

  template<PointOriginFor<quantity_spec> NewPO>
  [[nodiscard]] constexpr QuantityPointOf<NewPO{}> auto point_from(NewPO origin) const
  {
    if constexpr (is_same_v<NewPO, std::remove_const_t<decltype(point_origin)>>) {
      return *this;
    } else if constexpr (detail::is_derived_from_specialization_of_absolute_point_origin<NewPO>) {
      auto q = absolute();
      return quantity_point<reference, NewPO{}, typename decltype(q)::rep>(std::move(q));
    } else {
      auto q = absolute() - origin.quantity_point.absolute();
      return quantity_point<reference, NewPO{}, typename decltype(q)::rep>(std::move(q));
    }
  }

  // data access
#ifdef __cpp_explicit_this_parameter
  template<typename Self>
  [[nodiscard]] constexpr auto&& relative(this Self&& self) noexcept
  {
    return std::forward<Self>(self).q_;
  }
#else
  [[nodiscard]] constexpr quantity_type& relative() & noexcept { return q_; }
  [[nodiscard]] constexpr const quantity_type& relative() const& noexcept { return q_; }
  [[nodiscard]] constexpr quantity_type&& relative() && noexcept { return std::move(q_); }
  [[nodiscard]] constexpr const quantity_type&& relative() const&& noexcept { return std::move(q_); }
#endif

  [[nodiscard]] constexpr Quantity auto absolute() const noexcept
  {
    if constexpr (detail::is_derived_from_specialization_of_absolute_point_origin<
                    std::remove_const_t<decltype(point_origin)>>)
      return relative();
    else
      return point_origin.quantity_point.absolute() + relative();
  }

  template<Unit U>
    requires detail::QuantityConvertibleTo<quantity_type, quantity<::mp_units::reference<quantity_spec, U{}>{}, Rep>>
  [[nodiscard]] constexpr quantity_point<::mp_units::reference<quantity_spec, U{}>{}, PO, Rep> operator[](U) const
  {
    return quantity_point<quantity_spec[U{}], PO, Rep>{*this};
  }

  // member unary operators
  constexpr quantity_point& operator++()
    requires requires { ++q_; }
  {
    ++q_;
    return *this;
  }

  [[nodiscard]] constexpr quantity_point operator++(int)
    requires requires { q_++; }
  {
    return quantity_point(q_++);
  }

  constexpr quantity_point& operator--()
    requires requires { --q_; }
  {
    --q_;
    return *this;
  }

  [[nodiscard]] constexpr quantity_point operator--(int)
    requires requires { q_--; }
  {
    return quantity_point(q_--);
  }

  constexpr quantity_point& operator+=(const quantity_type& q)
    requires requires { q_ += q; }
  {
    q_ += q;
    return *this;
  }

  constexpr quantity_point& operator-=(const quantity_type& q)
    requires requires { q_ -= q; }
  {
    q_ -= q;
    return *this;
  }
};

// CTAD
template<Quantity Q>
explicit quantity_point(Q) -> quantity_point<Q::reference, absolute_point_origin<Q::quantity_spec>{}, typename Q::rep>;

template<Quantity Q, PointOrigin PO>
explicit quantity_point(Q, PO) -> quantity_point<Q::reference, PO{}, typename Q::rep>;

template<QuantityPointLike QP>
explicit quantity_point(QP)
  -> quantity_point<quantity_point_like_traits<QP>::reference, quantity_point_like_traits<QP>::point_origin,
                    typename quantity_point_like_traits<QP>::rep>;

template<auto R1, auto PO1, typename Rep1, auto R2, typename Rep2>
// TODO simplify when gcc catches up
  requires ReferenceOf<std::remove_const_t<decltype(R2)>, get_quantity_spec(R1)>
[[nodiscard]] constexpr QuantityPoint auto operator+(const quantity_point<R1, PO1, Rep1>& qp,
                                                     const quantity<R2, Rep2>& q)
  requires requires { qp.relative() + q; }
{
  auto temp = qp.relative() + q;
  using q_type = decltype(temp);
  return quantity_point<q_type::reference, PO1, typename q_type::rep>(std::move(temp));
}

template<auto R1, typename Rep1, auto R2, auto PO2, typename Rep2>
// TODO simplify when gcc catches up
  requires ReferenceOf<std::remove_const_t<decltype(R1)>, get_quantity_spec(R2)>
[[nodiscard]] constexpr QuantityPoint auto operator+(const quantity<R1, Rep1>& q,
                                                     const quantity_point<R2, PO2, Rep2>& qp)
  requires requires { q + qp.relative(); }
{
  return qp + q;
}

template<PointOrigin PO, Quantity Q>
  requires ReferenceOf<std::remove_const_t<decltype(Q::reference)>, PO::quantity_spec>
[[nodiscard]] constexpr quantity_point<Q::reference, PO{}, typename Q::rep> operator+(PO, Q&& q)
{
  return quantity_point<Q::reference, PO{}, typename Q::rep>(std::forward<Q>(q));
}

template<Quantity Q, PointOrigin PO>
  requires ReferenceOf<std::remove_const_t<decltype(Q::reference)>, PO::quantity_spec>
[[nodiscard]] constexpr quantity_point<Q::reference, PO{}, typename Q::rep> operator+(Q&& q, PO po)
{
  return po + std::forward<Q>(q);
}

template<auto R1, auto PO1, typename Rep1, auto R2, typename Rep2>
// TODO simplify when gcc catches up
  requires ReferenceOf<std::remove_const_t<decltype(R2)>, get_quantity_spec(R1)>
[[nodiscard]] constexpr QuantityPoint auto operator-(const quantity_point<R1, PO1, Rep1>& qp,
                                                     const quantity<R2, Rep2>& q)
  requires requires { qp.relative() - q; }
{
  const auto temp = qp.relative() - q;
  using q_type = decltype(temp);
  return quantity_point<q_type::reference, PO1, typename q_type::rep>(std::move(temp));
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
  requires requires { lhs.absolute() - rhs.absolute(); }
{
  if constexpr (is_same_v<std::remove_const_t<decltype(QP1::point_origin)>,
                          std::remove_const_t<decltype(QP2::point_origin)>>) {
    constexpr auto common_qs = common_quantity_spec(QP1::quantity_spec, QP1::point_origin.quantity_spec,
                                                    QP2::quantity_spec, QP2::point_origin.quantity_spec);
    return quantity_cast<common_qs>(lhs.relative() - rhs.relative());
  } else
    return lhs.absolute() - rhs.absolute();
}

template<PointOrigin PO, QuantityPointOf<PO{}> QP>
  requires ReferenceOf<std::remove_const_t<decltype(QP::reference)>, PO::quantity_spec>
[[nodiscard]] constexpr Quantity auto operator-(const QP& qp, PO)
{
  constexpr auto common_qs = common_quantity_spec(PO::quantity_spec, QP::quantity_spec, QP::point_origin.quantity_spec);
  if constexpr (detail::is_derived_from_specialization_of_absolute_point_origin<PO>) {
    if constexpr (is_same_v<std::remove_const_t<PO>, std::remove_const_t<decltype(QP::point_origin)>>)
      return quantity_cast<common_qs>(qp.relative());
    else
      return quantity_cast<common_qs>(qp.absolute());
  } else {
    if constexpr (is_same_v<std::remove_const_t<PO>, std::remove_const_t<decltype(QP::point_origin)>>)
      return quantity_cast<common_qs>(qp.relative());
    else
      return quantity_cast<common_qs>(qp.absolute() - PO::quantity_point.absolute());
  }
}

template<PointOrigin PO, QuantityPointOf<PO{}> QP>
  requires ReferenceOf<std::remove_const_t<decltype(QP::reference)>, PO::quantity_spec>
[[nodiscard]] constexpr Quantity auto operator-(PO po, const QP& qp)
{
  return -(qp - po);
}

template<PointOrigin PO1, PointOriginOf<PO1{}> PO2>
  requires QuantitySpecOf<std::remove_const_t<decltype(PO1::quantity_spec)>, PO2::quantity_spec> &&
           (detail::is_derived_from_specialization_of_relative_point_origin<PO1> ||
            detail::is_derived_from_specialization_of_relative_point_origin<PO2>)
[[nodiscard]] constexpr Quantity auto operator-(PO1 po1, PO2 po2)
{
  constexpr auto common_qs = common_quantity_spec(PO1::quantity_spec, PO2::quantity_spec);
  if constexpr (detail::is_derived_from_specialization_of_absolute_point_origin<PO1>) {
    return quantity_cast<common_qs>(-po2.quantity_point.absolute());
  } else if constexpr (detail::is_derived_from_specialization_of_absolute_point_origin<PO2>) {
    return quantity_cast<common_qs>(po1.quantity_point.absolute());
  } else {
    return quantity_cast<common_qs>(po1.quantity_point - po2.quantity_point);
  }
}

template<QuantityPoint QP1, QuantityPointOf<QP1::absolute_point_origin> QP2>
  requires std::three_way_comparable_with<typename QP1::quantity_type, typename QP2::quantity_type>
[[nodiscard]] constexpr auto operator<=>(const QP1& lhs, const QP2& rhs)
{
  if constexpr (is_same_v<std::remove_const_t<decltype(QP1::point_origin)>,
                          std::remove_const_t<decltype(QP2::point_origin)>>)
    return lhs.relative() <=> rhs.relative();
  else
    return lhs.absolute() <=> rhs.absolute();
}

template<QuantityPoint QP1, QuantityPointOf<QP1::absolute_point_origin> QP2>
  requires std::equality_comparable_with<typename QP1::quantity_type, typename QP2::quantity_type>
[[nodiscard]] constexpr bool operator==(const QP1& lhs, const QP2& rhs)
{
  if constexpr (is_same_v<std::remove_const_t<decltype(QP1::point_origin)>,
                          std::remove_const_t<decltype(QP2::point_origin)>>)
    return lhs.relative() == rhs.relative();
  else
    return lhs.absolute() == rhs.absolute();
}

}  // namespace mp_units
