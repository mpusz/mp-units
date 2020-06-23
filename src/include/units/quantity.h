
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
    ratio_divide<typename UnitFrom::ratio, typename UnitTo::ratio>::is_integral();

//  constrain operators to mpusz::units::quantity only
template<typename T>
concept ll_quantity_concept = detail::is_quantity<T>;

// base class for quantity
// no increase in size of derived object , see https://en.cppreference.com/w/cpp/language/ebo
struct quantity_friend_operations{

   template<ll_quantity_concept Lhs, ll_quantity_concept Rhs>
   [[nodiscard]] friend constexpr Quantity AUTO operator+(const Lhs& lhs, const Rhs& rhs)
     requires 
            equivalent_dim<typename Lhs::dimension,typename Rhs::dimension>  &&
            std::regular_invocable<std::plus<>, typename Lhs::rep, typename Rhs::rep>
   {
     using common_rep = decltype(lhs.count() + rhs.count());
     using ret = common_quantity<Lhs, Rhs, common_rep>;
     return ret(ret(lhs).count() + ret(rhs).count());
   }

   template<ll_quantity_concept Lhs, ll_quantity_concept Rhs>
   [[nodiscard]] friend  constexpr Quantity AUTO operator-(const Lhs& lhs, const Rhs& rhs)
     requires 
            equivalent_dim<typename Lhs::dimension,typename Rhs::dimension>  &&
            std::regular_invocable<std::plus<>, typename Lhs::rep, typename Rhs::rep>
   {
     using common_rep = decltype(lhs.count() + rhs.count());
     using ret = common_quantity<Lhs, Rhs, common_rep>;
     return ret(ret(lhs).count() - ret(rhs).count());
   }

   template<ll_quantity_concept Q, Scalar Value>
   [[nodiscard]] friend constexpr Quantity AUTO operator*(const Q& q, const Value& v)
     requires std::regular_invocable<std::multiplies<>, typename Q::rep, Value>
   {
     using common_rep = decltype(q.count() * v);
     using ret = quantity<typename Q::dimension, typename Q::unit, common_rep>;
     return ret(q.count() * v);
   }

   template<Scalar Value, ll_quantity_concept Q>
   [[nodiscard]] friend constexpr Quantity AUTO operator*(const Value& v, const Q& q)
     requires std::regular_invocable<std::multiplies<>, Value, typename Q::rep>
   {
     return q * v;
   }

   template<ll_quantity_concept Lhs, ll_quantity_concept Rhs>
   [[nodiscard]] friend constexpr Scalar AUTO operator*(const Lhs& lhs, const Rhs& rhs)
     requires std::regular_invocable<std::multiplies<>, typename Lhs::rep, typename Rhs::rep> &&
              equivalent_dim<typename Lhs::dimension, dim_invert<typename Rhs::dimension>>
   {
     using common_rep = decltype(lhs.count() * rhs.count());
     using ratio = ratio_multiply<typename Lhs::unit::ratio, typename Rhs::unit::ratio>;
     if constexpr (treat_as_floating_point<common_rep>) {
       return common_rep(lhs.count()) * common_rep(rhs.count()) * common_rep(ratio::num) * fpow10(ratio::exp) / common_rep(ratio::den);
     } else {
       return common_rep(lhs.count()) * common_rep(rhs.count()) * common_rep(ratio::num) * ipow10(ratio::exp) / common_rep(ratio::den);
     }
   }


   template<ll_quantity_concept Lhs, ll_quantity_concept Rhs>
   [[nodiscard]] friend constexpr Quantity AUTO operator*(const Lhs& lhs, const Rhs& rhs)
     requires std::regular_invocable<std::multiplies<>, typename Lhs::rep, typename Rhs::rep>
   {
     using lhs_dimension = typename get_dimension<Lhs>::type;
     using rhs_dimension = typename get_dimension<Rhs>::type;
     using dim = dimension_multiply<lhs_dimension, rhs_dimension>;
     using lhs_unit = typename get_unit<Lhs>::type;
     using rhs_unit = typename get_unit<Rhs>::type;
     using ratio1 = ratio_divide<typename lhs_unit::ratio, typename dimension_unit<lhs_dimension>::ratio>;
     using ratio2 = ratio_divide<typename rhs_unit::ratio, typename dimension_unit<rhs_dimension>::ratio>;
     using ratio = ratio_multiply<ratio_multiply<ratio1, ratio2>, typename dimension_unit<dim>::ratio>;
     using unit = downcast_unit<dim, ratio>;
     using common_rep = decltype(lhs.count() * rhs.count());
     using ret = quantity<dim, unit, common_rep>;
     return ret(lhs.count() * rhs.count());
   }

   template<Scalar Value, ll_quantity_concept Q>
   [[nodiscard]] friend constexpr Quantity AUTO operator/(const Value& v, const Q& q)
     requires std::regular_invocable<std::divides<>, Value, typename Q::rep>
   {
     Expects(q.count() != 0);

     using dim = dim_invert<typename Q::dimension>;
     using ratio = ratio<Q::unit::ratio::den, Q::unit::ratio::num, -Q::unit::ratio::exp>;
     using unit = downcast_unit<dim, ratio>;
     using common_rep = decltype(v / q.count());
     using ret = quantity<dim, unit, common_rep>;
     return ret(v / q.count());
   }

   template<ll_quantity_concept Q, Scalar Value>
   [[nodiscard]] friend constexpr Quantity AUTO operator/(const Q& q, const Value& v)
     requires std::regular_invocable<std::divides<>, typename Q::rep, Value>
   {
     Expects(v != Value{0});

     using common_rep = decltype(q.count() / v);
     using ret = quantity<typename Q::dimension, typename Q::unit, common_rep>;
     return ret(q.count() / v);
   }

   template<ll_quantity_concept Lhs , ll_quantity_concept Rhs>
   [[nodiscard]] friend constexpr Scalar AUTO operator/(const Lhs& lhs, const Rhs& rhs)
     requires std::regular_invocable<std::divides<>, typename Lhs::rep, typename Rhs::rep> &&
              equivalent_dim<typename Lhs::dimension,typename Rhs::dimension>
   {
     Expects(rhs.count() != 0);

     using common_rep = decltype(lhs.count() / rhs.count());
     using cq = common_quantity<Lhs,Rhs, common_rep>;
     return cq(lhs).count() / cq(rhs).count();
   }

   template<ll_quantity_concept Lhs , ll_quantity_concept Rhs>
   [[nodiscard]] friend constexpr Quantity AUTO operator/(const Lhs& lhs, const Rhs& rhs)
     requires std::regular_invocable<std::divides<>, typename Lhs::rep, typename Rhs::rep>
   {
     Expects(rhs.count() != 0);

     using common_rep = decltype(lhs.count() / rhs.count());
     using dim = dimension_divide<typename Lhs::dimension, typename Rhs::dimension>;
     using ratio1 = ratio_divide<typename Lhs::unit::ratio, typename dimension_unit<typename Lhs::dimension>::ratio>;
     using ratio2 = ratio_divide<typename Rhs::unit::ratio, typename dimension_unit<typename Rhs::dimension>::ratio>;
     using ratio = ratio_multiply<ratio_divide<ratio1, ratio2>, typename dimension_unit<dim>::ratio>;
     using unit = downcast_unit<dim, ratio>;
     using ret = quantity<dim, unit, common_rep>;
     return ret(lhs.count() / rhs.count());
   }

   template<ll_quantity_concept Q, Scalar Value>
   [[nodiscard]] friend constexpr Quantity AUTO operator%(const Q& q, const Value& v)
     requires (!treat_as_floating_point<typename Q::rep>) &&
              (!treat_as_floating_point<Value>) &&
              std::regular_invocable<std::modulus<>, typename Q::rep, Value>
   {
     using common_rep = decltype(q.count() % v);
     using ret = quantity<typename Q::dimension, typename Q::unit, common_rep>;
     return ret(q.count() % v);
   }

   template<ll_quantity_concept Lhs , ll_quantity_concept Rhs>
   [[nodiscard]] friend constexpr Quantity AUTO operator%(const Lhs& lhs, const Rhs& rhs)
     requires (!treat_as_floating_point<typename Lhs::rep>) &&
              (!treat_as_floating_point<typename Rhs::rep>) &&
              equivalent_dim<typename Lhs::dimension,typename Rhs::dimension> &&
              std::regular_invocable<std::modulus<>, typename Lhs::rep, typename Rhs::rep>
   {
     using common_rep = decltype(lhs.count() % rhs.count());
     using ret = common_quantity<Lhs,Rhs, common_rep>;
     return ret(ret(lhs).count() % ret(rhs).count());
   }

};

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
class quantity : detail::quantity_friend_operations {
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
    requires requires(T v) { { v++ } -> SAME_AS(T); }
  constexpr quantity operator++(int)
  // requires requires(rep v) { { v++ } -> std::same_as<rep>; }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    return quantity(value_++);
  }

  template<typename T = Rep>
    requires requires(T v) { { --v } -> SAME_AS(T&); }
  constexpr quantity& operator--()
  // requires requires(rep v) { { --v } -> std::same_as<rep&>; }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    --value_;
    return *this;
  }

  template<typename T = Rep>
    requires requires(T v) { { v-- } -> SAME_AS(T); }
  constexpr quantity operator--(int)
  // requires requires(rep v) { { v-- } -> std::same_as<rep>; }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    return quantity(value_--);
  }

  template<typename T = Rep>
    requires requires(T v1, T v2) { { v1 += v2 } -> SAME_AS(T&); }
  constexpr quantity& operator+=(const quantity& q)
  // requires requires(rep v1, rep v2) { { v1 += v2 } -> std::same_as<T&>; }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    value_ += q.count();
    return *this;
  }

  template<typename T = Rep>
    requires requires(T v1, T v2) { { v1 -= v2 } -> SAME_AS(T&); }
  constexpr quantity& operator-=(const quantity& q)
  // requires requires(rep v1, rep v2) { { v1 -= v2 } -> std::same_as<T&>; }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    value_ -= q.count();
    return *this;
  }

  template<typename T = Rep>
    requires requires(T v1, T v2) { { v1 *= v2 } -> SAME_AS(T&); }
  constexpr quantity& operator*=(const rep& rhs)
  // requires requires(rep v1, rep v2) { { v1 *= v2 } -> std::same_as<T&>; }  // TODO gated by gcc-9 (fixed in gcc-10)
  {
    value_ *= rhs;
    return *this;
  }

  template<typename T = Rep>
    requires requires(T v1, T v2) { { v1 /= v2 } -> SAME_AS(T&); }
  constexpr quantity& operator/=(const rep& rhs)
  // requires requires(rep v1, rep v2) { { v1 /= v2 } -> std::same_as<rep&>; }  // TODO gated by gcc-9 (fixed in gcc-10)
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
             std::totally_ordered_with<Rep, Rep2>
  {
    using cq = common_quantity<quantity, quantity<D2, U2, Rep2>>;
    return cq(lhs).count() <=> cq(rhs).count();
  }

  template<typename D2, typename U2, typename Rep2>
  [[nodiscard]] friend constexpr auto operator==(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
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

template <typename D, typename U, typename V> struct get_dimension<quantity<U,D,V> >{ 
    using type = quantity<U,D,V>::dimension;
};

template< typename D, typename U, typename V> struct get_unit<quantity<U,D,V> >{
   using type = quantity<U,D,V>::unit;
};

namespace detail {

template<typename D, typename U, typename Rep>
inline constexpr bool is_quantity<quantity<D, U, Rep>> = true;

}  // namespace detail


}  // namespace units
