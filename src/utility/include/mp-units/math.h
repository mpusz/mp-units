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

#include <mp-units/bits/external/hacks.h>
#include <mp-units/bits/value_cast.h>
#include <mp-units/customization_points.h>
#include <mp-units/quantity.h>
#include <mp-units/systems/angular/angular.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si/units.h>
#include <mp-units/unit.h>

// IWYU pragma: begin_exports
#include <cmath>
#include <cstdint>
// IWYU pragma: end_exports

#include <limits>

namespace mp_units {

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
template<std::intmax_t Num, std::intmax_t Den = 1, auto R, typename Rep>
  requires detail::non_zero<Den> &&
           requires { quantity_values<Rep>::one(); }
           [[nodiscard]] constexpr quantity<pow<Num, Den>(R), Rep> pow(const quantity<R, Rep>& q) noexcept
             requires requires { pow(q.number(), 1.0); } || requires { std::pow(q.number(), 1.0); }
{
  if constexpr (Num == 0) {
    return quantity<pow<Num, Den>(R), Rep>::one();
  } else if constexpr (ratio{Num, Den} == 1) {
    return q;
  } else {
    using std::pow;
    return make_quantity<pow<Num, Den>(R)>(
      static_cast<Rep>(pow(q.number(), static_cast<double>(Num) / static_cast<double>(Den))));
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
template<auto R, typename Rep>
[[nodiscard]] constexpr quantity<sqrt(R), Rep> sqrt(const quantity<R, Rep>& q) noexcept
  requires requires { sqrt(q.number()); } || requires { std::sqrt(q.number()); }
{
  using std::sqrt;
  return make_quantity<sqrt(R)>(static_cast<Rep>(sqrt(q.number())));
}

/**
 * @brief Computes the cubic root of a quantity
 *
 * Both the quantity value and its quantity specification are the base of the operation.
 *
 * @param q Quantity being the base of the operation
 * @return Quantity The result of computation
 */
template<auto R, typename Rep>
[[nodiscard]] constexpr quantity<cbrt(R), Rep> cbrt(const quantity<R, Rep>& q) noexcept
  requires requires { cbrt(q.number()); } || requires { std::cbrt(q.number()); }
{
  using std::cbrt;
  return make_quantity<cbrt(R)>(static_cast<Rep>(cbrt(q.number())));
}

/**
 * @brief Computes Euler's raised to the given power
 *
 * @note Such an operation has sense only for a dimensionless quantity.
 *
 * @param q Quantity being the base of the operation
 * @return Quantity The value of the same quantity type
 */
template<ReferenceOf<dimensionless> auto R, typename Rep>
[[nodiscard]] constexpr quantity<R, Rep> exp(const quantity<R, Rep>& q)
  requires requires { exp(q.number()); } || requires { std::exp(q.number()); }
{
  using std::exp;
  return value_cast<get_unit(R)>(
    make_quantity<detail::clone_reference_with<one>(R)>(static_cast<Rep>(exp(value_cast<one>(q).number()))));
}

/**
 * @brief Computes the absolute value of a quantity
 *
 * @param q Quantity being the base of the operation
 * @return Quantity The absolute value of a provided quantity
 */
template<auto R, typename Rep>
[[nodiscard]] constexpr quantity<R, Rep> abs(const quantity<R, Rep>& q) noexcept
  requires requires { abs(q.number()); } || requires { std::abs(q.number()); }
{
  using std::abs;
  return make_quantity<R>(static_cast<Rep>(abs(q.number())));
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
[[nodiscard]] constexpr quantity<R{}, Rep> epsilon(R r) noexcept
{
  return make_quantity<r>(static_cast<Rep>(std::numeric_limits<Rep>::epsilon()));
}

/**
 * @brief Computes the largest quantity with integer representation and unit type To with its number not greater than q
 *
 * @tparam q Quantity being the base of the operation
 * @return Quantity The rounded quantity with unit type To
 */
template<Unit auto To, auto R, typename Rep>
[[nodiscard]] constexpr quantity<detail::clone_reference_with<To>(R), Rep> floor(const quantity<R, Rep>& q) noexcept
  requires((!treat_as_floating_point<Rep>) || requires { floor(q.number()); } ||
           requires { std::floor(q.number()); }) &&
          (To == get_unit(R) || requires {
            ::mp_units::value_cast<To>(q);
            quantity_values<Rep>::one();
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
    if constexpr (To == get_unit(R)) {
      return make_quantity<detail::clone_reference_with<To>(R)>(static_cast<Rep>(floor(q.number())));
    } else {
      return handle_signed_results(make_quantity<detail::clone_reference_with<To>(q.reference)>(
        static_cast<Rep>(floor(value_cast<To>(q).number()))));
    }
  } else {
    if constexpr (To == get_unit(R)) {
      return value_cast<To>(q);
    } else {
      return handle_signed_results(value_cast<To>(q));
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
[[nodiscard]] constexpr quantity<detail::clone_reference_with<To>(R), Rep> ceil(const quantity<R, Rep>& q) noexcept
  requires((!treat_as_floating_point<Rep>) || requires { ceil(q.number()); } || requires { std::ceil(q.number()); }) &&
          (To == get_unit(R) || requires {
            ::mp_units::value_cast<To>(q);
            quantity_values<Rep>::one();
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
    if constexpr (To == get_unit(R)) {
      return make_quantity<detail::clone_reference_with<To>(q.reference)>(static_cast<Rep>(ceil(q.number())));
    } else {
      return handle_signed_results(make_quantity<detail::clone_reference_with<To>(q.reference)>(
        static_cast<Rep>(ceil(value_cast<To>(q).number()))));
    }
  } else {
    if constexpr (To == get_unit(R)) {
      return value_cast<To>(q);
    } else {
      return handle_signed_results(value_cast<To>(q));
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
[[nodiscard]] constexpr quantity<detail::clone_reference_with<To>(R), Rep> round(const quantity<R, Rep>& q) noexcept
  requires((!treat_as_floating_point<Rep>) || requires { round(q.number()); } ||
           requires { std::round(q.number()); }) &&
          (To == get_unit(R) || requires {
            ::mp_units::floor<To>(q);
            quantity_values<Rep>::one();
          })
{
  if constexpr (To == get_unit(R)) {
    if constexpr (treat_as_floating_point<Rep>) {
      using std::round;
      return make_quantity<detail::clone_reference_with<To>(q.reference)>(static_cast<Rep>(round(q.number())));
    } else {
      return value_cast<To>(q);
    }
  } else {
    const auto res_low = mp_units::floor<To>(q);
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
template<auto R1, typename Rep1, auto R2, typename Rep2>
[[nodiscard]] constexpr QuantityOf<get_quantity_spec(common_reference(R1, R2))> auto hypot(
  const quantity<R1, Rep1>& x, const quantity<R2, Rep2>& y) noexcept
  requires requires { common_reference(R1, R2); } &&
           (
             requires { hypot(x.number(), y.number()); } || requires { std::hypot(x.number(), y.number()); })
{
  constexpr auto ref = common_reference(R1, R2);
  constexpr auto unit = get_unit(ref);
  using std::hypot;
  return make_quantity<ref>(hypot(x.number_in(unit), y.number_in(unit)));
}

/**
 * @brief Computes the square root of the sum of the squares of x, y, and z,
 *        without undue overflow or underflow at intermediate stages of the computation
 */
template<auto R1, typename Rep1, auto R2, typename Rep2, auto R3, typename Rep3>
[[nodiscard]] constexpr QuantityOf<get_quantity_spec(common_reference(R1, R2, R3))> auto hypot(
  const quantity<R1, Rep1>& x, const quantity<R2, Rep2>& y, const quantity<R3, Rep3>& z) noexcept
  requires requires { common_reference(R1, R2, R3); } && (
                                                           requires { hypot(x.number(), y.number(), z.number()); } ||
                                                           requires { std::hypot(x.number(), y.number(), z.number()); })
{
  constexpr auto ref = common_reference(R1, R2);
  constexpr auto unit = get_unit(ref);
  using std::hypot;
  return make_quantity<ref>(hypot(x.number_in(unit), y.number_in(unit), z.number_in(unit)));
}

namespace isq {

template<ReferenceOf<angular_measure> auto R, typename Rep>
  requires treat_as_floating_point<Rep>
[[nodiscard]] inline quantity<one, Rep> sin(const quantity<R, Rep>& q) noexcept
  requires requires { sin(q.number()); } || requires { std::sin(q.number()); }
{
  using std::sin;
  return make_quantity<one>(static_cast<Rep>(sin(q.in(si::radian).number())));
}

template<ReferenceOf<angular_measure> auto R, typename Rep>
  requires treat_as_floating_point<Rep>
[[nodiscard]] inline quantity<one, Rep> cos(const quantity<R, Rep>& q) noexcept
  requires requires { cos(q.number()); } || requires { std::cos(q.number()); }
{
  using std::cos;
  return make_quantity<one>(static_cast<Rep>(cos(q.in(si::radian).number())));
}

template<ReferenceOf<angular_measure> auto R, typename Rep>
  requires treat_as_floating_point<Rep>
[[nodiscard]] inline quantity<one, Rep> tan(const quantity<R, Rep>& q) noexcept
  requires requires { tan(q.number()); } || requires { std::tan(q.number()); }
{
  using std::tan;
  return make_quantity<one>(static_cast<Rep>(tan(q.in(si::radian).number())));
}

template<ReferenceOf<dimensionless> auto R, typename Rep>
  requires treat_as_floating_point<Rep>
[[nodiscard]] inline quantity<si::radian, Rep> asin(const quantity<R, Rep>& q) noexcept
  requires requires { asin(q.number()); } || requires { std::asin(q.number()); }
{
  using std::asin;
  return make_quantity<si::radian>(static_cast<Rep>(asin(value_cast<one>(q).number())));
}

template<ReferenceOf<dimensionless> auto R, typename Rep>
  requires treat_as_floating_point<Rep>
[[nodiscard]] inline quantity<si::radian, Rep> acos(const quantity<R, Rep>& q) noexcept
  requires requires { acos(q.number()); } || requires { std::acos(q.number()); }
{
  using std::acos;
  return make_quantity<si::radian>(static_cast<Rep>(acos(value_cast<one>(q).number())));
}

template<ReferenceOf<dimensionless> auto R, typename Rep>
  requires treat_as_floating_point<Rep>
[[nodiscard]] inline quantity<si::radian, Rep> atan(const quantity<R, Rep>& q) noexcept
  requires requires { atan(q.number()); } || requires { std::atan(q.number()); }
{
  using std::atan;
  return make_quantity<si::radian>(static_cast<Rep>(atan(value_cast<one>(q).number())));
}

}  // namespace isq

namespace angular {

template<ReferenceOf<angle> auto R, typename Rep>
  requires treat_as_floating_point<Rep>
[[nodiscard]] inline quantity<one, Rep> sin(const quantity<R, Rep>& q) noexcept
  requires requires { sin(q.number()); } || requires { std::sin(q.number()); }
{
  using std::sin;
  return make_quantity<one>(static_cast<Rep>(sin(q.in(radian).number())));
}

template<ReferenceOf<angle> auto R, typename Rep>
  requires treat_as_floating_point<Rep>
[[nodiscard]] inline quantity<one, Rep> cos(const quantity<R, Rep>& q) noexcept
  requires requires { cos(q.number()); } || requires { std::cos(q.number()); }
{
  using std::cos;
  return make_quantity<one>(static_cast<Rep>(cos(q.in(radian).number())));
}

template<ReferenceOf<angle> auto R, typename Rep>
  requires treat_as_floating_point<Rep>
[[nodiscard]] inline quantity<one, Rep> tan(const quantity<R, Rep>& q) noexcept
  requires requires { tan(q.number()); } || requires { std::tan(q.number()); }
{
  using std::tan;
  return make_quantity<one>(static_cast<Rep>(tan(q.in(radian).number())));
}

template<ReferenceOf<dimensionless> auto R, typename Rep>
  requires treat_as_floating_point<Rep>
[[nodiscard]] inline quantity<radian, Rep> asin(const quantity<R, Rep>& q) noexcept
  requires requires { asin(q.number()); } || requires { std::asin(q.number()); }
{
  using std::asin;
  return make_quantity<radian>(static_cast<Rep>(asin(value_cast<one>(q).number())));
}

template<ReferenceOf<dimensionless> auto R, typename Rep>
  requires treat_as_floating_point<Rep>
[[nodiscard]] inline quantity<radian, Rep> acos(const quantity<R, Rep>& q) noexcept
  requires requires { acos(q.number()); } || requires { std::acos(q.number()); }
{
  using std::acos;
  return make_quantity<radian>(static_cast<Rep>(acos(value_cast<one>(q).number())));
}

template<ReferenceOf<dimensionless> auto R, typename Rep>
  requires treat_as_floating_point<Rep>
[[nodiscard]] inline quantity<radian, Rep> atan(const quantity<R, Rep>& q) noexcept
  requires requires { atan(q.number()); } || requires { std::atan(q.number()); }
{
  using std::atan;
  return make_quantity<radian>(static_cast<Rep>(atan(value_cast<one>(q).number())));
}

}  // namespace angular

}  // namespace mp_units
