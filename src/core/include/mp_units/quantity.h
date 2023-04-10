
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

template<QuantityLike Q>
using quantity_like_type = quantity<quantity_like_traits<Q>::reference, typename quantity_like_traits<Q>::rep>;

template<typename T, typename Arg>
concept RepSafeConstructibleFrom =  // exposition only
  std::constructible_from<T, Arg> && (treat_as_floating_point<T> || !treat_as_floating_point<Arg>);

// UFrom ratio is an exact multiple of UTo
template<auto UFrom, auto UTo>
concept Harmonic =  // exposition only
  Unit<decltype(UFrom)> && Unit<decltype(UTo)> &&
  is_integral(get_canonical_unit(UFrom).mag / get_canonical_unit(UTo).mag);

template<typename QFrom, typename QTo>
concept QuantityConvertibleTo =  // exposition only
  Quantity<QFrom> && Quantity<QTo> && implicitly_convertible(QFrom::quantity_spec, QTo::quantity_spec) &&
  convertible_to(QFrom::unit, QTo::unit) && requires(QFrom q) { detail::sudo_cast<QTo>(q); } &&
  (treat_as_floating_point<typename QTo::rep> ||
   (!treat_as_floating_point<typename QFrom::rep> && Harmonic<QFrom::unit, QTo::unit>));

template<quantity_character Ch, typename Func, typename T, typename U>
concept InvokeResultIsRepresentationOf =
  std::regular_invocable<Func, T, U> && RepresentationOf<std::invoke_result_t<Func, T, U>, Ch>;

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
    return +number() * reference;
  }

  [[nodiscard]] constexpr Quantity auto operator-() const
    requires std::regular_invocable<std::negate<>, rep>
  {
    return -number() * reference;
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
    requires(!Quantity<Rep2>) && requires(rep a, const Rep2 b) {
      {
        a *= b
      } -> std::same_as<rep&>;
    }
  constexpr quantity& operator*=(const Rep2& rhs)
  {
    number_ *= rhs;
    return *this;
  }

  template<QuantityOf<dimension_one> Q>
    requires(Q::unit == ::mp_units::one) && requires(rep a, const typename Q::rep b) {
      {
        a *= b
      } -> std::same_as<rep&>;
    }
  constexpr quantity& operator*=(const Q& rhs)
  {
    number_ *= rhs.number();
    return *this;
  }

  template<typename Rep2>
    requires(!Quantity<Rep2>) && requires(rep a, const Rep2 b) {
      {
        a /= b
      } -> std::same_as<rep&>;
    }
  constexpr quantity& operator/=(const Rep2& rhs)
  {
    gsl_ExpectsAudit(rhs != quantity_values<Rep2>::zero());
    number_ /= rhs;
    return *this;
  }

  template<QuantityOf<dimension_one> Q>
    requires(Q::unit == ::mp_units::one) && requires(rep a, const typename Q::rep b) {
      {
        a /= b
      } -> std::same_as<rep&>;
    }
  constexpr quantity& operator/=(const Q& rhs)
  {
    gsl_ExpectsAudit(rhs.number() != quantity_values<typename Q::rep>::zero());
    number_ /= rhs.number();
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
  template<Quantity Q>
    requires requires { common_reference(reference, Q::reference); } &&
             detail::InvokeResultIsRepresentationOf<common_quantity_spec(quantity_spec, Q::quantity_spec).character,
                                                    std::plus<>, rep, typename Q::rep>
  [[nodiscard]] friend constexpr Quantity auto operator+(const quantity& lhs, const Q& rhs)
  {
    constexpr auto ref = common_reference(reference, Q::reference);
    using ret = quantity<ref, decltype(lhs.number() + rhs.number())>;
    return (ret(lhs).number() + ret(rhs).number()) * ref;
  }

  template<Quantity Q>
    requires requires { common_reference(reference, Q::reference); } &&
             detail::InvokeResultIsRepresentationOf<common_quantity_spec(quantity_spec, Q::quantity_spec).character,
                                                    std::minus<>, rep, typename Q::rep>
  [[nodiscard]] friend constexpr Quantity auto operator-(const quantity& lhs, const Q& rhs)
  {
    constexpr auto ref = common_reference(reference, Q::reference);
    using ret = quantity<ref, decltype(lhs.number() - rhs.number())>;
    return (ret(lhs).number() - ret(rhs).number()) * ref;
  }

  template<Quantity Q>
    requires detail::InvokeResultIsRepresentationOf<(quantity_spec * Q::quantity_spec).character, std::multiplies<>,
                                                    rep, typename Q::rep>
  [[nodiscard]] friend constexpr Quantity auto operator*(const quantity& lhs, const Q& rhs)
  {
    return lhs.number() * rhs.number() * (reference * Q::reference);
  }

  template<typename Value>
    requires(!Quantity<Value>) &&
            detail::InvokeResultIsRepresentationOf<quantity_spec.character, std::multiplies<>, rep, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator*(const quantity& q, const Value& v)
  {
    return q.number() * v * reference;
  }

  template<typename Value>
    requires(!Quantity<Value>) &&
            detail::InvokeResultIsRepresentationOf<quantity_spec.character, std::multiplies<>, const Value&, rep>
  [[nodiscard]] friend constexpr Quantity auto operator*(const Value& v, const quantity& q)
  {
    return v * q.number() * reference;
  }

  template<Quantity Q>
    requires detail::InvokeResultIsRepresentationOf<(quantity_spec / Q::quantity_spec).character, std::divides<>, rep,
                                                    typename Q::rep>
  [[nodiscard]] friend constexpr Quantity auto operator/(const quantity& lhs, const Q& rhs)
  {
    gsl_ExpectsAudit(rhs.number() != quantity_values<typename Q::rep>::zero());
    return lhs.number() / rhs.number() * (reference / Q::reference);
  }

  template<typename Value>
    requires(!Quantity<Value>) &&
            detail::InvokeResultIsRepresentationOf<quantity_spec.character, std::divides<>, rep, const Value&>
  [[nodiscard]] friend constexpr Quantity auto operator/(const quantity& q, const Value& v)
  {
    gsl_ExpectsAudit(v != quantity_values<Value>::zero());
    return q.number() / v * reference;
  }

  template<typename Value>
    requires(!Quantity<Value>) &&
            detail::InvokeResultIsRepresentationOf<quantity_spec.character, std::divides<>, const Value&, rep>
  [[nodiscard]] friend constexpr Quantity auto operator/(const Value& v, const quantity& q)
  {
    return v / q.number() * (::mp_units::one / reference);
  }

  template<Quantity Q>
    requires(!treat_as_floating_point<rep>) && (!treat_as_floating_point<typename Q::rep>) &&
            requires {
              common_reference(reference, Q::reference);
            } && detail::InvokeResultIsRepresentationOf<quantity_spec.character, std::modulus<>, rep, typename Q::rep>
  [[nodiscard]] friend constexpr Quantity auto operator%(const quantity& lhs, const Q& rhs)
  {
    gsl_ExpectsAudit(rhs.number() != quantity_values<rep>::zero());
    constexpr auto ref = common_reference(reference, Q::reference);
    using ret = quantity<ref, decltype(lhs.number() % rhs.number())>;
    return (ret(lhs).number() % ret(rhs).number()) * ref;
  }

  template<Quantity Q>
    requires requires { common_reference(reference, Q::reference); } &&
             std::equality_comparable_with<rep, typename Q::rep>
  [[nodiscard]] friend constexpr bool operator==(const quantity& lhs, const Q& rhs)
  {
    using ct = std::common_type_t<quantity, Q>;
    return ct(lhs).number() == ct(rhs).number();
  }

  template<Quantity Q>
    requires requires { common_reference(reference, Q::reference); } &&
             std::three_way_comparable_with<rep, typename Q::rep>
  [[nodiscard]] friend constexpr auto operator<=>(const quantity& lhs, const Q& rhs)
  {
    using ct = std::common_type_t<quantity, Q>;
    return ct(lhs).number() <=> ct(rhs).number();
  }


  // dimensionless quantities support for interacting with raw values
  template<typename Value>
    requires(dimension == dimension_one) &&
            (unit == ::mp_units::one) && detail::RepSafeConstructibleFrom<rep, std::remove_cvref_t<Value>>
  constexpr explicit(false) quantity(Value&& v) : number_(std::forward<Value>(v))
  {
  }

  template<typename Value>
    requires(!Quantity<Value>) && (dimension == dimension_one) &&
            detail::InvokeResultIsRepresentationOf<quantity_spec.character, std::plus<>, rep, Value>
  [[nodiscard]] friend constexpr Quantity auto operator+(const quantity& q, const Value& v)
  {
    return q + v * ::mp_units::one;
  }

  template<typename Value>
    requires(!Quantity<Value>) && (dimension == dimension_one) &&
            detail::InvokeResultIsRepresentationOf<quantity_spec.character, std::plus<>, Value, rep>
  [[nodiscard]] friend constexpr Quantity auto operator+(const Value& v, const quantity& q)
  {
    return v * ::mp_units::one + q;
  }

  template<typename Value>
    requires(!Quantity<Value>) && (dimension == dimension_one) &&
            detail::InvokeResultIsRepresentationOf<quantity_spec.character, std::minus<>, rep, Value>
  [[nodiscard]] friend constexpr Quantity auto operator-(const quantity& q, const Value& v)
  {
    return q - v * ::mp_units::one;
  }

  template<typename Value>
    requires(!Quantity<Value>) && (dimension == dimension_one) &&
            detail::InvokeResultIsRepresentationOf<quantity_spec.character, std::minus<>, Value, rep>
  [[nodiscard]] friend constexpr Quantity auto operator-(const Value& v, const quantity& q)
  {
    return v * ::mp_units::one - q;
  }

  template<typename Value>
    requires(!Quantity<Value>) && (dimension == dimension_one) && (unit == ::mp_units::one) &&
            (!treat_as_floating_point<Value>) && (!treat_as_floating_point<rep>) &&
            detail::InvokeResultIsRepresentationOf<quantity_spec.character, std::modulus<>, Value, rep>
  [[nodiscard]] friend constexpr Quantity auto operator%(const Value& v, const quantity& q)
  {
    gsl_ExpectsAudit(q.number() != quantity_values<rep>::zero());
    return (v % q.number()) * reference;
  }

  template<typename Value>
    requires(!Quantity<Value>) && (dimension == dimension_one) && std::equality_comparable_with<rep, Value>
  [[nodiscard]] friend constexpr bool operator==(const quantity& q, const Value& v)
  {
    return q == v * ::mp_units::one;
  }

  template<typename Value>
    requires(!Quantity<Value>) && (dimension == dimension_one) && std::three_way_comparable_with<rep, Value>
  [[nodiscard]] friend constexpr auto operator<=>(const quantity& q, const Value& v)
  {
    return q <=> v * ::mp_units::one;
  }

private:
  template<Reference R2, typename Rep2>
    requires RepresentationOf<std::remove_cvref_t<Rep2>, get_quantity_spec(R2{}).character>
  friend constexpr quantity<R2{}, std::remove_cvref_t<Rep2>> operator*(Rep2&& lhs, R2);

  template<typename Value>
    requires detail::RepSafeConstructibleFrom<rep, std::remove_cvref_t<Value>>
  constexpr explicit quantity(Value&& v) : number_(std::forward<Value>(v))
  {
  }
};

// CTAD
template<QuantityLike Q>
explicit quantity(Q) -> quantity<quantity_like_traits<Q>::reference, typename quantity_like_traits<Q>::rep>;

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

// dimensionless quantities support for interacting with raw values
template<mp_units::Quantity Q, typename Value>
  requires(!mp_units::Quantity<Value>) && (Q::dimension == mp_units::dimension_one) && (Q::unit == mp_units::one) &&
          requires { typename common_type_t<typename Q::rep, Value>; }
struct common_type<Q, Value> {
public:
  using type = mp_units::quantity<mp_units::one, common_type_t<typename Q::rep, Value>>;
};

template<mp_units::Quantity Q, typename Value>
  requires(!mp_units::Quantity<Value>) && (Q::dimension == mp_units::dimension_one) && (Q::unit == mp_units::one) &&
          requires { typename common_type_t<typename Q::rep, Value>; }
struct common_type<Value, Q> : common_type<Q, Value> {
};

}  // namespace std
