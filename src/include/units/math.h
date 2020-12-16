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
 * @tparam N Exponent
 * @param q Quantity being the base of the operation
 * @return Quantity The result of computation 
 */
template<std::intmax_t N, Quantity Q>
[[nodiscard]] inline auto pow(const Q& q) noexcept
  requires requires { std::pow(q.count(), N); }
{
  using rep = TYPENAME Q::rep;
  if constexpr(N == 0) {
    return rep(1);
  }
  else {
    using dim = dimension_pow<typename Q::dimension, N>;
    using unit = downcast_unit<dim, pow<N>(Q::unit::ratio)>;
    return quantity<dim, unit, rep>(static_cast<rep>(std::pow(q.count(), N)));
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
  using dim = dimension_sqrt<typename Q::dimension>;
  using unit = downcast_unit<dim, sqrt(Q::unit::ratio)>;
  using rep = TYPENAME Q::rep;
  return quantity<dim, unit, rep>(static_cast<rep>(std::sqrt(q.count())));
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
{
  return quantity_cast<U>(dimensionless<one, Rep>(std::exp(quantity_cast<one>(q).count())));
}

/**
 * @brief Computes the absolute value of a quantity
 * 
 * @param q Quantity being the base of the operation
 * @return Quantity The absolute value of a provided quantity
 */
template<typename D, typename U, typename Rep>
[[nodiscard]] inline quantity<D, U, Rep> abs(const quantity<D, U, Rep>& q) noexcept
  requires requires { std::abs(q.count()); }
{
  return quantity<D, U, Rep>(std::abs(q.count()));
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
