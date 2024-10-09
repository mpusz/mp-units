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

// NOLINTBEGIN(*-avoid-c-arrays)
#pragma once

// IWYU pragma: private, include <mp-units/framework.h>
#include <mp-units/bits/hacks.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/compat_macros.h>
#include <mp-units/ext/algorithm.h>
#include <mp-units/ext/fixed_string.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/ext/contracts.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <compare>  // IWYU pragma: export
#include <cstddef>
#include <cstdint>
#endif
#endif

#if __cpp_lib_text_encoding
#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <text_encoding>
#endif
#endif
static_assert(std::text_encoding::literal().mib() == std::text_encoding::id::UTF8);
#endif

namespace mp_units {

// NOLINTNEXTLINE(readability-enum-initial-value)
MP_UNITS_EXPORT enum class text_encoding : std::int8_t {
  utf8,  // µs; m³;  L²MT⁻³
  unicode [[deprecated("Use `utf8` instead")]] = utf8,
  portable,  // us; m^3; L^2MT^-3
  ascii [[deprecated("Use `portable` instead")]] = portable,
  default_encoding = utf8
};

namespace detail {

constexpr bool is_basic_literal_character_set_char(char ch)
{
  // https://en.cppreference.com/w/cpp/language/charset
  return ch == 0x00 || (0x07 <= ch && ch <= 0x0D) || (0x20 <= ch && ch <= 0x7E);
};

template<std::size_t N>
constexpr bool is_basic_literal_character_set(const char (&txt)[N]) noexcept
{
  return detail::all_of(std::begin(txt), std::end(txt), is_basic_literal_character_set_char);
}

template<std::size_t N>
constexpr fixed_u8string<N> to_u8string(fixed_string<N> txt)
{
  return std::bit_cast<fixed_u8string<N>>(txt);
}

}  // namespace detail


/**
 * @brief A symbol text representation
 *
 * This class template is responsible for definition and handling of a symbol text
 * representation. In the libary it is used to define symbols of units and prefixes.
 * Each symbol can have two versions: UTF-8 and portable.
 *
 * @tparam N The size of a UTF-8 symbol
 * @tparam M The size of the portable symbol
 */
MP_UNITS_EXPORT template<std::size_t N, std::size_t M>
class symbol_text {
public:
  fixed_u8string<N> utf8_;
  fixed_string<M> portable_;

  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr explicit(false) symbol_text(char ch) : utf8_(static_cast<char8_t>(ch)), portable_(ch)
  {
    MP_UNITS_EXPECTS(detail::is_basic_literal_character_set_char(ch));
  }

  // NOLINTNEXTLINE(*-avoid-c-arrays, google-explicit-constructor, hicpp-explicit-conversions)
  consteval explicit(false) symbol_text(const char (&txt)[N + 1]) :
      utf8_(detail::to_u8string(basic_fixed_string{txt})), portable_(txt)
  {
    MP_UNITS_EXPECTS(txt[N] == char{});
    MP_UNITS_EXPECTS(detail::is_basic_literal_character_set(txt));
  }

  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr explicit(false) symbol_text(const fixed_string<N>& txt) : utf8_(detail::to_u8string(txt)), portable_(txt)
  {
    MP_UNITS_EXPECTS(detail::is_basic_literal_character_set(txt.data_));
  }

  // NOLINTNEXTLINE(*-avoid-c-arrays)
  consteval symbol_text(const char8_t (&u)[N + 1], const char (&a)[M + 1]) : utf8_(u), portable_(a)
  {
    MP_UNITS_EXPECTS(u[N] == char8_t{});
    MP_UNITS_EXPECTS(a[M] == char{});
    MP_UNITS_EXPECTS(detail::is_basic_literal_character_set(a));
  }

  constexpr symbol_text(const fixed_u8string<N>& utf8, const fixed_string<M>& portable) :
      utf8_(utf8), portable_(portable)
  {
    MP_UNITS_EXPECTS(detail::is_basic_literal_character_set(portable.data_));
  }

  [[nodiscard]] constexpr const auto& utf8() const { return utf8_; }
  [[nodiscard]] constexpr const auto& portable() const { return portable_; }
  [[deprecated("Use `utf8()` instead")]] constexpr const auto& unicode() const { return utf8(); }
  [[deprecated("Use `portable()` instead")]] constexpr const auto& ascii() const { return portable(); }

  [[nodiscard]] constexpr bool empty() const
  {
    MP_UNITS_ASSERT_DEBUG(utf8().empty() == portable().empty());
    return utf8().empty();
  }

  template<std::size_t N2, std::size_t M2>
  [[nodiscard]] constexpr friend symbol_text<N + N2, M + M2> operator+(const symbol_text& lhs,
                                                                       const symbol_text<N2, M2>& rhs)
  {
    return symbol_text<N + N2, M + M2>(lhs.utf8() + rhs.utf8(), lhs.portable() + rhs.portable());
  }

  template<std::size_t N2, std::size_t M2>
  [[nodiscard]] friend constexpr auto operator<=>(const symbol_text& lhs, const symbol_text<N2, M2>& rhs) noexcept
  {
    MP_UNITS_DIAGNOSTIC_PUSH
    MP_UNITS_DIAGNOSTIC_IGNORE_ZERO_AS_NULLPOINTER_CONSTANT
    if (const auto cmp = lhs.utf8() <=> rhs.utf8(); cmp != 0) return cmp;
    MP_UNITS_DIAGNOSTIC_POP
    return lhs.portable() <=> rhs.portable();
  }

  template<std::size_t N2, std::size_t M2>
  [[nodiscard]] friend constexpr bool operator==(const symbol_text& lhs, const symbol_text<N2, M2>& rhs) noexcept
  {
    return lhs.utf8() == rhs.utf8() && lhs.portable() == rhs.portable();
  }
};

symbol_text(char) -> symbol_text<1, 1>;

template<std::size_t N>
symbol_text(const char (&)[N]) -> symbol_text<N - 1, N - 1>;

template<std::size_t N>
symbol_text(const fixed_string<N>&) -> symbol_text<N, N>;

template<std::size_t N, std::size_t M>
symbol_text(const char8_t (&)[N], const char (&)[M]) -> symbol_text<N - 1, M - 1>;

template<std::size_t N, std::size_t M>
symbol_text(const fixed_u8string<N>&, const fixed_string<M>&) -> symbol_text<N, M>;

}  // namespace mp_units
// NOLINTEND(*-avoid-c-arrays)
