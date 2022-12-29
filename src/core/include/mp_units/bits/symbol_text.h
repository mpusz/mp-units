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
#include <mp_units/bits/external/fixed_string.h>
#include <mp_units/bits/external/hacks.h>
#include <compare>
#include <cstddef>
#include <cstdint>
// IWYU pragma: end_exports

#include <gsl/gsl-lite.hpp>

namespace mp_units {

namespace detail {

constexpr void validate_ascii_char([[maybe_unused]] char c) noexcept { gsl_Expects((c & 0x80) == 0); }

template<std::size_t N>
constexpr void validate_ascii_string([[maybe_unused]] const char (&s)[N + 1]) noexcept
{
#ifndef NDEBUG
  if constexpr (N != 0)
    for (size_t i = 0; i < N; ++i) validate_ascii_char(s[i]);
#endif
}

}  // namespace detail


/**
 * @brief A symbol text representation
 *
 * This class template is responsible for definition and handling of a symbol text
 * representation. In the libary it is used to define symbols of units and prefixes.
 * Each symbol can have two versions: Unicode and ASCI-only.
 *
 * @tparam UnicodeCharT Character type to be used for a Unicode representation
 * @tparam N The size of a Unicode symbol
 * @tparam M The size of the ASCII-only symbol
 */
template<typename UnicodeCharT, std::size_t N, std::size_t M>
struct basic_symbol_text {
  basic_fixed_string<UnicodeCharT, N> unicode_;
  basic_fixed_string<char, M> ascii_;

  constexpr explicit(false) basic_symbol_text(char txt) noexcept : unicode_(txt), ascii_(txt)
  {
    detail::validate_ascii_char(txt);
  }
  constexpr basic_symbol_text(UnicodeCharT u, char a) noexcept : unicode_(u), ascii_(a)
  {
    detail::validate_ascii_char(a);
  }
  constexpr explicit(false) basic_symbol_text(const char (&txt)[N + 1]) noexcept : unicode_(txt), ascii_(txt)
  {
    detail::validate_ascii_string<N>(txt);
  }
  constexpr explicit(false) basic_symbol_text(const basic_fixed_string<char, N>& txt) noexcept :
      unicode_(txt), ascii_(txt)
  {
    detail::validate_ascii_string<N>(txt.data_);
  }
  constexpr basic_symbol_text(const UnicodeCharT (&u)[N + 1], const char (&a)[M + 1]) noexcept : unicode_(u), ascii_(a)
  {
    detail::validate_ascii_string<M>(a);
  }
  constexpr basic_symbol_text(const basic_fixed_string<UnicodeCharT, N>& u,
                              const basic_fixed_string<char, M>& a) noexcept :
      unicode_(u), ascii_(a)
  {
    detail::validate_ascii_string<M>(a.data_);
  }

  [[nodiscard]] constexpr auto& unicode() { return unicode_; }
  [[nodiscard]] constexpr const auto& unicode() const { return unicode_; }
  [[nodiscard]] constexpr auto& ascii() { return ascii_; }
  [[nodiscard]] constexpr const auto& ascii() const { return ascii_; }

  [[nodiscard]] constexpr bool empty() const { return unicode().empty() && ascii().empty(); }

  template<std::size_t N2, std::size_t M2>
  [[nodiscard]] constexpr friend basic_symbol_text<UnicodeCharT, N + N2, M + M2> operator+(
    const basic_symbol_text& lhs, const basic_symbol_text<UnicodeCharT, N2, M2>& rhs) noexcept
  {
    return basic_symbol_text<UnicodeCharT, N + N2, M + M2>(lhs.unicode() + rhs.unicode(), lhs.ascii() + rhs.ascii());
  }

  template<std::size_t N2>
  [[nodiscard]] constexpr friend basic_symbol_text<UnicodeCharT, N + N2, M + N2> operator+(
    const basic_symbol_text& lhs, const basic_fixed_string<UnicodeCharT, N2>& rhs) noexcept
  {
    return lhs + basic_symbol_text<UnicodeCharT, N2, N2>(rhs);
  }

  template<std::size_t N2>
  [[nodiscard]] constexpr friend basic_symbol_text<UnicodeCharT, N + N2, M + N2> operator+(
    const basic_fixed_string<UnicodeCharT, N2>& lhs, const basic_symbol_text& rhs) noexcept
  {
    return basic_symbol_text<UnicodeCharT, N2, N2>(lhs) + rhs;
  }

  template<std::size_t N2>
  [[nodiscard]] constexpr friend basic_symbol_text<UnicodeCharT, N + N2 - 1, M + N2 - 1> operator+(
    const basic_symbol_text& lhs, const UnicodeCharT (&rhs)[N2]) noexcept
  {
    return lhs + basic_symbol_text<UnicodeCharT, N2 - 1, N2 - 1>(rhs);
  }

  template<std::size_t N2>
  [[nodiscard]] constexpr friend basic_symbol_text<UnicodeCharT, N + N2 - 1, M + N2 - 1> operator+(
    const UnicodeCharT (&lhs)[N2], const basic_symbol_text& rhs) noexcept
  {
    return basic_symbol_text<UnicodeCharT, N2 - 1, N2 - 1>(lhs) + rhs;
  }

  [[nodiscard]] constexpr friend basic_symbol_text<UnicodeCharT, N + 1, M + 1> operator+(const basic_symbol_text& lhs,
                                                                                         UnicodeCharT rhs) noexcept
  {
    return lhs + basic_symbol_text<UnicodeCharT, 1, 1>(rhs);
  }

  [[nodiscard]] constexpr friend basic_symbol_text<UnicodeCharT, N + 1, M + 1> operator+(
    UnicodeCharT lhs, const basic_symbol_text& rhs) noexcept
  {
    return basic_symbol_text<UnicodeCharT, 1, 1>(lhs) + rhs;
  }

  template<typename UnicodeCharT2, std::size_t N2, std::size_t M2>
  [[nodiscard]] friend constexpr auto operator<=>(const basic_symbol_text& lhs,
                                                  const basic_symbol_text<UnicodeCharT2, N2, M2>& rhs) noexcept
  {
    UNITS_DIAGNOSTIC_PUSH
    UNITS_DIAGNOSTIC_IGNORE_ZERO_AS_NULLPOINTER_CONSTANT
    if (const auto cmp = lhs.unicode() <=> rhs.unicode(); cmp != 0) return cmp;
    UNITS_DIAGNOSTIC_POP
    return lhs.ascii() <=> rhs.ascii();
  }

  template<typename UnicodeCharT2, std::size_t N2, std::size_t M2>
  [[nodiscard]] friend constexpr bool operator==(const basic_symbol_text& lhs,
                                                 const basic_symbol_text<UnicodeCharT2, N2, M2>& rhs) noexcept
  {
    return lhs.unicode() == rhs.unicode() && lhs.ascii() == rhs.ascii();
  }
};

basic_symbol_text(char)->basic_symbol_text<char, 1, 1>;

template<typename UnicodeCharT>
basic_symbol_text(UnicodeCharT, char) -> basic_symbol_text<UnicodeCharT, 1, 1>;

template<std::size_t N>
basic_symbol_text(const char (&)[N]) -> basic_symbol_text<char, N - 1, N - 1>;

template<std::size_t N>
basic_symbol_text(const basic_fixed_string<char, N>&) -> basic_symbol_text<char, N, N>;

template<typename UnicodeCharT, std::size_t N, std::size_t M>
basic_symbol_text(const UnicodeCharT (&)[N], const char (&)[M]) -> basic_symbol_text<UnicodeCharT, N - 1, M - 1>;

template<typename UnicodeCharT, std::size_t N, std::size_t M>
basic_symbol_text(const basic_fixed_string<UnicodeCharT, N>&, const basic_fixed_string<char, M>&)
  -> basic_symbol_text<UnicodeCharT, N, M>;

}  // namespace mp_units
