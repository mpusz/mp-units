
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

#include <units/bits/common_quantity.h>
#include <units/bits/dimension_op.h>
#include <units/bits/pow.h>
#include <units/bits/unit_text.h>
#include <units/generic/dimensionless.h>
#include <units/quantity_cast.h>
#include <compare>
#include <ostream>

namespace units {

namespace detail {

template<typename From, typename To>
concept safe_convertible = // exposition only
    std::convertible_to<From, To> &&
    (treat_as_floating_point<To> || (!treat_as_floating_point<From>));

template<typename Rep, typename QuantityFrom, typename QuantityTo>
concept safe_divisible = // exposition only
    treat_as_floating_point<Rep> ||
    is_integral(quantity_ratio(QuantityFrom{}) / quantity_ratio(QuantityTo{}));

} // namespace detail

/**
 * @brief A quantity
 *
 * Property of a phenomenon, body, or substance, where the property has a magnitude that can be
 * expressed by means of a number and a measurement unit.
 *
 * @tparam D a dimension of the quantity (can be either a BaseDimension or a DerivedDimension)
 * @tparam U a measurement unit of the quantity
 * @tparam Rep a type to be used to represent values of a quantity
 */
template<Dimension D, UnitOf<D> U, ScalableNumber Rep = double>
class quantity {
  Rep value_{};

public:
  using dimension = D;
  using unit = U;
  using rep = Rep;

  quantity() = default;
  quantity(const quantity&) = default;
  quantity(quantity&&) = default;

  template<ScalableNumber Value>
    requires detail::safe_convertible<Value, rep>
  constexpr explicit(!(equivalent<quantity, dimensionless<one, Rep>>)) quantity(const Value& v) : value_{static_cast<rep>(v)} {}

  template<Quantity Q2>
    requires equivalent<D, typename Q2::dimension> &&
             detail::safe_convertible<typename Q2::rep, rep> &&
             detail::safe_divisible<rep, Q2, quantity>
  constexpr quantity(const Q2& q) : value_{quantity_cast<quantity>(q).count()} {}

  quantity& operator=(const quantity&) = default;
  quantity& operator=(quantity&&) = default;

  [[nodiscard]] constexpr rep count() const noexcept { return value_; }

  [[nodiscard]] static constexpr quantity zero() noexcept
    requires requires { quantity_values<Rep>::zero(); }
  {
    return quantity(quantity_values<Rep>::zero());
  }

  [[nodiscard]] static constexpr quantity one() noexcept
    requires requires { quantity_values<Rep>::one(); }
  {
    return quantity(quantity_values<Rep>::one());
  }

  [[nodiscard]] static constexpr quantity min() noexcept
    requires requires { quantity_values<Rep>::min(); }
  {
    return quantity(quantity_values<Rep>::min());
  }

  [[nodiscard]] static constexpr quantity max() noexcept
    requires requires { quantity_values<Rep>::max(); }
  {
    return quantity(quantity_values<Rep>::max());
  }

  [[nodiscard]] constexpr quantity operator+() const { return *this; }

  [[nodiscard]] constexpr quantity operator-() const
    requires std::regular_invocable<std::negate<>, rep>
  {
    return quantity(-count());
  }

  constexpr quantity& operator++()
    requires requires(rep v) { { ++v } -> std::same_as<rep&>; }
  {
    ++value_;
    return *this;
  }

  [[nodiscard]] constexpr quantity operator++(int)
    requires requires(rep v) { { v++ } -> std::same_as<rep>; }
  {
    return quantity(value_++);
  }

  constexpr quantity& operator--()
    requires requires(rep v) { { --v } -> std::same_as<rep&>; }
  {
    --value_;
    return *this;
  }

  [[nodiscard]] constexpr quantity operator--(int)
    requires requires(rep v) { { v-- } -> std::same_as<rep>; }
  {
    return quantity(value_--);
  }

  template<typename Rep2>
    requires detail::safe_convertible<Rep2, rep>
  constexpr quantity& operator+=(const quantity<D, U, Rep2>& q)
  {
    value_ += q.count();
    return *this;
  }

  template<typename Rep2>
    requires detail::safe_convertible<Rep2, rep>
  constexpr quantity& operator-=(const quantity<D, U, Rep2>& q)
  {
    value_ -= q.count();
    return *this;
  }

  template<typename Rep2>
    requires detail::safe_convertible<Rep2, rep>
  constexpr quantity& operator*=(const Rep2& rhs)
  {
    value_ *= rhs;
    return *this;
  }

  template<typename Rep2>
    requires detail::safe_convertible<Rep2, rep>
  constexpr quantity& operator/=(const Rep2& rhs)
  {
    value_ /= rhs;
    return *this;
  }

  template<ScalableNumber Value>
    requires (!treat_as_floating_point<rep>) &&
             (!treat_as_floating_point<Value>)
  constexpr quantity& operator%=(const Value& rhs)
    requires requires(rep v1, Value v2) { { v1 %= v2 } -> std::same_as<rep&>; }
  {
    value_ %= rhs;
    return *this;
  }

  constexpr quantity& operator%=(const quantity& q)
    requires (!treat_as_floating_point<rep>) &&
             requires(rep v1, rep v2) { { v1 %= v2 } -> std::same_as<rep&>; }
  {
    value_ %= q.count();
    return *this;
  }

  // Hidden Friends
  // Below friend functions are to be found via argument-dependent lookup only

  [[nodiscard]] friend constexpr quantity operator+(const quantity& lhs, const quantity& rhs)
    requires std::regular_invocable<std::plus<>, Rep, Rep>
  {
    return quantity(lhs.count() + rhs.count());
  }

  template<typename D2, typename U2, typename Rep2>
    requires std::regular_invocable<std::plus<>, Rep, Rep2> && equivalent<D, D2>
  [[nodiscard]] friend constexpr Quantity auto operator+(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
  {
    using common_rep = decltype(lhs.count() + rhs.count());
    using ret = common_quantity<quantity, quantity<D2, U2, Rep2>, common_rep>;
    return ret(ret(lhs).count() + ret(rhs).count());
  }

  [[nodiscard]] friend constexpr quantity operator-(const quantity& lhs, const quantity& rhs)
    requires std::regular_invocable<std::minus<>, Rep, Rep>
  {
    return quantity(lhs.count() - rhs.count());
  }

  template<typename D2, typename U2, typename Rep2>
    requires std::regular_invocable<std::minus<>, Rep, Rep2> && equivalent<D, D2>
  [[nodiscard]] friend constexpr Quantity auto operator-(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
  {
    using common_rep = decltype(lhs.count() - rhs.count());
    using ret = common_quantity<quantity, quantity<D2, U2, Rep2>, common_rep>;
    return ret(ret(lhs).count() - ret(rhs).count());
  }

  template<ScalableNumber Value>
    requires std::regular_invocable<std::multiplies<>, Rep, Value>
  [[nodiscard]] friend constexpr Quantity auto operator*(const quantity& q, const Value& v)
  {
    using common_rep = decltype(q.count() * v);
    using ret = quantity<D, U, common_rep>;
    return ret(q.count() * v);
  }

  template<ScalableNumber Value>
    requires std::regular_invocable<std::multiplies<>, Value, Rep>
  [[nodiscard]] friend constexpr Quantity auto operator*(const Value& v, const quantity& q)
  {
    return q * v;
  }

  template<typename D2, typename U2, typename Rep2>
    requires std::regular_invocable<std::multiplies<>, Rep, Rep2>
  [[nodiscard]] friend constexpr Quantity auto operator*(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
  {
    using dim = dimension_multiply<D, D2>;
    using ret_unit = downcast_unit<dim, (U::ratio / dimension_unit<D>::ratio) * (U2::ratio / dimension_unit<D2>::ratio) * dimension_unit<dim>::ratio>;
    using common_rep = decltype(lhs.count() * rhs.count());
    using ret = quantity<dim, ret_unit, common_rep>;
    return ret(lhs.count() * rhs.count());
  }

  template<ScalableNumber Value>
    requires std::regular_invocable<std::divides<>, Value, Rep>
  [[nodiscard]] friend constexpr Quantity auto operator/(const Value& v, const quantity& q)
  {
    // Expects(q.count() != zero().count());

    using dim = dim_invert<D>;
    using ret_unit = downcast_unit<dim, ratio(U::ratio.den, U::ratio.num, -U::ratio.exp)>;
    using common_rep = decltype(v / q.count());
    using ret = quantity<dim, ret_unit, common_rep>;
    return ret(v / q.count());
  }

  template<ScalableNumber Value>
    requires std::regular_invocable<std::divides<>, Rep, Value>
  [[nodiscard]] friend constexpr Quantity auto operator/(const quantity& q, const Value& v)
  {
    // Expects(v != zero().count());

    using common_rep = decltype(q.count() / v);
    using ret = quantity<D, U, common_rep>;
    return ret(q.count() / v);
  }

  template<typename D2, typename U2, typename Rep2>
    requires std::regular_invocable<std::divides<>, Rep, Rep2>
  [[nodiscard]] friend constexpr Quantity auto operator/(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
  {
    // Expects(rhs.count() != zero().count());

    using common_rep = decltype(lhs.count() / rhs.count());
    using dim = dimension_divide<D, D2>;
    using ret_unit = downcast_unit<dim, (U::ratio / dimension_unit<D>::ratio) / (U2::ratio / dimension_unit<D2>::ratio) * dimension_unit<dim>::ratio>;
    using ret = quantity<dim, ret_unit, common_rep>;
    return ret(lhs.count() / rhs.count());
  }

  template<ScalableNumber Value>
    requires (!treat_as_floating_point<Rep>) &&
            (!treat_as_floating_point<Value>) &&
            std::regular_invocable<std::modulus<>, Rep, Value>
  [[nodiscard]] friend constexpr Quantity auto operator%(const quantity& q, const Value& v)
  {
    using common_rep = decltype(q.count() % v);
    using ret = quantity<D, U, common_rep>;
    return ret(q.count() % v);
  }

  template<typename U2, typename Rep2>
    requires (!treat_as_floating_point<Rep>) &&
            (!treat_as_floating_point<Rep2>) &&
            std::regular_invocable<std::modulus<>, Rep, Rep2>
  [[nodiscard]] friend constexpr Quantity auto operator%(const quantity& lhs, const quantity<D, U2, Rep2>& rhs)
  {
    using common_rep = decltype(lhs.count() % rhs.count());
    using ret = common_quantity<quantity, quantity<D, U2, Rep2>, common_rep>;
    return ret(ret(lhs).count() % ret(rhs).count());
  }

  template<typename D2, typename U2, typename Rep2>
    requires equivalent<D, D2> &&
             std::three_way_comparable_with<Rep, Rep2>
  [[nodiscard]] friend constexpr auto operator<=>(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
  {
    using cq = common_quantity<quantity, quantity<D2, U2, Rep2>>;
    return cq(lhs).count() <=> cq(rhs).count();
  }

  template<typename D2, typename U2, typename Rep2>
    requires equivalent<D, D2> &&
             std::equality_comparable_with<Rep, Rep2>
  [[nodiscard]] friend constexpr bool operator==(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
  {
    using cq = common_quantity<quantity, quantity<D2, U2, Rep2>>;
    return cq(lhs).count() == cq(rhs).count();
  }

  template<class CharT, class Traits>
  friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const quantity& q)
  {
    os << q.count();
    constexpr auto symbol = detail::unit_text<D, U>();
    if constexpr (symbol.standard().size()) {
      os << " " << symbol.standard();
    }
    return os;
  }
};

namespace detail {

template<typename D, typename U, typename Rep>
inline constexpr bool is_quantity<quantity<D, U, Rep>> = true;

}  // namespace detail

}  // namespace units
