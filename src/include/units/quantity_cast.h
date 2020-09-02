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

#include <units/concepts.h>
#include <units/customization_points.h>
#include <units/bits/dimension_op.h>
#include <units/bits/external/type_traits.h>
#include <cassert>

namespace units {

template<Dimension D, UnitOf<D> U, Scalar Rep>
class quantity;

template<Dimension D, UnitOf<D> U, Scalar Rep>
class quantity_point;

namespace detail {

constexpr std::intmax_t ipow10(std::intmax_t exp)
{
  assert(exp >= 0);
  if (exp == 0) return 1;
  std::intmax_t result = 1;
  while (exp > 0) {
    result *= 10;
    --exp;
  }
  return result;
}

template<typename Rep>
constexpr Rep fpow10(std::intmax_t exp)
{
  if (exp == 0) return Rep(1.0);
  Rep result = Rep(1.0);
  if (exp < 0) {
    while (exp < 0) {
      result = result / Rep(10.0);
      ++exp;
    }
  } else {
    while (exp > 0) {
      result = result * Rep(10.0);
      --exp;
    }
  }
  return result;
}

template<typename D, typename U, typename Rep>
constexpr auto quantity_ratio(const quantity<D, U, Rep>&)
{
  if constexpr(BaseDimension<D>) {
    return U::ratio;
  }
  else {
    return D::base_units_ratio * U::ratio / D::coherent_unit::ratio;
  }
}

} // namespace detail

// QuantityOf
template<typename T, typename Dim>
concept QuantityOf = Quantity<T> && Dimension<Dim> && equivalent<typename T::dimension, Dim>;

// quantity_cast
namespace detail {

template<typename To, ratio CRatio, typename CRep, bool NumIsOne, bool DenIsOne, bool ExpIsZero>
struct quantity_cast_impl;

template<typename To, ratio CRatio, typename CRep>
struct quantity_cast_impl<To, CRatio, CRep, true, true, true> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    return To(static_cast<To::rep>(q.count()));
  }
};

template<typename To, ratio CRatio, constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, true, true, false> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    if constexpr (treat_as_floating_point<CRep>) {
      return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(fpow10<CRep>(CRatio.exp))));
    } else {
      if constexpr (CRatio.exp > 0) {
        return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(ipow10(CRatio.exp))));
      }
      else {
        return To(static_cast<To::rep>(static_cast<CRep>(q.count()) / static_cast<CRep>(ipow10(-CRatio.exp))));
      }
    }
  }
};

template<typename To, ratio CRatio, constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, false, false, true> {
  template<typename Q>
  static constexpr To cast(const Q& q)
  {
    return To(static_cast<To::rep>(static_cast<CRep>(q.count()) *
                                     (static_cast<CRep>(CRatio.num) /
                                      static_cast<CRep>(CRatio.den))));
  }
};

template<typename To, ratio CRatio, constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, false, false, false> {
  template<typename Q>
  static constexpr To cast(const Q& q)
  {
    if constexpr (treat_as_floating_point<CRep>) {
      return To(static_cast<To::rep>(static_cast<CRep>(q.count()) *
                                     static_cast<CRep>(fpow10<CRep>(CRatio.exp)) *
                                     (static_cast<CRep>(CRatio.num) /
                                      static_cast<CRep>(CRatio.den))));
    } else {
      if constexpr (CRatio.exp > 0) {
        return To(static_cast<To::rep>(static_cast<CRep>(q.count()) *
                                      static_cast<CRep>(CRatio.num) *
                                      static_cast<CRep>(ipow10(CRatio.exp)) /
                                      static_cast<CRep>(CRatio.den)));
      }
      else {
        return To(static_cast<To::rep>(static_cast<CRep>(q.count()) *
                                      static_cast<CRep>(CRatio.num) /
                                      (static_cast<CRep>(CRatio.den) *
                                       static_cast<CRep>(ipow10(-CRatio.exp)))));
      }
    }
  }
};

template<typename To, ratio CRatio, constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, true, false, true> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    return To(static_cast<To::rep>(static_cast<CRep>(q.count()) / static_cast<CRep>(CRatio.den)));
  }
};

template<typename To, ratio CRatio, constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, true, false, false> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    if constexpr (treat_as_floating_point<CRep>) {
      return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(fpow10<CRep>(CRatio.exp)) * (CRep{1} / static_cast<CRep>(CRatio.den))));
    } else {
      if constexpr (CRatio.exp > 0) {
        return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(ipow10(CRatio.exp)) / static_cast<CRep>(CRatio.den)));
      }
      else {
        return To(static_cast<To::rep>(static_cast<CRep>(q.count()) / (static_cast<CRep>(ipow10(-CRatio.exp)) * static_cast<CRep>(CRatio.den))));
      }
    }
  }
};

template<typename To, ratio CRatio, constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, false, true, true> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(CRatio.num)));
  }
};

template<typename To, ratio CRatio, constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, false, true, false> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    if constexpr (treat_as_floating_point<CRep>) {
      return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(CRatio.num) * static_cast<CRep>(fpow10<CRep>(CRatio.exp))));
    } else {
      if constexpr (CRatio.exp > 0) {
        return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(CRatio.num) * static_cast<CRep>(ipow10(CRatio.exp))));
      }
      else {
        return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(CRatio.num) / static_cast<CRep>(ipow10(-CRatio.exp))));
      }
    }
  }
};

template<typename To, ratio CRatio, not_constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, true, true, false> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    if constexpr (treat_as_floating_point<CRep>) {
      return To(static_cast<To::rep>(q.count() * fpow10<CRep>(CRatio.exp)));
    } else {
      if constexpr (CRatio.exp > 0) {
        return To(static_cast<To::rep>(q.count() * ipow10(CRatio.exp)));
      }
      else {
        return To(static_cast<To::rep>(q.count() / ipow10(-CRatio.exp)));
      }
    }
  }
};

template<typename To, ratio CRatio, not_constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, false, false, true> {
  template<typename Q>
  static constexpr To cast(const Q& q)
  {
    return To(static_cast<To::rep>(q.count() * (CRatio.num / CRatio.den)));
  }
};

template<typename To, ratio CRatio, not_constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, false, false, false> {
  template<typename Q>
  static constexpr To cast(const Q& q)
  {
    if constexpr (treat_as_floating_point<CRep>) {
      return To(static_cast<To::rep>(q.count() * fpow10<CRep>(CRatio.exp) * (CRatio.num / CRatio.den)));
    } else {
      if constexpr (CRatio.exp > 0) {
        return To(static_cast<To::rep>(q.count() * CRatio.num * ipow10(CRatio.exp) / CRatio.den));
      }
      else {
        return To(static_cast<To::rep>(q.count()) * CRatio.num / (CRatio.den * ipow10(-CRatio.exp)));
      }
    }
  }
};

template<typename To, ratio CRatio, not_constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, true, false, true> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    return To(static_cast<To::rep>(q.count() / CRatio.den));
  }
};

template<typename To, ratio CRatio, not_constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, true, false, false> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    if constexpr (treat_as_floating_point<CRep>) {
      return To(static_cast<To::rep>(q.count() * fpow10<CRep>(CRatio.exp) / CRatio.den));
    } else {
      if constexpr (CRatio.exp > 0) {
        return To(static_cast<To::rep>(q.count() * ipow10(CRatio.exp) / CRatio.den));
      }
      else {
        return To(static_cast<To::rep>(q.count() / (ipow10(-CRatio.exp) * CRatio.den)));
      }
    }
  }
};

template<typename To, ratio CRatio, not_constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, false, true, true> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    return To(static_cast<To::rep>(q.count() * CRatio.num));
  }
};

template<typename To, ratio CRatio, not_constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, false, true, false> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    if constexpr (treat_as_floating_point<CRep>) {
      return To(static_cast<To::rep>(q.count() * CRatio.num * fpow10<CRep>(CRatio.exp)));
    } else {
      if constexpr (CRatio.exp > 0) {
        return To(static_cast<To::rep>(q.count() * CRatio.num * ipow10(CRatio.exp)));
      }
      else {
        return To(static_cast<To::rep>(q.count() * CRatio.num / ipow10(-CRatio.exp)));
      }
    }
  }
};

template<typename Q1, typename Q2>
constexpr ratio cast_ratio(const Q1& from, const Q2& to)
{
  using FromU = Q1::unit;
  using ToU = Q2::unit;
  if constexpr(same_unit_reference<FromU, ToU>::value) {
    return FromU::ratio / ToU::ratio;
  }
  else {
    return quantity_ratio(from) / quantity_ratio(to);
  }
}

}  // namespace detail

/**
 * @brief Explcit cast of a quantity
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In such cases an explicit cast have to be used.
 *
 * This cast gets the target quantity type to cast to. For example:
 *
 * auto q1 = units::quantity_cast<units::physical::si::time<units::physical::si::second>>(1q_ms);
 *
 * @tparam To a target quantity type to cast to
 */
template<Quantity To, typename D, typename U, typename Rep>
[[nodiscard]] constexpr auto quantity_cast(const quantity<D, U, Rep>& q)
  requires QuantityOf<To, D>
{
  using c_ratio = std::integral_constant<ratio, detail::cast_ratio(quantity<D, U, Rep>(), To())>;
  using c_rep = std::common_type_t<typename To::rep, Rep>;
  using ret_unit = downcast_unit<typename To::dimension, To::unit::ratio>;
  using ret = quantity<typename To::dimension, ret_unit, typename To::rep>;
  using cast = detail::quantity_cast_impl<ret, c_ratio::value, c_rep, c_ratio::value.num == 1, c_ratio::value.den == 1, c_ratio::value.exp == 0>;
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
 * auto q1 = units::quantity_cast<units::physical::si::acceleration>(200q_Gal);
 *
 * @tparam ToD a dimension type to use for a target quantity
 */
template<Dimension ToD, typename D, typename U, typename Rep>
[[nodiscard]] constexpr auto quantity_cast(const quantity<D, U, Rep>& q)
  requires equivalent<ToD, D>
{
  return quantity_cast<quantity<ToD, dimension_unit<ToD>, Rep>>(q);
}

/**
 * @brief Explcit cast of a quantity
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In such cases an explicit cast have to be used.
 *
 * This cast gets only the target unit to cast to. For example:
 *
 * auto q1 = units::quantity_cast<units::physical::si::second>(1q_ms);
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
 * auto q1 = units::quantity_cast<int>(1q_ms);
 *
 * @tparam ToRep a representation type to use for a target quantity
 */
template<Scalar ToRep, typename D, typename U, typename Rep>
[[nodiscard]] constexpr auto quantity_cast(const quantity<D, U, Rep>& q)
{
  return quantity_cast<quantity<D, U, ToRep>>(q);
}

/**
 * @brief Explcit cast of a quantity point
 *
 * Implicit conversions between quantity points of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In other cases an explicit cast has to be used.
 *
 * This cast gets the target quantity point type to cast to or anything that works for quantity_cast. For example:
 *
 * auto q1 = units::quantity_point_cast<decltype(quantity_point{0q_s})>(quantity_point{1q_ms});
 * auto q1 = units::quantity_point_cast<units::physical::si::time<units::physical::si::second>>(quantity_point{1q_ms});
 * auto q1 = units::quantity_point_cast<units::physical::si::acceleration>(quantity_point{200q_Gal});
 * auto q1 = units::quantity_point_cast<units::physical::si::second>(quantity_point{1q_ms});
 * auto q1 = units::quantity_point_cast<int>(quantity_point{1q_ms});
 *
 * @tparam CastSpec a target quantity point type to cast to or anything that works for quantity_cast
 */
template<typename CastSpec, typename D, typename U, typename Rep>
[[nodiscard]] constexpr auto quantity_point_cast(const quantity_point<D, U, Rep>& qp)
  requires is_instantiation_of<CastSpec, quantity_point> ||
           requires(quantity<D, U, Rep> q) { quantity_cast<CastSpec>(q); }
{
  if constexpr (is_instantiation_of<CastSpec, quantity_point>)
    return quantity_point(quantity_cast<typename CastSpec::quantity_type>(qp.relative()));
  else
    return quantity_point(quantity_cast<CastSpec>(qp.relative()));
}

}  // namespace units
