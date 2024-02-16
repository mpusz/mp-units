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
#include <compare>
#include <cstddef>
#include <cstdint>
// IWYU pragma: end_exports

#include <gsl/gsl-lite.hpp>

#if __cpp_lib_text_encoding
#include <text_encoding>
static_assert(std::text_encoding::literal().mib() == std::text_encoding::id::UTF8);
#endif

namespace mp_units {

namespace detail {

constexpr void validate_ascii_char([[maybe_unused]] char c) noexcept
{
  // check if character belongs to basic character literal set
  // https://en.cppreference.com/w/cpp/language/charset
  gsl_Expects(c == 0x00 || (0x07 <= c && c <= 0x0D) || (0x20 <= c && c <= 0x7E));
}

template<std::size_t N>
constexpr void validate_ascii_string([[maybe_unused]] const char (&s)[N + 1]) noexcept
{
#ifndef NDEBUG
  if constexpr (N != 0)
    for (size_t i = 0; i < N; ++i) validate_ascii_char(s[i]);
#endif
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
template<std::size_t N, std::size_t M>
struct basic_symbol_text {
  fixed_u8string<N> unicode_;
  fixed_string<M> ascii_;

  constexpr explicit(false) basic_symbol_text(char txt) : unicode_(static_cast<char8_t>(txt)), ascii_(txt)
  {
    detail::validate_ascii_char(txt);
  }

  constexpr explicit(false) basic_symbol_text(const char (&txt)[N + 1]) :
      unicode_(detail::to_u8string(basic_fixed_string{txt})), ascii_(txt)
  {
    detail::validate_ascii_string<N>(txt);
  }

  constexpr explicit(false) basic_symbol_text(const fixed_string<N>& txt) :
      unicode_(detail::to_u8string(txt)), ascii_(txt)
  {
    detail::validate_ascii_string<N>(txt.data_);
  }

  constexpr basic_symbol_text(const char8_t (&u)[N + 1], const char (&a)[M + 1]) : unicode_(u), ascii_(a)
  {
    detail::validate_ascii_string<M>(a);
  }

  constexpr basic_symbol_text(const fixed_u8string<N>& u, const fixed_string<M>& a) : unicode_(u), ascii_(a)
  {
    detail::validate_ascii_string<M>(a.data_);
  }

  [[nodiscard]] constexpr const auto& unicode() const { return unicode_; }
  [[nodiscard]] constexpr const auto& ascii() const { return ascii_; }

  [[nodiscard]] constexpr bool empty() const { return unicode().empty() && ascii().empty(); }

  template<std::size_t N2, std::size_t M2>
  [[nodiscard]] constexpr friend basic_symbol_text<N + N2, M + M2> operator+(const basic_symbol_text& lhs,
                                                                             const basic_symbol_text<N2, M2>& rhs)
  {
    return basic_symbol_text<N + N2, M + M2>(lhs.unicode() + rhs.unicode(), lhs.ascii() + rhs.ascii());
  }

  template<std::size_t N2, std::size_t M2>
  [[nodiscard]] friend constexpr auto operator<=>(const basic_symbol_text& lhs,
                                                  const basic_symbol_text<N2, M2>& rhs) noexcept
  {
    MP_UNITS_DIAGNOSTIC_PUSH
    MP_UNITS_DIAGNOSTIC_IGNORE_ZERO_AS_NULLPOINTER_CONSTANT
    if (const auto cmp = lhs.unicode() <=> rhs.unicode(); cmp != 0) return cmp;
    MP_UNITS_DIAGNOSTIC_POP
    return lhs.ascii() <=> rhs.ascii();
  }

  template<std::size_t N2, std::size_t M2>
  [[nodiscard]] friend constexpr bool operator==(const basic_symbol_text& lhs,
                                                 const basic_symbol_text<N2, M2>& rhs) noexcept
  {
    return lhs.unicode() == rhs.unicode() && lhs.ascii() == rhs.ascii();
  }
};

basic_symbol_text(char) -> basic_symbol_text<1, 1>;

template<std::size_t N>
basic_symbol_text(const char (&)[N]) -> basic_symbol_text<N - 1, N - 1>;

template<std::size_t N>
basic_symbol_text(const fixed_string<N>&) -> basic_symbol_text<N, N>;

template<std::size_t N, std::size_t M>
basic_symbol_text(const char8_t (&)[N], const char (&)[M]) -> basic_symbol_text<N - 1, M - 1>;

template<std::size_t N, std::size_t M>
basic_symbol_text(const fixed_u8string<N>&, const fixed_string<M>&) -> basic_symbol_text<N, M>;

}  // namespace mp_units
