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

// IWYU pragma: begin_exports
#include <mp-units/bits/external/fixed_string.h>
#include <mp-units/bits/external/hacks.h>
#include <mp-units/bits/module_macros.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <gsl/gsl-lite.hpp>
#include <compare>
#include <cstddef>
#include <cstdint>
#endif

#if __cpp_lib_text_encoding
#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <text_encoding>
#endif
static_assert(std::text_encoding::literal().mib() == std::text_encoding::id::UTF8);
#endif

namespace mp_units {

namespace detail {

constexpr bool is_basic_literal_character_set_char(char ch)
{
  // https://en.cppreference.com/w/cpp/language/charset
  return ch == 0x00 || (0x07 <= ch && ch <= 0x0D) || (0x20 <= ch && ch <= 0x7E);
};

template<std::size_t N>
constexpr bool is_basic_literal_character_set(const char (&txt)[N]) noexcept
{
  for (auto ch : txt)
    if (!is_basic_literal_character_set_char(ch)) return false;
  return true;
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
 * Each symbol can have two versions: Unicode and ASCI-only.
 *
 * @tparam N The size of a Unicode symbol
 * @tparam M The size of the ASCII-only symbol
 */
MP_UNITS_EXPORT template<std::size_t N, std::size_t M>
struct symbol_text {
  fixed_u8string<N> unicode_;
  fixed_string<M> ascii_;

  constexpr explicit(false) symbol_text(char ch) : unicode_(static_cast<char8_t>(ch)), ascii_(ch)
  {
    gsl_Expects(detail::is_basic_literal_character_set_char(ch));
  }

  constexpr explicit(false) symbol_text(const char (&txt)[N + 1]) :
      unicode_(detail::to_u8string(basic_fixed_string{txt})), ascii_(txt)
  {
    gsl_Expects(txt[N] == char{});
    gsl_Expects(detail::is_basic_literal_character_set(txt));
  }

  constexpr explicit(false) symbol_text(const fixed_string<N>& txt) : unicode_(detail::to_u8string(txt)), ascii_(txt)
  {
    gsl_Expects(detail::is_basic_literal_character_set(txt.data_));
  }

  constexpr symbol_text(const char8_t (&u)[N + 1], const char (&a)[M + 1]) : unicode_(u), ascii_(a)
  {
    gsl_Expects(u[N] == char8_t{});
    gsl_Expects(a[M] == char{});
    gsl_Expects(detail::is_basic_literal_character_set(a));
  }

  constexpr symbol_text(const fixed_u8string<N>& u, const fixed_string<M>& a) : unicode_(u), ascii_(a)
  {
    gsl_Expects(detail::is_basic_literal_character_set(a.data_));
  }

  [[nodiscard]] constexpr const auto& unicode() const { return unicode_; }
  [[nodiscard]] constexpr const auto& ascii() const { return ascii_; }

  [[nodiscard]] constexpr bool empty() const
  {
    gsl_AssertDebug(unicode().empty() == ascii().empty());
    return unicode().empty();
  }

  template<std::size_t N2, std::size_t M2>
  [[nodiscard]] constexpr friend symbol_text<N + N2, M + M2> operator+(const symbol_text& lhs,
                                                                       const symbol_text<N2, M2>& rhs)
  {
    return symbol_text<N + N2, M + M2>(lhs.unicode() + rhs.unicode(), lhs.ascii() + rhs.ascii());
  }

  template<std::size_t N2, std::size_t M2>
  [[nodiscard]] friend constexpr auto operator<=>(const symbol_text& lhs, const symbol_text<N2, M2>& rhs) noexcept
  {
    MP_UNITS_DIAGNOSTIC_PUSH
    MP_UNITS_DIAGNOSTIC_IGNORE_ZERO_AS_NULLPOINTER_CONSTANT
    if (const auto cmp = lhs.unicode() <=> rhs.unicode(); cmp != 0) return cmp;
    MP_UNITS_DIAGNOSTIC_POP
    return lhs.ascii() <=> rhs.ascii();
  }

  template<std::size_t N2, std::size_t M2>
  [[nodiscard]] friend constexpr bool operator==(const symbol_text& lhs, const symbol_text<N2, M2>& rhs) noexcept
  {
    return lhs.unicode() == rhs.unicode() && lhs.ascii() == rhs.ascii();
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
