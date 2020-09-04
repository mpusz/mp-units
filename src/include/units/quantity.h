
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
#include <units/bits/to_string.h>
#include <units/quantity_cast.h>

#if __GNUC__ >= 10
#include <compare>
#endif

#include <ostream>

namespace units {

namespace detail {

template<typename From, typename To>
concept safe_convertible = // exposition only
    std::convertible_to<From, To> &&
    (treat_as_floating_point<To> || (!treat_as_floating_point<From>));

template<typename Rep, typename UnitFrom, typename UnitTo>
concept safe_divisible = // exposition only
    treat_as_floating_point<Rep> ||
    is_integral(UnitFrom::ratio / UnitTo::ratio);

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
template<Dimension D, UnitOf<D> U, Scalar Rep = double>
class quantity {
  Rep value_{};

public:
  using dimension = D;
  using unit = U;
  using rep = Rep;

  quantity() = default;
  quantity(const quantity&) = default;
  quantity(quantity&&) = default;

  template<Scalar Value>
    requires detail::safe_convertible<Value, rep>
  constexpr explicit quantity(const Value& v) : value_{static_cast<rep>(v)} {}

  template<Quantity Q2>
    requires equivalent_dim<D, typename Q2::dimension> &&
             detail::safe_convertible<typename Q2::rep, rep> &&
             detail::safe_divisible<rep, typename Q2::unit, unit>
  constexpr quantity(const Q2& q) : value_{quantity_cast<quantity>(q).count()} {}

  quantity& operator=(const quantity&) = default;
  quantity& operator=(quantity&&) = default;

  [[nodiscard]] constexpr rep count() const noexcept { return value_; }

  template<typename T = Rep>
  [[nodiscard]] static constexpr quantity zero() noexcept
    requires requires { quantity_values<T>::zero(); }
  // requires requires { quantity_values<Rep>::zero(); }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    return quantity(quantity_values<Rep>::zero());
  }

  template<typename T = Rep>
  [[nodiscard]] static constexpr quantity one() noexcept
    requires requires { quantity_values<T>::one(); }
  // requires requires { quantity_values<Rep>::one(); }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    return quantity(quantity_values<Rep>::one());
  }

  template<typename T = Rep>
  [[nodiscard]] static constexpr quantity min() noexcept
    requires requires { quantity_values<T>::min(); }
  // requires requires { quantity_values<Rep>::min(); }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    return quantity(quantity_values<Rep>::min());
  }

  template<typename T = Rep>
  [[nodiscard]] static constexpr quantity max() noexcept
    requires requires { quantity_values<T>::max(); }
  // requires requires { quantity_values<Rep>::max(); }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    return quantity(quantity_values<Rep>::max());
  }

  [[nodiscard]] constexpr quantity operator+() const { return *this; }

  template<typename T = Rep>
  [[nodiscard]] constexpr quantity operator-() const
    requires std::regular_invocable<std::negate<>, T>
  // requires std::regular_invocable<std::negate<>, rep>  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    return quantity(-count());
  }

  template<typename T = Rep>
  constexpr quantity& operator++()
    requires requires(T v) { { ++v } -> SAME_AS(T&); }
  // requires requires(rep v) { { ++v } -> std::same_as<rep&>; }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    ++value_;
    return *this;
  }

  template<typename T = Rep>
  [[nodiscard]] constexpr quantity operator++(int)
    requires requires(T v) { { v++ } -> SAME_AS(T); }
  // requires requires(rep v) { { v++ } -> std::same_as<rep>; }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    return quantity(value_++);
  }

  template<typename T = Rep>
  constexpr quantity& operator--()
    requires requires(T v) { { --v } -> SAME_AS(T&); }
  // requires requires(rep v) { { --v } -> std::same_as<rep&>; }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    --value_;
    return *this;
  }

  template<typename T = Rep>
  [[nodiscard]] constexpr quantity operator--(int)
    requires requires(T v) { { v-- } -> SAME_AS(T); }
  // requires requires(rep v) { { v-- } -> std::same_as<rep>; }  // TODO gated by gcc-9 (fixed in gcc-10)
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

  template<Scalar Value, typename T = Rep>
  constexpr quantity& operator%=(const Value& rhs)
    requires (!treat_as_floating_point<rep>) &&
             (!treat_as_floating_point<Value>) &&
             requires(T v1, Value v2) { { v1 %= v2 } -> SAME_AS(T&); }
  //  requires(rep v1, Value v2) { { v1 %= v2 } -> SAME_AS(rep&); }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    value_ %= rhs;
    return *this;
  }

  template<typename T = Rep>
  constexpr quantity& operator%=(const quantity& q)
    requires (!treat_as_floating_point<rep>) &&
             requires(T v1, T v2) { { v1 %= v2 } -> SAME_AS(T&); }
  // requires(rep v1, rep v2) { { v1 %= v2 } -> std::same_as<rep&>; }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    value_ %= q.count();
    return *this;
  }

  // Hidden Friends
  // Below friend functions are to be found via argument-dependent lookup only
#if __GNUC__ >= 10

  template<typename D2, typename U2, typename Rep2>
  [[nodiscard]] friend constexpr auto operator<=>(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
    requires equivalent_dim<D, D2> &&
             std::three_way_comparable_with<Rep, Rep2>
  {
    using cq = common_quantity<quantity, quantity<D2, U2, Rep2>>;
    return cq(lhs).count() <=> cq(rhs).count();
  }

  template<typename D2, typename U2, typename Rep2>
  [[nodiscard]] friend constexpr bool operator==(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
    requires equivalent_dim<D, D2> &&
             std::equality_comparable_with<Rep, Rep2>
  {
    using cq = common_quantity<quantity, quantity<D2, U2, Rep2>>;
    return cq(lhs).count() == cq(rhs).count();
  }

#else

  template<typename D2, typename U2, typename Rep2>
  [[nodiscard]] friend constexpr bool operator==(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
    requires equivalent_dim<D, D2> &&
             std::equality_comparable_with<Rep, Rep2>
  {
    using cq = common_quantity<quantity, quantity<D2, U2, Rep2>>;
    return cq(lhs).count() == cq(rhs).count();
  }

  template<typename D2, typename U2, typename Rep2>
  [[nodiscard]] friend constexpr bool operator!=(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
    requires equivalent_dim<D, D2> &&
             std::equality_comparable_with<Rep, Rep2>
  {
    return !(lhs == rhs);
  }

  template<typename D2, typename U2, typename Rep2>
  [[nodiscard]] friend constexpr bool operator<(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
    requires equivalent_dim<D, D2> &&
             std::totally_ordered_with<Rep, Rep2>
  {
    using cq = common_quantity<quantity, quantity<D2, U2, Rep2>>;
    return cq(lhs).count() < cq(rhs).count();
  }

  template<typename D2, typename U2, typename Rep2>
  [[nodiscard]] friend constexpr bool operator<=(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
    requires equivalent_dim<D, D2> &&
             std::totally_ordered_with<Rep, Rep2>
  {
    return !(rhs < lhs);
  }

  template<typename D2, typename U2, typename Rep2>
  [[nodiscard]] friend constexpr bool operator>(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
    requires equivalent_dim<D, D2> &&
             std::totally_ordered_with<Rep, Rep2>
  {
    return rhs < lhs;
  }

  template<typename D2, typename U2, typename Rep2>
  [[nodiscard]] friend constexpr bool operator>=(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
    requires equivalent_dim<D, D2> &&
             std::totally_ordered_with<Rep, Rep2>
  {
    return !(lhs < rhs);
  }

#endif

  template<class CharT, class Traits>
  friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const quantity& q)
  {
    return os << detail::to_string<CharT, Traits>(q);
  }
};

template<typename D, typename U1, typename Rep1, typename U2, typename Rep2>
[[nodiscard]] constexpr Quantity AUTO operator+(const quantity<D, U1, Rep1>& lhs, const quantity<D, U2, Rep2>& rhs)
  requires std::regular_invocable<std::plus<>, Rep1, Rep2>
{
  using common_rep = decltype(lhs.count() + rhs.count());
  using ret = common_quantity<quantity<D, U1, Rep1>, quantity<D, U2, Rep2>, common_rep>;
  return ret(ret(lhs).count() + ret(rhs).count());
}

template<typename D, typename U1, typename Rep1, typename U2, typename Rep2>
[[nodiscard]] constexpr Quantity AUTO operator-(const quantity<D, U1, Rep1>& lhs, const quantity<D, U2, Rep2>& rhs)
  requires std::regular_invocable<std::minus<>, Rep1, Rep2>
{
  using common_rep = decltype(lhs.count() - rhs.count());
  using ret = common_quantity<quantity<D, U1, Rep1>, quantity<D, U2, Rep2>, common_rep>;
  return ret(ret(lhs).count() - ret(rhs).count());
}

template<typename D, typename U, typename Rep, Scalar Value>
[[nodiscard]] constexpr Quantity AUTO operator*(const quantity<D, U, Rep>& q, const Value& v)
  requires std::regular_invocable<std::multiplies<>, Rep, Value>
{
  using common_rep = decltype(q.count() * v);
  using ret = quantity<D, U, common_rep>;
  return ret(q.count() * v);
}

template<Scalar Value, typename D, typename U, typename Rep>
[[nodiscard]] constexpr Quantity AUTO operator*(const Value& v, const quantity<D, U, Rep>& q)
  requires std::regular_invocable<std::multiplies<>, Value, Rep>
{
  return q * v;
}

template<typename D1, typename U1, typename Rep1, typename D2, typename U2, typename Rep2>
[[nodiscard]] constexpr Scalar AUTO operator*(const quantity<D1, U1, Rep1>& lhs, const quantity<D2, U2, Rep2>& rhs)
  requires std::regular_invocable<std::multiplies<>, Rep1, Rep2> &&
           equivalent_dim<D1, dim_invert<D2>>
{
  using common_rep = decltype(lhs.count() * rhs.count());
  const ratio r = U1::ratio * U2::ratio;
    if constexpr (treat_as_floating_point<common_rep>) {
      return lhs.count() * rhs.count() * static_cast<common_rep>(r.num * fpow10<common_rep>(r.exp)) / static_cast<common_rep>(r.den);
    } else {
      return lhs.count() * rhs.count() * static_cast<common_rep>(r.num * ipow10(r.exp)) / static_cast<common_rep>(r.den);
    }
}

template<typename D1, typename U1, typename Rep1, typename D2, typename U2, typename Rep2>
[[nodiscard]] constexpr Quantity AUTO operator*(const quantity<D1, U1, Rep1>& lhs, const quantity<D2, U2, Rep2>& rhs)
  requires std::regular_invocable<std::multiplies<>, Rep1, Rep2>
{
  using dim = dimension_multiply<D1, D2>;
  using unit = downcast_unit<dim, (U1::ratio / dimension_unit<D1>::ratio) * (U2::ratio / dimension_unit<D2>::ratio) * dimension_unit<dim>::ratio>;
  using common_rep = decltype(lhs.count() * rhs.count());
  using ret = quantity<dim, unit, common_rep>;
  return ret(lhs.count() * rhs.count());
}

template<Scalar Value, typename D, typename U, typename Rep>
[[nodiscard]] constexpr Quantity AUTO operator/(const Value& v, const quantity<D, U, Rep>& q)
  requires std::regular_invocable<std::divides<>, Value, Rep>
{
  Expects(q.count() != 0);

  using dim = dim_invert<D>;
  using unit = downcast_unit<dim, ratio(U::ratio.den, U::ratio.num, -U::ratio.exp)>;
  using common_rep = decltype(v / q.count());
  using ret = quantity<dim, unit, common_rep>;
  return ret(v / q.count());
}

template<typename D, typename U, typename Rep, Scalar Value>
[[nodiscard]] constexpr Quantity AUTO operator/(const quantity<D, U, Rep>& q, const Value& v)
  requires std::regular_invocable<std::divides<>, Rep, Value>
{
  Expects(v != Value{0});

  using common_rep = decltype(q.count() / v);
  using ret = quantity<D, U, common_rep>;
  return ret(q.count() / v);
}

template<typename D1, typename U1, typename Rep1, typename D2, typename U2, typename Rep2>
[[nodiscard]] constexpr Scalar AUTO operator/(const quantity<D1, U1, Rep1>& lhs, const quantity<D2, U2, Rep2>& rhs)
  requires std::regular_invocable<std::divides<>, Rep1, Rep2> &&
           equivalent_dim<D1, D2>
{
  Expects(rhs.count() != 0);

  using common_rep = decltype(lhs.count() / rhs.count());
  using cq = common_quantity<quantity<D1, U1, Rep1>, quantity<D2, U2, Rep2>, common_rep>;
  return cq(lhs).count() / cq(rhs).count();
}

template<typename D1, typename U1, typename Rep1, typename D2, typename U2, typename Rep2>
[[nodiscard]] constexpr Quantity AUTO operator/(const quantity<D1, U1, Rep1>& lhs, const quantity<D2, U2, Rep2>& rhs)
  requires std::regular_invocable<std::divides<>, Rep1, Rep2>
{
  Expects(rhs.count() != 0);

  using common_rep = decltype(lhs.count() / rhs.count());
  using dim = dimension_divide<D1, D2>;
  using unit = downcast_unit<dim, (U1::ratio / dimension_unit<D1>::ratio) / (U2::ratio / dimension_unit<D2>::ratio) * dimension_unit<dim>::ratio>;
  using ret = quantity<dim, unit, common_rep>;
  return ret(lhs.count() / rhs.count());
}

template<typename D, typename U, typename Rep, Scalar Value>
[[nodiscard]] constexpr Quantity AUTO operator%(const quantity<D, U, Rep>& q, const Value& v)
  requires (!treat_as_floating_point<Rep>) &&
           (!treat_as_floating_point<Value>) &&
           std::regular_invocable<std::modulus<>, Rep, Value>
{
  using common_rep = decltype(q.count() % v);
  using ret = quantity<D, U, common_rep>;
  return ret(q.count() % v);
}

template<typename D, typename U1, typename Rep1, typename U2, typename Rep2>
[[nodiscard]] constexpr Quantity AUTO operator%(const quantity<D, U1, Rep1>& lhs, const quantity<D, U2, Rep2>& rhs)
  requires (!treat_as_floating_point<Rep1>) &&
           (!treat_as_floating_point<Rep2>) &&
           std::regular_invocable<std::modulus<>, Rep1, Rep2>
{
  using common_rep = decltype(lhs.count() % rhs.count());
  using ret = common_quantity<quantity<D, U1, Rep1>, quantity<D, U2, Rep2>, common_rep>;
  return ret(ret(lhs).count() % ret(rhs).count());
}

namespace detail {

template<typename D, typename U, typename Rep>
inline constexpr bool is_quantity<quantity<D, U, Rep>> = true;

}  // namespace detail

}  // namespace units
