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
             requires requires { pow(q.value(), 1.0); } || requires { std::pow(q.value(), 1.0); }
{
  if constexpr (Num == 0) {
    return quantity<pow<Num, Den>(R), Rep>::one();
  } else if constexpr (ratio{Num, Den} == 1) {
    return q;
  } else {
    using std::pow;
    return make_quantity<pow<Num, Den>(R)>(
      static_cast<Rep>(pow(q.value(), static_cast<double>(Num) / static_cast<double>(Den))));
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
  requires requires { sqrt(q.value()); } || requires { std::sqrt(q.value()); }
{
  using std::sqrt;
  return make_quantity<sqrt(R)>(static_cast<Rep>(sqrt(q.value())));
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
  requires requires { cbrt(q.value()); } || requires { std::cbrt(q.value()); }
{
  using std::cbrt;
  return make_quantity<cbrt(R)>(static_cast<Rep>(cbrt(q.value())));
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
  requires requires { exp(q.value()); } || requires { std::exp(q.value()); }
{
  using std::exp;
  return value_cast<get_unit(R)>(
    make_quantity<detail::clone_reference_with<one>(R)>(static_cast<Rep>(exp(value_cast<one>(q).value()))));
}

/**
 * @brief Computes the absolute value of a quantity
 *
 * @param q Quantity being the base of the operation
 * @return Quantity The absolute value of a provided quantity
 */
template<auto R, typename Rep>
[[nodiscard]] constexpr quantity<R, Rep> abs(const quantity<R, Rep>& q) noexcept
  requires requires { abs(q.value()); } || requires { std::abs(q.value()); }
{
  using std::abs;
  return make_quantity<R>(static_cast<Rep>(abs(q.value())));
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
  requires((!treat_as_floating_point<Rep>) || requires { floor(q.value()); } || requires { std::floor(q.value()); }) &&
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
      return make_quantity<detail::clone_reference_with<To>(R)>(static_cast<Rep>(floor(q.value())));
    } else {
      return handle_signed_results(
        make_quantity<detail::clone_reference_with<To>(R)>(static_cast<Rep>(floor(value_cast<To>(q).value()))));
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
  requires((!treat_as_floating_point<Rep>) || requires { ceil(q.value()); } || requires { std::ceil(q.value()); }) &&
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
      return make_quantity<detail::clone_reference_with<To>(R)>(static_cast<Rep>(ceil(q.value())));
    } else {
      return handle_signed_results(
        make_quantity<detail::clone_reference_with<To>(R)>(static_cast<Rep>(ceil(value_cast<To>(q).value()))));
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
  requires((!treat_as_floating_point<Rep>) || requires { round(q.value()); } || requires { std::round(q.value()); }) &&
          (To == get_unit(R) || requires {
            ::mp_units::floor<To>(q);
            quantity_values<Rep>::one();
          })
{
  if constexpr (To == get_unit(R)) {
    if constexpr (treat_as_floating_point<Rep>) {
      using std::round;
      return make_quantity<detail::clone_reference_with<To>(R)>(static_cast<Rep>(round(q.value())));
    } else {
      return value_cast<To>(q);
    }
  } else {
    const auto res_low = mp_units::floor<To>(q);
    const auto res_high = res_low + res_low.one();
    const auto diff0 = q - res_low;
    const auto diff1 = res_high - q;
    if (diff0 == diff1) {
      if (static_cast<int>(res_low.value()) & 1) {
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
             requires { hypot(x.value(), y.value()); } || requires { std::hypot(x.value(), y.value()); })
{
  constexpr auto ref = common_reference(R1, R2);
  constexpr auto unit = get_unit(ref);
  using std::hypot;
  return make_quantity<ref>(hypot(x.value_in(unit), y.value_in(unit)));
}

/**
 * @brief Computes the square root of the sum of the squares of x, y, and z,
 *        without undue overflow or underflow at intermediate stages of the computation
 */
template<auto R1, typename Rep1, auto R2, typename Rep2, auto R3, typename Rep3>
[[nodiscard]] constexpr QuantityOf<get_quantity_spec(common_reference(R1, R2, R3))> auto hypot(
  const quantity<R1, Rep1>& x, const quantity<R2, Rep2>& y, const quantity<R3, Rep3>& z) noexcept
  requires requires { common_reference(R1, R2, R3); } && (
                                                           requires { hypot(x.value(), y.value(), z.value()); } ||
                                                           requires { std::hypot(x.value(), y.value(), z.value()); })
{
  constexpr auto ref = common_reference(R1, R2);
  constexpr auto unit = get_unit(ref);
  using std::hypot;
  return make_quantity<ref>(hypot(x.value_in(unit), y.value_in(unit), z.value_in(unit)));
}

namespace isq {

template<ReferenceOf<angular_measure> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<dimensionless> auto sin(const quantity<R, Rep>& q) noexcept
  requires requires { sin(q.value()); } || requires { std::sin(q.value()); }
{
  using std::sin;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(sin(value_cast<si::radian>(q).value()));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return make_quantity<one>(sin(value_cast<rep>(q).value_in(si::radian)));
  } else
    return make_quantity<one>(sin(q.value_in(si::radian)));
}

template<ReferenceOf<angular_measure> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<dimensionless> auto cos(const quantity<R, Rep>& q) noexcept
  requires requires { cos(q.value()); } || requires { std::cos(q.value()); }
{
  using std::cos;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(cos(value_cast<si::radian>(q).value()));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return make_quantity<one>(cos(value_cast<rep>(q).value_in(si::radian)));
  } else
    return make_quantity<one>(cos(q.value_in(si::radian)));
}

template<ReferenceOf<angular_measure> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<dimensionless> auto tan(const quantity<R, Rep>& q) noexcept
  requires requires { tan(q.value()); } || requires { std::tan(q.value()); }
{
  using std::tan;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(tan(value_cast<si::radian>(q).value()));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return make_quantity<one>(tan(value_cast<rep>(q).value_in(si::radian)));
  } else
    return make_quantity<one>(tan(q.value_in(si::radian)));
}

template<ReferenceOf<dimensionless> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<isq::angular_measure> auto asin(const quantity<R, Rep>& q) noexcept
  requires requires { asin(q.value()); } || requires { std::asin(q.value()); }
{
  using std::asin;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(asin(value_cast<one>(q).value()));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return make_quantity<si::radian>(asin(value_cast<rep>(q).value_in(one)));
  } else
    return make_quantity<si::radian>(asin(q.value_in(one)));
}

template<ReferenceOf<dimensionless> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<isq::angular_measure> auto acos(const quantity<R, Rep>& q) noexcept
  requires requires { acos(q.value()); } || requires { std::acos(q.value()); }
{
  using std::acos;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(acos(value_cast<one>(q).value()));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return make_quantity<si::radian>(acos(value_cast<rep>(q).value_in(one)));
  } else
    return make_quantity<si::radian>(acos(q.value_in(one)));
}

template<ReferenceOf<dimensionless> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<isq::angular_measure> auto atan(const quantity<R, Rep>& q) noexcept
  requires requires { atan(q.value()); } || requires { std::atan(q.value()); }
{
  using std::atan;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(atan(value_cast<one>(q).value()));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return make_quantity<si::radian>(atan(value_cast<rep>(q).value_in(one)));
  } else
    return make_quantity<si::radian>(atan(q.value_in(one)));
}

}  // namespace isq

namespace angular {

template<ReferenceOf<angle> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<dimensionless> auto sin(const quantity<R, Rep>& q) noexcept
  requires requires { sin(q.value()); } || requires { std::sin(q.value()); }
{
  using std::sin;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(sin(value_cast<radian>(q).value()));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return make_quantity<one>(sin(value_cast<rep>(q).value_in(radian)));
  } else
    return make_quantity<one>(sin(q.value_in(radian)));
}

template<ReferenceOf<angle> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<dimensionless> auto cos(const quantity<R, Rep>& q) noexcept
  requires requires { cos(q.value()); } || requires { std::cos(q.value()); }
{
  using std::cos;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(cos(value_cast<radian>(q).value()));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return make_quantity<one>(cos(value_cast<rep>(q).value_in(radian)));
  } else
    return make_quantity<one>(cos(q.value_in(radian)));
}

template<ReferenceOf<angle> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<dimensionless> auto tan(const quantity<R, Rep>& q) noexcept
  requires requires { tan(q.value()); } || requires { std::tan(q.value()); }
{
  using std::tan;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(tan(value_cast<radian>(q).value()));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return make_quantity<one>(tan(value_cast<rep>(q).value_in(radian)));
  } else
    return make_quantity<one>(tan(q.value_in(radian)));
}

template<ReferenceOf<dimensionless> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<angle> auto asin(const quantity<R, Rep>& q) noexcept
  requires requires { asin(q.value()); } || requires { std::asin(q.value()); }
{
  using std::asin;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(asin(value_cast<one>(q).value()));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return make_quantity<radian>(asin(value_cast<rep>(q).value_in(one)));
  } else
    return make_quantity<radian>(asin(q.value_in(one)));
}

template<ReferenceOf<dimensionless> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<angle> auto acos(const quantity<R, Rep>& q) noexcept
  requires requires { acos(q.value()); } || requires { std::acos(q.value()); }
{
  using std::acos;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(acos(value_cast<one>(q).value()));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return make_quantity<radian>(acos(value_cast<rep>(q).value_in(one)));
  } else
    return make_quantity<radian>(acos(q.value_in(one)));
}

template<ReferenceOf<dimensionless> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<angle> auto atan(const quantity<R, Rep>& q) noexcept
  requires requires { atan(q.value()); } || requires { std::atan(q.value()); }
{
  using std::atan;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(atan(value_cast<one>(q).value()));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return make_quantity<radian>(atan(value_cast<rep>(q).value_in(one)));
  } else
    return make_quantity<radian>(atan(q.value_in(one)));
}

}  // namespace angular

}  // namespace mp_units
