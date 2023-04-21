
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

#include <mp_units/bits/quantity_point_concepts.h>
#include <mp_units/customization_points.h>
#include <mp_units/quantity.h>
#include <compare>

namespace mp_units {

template<QuantitySpec auto Q>
struct absolute_point_origin {
  static constexpr QuantitySpec auto quantity_spec = Q;
};

namespace detail {

[[nodiscard]] consteval PointOrigin auto get_absolute_point_origin(PointOrigin auto po)
{
  if constexpr (requires { po.absolute_point_origin; })
    return po.absolute_point_origin;
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

  template<QuantityPointOf<point_origin> QP2>
    requires std::convertible_to<typename QP2::quantity_type, quantity_type>
  constexpr explicit(false) quantity_point(const QP2& qp) : q_(qp.relative())
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
  [[nodiscard]] constexpr const quantity_type& relative() const& noexcept { return q_; }
  [[nodiscard]] constexpr quantity_type&& relative() && noexcept { return std::move(q_); }
  [[nodiscard]] constexpr const quantity_type&& relative() const&& noexcept { return std::move(q_); }

  [[nodiscard]] constexpr Quantity auto absolute() const noexcept
  {
    if constexpr (detail::is_derived_from_specialization_of_absolute_point_origin<
                    std::remove_const_t<decltype(point_origin)>>)
      return relative();
    else
      return point_origin.absolute() + relative();
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

  // Hidden Friends
  // Below friend functions are to be found via argument-dependent lookup only
  template<Quantity Q>
  [[nodiscard]] friend constexpr QuantityPoint auto operator+(const quantity_point& lhs, const Q& rhs)
    requires requires(quantity_type q) { q + rhs; }
  {
    const auto q = lhs.relative() + rhs;
    using q_type = decltype(q);
    return quantity_point<q_type::reference, point_origin, typename q_type::rep>(q);
  }

  template<Quantity Q>
  [[nodiscard]] friend constexpr QuantityPoint auto operator+(const Q& lhs, const quantity_point& rhs)
    requires requires { rhs + lhs; }
  {
    return rhs + lhs;
  }

  template<Quantity Q>
  [[nodiscard]] friend constexpr QuantityPoint auto operator-(const quantity_point& lhs, const Q& rhs)
    requires requires(quantity_type q) { q - rhs; }
  {
    const auto q = lhs.relative() - rhs;
    using q_type = decltype(q);
    return quantity_point<q_type::reference, point_origin, typename q_type::rep>(q);
  }

  template<QuantityPointOf<absolute_point_origin> QP>
  [[nodiscard]] friend constexpr Quantity auto operator-(const quantity_point& lhs, const QP& rhs)
    requires requires(quantity_type q) { q - rhs.absolute(); }
  {
    return lhs.absolute() - rhs.absolute();
  }

  template<QuantityPointOf<absolute_point_origin> QP>
    requires std::three_way_comparable_with<quantity_type, typename QP::quantity_type>
  [[nodiscard]] friend constexpr auto operator<=>(const quantity_point& lhs, const QP& rhs)
  {
    return lhs.relative() <=> rhs.relative();
  }

  template<QuantityPointOf<absolute_point_origin> QP>
    requires std::equality_comparable_with<quantity_type, typename QP::quantity_type>
  [[nodiscard]] friend constexpr bool operator==(const quantity_point& lhs, const QP& rhs)
  {
    return lhs.relative() == rhs.relative();
  }
};

// CTAD
template<Representation Rep>
explicit quantity_point(Rep) -> quantity_point<dimensionless[one], absolute_point_origin<dimensionless>{}, Rep>;

template<Quantity Q>
explicit quantity_point(Q) -> quantity_point<Q::reference, absolute_point_origin<Q::quantity_spec>{}, typename Q::rep>;

template<QuantityLike Q>
explicit quantity_point(Q)
  -> quantity_point<quantity_like_traits<Q>::reference, absolute_point_origin<quantity_like_traits<Q>::quantity_spec>{},
                    typename quantity_like_traits<Q>::rep>;

template<QuantityPointLike QP>
explicit quantity_point(QP)
  -> quantity_point<quantity_point_like_traits<QP>::reference, quantity_point_like_traits<QP>::point_origin,
                    typename quantity_point_like_traits<QP>::rep>;

}  // namespace mp_units