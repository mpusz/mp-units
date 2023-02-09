
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

#include <mp_units/bits/dimension_concepts.h>
#include <mp_units/bits/quantity_concepts.h>
#include <mp_units/bits/quantity_spec_concepts.h>
#include <mp_units/bits/reference_concepts.h>
#include <mp_units/bits/representation_concepts.h>
#include <mp_units/bits/sudo_cast.h>
#include <mp_units/bits/unit_concepts.h>
#include <mp_units/customization_points.h>
#include <mp_units/reference.h>
#include <compare>
#include <utility>

// the below is not used in this header but should be exposed with it
#include <mp_units/bits/quantity_cast.h>
#include <mp_units/bits/value_cast.h>

namespace mp_units {

namespace detail {

template<typename T>
concept QuantityOne = QuantityOf<T, dimensionless[one]>;

template<QuantityLike Q>
using quantity_like_type = quantity<quantity_like_traits<Q>::reference, typename quantity_like_traits<Q>::rep>;

template<typename T, typename Arg>
concept RepSafeConstructibleFrom =  // exposition only
  std::constructible_from<T, Arg> && (treat_as_floating_point<T> || !treat_as_floating_point<Arg>);

// QFrom ratio is an exact multiple of QTo
template<typename QFrom, typename QTo>
concept Harmonic =  // exposition only
  Quantity<QFrom> && Quantity<QTo> &&
  is_integral(get_canonical_unit(QFrom::unit).mag / get_canonical_unit(QTo::unit).mag);

template<typename QFrom, typename QTo>
concept QuantityConvertibleTo =  // exposition only
  Quantity<QFrom> && Quantity<QTo> && requires(QFrom q) { detail::sudo_cast<QTo>(q); } &&
  (treat_as_floating_point<typename QTo::rep> ||
   (!treat_as_floating_point<typename QFrom::rep> && Harmonic<QFrom, QTo>));

template<quantity_character Ch, typename Func, typename T, typename U>
concept InvokeResultOf = std::regular_invocable<Func, T, U> && RepresentationOf<std::invoke_result_t<Func, T, U>, Ch>;

}  // namespace detail

/**
 * @brief A quantity
 *
 * Property of a phenomenon, body, or substance, where the property has a magnitude that can be
 * expressed by means of a number and a reference.
 *
 * @tparam R a reference of the quantity providing all information about quantity properties
 * @tparam Rep a type to be used to represent values of a quantity
 */
template<Reference auto R, RepresentationOf<get_quantity_spec(R).character> Rep = double>
class quantity {
public:
  Rep number_;  // needs to be public for a structural type

  // member types and values
  static constexpr Reference auto reference = R;
  static constexpr QuantitySpec auto quantity_spec = get_quantity_spec(reference);
  static constexpr Dimension auto dimension = quantity_spec.dimension;
  static constexpr Unit auto unit = get_unit(reference);
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

  template<typename Value>
    requires detail::RepSafeConstructibleFrom<rep, Value>
  constexpr explicit(!detail::QuantityOne<quantity>) quantity(Value&& v) : number_(std::forward<Value>(v))
  {
  }

  template<detail::QuantityConvertibleTo<quantity> Q>
  constexpr explicit(false) quantity(const Q& q) : number_(detail::sudo_cast<quantity>(q).number())
  {
  }

  template<QuantityLike Q>
    requires detail::QuantityConvertibleTo<detail::quantity_like_type<Q>, quantity>
  constexpr explicit quantity(const Q& q) : quantity(detail::quantity_like_type<Q>(quantity_like_traits<Q>::number(q)))
  {
  }

  quantity& operator=(const quantity&) = default;
  quantity& operator=(quantity&&) = default;

  // data access
  [[nodiscard]] constexpr const rep& number() const& noexcept { return number_; }
  [[nodiscard]] constexpr rep&& number() && noexcept { return std::move(number_); }
  [[nodiscard]] constexpr const rep&& number() const&& noexcept { return std::move(number_); }

  template<Unit U>
    requires requires(quantity q) { q[U{}]; }
  [[nodiscard]] constexpr rep number_in(U) const noexcept
  {
    return (*this)[U{}].number();
  }

  template<Unit U>
    requires detail::QuantityConvertibleTo<quantity, quantity<::mp_units::reference<quantity_spec, U{}>{}, Rep>>
  [[nodiscard]] constexpr quantity<::mp_units::reference<quantity_spec, U{}>{}, Rep> operator[](U) const
  {
    return quantity<::mp_units::reference<quantity_spec, U{}>{}, Rep>{*this};
  }

  // member unary operators
  [[nodiscard]] constexpr Quantity auto operator+() const
    requires requires(rep v) {
      {
        +v
      } -> std::common_with<rep>;
    }
  {
    using ret = quantity<R, decltype(+number())>;
    return ret(+number());
  }

  [[nodiscard]] constexpr Quantity auto operator-() const
    requires std::regular_invocable<std::negate<>, rep>
  {
    using ret = quantity<R, decltype(-number())>;
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

  template<detail::QuantityOne Q>
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

  template<detail::QuantityOne Q>
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
    requires(!treat_as_floating_point<rep>) && (!treat_as_floating_point<Rep2>) && requires(rep a, const Rep2 b) {
      {
        a %= b
      } -> std::same_as<rep&>;
    }
  {
    gsl_ExpectsAudit(rhs != quantity_values<Rep2>::zero());
    number_ %= rhs;
    return *this;
  }

  template<detail::QuantityOne Q>
  constexpr quantity& operator%=(const Q& rhs)
    requires(!treat_as_floating_point<rep>) && (!treat_as_floating_point<typename Q::rep>) &&
            requires(rep a, const typename Q::rep b) {
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
    requires(!treat_as_floating_point<rep>) && requires(rep a, rep b) {
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
      requires unit == ::mp_units::one;
      requires detail::InvokeResultOf<quantity_spec.character, std::plus<>, rep, Value>;
    }
  {
    return ::mp_units::quantity(lhs.number() + rhs);
  }

  template<typename Value>
  [[nodiscard]] friend constexpr Quantity auto operator+(const Value& lhs, const quantity& rhs)
    requires requires {  // TODO: Simplify when Clang catches up.
      requires !Quantity<Value>;
      requires unit == ::mp_units::one;
      requires detail::InvokeResultOf<quantity_spec.character, std::plus<>, Value, rep>;
    }
  {
    return ::mp_units::quantity(lhs + rhs.number());
  }

  template<typename Value>
  [[nodiscard]] friend constexpr Quantity auto operator-(const quantity& lhs, const Value& rhs)
    requires requires {  // TODO: Simplify when Clang catches up.
      requires !Quantity<Value>;
      requires unit == ::mp_units::one;
      requires detail::InvokeResultOf<quantity_spec.character, std::minus<>, rep, Value>;
    }
  {
    return ::mp_units::quantity(lhs.number() - rhs);
  }

  template<typename Value>
  [[nodiscard]] friend constexpr Quantity auto operator-(const Value& lhs, const quantity& rhs)
    requires requires {  // TODO: Simplify when Clang catches up.
      requires !Quantity<Value>;
      requires unit == ::mp_units::one;
      requires detail::InvokeResultOf<quantity_spec.character, std::minus<>, Value, rep>;
    }
  {
    return ::mp_units::quantity(lhs - rhs.number());
  }

  template<Representation Value>
    requires detail::InvokeResultOf<quantity_spec.character, std::multiplies<>, rep, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator*(const quantity& q, const Value& v)
  {
    using ret = quantity<R, std::invoke_result_t<std::multiplies<>, rep, Value>>;
    return ret(q.number() * v);
  }

  template<Representation Value>
    requires detail::InvokeResultOf<quantity_spec.character, std::multiplies<>, const Value&, rep>
  [[nodiscard]] friend constexpr Quantity auto operator*(const Value& v, const quantity& q)
  {
    using ret = quantity<R, std::invoke_result_t<std::multiplies<>, Value, rep>>;
    return ret(v * q.number());
  }

  template<typename Value>
    requires(!Quantity<Value>) && detail::InvokeResultOf<quantity_spec.character, std::divides<>, rep, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator/(const quantity& q, const Value& v)
  {
    gsl_ExpectsAudit(v != quantity_values<Value>::zero());
    using ret = quantity<R, std::invoke_result_t<std::divides<>, rep, Value>>;
    return ret(q.number() / v);
  }

  template<typename Value>
    requires(!Quantity<Value>) && detail::InvokeResultOf<quantity_spec.character, std::divides<>, const Value&, rep>
  [[nodiscard]] friend constexpr Quantity auto operator/(const Value& v, const quantity& q)
  {
    return v / q.number() * (dimensionless[::mp_units::one] / reference);
  }

  template<typename Value>
    requires(!Quantity<Value>) && (!treat_as_floating_point<rep>) && (!treat_as_floating_point<Value>) &&
            detail::InvokeResultOf<quantity_spec.character, std::modulus<>, rep, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator%(const quantity& q, const Value& v)
  {
    gsl_ExpectsAudit(v != quantity_values<Value>::zero());
    using ret = quantity<R, std::invoke_result_t<std::modulus<>, rep, Value>>;
    return ret(q.number() % v);
  }

  [[nodiscard]] friend constexpr Quantity auto operator%(const quantity& lhs, const quantity& rhs)
    requires(!treat_as_floating_point<rep>) && detail::InvokeResultOf<quantity_spec.character, std::modulus<>, rep, rep>
  {
    gsl_ExpectsAudit(rhs.number() != quantity_values<rep>::zero());
    using ret = quantity<R, std::invoke_result_t<std::modulus<>, rep, rep>>;
    return ret(lhs.number() % rhs.number());
  }

  [[nodiscard]] friend constexpr auto operator<=>(const quantity& lhs, const quantity& rhs)
    requires std::three_way_comparable<rep>
#if __cpp_impl_three_way_comparison
  = default;
#else
  {
    return lhs.number() <=> rhs.number();
  }
#endif

  [[nodiscard]] friend constexpr bool operator==(const quantity& lhs, const quantity& rhs) = default;
};

// CTAD
template<auto R, typename Rep>
explicit(false) quantity(quantity<R, Rep>) -> quantity<R, Rep>;

template<RepresentationOf<mp_units::quantity_character::scalar> Rep>
explicit(false) quantity(Rep) -> quantity<dimensionless[one], Rep>;

#if !UNITS_COMP_CLANG || UNITS_COMP_CLANG > 16
template<auto R, RepresentationOf<R.quantity_spec.character> Rep>
explicit(false) quantity(Rep&&) -> quantity<R, Rep>;
#endif

template<QuantityLike Q>
explicit quantity(Q) -> quantity<quantity_like_traits<Q>::reference, typename quantity_like_traits<Q>::rep>;

// non-member binary operators
template<Quantity Q1, Quantity Q2>
  requires(get_kind(Q1::quantity_spec) == get_kind(Q2::quantity_spec)) &&
          detail::InvokeResultOf<common_quantity_spec(Q1::quantity_spec, Q2::quantity_spec).character, std::plus<>,
                                 typename Q1::rep, typename Q2::rep>
[[nodiscard]] constexpr Quantity auto operator+(const Q1& lhs, const Q2& rhs)
{
  constexpr auto ref = common_reference(Q1::reference, Q2::reference);
  using ret = quantity<ref, decltype(lhs.number() + rhs.number())>;
  return ret(ret(lhs).number() + ret(rhs).number());
}

template<Quantity Q1, Quantity Q2>
  requires(get_kind(Q1::quantity_spec) == get_kind(Q2::quantity_spec)) &&
          detail::InvokeResultOf<common_quantity_spec(Q1::quantity_spec, Q2::quantity_spec).character, std::minus<>,
                                 typename Q1::rep, typename Q2::rep>
[[nodiscard]] constexpr Quantity auto operator-(const Q1& lhs, const Q2& rhs)
{
  constexpr auto ref = common_reference(Q1::reference, Q2::reference);
  using ret = quantity<ref, decltype(lhs.number() - rhs.number())>;
  return ret(ret(lhs).number() - ret(rhs).number());
}

template<Quantity Q1, Quantity Q2>
  requires detail::InvokeResultOf<(Q1::quantity_spec * Q2::quantity_spec).character, std::multiplies<>,
                                  typename Q1::rep, typename Q2::rep>
[[nodiscard]] constexpr Quantity auto operator*(const Q1& lhs, const Q2& rhs)
{
  return lhs.number() * rhs.number() * (Q1::reference * Q2::reference);
}

template<Quantity Q1, Quantity Q2>
  requires detail::InvokeResultOf<(Q1::quantity_spec / Q2::quantity_spec).character, std::divides<>, typename Q1::rep,
                                  typename Q2::rep>
[[nodiscard]] constexpr Quantity auto operator/(const Q1& lhs, const Q2& rhs)
{
  gsl_ExpectsAudit(rhs.number() != quantity_values<typename Q2::rep>::zero());
  return lhs.number() / rhs.number() * (Q1::reference / Q2::reference);
}

template<Quantity Q1, Quantity Q2>
  requires(!treat_as_floating_point<typename Q1::rep>) && (!treat_as_floating_point<typename Q2::rep>) &&
          (interconvertible(Q1::reference, Q2::reference) || QuantityOf<Q2, dimensionless>) &&
          detail::InvokeResultOf<Q1::reference.character, std::modulus<>, typename Q1::rep, typename Q2::rep>
[[nodiscard]] constexpr Quantity auto operator%(const Q1& lhs, const Q2& rhs)
{
  gsl_ExpectsAudit(rhs.number() != quantity_values<typename Q2::rep>::zero());
  using ret = quantity<Q1::reference, std::invoke_result_t<std::modulus<>, typename Q1::rep, typename Q2::rep>>;
  return ret(lhs.number() % rhs.number());
}

template<Quantity Q1, Quantity Q2>
  requires(get_kind(Q1::quantity_spec) == get_kind(Q2::quantity_spec)) &&
          std::three_way_comparable_with<typename Q1::rep, typename Q2::rep>
[[nodiscard]] constexpr auto operator<=>(const Q1& lhs, const Q2& rhs)
{
  constexpr auto ref = common_reference(Q1::reference, Q2::reference);
  return quantity<ref, typename Q1::rep>(lhs).number() <=> quantity<ref, typename Q2::rep>(rhs).number();
}

template<Quantity Q1, Quantity Q2>
  requires(get_kind(Q1::quantity_spec) == get_kind(Q2::quantity_spec)) &&
          std::equality_comparable_with<typename Q1::rep, typename Q2::rep>
[[nodiscard]] constexpr bool operator==(const Q1& lhs, const Q2& rhs)
{
  constexpr auto ref = common_reference(Q1::reference, Q2::reference);
  return quantity<ref, typename Q1::rep>(lhs).number() == quantity<ref, typename Q2::rep>(rhs).number();
}

}  // namespace mp_units

namespace std {

template<mp_units::Quantity Q1, mp_units::Quantity Q2>
  requires requires {
    {
      mp_units::common_reference(Q1::reference, Q2::reference)
    } -> mp_units::Reference;
    typename common_type_t<typename Q1::rep, typename Q2::rep>;
  }
struct common_type<Q1, Q2> {
public:
  using type = mp_units::quantity<mp_units::common_reference(Q1::reference, Q2::reference),
                                  common_type_t<typename Q1::rep, typename Q2::rep>>;
};

}  // namespace std
