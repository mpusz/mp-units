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
#include <mp-units/bits/diagnostics.h>
#include <mp-units/bits/hacks.h>
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

/**
 * Selects when a unit symbol divides with a solidus and when it uses negative exponents
 *
 * The default writes a solidus only for a single unit in the denominator. A publication that asks
 * for negative exponents throughout selects `never`:
 *
 * @code{.cpp}
 * constexpr std::string_view text_unit = unit_symbol(m / s);  // "m/s"
 * constexpr std::string_view paper_unit =
 *   unit_symbol<{.solidus = unit_symbol_solidus::never}>(m / s);  // "m s⁻¹"
 * @endcode
 *
 * The `1`, `a`, and `n` tokens of a format specification select `one_denominator`, `always`,
 * and `never`.
 *
 * @see @ref character_set for the choice between Unicode and the basic character set
 * @see @ref unit_symbol_separator for the separator of the multiplied units
 * @see @ref unit_symbol_formatting for all the options `unit_symbol` takes
 * @see [Text Output](docs:users_guide/framework_basics/text_output) in the User's Guide
 */
// NOLINTNEXTLINE(readability-enum-initial-value)
enum class unit_symbol_solidus {
  /// A solidus for a single unit in the denominator, as in `m/s`, negative exponents otherwise, as in `kg m⁻¹ s⁻¹`
  one_denominator,
  /// Always a solidus, as in `m/s` and `kg/(m s)`
  always,
  /// Never a solidus, as in `m s⁻¹` and `kg m⁻¹ s⁻¹`
  never,
  /// The former default, `one_denominator`
  default_solidus [[deprecated("2.6.0: Name the value explicitly, e.g. `one_denominator`")]] = one_denominator
};

// The result of `dot_unit` is stated after the sample, not in it: MrDocs splits the `⋅` of a code block
// (cppalliance/mrdocs#1383). Move it back into a trailing comment once that is fixed.
/**
 * Selects what separates the multiplied units of a unit symbol
 *
 * The default is a space. A typeset document may select a half-high dot instead:
 *
 * @code{.cpp}
 * constexpr std::string_view text_unit = unit_symbol(kg * m2 / s2);  // "kg m²/s²"
 * constexpr std::string_view dot_unit =
 *   unit_symbol<{.separator = unit_symbol_separator::half_high_dot}>(kg * m2 / s2);
 * @endcode
 *
 * Here `dot_unit` is `kg⋅m²/s²`. The `s` and `d` tokens of a format specification select `space` and `half_high_dot`.
 *
 * @see @ref character_set for the choice between Unicode and the basic character set
 * @see @ref unit_symbol_solidus for when a solidus divides
 * @see @ref unit_symbol_formatting for all the options `unit_symbol` takes
 * @see [Text Output](docs:users_guide/framework_basics/text_output) in the User's Guide
 */
// NOLINTNEXTLINE(readability-enum-initial-value)
enum class unit_symbol_separator {
  /// A space, as in `kg m²/s²`
  space,
  /// A half-high dot, as in `kg⋅m²/s²`, valid only with `unicode`, and an error with `basic`
  half_high_dot,
  /// The former default, `space`
  default_separator [[deprecated("2.6.0: Name the value explicitly, e.g. `space`")]] = space
};

// The suppression covers the whole definition: Clang up to 21 reports the deprecated member from
// the implicitly defined copy constructor, while a user naming `encoding` is still warned
MP_UNITS_DIAGNOSTIC_PUSH
MP_UNITS_DIAGNOSTIC_IGNORE_DEPRECATED
struct unit_symbol_formatting {
  MP_UNITS_DEPRECATED_HACK([[deprecated("2.5.0: Use `char_set` instead")]])
  character_set encoding = character_set::unicode;
  character_set char_set = encoding;

  unit_symbol_solidus solidus = unit_symbol_solidus::one_denominator;
  unit_symbol_separator separator = unit_symbol_separator::space;
};
MP_UNITS_DIAGNOSTIC_POP

}  // namespace mp_units
