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

#include <units/bits/dimension_op.h>
#include <units/bits/external/type_traits.h>
#include <units/concepts.h>
#include <units/customization_points.h>
#include <units/magnitude.h>
#include <units/reference.h>

UNITS_DIAGNOSTIC_PUSH
// warning C4244: 'argument': conversion from 'intmax_t' to 'T', possible loss of data with T=int
UNITS_DIAGNOSTIC_IGNORE_LOSS_OF_DATA

namespace units {

template<Dimension D, UnitOf<D> U, Representation Rep>
class quantity;

template<PointOrigin O, UnitOf<typename O::dimension> U, Representation Rep>
class quantity_point;

template<Kind K, UnitOf<typename K::dimension> U, Representation Rep>
class quantity_kind;

template<PointKind PK, UnitOf<typename PK::dimension> U, Representation Rep>
class quantity_point_kind;

namespace detail {

template<Quantity Q>
inline constexpr Magnitude auto quantity_magnitude = decltype(Q::reference)::mag;

template<typename QFrom, typename QTo>
inline constexpr Magnitude auto cast_magnitude = [] {
  using FromU = TYPENAME QFrom::unit;
  using ToU = TYPENAME QTo::unit;
  if constexpr (same_unit_reference<FromU, ToU>::value) {
    return FromU::mag / ToU::mag;
  } else {
    return quantity_magnitude<QFrom> / quantity_magnitude<QTo>;
  }
}();

template<typename From, typename To>
struct cast_traits;

template<typename From, typename To>
  requires common_type_with_<std::common_type_t<From, To>, std::intmax_t>
struct cast_traits<From, To> {
  using ratio_type = std::common_type_t<std::common_type_t<From, To>, std::intmax_t>;
  using rep_type = ratio_type;
};

template<typename From, typename To>
  requires(!common_type_with_<std::common_type_t<From, To>, std::intmax_t> &&
           scalable_number_<std::common_type_t<From, To>, std::intmax_t> &&
           requires { typename std::common_type_t<From, To>::value_type; } &&
           common_type_with_<typename std::common_type_t<From, To>::value_type, std::intmax_t>)
struct cast_traits<From, To> {
  using ratio_type = std::common_type_t<typename std::common_type_t<From, To>::value_type, std::intmax_t>;
  using rep_type = std::common_type_t<From, To>;
};

template<typename From, typename To>
  requires(!common_type_with_<std::common_type_t<From, To>, std::intmax_t> &&
           scalable_number_<std::common_type_t<From, To>, std::intmax_t> &&
           requires { typename std::common_type_t<From, To>::element_type; } &&
           common_type_with_<typename std::common_type_t<From, To>::element_type, std::intmax_t>)
struct cast_traits<From, To> {
  using ratio_type = std::common_type_t<typename std::common_type_t<From, To>::element_type, std::intmax_t>;
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
 * auto q1 = units::quantity_cast<units::isq::si::time<units::isq::si::second>>(1_q_ms);
 *
 * @tparam To a target quantity type to cast to
 */
template<Quantity To, typename D, typename U, scalable_with_<typename To::rep> Rep>
  requires QuantityOf<To, D> && (std::constructible_from<typename To::rep, std::common_type_t<typename To::rep, Rep>>)
[[nodiscard]] constexpr auto quantity_cast(const quantity<D, U, Rep>& q)
{
  using traits = detail::cast_traits<Rep, typename To::rep>;
  using ratio_type = TYPENAME traits::ratio_type;
  using rep_type = TYPENAME traits::rep_type;

  constexpr Magnitude auto c_mag = detail::cast_magnitude<quantity<D, U, Rep>, To>;
  constexpr Magnitude auto num = numerator(c_mag);
  constexpr Magnitude auto den = denominator(c_mag);
  constexpr Magnitude auto irr = c_mag * (den / num);

  constexpr auto val = [](Magnitude auto m) { return get_value<ratio_type>(m); };
  return To(static_cast<TYPENAME To::rep>(static_cast<rep_type>(q.number()) * val(num) / val(den) * val(irr)));
}

/**
 * @brief Explicit cast of a quantity
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In such cases an explicit cast have to be used.
 *
 * This cast gets only the target dimension to cast to. For example:
 *
 * auto q1 = units::quantity_cast<units::isq::si::dim_acceleration>(200_q_Gal);
 *
 * @tparam ToD a dimension type to use for a target quantity
 */
template<Dimension ToD, typename D, typename U, typename Rep>
  requires equivalent<ToD, D>
[[nodiscard]] constexpr auto quantity_cast(const quantity<D, U, Rep>& q)
{
  return quantity_cast<quantity<ToD, downcast_unit<ToD, U::mag>, Rep>>(q);
}

/**
 * @brief Explicit cast of a quantity
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In such cases an explicit cast have to be used.
 *
 * This cast gets only the target unit to cast to. For example:
 *
 * auto q1 = units::quantity_cast<units::isq::si::second>(1_q_ms);
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
 * auto q1 = units::quantity_cast<units::isq::si::dim_speed, units::isq::si::kilometre_per_hour>(v1);
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
template<Representation ToRep, typename D, typename U, scalable_with_<ToRep> Rep>
  requires(std::constructible_from<ToRep, std::common_type_t<ToRep, Rep>>)
[[nodiscard]] constexpr auto quantity_cast(const quantity<D, U, Rep>& q)
{
  return quantity_cast<quantity<D, U, ToRep>>(q);
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
 * auto q1 = units::quantity_point_cast<units::isq::si::time<units::isq::si::second>>(quantity_point{1_q_ms});
 * auto q1 = units::quantity_point_cast<units::isq::si::dim_acceleration>(quantity_point{200_q_Gal});
 * auto q1 = units::quantity_point_cast<units::isq::si::second>(quantity_point{1_q_ms});
 * auto q1 = units::quantity_point_cast<int>(quantity_point{1_q_ms});
 *
 * @tparam CastSpec a target quantity point type to cast to or anything that works for quantity_cast
 */
template<typename CastSpec, typename O, typename U, typename Rep>
[[nodiscard]] constexpr auto quantity_point_cast(const quantity_point<O, U, Rep>& qp)
  requires requires {
    requires is_specialization_of<CastSpec, quantity_point>;
    requires requires { quantity_cast<typename CastSpec::quantity_type>(qp.relative()); };
    requires equivalent<O, typename CastSpec::origin>;
  } ||  // TODO: Simplify when Clang catches up.
           requires { quantity_cast<CastSpec>(qp.relative()); }
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
 * auto q1 = units::quantity_point_cast<units::isq::si::dim_speed, units::isq::si::kilometre_per_hour>(v1);
 *
 * @note This cast is especially useful when working with quantity points of unknown dimensions
 * (@c unknown_dimension).
 *
 * @tparam ToD a dimension type to use for a target quantity
 * @tparam ToU a unit type to use for a target quantity
 */
template<Dimension ToD, Unit ToU, typename O, typename U, typename Rep>
  requires equivalent<ToD, typename O::dimension> && UnitOf<ToU, ToD> && RebindablePointOriginFor<O, ToD>
[[nodiscard]] constexpr auto quantity_point_cast(const quantity_point<O, U, Rep>& q)
{
  return quantity_point_cast<quantity_point<rebind_point_origin_dimension<O, ToD>, ToU, Rep>>(q);
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
 * auto q1 = units::quantity_kind_cast<units::isq::si::length<units::isq::si::metre>>(ns::width{1 * mm});
 * auto q1 = units::quantity_kind_cast<units::isq::si::dim_acceleration>(ns::rate_of_climb{200 * Gal});
 * auto q1 = units::quantity_kind_cast<units::isq::si::metre>(ns::width{1 * mm});
 * auto q1 = units::quantity_kind_cast<int>(ns::width{1.0 * mm});
 *
 * @tparam CastSpec a target (quantity) kind type to cast to or anything that works for quantity_cast
 */
template<typename CastSpec, typename K, typename U, typename Rep>
[[nodiscard]] constexpr QuantityKind auto quantity_kind_cast(const quantity_kind<K, U, Rep>& qk)
  requires requires {
    requires is_specialization_of<CastSpec, quantity_kind>;
    requires requires { quantity_cast<typename CastSpec::quantity_type>(qk.common()); };
  } || requires {
    requires Kind<CastSpec>;
    requires UnitOf<U, typename CastSpec::dimension>;
  } || requires { quantity_cast<CastSpec>(qk.common()); }  // TODO: Simplify when Clang catches up.
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
 * auto q1 = units::quantity_kind_cast<ns::height_kind, units::isq::si::kilometre>(w);
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
 * This cast gets the target (quantity) point kind type to cast to or anything that works for quantity_kind_cast. For
 * example:
 *
 * auto q1 = units::quantity_point_kind_cast<decltype(ns::x_coordinate{1 * m))>(ns::x_coordinate{1 * mm});
 * auto q1 = units::quantity_point_kind_cast<decltype(ns::width{1 * m})>(ns::x_coordinate{1 * mm});
 * auto q1 = units::quantity_point_kind_cast<ns::y_coordinate_kind>(ns::x_coordinate{1 * m});
 * auto q1 = units::quantity_point_kind_cast<ns::height_kind>(ns::x_coordinate{1 * m});
 * auto q1 = units::quantity_point_kind_cast<units::isq::si::length<units::isq::si::metre>>(ns::x_coordinate{1 * mm});
 * auto q1 = units::quantity_point_kind_cast<units::isq::si::dim_acceleration>(quantity_point_kind(ns::rate_of_climb{200
 * * Gal})); auto q1 = units::quantity_point_kind_cast<units::isq::si::metre>(ns::x_coordinate{1 * mm}); auto q1 =
 * units::quantity_point_kind_cast<int>(ns::x_coordinate{1.0 * mm});
 *
 * @tparam CastSpec a target (quantity) point kind type to cast to or anything that works for quantity_kind_cast
 */
template<typename CastSpec, typename PK, typename U, typename Rep>
[[nodiscard]] constexpr QuantityPointKind auto quantity_point_kind_cast(const quantity_point_kind<PK, U, Rep>& qpk)
  requires requires {
    requires is_specialization_of<CastSpec, quantity_point_kind>;
    requires requires { quantity_kind_cast<typename CastSpec::quantity_kind_type>(qpk.relative()); };
    requires equivalent<typename PK::origin, typename CastSpec::point_kind_type::origin>;
  } || requires { requires PointKind<CastSpec> && UnitOf<U, typename CastSpec::dimension>; } ||
           requires { quantity_kind_cast<CastSpec>(qpk.relative()); }  // TODO: Simplify when Clang catches up.
{
  if constexpr (is_specialization_of<CastSpec, quantity_point_kind>)
    return CastSpec(quantity_kind_cast<typename CastSpec::quantity_kind_type>(qpk.relative()));
  else if constexpr (PointKind<CastSpec>)
    return quantity_point_kind(quantity_kind_cast<typename CastSpec::base_kind>(qpk.relative()));
  else
    return quantity_point_kind(quantity_kind_cast<CastSpec>(qpk.relative()));
}

/**
 * @brief Explicit cast of a quantity point kind
 *
 * Implicit conversions between quantity point kinds of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In other cases an explicit cast has to be used.
 *
 * This cast gets both the target point kind and unit to cast to. For example:
 *
 * auto q1 = units::quantity_point_kind_cast<ns::y_coordinate_kind, units::isq::si::kilometre>(x);
 *
 * @note This cast is especially useful when working with quantity point kinds of unknown point kind.
 *
 * @tparam ToPK the point kind type to use for the target quantity
 * @tparam ToU the unit type to use for the target quantity
 */
template<PointKind ToPK, Unit ToU, typename PK, typename U, typename Rep>
  requires equivalent<typename ToPK::dimension, typename PK::dimension> && UnitOf<ToU, typename ToPK::dimension>
[[nodiscard]] constexpr QuantityPointKind auto quantity_point_kind_cast(const quantity_point_kind<PK, U, Rep>& qpk)
{
  return quantity_point_kind_cast<quantity_point_kind<ToPK, ToU, Rep>>(qpk);
}

}  // namespace units

UNITS_DIAGNOSTIC_POP
