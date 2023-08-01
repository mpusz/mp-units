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
  static constexpr QuantityPoint auto parent_origin = QP;
  static constexpr QuantitySpec auto quantity_spec = QP.quantity_spec;
};

namespace detail {

[[nodiscard]] consteval PointOrigin auto get_absolute_point_origin(PointOrigin auto po)
{
  if constexpr (requires { po.parent_origin.absolute_point_origin; })
    return po.parent_origin.absolute_point_origin;
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
  constexpr explicit quantity_point(T&& v) : q_(std::forward<T>(v))
  {
  }

  template<auto R2, typename Rep2>
    requires std::constructible_from<quantity_type, quantity<R2, Rep2>>
  constexpr explicit(!std::convertible_to<quantity<R2, Rep2>, quantity_type>)
    quantity_point(const quantity_point<R2, point_origin, Rep2>& qp) :
      q_(qp.relative())
  {
  }

  template<QuantityPointOf<absolute_point_origin> QP>
    requires std::constructible_from<quantity_type, typename QP::quantity_type>
  constexpr explicit(!std::convertible_to<typename QP::quantity_type, quantity_type>) quantity_point(const QP& qp) :
      q_(qp.relative() + QP::zero().absolute() - zero().absolute())
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

  // data access
  [[nodiscard]] constexpr quantity_type& relative() & noexcept { return q_; }
  [[nodiscard]] constexpr const quantity_type& relative() const& noexcept { return q_; }
  [[nodiscard]] constexpr quantity_type&& relative() && noexcept { return std::move(q_); }
  [[nodiscard]] constexpr const quantity_type&& relative() const&& noexcept { return std::move(q_); }

  [[nodiscard]] constexpr Quantity auto absolute() const noexcept
  {
    if constexpr (detail::is_derived_from_specialization_of_absolute_point_origin<
                    std::remove_const_t<decltype(point_origin)>>)
      return relative();
    else
      return point_origin.parent_origin.absolute() + relative();
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

template<QuantityPointLike QP>
explicit quantity_point(QP)
  -> quantity_point<quantity_point_like_traits<QP>::reference, quantity_point_like_traits<QP>::point_origin,
                    typename quantity_point_like_traits<QP>::rep>;

template<auto R1, auto PO1, typename Rep1, auto R2, typename Rep2>
// TODO simplify when gcc catches up
  requires ReferenceOf<std::remove_const_t<decltype(R2)>, get_quantity_spec(R1)>
[[nodiscard]] constexpr QuantityPoint auto operator+(const quantity_point<R1, PO1, Rep1>& lhs,
                                                     const quantity<R2, Rep2>& rhs)
  requires requires { lhs.relative() + rhs; }
{
  const auto q = lhs.relative() + rhs;
  using q_type = decltype(q);
  return quantity_point<q_type::reference, PO1, typename q_type::rep>(q);
}

template<auto R1, typename Rep1, auto R2, auto PO2, typename Rep2>
// TODO simplify when gcc catches up
  requires ReferenceOf<std::remove_const_t<decltype(R1)>, get_quantity_spec(R2)>
[[nodiscard]] constexpr QuantityPoint auto operator+(const quantity<R1, Rep1>& lhs,
                                                     const quantity_point<R2, PO2, Rep2>& rhs)
  requires requires { rhs + lhs; }
{
  return rhs + lhs;
}

template<auto R1, auto PO1, typename Rep1, auto R2, typename Rep2>
// TODO simplify when gcc catches up
  requires ReferenceOf<std::remove_const_t<decltype(R2)>, get_quantity_spec(R1)>
[[nodiscard]] constexpr QuantityPoint auto operator-(const quantity_point<R1, PO1, Rep1>& lhs,
                                                     const quantity<R2, Rep2>& rhs)
  requires requires { lhs.relative() - rhs; }
{
  const auto q = lhs.relative() - rhs;
  using q_type = decltype(q);
  return quantity_point<q_type::reference, PO1, typename q_type::rep>(q);
}

template<QuantityPoint QP1, QuantityPointOf<QP1::absolute_point_origin> QP2>
[[nodiscard]] constexpr Quantity auto operator-(const QP1& lhs, const QP2& rhs)
  requires requires { lhs.absolute() - rhs.absolute(); }
{
  return lhs.absolute() - rhs.absolute();
}

template<QuantityPoint QP1, QuantityPointOf<QP1::absolute_point_origin> QP2>
  requires std::three_way_comparable_with<typename QP1::quantity_type, typename QP2::quantity_type>
[[nodiscard]] constexpr auto operator<=>(const QP1& lhs, const QP2& rhs)
{
  return lhs.absolute() <=> rhs.absolute();
}

template<QuantityPoint QP1, QuantityPointOf<QP1::absolute_point_origin> QP2>
  requires std::equality_comparable_with<typename QP1::quantity_type, typename QP2::quantity_type>
[[nodiscard]] constexpr bool operator==(const QP1& lhs, const QP2& rhs)
{
  return lhs.absolute() == rhs.absolute();
}

}  // namespace mp_units
