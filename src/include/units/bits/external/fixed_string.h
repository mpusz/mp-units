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

#include <cstdlib>
#include <ostream>

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

  constexpr basic_fixed_string(CharT ch) noexcept { data_[0] = ch; }

  constexpr basic_fixed_string(const CharT (&txt)[N + 1]) noexcept
  {
    for (std::size_t i = 0; i < N; ++i) data_[i] = txt[i];
  }

  [[nodiscard]] constexpr std::size_t size() const noexcept { return N; }
  [[nodiscard]] constexpr const CharT* c_str() const noexcept { return data_; }
  [[nodiscard]] constexpr const CharT& operator[](std::size_t index) const noexcept { return data_[index]; }
  [[nodiscard]] constexpr CharT operator[](std::size_t index) noexcept { return data_[index]; }

  [[nodiscard]] constexpr iterator begin() noexcept { return std::begin(data_); }
  [[nodiscard]] constexpr const_iterator begin() const noexcept { return std::begin(data_); }
  [[nodiscard]] constexpr iterator end() noexcept { return std::end(data_); }
  [[nodiscard]] constexpr const_iterator end() const noexcept { return std::end(data_); }

  template<std::size_t N2>
  [[nodiscard]] constexpr friend basic_fixed_string<CharT, N + N2> operator+(
      const basic_fixed_string& lhs, const basic_fixed_string<CharT, N2>& rhs) noexcept
  {
    CharT txt[N + N2 + 1] = {};

    for (size_t i = 0; i != N; ++i) txt[i] = lhs[i];
    for (size_t i = 0; i != N2; ++i) txt[N + i] = rhs[i];

    return basic_fixed_string<CharT, N + N2>(txt);
  }

#if __GNUC__ >= 10

  template<typename CharT2, std::size_t N2>
  [[nodiscard]] friend constexpr auto operator<=>(const basic_fixed_string& lhs,
                                                  const basic_fixed_string<CharT2, N2>& rhs)
  {
    return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
  }

  template<typename CharT2, std::size_t N2>
  [[nodiscard]] friend constexpr bool operator==(const basic_fixed_string& lhs,
                                                 const basic_fixed_string<CharT2, N2>& rhs)
  {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
  }

#else

  [[nodiscard]] constexpr friend bool operator==(const basic_fixed_string& lhs, const basic_fixed_string& rhs) noexcept
  {
    for (size_t i = 0; i != lhs.size(); ++i)
      if (lhs.data_[i] != rhs.data_[i]) return false;
    return true;
  }

  [[nodiscard]] constexpr friend bool operator!=(const basic_fixed_string& lhs, const basic_fixed_string& rhs) noexcept
  {
    return !(lhs == rhs);
  }

  template<typename CharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator==(const basic_fixed_string&,
                                                 const basic_fixed_string<CharT2, N2>&) noexcept
  {
    return false;
  }

  template<typename CharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator!=(const basic_fixed_string&,
                                                 const basic_fixed_string<CharT2, N2>&) noexcept
  {
    return true;
  }

  template<typename CharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator<(const basic_fixed_string& lhs,
                                                const basic_fixed_string<CharT2, N2>& rhs) noexcept
  {
    using std::begin, std::end;
    auto first1 = begin(lhs.data_);
    auto first2 = begin(rhs.data_);
    const auto last1 = std::prev(end(lhs.data_));  // do not waste time for '\0'
    const auto last2 = std::prev(end(rhs.data_));

    for (; (first1 != last1) && (first2 != last2); ++first1, (void)++first2) {
      if (*first1 < *first2) return true;
      if (*first2 < *first1) return false;
    }
    return first1 == last1 && first2 != last2;
  }

  template<typename CharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator>(const basic_fixed_string& lhs,
                                                const basic_fixed_string<CharT2, N2>& rhs) noexcept
  {
    return rhs < lhs;
  }

  template<typename CharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator<=(const basic_fixed_string& lhs,
                                                const basic_fixed_string<CharT2, N2>& rhs) noexcept
  {
    return !(rhs < lhs);
  }

  template<typename CharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator>=(const basic_fixed_string& lhs,
                                                 const basic_fixed_string<CharT2, N2>& rhs) noexcept
  {
    return !(lhs < rhs);
  }

#endif

  template<class Traits>
  friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os,
                                                       const basic_fixed_string& txt)
  {
    return os << txt.c_str();
  }
};

template<typename CharT, std::size_t N>
basic_fixed_string(const CharT (&str)[N]) -> basic_fixed_string<CharT, N - 1>;

template<typename CharT>
basic_fixed_string(CharT) -> basic_fixed_string<CharT, 1>;

template<std::size_t N>
using fixed_string = basic_fixed_string<char, N>;

}  // namespace units
