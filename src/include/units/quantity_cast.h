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
#include <units/bits/pow.h>
#include <cassert>

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable:4244) // warning C4244: 'argument': conversion from 'intmax_t' to 'T', possible loss of data with T=int
#endif //_MSC_VER

namespace units {

template<Dimension D, UnitOf<D> U, QuantityValue Rep>
class quantity;

template<Dimension D, UnitOf<D> U, QuantityValue Rep>
class quantity_point;

namespace detail {

template<QuantityLike Q>
constexpr auto quantity_ratio(const Q&)
{
  using traits = quantity_traits<Q>;
  using dim = TYPENAME traits::dimension;
  using unit = TYPENAME traits::unit;
  if constexpr(BaseDimension<dim>) {
    return unit::ratio;
  }
  else {
    return dim::base_units_ratio * unit::ratio / dim::coherent_unit::ratio;
  }
}

template<typename Q1, typename Q2>
constexpr ratio cast_ratio(const Q1& from, const Q2& to)
{
  using FromU = TYPENAME Q1::unit;
  using ToU = TYPENAME Q2::unit;
  if constexpr(same_unit_reference<FromU, ToU>::value) {
    return FromU::ratio / ToU::ratio;
  }
  else {
    return quantity_ratio(from) / quantity_ratio(to);
  }
}

template<typename From, typename To>
struct cast_traits;

template<typename From, typename To>
  requires common_type_with_<std::common_type_t<From, To>, std::intmax_t>
struct cast_traits<From, To> {
  using ratio_type = std::common_type_t<std::common_type_t<From, To>, std::intmax_t>;
  using rep_type = ratio_type;
};

template<typename From, typename To>
  requires (!common_type_with_<std::common_type_t<From, To>, std::intmax_t>) &&
          scalable_number_<std::common_type_t<From, To>, std::intmax_t> && 
          requires { typename std::common_type_t<From, To>::value_type; } &&
          common_type_with_<typename std::common_type_t<From, To>::value_type, std::intmax_t>
struct cast_traits<From, To> {
  using ratio_type = std::common_type_t<typename std::common_type_t<From, To>::value_type, std::intmax_t>;
  using rep_type = std::common_type_t<From, To>;
};

}  // namespace detail

/**
 * @brief Explicit cast of a quantity
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In such cases an explicit cast have to be used.
 *
 * This cast gets the target quantity type to cast to. For example:
 *
 * auto q1 = units::quantity_cast<units::physical::si::time<units::physical::si::second>>(1_q_ms);
 *
 * @tparam To a target quantity type to cast to
 */
template<Quantity To, QuantityLike From>
  requires QuantityOf<To, typename quantity_traits<From>::dimension> &&
           scalable_with_<typename quantity_traits<From>::rep, typename To::rep>
[[nodiscard]] constexpr auto quantity_cast(const From& q)
{
  using traits = quantity_traits<From>;
  using from_dim = TYPENAME traits::dimension;
  using from_unit = TYPENAME traits::unit;
  using from_rep = TYPENAME traits::rep;
  using ret_unit = downcast_unit<typename To::dimension, To::unit::ratio>;
  using ret = quantity<typename To::dimension, ret_unit, typename To::rep>;
  using cast_traits = detail::cast_traits<from_rep, typename To::rep>;
  using ratio_type = TYPENAME cast_traits::ratio_type;
  using rep_type = TYPENAME cast_traits::rep_type;
  constexpr auto c_ratio = detail::cast_ratio(quantity<from_dim, from_unit, from_rep>(), To());

  if constexpr (treat_as_floating_point<rep_type>) {
    return ret(static_cast<TYPENAME To::rep>(static_cast<rep_type>(traits::count(q)) *
                              (static_cast<ratio_type>(c_ratio.num) * detail::fpow10<ratio_type>(c_ratio.exp) / static_cast<ratio_type>(c_ratio.den))));
  }
  else {
    if constexpr (c_ratio.exp > 0) {
      return ret(static_cast<TYPENAME To::rep>(static_cast<rep_type>(traits::count(q)) *
                              (static_cast<ratio_type>(c_ratio.num) * static_cast<ratio_type>(detail::ipow10(c_ratio.exp))) /
                              static_cast<ratio_type>(c_ratio.den)));
    }
    else {
      return ret(static_cast<TYPENAME To::rep>(static_cast<rep_type>(traits::count(q)) *
                              static_cast<ratio_type>(c_ratio.num) /
                              (static_cast<ratio_type>(c_ratio.den) * static_cast<ratio_type>(detail::ipow10(-c_ratio.exp)))));
    }
  }
}

/**
 * @brief Explicit cast of a quantity
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In such cases an explicit cast have to be used.
 *
 * This cast gets only the target dimension to cast to. For example:
 *
 * auto q1 = units::quantity_cast<units::physical::si::acceleration>(200_q_Gal);
 *
 * @tparam ToD a dimension type to use for a target quantity
 */
template<Dimension ToD, QuantityLike From>
  requires equivalent<ToD, typename quantity_traits<From>::dimension>
[[nodiscard]] constexpr auto quantity_cast(const From& q)
{
  return quantity_cast<quantity<ToD, dimension_unit<ToD>, typename quantity_traits<From>::rep>>(q);
}

/**
 * @brief Explicit cast of a quantity
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In such cases an explicit cast have to be used.
 *
 * This cast gets only the target unit to cast to. For example:
 *
 * auto q1 = units::quantity_cast<units::physical::si::second>(1_q_ms);
 *
 * @tparam ToU a unit type to use for a target quantity
 */
template<Unit ToU, QuantityLike From>
  requires UnitOf<ToU, typename quantity_traits<From>::dimension>
[[nodiscard]] constexpr auto quantity_cast(const From& q)
{
  return quantity_cast<quantity<typename quantity_traits<From>::dimension, ToU, typename quantity_traits<From>::rep>>(q);
}

/**
 * @brief Explicit cast of a quantity
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In such cases an explicit cast have to be used.
 *
 * This cast gets both the target dimension and unit to cast to. For example:
 *
 * auto q1 = units::quantity_cast<units::physical::si::dim_speed, units::physical::si::kilometre_per_hour>(v1);
 *
 * @note This cast is especially useful when working with quantities of unknown dimensions
 * (@c unknown_dimension).
 * 
 * @tparam ToD a dimension type to use for a target quantity
 * @tparam ToU a unit type to use for a target quantity
 */
template<Dimension ToD, Unit ToU, QuantityLike From>
  requires equivalent<ToD, typename quantity_traits<From>::dimension> && UnitOf<ToU, ToD>
[[nodiscard]] constexpr auto quantity_cast(const From& q)
{
  return quantity_cast<quantity<ToD, ToU, typename quantity_traits<From>::rep>>(q);
}

/**
 * @brief Explicit cast of a quantity
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In such cases an explicit cast have to be used.
 *
 * This cast gets only representation to cast to. For example:
 *
 * auto q1 = units::quantity_cast<int>(1_q_ms);
 *
 * @tparam ToRep a representation type to use for a target quantity
 */
template<QuantityValue ToRep, QuantityLike From>
  requires scalable_with_<typename quantity_traits<From>::rep, ToRep>
[[nodiscard]] constexpr auto quantity_cast(const From& q)
{
  return quantity_cast<quantity<typename quantity_traits<From>::dimension, typename quantity_traits<From>::unit, ToRep>>(q);
}

/**
 * @brief Explicit cast of a quantity point
 *
 * Implicit conversions between quantity points of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In other cases an explicit cast has to be used.
 *
 * This cast gets the target quantity point type to cast to or anything that works for quantity_cast. For example:
 *
 * auto q1 = units::quantity_point_cast<decltype(quantity_point{0_q_s})>(quantity_point{1_q_ms});
 * auto q1 = units::quantity_point_cast<units::physical::si::time<units::physical::si::second>>(quantity_point{1_q_ms});
 * auto q1 = units::quantity_point_cast<units::physical::si::acceleration>(quantity_point{200_q_Gal});
 * auto q1 = units::quantity_point_cast<units::physical::si::second>(quantity_point{1_q_ms});
 * auto q1 = units::quantity_point_cast<int>(quantity_point{1_q_ms});
 *
 * @tparam CastSpec a target quantity point type to cast to or anything that works for quantity_cast
 */
template<typename CastSpec, typename D, typename U, typename Rep>
  requires is_specialization_of<CastSpec, quantity_point> ||
           requires(quantity<D, U, Rep> q) { quantity_cast<CastSpec>(q); }
[[nodiscard]] constexpr auto quantity_point_cast(const quantity_point<D, U, Rep>& qp)
{
  if constexpr (is_specialization_of<CastSpec, quantity_point>)
    return quantity_point(quantity_cast<typename CastSpec::quantity_type>(qp.relative()));
  else
    return quantity_point(quantity_cast<CastSpec>(qp.relative()));
}

/**
 * @brief Explicit cast of a quantity point
 *
 * Implicit conversions between quantity points of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In other cases an explicit cast has to be used.
 *
 * This cast gets both the target dimension and unit to cast to. For example:
 *
 * auto q1 = units::quantity_point_cast<units::physical::si::dim_speed, units::physical::si::kilometre_per_hour>(v1);
 *
 * @note This cast is especially useful when working with quantity points of unknown dimensions
 * (@c unknown_dimension).
 * 
 * @tparam ToD a dimension type to use for a target quantity
 * @tparam ToU a unit type to use for a target quantity
 */
template<Dimension ToD, Unit ToU, typename D, typename U, typename Rep>
  requires equivalent<ToD, D> && UnitOf<ToU, ToD>
[[nodiscard]] constexpr auto quantity_point_cast(const quantity_point<D, U, Rep>& q)
{
  return quantity_point_cast<quantity_point<ToD, ToU, Rep>>(q);
}

}  // namespace units

#ifdef _MSC_VER
#pragma warning (pop)
#endif //_MSC_VER
