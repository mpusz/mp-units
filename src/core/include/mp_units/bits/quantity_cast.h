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

#include <mp_units/bits/external/type_traits.h>
#include <mp_units/bits/magnitude.h>
#include <mp_units/bits/quantity_concepts.h>
#include <mp_units/customization_points.h>
#include <mp_units/dimension.h>
#include <mp_units/reference.h>
#include <mp_units/unit.h>

UNITS_DIAGNOSTIC_PUSH
// warning C4244: 'argument': conversion from 'intmax_t' to 'T', possible loss of data with T=int
UNITS_DIAGNOSTIC_IGNORE_LOSS_OF_DATA

namespace mp_units {

template<Reference auto R, RepresentationOf<get_quantity_spec(R).character> Rep>
class quantity;

// template<PointOrigin O, UnitOf<typename O::dimension> U, Representation Rep>
// class quantity_point;

/**
 * @brief Explicit cast of a quantity
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In truncating cases an explicit cast have to be used.
 *
 * This cast gets the target quantity type to cast to. For example:
 *
 * auto q1 = 1234. * isq::length[mm];
 * auto q2 = quantity_cast<quantity<isq::height[m], int>>(q1);
 *
 * @tparam To a target quantity type to cast to
 */
template<Quantity To, auto R, typename Rep>
  requires(interconvertible(To::reference, R)) &&
          ((get_unit(R) == To::unit && std::constructible_from<typename To::rep, Rep>) ||
           (get_unit(R) != To::unit))  // && scalable_with_<typename To::rep>))
// TODO how to constrain the second part here?
[[nodiscard]] constexpr auto quantity_cast(const quantity<R, Rep>& q)
{
  if constexpr (get_unit(R) == To::unit) {
    // no scaling of the number needed
    return To(static_cast<TYPENAME To::rep>(q.number()));  // this is the only (and recommended) way to do
                                                           // a truncating conversion on a number, so we are
                                                           // using static_cast to suppress all the compiler
                                                           // warnings on conversions
  } else {
    // scale the number
    using rep_type = decltype([] {
      // determines the best representation type
      if constexpr (requires { typename std::common_type_t<Rep, typename To::rep>; })
        // returns a common type of two representation types if available
        // i.e. `double` and `int` will end up with `double` precision
        return std::common_type_t<Rep, typename To::rep>{};
      else
        return Rep{};
    }());
    using multiplier_type = decltype([] {
      // widen the type to prevent overflows
      using wider_type = decltype(rep_type{} * std::intmax_t{});
      // check if `wider_type` supports scaling operations
      if constexpr (requires(wider_type v) { v* v / v; })
        // if the `wider_type` can handle scaling operations then use it to improve accuracy
        return wider_type{};
      else
        // needed for example for linear algebra where `op/` on matrix types is not available
        return std::intmax_t{};
    }());

    constexpr Magnitude auto c_mag =
      detail::get_canonical_unit(get_unit(R)).mag / detail::get_canonical_unit(To::unit).mag;
    constexpr Magnitude auto num = numerator(c_mag);
    constexpr Magnitude auto den = denominator(c_mag);
    constexpr Magnitude auto irr = c_mag * (den / num);

    constexpr auto val = [](Magnitude auto m) { return get_value<multiplier_type>(m); };
    return To(static_cast<TYPENAME To::rep>(static_cast<rep_type>(q.number()) * val(num) / val(den) * val(irr)));
  }
}

/**
 * @brief Explicit cast of a quantity
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In truncating cases an explicit cast have to be used.
 *
 * This cast gets only the target quantity specification to cast to. For example:
 *
 * auto v = quantity_cast<isq::velocity>(120 * isq::length[km] / (2 * isq::time[h]));
 *
 * @tparam ToQS a quantity specification to use for a target quantity
 */
template<QuantitySpec auto ToQS, auto R, typename Rep>
  requires(interconvertible(ToQS, get_quantity_spec(R)))
[[nodiscard]] constexpr auto quantity_cast(const quantity<R, Rep>& q)
{
  constexpr reference<ToQS, quantity<R, Rep>::unit> r;
  return quantity_cast<quantity<r, Rep>>(q);
}

/**
 * @brief Explicit cast of a quantity
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In truncating cases an explicit cast have to be used.
 *
 * This cast gets only the target unit to cast to. For example:
 *
 * auto d = quantity_cast<si::second>(1234 * isq::time[ms]);
 *
 * @tparam ToU a unit to use for a target quantity
 */
template<Unit auto ToU, auto R, typename Rep>
  requires(interconvertible(ToU, get_unit(R)))
[[nodiscard]] constexpr auto quantity_cast(const quantity<R, Rep>& q)
{
  constexpr reference<quantity<R, Rep>::quantity_spec, ToU> r;
  return quantity_cast<quantity<r, Rep>>(q);
}

/**
 * @brief Explicit cast of a quantity
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In truncating cases an explicit cast have to be used.
 *
 * This cast gets only representation to cast to. For example:
 *
 * auto q = quantity_cast<int>(1.23 * isq::time[ms]);
 *
 * @tparam ToRep a representation type to use for a target quantity
 */
template<Representation ToRep, auto R, typename Rep>
  requires RepresentationOf<ToRep, get_quantity_spec(R).character> && std::constructible_from<ToRep, Rep>
[[nodiscard]] constexpr auto quantity_cast(const quantity<R, Rep>& q)
{
  return quantity_cast<quantity<R, ToRep>>(q);
}

// /**
//  * @brief Explicit cast of a quantity point
//  *
//  * Implicit conversions between quantity points of different types are allowed only for "safe"
//  * (i.e. non-truncating) conversion. In other cases an explicit cast has to be used.
//  *
//  * This cast gets the target quantity point type to cast to or anything that works for quantity_cast. For example:
//  *
//  * auto q1 = mp_units::quantity_point_cast<decltype(quantity_point{0_q_s})>(quantity_point{1_q_ms});
//  * auto q1 =
//  mp_units::quantity_point_cast<mp_units::isq::si::time<mp_units::isq::si::second>>(quantity_point{1_q_ms});
//  * auto q1 = mp_units::quantity_point_cast<mp_units::isq::si::dim_acceleration>(quantity_point{200_q_Gal});
//  * auto q1 = mp_units::quantity_point_cast<mp_units::isq::si::second>(quantity_point{1_q_ms});
//  * auto q1 = mp_units::quantity_point_cast<int>(quantity_point{1_q_ms});
//  *
//  * @tparam CastSpec a target quantity point type to cast to or anything that works for quantity_cast
//  */
// template<typename CastSpec, typename O, typename U, typename Rep>
// [[nodiscard]] constexpr auto quantity_point_cast(const quantity_point<O, U, Rep>& qp)
//   requires requires {
//              requires is_specialization_of<CastSpec, quantity_point>;
//              requires requires { quantity_cast<typename CastSpec::quantity_type>(qp.relative()); };
//              requires equivalent<O, typename CastSpec::origin>;
//            } ||  // TODO: Simplify when Clang catches up.
//            requires { quantity_cast<CastSpec>(qp.relative()); }
// {
//   if constexpr (is_specialization_of<CastSpec, quantity_point>)
//     return quantity_point(quantity_cast<typename CastSpec::quantity_type>(qp.relative()));
//   else
//     return quantity_point(quantity_cast<CastSpec>(qp.relative()));
// }

// /**
//  * @brief Explicit cast of a quantity point
//  *
//  * Implicit conversions between quantity points of different types are allowed only for "safe"
//  * (i.e. non-truncating) conversion. In other cases an explicit cast has to be used.
//  *
//  * This cast gets both the target dimension and unit to cast to. For example:
//  *
//  * auto q1 = mp_units::quantity_point_cast<mp_units::isq::si::dim_speed, mp_units::isq::si::kilometre_per_hour>(v1);
//  *
//  * @note This cast is especially useful when working with quantity points of unknown dimensions
//  * (@c unknown_dimension).
//  *
//  * @tparam ToD a dimension type to use for a target quantity
//  * @tparam ToU a unit type to use for a target quantity
//  */
// template<Dimension ToD, Unit ToU, typename O, typename U, typename Rep>
//   requires equivalent<ToD, typename O::dimension> && UnitOf<ToU, ToD> && RebindablePointOriginFor<O, ToD>
// [[nodiscard]] constexpr auto quantity_point_cast(const quantity_point<O, U, Rep>& q)
// {
//   return quantity_point_cast<quantity_point<rebind_point_origin_dimension<O, ToD>, ToU, Rep>>(q);
// }

}  // namespace mp_units

UNITS_DIAGNOSTIC_POP
