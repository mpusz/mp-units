
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

// IWYU pragma: begin_exports
#include <units/point_origin.h>
#include <units/quantity.h>
#include <compare>
// IWYU pragma: end_exports

#include <units/customization_points.h>
#include <units/generic/dimensionless.h>

namespace units {

/**
 * @brief A statically unspecified quantity point origin
 *
 * An origin, unspecified in the type system, from which an absolute quantity is measured from.
 *
 * @tparam D a dimension of the quantity point (can be either a BaseDimension or a DerivedDimension)
 */
template<Dimension D>
struct dynamic_origin : point_origin<D> {
  template<Dimension D2>
  using rebind = dynamic_origin<D2>;
};

/**
 * @brief A quantity point
 *
 * An absolute quantity measured from an origin.
 *
 * @tparam O a type that represents the origin from which the quantity point is measured from
 * @tparam U a measurement unit of the quantity point
 * @tparam Rep a type to be used to represent values of a quantity point
 */
template<PointOrigin O, UnitOf<typename O::dimension> U, Representation Rep = double>
class quantity_point {
public:
  using origin = O;
  using quantity_type = quantity<typename origin::dimension, U, Rep>;
  using dimension = typename quantity_type::dimension;
  using unit = typename quantity_type::unit;
  using rep = typename quantity_type::rep;
  static constexpr units::reference<dimension, unit> reference{};

private:
  quantity_type q_{};

public:
  quantity_point() = default;
  quantity_point(const quantity_point&) = default;
  quantity_point(quantity_point&&) = default;

  template<typename T>
    requires std::constructible_from<quantity_type, T>
  constexpr explicit quantity_point(T&& t) : q_(std::forward<T>(t))
  {
  }

  template<QuantityPointOf<origin> QP2>
    requires std::convertible_to<typename QP2::quantity_type, quantity_type>
  constexpr explicit(false) quantity_point(const QP2& qp) : q_(qp.relative())
  {
  }

  template<QuantityPointLike QP>
  constexpr explicit quantity_point(const QP& qp)
    requires std::is_constructible_v<quantity_type, decltype(quantity_point_like_traits<QP>::relative(qp))> &&
             equivalent<origin, typename quantity_point_like_traits<QP>::origin>
      : q_(quantity_point_like_traits<QP>::relative(qp))
  {
  }

  quantity_point& operator=(const quantity_point&) = default;
  quantity_point& operator=(quantity_point&&) = default;

  [[nodiscard]] constexpr quantity_type& relative() & noexcept { return q_; }
  [[nodiscard]] constexpr const quantity_type& relative() const& noexcept { return q_; }
  [[nodiscard]] constexpr quantity_type&& relative() && noexcept { return std::move(q_); }
  [[nodiscard]] constexpr const quantity_type&& relative() const&& noexcept { return std::move(q_); }

  [[nodiscard]] static constexpr quantity_point min() noexcept
    requires requires { quantity_type::min(); }
  {
    return quantity_point(quantity_type::min());
  }

  [[nodiscard]] static constexpr quantity_point max() noexcept
    requires requires { quantity_type::max(); }
  {
    return quantity_point(quantity_type::max());
  }

  constexpr quantity_point& operator++()
    requires requires(quantity_type q) { ++q; }
  {
    ++q_;
    return *this;
  }

  [[nodiscard]] constexpr quantity_point operator++(int)
    requires requires(quantity_type q) { q++; }
  {
    return quantity_point(q_++);
  }

  constexpr quantity_point& operator--()
    requires requires(quantity_type q) { --q; }
  {
    --q_;
    return *this;
  }

  [[nodiscard]] constexpr quantity_point operator--(int)
    requires requires(quantity_type q) { q--; }
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
    requires requires(quantity_type q1, quantity_type q2) { q1 -= q2; }
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
    return quantity_point<rebind_point_origin_dimension<origin, typename q_type::dimension>, typename q_type::unit,
                          typename q_type::rep>(q);
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
    return quantity_point<rebind_point_origin_dimension<origin, typename q_type::dimension>, typename q_type::unit,
                          typename q_type::rep>(q);
  }

  template<QuantityPointOf<origin> QP>
  [[nodiscard]] friend constexpr Quantity auto operator-(const quantity_point& lhs, const QP& rhs)
    requires requires(quantity_type q) { q - rhs.relative(); }
  {
    return lhs.relative() - rhs.relative();
  }

  template<QuantityPointOf<origin> QP>
    requires std::three_way_comparable_with<quantity_type, typename QP::quantity_type>
  [[nodiscard]] friend constexpr auto operator<=>(const quantity_point& lhs, const QP& rhs)
  {
    return lhs.relative() <=> rhs.relative();
  }

  template<QuantityPointOf<origin> QP>
    requires std::equality_comparable_with<quantity_type, typename QP::quantity_type>
  [[nodiscard]] friend constexpr bool operator==(const quantity_point& lhs, const QP& rhs)
  {
    return lhs.relative() == rhs.relative();
  }
};

template<Representation Rep>
explicit quantity_point(Rep) -> quantity_point<dynamic_origin<dim_one>, one, Rep>;

template<Quantity Q>
explicit quantity_point(Q) -> quantity_point<dynamic_origin<typename Q::dimension>, typename Q::unit, typename Q::rep>;

template<QuantityLike Q>
explicit quantity_point(Q)
  -> quantity_point<dynamic_origin<typename quantity_like_traits<Q>::dimension>, typename quantity_like_traits<Q>::unit,
                    typename quantity_like_traits<Q>::rep>;

template<QuantityPointLike QP>
explicit quantity_point(QP)
  -> quantity_point<typename quantity_point_like_traits<QP>::origin, typename quantity_point_like_traits<QP>::unit,
                    typename quantity_point_like_traits<QP>::rep>;

namespace detail {

template<typename O, typename U, typename Rep>
inline constexpr bool is_quantity_point<quantity_point<O, U, Rep>> = true;

}  // namespace detail

}  // namespace units
