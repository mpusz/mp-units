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

#include <mp-units/bits/module_macros.h>
#include <mp-units/systems/si/prefixes.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#if MP_UNITS_HOSTED
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cmath>
#endif  // MP_UNITS_IMPORT_STD
#endif  // MP_UNITS_HOSTED
#endif  // MP_UNITS_IN_MODULE_INTERFACE

namespace mp_units::si {

MP_UNITS_EXPORT_BEGIN

#if MP_UNITS_HOSTED

enum class prefix_range : std::uint8_t { engineering, full };

/**
 * @brief Calls an invocable with a quantity scaled to an automatically selected SI prefix
 *
 * Selects an SI prefix such that the integral part of the quantity's numerical value
 * has at least @c min_integral_digits digits.
 *
 * @tparam Q Quantity type
 * @tparam Func Invocable type that accepts a quantity
 * @tparam U PrefixableUnit type
 *
 * @param func Invocable to call with the scaled quantity
 * @param q Quantity to scale
 * @param u Unit to use as the base for prefix selection
 * @param range Prefix selection mode:
 *              - @c prefix_range::engineering selects only powers of 1000 (kilo, mega, milli, etc.),
 *                resulting in values in range [1.0, 1000)
 *              - @c prefix_range::full selects all SI prefixes including deca, hecto, deci, centi,
 *                resulting in values values in [1.0, 10) within the milli–kilo range, [1.0, 1000) elsewhere
 * @param min_integral_digits Minimum number of digits in the integral part (default: 1)
 *
 * @return The result of calling @c func with the scaled quantity
 *
 * @note For @c min_integral_digits=1:
 *       - engineering mode: values displayed in range [1.0, 999.999...]
 *       - full mode: values displayed in range [1.0, 9.999...]
 */
template<Quantity Q, std::invocable<Q> Func, PrefixableUnit U, auto Character = quantity_character{}>
  requires RepresentationOf<typename Q::rep, Character> && treat_as_floating_point<typename Q::rep> &&
           requires(Q::rep v) {
             requires requires { abs(v); } || requires { std::abs(v); };
             requires requires { log10(v); } || requires { std::log10(v); };
             requires requires { floor(v); } || requires { std::floor(v); };
           }
constexpr decltype(auto) invoke_with_prefixed(Func func, Q q, U u, prefix_range range = prefix_range::engineering,
                                              int min_integral_digits = 1)
{
  if (q == 0) return func(q.in(u));

  using std::abs, std::log10, std::floor;

  // Calculate the order of magnitude to determine appropriate prefix
  const auto value = q.numerical_value_in(u);
  const auto mag = static_cast<int>(floor(log10(abs(value))));

  // Exponent ensures value has at least min_integral_digits in integral part
  // For min_integral_digits=1: select prefix giving value in [1, base) where base is 1000 or 10
  const int exponent = mag - (min_integral_digits - 1);

  // Try each SI prefix from largest to smallest
  // clang-format off
  if (exponent >= 30)                                     return func(q.in(si::quetta<U{}>));
  else if (exponent >= 27)                                return func(q.in(si::ronna<U{}>));
  else if (exponent >= 24)                                return func(q.in(si::yotta<U{}>));
  else if (exponent >= 21)                                return func(q.in(si::zetta<U{}>));
  else if (exponent >= 18)                                return func(q.in(si::exa<U{}>));
  else if (exponent >= 15)                                return func(q.in(si::peta<U{}>));
  else if (exponent >= 12)                                return func(q.in(si::tera<U{}>));
  else if (exponent >= 9)                                 return func(q.in(si::giga<U{}>));
  else if (exponent >= 6)                                 return func(q.in(si::mega<U{}>));
  else if (exponent >= 3)                                 return func(q.in(si::kilo<U{}>));
  else if (exponent >= 2 && range == prefix_range::full)  return func(q.in(si::hecto<U{}>));
  else if (exponent >= 1 && range == prefix_range::full)  return func(q.in(si::deca<U{}>));
  else if (exponent >= 0)                                 return func(q.in(U{}));
  else if (exponent >= -1 && range == prefix_range::full) return func(q.in(si::deci<U{}>));
  else if (exponent >= -2 && range == prefix_range::full) return func(q.in(si::centi<U{}>));
  else if (exponent >= -3)                                return func(q.in(si::milli<U{}>));
  else if (exponent >= -6)                                return func(q.in(si::micro<U{}>));
  else if (exponent >= -9)                                return func(q.in(si::nano<U{}>));
  else if (exponent >= -12)                               return func(q.in(si::pico<U{}>));
  else if (exponent >= -15)                               return func(q.in(si::femto<U{}>));
  else if (exponent >= -18)                               return func(q.in(si::atto<U{}>));
  else if (exponent >= -21)                               return func(q.in(si::zepto<U{}>));
  else if (exponent >= -24)                               return func(q.in(si::yocto<U{}>));
  else if (exponent >= -27)                               return func(q.in(si::ronto<U{}>));
  else                                                    return func(q.in(si::quecto<U{}>));
  // clang-format on
}

#endif  // MP_UNITS_HOSTED

MP_UNITS_EXPORT_END

}  // namespace mp_units::si
