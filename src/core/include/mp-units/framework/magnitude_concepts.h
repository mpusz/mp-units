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

// IWYU pragma: private, include <mp-units/framework.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/ext/type_traits.h>
#include <mp-units/framework/expression_template.h>
#include <mp-units/framework/symbol_text.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#endif
#endif

namespace mp_units {

#if defined MP_UNITS_COMP_CLANG || MP_UNITS_COMP_CLANG < 18
MP_UNITS_EXPORT template<symbol_text Symbol>
#else
MP_UNITS_EXPORT template<symbol_text Symbol, long double Value>
  requires(Value > 0)
#endif
struct mag_constant;

MP_UNITS_EXPORT template<typename T>
concept MagConstant = detail::TagType<T> && is_derived_from_specialization_of_v<T, mag_constant>;

namespace detail {

/**
 * @brief  Any type which can be used as a basis vector in a PowerV.
 *
 * We have two categories.
 *
 * The first is just an integral type (either `int` or `std::intmax_t`). This is for prime number bases.
 * These can always be used directly as NTTPs.
 *
 * The second category is a _custom tag type_, which inherits from `mag_constant` and has a static member variable
 * `value` of type `long double` that holds its value. We choose `long double` to get the greatest degree of precision;
 * users who need a different type can convert from this at compile time.  This category is for any irrational base
 * we admit into our representation (on which, more details below).
 */
template<typename T>
concept PowerVBase = one_of<T, int, std::intmax_t> || MagConstant<T>;

}  // namespace detail

template<detail::PowerVBase auto V, int Num, int... Den>
  requires(detail::valid_ratio<Num, Den...> && !detail::ratio_one<Num, Den...>)
struct power_v;

namespace detail {

template<typename T>
concept MagnitudeSpecExpr = PowerVBase<T> || is_specialization_of_v<T, power_v>;

}

template<detail::MagnitudeSpecExpr auto... Ms>
struct magnitude;

/**
 * @brief  Concept to detect whether T is a valid Magnitude.
 */
MP_UNITS_EXPORT template<typename T>
concept Magnitude = is_specialization_of_v<T, magnitude>;

}  // namespace mp_units
