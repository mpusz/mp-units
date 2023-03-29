
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

#include <units/bits/common_type.h>
#include <units/generic/dimensionless.h>

// IWYU pragma: begin_exports
#include <units/quantity_cast.h>
#include <units/ratio.h>
#include <compare>
// IWYU pragma: end_exports

#include <units/reference.h>
#include <utility>

namespace units {

namespace detail {

template<Reference auto R>  // TODO: Replace with `v * R` pending https://github.com/BobSteagall/wg21/issues/58.
inline constexpr auto make_quantity = [](auto&& v) {
  using Rep = std::remove_cvref_t<decltype(v)>;
  using ref_type = decltype(R);
  using dimension = TYPENAME ref_type::dimension;
  using unit = TYPENAME ref_type::unit;
  return quantity<dimension, unit, Rep>(std::forward<decltype(v)>(v));
};

template<typename T>
concept quantity_one =
  Quantity<T> &&
  (std::same_as<typename T::dimension, dim_one> || std::same_as<typename T::dimension, unknown_dimension<>>)&&detail::
    equivalent_unit<typename T::unit, typename T::dimension, ::units::one, typename T::dimension>::value;

}  // namespace detail

template<typename T>
concept floating_point_ =  // exposition only
  (Quantity<T> && treat_as_floating_point<typename T::rep>) || (!Quantity<T> && treat_as_floating_point<T>);

template<typename From, typename To>
concept safe_convertible_to_ =  // exposition only
  (!Quantity<From>)&&(!Quantity<To>)&&std::convertible_to<From, To> &&
  (floating_point_<To> || (!floating_point_<From>));

// QFrom ratio is an exact multiple of QTo
template<typename QFrom, typename QTo>
concept harmonic_ =  // exposition only
  Quantity<QFrom> && Quantity<QTo> && is_integral(detail::quantity_magnitude<QFrom> / detail::quantity_magnitude<QTo>);

template<typename QFrom, typename QTo>
concept safe_castable_to_ =  // exposition only
  Quantity<QFrom> && QuantityOf<QTo, typename QFrom::dimension> &&
  scalable_with_<typename QFrom::rep, typename QTo::rep> &&
  (floating_point_<QTo> || (!floating_point_<QFrom> && harmonic_<QFrom, QTo>));

template<typename Func, typename T, typename U>
concept quantity_value_for_ = std::regular_invocable<Func, T, U> && Representation<std::invoke_result_t<Func, T, U>>;

template<typename T, typename Func, typename U, typename V>
concept invoke_result_convertible_to_ =
  Representation<T> && quantity_value_for_<Func, U, V> && safe_convertible_to_<T, std::invoke_result_t<Func, U, V>>;

template<typename Func, typename Q, typename V>
concept have_quantity_for_ = Quantity<Q> && (!Quantity<V>)&&quantity_value_for_<Func, typename Q::rep, V>;

template<QuantityLike Q>
using quantity_like_type = quantity<typename quantity_like_traits<Q>::dimension, typename quantity_like_traits<Q>::unit,
                                    typename quantity_like_traits<Q>::rep>;

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
template<Dimension D, UnitOf<D> U, Representation Rep = double>
class quantity {
  Rep number_;
public:
  // member types and values
  using dimension = D;
  using unit = U;
  using rep = Rep;
  static constexpr units::reference<dimension, unit> reference{};

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

  template<typename Value>
    requires(safe_convertible_to_<std::remove_cvref_t<Value>, rep>)
  constexpr explicit(!detail::quantity_one<quantity>) quantity(Value&& v) : number_(std::forward<Value>(v))
  {
  }

  template<safe_castable_to_<quantity> Q>
  constexpr explicit(false) quantity(const Q& q) : number_(quantity_cast<quantity>(q).number())
  {
  }

  template<QuantityLike Q>
    requires(safe_castable_to_<quantity_like_type<Q>, quantity>)
  constexpr explicit quantity(const Q& q) : quantity(quantity_like_type<Q>(quantity_like_traits<Q>::number(q)))
  {
  }

  quantity& operator=(const quantity&) = default;
  quantity& operator=(quantity&&) = default;

  // data access
  [[nodiscard]] constexpr rep& number() & noexcept { return number_; }
  [[nodiscard]] constexpr const rep& number() const& noexcept { return number_; }
  [[nodiscard]] constexpr rep&& number() && noexcept { return std::move(number_); }
  [[nodiscard]] constexpr const rep&& number() const&& noexcept { return std::move(number_); }

  // member unary operators
  [[nodiscard]] constexpr Quantity auto operator+() const
    requires requires(rep v) {
      {
        +v
      } -> std::common_with<rep>;
    }
  {
    using ret = quantity<D, U, decltype(+number())>;
    return ret(+number());
  }

  [[nodiscard]] constexpr Quantity auto operator-() const
    requires(std::regular_invocable<std::negate<>, rep>)
  {
    using ret = quantity<D, U, decltype(-number())>;
    return ret(-number());
  }

  constexpr quantity& operator++()
    requires requires(rep v) {
      {
        ++v
      } -> std::same_as<rep&>;
    }
  {
    ++number_;
    return *this;
  }

  [[nodiscard]] constexpr quantity operator++(int)
    requires requires(rep v) {
      {
        v++
      } -> std::same_as<rep>;
    }
  {
    return quantity(number_++);
  }

  constexpr quantity& operator--()
    requires requires(rep v) {
      {
        --v
      } -> std::same_as<rep&>;
    }
  {
    --number_;
    return *this;
  }

  [[nodiscard]] constexpr quantity operator--(int)
    requires requires(rep v) {
      {
        v--
      } -> std::same_as<rep>;
    }
  {
    return quantity(number_--);
  }

  constexpr quantity& operator+=(const quantity& q)
    requires requires(rep a, rep b) {
      {
        a += b
      } -> std::same_as<rep&>;
    }
  {
    number_ += q.number();
    return *this;
  }

  constexpr quantity& operator-=(const quantity& q)
    requires requires(rep a, rep b) {
      {
        a -= b
      } -> std::same_as<rep&>;
    }
  {
    number_ -= q.number();
    return *this;
  }

  template<typename Rep2>
  constexpr quantity& operator*=(const Rep2& rhs)
    requires requires(rep a, const Rep2 b) {
      {
        a *= b
      } -> std::same_as<rep&>;
    }
  {
    number_ *= rhs;
    return *this;
  }
  template<detail::quantity_one Q>
  constexpr quantity& operator*=(const Q& rhs)
    requires requires(rep a, const typename Q::rep b) {
      {
        a *= b
      } -> std::same_as<rep&>;
    }
  {
    number_ *= rhs.number();
    return *this;
  }

  template<typename Rep2>
  constexpr quantity& operator/=(const Rep2& rhs)
    requires requires(rep a, const Rep2 b) {
      {
        a /= b
      } -> std::same_as<rep&>;
    }
  {
    gsl_ExpectsAudit(rhs != quantity_values<Rep2>::zero());
    number_ /= rhs;
    return *this;
  }
  template<detail::quantity_one Q>
  constexpr quantity& operator/=(const Q& rhs)
    requires requires(rep a, const typename Q::rep b) {
      {
        a /= b
      } -> std::same_as<rep&>;
    }
  {
    gsl_ExpectsAudit(rhs.number() != quantity_values<typename Q::rep>::zero());
    number_ /= rhs.number();
    return *this;
  }

  template<typename Rep2>
  constexpr quantity& operator%=(const Rep2& rhs)
    requires(!floating_point_<rep>) && (!floating_point_<Rep2>) && requires(rep a, const Rep2 b) {
      {
        a %= b
      } -> std::same_as<rep&>;
    }
  {
    gsl_ExpectsAudit(rhs != quantity_values<Rep2>::zero());
    number_ %= rhs;
    return *this;
  }

  template<detail::quantity_one Q>
  constexpr quantity& operator%=(const Q& rhs)
    requires(!floating_point_<rep>) && (!floating_point_<typename Q::rep>) && requires(rep a, const typename Q::rep b) {
      {
        a %= b
      } -> std::same_as<rep&>;
    }
  {
    gsl_ExpectsAudit(rhs.number() != quantity_values<typename Q::rep>::zero());
    number_ %= rhs.number();
    return *this;
  }

  constexpr quantity& operator%=(const quantity& q)
    requires(!floating_point_<rep>) && requires(rep a, rep b) {
      {
        a %= b
      } -> std::same_as<rep&>;
    }
  {
    gsl_ExpectsAudit(q.number() != quantity_values<rep>::zero());
    number_ %= q.number();
    return *this;
  }

  // Hidden Friends
  // Below friend functions are to be found via argument-dependent lookup only
  template<typename Value>
  [[nodiscard]] friend constexpr Quantity auto operator+(const quantity& lhs, const Value& rhs)
    requires requires {  // TODO: Simplify when Clang catches up.
      requires !Quantity<Value>;
      requires is_same_v<unit, units::one>;
      requires invoke_result_convertible_to_<rep, std::plus<>, rep, Value>;
    }
  {
    return units::quantity(lhs.number() + rhs);
  }
  template<typename Value>
  [[nodiscard]] friend constexpr Quantity auto operator+(const Value& lhs, const quantity& rhs)
    requires requires {  // TODO: Simplify when Clang catches up.
      requires !Quantity<Value>;
      requires is_same_v<unit, units::one>;
      requires invoke_result_convertible_to_<rep, std::plus<>, Value, rep>;
    }
  {
    return units::quantity(lhs + rhs.number());
  }

  template<typename Value>
  [[nodiscard]] friend constexpr Quantity auto operator-(const quantity& lhs, const Value& rhs)
    requires requires {  // TODO: Simplify when Clang catches up.
      requires !Quantity<Value>;
      requires is_same_v<unit, units::one>;
      requires invoke_result_convertible_to_<rep, std::minus<>, rep, Value>;
    }
  {
    return units::quantity(lhs.number() - rhs);
  }
  template<typename Value>
  [[nodiscard]] friend constexpr Quantity auto operator-(const Value& lhs, const quantity& rhs)
    requires requires {  // TODO: Simplify when Clang catches up.
      requires !Quantity<Value>;
      requires is_same_v<unit, units::one>;
      requires invoke_result_convertible_to_<rep, std::minus<>, Value, rep>;
    }
  {
    return units::quantity(lhs - rhs.number());
  }

  template<Representation Value>
    requires(invoke_result_convertible_to_<rep, std::multiplies<>, rep, const Value&>)
  [[nodiscard]] friend constexpr Quantity auto operator*(const quantity& q, const Value& v)
  {
    using ret = quantity<D, U, std::invoke_result_t<std::multiplies<>, rep, Value>>;
    return ret(q.number() * v);
  }

  template<Representation Value>
    requires(invoke_result_convertible_to_<rep, std::multiplies<>, const Value&, rep>)
  [[nodiscard]] friend constexpr Quantity auto operator*(const Value& v, const quantity& q)
  {
    using ret = quantity<D, U, std::invoke_result_t<std::multiplies<>, Value, rep>>;
    return ret(v * q.number());
  }

  template<Representation Value>
    requires(invoke_result_convertible_to_<rep, std::divides<>, rep, const Value&>)
  [[nodiscard]] friend constexpr Quantity auto operator/(const quantity& q, const Value& v)
  {
    gsl_ExpectsAudit(v != quantity_values<Value>::zero());
    using ret = quantity<D, U, std::invoke_result_t<std::divides<>, rep, Value>>;
    return ret(q.number() / v);
  }

  template<Representation Value>
    requires(invoke_result_convertible_to_<rep, std::divides<>, const Value&, rep>)
  [[nodiscard]] friend constexpr Quantity auto operator/(const Value& v, const quantity& q)
  {
    return detail::make_quantity<::units::reference<dim_one, ::units::one>{} / reference>(v / q.number());
  }

  template<typename Value>
    requires(!Quantity<Value>) && (!floating_point_<rep>) && (!floating_point_<Value>) &&
            (invoke_result_convertible_to_<rep, std::modulus<>, rep, const Value&>)
  [[nodiscard]] friend constexpr Quantity auto operator%(const quantity& q, const Value& v)
  {
    gsl_ExpectsAudit(v != quantity_values<Value>::zero());
    using ret = quantity<D, U, std::invoke_result_t<std::modulus<>, rep, Value>>;
    return ret(q.number() % v);
  }

  [[nodiscard]] friend constexpr Quantity auto operator%(const quantity& lhs, const quantity& rhs)
    requires(!floating_point_<rep>) && (invoke_result_convertible_to_<rep, std::modulus<>, rep, rep>)
  {
    gsl_ExpectsAudit(rhs.number() != quantity_values<rep>::zero());
    using ret = quantity<D, U, std::invoke_result_t<std::modulus<>, rep, rep>>;
    return ret(lhs.number() % rhs.number());
  }

  [[nodiscard]] friend constexpr auto operator<=>(const quantity& lhs, const quantity& rhs)
    requires std::three_way_comparable<rep>
#if UNITS_COMP_GCC == 10 && UNITS_COMP_GCC_MINOR >= 2
  = default;
#else
  {
    return lhs.number() <=> rhs.number();
  }
#endif

  [[nodiscard]] friend constexpr bool operator==(const quantity& lhs, const quantity& rhs) = default;
};

// CTAD
#if !UNITS_COMP_CLANG || UNITS_COMP_CLANG > 17
template<typename D, typename U, typename Rep>
explicit(false) quantity(Rep&&) -> quantity<D, U, Rep>;
#endif

template<typename D, typename U, typename Rep>
explicit(false) quantity(quantity<D, U, Rep>) -> quantity<D, U, Rep>;

template<Representation Rep>
explicit(false) quantity(Rep) -> quantity<dim_one, one, Rep>;

template<QuantityLike Q>
explicit quantity(Q) -> quantity<typename quantity_like_traits<Q>::dimension, typename quantity_like_traits<Q>::unit,
                                 typename quantity_like_traits<Q>::rep>;

// non-member binary operators
template<Quantity Q1, QuantityEquivalentTo<Q1> Q2>
  requires(quantity_value_for_<std::plus<>, typename Q1::rep, typename Q2::rep>)
[[nodiscard]] constexpr Quantity auto operator+(const Q1& lhs, const Q2& rhs)
{
  using ref = detail::common_quantity_reference<Q1, Q2>;
  using ret = quantity<typename ref::dimension, typename ref::unit, decltype(lhs.number() + rhs.number())>;
  return ret(ret(lhs).number() + ret(rhs).number());
}

template<Quantity Q1, QuantityEquivalentTo<Q1> Q2>
  requires(quantity_value_for_<std::minus<>, typename Q1::rep, typename Q2::rep>)
[[nodiscard]] constexpr Quantity auto operator-(const Q1& lhs, const Q2& rhs)
{
  using ref = detail::common_quantity_reference<Q1, Q2>;
  using ret = quantity<typename ref::dimension, typename ref::unit, decltype(lhs.number() - rhs.number())>;
  return ret(ret(lhs).number() - ret(rhs).number());
}

template<Quantity Q1, Quantity Q2>
  requires(quantity_value_for_<std::multiplies<>, typename Q1::rep, typename Q2::rep>)
[[nodiscard]] constexpr Quantity auto operator*(const Q1& lhs, const Q2& rhs)
{
  return detail::make_quantity<Q1::reference * Q2::reference>(lhs.number() * rhs.number());
}

template<Quantity Q1, Quantity Q2>
  requires(quantity_value_for_<std::divides<>, typename Q1::rep, typename Q2::rep>)
[[nodiscard]] constexpr Quantity auto operator/(const Q1& lhs, const Q2& rhs)
{
  gsl_ExpectsAudit(rhs.number() != quantity_values<typename Q2::rep>::zero());
  return detail::make_quantity<Q1::reference / Q2::reference>(lhs.number() / rhs.number());
}

template<Quantity Q1, Quantity Q2>
  requires(!floating_point_<typename Q1::rep>) && (!floating_point_<typename Q2::rep>) &&
          (QuantityEquivalentTo<Q2, Q1> || Dimensionless<Q2>) &&
          (quantity_value_for_<std::modulus<>, typename Q1::rep, typename Q2::rep>)
[[nodiscard]] constexpr Quantity auto operator%(const Q1& lhs, const Q2& rhs)
{
  gsl_ExpectsAudit(rhs.number() != quantity_values<typename Q2::rep>::zero());
  using ret = quantity<typename Q1::dimension, typename Q1::unit,
                       std::invoke_result_t<std::modulus<>, typename Q1::rep, typename Q2::rep>>;
  return ret(lhs.number() % rhs.number());
}

template<Quantity Q1, QuantityEquivalentTo<Q1> Q2>
  requires std::three_way_comparable_with<typename Q1::rep, typename Q2::rep>
[[nodiscard]] constexpr auto operator<=>(const Q1& lhs, const Q2& rhs)
{
  using cq = std::common_type_t<Q1, Q2>;
  return cq(lhs).number() <=> cq(rhs).number();
}

template<Quantity Q1, QuantityEquivalentTo<Q1> Q2>
  requires std::equality_comparable_with<typename Q1::rep, typename Q2::rep>
[[nodiscard]] constexpr bool operator==(const Q1& lhs, const Q2& rhs)
{
  using cq = std::common_type_t<Q1, Q2>;
  return cq(lhs).number() == cq(rhs).number();
}

// type traits
namespace detail {

template<typename D, typename U, typename Rep>
inline constexpr bool is_quantity<quantity<D, U, Rep>> = true;

template<typename T>
  requires units::is_derived_from_specialization_of<T, units::quantity>
inline constexpr bool is_quantity<T> = true;

}  // namespace detail

}  // namespace units
