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
#include <units/bits/external/hacks.h>
#include <units/generic/dimensionless.h>
#include <units/quantity.h>
#include <units/unit.h>

// IWYU pragma: begin_exports
#include <cmath>
#include <cstdint>
// IWYU pragma: end_exports

#include <limits>

namespace units {

/**
 * @brief Computes the value of a quantity raised to the power `N`
 *
 * Both the quantity value and its dimension are the base of the operation.
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
  } else {
    using dim = dimension_pow<typename Q::dimension, Num, Den>;
    using unit = downcast_unit<dim, pow<Num, Den>(Q::unit::ratio)>;
    using std::pow;
    return quantity<dim, unit, rep>(
        static_cast<rep>(pow(q.number(), static_cast<double>(Num) / static_cast<double>(Den))));
  }
}

/**
 * @brief Computes the square root of a quantity
 *
 * Both the quantity value and its dimension are the base of the operation.
 *
 * @param q Quantity being the base of the operation
 * @return Quantity The result of computation
 */
template<Quantity Q>
[[nodiscard]] inline Quantity auto sqrt(const Q& q) noexcept
  requires requires { sqrt(q.number()); }  ||  requires { std::sqrt(q.number()); }
{
  using dim = dimension_pow<typename Q::dimension, 1, 2>;
  using unit = downcast_unit<dim, sqrt(Q::unit::ratio)>;
  using rep = TYPENAME Q::rep;
  using std::sqrt;
  return quantity<dim, unit, rep>(static_cast<rep>(sqrt(q.number())));
}

/**
 * @brief Computes the cubic root of a quantity
 *
 * Both the quantity value and its dimension are the base of the operation.
 *
 * @param q Quantity being the base of the operation
 * @return Quantity The result of computation
 */
template<Quantity Q>
[[nodiscard]] inline Quantity auto cbrt(const Q& q) noexcept
  requires requires { cbrt(q.number()); } || requires { std::cbrt(q.number()); }
{
  using dim = dimension_pow<typename Q::dimension, 1, 3>;
  using unit = downcast_unit<dim, cbrt(Q::unit::ratio)>;
  using rep = TYPENAME Q::rep;
  using std::cbrt;
  return quantity<dim, unit, rep>(static_cast<rep>(cbrt(q.number())));
}

/**
 * @brief Computes Euler's raised to the given power
 *
 * @note Such an operation has sense only for a dimensionless quantity.
 *
 * @param q Quantity being the base of the operation
 * @return Quantity The value of the same quantity type
 */
template<typename U, typename Rep>
[[nodiscard]] inline dimensionless<U, Rep> exp(const dimensionless<U, Rep>& q)
  requires requires { exp(q.number()); } || requires { std::exp(q.number()); }
{
  using std::exp;
  return quantity_cast<U>(dimensionless<one, Rep>(exp(quantity_cast<one>(q).number())));
}

/**
 * @brief Computes the absolute value of a quantity
 *
 * @param q Quantity being the base of the operation
 * @return Quantity The absolute value of a provided quantity
 */
template<typename D, typename U, typename Rep>
[[nodiscard]] inline quantity<D, U, Rep> abs(const quantity<D, U, Rep>& q) noexcept
  requires requires { abs(q.number()); } ||  requires { std::abs(q.number()); }
{
  using std::abs;
  return quantity<D, U, Rep>(abs(q.number()));
}

/**
 * @brief Returns the epsilon of the quantity
 *
 * The returned value is defined by a <tt>std::numeric_limits<typename Q::rep>::epsilon()</tt>.
 *
 * @tparam Q Quantity type being the base of the operation
 * @return Quantity The epsilon value for quantity's representation type
 */
template<Quantity Q>
  requires requires { std::numeric_limits<typename Q::rep>::epsilon(); }
[[nodiscard]] constexpr Quantity auto epsilon() noexcept
{
  return Q(std::numeric_limits<typename Q::rep>::epsilon());
}

/**
 * @brief Computes the largest quantity with integer representation and unit type To with its number not greater than q
 *
 * @tparam q Quantity being the base of the operation
 * @return Quantity The rounded quantity with unit type To
 */
template<Unit To, typename D, typename U, typename Rep>
[[nodiscard]] constexpr quantity<D, To, Rep> floor(const quantity<D, U, Rep>& q) noexcept
  requires (!treat_as_floating_point<Rep>) ||
    requires { floor(q.number()); } ||
    requires { std::floor(q.number()); }
{
  const auto handle_signed_results = [&]<typename T>(const T& res) {
    if (res > q)
      return res - T::one();
    return res;
  };
  if constexpr(treat_as_floating_point<Rep>) {
    using std::floor;
    if constexpr(std::is_same_v<To, U>) {
      return quantity<D, To, Rep>(floor(q.number()));
    }
    else {
      return handle_signed_results(quantity<D, To, Rep>(floor(quantity_cast<To>(q).number())));
    }
  }
  else {
    if constexpr(std::is_same_v<To, U>) {
      return q;
    }
    else {
      return handle_signed_results(quantity_cast<To>(q));
    }
  }
}

}  // namespace units
