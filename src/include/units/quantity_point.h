
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

namespace units {

/**
 * @brief A quantity point
 *
 * Property of a phenomenon, body, or substance, where the property has a magnitude that can be
 * expressed by means of a number and a measurement unit.
 *
 * @tparam D a dimension of the quantity point (can be either a BaseDimension or a DerivedDimension)
 * @tparam U a measurement unit of the quantity point
 * @tparam Rep a type to be used to represent values of a quantity point
 */
template<Dimension D, UnitOf<D> U, Scalar Rep = double>
class quantity_point {
public:
  using quantity_type = quantity<D, U, Rep>;
  using dimension = typename quantity_type::dimension;
  using unit = typename quantity_type::unit;
  using rep = typename quantity_type::rep;

private:
  quantity_type q_{quantity_type::zero()};

public:
  quantity_point() = default;
  quantity_point(const quantity_point&) = default;
  quantity_point(quantity_point&&) = default;

  template<Quantity Q>
    requires std::is_convertible_v<Q, quantity_type>
  constexpr explicit quantity_point(const Q& q) : q_{q} {}

  template<QuantityPoint QP2>
    requires std::is_convertible_v<typename QP2::quantity_type, quantity_type>
  constexpr quantity_point(const QP2& qp) : q_{qp.relative()} {}

  quantity_point& operator=(const quantity_point&) = default;
  quantity_point& operator=(quantity_point&&) = default;

  [[nodiscard]] constexpr quantity_type relative() const noexcept { return q_; }

  template<typename Q = quantity_type>
  [[nodiscard]] static constexpr quantity_point min() noexcept
    requires requires { Q::min(); }
  // requires requires { quantity_type::min(); }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    return quantity_point(quantity_type::min());
  }

  template<typename Q = quantity_type>
  [[nodiscard]] static constexpr quantity_point max() noexcept
    requires requires { Q::max(); }
  // requires requires { quantity_type::max(); }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    return quantity_point(quantity_type::max());
  }

  template<typename Q = quantity_type>
    requires requires(Q q) { ++q; }
  constexpr quantity_point& operator++()
  // requires requires(quantity_type) { ++q; }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    ++q_;
    return *this;
  }

  template<typename Q = quantity_type>
    requires requires(Q q) { q++; }
  constexpr quantity_point operator++(int)
  // requires requires(quantity_type q) { q++; }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    return quantity_point(q_++);
  }

  template<typename Q = quantity_type>
    requires requires(Q q) { --q; }
  constexpr quantity_point& operator--()
  // requires requires(quantity_type q) { --q; }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    --q_;
    return *this;
  }

  template<typename Q = quantity_type>
    requires requires(Q q) { q--; }
  constexpr quantity_point operator--(int)
  // requires requires(quantity_type q) { q--; }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    return quantity_point(q_--);
  }

  template<typename Q = quantity_type>
    requires requires(Q q1, Q q2) { q1 += q2; }
  constexpr quantity_point& operator+=(const quantity_type& q)
  // requires requires(quantity_type q) { q += q; }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    q_ += q;
    return *this;
  }

  template<typename Q = quantity_type>
    requires requires(Q q1, Q q2) { q1 -= q2; }
  constexpr quantity_point& operator-=(const quantity_type& q)
  // requires requires(quantity_type q) { q1 -= q2; }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    q_ -= q;
    return *this;
  }

  // Hidden Friends
  // Below friend functions are to be found via argument-dependent lookup only
#if __GNUC__ >= 10

  template<QuantityPoint QP>
  [[nodiscard]] friend constexpr auto operator<=>(const quantity_point& lhs, const QP& rhs)
    requires std::three_way_comparable_with<quantity_type, typename QP::quantity_type>
  {
    return lhs.q_ <=> rhs.relative();
  }

  template<QuantityPoint QP>
  [[nodiscard]] friend constexpr auto operator==(const quantity_point& lhs, const QP& rhs)
    requires std::equality_comparable_with<quantity_type, typename QP::quantity_type>
  {
    return lhs.q_ == rhs.relative();
  }

#else

  template<QuantityPoint QP>
  [[nodiscard]] friend constexpr bool operator==(const quantity_point& lhs, const QP& rhs)
    requires std::equality_comparable_with<quantity_type, typename QP::quantity_type>
  {
    return lhs.q_ == rhs.relative();
  }

  template<QuantityPoint QP>
  [[nodiscard]] friend constexpr bool operator!=(const quantity_point& lhs, const QP& rhs)
    requires std::equality_comparable_with<quantity_type, typename QP::quantity_type>
  {
    return !(lhs == rhs);
  }

  template<QuantityPoint QP>
  [[nodiscard]] friend constexpr bool operator<(const quantity_point& lhs, const QP& rhs)
    requires std::totally_ordered_with<quantity_type, typename QP::quantity_type>
  {
    return lhs.q_ < rhs.relative();
  }

  template<QuantityPoint QP>
  [[nodiscard]] friend constexpr bool operator<=(const quantity_point& lhs, const QP& rhs)
    requires std::totally_ordered_with<quantity_type, typename QP::quantity_type>
  {
    return !(rhs < lhs);
  }

  template<QuantityPoint QP>
  [[nodiscard]] friend constexpr bool operator>(const quantity_point& lhs, const QP& rhs)
    requires std::totally_ordered_with<quantity_type, typename QP::quantity_type>
  {
    return rhs < lhs;
  }

  template<QuantityPoint QP>
  [[nodiscard]] friend constexpr bool operator>=(const quantity_point& lhs, const QP& rhs)
    requires std::totally_ordered_with<quantity_type, typename QP::quantity_type>
  {
    return !(lhs < rhs);
  }

#endif
};

template<typename D, typename U, typename Rep>
quantity_point(quantity<D, U, Rep>) -> quantity_point<D, U, Rep>;

[[nodiscard]] constexpr QuantityPoint AUTO operator+(const QuantityPoint AUTO& lhs, const Quantity AUTO& rhs)
  requires requires { lhs.relative() + rhs; }
{
  return quantity_point(lhs.relative() + rhs);
}

[[nodiscard]] constexpr QuantityPoint AUTO operator+(const Quantity AUTO& lhs, const QuantityPoint AUTO& rhs)
  requires requires { rhs + lhs; }
{
  return rhs + lhs;
}

[[nodiscard]] constexpr QuantityPoint AUTO operator-(const QuantityPoint AUTO& lhs, const Quantity AUTO& rhs)
  requires requires { lhs.relative() - rhs; }
{
  return quantity_point(lhs.relative() - rhs);
}

template<QuantityPoint QP1, QuantityPoint QP2>
[[nodiscard]] constexpr Quantity AUTO operator-(const QP1& lhs, const QP2& rhs)
  requires requires { lhs.relative() - rhs.relative(); }
{
  return lhs.relative() - rhs.relative();
}

namespace detail {

template<typename D, typename U, typename Rep>
inline constexpr bool is_quantity_point<quantity_point<D, U, Rep>> = true;

}  // namespace detail

}  // namespace units
