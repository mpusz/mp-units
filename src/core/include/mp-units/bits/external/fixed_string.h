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

// TODO use <algorithm> when moved to C++20 modules (parsing takes too long for each translation unit)
#include <mp-units/bits/external/algorithm.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/compat_macros.h>  // IWYU pragma: keep

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <gsl/gsl-lite.hpp>
#include <compare>
#include <cstddef>
#include <cstdlib>
#include <ostream>
#include <string_view>
#endif

MP_UNITS_EXPORT
namespace mp_units {

/**
 * @brief A compile-time fixed string
 *
 * @tparam CharT Character type to be used by the string
 * @tparam N The size of the string
 */
template<typename CharT, std::size_t N>
struct basic_fixed_string {
  CharT data_[N + 1] = {};

  using value_type = CharT;
  using pointer = CharT*;
  using const_pointer = const CharT*;
  using reference = CharT&;
  using const_reference = const CharT&;
  using const_iterator = const CharT*;
  using iterator = const_iterator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  constexpr explicit(false) basic_fixed_string(const CharT (&txt)[N + 1]) noexcept
  {
    gsl_Expects(txt[N] == CharT{});
    for (std::size_t i = 0; i < N; ++i) data_[i] = txt[i];
  }

  template<std::input_iterator It, std::sentinel_for<It> S>
    requires std::convertible_to<std::iter_value_t<It>, CharT>
  constexpr explicit basic_fixed_string(It first, S last) noexcept
  {
    gsl_Expects(std::distance(first, last) == N);
    for (auto it = data_; first != last; ++first, ++it) *it = *first;
  }

  template<std::convertible_to<CharT>... Rest>
    requires(1 + sizeof...(Rest) == N)
  constexpr explicit basic_fixed_string(CharT first, Rest... rest) noexcept : data_{first, rest..., CharT{}}
  {
  }

  [[nodiscard]] constexpr bool empty() const noexcept { return N == 0; }
  [[nodiscard]] constexpr size_type size() const noexcept { return N; }
  [[nodiscard]] constexpr const_pointer data() const noexcept { return data_; }
  [[nodiscard]] constexpr const CharT* c_str() const noexcept { return data(); }
  [[nodiscard]] constexpr value_type operator[](size_type index) const noexcept
  {
    gsl_Expects(index < N);
    return data()[index];
  }

  [[nodiscard]] constexpr const_iterator begin() const noexcept { return data(); }
  [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return data(); }
  [[nodiscard]] constexpr const_iterator end() const noexcept { return data() + size(); }
  [[nodiscard]] constexpr const_iterator cend() const noexcept { return data() + size(); }

  [[nodiscard]] constexpr operator std::basic_string_view<CharT>() const noexcept
  {
    return std::basic_string_view<CharT>(cbegin(), cend());
  }

  template<std::size_t N2>
  [[nodiscard]] constexpr friend basic_fixed_string<CharT, N + N2> operator+(
    const basic_fixed_string& lhs, const basic_fixed_string<CharT, N2>& rhs) noexcept
  {
    CharT txt[N + N2 + 1] = {};

    for (size_t i = 0; i != N; ++i) txt[i] = lhs[i];
    for (size_t i = 0; i != N2; ++i) txt[N + i] = rhs[i];

    return basic_fixed_string<CharT, N + N2>(txt);
  }

  [[nodiscard]] constexpr bool operator==(const basic_fixed_string&) const = default;

  template<std::size_t N2>
  [[nodiscard]] friend constexpr bool operator==(const basic_fixed_string&, const basic_fixed_string<CharT, N2>&)
  {
    return false;
  }

  template<std::size_t N2>
  [[nodiscard]] friend constexpr auto operator<=>(const basic_fixed_string& lhs,
                                                  const basic_fixed_string<CharT, N2>& rhs)
  {
    // TODO std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    return detail::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
  }

  template<typename Traits>
  friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os,
                                                       const basic_fixed_string<CharT, N>& str)
  {
    return os << str.c_str();
  }
};

template<typename CharT, std::size_t N>
basic_fixed_string(const CharT (&str)[N]) -> basic_fixed_string<CharT, N - 1>;

template<typename CharT, std::convertible_to<CharT>... Rest>
basic_fixed_string(CharT, Rest...) -> basic_fixed_string<CharT, 1 + sizeof...(Rest)>;

template<std::size_t N>
using fixed_string = basic_fixed_string<char, N>;

template<std::size_t N>
using fixed_wstring = basic_fixed_string<wchar_t, N>;

template<std::size_t N>
using fixed_u8string = basic_fixed_string<char8_t, N>;

template<std::size_t N>
using fixed_u16string = basic_fixed_string<char16_t, N>;

template<std::size_t N>
using fixed_u32string = basic_fixed_string<char32_t, N>;

}  // namespace mp_units

template<typename CharT, std::size_t N>
struct MP_UNITS_STD_FMT::formatter<mp_units::basic_fixed_string<CharT, N>> : formatter<std::basic_string_view<CharT>> {
  template<typename FormatContext>
  auto format(const mp_units::basic_fixed_string<CharT, N>& str, FormatContext& ctx) const -> decltype(ctx.out())
  {
    return formatter<std::basic_string_view<CharT>>::format(std::basic_string_view<CharT>(str), ctx);
  }
};
