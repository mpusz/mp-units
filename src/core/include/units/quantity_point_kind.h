
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
#include <units/quantity_kind.h>
#include <units/quantity_point.h>
#include <compare>
// IWYU pragma: end_exports

namespace units {

/**
 * @brief A quantity point kind
 *
 * An absolute quantity kind measured from an origin.
 *
 * @tparam PK the point kind of quantity point
 * @tparam U the measurement unit of the quantity point kind
 * @tparam Rep the type to be used to represent values of the quantity point kind
 */
template<PointKind PK, UnitOf<typename PK::dimension> U, Representation Rep = double>
class quantity_point_kind {
public:
  using point_kind_type = PK;
  using kind_type = typename PK::base_kind;
  using origin = typename point_kind_type::origin;
  using quantity_kind_type = quantity_kind<kind_type, U, Rep>;
  using quantity_type = typename quantity_kind_type::quantity_type;
  using dimension = typename quantity_type::dimension;
  using unit = typename quantity_type::unit;
  using rep = typename quantity_type::rep;
  static constexpr units::reference<dimension, unit> reference{};

private:
  quantity_kind_type qk_;

public:
  quantity_point_kind() = default;
  quantity_point_kind(const quantity_point_kind&) = default;
  quantity_point_kind(quantity_point_kind&&) = default;

  template<typename T>
    requires std::constructible_from<quantity_kind_type, T>
  constexpr explicit quantity_point_kind(T&& t) : qk_(std::forward<T>(t))
  {
  }

  constexpr explicit quantity_point_kind(const quantity_point<origin, U, Rep>& qp) : qk_(qp.relative()) {}
  constexpr explicit quantity_point_kind(quantity_point<origin, U, Rep>&& qp) : qk_(std::move(qp).relative()) {}

  template<QuantityPointLike QP>
    requires(std::constructible_from<quantity_point<origin, U, Rep>, QP>)
  constexpr explicit quantity_point_kind(const QP& qp) : qk_(quantity_point_like_traits<QP>::relative(qp))
  {
  }

  template<QuantityPointKindOf<point_kind_type> QPK2>
    requires std::convertible_to<typename QPK2::quantity_kind_type, quantity_kind_type>
  constexpr explicit(false) quantity_point_kind(const QPK2& qpk) : qk_(qpk.relative())
  {
  }

  quantity_point_kind& operator=(const quantity_point_kind&) = default;
  quantity_point_kind& operator=(quantity_point_kind&&) = default;

  [[nodiscard]] constexpr quantity_kind_type& relative() & noexcept { return qk_; }
  [[nodiscard]] constexpr const quantity_kind_type& relative() const& noexcept { return qk_; }
  [[nodiscard]] constexpr quantity_kind_type&& relative() && noexcept { return std::move(qk_); }
  [[nodiscard]] constexpr const quantity_kind_type&& relative() const&& noexcept { return std::move(qk_); }

  [[nodiscard]] static constexpr quantity_point_kind min() noexcept
    requires requires { quantity_kind_type::min(); }
  {
    return quantity_point_kind(quantity_kind_type::min());
  }

  [[nodiscard]] static constexpr quantity_point_kind max() noexcept
    requires requires { quantity_kind_type::max(); }
  {
    return quantity_point_kind(quantity_kind_type::max());
  }

  constexpr quantity_point_kind& operator++()
    requires requires(quantity_kind_type qk) { ++qk; }
  {
    ++qk_;
    return *this;
  }

  [[nodiscard]] constexpr quantity_point_kind operator++(int)
    requires requires(quantity_kind_type qk) { qk++; }
  {
    return quantity_point_kind(qk_++);
  }

  constexpr quantity_point_kind& operator--()
    requires requires(quantity_kind_type qk) { --qk; }
  {
    --qk_;
    return *this;
  }

  [[nodiscard]] constexpr quantity_point_kind operator--(int)
    requires requires(quantity_kind_type qk) { qk--; }
  {
    return quantity_point_kind(qk_--);
  }

  constexpr quantity_point_kind& operator+=(const quantity_kind_type& qk)
    requires requires(quantity_kind_type qk1, const quantity_kind_type qk2) { qk1 += qk2; }
  {
    qk_ += qk;
    return *this;
  }

  constexpr quantity_point_kind& operator-=(const quantity_kind_type& qk)
    requires requires(quantity_kind_type qk1, const quantity_kind_type qk2) { qk1 -= qk2; }
  {
    qk_ -= qk;
    return *this;
  }

  // Hidden Friends
  // Below friend functions are to be found via argument-dependent lookup only

  template<QuantityKind QK>
  [[nodiscard]] friend constexpr QuantityPointKind auto operator+(const quantity_point_kind& lhs, const QK& rhs)
    requires requires(quantity_kind_type qk) { qk + rhs; }
  {
    return units::quantity_point_kind(lhs.relative() + rhs);
  }

  template<QuantityKind QK>
  [[nodiscard]] friend constexpr QuantityPointKind auto operator+(const QK& lhs, const quantity_point_kind& rhs)
    requires requires(quantity_kind_type qk) { lhs + qk; }
  {
    return units::quantity_point_kind(lhs + rhs.relative());
  }

  template<QuantityKind QK>
  [[nodiscard]] friend constexpr QuantityPointKind auto operator-(const quantity_point_kind& lhs, const QK& rhs)
    requires requires(quantity_kind_type qk) { qk - rhs; }
  {
    return units::quantity_point_kind(lhs.relative() - rhs);
  }

  template<QuantityPointKindOf<point_kind_type> QPK>
  [[nodiscard]] friend constexpr QuantityKind auto operator-(const quantity_point_kind& lhs, const QPK& rhs)
    requires requires(quantity_kind_type qk) { qk - qk; }
  {
    return lhs.relative() - rhs.relative();
  }

  template<QuantityPointKindOf<point_kind_type> QPK>
    requires std::three_way_comparable_with<quantity_kind_type, typename QPK::quantity_kind_type>
  [[nodiscard]] friend constexpr auto operator<=>(const quantity_point_kind& lhs, const QPK& rhs)
  {
    return lhs.relative() <=> rhs.relative();
  }

  template<QuantityPointKindOf<point_kind_type> QPK>
    requires std::equality_comparable_with<quantity_kind_type, typename QPK::quantity_kind_type>
  [[nodiscard]] friend constexpr bool operator==(const quantity_point_kind& lhs, const QPK& rhs)
  {
    return lhs.relative() == rhs.relative();
  }
};

template<QuantityKind QK>
quantity_point_kind(QK)
  -> quantity_point_kind<downcast_point_kind<typename QK::kind_type>, typename QK::unit, typename QK::rep>;

namespace detail {

template<typename PK, typename U, typename Rep>
inline constexpr bool is_quantity_point_kind<quantity_point_kind<PK, U, Rep>> = true;

}  // namespace detail

}  // namespace units
