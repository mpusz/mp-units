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

#include <mp-units/bits/external/hacks.h>  // IWYU pragma: keep
// TODO use <algorithm> when moved to C++20 modules (parsing takes too long for each translation unit)
#include <mp-units/bits/external/algorithm.h>

// IWYU pragma: begin_exports
#include <compare>
#include <cstdlib>
// IWYU pragma: end_exports

#include <cstddef>


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

  constexpr explicit(false) basic_fixed_string(CharT ch) noexcept
    requires(N == 1)
  {
    data_[0] = ch;
  }

  constexpr explicit(false) basic_fixed_string(const CharT (&txt)[N + 1]) noexcept
  {
    if constexpr (N != 0)
      for (std::size_t i = 0; i < N; ++i) data_[i] = txt[i];
  }

  constexpr basic_fixed_string(const CharT* ptr, std::integral_constant<std::size_t, N>) noexcept
  {
    if constexpr (N != 0)
      for (std::size_t i = 0; i < N; ++i) data_[i] = *ptr++;
  }

  [[nodiscard]] constexpr bool empty() const noexcept { return N == 0; }
  [[nodiscard]] constexpr size_type size() const noexcept { return N; }
  [[nodiscard]] constexpr const_pointer data() const noexcept { return data_; }
  [[nodiscard]] constexpr const CharT* c_str() const noexcept { return data(); }
  [[nodiscard]] constexpr const_reference operator[](size_type index) const noexcept { return data()[index]; }

  [[nodiscard]] constexpr const_iterator begin() const noexcept { return data(); }
  [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return data(); }
  [[nodiscard]] constexpr const_iterator end() const noexcept { return data() + size(); }
  [[nodiscard]] constexpr const_iterator cend() const noexcept { return data() + size(); }

  [[nodiscard]] constexpr std::basic_string_view<CharT> view() const noexcept
  {
    return std::basic_string_view<CharT>(*this);
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

  [[nodiscard]] constexpr bool operator==(const basic_fixed_string& other) const
  {
    if (size() != other.size()) return false;
    return detail::equal(begin(), end(), other.begin());  // TODO std::ranges::equal(*this, other)
  }

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
};

template<typename CharT>
basic_fixed_string(CharT) -> basic_fixed_string<CharT, 1>;

template<typename CharT, std::size_t N>
basic_fixed_string(const CharT (&str)[N]) -> basic_fixed_string<CharT, N - 1>;

template<typename CharT, std::size_t N>
basic_fixed_string(const CharT* ptr, std::integral_constant<std::size_t, N>) -> basic_fixed_string<CharT, N>;

template<std::size_t N>
using fixed_string = basic_fixed_string<char, N>;

}  // namespace mp_units
