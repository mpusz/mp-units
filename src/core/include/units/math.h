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
  requires ((!treat_as_floating_point<Rep>) ||
    requires { floor(q.number()); } ||
    requires { std::floor(q.number()); }) &&
    (std::same_as<To, U> || requires {
      ::units::quantity_cast<To>(q);
      quantity<D, To, Rep>::one();
    })
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

/**
 * @brief Overload of @c ::units::floor<Unit>() using the unit type of To
 *
 * @tparam q Quantity being the base of the operation
 * @return Quantity The rounded quantity with unit type of quantity To
 */
template<Quantity To, typename D, typename U, typename Rep>
[[nodiscard]] constexpr quantity<D, typename To::unit, Rep> floor(const quantity<D, U, Rep>& q) noexcept
  requires std::same_as<typename To::dimension, D> &&
    std::same_as<typename To::rep, Rep> &&
    requires {
      ::units::floor<typename To::unit>(q);
    }
{
  return ::units::floor<typename To::unit>(q);
}

/**
 * @brief Computes the smallest quantity with integer representation and unit type To with its number not less than q
 *
 * @tparam q Quantity being the base of the operation
 * @return Quantity The rounded quantity with unit type To
 */
template<Unit To, typename D, typename U, typename Rep>
[[nodiscard]] constexpr quantity<D, To, Rep> ceil(const quantity<D, U, Rep>& q) noexcept
  requires ((!treat_as_floating_point<Rep>) ||
    requires { ceil(q.number()); } ||
    requires { std::ceil(q.number()); }) &&
    (std::same_as<To, U> || requires {
      ::units::quantity_cast<To>(q);
      quantity<D, To, Rep>::one();
    })
{
  const auto handle_signed_results = [&]<typename T>(const T& res) {
    if (res < q)
      return res + T::one();
    return res;
  };
  if constexpr(treat_as_floating_point<Rep>) {
    using std::ceil;
    if constexpr(std::is_same_v<To, U>) {
      return quantity<D, To, Rep>(ceil(q.number()));
    }
    else {
      return handle_signed_results(quantity<D, To, Rep>(ceil(quantity_cast<To>(q).number())));
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

/**
 * @brief Overload of @c ::units::ceil<Unit>() using the unit type of To
 *
 * @tparam q Quantity being the base of the operation
 * @return Quantity The rounded quantity with unit type of quantity To
 */
template<Quantity To, typename D, typename U, typename Rep>
[[nodiscard]] constexpr quantity<D, typename To::unit, Rep> ceil(const quantity<D, U, Rep>& q) noexcept
  requires std::same_as<typename To::dimension, D> &&
    std::same_as<typename To::rep, Rep> &&
    requires {
      ::units::ceil<typename To::unit>(q);
    }
{
  return ::units::ceil<typename To::unit>(q);
}

/**
 * @brief Computes the nearest quantity with integer representation and unit type To to q
 *
 * Rounding halfway cases away from zero, regardless of the current rounding mode.
 *
 * @tparam q Quantity being the base of the operation
 * @return Quantity The rounded quantity with unit type To
 */
template<Unit To, typename D, typename U, typename Rep>
[[nodiscard]] constexpr quantity<D, To, Rep> round(const quantity<D, U, Rep>& q) noexcept
  requires (!treat_as_floating_point<Rep>) ||
    requires { round(q.number()); } ||
    requires { std::round(q.number()); }
    // TODO add require for units::floor
{
  if constexpr(std::is_same_v<To, U>) {
    if constexpr(treat_as_floating_point<Rep>) {
      using std::round;
      return quantity<D, To, Rep>(round(q.number()));
    }
    else {
      return q;
    }
  }
  else {
    const auto res_low = units::floor<To>(q);
    const auto res_high = res_low + decltype(res_low)::one();
    const auto diff0 = q - res_low;
    const auto diff1 = res_high - q;
    if (diff0 == diff1) {
      if (static_cast<int>(res_low.number()) & 1)
        return res_high;
      return res_low;
    } else if (diff0 < diff1) {
      return res_low;
    }
    return res_high;
  }
}

}  // namespace units
