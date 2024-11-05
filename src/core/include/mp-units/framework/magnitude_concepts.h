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
concept MagConstant = detail::SymbolicConstant<T> && is_derived_from_specialization_of_v<T, mag_constant>;

template<auto... Ms>
struct magnitude;

/**
 * @brief  Concept to detect whether T is a valid Magnitude.
 */
MP_UNITS_EXPORT template<typename T>
concept Magnitude = is_specialization_of_v<T, magnitude>;

}  // namespace mp_units
