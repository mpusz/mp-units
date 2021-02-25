
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

#include <units/quantity.h>
#include <units/origin.h>
#include <compare>

namespace units {

/**
 * @brief A quantity point
 *
 * An absolute quantity with respect to zero (which represents some origin).
 *
 * @tparam D a dimension of the quantity point (can be either a BaseDimension or a DerivedDimension)
 * @tparam U a measurement unit of the quantity point
 * @tparam Rep a type to be used to represent values of a quantity point
 * @tparam Orig an abstract origin point with respect to which to define the quantity point
 */
template<Dimension D, UnitOf<D> U, QuantityValue Rep = double, PointOrigin Orig = default_point_origin<typename dimension_unit<D>::reference>>
class quantity_point {
public:
  using quantity_type = quantity<D, U, Rep>;
  using dimension = typename quantity_type::dimension;
  using unit = typename quantity_type::unit;
  using rep = typename quantity_type::rep;
  using origin = Orig;

private:
  quantity_type q_{};

public:
  quantity_point() = default;
  quantity_point(const quantity_point&) = default;
  quantity_point(quantity_point&&) = default;

  template<safe_convertible_to_<rep> Value>
    requires is_same_v<dimension, dim_one> && std::is_constructible_v<quantity_type, Value>
  constexpr explicit quantity_point(const Value& v) : q_(v) {}

  constexpr explicit quantity_point(const quantity_type& q) : q_{q} {}

  template<QuantityLike Q>
    requires std::is_constructible_v<quantity_type, Q>
  constexpr explicit quantity_point(const Q& q) : q_{q} {}

  template<QuantityPointLike QP>
  constexpr explicit quantity_point(const QP& qp)
    requires (
          std::is_constructible_v<quantity_type, decltype(quantity_point_like_traits<QP>::relative(qp))>
            && std::is_same_v<origin, typename quantity_point_like_traits<QP>::origin> )
    : q_{quantity_point_like_traits<QP>::relative(qp)} {}

  template<QuantityPoint QP>
    requires ( std::is_convertible_v<typename QP::quantity_type, quantity_type>
        && std::is_same_v<origin, typename QP::origin>)
  constexpr quantity_point(const QP& qp) : q_{qp.relative()} {}

  quantity_point& operator=(const quantity_point&) = default;
  quantity_point& operator=(quantity_point&&) = default;

  [[nodiscard]] constexpr quantity_type relative() const noexcept { return q_; }

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
    requires requires(quantity_type q) { q += q; }
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
    requires requires { lhs.relative() + rhs; }
  {
    const auto q = lhs.relative() + rhs;
    using q_type = decltype(q);
    return quantity_point<typename q_type::dimension, typename q_type::unit, typename q_type::rep, origin>(q);
  }

  template<Quantity Q>
  [[nodiscard]] friend constexpr QuantityPoint auto operator+(const Q& lhs, const quantity_point& rhs)
    requires requires { rhs + lhs; }
  {
    return rhs + lhs;
  }

  template<Quantity Q>
  [[nodiscard]] friend constexpr QuantityPoint auto operator-(const quantity_point& lhs, const Q& rhs)
    requires requires { lhs.relative() - rhs; }
  {
    const auto q = lhs.relative() - rhs;
    using q_type = decltype(q);
    return quantity_point<typename q_type::dimension, typename q_type::unit, typename q_type::rep, origin>(q);
  }

  template<QuantityPoint QP> requires std::is_same_v<typename QP::origin, origin>
  [[nodiscard]] friend constexpr Quantity auto operator-(const quantity_point& lhs, const QP& rhs)
    requires requires { lhs.relative() - rhs.relative(); }
  {
    return lhs.relative() - rhs.relative();
  }

  template<QuantityPoint QP>
    requires (std::three_way_comparable_with<quantity_type, typename QP::quantity_type>
        && std::is_same_v<origin, typename QP::origin>)
  [[nodiscard]] friend constexpr auto operator<=>(const quantity_point& lhs, const QP& rhs)
  {
    return lhs.relative() <=> rhs.relative();
  }

  template<QuantityPoint QP>
    requires (std::three_way_comparable_with<quantity_type, typename QP::quantity_type>
              && std::is_same_v<origin, typename QP::origin>)
  [[nodiscard]] friend constexpr bool operator==(const quantity_point& lhs, const QP& rhs)
  {
    return lhs.relative() == rhs.relative();
  }

};

template<QuantityValue V>
explicit(false) quantity_point(V) -> quantity_point<dim_one, one, V>;

template<QuantityLike Q>
quantity_point(Q) -> quantity_point<typename quantity_like_traits<Q>::dimension,
  typename quantity_like_traits<Q>::unit, typename quantity_like_traits<Q>::rep>;

template<QuantityPointLike QP>
explicit quantity_point(QP) -> quantity_point<typename quantity_point_like_traits<QP>::dimension,
                                              typename quantity_point_like_traits<QP>::unit,
                                              typename quantity_point_like_traits<QP>::rep,
                                              typename quantity_point_like_traits<QP>::origin>;


template <PointOrigin Orig, Dimension D, UnitOf<D> U, QuantityValue Rep>
inline constexpr quantity_point<D, U, Rep, Orig> absolute(const quantity<D,U,Rep> &q) noexcept {
  return quantity_point<D, U, Rep, Orig>(q);
}

namespace detail {

template<typename D, typename U, typename Rep, typename Orig>
inline constexpr bool is_quantity_point<quantity_point<D, U, Rep, Orig>> = true;

}  // namespace detail

}  // namespace units
