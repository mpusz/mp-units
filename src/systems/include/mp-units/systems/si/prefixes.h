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

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework/unit.h>
#if MP_UNITS_HOSTED
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cmath>
#endif  // MP_UNITS_IMPORT_STD
#endif  // MP_UNITS_HOSTED
#endif  // MP_UNITS_IN_MODULE_INTERFACE

namespace mp_units::si {

// clang-format off
template<PrefixableUnit U> struct quecto_ final : prefixed_unit<"q", mag_power<10, -30>, U{}> {};
template<PrefixableUnit U> struct ronto_  final : prefixed_unit<"r", mag_power<10, -27>, U{}> {};
template<PrefixableUnit U> struct yocto_  final : prefixed_unit<"y", mag_power<10, -24>, U{}> {};
template<PrefixableUnit U> struct zepto_  final : prefixed_unit<"z", mag_power<10, -21>, U{}> {};
template<PrefixableUnit U> struct atto_   final : prefixed_unit<"a", mag_power<10, -18>, U{}> {};
template<PrefixableUnit U> struct femto_  final : prefixed_unit<"f", mag_power<10, -15>, U{}> {};
template<PrefixableUnit U> struct pico_   final : prefixed_unit<"p", mag_power<10, -12>, U{}> {};
template<PrefixableUnit U> struct nano_   final : prefixed_unit<"n", mag_power<10, -9>, U{}> {};
template<PrefixableUnit U> struct micro_  final : prefixed_unit<symbol_text{u8"µ", "u"}, mag_power<10, -6>, U{}> {};
template<PrefixableUnit U> struct milli_  final : prefixed_unit<"m", mag_power<10, -3>, U{}> {};
template<PrefixableUnit U> struct centi_  final : prefixed_unit<"c", mag_power<10, -2>, U{}> {};
template<PrefixableUnit U> struct deci_   final : prefixed_unit<"d", mag_power<10, -1>, U{}> {};
template<PrefixableUnit U> struct deca_   final : prefixed_unit<"da", mag_power<10, 1>, U{}> {};
template<PrefixableUnit U> struct hecto_  final : prefixed_unit<"h", mag_power<10, 2>, U{}> {};
template<PrefixableUnit U> struct kilo_   final : prefixed_unit<"k", mag_power<10, 3>, U{}> {};
template<PrefixableUnit U> struct mega_   final : prefixed_unit<"M", mag_power<10, 6>, U{}> {};
template<PrefixableUnit U> struct giga_   final : prefixed_unit<"G", mag_power<10, 9>, U{}> {};
template<PrefixableUnit U> struct tera_   final : prefixed_unit<"T", mag_power<10, 12>, U{}> {};
template<PrefixableUnit U> struct peta_   final : prefixed_unit<"P", mag_power<10, 15>, U{}> {};
template<PrefixableUnit U> struct exa_    final : prefixed_unit<"E", mag_power<10, 18>, U{}> {};
template<PrefixableUnit U> struct zetta_  final : prefixed_unit<"Z", mag_power<10, 21>, U{}> {};
template<PrefixableUnit U> struct yotta_  final : prefixed_unit<"Y", mag_power<10, 24>, U{}> {};
template<PrefixableUnit U> struct ronna_  final : prefixed_unit<"R", mag_power<10, 27>, U{}> {};
template<PrefixableUnit U> struct quetta_ final : prefixed_unit<"Q", mag_power<10, 30>, U{}> {};

MP_UNITS_EXPORT_BEGIN

template<PrefixableUnit auto U> constexpr quecto_<MP_UNITS_REMOVE_CONST(decltype(U))> quecto;
template<PrefixableUnit auto U> constexpr ronto_<MP_UNITS_REMOVE_CONST(decltype(U))> ronto;
template<PrefixableUnit auto U> constexpr yocto_<MP_UNITS_REMOVE_CONST(decltype(U))> yocto;
template<PrefixableUnit auto U> constexpr zepto_<MP_UNITS_REMOVE_CONST(decltype(U))> zepto;
template<PrefixableUnit auto U> constexpr atto_<MP_UNITS_REMOVE_CONST(decltype(U))> atto;
template<PrefixableUnit auto U> constexpr femto_<MP_UNITS_REMOVE_CONST(decltype(U))> femto;
template<PrefixableUnit auto U> constexpr pico_<MP_UNITS_REMOVE_CONST(decltype(U))> pico;
template<PrefixableUnit auto U> constexpr nano_<MP_UNITS_REMOVE_CONST(decltype(U))> nano;
template<PrefixableUnit auto U> constexpr micro_<MP_UNITS_REMOVE_CONST(decltype(U))> micro;
template<PrefixableUnit auto U> constexpr milli_<MP_UNITS_REMOVE_CONST(decltype(U))> milli;
template<PrefixableUnit auto U> constexpr centi_<MP_UNITS_REMOVE_CONST(decltype(U))> centi;
template<PrefixableUnit auto U> constexpr deci_<MP_UNITS_REMOVE_CONST(decltype(U))> deci;
template<PrefixableUnit auto U> constexpr deca_<MP_UNITS_REMOVE_CONST(decltype(U))> deca;
template<PrefixableUnit auto U> constexpr hecto_<MP_UNITS_REMOVE_CONST(decltype(U))> hecto;
template<PrefixableUnit auto U> constexpr kilo_<MP_UNITS_REMOVE_CONST(decltype(U))> kilo;
template<PrefixableUnit auto U> constexpr mega_<MP_UNITS_REMOVE_CONST(decltype(U))> mega;
template<PrefixableUnit auto U> constexpr giga_<MP_UNITS_REMOVE_CONST(decltype(U))> giga;
template<PrefixableUnit auto U> constexpr tera_<MP_UNITS_REMOVE_CONST(decltype(U))> tera;
template<PrefixableUnit auto U> constexpr peta_<MP_UNITS_REMOVE_CONST(decltype(U))> peta;
template<PrefixableUnit auto U> constexpr exa_<MP_UNITS_REMOVE_CONST(decltype(U))> exa;
template<PrefixableUnit auto U> constexpr zetta_<MP_UNITS_REMOVE_CONST(decltype(U))> zetta;
template<PrefixableUnit auto U> constexpr yotta_<MP_UNITS_REMOVE_CONST(decltype(U))> yotta;
template<PrefixableUnit auto U> constexpr ronna_<MP_UNITS_REMOVE_CONST(decltype(U))> ronna;
template<PrefixableUnit auto U> constexpr quetta_<MP_UNITS_REMOVE_CONST(decltype(U))> quetta;
// clang-format on

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
 *                resulting in values in range [1.0, 10.0)
 * @param min_integral_digits Minimum number of digits in the integral part (default: 1)
 *
 * @return The result of calling @c func with the scaled quantity
 *
 * @note For @c min_integral_digits=1:
 *       - engineering mode: values displayed in range [1.0, 999.999...]
 *       - full mode: values displayed in range [1.0, 9.999...]
 */
template<Quantity Q, std::invocable<Q> Func, PrefixableUnit U, auto Character = quantity_character::real_scalar>
  requires RepresentationOf<typename Q::rep, Character> && treat_as_floating_point<typename Q::rep> &&
           requires(Q::rep v) {
             requires requires { abs(v); } || requires { std::abs(v); };
             requires requires { log10(v); } || requires { std::log10(v); };
             requires requires { floor(v); } || requires { std::floor(v); };
           }
decltype(auto) invoke_with_prefixed(Func func, Q q, U u, prefix_range range = prefix_range::engineering,
                                    int min_integral_digits = 1)
{
  if (is_eq_zero(q)) return func(q.in(u));

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
