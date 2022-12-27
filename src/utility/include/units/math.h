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

#include <units/angular/angular.h>
#include <units/bits/external/hacks.h>
#include <units/isq/space_and_time.h>
#include <units/quantity.h>
#include <units/si/units.h>
#include <units/unit.h>

// IWYU pragma: begin_exports
#include <cmath>
#include <cstdint>
// IWYU pragma: end_exports

#include <limits>

namespace units {

/**
 * @brief Computes the value of a quantity raised to the `Num/Den` power
 *
 * Both the quantity value and its quantity specification are the base of the operation.
 *
 * @tparam Num Exponent numerator
 * @tparam Den Exponent denominator
 * @param q Quantity being the base of the operation
 * @return Quantity The result of computation
 */
template<std::intmax_t Num, std::intmax_t Den = 1, Quantity Q>
  requires detail::non_zero<Den>
[[nodiscard]] inline auto pow(const Q& q) noexcept
  requires requires { pow(q.number(), 1.0); } || requires { std::pow(q.number(), 1.0); }
{
  using rep = TYPENAME Q::rep;
  if constexpr (Num == 0) {
    return rep(1);
  } else if constexpr (ratio{Num, Den} == 1) {
    return q;
  } else {
    using std::pow;
    return quantity<reference<pow<Num, Den>(Q::quantity_spec), pow<Num, Den>(Q::unit)>{}, rep>(
      static_cast<rep>(pow(q.number(), static_cast<double>(Num) / static_cast<double>(Den))));
  }
}

/**
 * @brief Computes the square root of a quantity
 *
 * Both the quantity value and its quantity specification are the base of the operation.
 *
 * @param q Quantity being the base of the operation
 * @return Quantity The result of computation
 */
template<Quantity Q>
[[nodiscard]] inline Quantity auto sqrt(const Q& q) noexcept
  requires requires { sqrt(q.number()); } || requires { std::sqrt(q.number()); }
{
  using rep = TYPENAME Q::rep;
  using std::sqrt;
  return quantity<reference<pow<1, 2>(Q::quantity_spec), pow<1, 2>(Q::unit)>{}, rep>(
    static_cast<rep>(sqrt(q.number())));
}

/**
 * @brief Computes the cubic root of a quantity
 *
 * Both the quantity value and its quantity specification are the base of the operation.
 *
 * @param q Quantity being the base of the operation
 * @return Quantity The result of computation
 */
template<Quantity Q>
[[nodiscard]] inline Quantity auto cbrt(const Q& q) noexcept
  requires requires { cbrt(q.number()); } || requires { std::cbrt(q.number()); }
{
  using rep = TYPENAME Q::rep;
  using std::cbrt;
  return quantity<reference<pow<1, 3>(Q::quantity_spec), pow<1, 3>(Q::unit)>{}, rep>(
    static_cast<rep>(cbrt(q.number())));
}

/**
 * @brief Computes Euler's raised to the given power
 *
 * @note Such an operation has sense only for a dimensionless quantity.
 *
 * @param q Quantity being the base of the operation
 * @return Quantity The value of the same quantity type
 */
template<quantity_of<dimension_one> Q>
[[nodiscard]] inline Q exp(const Q& q)
  requires requires { exp(q.number()); } || requires { std::exp(q.number()); }
{
  using std::exp;
  return quantity_cast<Q::unit>(exp(quantity_cast<one>(q).number()) * dimensionless[one]);
}

/**
 * @brief Computes the absolute value of a quantity
 *
 * @param q Quantity being the base of the operation
 * @return Quantity The absolute value of a provided quantity
 */
template<Quantity Q>
[[nodiscard]] inline Q abs(const Q& q) noexcept
  requires requires { abs(q.number()); } || requires { std::abs(q.number()); }
{
  using std::abs;
  return Q(abs(q.number()));
}

/**
 * @brief Returns the epsilon of the quantity
 *
 * The returned value is defined by a <tt>std::numeric_limits<typename Q::rep>::epsilon()</tt>.
 *
 * @tparam Q Quantity type being the base of the operation
 * @return Quantity The epsilon value for quantity's representation type
 */
template<Representation Rep, Reference R>
  requires requires { std::numeric_limits<Rep>::epsilon(); }
[[nodiscard]] constexpr Quantity auto epsilon(R r) noexcept
{
  return std::numeric_limits<Rep>::epsilon() * r;
}

/**
 * @brief Computes the largest quantity with integer representation and unit type To with its number not greater than q
 *
 * @tparam q Quantity being the base of the operation
 * @return Quantity The rounded quantity with unit type To
 */
template<Unit auto To, auto R, typename Rep>
[[nodiscard]] constexpr quantity<reference<R.quantity_spec, To>{}, Rep> floor(const quantity<R, Rep>& q) noexcept
  requires((!treat_as_floating_point<Rep>) || requires { floor(q.number()); } ||
           requires { std::floor(q.number()); }) &&
          (To == R.unit || requires {
            ::units::quantity_cast<To>(q);
            quantity<reference<R.quantity_spec, To>{}, Rep>::one();
          })
{
  const auto handle_signed_results = [&]<typename T>(const T& res) {
    if (res > q) {
      return res - T::one();
    }
    return res;
  };
  if constexpr (treat_as_floating_point<Rep>) {
    using std::floor;
    if constexpr (To == R.unit) {
      return quantity<reference<R.quantity_spec, To>{}, Rep>(floor(q.number()));
    } else {
      return handle_signed_results(
        quantity<reference<R.quantity_spec, To>{}, Rep>(floor(quantity_cast<To>(q).number())));
    }
  } else {
    if constexpr (To == R.unit) {
      return quantity_cast<To>(q);
    } else {
      return handle_signed_results(quantity_cast<To>(q));
    }
  }
}

/**
 * @brief Computes the smallest quantity with integer representation and unit type To with its number not less than q
 *
 * @tparam q Quantity being the base of the operation
 * @return Quantity The rounded quantity with unit type To
 */
template<Unit auto To, auto R, typename Rep>
[[nodiscard]] constexpr quantity<reference<R.quantity_spec, To>{}, Rep> ceil(const quantity<R, Rep>& q) noexcept
  requires((!treat_as_floating_point<Rep>) || requires { ceil(q.number()); } || requires { std::ceil(q.number()); }) &&
          (To == R.unit || requires {
            ::units::quantity_cast<To>(q);
            quantity<reference<R.quantity_spec, To>{}, Rep>::one();
          })
{
  const auto handle_signed_results = [&]<typename T>(const T& res) {
    if (res < q) {
      return res + T::one();
    }
    return res;
  };
  if constexpr (treat_as_floating_point<Rep>) {
    using std::ceil;
    if constexpr (To == R.unit) {
      return quantity<reference<R.quantity_spec, To>{}, Rep>(ceil(q.number()));
    } else {
      return handle_signed_results(
        quantity<reference<R.quantity_spec, To>{}, Rep>(ceil(quantity_cast<To>(q).number())));
    }
  } else {
    if constexpr (To == R.unit) {
      return quantity_cast<To>(q);
    } else {
      return handle_signed_results(quantity_cast<To>(q));
    }
  }
}

/**
 * @brief Computes the nearest quantity with integer representation and unit type To to q
 *
 * Rounding halfway cases away from zero, regardless of the current rounding mode.
 *
 * @tparam q Quantity being the base of the operation
 * @return Quantity The rounded quantity with unit type To
 */
template<Unit auto To, auto R, typename Rep>
[[nodiscard]] constexpr quantity<reference<R.quantity_spec, To>{}, Rep> round(const quantity<R, Rep>& q) noexcept
  requires((!treat_as_floating_point<Rep>) || requires { round(q.number()); } ||
           requires { std::round(q.number()); }) &&
          (To == R.unit || requires {
            ::units::floor<To>(q);
            quantity<reference<R.quantity_spec, To>{}, Rep>::one();
          })
{
  if constexpr (To == R.unit) {
    if constexpr (treat_as_floating_point<Rep>) {
      using std::round;
      return quantity<reference<R.quantity_spec, To>{}, Rep>(round(q.number()));
    } else {
      return quantity_cast<To>(q);
    }
  } else {
    const auto res_low = units::floor<To>(q);
    const auto res_high = res_low + res_low.one();
    const auto diff0 = q - res_low;
    const auto diff1 = res_high - q;
    if (diff0 == diff1) {
      if (static_cast<int>(res_low.number()) & 1) {
        return res_high;
      }
      return res_low;
    }
    if (diff0 < diff1) {
      return res_low;
    }
    return res_high;
  }
}

/**
 * @brief Computes the square root of the sum of the squares of x and y,
 *        without undue overflow or underflow at intermediate stages of the computation
 */
template<Quantity Q1, Quantity Q2>
[[nodiscard]] inline quantity_of<common_reference(Q1::reference, Q2::reference)> auto hypot(const Q1& x,
                                                                                            const Q2& y) noexcept
  requires requires { common_reference(Q1::reference, Q2::reference); } &&
           (
             requires { hypot(x.number(), y.number()); } || requires { std::hypot(x.number(), y.number()); })
{
  using std::hypot;
  using type = quantity<common_reference(Q1::reference, Q2::reference), decltype(hypot(x.number(), y.number()))>;
  return type{hypot(type{x}.number(), type{y}.number())};
}

/**
 * @brief Computes the square root of the sum of the squares of x, y, and z,
 *        without undue overflow or underflow at intermediate stages of the computation
 */
template<Quantity Q1, Quantity Q2, Quantity Q3>
[[nodiscard]] inline quantity_of<common_reference(Q1::reference, Q2::reference, Q3::reference)> auto hypot(
  const Q1& x, const Q2& y, const Q3& z) noexcept
  requires requires { common_reference(Q1::reference, Q2::reference, Q3::reference); } &&
           (
             requires { hypot(x.number(), y.number(), z.number()); } ||
             requires { std::hypot(x.number(), y.number(), z.number()); })
{
  using std::hypot;
  using type = quantity<common_reference(Q1::reference, Q2::reference, Q3::reference),
                        decltype(hypot(x.number(), y.number(), z.number()))>;
  return type{hypot(type{x}.number(), type{y}.number(), type{z}.number())};
}

namespace isq {

template<weak_quantity_of<angular_measure> Q>
  requires treat_as_floating_point<typename Q::rep>
[[nodiscard]] inline quantity_of<dimensionless[one]> auto sin(const Q& q) noexcept
  requires requires { sin(q.number()); } || requires { std::sin(q.number()); }
{
  using std::sin;
  return quantity{sin(q[si::radian].number())};
}

template<weak_quantity_of<angular_measure> Q>
  requires treat_as_floating_point<typename Q::rep>
[[nodiscard]] inline quantity_of<dimensionless[one]> auto cos(const Q& q) noexcept
  requires requires { cos(q.number()); } || requires { std::cos(q.number()); }
{
  using std::cos;
  return quantity{cos(q[si::radian].number())};
}

template<weak_quantity_of<angular_measure> Q>
  requires treat_as_floating_point<typename Q::rep>
[[nodiscard]] inline quantity_of<dimensionless[one]> auto tan(const Q& q) noexcept
  requires requires { tan(q.number()); } || requires { std::tan(q.number()); }
{
  using std::tan;
  return quantity{tan(q[si::radian].number())};
}

template<quantity_of<dimension_one> Q>
  requires treat_as_floating_point<typename Q::rep>
[[nodiscard]] inline quantity_of<angular_measure[si::radian]> auto asin(const Q& q) noexcept
  requires requires { asin(q.number()); } || requires { std::asin(q.number()); }
{
  using std::asin;
  return asin(quantity_cast<one>(q).number()) * angular_measure[si::radian];
}

template<quantity_of<dimension_one> Q>
  requires treat_as_floating_point<typename Q::rep>
[[nodiscard]] inline quantity_of<angular_measure[si::radian]> auto acos(const Q& q) noexcept
  requires requires { acos(q.number()); } || requires { std::acos(q.number()); }
{
  using std::acos;
  return acos(quantity_cast<one>(q).number()) * angular_measure[si::radian];
}

template<quantity_of<dimension_one> Q>
  requires treat_as_floating_point<typename Q::rep>
[[nodiscard]] inline quantity_of<angular_measure[si::radian]> auto atan(const Q& q) noexcept
  requires requires { atan(q.number()); } || requires { std::atan(q.number()); }
{
  using std::atan;
  return atan(quantity_cast<one>(q).number()) * angular_measure[si::radian];
}

}  // namespace isq

namespace angular {

// TODO cannot use `weak_quantity_of<angle>` as it is not interconvertible with `isq_angle::angular_measure`
template<quantity_of<dim_angle> Q>
  requires treat_as_floating_point<typename Q::rep>
[[nodiscard]] inline quantity_of<dimensionless[one]> auto sin(const Q& q) noexcept
  requires requires { sin(q.number()); } || requires { std::sin(q.number()); }
{
  using std::sin;
  return quantity{sin(q[radian].number())};
}

template<quantity_of<dim_angle> Q>
  requires treat_as_floating_point<typename Q::rep>
[[nodiscard]] inline quantity_of<dimensionless[one]> auto cos(const Q& q) noexcept
  requires requires { cos(q.number()); } || requires { std::cos(q.number()); }
{
  using std::cos;
  return quantity{cos(q[radian].number())};
}

template<quantity_of<dim_angle> Q>
  requires treat_as_floating_point<typename Q::rep>
[[nodiscard]] inline quantity_of<dimensionless[one]> auto tan(const Q& q) noexcept
  requires requires { tan(q.number()); } || requires { std::tan(q.number()); }
{
  using std::tan;
  return quantity{tan(q[radian].number())};
}

template<quantity_of<dimension_one> Q>
  requires treat_as_floating_point<typename Q::rep>
[[nodiscard]] inline quantity_of<angle[radian]> auto asin(const Q& q) noexcept
  requires requires { asin(q.number()); } || requires { std::asin(q.number()); }
{
  using std::asin;
  return asin(quantity_cast<one>(q).number()) * angle[radian];
}

template<quantity_of<dimension_one> Q>
  requires treat_as_floating_point<typename Q::rep>
[[nodiscard]] inline quantity_of<angle[radian]> auto acos(const Q& q) noexcept
  requires requires { acos(q.number()); } || requires { std::acos(q.number()); }
{
  using std::acos;
  return acos(quantity_cast<one>(q).number()) * angle[radian];
}

template<quantity_of<dimension_one> Q>
  requires treat_as_floating_point<typename Q::rep>
[[nodiscard]] inline quantity_of<angle[radian]> auto atan(const Q& q) noexcept
  requires requires { atan(q.number()); } || requires { std::atan(q.number()); }
{
  using std::atan;
  return atan(quantity_cast<one>(q).number()) * angle[radian];
}

}  // namespace angular

}  // namespace units
