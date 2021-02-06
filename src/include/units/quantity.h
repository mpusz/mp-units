
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

namespace units {

template<typename T>
concept floating_point_ = // exposition only
  (Quantity<T> && treat_as_floating_point<typename T::rep>) ||
  (!Quantity<T> && treat_as_floating_point<T>);

template<typename From, typename To>
concept safe_convertible_to_ = // exposition only
    (!Quantity<From>) &&
    (!Quantity<To>) &&
    std::convertible_to<From, To> &&
    (floating_point_<To> || (!floating_point_<From>));

// QFrom ratio is an exact multiple of QTo
template<typename QFrom, typename QTo>
concept harmonic_ = // exposition only
    Quantity<QFrom> &&
    Quantity<QTo> &&
    requires(QFrom from, QTo to) { requires is_integral(detail::quantity_ratio(from) / detail::quantity_ratio(to)); };

template<typename QFrom, typename QTo>
concept safe_castable_to_ = // exposition only
    Quantity<QFrom> &&
    QuantityOf<QTo, typename QFrom::dimension> &&
    scalable_with_<typename QFrom::rep, typename QTo::rep> &&
    (floating_point_<QTo> || (!floating_point_<QFrom> && harmonic_<QFrom, QTo>));

template<typename Func, typename T, typename U>
concept quantity_value_for_ =
  std::regular_invocable<Func, T, U> &&
  QuantityValue<std::invoke_result_t<Func, T, U>>;

template<typename T, typename Func, typename U, typename V>
concept invoke_result_convertible_to_ =
  QuantityValue<T> &&
  quantity_value_for_<Func, U, V> &&
  safe_convertible_to_<T, std::invoke_result_t<Func, U, V>>;

template<typename Func, typename Q, typename V>
concept have_quantity_for_ =
  Quantity<Q> &&
  (!Quantity<V>) &&
  quantity_value_for_<Func, typename Q::rep, V>;

template<typename Func, Quantity Q1, QuantityEquivalentTo<Q1> Q2>
  requires quantity_value_for_<Func, typename Q1::rep, typename Q2::rep>
using common_quantity_for = common_quantity<Q1, Q2, std::invoke_result_t<Func, typename Q1::rep, typename Q2::rep>>;

template<QuantityLike Q>
using quantity_like_type = quantity<typename quantity_like_traits<Q>::dimension, typename quantity_like_traits<Q>::unit, typename quantity_like_traits<Q>::rep>;

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
template<Dimension D, UnitOf<D> U, QuantityValue Rep = double>
class quantity {
  Rep value_;
public:
  // member types
  using dimension = D;
  using unit = U;
  using rep = Rep;

  // static member functions
  [[nodiscard]] static constexpr quantity zero() noexcept
    requires requires { quantity_values<rep>::zero(); }
  {
    return quantity(quantity_values<rep>::zero());
  }

  [[nodiscard]] static constexpr quantity one() noexcept
    requires requires { quantity_values<rep>::one(); }
  {
    return quantity(quantity_values<rep>::one());
  }

  [[nodiscard]] static constexpr quantity min() noexcept
    requires requires { quantity_values<rep>::min(); }
  {
    return quantity(quantity_values<rep>::min());
  }

  [[nodiscard]] static constexpr quantity max() noexcept
    requires requires { quantity_values<rep>::max(); }
  {
    return quantity(quantity_values<rep>::max());
  }

  // construction, assignment, destruction
  quantity() = default;
  quantity(const quantity&) = default;
  quantity(quantity&&) = default;

  template<safe_convertible_to_<rep> Value>
  explicit(!(is_same_v<dimension, dim_one> && is_same_v<unit, ::units::one>))
  constexpr quantity(const Value& v) : value_(v) {}

  template<safe_castable_to_<quantity> Q>
  constexpr quantity(const Q& q) : value_(quantity_cast<quantity>(q).count()) {}

  template<QuantityLike Q>
    requires safe_castable_to_<quantity_like_type<Q>, quantity>
  explicit constexpr quantity(const Q& q) : quantity(quantity_like_type<Q>(quantity_like_traits<Q>::count(q))) {}

  quantity& operator=(const quantity&) = default;
  quantity& operator=(quantity&&) = default;

  // data access
  [[nodiscard]] constexpr rep count() const noexcept { return value_; }

  // member unary operators
  [[nodiscard]] constexpr quantity operator+() const
    requires requires(rep v) { { +v } -> std::same_as<rep>; }
  {
    return *this;
  }

  [[nodiscard]] constexpr Quantity auto operator-() const
    requires std::regular_invocable<std::negate<>, rep>
  {
    using ret = quantity<D, U, decltype(-count())>;
    return ret(-count());
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

  constexpr quantity& operator+=(const quantity& q)
    requires requires(rep a, rep b) { { a += b } -> std::same_as<rep&>; }
  {
    value_ += q.count();
    return *this;
  }

  constexpr quantity& operator-=(const quantity& q)
    requires requires(rep a, rep b) { { a -= b } -> std::same_as<rep&>; }
  {
    value_ -= q.count();
    return *this;
  }

  template<typename Rep2>
  constexpr quantity& operator*=(const Rep2& rhs)
    requires requires(rep a, const Rep2 b) { { a *= b } -> std::same_as<rep&>; }
  {
    value_ *= rhs;
    return *this;
  }

  template<typename Rep2>
  constexpr quantity& operator/=(const Rep2& rhs)
    requires requires(rep a, const Rep2 b) { { a /= b } -> std::same_as<rep&>; }
  {
    value_ /= rhs;
    return *this;
  }

  constexpr quantity& operator%=(const rep& rhs)
    requires (!floating_point_<rep>) &&
             requires(rep a, const rep b) { { a %= b } -> std::same_as<rep&>; }
  {
    value_ %= rhs;
    return *this;
  }

  constexpr quantity& operator%=(const quantity& q)
    requires (!floating_point_<rep>) &&
             requires(rep a, rep b) { { a %= b } -> std::same_as<rep&>; }
  {
    value_ %= q.count();
    return *this;
  }

  // Hidden Friends
  // Below friend functions are to be found via argument-dependent lookup only
  [[nodiscard]] friend constexpr quantity operator+(const quantity& lhs, const quantity& rhs)
    requires invoke_result_convertible_to_<rep, std::plus<>, rep, rep>
  {
    return quantity(lhs.count() + rhs.count());
  }

  [[nodiscard]] friend constexpr quantity operator-(const quantity& lhs, const quantity& rhs)
    requires invoke_result_convertible_to_<rep, std::minus<>, rep, rep>
  {
    return quantity(lhs.count() - rhs.count());
  }

  template<typename Value>
    requires (!Quantity<Value>) &&
          invoke_result_convertible_to_<rep, std::multiplies<>, rep, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator*(const quantity& q, const Value& v)
  {
    using ret = quantity<D, U, std::invoke_result_t<std::multiplies<>, rep, Value>>;
    return ret(q.count() * v);
  }

  template<typename Value>
    requires (!Quantity<Value>) &&
          invoke_result_convertible_to_<rep, std::multiplies<>, const Value&, rep>
  [[nodiscard]] friend constexpr Quantity auto operator*(const Value& v, const quantity& q)
  {
    using ret = quantity<D, U, std::invoke_result_t<std::multiplies<>, Value, rep>>;
    return ret(v * q.count());
  }

  template<typename Value>
    requires (!Quantity<Value>) &&
          invoke_result_convertible_to_<rep, std::divides<>, rep, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator/(const quantity& q, const Value& v)
  {
    gsl_ExpectsAudit(v != quantity_values<Value>::zero());
    using ret = quantity<D, U, std::invoke_result_t<std::divides<>, rep, Value>>;
    return ret(q.count() / v);
  }

  template<typename Value>
    requires (!Quantity<Value>) &&
          invoke_result_convertible_to_<rep, std::divides<>, const Value&, rep>
  [[nodiscard]] friend constexpr Quantity auto operator/(const Value& v, const quantity& q)
  {
    gsl_ExpectsAudit(q.count() != quantity_values<rep>::zero());
    using dim = dim_invert<D>;
    using ret_unit = downcast_unit<dim, inverse(U::ratio)>;
    using ret = quantity<dim, ret_unit, std::invoke_result_t<std::divides<>, Value, rep>>;
    return ret(v / q.count());
  }

  template<typename Value>
    requires (!Quantity<Value>) && (!floating_point_<rep>) && (!floating_point_<Value>) &&
            invoke_result_convertible_to_<rep, std::modulus<>, rep, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator%(const quantity& q, const Value& v)
  {
    using ret = quantity<D, U, std::invoke_result_t<std::modulus<>, rep, Value>>;
    return ret(q.count() % v);
  }

  [[nodiscard]] friend constexpr quantity operator%(const quantity& lhs, const quantity& rhs)
    requires (!floating_point_<rep>) &&
            invoke_result_convertible_to_<rep, std::modulus<>, rep, rep>
  {
    return quantity(lhs.count() % rhs.count());
  }

  [[nodiscard]] friend constexpr auto operator<=>(const quantity& lhs, const quantity& rhs)
    requires std::three_way_comparable<rep>
#if COMP_GCC == 10 && COMP_GCC_MINOR >= 2
  = default;
#else
  {
    return lhs.count() <=> rhs.count();
  }
#endif

  [[nodiscard]] friend constexpr bool operator==(const quantity& lhs, const quantity& rhs) = default;
};

// CTAD
template<QuantityValue V>
explicit(false) quantity(V) -> quantity<dim_one, one, V>;

template<QuantityLike Q>
explicit quantity(Q) -> quantity_like_type<Q>;

// non-member binary operators
template<Quantity Q1, QuantityEquivalentTo<Q1> Q2>
  requires quantity_value_for_<std::plus<>, typename Q1::rep, typename Q2::rep>
[[nodiscard]] constexpr Quantity auto operator+(const Q1& lhs, const Q2& rhs)
{
  using ret = common_quantity_for<std::plus<>, Q1, Q2>;
  return ret(ret(lhs).count() + ret(rhs).count());
}

template<Quantity Q1, QuantityEquivalentTo<Q1> Q2>
  requires quantity_value_for_<std::minus<>, typename Q1::rep, typename Q2::rep>
[[nodiscard]] constexpr Quantity auto operator-(const Q1& lhs, const Q2& rhs)
{
  using ret = common_quantity_for<std::minus<>, Q1, Q2>;
  return ret(ret(lhs).count() - ret(rhs).count());
}

template<typename D1, typename U1, typename Rep1, typename D2, typename U2, typename Rep2>
  requires quantity_value_for_<std::multiplies<>, Rep1, Rep2>
[[nodiscard]] constexpr Quantity auto operator*(const quantity<D1, U1, Rep1>& lhs, const quantity<D2, U2, Rep2>& rhs)
{
  using dim = dimension_multiply<D1, D2>;
  using unit = downcast_unit<dim, (U1::ratio / dimension_unit<D1>::ratio) * (U2::ratio / dimension_unit<D2>::ratio) * dimension_unit<dim>::ratio>;
  using ret = quantity<dim, unit, std::invoke_result_t<std::multiplies<>, Rep1, Rep2>>;
  return ret(lhs.count() * rhs.count());
}

template<typename D1, typename U1, typename Rep1, typename D2, typename U2, typename Rep2>
  requires quantity_value_for_<std::divides<>, Rep1, Rep2>
[[nodiscard]] constexpr Quantity auto operator/(const quantity<D1, U1, Rep1>& lhs, const quantity<D2, U2, Rep2>& rhs)
{
  gsl_ExpectsAudit(rhs.count() != quantity_values<Rep2>::zero());
  using dim = dimension_divide<D1, D2>;
  using unit = downcast_unit<dim, (U1::ratio / dimension_unit<D1>::ratio) / (U2::ratio / dimension_unit<D2>::ratio) * dimension_unit<dim>::ratio>;
  using ret = quantity<dim, unit, std::invoke_result_t<std::divides<>, Rep1, Rep2>>;
  return ret(lhs.count() / rhs.count());
}

template<typename D1, typename U1, typename Rep1, typename U2, typename Rep2>
  requires (!floating_point_<Rep1>) && (!floating_point_<Rep2>) &&
          quantity_value_for_<std::modulus<>, Rep1, Rep2>
[[nodiscard]] constexpr Quantity auto operator%(const quantity<D1, U1, Rep1>& lhs, const quantity<dim_one, U2, Rep2>& rhs)
{
  using unit = downcast_unit<D1, U1::ratio * U2::ratio>;
  using ret = quantity<D1, unit, std::invoke_result_t<std::modulus<>, Rep1, Rep2>>;
  return ret(lhs.count() % rhs.count());
}

template<Quantity Q1, QuantityEquivalentTo<Q1> Q2>
  requires (!floating_point_<typename Q1::rep>) && (!floating_point_<typename Q2::rep>) &&
          quantity_value_for_<std::modulus<>, typename Q1::rep, typename Q2::rep>
[[nodiscard]] constexpr Quantity auto operator%(const Q1& lhs, const Q2& rhs)
{
  using ret = common_quantity_for<std::modulus<>, Q1, Q2>;
  return ret(ret(lhs).count() % ret(rhs).count());
}

template<Quantity Q1, QuantityEquivalentTo<Q1> Q2>
  requires std::three_way_comparable_with<typename Q1::rep, typename Q2::rep>
[[nodiscard]] constexpr auto operator<=>(const Q1& lhs, const Q2& rhs)
{
  using cq = common_quantity<Q1, Q2>;
  return cq(lhs).count() <=> cq(rhs).count();
}

template<Quantity Q1, QuantityEquivalentTo<Q1> Q2>
  requires std::equality_comparable_with<typename Q1::rep, typename Q2::rep>
[[nodiscard]] constexpr bool operator==(const Q1& lhs, const Q2& rhs)
{
  using cq = common_quantity<Q1, Q2>;
  return cq(lhs).count() == cq(rhs).count();
}

// type traits
namespace detail {

template<typename D, typename U, typename Rep>
inline constexpr bool is_quantity<quantity<D, U, Rep>> = true;

}  // namespace detail

}  // namespace units
