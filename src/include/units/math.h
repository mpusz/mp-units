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
#include <units/quantity.h>
#include <cmath>
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
[[nodiscard]] inline auto pow(const Q& q) noexcept requires requires { std::pow(q.count(), 1.0); }
{
  using rep = TYPENAME Q::rep;
  if constexpr (Num == 0) {
    return rep(1);
  } else {
    using dim = dimension_pow<typename Q::dimension, Num, Den>;
    using unit = downcast_unit<dim, pow<Num, Den>(Q::unit::ratio)>;
    return quantity<dim, unit, rep>(
        static_cast<rep>(std::pow(q.count(), static_cast<double>(Num) / static_cast<double>(Den))));
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
  requires requires { std::sqrt(q.count()); }
{
  using dim = dimension_pow<typename Q::dimension, 1, 2>;
  using unit = downcast_unit<dim, sqrt(Q::unit::ratio)>;
  using rep = TYPENAME Q::rep;
  return quantity<dim, unit, rep>(static_cast<rep>(std::sqrt(q.count())));
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
  requires requires { std::cbrt(q.count()); }
{
  using dim = dimension_pow<typename Q::dimension, 1, 3>;
  using unit = downcast_unit<dim, cbrt(Q::unit::ratio)>;
  using rep = TYPENAME Q::rep;
  return quantity<dim, unit, rep>(static_cast<rep>(std::cbrt(q.count())));
}

/**
 * @brief Computes Euler's raised to the given power
 * 
 * @param q Quantity being the base of the operation
 * @return Quantity The value of the same quantity type
 */
template<typename D, typename U, typename Rep>
[[nodiscard]] inline quantity<D, U, Rep> exp(const quantity<D, U, Rep>& q)
{
  using coherent_unit = dimension_unit<D>;
  return quantity_cast<U>(quantity<D, coherent_unit, Rep>(std::exp(quantity_cast<coherent_unit>(q).count())));
}

/**
 * @brief Computes the absolute value of a quantity
 * 
 * @param q Quantity being the base of the operation
 * @return Quantity The absolute value of a provided quantity
 */
template<Quantity Q>
[[nodiscard]] inline Quantity auto abs(const Q& q) noexcept
  requires requires { std::abs(q.count()); }
{
  return Q(std::abs(q.count()));
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

}  // namespace units
