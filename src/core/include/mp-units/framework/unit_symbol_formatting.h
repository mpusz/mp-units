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
#include <mp-units/framework/symbol_text.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cstdint>
#endif
#endif

MP_UNITS_EXPORT
namespace mp_units {

// NOLINTNEXTLINE(readability-enum-initial-value)
enum class unit_symbol_solidus : std::int8_t {
  one_denominator,  // m/s;   kg m⁻¹ s⁻¹
  always,           // m/s;   kg/(m s)
  never,            // m s⁻¹; kg m⁻¹ s⁻¹
  default_denominator = one_denominator
};

// NOLINTNEXTLINE(readability-enum-initial-value)
enum class unit_symbol_separator : std::int8_t {
  space,          // kg m²/s²
  half_high_dot,  // kg⋅m²/s²  (valid only for utf8 encoding)
  default_separator = space
};

struct unit_symbol_formatting {
  text_encoding encoding = text_encoding::default_encoding;
  unit_symbol_solidus solidus = unit_symbol_solidus::default_denominator;
  unit_symbol_separator separator = unit_symbol_separator::default_separator;
};

}  // namespace mp_units
