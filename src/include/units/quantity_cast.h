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
#include <units/bits/common_quantity.h>
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

template<Dimension D, UnitOf<D> U, QuantityValue Rep, PointOrigin Orig>
class quantity_point;

template<Kind K, UnitOf<typename K::dimension> U, QuantityValue Rep>
class quantity_kind;

template<PointKind PK, UnitOf<typename PK::dimension> U, QuantityValue Rep, PointOrigin Orig>
class quantity_point_kind;

namespace detail {

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
template<Quantity To, typename D, typename U, scalable_with_<typename To::rep> Rep>
  requires QuantityOf<To, D> && std::constructible_from<typename To::rep, std::common_type_t<typename To::rep, Rep>>
[[nodiscard]] constexpr auto quantity_cast(const quantity<D, U, Rep>& q)
{
  using traits = detail::cast_traits<Rep, typename To::rep>;
  using ratio_type = TYPENAME traits::ratio_type;
  using rep_type = TYPENAME traits::rep_type;
  constexpr auto c_ratio = detail::cast_ratio(quantity<D, U, Rep>(), To());

  if constexpr (treat_as_floating_point<rep_type>) {
    return To(static_cast<TYPENAME To::rep>(static_cast<rep_type>(q.count()) *
                              (static_cast<ratio_type>(c_ratio.num) * detail::fpow10<ratio_type>(c_ratio.exp) / static_cast<ratio_type>(c_ratio.den))));
  }
  else {
    if constexpr (c_ratio.exp > 0) {
      return To(static_cast<TYPENAME To::rep>(static_cast<rep_type>(q.count()) *
                              (static_cast<ratio_type>(c_ratio.num) * static_cast<ratio_type>(detail::ipow10(c_ratio.exp))) /
                              static_cast<ratio_type>(c_ratio.den)));
    }
    else {
      return To(static_cast<TYPENAME To::rep>(static_cast<rep_type>(q.count()) *
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
 * auto q1 = units::quantity_cast<units::physical::si::dim_acceleration>(200_q_Gal);
 *
 * @tparam ToD a dimension type to use for a target quantity
 */
template<Dimension ToD, typename D, typename U, typename Rep>
  requires equivalent<ToD, D>
[[nodiscard]] constexpr auto quantity_cast(const quantity<D, U, Rep>& q)
{
  return quantity_cast<quantity<ToD, downcast_unit<ToD, U::ratio>, Rep>>(q);
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
template<Unit ToU, typename D, typename U, typename Rep>
  requires UnitOf<ToU, D>
[[nodiscard]] constexpr auto quantity_cast(const quantity<D, U, Rep>& q)
{
  return quantity_cast<quantity<D, ToU, Rep>>(q);
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
template<Dimension ToD, Unit ToU, typename D, typename U, typename Rep>
  requires equivalent<ToD, D> && UnitOf<ToU, ToD>
[[nodiscard]] constexpr auto quantity_cast(const quantity<D, U, Rep>& q)
{
  return quantity_cast<quantity<ToD, ToU, Rep>>(q);
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
template<QuantityValue ToRep, typename D, typename U, scalable_with_<ToRep> Rep>
  requires std::constructible_from<ToRep, std::common_type_t<ToRep, Rep>>
[[nodiscard]] constexpr auto quantity_cast(const quantity<D, U, Rep>& q)
{
  return quantity_cast<quantity<D, U, ToRep>>(q);
}

/**
 * @brief Explicit cast of a quantity point
 *
 * Implicit conversions between quantity points of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In other cases an explicit cast has to be used.
 * If the cast re-references the origin in the quantity point, the distance between the origins must be known
 * and fixed; the value will the correctly be adjusted to refer to the same physical point.
 *
 * This cast gets the target quantity point type to cast to, the new point origin or anything that works for quantity_cast.
 * For example:
 *
 * auto q1 = units::quantity_point_cast<decltype(quantity_point{0_q_s})>(quantity_point{1_q_ms});
 * auto q1 = units::quantity_point_cast<decltype(0_qp_deg_F)>(1_qp_deg_C);
 * auto q1 = units::quantity_point_cast<units::physical::si::celsius_temperature_origin>(1_qp_deg_F);
 * auto q1 = units::quantity_point_cast<units::physical::si::time<units::physical::si::second>>(quantity_point{1_q_ms});
 * auto q1 = units::quantity_point_cast<units::physical::si::dim_acceleration>(quantity_point{200_q_Gal});
 * auto q1 = units::quantity_point_cast<units::physical::si::second>(quantity_point{1_q_ms});
 * auto q1 = units::quantity_point_cast<int>(quantity_point{1_q_ms});
 *
 * @tparam CastSpec a target quantity point type to cast to or anything that works for quantity_cast
 */
template<typename CastSpec, typename D, typename U, typename Rep, typename Orig>
  requires (is_specialization_of<CastSpec, quantity_point> && fixed_known_offset<typename CastSpec::origin,Orig>) ||
           (PointOrigin<CastSpec> && fixed_known_offset<CastSpec,Orig>) ||
           requires(quantity<D, U, Rep> q) { quantity_cast<CastSpec>(q); }
[[nodiscard]] constexpr auto quantity_point_cast(const quantity_point<D, U, Rep, Orig>& qp)
{
  if constexpr (is_specialization_of<CastSpec, quantity_point>) {
    using FromQ = quantity<D, U, Rep>;
    using ToQ = TYPENAME CastSpec::quantity_type;
    using Q = std::common_type_t<FromQ, ToQ>;
    using ToOrig = TYPENAME CastSpec::origin;
    return absolute<ToOrig>(quantity_cast<ToQ>(qp.relative() + offset_between_origins<Q, Orig, ToOrig>));
  } else {
    if constexpr (PointOrigin<CastSpec>)
      return quantity_point_cast<quantity_point<D, U, Rep, CastSpec>>(qp);
    else
      return absolute<Orig>(quantity_cast<CastSpec>(qp.relative()));
  }
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
template<Dimension ToD, Unit ToU, typename D, typename U, typename Rep, typename Orig>
  requires equivalent<ToD, D> && UnitOf<ToU, ToD>
[[nodiscard]] constexpr auto quantity_point_cast(const quantity_point<D, U, Rep, Orig>& q)
{
  return quantity_point_cast<quantity_point<ToD, ToU, Rep, Orig>>(q);
}

/**
 * @brief Explicit cast of a quantity kind
 *
 * Implicit conversions between quantity kinds of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In other cases an explicit cast has to be used.
 *
 * This cast gets the target (quantity) kind type to cast to or anything that works for quantity_cast. For example:
 *
 * auto q1 = units::quantity_kind_cast<decltype(ns::width{1 * m})>(quantity_kind{ns::width{1 * mm});
 * auto q1 = units::quantity_kind_cast<ns::height_kind>(ns::width{1 * m});
 * auto q1 = units::quantity_kind_cast<units::physical::si::length<units::physical::si::metre>>(ns::width{1 * mm});
 * auto q1 = units::quantity_kind_cast<units::physical::si::dim_acceleration>(ns::rate_of_climb{200 * Gal});
 * auto q1 = units::quantity_kind_cast<units::physical::si::metre>(ns::width{1 * mm});
 * auto q1 = units::quantity_kind_cast<int>(ns::width{1.0 * mm});
 *
 * @tparam CastSpec a target (quantity) kind type to cast to or anything that works for quantity_cast
 */
template<typename CastSpec, typename K, typename U, typename Rep>
[[nodiscard]] constexpr QuantityKind auto quantity_kind_cast(const quantity_kind<K, U, Rep>& qk)
  requires (is_specialization_of<CastSpec, quantity_kind> &&
              requires { quantity_cast<typename CastSpec::quantity_type>(qk.common()); }) ||
           (Kind<CastSpec> && UnitOf<U, typename CastSpec::dimension>) ||
           requires { quantity_cast<CastSpec>(qk.common()); }
{
  if constexpr (is_specialization_of<CastSpec, quantity_kind>)
    return CastSpec(quantity_cast<typename CastSpec::quantity_type>(qk.common()));
  else if constexpr (Kind<CastSpec>)
    return quantity_kind<CastSpec, U, Rep>(qk.common());
  else {
    auto q{quantity_cast<CastSpec>(qk.common())};
    using Q = decltype(q);
    return quantity_kind<K, typename Q::unit, typename Q::rep>(static_cast<Q&&>(q));
  }
}

/**
 * @brief Explicit cast of a quantity kind
 *
 * Implicit conversions between quantity kinds of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In other cases an explicit cast has to be used.
 *
 * This cast gets both the target kind and unit to cast to. For example:
 *
 * auto q1 = units::quantity_kind_cast<ns::height_kind, units::physical::si::kilometre>(w);
 *
 * @note This cast is especially useful when working with quantity kinds of unknown kind.
 *
 * @tparam ToK the kind type to use for the target quantity
 * @tparam ToU the unit type to use for the target quantity
 */
template<Kind ToK, Unit ToU, typename K, typename U, typename Rep>
  requires equivalent<typename ToK::dimension, typename K::dimension> && UnitOf<ToU, typename ToK::dimension>
[[nodiscard]] constexpr QuantityKind auto quantity_kind_cast(const quantity_kind<K, U, Rep>& qk)
{
  return quantity_kind_cast<quantity_kind<ToK, ToU, Rep>>(qk);
}

/**
 * @brief Explicit cast of a quantity point kind
 *
 * Implicit conversions between quantity point kinds of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In other cases an explicit cast has to be used.
 *
 * This cast gets the target (quantity) point kind type to cast to or anything that works for quantity_kind_cast. For example:
 *
 * auto q1 = units::quantity_point_kind_cast<decltype(ns::x_coordinate{1 * m))>(ns::x_coordinate{1 * mm});
 * auto q1 = units::quantity_point_kind_cast<decltype(ns::width{1 * m})>(ns::x_coordinate{1 * mm});
 * auto q1 = units::quantity_point_kind_cast<ns::y_coordinate_kind>(ns::x_coordinate{1 * m});
 * auto q1 = units::quantity_point_kind_cast<ns::height_kind>(ns::x_coordinate{1 * m});
 * auto q1 = units::quantity_point_kind_cast<units::physical::si::length<units::physical::si::metre>>(ns::x_coordinate{1 * mm});
 * auto q1 = units::quantity_point_kind_cast<units::physical::si::dim_acceleration>(quantity_point_kind(ns::rate_of_climb{200 * Gal}));
 * auto q1 = units::quantity_point_kind_cast<units::physical::si::metre>(ns::x_coordinate{1 * mm});
 * auto q1 = units::quantity_point_kind_cast<int>(ns::x_coordinate{1.0 * mm});
 *
 * @tparam CastSpec a target (quantity) point kind type to cast to or anything that works for quantity_kind_cast
 */
template<typename CastSpec, typename PK, typename U, typename Rep, typename Orig>
[[nodiscard]] constexpr QuantityPointKind auto quantity_point_kind_cast(const quantity_point_kind<PK, U, Rep, Orig>& qpk)
  requires (is_specialization_of<CastSpec, quantity_point_kind> &&
              requires { quantity_kind_cast<typename CastSpec::quantity_kind_type>(qpk.relative()); } &&
              fixed_known_offset<typename CastSpec::origin,Orig>) ||
           (PointOrigin<CastSpec> && fixed_known_offset<CastSpec,Orig>) ||
           (PointKind<CastSpec> && UnitOf<U, typename CastSpec::dimension>) ||
           requires { quantity_kind_cast<CastSpec>(qpk.relative()); }
{
  if constexpr (is_specialization_of<CastSpec, quantity_point_kind>) {
    using FromQ = quantity<typename PK::dimension, U, Rep>;
    using ToQ = TYPENAME CastSpec::quantity_type;
    using ToQK = TYPENAME CastSpec::quantity_kind_type;
    using Q = std::common_type_t<FromQ, ToQ>;
    using QK = quantity_kind<typename PK::base_kind, typename Q::unit, typename Q::rep>;
    using ToOrig = TYPENAME CastSpec::origin;
    return absolute<ToOrig>(quantity_kind_cast<ToQK>(qpk.relative() + QK(offset_between_origins<Q, Orig, ToOrig>)));
  } else {
    if constexpr (PointOrigin<CastSpec>)
      return quantity_point_kind_cast<quantity_point<PK, U, Rep, CastSpec>>(qpk);
    else if constexpr (PointKind<CastSpec>)
      return absolute<Orig>(quantity_kind_cast<typename CastSpec::base_kind>(qpk.relative()));
    else
      return absolute<Orig>(quantity_kind_cast<CastSpec>(qpk.relative()));
  }
}

/**
 * @brief Explicit cast of a quantity point kind
 *
 * Implicit conversions between quantity point kinds of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In other cases an explicit cast has to be used.
 *
 * This cast gets both the target point kind and unit to cast to. For example:
 *
 * auto q1 = units::quantity_point_kind_cast<ns::y_coordinate_kind, units::physical::si::kilometre>(x);
 *
 * @note This cast is especially useful when working with quantity point kinds of unknown point kind.
 *
 * @tparam ToPK the point kind type to use for the target quantity
 * @tparam ToU the unit type to use for the target quantity
 */
template<PointKind ToPK, Unit ToU, typename PK, typename U, typename Rep, typename Orig>
  requires equivalent<typename ToPK::dimension, typename PK::dimension> && UnitOf<ToU, typename ToPK::dimension>
[[nodiscard]] constexpr QuantityPointKind auto quantity_point_kind_cast(const quantity_point_kind<PK, U, Rep, Orig>& qpk)
{
  return quantity_point_kind_cast<quantity_point_kind<ToPK, ToU, Rep, Orig>>(qpk);
}

}  // namespace units

#ifdef _MSC_VER
#pragma warning (pop)
#endif //_MSC_VER
