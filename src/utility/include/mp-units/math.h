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
             requires requires { pow(q.numerical_value_ref_in(q.unit), 1.0); } ||
                      requires { std::pow(q.numerical_value_ref_in(q.unit), 1.0); }
{
  if constexpr (Num == 0) {
    return quantity<pow<Num, Den>(R), Rep>::one();
  } else if constexpr (ratio{Num, Den} == 1) {
    return q;
  } else {
    using std::pow;
    return {
      static_cast<Rep>(pow(q.numerical_value_ref_in(q.unit), static_cast<double>(Num) / static_cast<double>(Den))),
      pow<Num, Den>(R)};
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
  requires requires { sqrt(q.numerical_value_ref_in(q.unit)); } ||
           requires { std::sqrt(q.numerical_value_ref_in(q.unit)); }
{
  using std::sqrt;
  return {static_cast<Rep>(sqrt(q.numerical_value_ref_in(q.unit))), sqrt(R)};
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
  requires requires { cbrt(q.numerical_value_ref_in(q.unit)); } ||
           requires { std::cbrt(q.numerical_value_ref_in(q.unit)); }
{
  using std::cbrt;
  return {static_cast<Rep>(cbrt(q.numerical_value_ref_in(q.unit))), cbrt(R)};
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
  requires requires { exp(q.numerical_value_ref_in(q.unit)); } ||
           requires { std::exp(q.numerical_value_ref_in(q.unit)); }
{
  using std::exp;
  return value_cast<get_unit(R)>(
    quantity{static_cast<Rep>(exp(q.force_numerical_value_in(q.unit))), detail::clone_reference_with<one>(R)});
}

/**
 * @brief Computes the absolute value of a quantity
 *
 * @param q Quantity being the base of the operation
 * @return Quantity The absolute value of a provided quantity
 */
template<auto R, typename Rep>
[[nodiscard]] constexpr quantity<R, Rep> abs(const quantity<R, Rep>& q) noexcept
  requires requires { abs(q.numerical_value_ref_in(q.unit)); } ||
           requires { std::abs(q.numerical_value_ref_in(q.unit)); }
{
  using std::abs;
  return {static_cast<Rep>(abs(q.numerical_value_ref_in(q.unit))), R};
}

/**
 * @brief Computes the fma of 3 quantities
 *
 * @param a: Multiplicand
 * @param x: Multiplicand
 * @param b: Addend
 * @return Quantity: The nearest floating point representable to ax+b
 */
template<auto R, auto S, typename Rep>
[[nodiscard]] constexpr quantity<R, Rep> fma(const quantity<R, Rep>& a, const quantity<S, Rep>& x,
                                             const quantity<R, Rep>& b) noexcept
  requires requires {
    fma(a.numerical_value_ref_in(a.unit), x.numerical_value_ref_in(x.unit), b.numerical_value_ref_in(b.unit));
  } || requires {
    std::fma(a.numerical_value_ref_in(a.unit), x.numerical_value_ref_in(x.unit), b.numerical_value_ref_in(b.unit));
  }
{
  using std::fma;
  return {static_cast<Rep>(
            fma(a.numerical_value_ref_in(a.unit), x.numerical_value_ref_in(x.unit), b.numerical_value_ref_in(b.unit))),
          R};
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
  return {static_cast<Rep>(std::numeric_limits<Rep>::epsilon()), r};
}

/**
 * @brief Computes the largest quantity with integer representation and unit type To with its number not greater than q
 *
 * @tparam q Quantity being the base of the operation
 * @return Quantity The rounded quantity with unit type To
 */
template<Unit auto To, auto R, typename Rep>
[[nodiscard]] constexpr quantity<detail::clone_reference_with<To>(R), Rep> floor(const quantity<R, Rep>& q) noexcept
  requires((!treat_as_floating_point<Rep>) || requires { floor(q.numerical_value_ref_in(q.unit)); } ||
           requires { std::floor(q.numerical_value_ref_in(q.unit)); }) &&
          (To == get_unit(R) || requires {
            q.force_in(To);
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
      return {static_cast<Rep>(floor(q.numerical_value_ref_in(q.unit))), detail::clone_reference_with<To>(R)};
    } else {
      return handle_signed_results(
        quantity{static_cast<Rep>(floor(q.force_numerical_value_in(To))), detail::clone_reference_with<To>(R)});
    }
  } else {
    if constexpr (To == get_unit(R)) {
      return q.force_in(To);
    } else {
      return handle_signed_results(q.force_in(To));
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
  requires((!treat_as_floating_point<Rep>) || requires { ceil(q.numerical_value_ref_in(q.unit)); } ||
           requires { std::ceil(q.numerical_value_ref_in(q.unit)); }) &&
          (To == get_unit(R) || requires {
            q.force_in(To);
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
      return {static_cast<Rep>(ceil(q.numerical_value_ref_in(q.unit))), detail::clone_reference_with<To>(R)};
    } else {
      return handle_signed_results(
        quantity{static_cast<Rep>(ceil(q.force_numerical_value_in(To))), detail::clone_reference_with<To>(R)});
    }
  } else {
    if constexpr (To == get_unit(R)) {
      return q.force_in(To);
    } else {
      return handle_signed_results(q.force_in(To));
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
  requires((!treat_as_floating_point<Rep>) || requires { round(q.numerical_value_ref_in(q.unit)); } ||
           requires { std::round(q.numerical_value_ref_in(q.unit)); }) &&
          (To == get_unit(R) || requires {
            ::mp_units::floor<To>(q);
            quantity_values<Rep>::one();
          })
{
  if constexpr (To == get_unit(R)) {
    if constexpr (treat_as_floating_point<Rep>) {
      using std::round;
      return {static_cast<Rep>(round(q.numerical_value_ref_in(q.unit))), detail::clone_reference_with<To>(R)};
    } else {
      return q.force_in(To);
    }
  } else {
    const auto res_low = mp_units::floor<To>(q);
    const auto res_high = res_low + res_low.one();
    const auto diff0 = q - res_low;
    const auto diff1 = res_high - q;
    if (diff0 == diff1) {
      if (static_cast<int>(res_low.numerical_value_ref_in(To)) & 1) {
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
 * @brief Computes the inverse of a quantity in a provided unit
 */
template<Unit auto To, auto R, typename Rep>
[[nodiscard]] constexpr QuantityOf<dimensionless / get_quantity_spec(R)> auto inverse(const quantity<R, Rep>& q)
  requires requires {
    quantity_values<Rep>::one();
    value_cast<To>(1 / q);
  }
{
  return (quantity_values<Rep>::one() * one).force_in(To * quantity<R, Rep>::unit) / q;
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
             requires { hypot(x.numerical_value_ref_in(x.unit), y.numerical_value_ref_in(y.unit)); } ||
             requires { std::hypot(x.numerical_value_ref_in(x.unit), y.numerical_value_ref_in(y.unit)); })
{
  constexpr auto ref = common_reference(R1, R2);
  constexpr auto unit = get_unit(ref);
  using std::hypot;
  return quantity{hypot(x.numerical_value_in(unit), y.numerical_value_in(unit)), ref};
}

/**
 * @brief Computes the square root of the sum of the squares of x, y, and z,
 *        without undue overflow or underflow at intermediate stages of the computation
 */
template<auto R1, typename Rep1, auto R2, typename Rep2, auto R3, typename Rep3>
[[nodiscard]] constexpr QuantityOf<get_quantity_spec(common_reference(R1, R2, R3))> auto hypot(
  const quantity<R1, Rep1>& x, const quantity<R2, Rep2>& y, const quantity<R3, Rep3>& z) noexcept
  requires requires { common_reference(R1, R2, R3); } &&
           (
             requires {
               hypot(x.numerical_value_ref_in(x.unit), y.numerical_value_ref_in(y.unit),
                     z.numerical_value_ref_in(z.unit));
             } ||
             requires {
               std::hypot(x.numerical_value_ref_in(x.unit), y.numerical_value_ref_in(y.unit),
                          z.numerical_value_ref_in(z.unit));
             })
{
  constexpr auto ref = common_reference(R1, R2);
  constexpr auto unit = get_unit(ref);
  using std::hypot;
  return quantity{hypot(x.numerical_value_in(unit), y.numerical_value_in(unit), z.numerical_value_in(unit)), ref};
}

namespace isq {

template<ReferenceOf<angular_measure> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<dimensionless> auto sin(const quantity<R, Rep>& q) noexcept
  requires requires { sin(q.numerical_value_ref_in(q.unit)); } ||
           requires { std::sin(q.numerical_value_ref_in(q.unit)); }
{
  using std::sin;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(sin(q.force_numerical_value_in(si::radian)));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return quantity{sin(value_cast<rep>(q).numerical_value_in(si::radian)), one};
  } else
    return quantity{sin(q.numerical_value_in(si::radian)), one};
}

template<ReferenceOf<angular_measure> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<dimensionless> auto cos(const quantity<R, Rep>& q) noexcept
  requires requires { cos(q.numerical_value_ref_in(q.unit)); } ||
           requires { std::cos(q.numerical_value_ref_in(q.unit)); }
{
  using std::cos;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(cos(q.force_numerical_value_in(si::radian)));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return quantity{cos(value_cast<rep>(q).numerical_value_in(si::radian)), one};
  } else
    return quantity{cos(q.numerical_value_in(si::radian)), one};
}

template<ReferenceOf<angular_measure> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<dimensionless> auto tan(const quantity<R, Rep>& q) noexcept
  requires requires { tan(q.numerical_value_ref_in(q.unit)); } ||
           requires { std::tan(q.numerical_value_ref_in(q.unit)); }
{
  using std::tan;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(tan(q.force_numerical_value_in(si::radian)));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return quantity{tan(value_cast<rep>(q).numerical_value_in(si::radian)), one};
  } else
    return quantity{tan(q.numerical_value_in(si::radian)), one};
}

template<ReferenceOf<dimensionless> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<isq::angular_measure> auto asin(const quantity<R, Rep>& q) noexcept
  requires requires { asin(q.numerical_value_ref_in(q.unit)); } ||
           requires { std::asin(q.numerical_value_ref_in(q.unit)); }
{
  using std::asin;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(asin(q.force_numerical_value_in(one)));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return quantity{asin(value_cast<rep>(q).numerical_value_in(one)), si::radian};
  } else
    return quantity{asin(q.numerical_value_in(one)), si::radian};
}

template<ReferenceOf<dimensionless> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<isq::angular_measure> auto acos(const quantity<R, Rep>& q) noexcept
  requires requires { acos(q.numerical_value_ref_in(q.unit)); } ||
           requires { std::acos(q.numerical_value_ref_in(q.unit)); }
{
  using std::acos;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(acos(q.force_numerical_value_in(one)));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return quantity{acos(value_cast<rep>(q).numerical_value_in(one)), si::radian};
  } else
    return quantity{acos(q.numerical_value_in(one)), si::radian};
}

template<ReferenceOf<dimensionless> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<isq::angular_measure> auto atan(const quantity<R, Rep>& q) noexcept
  requires requires { atan(q.numerical_value_ref_in(q.unit)); } ||
           requires { std::atan(q.numerical_value_ref_in(q.unit)); }
{
  using std::atan;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(atan(q.force_numerical_value_in(one)));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return quantity{atan(value_cast<rep>(q).numerical_value_in(one)), si::radian};
  } else
    return quantity{atan(q.numerical_value_in(one)), si::radian};
}

}  // namespace isq

namespace angular {

template<ReferenceOf<angle> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<dimensionless> auto sin(const quantity<R, Rep>& q) noexcept
  requires requires { sin(q.numerical_value_ref_in(q.unit)); } ||
           requires { std::sin(q.numerical_value_ref_in(q.unit)); }
{
  using std::sin;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(sin(q.force_numerical_value_in(radian)));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return quantity{sin(value_cast<rep>(q).numerical_value_in(radian)), one};
  } else
    return quantity{sin(q.numerical_value_in(radian)), one};
}

template<ReferenceOf<angle> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<dimensionless> auto cos(const quantity<R, Rep>& q) noexcept
  requires requires { cos(q.numerical_value_ref_in(q.unit)); } ||
           requires { std::cos(q.numerical_value_ref_in(q.unit)); }
{
  using std::cos;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(cos(q.force_numerical_value_in(radian)));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return quantity{cos(value_cast<rep>(q).numerical_value_in(radian)), one};
  } else
    return quantity{cos(q.numerical_value_in(radian)), one};
}

template<ReferenceOf<angle> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<dimensionless> auto tan(const quantity<R, Rep>& q) noexcept
  requires requires { tan(q.numerical_value_ref_in(q.unit)); } ||
           requires { std::tan(q.numerical_value_ref_in(q.unit)); }
{
  using std::tan;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(tan(q.force_numerical_value_in(radian)));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return quantity{tan(value_cast<rep>(q).numerical_value_in(radian)), one};
  } else
    return quantity{tan(q.numerical_value_in(radian)), one};
}

template<ReferenceOf<dimensionless> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<angle> auto asin(const quantity<R, Rep>& q) noexcept
  requires requires { asin(q.numerical_value_ref_in(q.unit)); } ||
           requires { std::asin(q.numerical_value_ref_in(q.unit)); }
{
  using std::asin;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(asin(q.force_numerical_value_in(one)));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return quantity{asin(value_cast<rep>(q).numerical_value_in(one)), radian};
  } else
    return quantity{asin(q.numerical_value_in(one)), radian};
}

template<ReferenceOf<dimensionless> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<angle> auto acos(const quantity<R, Rep>& q) noexcept
  requires requires { acos(q.numerical_value_ref_in(q.unit)); } ||
           requires { std::acos(q.numerical_value_ref_in(q.unit)); }
{
  using std::acos;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(acos(q.force_numerical_value_in(one)));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return quantity{acos(value_cast<rep>(q).numerical_value_in(one)), radian};
  } else
    return quantity{acos(q.numerical_value_in(one)), radian};
}

template<ReferenceOf<dimensionless> auto R, typename Rep>
[[nodiscard]] inline QuantityOf<angle> auto atan(const quantity<R, Rep>& q) noexcept
  requires requires { atan(q.numerical_value_ref_in(q.unit)); } ||
           requires { std::atan(q.numerical_value_ref_in(q.unit)); }
{
  using std::atan;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(atan(q.force_numerical_value_in(one)));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return quantity{atan(value_cast<rep>(q).numerical_value_in(one)), radian};
  } else
    return quantity{atan(q.numerical_value_in(one)), radian};
}

}  // namespace angular

}  // namespace mp_units
