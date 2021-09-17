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

#include <units/bits/external/hacks.h> // IWYU pragma: keep

// IWYU pragma: begin_exports
#include <cstdlib>
#include <compare>
// IWYU pragma: end_exports

#include <cstddef>

// TODO use <algorithm> when moved to C++20 modules (parsing takes too long for each translation unit)

namespace units {

/**
 * @brief A compile-time fixed string
 * 
 * @tparam CharT Character type to be used by the string
 * @tparam N The size of the string
 */
template<typename CharT, std::size_t N>
struct basic_fixed_string {
  CharT data_[N + 1] = {};

  using iterator = CharT*;
  using const_iterator = const CharT*;

  constexpr explicit(false) basic_fixed_string(CharT ch) noexcept { data_[0] = ch; }

  constexpr explicit(false) basic_fixed_string(const CharT (&txt)[N + 1]) noexcept
  {
    if constexpr (N != 0)
      for (std::size_t i = 0; i < N; ++i) data_[i] = txt[i];
  }

  [[nodiscard]] constexpr bool empty() const noexcept { return N == 0; }
  [[nodiscard]] constexpr std::size_t size() const noexcept { return N; }
  [[nodiscard]] constexpr const CharT* data() const noexcept { return data_; }
  [[nodiscard]] constexpr const CharT* c_str() const noexcept { return data(); }
  [[nodiscard]] constexpr const CharT& operator[](std::size_t index) const noexcept { return data()[index]; }
  [[nodiscard]] constexpr CharT operator[](std::size_t index) noexcept { return data()[index]; }

  [[nodiscard]] constexpr iterator begin() noexcept { return data(); }
  [[nodiscard]] constexpr const_iterator begin() const noexcept { return data(); }
  [[nodiscard]] constexpr iterator end() noexcept { return data() + size(); }
  [[nodiscard]] constexpr const_iterator end() const noexcept { return data() + size(); }

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
    if (size() != other.size())
      return false;
    for (size_t i = 0; i != size(); ++i) {
      if ((*this)[i] != other[i])
        return false;
    }
    return true;
    // return std::ranges::equal(*this, other);
  }

  template<std::size_t N2>
  [[nodiscard]] friend constexpr bool operator==(const basic_fixed_string&, const basic_fixed_string<CharT, N2>&) { return false; }

  template<std::size_t N2>
  [[nodiscard]] friend constexpr auto operator<=>(const basic_fixed_string& lhs, const basic_fixed_string<CharT, N2>& rhs)
  {
    auto first1 = lhs.begin();
    const auto last1 = lhs.end();
    auto first2 = rhs.begin();
    const auto last2 = rhs.end();
    auto comp = std::compare_three_way();

    for (; first1 != last1 && first2 != last2; ++first1, ++first2)
      if (auto cmp = comp(*first1, *first2); cmp != 0)
        return cmp;
 
    return first1 != last1 ? std::strong_ordering::greater :
           first2 != last2 ? std::strong_ordering::less :
                             std::strong_ordering::equal;
    // return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
  }
};

template<typename CharT, std::size_t N>
basic_fixed_string(const CharT (&str)[N]) -> basic_fixed_string<CharT, N - 1>;

template<typename CharT>
basic_fixed_string(CharT) -> basic_fixed_string<CharT, 1>;

template<std::size_t N>
using fixed_string = basic_fixed_string<char, N>;

}  // namespace units
