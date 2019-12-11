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

#include <units/bits/concepts.h>
#include <units/bits/dimension_op.h>
#include <units/bits/unit_text.h>

#if __GNUC__ >= 10
#include <compare>
#endif

#include <limits>
#include <ostream>

namespace units {

// Quantity
namespace detail {

template<typename T>
inline constexpr bool is_quantity = false;

// partial specialization below after the first quantity forward declaration

}  // namespace detail

template<typename T>
concept Quantity = detail::is_quantity<T>;

// QuantityOf
template<typename T, typename Dim>
concept QuantityOf = Quantity<T> && Dimension<Dim> && equivalent_dim<typename T::dimension, Dim>;

// Scalar
template<typename T>
concept Scalar = (!Quantity<T>) && std::regular<T> && std::totally_ordered<T> && detail::basic_arithmetic<T>;

template<Dimension D, UnitOf<D> U, Scalar Rep>
class quantity;

namespace detail {

template<typename D, typename U, typename Rep>
inline constexpr bool is_quantity<quantity<D, U, Rep>> = true;

}  // namespace detail

// common_quantity
namespace detail {

template<typename Q1, typename Q2, typename Rep>
struct common_quantity_impl;

template<typename D, typename U, typename Rep1, typename Rep2, typename Rep>
struct common_quantity_impl<quantity<D, U, Rep1>, quantity<D, U, Rep2>, Rep> {
  using type = quantity<D, U, Rep>;
};

template<typename D, typename U1, typename Rep1, typename U2, typename Rep2, typename Rep>
struct common_quantity_impl<quantity<D, U1, Rep1>, quantity<D, U2, Rep2>, Rep> {
  using type = quantity<D, downcast_unit<D, common_ratio<typename U1::ratio, typename U2::ratio>>, Rep>;
};

template<typename D1, typename U1, typename Rep1, typename D2, typename U2, typename Rep2, typename Rep>
  requires same_unit_reference<dimension_unit<D1>, dimension_unit<D2>>::value
struct common_quantity_impl<quantity<D1, U1, Rep1>, quantity<D2, U2, Rep2>, Rep> {
  using type = quantity<D1, downcast_unit<D1, common_ratio<typename U1::ratio, typename U2::ratio>>, Rep>;
};

template<typename D1, typename U1, typename Rep1, typename D2, typename U2, typename Rep2, typename Rep>
struct common_quantity_impl<quantity<D1, U1, Rep1>, quantity<D2, U2, Rep2>, Rep> {
  using ratio1 = ratio_multiply<typename D1::base_units_ratio, typename U1::ratio>;
  using ratio2 = ratio_multiply<typename D2::base_units_ratio, typename U2::ratio>;
  using type = quantity<D1, downcast_unit<D1, common_ratio<ratio1, ratio2>>, Rep>;
};

}  // namespace detail

template<Quantity Q1, Quantity Q2, Scalar Rep = std::common_type_t<typename Q1::rep, typename Q2::rep>>
  requires equivalent_dim<typename Q1::dimension, typename Q2::dimension>
using common_quantity = detail::common_quantity_impl<Q1, Q2, Rep>::type;

// quantity_cast
namespace detail {

template<typename To, typename CRatio, typename CRep, bool NumIsOne = false, bool DenIsOne = false>
struct quantity_cast_impl {
  template<typename Q>
  static constexpr To cast(const Q& q)
  {
    if constexpr (treat_as_floating_point<CRep>) {
      return To(static_cast<To::rep>(static_cast<CRep>(q.count()) *
                                     (static_cast<CRep>(CRatio::num) / static_cast<CRep>(CRatio::den))));
    } else {
      return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(CRatio::num) /
                                     static_cast<CRep>(CRatio::den)));
    }
  }
};

template<typename To, typename CRatio, typename CRep>
struct quantity_cast_impl<To, CRatio, CRep, true, true> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    return To(static_cast<To::rep>(q.count()));
  }
};

template<typename To, typename CRatio, typename CRep>
struct quantity_cast_impl<To, CRatio, CRep, true, false> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    if constexpr (treat_as_floating_point<CRep>) {
      return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * (CRep{1} / static_cast<CRep>(CRatio::den))));
    } else {
      return To(static_cast<To::rep>(static_cast<CRep>(q.count()) / static_cast<CRep>(CRatio::den)));
    }
  }
};

template<typename To, typename CRatio, typename CRep>
struct quantity_cast_impl<To, CRatio, CRep, false, true> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(CRatio::num)));
  }
};

template<Dimension FromD, Unit FromU, Dimension ToD, Unit ToU>
struct cast_ratio;

template<BaseDimension FromD, Unit FromU, BaseDimension ToD, Unit ToU>
struct cast_ratio<FromD, FromU, ToD, ToU> {
  using type = ratio_divide<typename FromU::ratio, typename ToU::ratio>;
};

template<DerivedDimension FromD, Unit FromU, DerivedDimension ToD, Unit ToU>
  requires same_unit_reference<FromU, ToU>::value
struct cast_ratio<FromD, FromU, ToD, ToU> {
  using type = ratio_divide<typename FromU::ratio, typename ToU::ratio>;
};

template<DerivedDimension FromD, Unit FromU, DerivedDimension ToD, Unit ToU>
struct cast_ratio<FromD, FromU, ToD, ToU> {
  using from_ratio = ratio_multiply<typename FromD::base_units_ratio, typename FromU::ratio>;
  using to_ratio = ratio_multiply<typename ToD::base_units_ratio, typename ToU::ratio>;
  using type = ratio_divide<from_ratio, to_ratio>;
};

}  // namespace detail

/**
 * @brief Explcit cast of a quantity
 * 
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In such cases an explicit cast have to be used.
 * 
 * This cast gets the target quantity type to cast to. For example:
 * 
 * auto q1 = units::quantity_cast<units::si::time<units::si::second>>(1ms);
 * 
 * @tparam To a target quantity type to cast to
 */
template<Quantity To, typename D, typename U, typename Rep>
[[nodiscard]] constexpr auto quantity_cast(const quantity<D, U, Rep>& q)
  requires QuantityOf<To, D> &&
           detail::basic_arithmetic<std::common_type_t<typename To::rep, Rep, intmax_t>>
{
  using c_ratio = detail::cast_ratio<D, U, typename To::dimension, typename To::unit>::type;
  using c_rep = std::common_type_t<typename To::rep, Rep, intmax_t>;
  using ret_unit = downcast_unit<typename To::dimension, typename To::unit::ratio>;
  using ret = quantity<typename To::dimension, ret_unit, typename To::rep>;
  using cast = detail::quantity_cast_impl<ret, c_ratio, c_rep, c_ratio::num == 1, c_ratio::den == 1>;
  return cast::cast(q);
}

/**
 * @brief Explcit cast of a quantity
 * 
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In such cases an explicit cast have to be used.
 * 
 * This cast gets only the target dimension to cast to. For example:
 * 
 * auto q1 = units::quantity_cast<units::si::acceleration>(200Gal);
 * 
 * @tparam ToD a dimension type to use for a target quantity
 */
template<Dimension ToD, typename D, typename U, typename Rep>
[[nodiscard]] constexpr auto quantity_cast(const quantity<D, U, Rep>& q)
  requires equivalent_dim<ToD, D>
{
  return quantity_cast<quantity<ToD, U, Rep>>(q);
}

/**
 * @brief Explcit cast of a quantity
 * 
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In such cases an explicit cast have to be used.
 * 
 * This cast gets only the target unit to cast to. For example:
 * 
 * auto q1 = units::quantity_cast<units::si::second>(1ms);
 * 
 * @tparam ToU a unit type to use for a target quantity
 */
template<Unit ToU, typename D, typename U, typename Rep>
[[nodiscard]] constexpr auto quantity_cast(const quantity<D, U, Rep>& q)
  requires UnitOf<ToU, D>
{
  return quantity_cast<quantity<D, ToU, Rep>>(q);
}

/**
 * @brief Explcit cast of a quantity
 * 
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In such cases an explicit cast have to be used.
 * 
 * This cast gets only representation to cast to. For example:
 * 
 * auto q1 = units::quantity_cast<int>(1ms);
 * 
 * @tparam ToRep a representation type to use for a target quantity
 */
template<Scalar ToRep, typename D, typename U, typename Rep>
[[nodiscard]] constexpr auto quantity_cast(const quantity<D, U, Rep>& q)
  requires detail::basic_arithmetic<std::common_type_t<ToRep, Rep, intmax_t>>
{
  return quantity_cast<quantity<D, U, ToRep>>(q);
}

/**
 * @brief A type trait that defines zero, one, min, and max for a representation type
 * 
 * The zero, one, min, and max member functions in units::quantity forward their work to
 * these methods. This type can be specialized if the representation Rep requires a specific
 * implementation to return these quantity objects.
 * 
 * @tparam Rep a representation type for which a type trait is defined
 */
template<Scalar Rep>
struct quantity_values {
  static constexpr Rep zero() noexcept { return Rep(0); }
  static constexpr Rep one() noexcept { return Rep(1); }
  static constexpr Rep min() noexcept { return std::numeric_limits<Rep>::lowest(); }
  static constexpr Rep max() noexcept { return std::numeric_limits<Rep>::max(); }
};

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
  Rep value_;

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
    requires std::regular_invocable<std::ranges::negate, T>
  // requires std::regular_invocable<std::ranges::negate, rep>  // TODO gated by gcc-9 (fixed in gcc-10)
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

  template<typename U2, typename Rep2>
  [[nodiscard]] friend constexpr Quantity AUTO operator+(const quantity& lhs, const quantity<D, U2, Rep2>& rhs)
    requires detail::basic_arithmetic<Rep, Rep2>
  {
    using common_rep = decltype(lhs.count() + rhs.count());
    using ret = common_quantity<quantity, quantity<D, U2, Rep2>, common_rep>;
    return ret(ret(lhs).count() + ret(rhs).count());
  }

  template<typename U2, typename Rep2>
  [[nodiscard]] friend constexpr Quantity AUTO operator-(const quantity& lhs, const quantity<D, U2, Rep2>& rhs)
    requires detail::basic_arithmetic<Rep, Rep2>
  {
    using common_rep = decltype(lhs.count() - rhs.count());
    using ret = common_quantity<quantity, quantity<D, U2, Rep2>, common_rep>;
    return ret(ret(lhs).count() - ret(rhs).count());
  }

  template<Scalar Value>
  [[nodiscard]] friend constexpr Quantity AUTO operator%(const quantity& q, const Value& v)
    requires (!treat_as_floating_point<Rep>) &&
            (!treat_as_floating_point<Value>) &&
            std::magma<std::ranges::modulus, Rep, Value>
  {
    using common_rep = decltype(q.count() % v);
    using ret = quantity<D, U, common_rep>;
    return ret(q.count() % v);
  }

  template<typename U2, typename Rep2>
  [[nodiscard]] friend constexpr Quantity AUTO operator%(const quantity& lhs, const quantity<D, U2, Rep2>& rhs)
    requires (!treat_as_floating_point<Rep>) &&
            (!treat_as_floating_point<Rep2>) &&
            std::magma<std::ranges::modulus, Rep, Rep2>
  {
    using common_rep = decltype(lhs.count() % rhs.count());
    using ret = common_quantity<quantity, quantity<D, U2, Rep2>, common_rep>;
    return ret(ret(lhs).count() % ret(rhs).count());
  }

#if __GNUC__ >= 10

  template<typename D2, typename U2, typename Rep2>
  [[nodiscard]] friend constexpr auto operator<=>(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
    requires equivalent_dim<D, D2> &&
            detail::basic_arithmetic<Rep, Rep2> &&
            std::totally_ordered_with<Rep, Rep2>
  {
    using cq = common_quantity<quantity, quantity<D2, U2, Rep2>>;
    return cq(lhs).count() <=> cq(rhs).count();
  }

  // TODO op== not needed (gcc bug)
  template<typename D2, typename U2, typename Rep2>
  [[nodiscard]] friend constexpr auto operator==(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
    requires equivalent_dim<D, D2> &&
            detail::basic_arithmetic<Rep, Rep2> &&
            std::equality_comparable_with<Rep, Rep2>
  {
    using cq = common_quantity<quantity, quantity<D2, U2, Rep2>>;
    return cq(lhs).count() == cq(rhs).count();
  }

#else

  template<typename D2, typename U2, typename Rep2>
  [[nodiscard]] friend constexpr bool operator==(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
    requires equivalent_dim<D, D2> &&
            detail::basic_arithmetic<Rep, Rep2> &&
            std::equality_comparable_with<Rep, Rep2>
  {
    using cq = common_quantity<quantity, quantity<D2, U2, Rep2>>;
    return cq(lhs).count() == cq(rhs).count();
  }

  template<typename D2, typename U2, typename Rep2>
  [[nodiscard]] friend constexpr bool operator!=(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
    requires equivalent_dim<D, D2> &&
            detail::basic_arithmetic<Rep, Rep2> &&
            std::equality_comparable_with<Rep, Rep2>
  {
    return !(lhs == rhs);
  }

  template<typename D2, typename U2, typename Rep2>
  [[nodiscard]] friend constexpr bool operator<(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
    requires equivalent_dim<D, D2> &&
            detail::basic_arithmetic<Rep, Rep2> &&
            std::totally_ordered_with<Rep, Rep2>
  {
    using cq = common_quantity<quantity, quantity<D2, U2, Rep2>>;
    return cq(lhs).count() < cq(rhs).count();
  }

  template<typename D2, typename U2, typename Rep2>
  [[nodiscard]] friend constexpr bool operator<=(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
    requires equivalent_dim<D, D2> &&
            detail::basic_arithmetic<Rep, Rep2> &&
            std::totally_ordered_with<Rep, Rep2>
  {
    return !(rhs < lhs);
  }

  template<typename D2, typename U2, typename Rep2>
  [[nodiscard]] friend constexpr bool operator>(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
    requires equivalent_dim<D, D2> &&
            detail::basic_arithmetic<Rep, Rep2> &&
            std::totally_ordered_with<Rep, Rep2>
  {
    return rhs < lhs;
  }

  template<typename D2, typename U2, typename Rep2>
  [[nodiscard]] friend constexpr bool operator>=(const quantity& lhs, const quantity<D2, U2, Rep2>& rhs)
    requires equivalent_dim<D, D2> &&
            detail::basic_arithmetic<Rep, Rep2> &&
            std::totally_ordered_with<Rep, Rep2>
  {
    return !(lhs < rhs);
  }

#endif

  template<class CharT, class Traits>
  friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const quantity& q)
  {
    return os << q.count() << " " << detail::unit_text<quantity::dimension, quantity::unit>();
  }
};

// TODO make hidden friends when moved to gcc-10

template<typename D, typename U, typename Rep, Scalar Value>
[[nodiscard]] constexpr Quantity AUTO operator*(const quantity<D, U, Rep>& q, const Value& v)
  requires std::magma<std::ranges::times, Rep, Value>
{
  using common_rep = decltype(q.count() * v);
  using ret = quantity<D, U, common_rep>;
  return ret(q.count() * v);
}

template<Scalar Value, typename D, typename U, typename Rep>
[[nodiscard]] constexpr Quantity AUTO operator*(const Value& v, const quantity<D, U, Rep>& q)
  requires std::magma<std::ranges::times, Value, Rep>
{
  return q * v;
}

template<typename D1, typename U1, typename Rep1, typename D2, typename U2, typename Rep2>
[[nodiscard]] constexpr Scalar AUTO operator*(const quantity<D1, U1, Rep1>& lhs, const quantity<D2, U2, Rep2>& rhs)
  requires detail::basic_arithmetic<Rep1, Rep2> && equivalent_dim<D1, dim_invert<D2>>
{
  using common_rep = decltype(lhs.count() * rhs.count());
  using ratio = ratio_multiply<typename U1::ratio, typename U2::ratio>;
  return common_rep(lhs.count()) * common_rep(rhs.count()) * common_rep(ratio::num) / common_rep(ratio::den);
}

template<typename D1, typename U1, typename Rep1, typename D2, typename U2, typename Rep2>
[[nodiscard]] constexpr Quantity AUTO operator*(const quantity<D1, U1, Rep1>& lhs, const quantity<D2, U2, Rep2>& rhs)
  requires detail::basic_arithmetic<Rep1, Rep2> && (!equivalent_dim<D1, dim_invert<D2>>)  // TODO equivalent_derived_dim?
{
  using dim = dimension_multiply<D1, D2>;
  using ratio1 = ratio_divide<typename U1::ratio, typename dimension_unit<D1>::ratio>;
  using ratio2 = ratio_divide<typename U2::ratio, typename dimension_unit<D2>::ratio>;
  using ratio = ratio_multiply<ratio_multiply<ratio1, ratio2>, typename dimension_unit<dim>::ratio>;
  using unit = downcast_unit<dim, ratio>;
  using common_rep = decltype(lhs.count() * rhs.count());
  using ret = quantity<dim, unit, common_rep>;
  return ret(lhs.count() * rhs.count());
}

template<typename D, Scalar Value, typename U, typename Rep>
[[nodiscard]] constexpr Quantity AUTO operator/(const Value& v, const quantity<D, U, Rep>& q)
  requires std::magma<std::ranges::divided_by, Value, Rep>
{
  Expects(q.count() != 0);

  using dim = dim_invert<D>;
  using ratio = ratio<U::ratio::den, U::ratio::num>;
  using unit = downcast_unit<dim, ratio>;
  using common_rep = decltype(v / q.count());
  using ret = quantity<dim, unit, common_rep>;
  return ret(v / q.count());
}

template<typename D, typename U, typename Rep, Scalar Value>
[[nodiscard]] constexpr Quantity AUTO operator/(const quantity<D, U, Rep>& q, const Value& v)
  requires std::magma<std::ranges::divided_by, Rep, Value>
{
  Expects(v != Value{0});

  using common_rep = decltype(q.count() / v);
  using ret = quantity<D, U, common_rep>;
  return ret(q.count() / v);
}

template<typename D1, typename U1, typename Rep1, typename D2, typename U2, typename Rep2>
[[nodiscard]] constexpr Scalar AUTO operator/(const quantity<D1, U1, Rep1>& lhs, const quantity<D2, U2, Rep2>& rhs)
  requires detail::basic_arithmetic<Rep1, Rep2> && equivalent_dim<D1, D2>
{
  Expects(rhs.count() != 0);

  using common_rep = decltype(lhs.count() / rhs.count());
  using cq = common_quantity<quantity<D1, U1, Rep1>, quantity<D2, U2, Rep2>, common_rep>;
  return cq(lhs).count() / cq(rhs).count();
}

template<typename D1, typename U1, typename Rep1, typename D2, typename U2, typename Rep2>
[[nodiscard]] constexpr Quantity AUTO operator/(const quantity<D1, U1, Rep1>& lhs, const quantity<D2, U2, Rep2>& rhs)
  requires detail::basic_arithmetic<Rep1, Rep2> && (!equivalent_dim<D1, D2>)  // TODO equivalent_derived_dim?
{
  Expects(rhs.count() != 0);

  using common_rep = decltype(lhs.count() / rhs.count());
  using dim = dimension_divide<D1, D2>;
  using ratio1 = ratio_divide<typename U1::ratio, typename dimension_unit<D1>::ratio>;
  using ratio2 = ratio_divide<typename U2::ratio, typename dimension_unit<D2>::ratio>;
  using ratio = ratio_multiply<ratio_divide<ratio1, ratio2>, typename dimension_unit<dim>::ratio>;
  using unit = downcast_unit<dim, ratio>;
  using ret = quantity<dim, unit, common_rep>;
  return ret(lhs.count() / rhs.count());
}

}  // namespace units
