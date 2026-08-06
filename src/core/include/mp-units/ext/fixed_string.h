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

// To be replaced with:
// P3094: std::basic_fixed_string

// NOLINTBEGIN(*-avoid-c-arrays)
#pragma once

#include <mp-units/bits/hacks.h>  // IWYU pragma: keep
#include <mp-units/bits/module_macros.h>
#include <mp-units/compat_macros.h>  // IWYU pragma: keep
#include <mp-units/ext/type_traits.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/ext/contracts.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <compare>  // IWYU pragma: export
#include <concepts>
#include <cstddef>
#include <cstdlib>
#include <ranges>
#include <string_view>
#endif  // MP_UNITS_IMPORT_STD

#if MP_UNITS_HOSTED
#include <mp-units/ext/format.h>
#ifndef MP_UNITS_IMPORT_STD
#include <iosfwd>
#endif
#endif  // MP_UNITS_HOSTED
#endif  // MP_UNITS_IN_MODULE_INTERFACE

// The forward declaration must be exported just like the definition below: in a module build an
// unexported declaration has module linkage, and the exported definition then cannot redeclare it.
MP_UNITS_EXPORT
namespace mp_units {

template<typename CharT, std::size_t N>
class basic_fixed_string;

}

namespace mp_units {

namespace detail {

// Hidden-friend interface for `basic_fixed_string`. Concatenation and comparison are
// heterogeneous in the size parameter, so they were never members to begin with; hosting them
// in a non-template base means the whole set is declared ONCE per program instead of once per
// `basic_fixed_string<CharT, N>` specialization. A TU that only includes an mp-units system
// header already mints ~22 of those (98 for the full CODATA set), purely to spell unit symbols
// - none of which ever concatenates at runtime. ADL still finds every operator, because a base
// class is an associated class of its derived type.
struct fixed_string_iface {
  template<typename CharT, std::size_t N, std::size_t N2>
  [[nodiscard]] constexpr friend basic_fixed_string<CharT, N + N2> operator+(
    const basic_fixed_string<CharT, N>& lhs, const basic_fixed_string<CharT, N2>& rhs) noexcept
  {
    CharT txt[N + N2];
    CharT* it = txt;
    for (CharT ch : lhs) *it++ = ch;
    for (CharT ch : rhs) *it++ = ch;
    return basic_fixed_string<CharT, N + N2>(txt, it);
  }

  template<typename CharT, std::size_t N>
  [[nodiscard]] constexpr friend basic_fixed_string<CharT, N + 1> operator+(const basic_fixed_string<CharT, N>& lhs,
                                                                            CharT rhs) noexcept
  {
    CharT txt[N + 1];
    CharT* it = txt;
    for (CharT ch : lhs) *it++ = ch;
    *it++ = rhs;
    return basic_fixed_string<CharT, N + 1>(txt, it);
  }

  template<typename CharT, std::size_t N>
  [[nodiscard]] constexpr friend basic_fixed_string<CharT, 1 + N> operator+(
    const CharT lhs, const basic_fixed_string<CharT, N>& rhs) noexcept
  {
    CharT txt[1 + N];
    CharT* it = txt;
    *it++ = lhs;
    for (CharT ch : rhs) *it++ = ch;
    return basic_fixed_string<CharT, 1 + N>(txt, it);
  }

  template<typename CharT, std::size_t N, std::size_t N2>
  [[nodiscard]] consteval friend basic_fixed_string<CharT, N + N2 - 1> operator+(
    const basic_fixed_string<CharT, N>& lhs, const CharT (&rhs)[N2]) noexcept
  {
    MP_UNITS_PRECONDITION(rhs[N2 - 1] == CharT{});
    CharT txt[N + N2];
    CharT* it = txt;
    for (CharT ch : lhs) *it++ = ch;
    for (CharT ch : rhs) *it++ = ch;
    return txt;
  }

  template<typename CharT, std::size_t N, std::size_t N1>
  [[nodiscard]] consteval friend basic_fixed_string<CharT, N1 + N - 1> operator+(
    const CharT (&lhs)[N1], const basic_fixed_string<CharT, N>& rhs) noexcept
  {
    MP_UNITS_PRECONDITION(lhs[N1 - 1] == CharT{});
    CharT txt[N1 + N];
    CharT* it = txt;
    for (std::size_t i = 0; i != N1 - 1; ++i) *it++ = lhs[i];
    for (CharT ch : rhs) *it++ = ch;
    *it++ = CharT();
    return txt;
  }

  // non-member comparison functions
  template<typename CharT, std::size_t N, std::size_t N2>
  [[nodiscard]] friend constexpr bool operator==(const basic_fixed_string<CharT, N>& lhs,
                                                 const basic_fixed_string<CharT, N2>& rhs)
  {
    return lhs.view() == rhs.view();
  }

  template<typename CharT, std::size_t N, std::size_t N2>
  [[nodiscard]] friend consteval bool operator==(const basic_fixed_string<CharT, N>& lhs, const CharT (&rhs)[N2])
  {
    MP_UNITS_PRECONDITION(rhs[N2 - 1] == CharT{});
    return lhs.view() == std::basic_string_view<CharT>(std::cbegin(rhs), std::cend(rhs) - 1);
  }

  template<typename CharT, std::size_t N, std::size_t N2>
  [[nodiscard]] friend constexpr auto operator<=>(const basic_fixed_string<CharT, N>& lhs,
                                                  const basic_fixed_string<CharT, N2>& rhs)
  {
    return lhs.view() <=> rhs.view();
  }

  template<typename CharT, std::size_t N, std::size_t N2>
  [[nodiscard]] friend consteval auto operator<=>(const basic_fixed_string<CharT, N>& lhs, const CharT (&rhs)[N2])
  {
    MP_UNITS_PRECONDITION(rhs[N2 - 1] == CharT{});
    return lhs.view() <=> std::basic_string_view<CharT>(std::cbegin(rhs), std::cend(rhs) - 1);
  }

  // specialized algorithms
  //
  // A hidden friend on purpose: the customization point is meant to be reached through the
  // `using std::swap; swap(lhs, rhs);` two-step, and hosting it here means a qualified
  // `mp_units::swap(lhs, rhs)` - which defeats that mechanism and is never the right call -
  // does not compile in the first place.
  template<typename CharT, std::size_t N>
  friend constexpr void swap(basic_fixed_string<CharT, N>& lhs, basic_fixed_string<CharT, N>& rhs) noexcept
  {
    lhs.swap(rhs);
  }

  // inserters and extractors
#if MP_UNITS_HOSTED
  template<typename CharT, std::size_t N>
  friend std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, const basic_fixed_string<CharT, N>& str)
  {
    return os << str.c_str();
  }
#endif
};

}  // namespace detail

}  // namespace mp_units

MP_UNITS_EXPORT
namespace mp_units {

/**
 * @brief A compile-time fixed string
 *
 * @tparam CharT Character type to be used by the string
 * @tparam N The size of the string
 */
template<typename CharT, std::size_t N>
class basic_fixed_string : public detail::fixed_string_iface {
public:
  CharT data_[N + 1] = {};  // exposition only

  // types
  using value_type = CharT;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using reference = value_type&;
  using const_reference = const value_type&;
  using const_iterator = const value_type*;
  using iterator = const_iterator;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using reverse_iterator = const_reverse_iterator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  // construction and assignment
  template<std::same_as<CharT>... Chars>
    requires(sizeof...(Chars) == N) && (... && !std::is_pointer_v<Chars>)
  [[nodiscard]] constexpr explicit basic_fixed_string(Chars... chars) noexcept : data_{chars..., CharT{}}
  {
  }

  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  [[nodiscard]] consteval explicit(false) basic_fixed_string(const CharT (&txt)[N + 1]) noexcept
  {
    MP_UNITS_PRECONDITION(txt[N] == CharT{});
    for (std::size_t i = 0; i < N; ++i) data_[i] = txt[i];
  }

  template<std::input_iterator It, std::sentinel_for<It> S>
    requires std::same_as<std::iter_value_t<It>, CharT>
  [[nodiscard]] constexpr basic_fixed_string(It begin, S end)
  {
    MP_UNITS_PRECONDITION(std::distance(begin, end) == N);
    for (auto it = data_; begin != end; ++begin, ++it) *it = *begin;
  }

  template<std::ranges::input_range R>
    requires std::same_as<std::ranges::range_value_t<R>, CharT>
  [[nodiscard]] constexpr basic_fixed_string(std::from_range_t, R&& r)
  {
    MP_UNITS_PRECONDITION(std::ranges::size(r) == N);
    for (auto it = data_; auto&& v : std::forward<R>(r)) *it++ = std::forward<decltype(v)>(v);
  }

  [[nodiscard]] constexpr basic_fixed_string(const basic_fixed_string&) noexcept = default;
  constexpr basic_fixed_string& operator=(const basic_fixed_string&) noexcept = default;

  // iterator support
  [[nodiscard]] constexpr const_iterator begin() const noexcept { return data(); }
  [[nodiscard]] constexpr const_iterator end() const noexcept { return data() + size(); }
  [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return begin(); }
  [[nodiscard]] constexpr const_iterator cend() const noexcept { return end(); }
  [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
  [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
  [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return rend(); }

  // capacity
  static constexpr std::integral_constant<size_type, N> size{};
  static constexpr std::integral_constant<size_type, N> length{};
  static constexpr std::integral_constant<size_type, N> max_size{};
  static constexpr std::bool_constant<N == 0> empty{};

  // element access
  [[nodiscard]] constexpr const_reference operator[](size_type pos) const MP_UNITS_PRE(pos < N)
  {
    MP_UNITS_EXPECTS(pos < N);
    return data()[pos];
  }

#if MP_UNITS_HOSTED
  [[nodiscard]] constexpr const_reference at(size_type pos) const
  {
    if (pos >= size()) throw std::out_of_range("basic_fixed_string::at");
    return (*this)[pos];
  }
#endif

  [[nodiscard]] constexpr const_reference front() const MP_UNITS_PRE(!empty())
  {
    MP_UNITS_EXPECTS(!empty());
    return (*this)[0];
  }
  [[nodiscard]] constexpr const_reference back() const MP_UNITS_PRE(!empty())
  {
    MP_UNITS_EXPECTS(!empty());
    return (*this)[N - 1];
  }

  // modifiers
  constexpr void swap(basic_fixed_string& s) noexcept
  {
    // element-wise rather than `swap_ranges`: `begin()`/`end()` yield `const_iterator`, so the
    // range algorithms cannot write through them. `data_[N]` is the terminator in both objects.
    for (std::size_t i = 0; i != N; ++i) {
      const CharT tmp = data_[i];
      data_[i] = s.data_[i];
      s.data_[i] = tmp;
    }
  }

  // string operations
  [[nodiscard]] constexpr const_pointer c_str() const noexcept { return data(); }
  [[nodiscard]] constexpr const_pointer data() const noexcept { return static_cast<const_pointer>(data_); }
  [[nodiscard]] constexpr std::basic_string_view<CharT> view() const noexcept
  {
    return std::basic_string_view<CharT>(cbegin(), cend());
  }
  // NOLINTNEXTLINE(*-explicit-conversions, google-explicit-constructor)
  [[nodiscard]] constexpr explicit(false) operator std::basic_string_view<CharT>() const noexcept { return view(); }
};

// deduction guides
template<typename CharT, std::same_as<CharT>... Rest>
basic_fixed_string(CharT, Rest...) -> basic_fixed_string<CharT, 1 + sizeof...(Rest)>;

template<typename CharT, std::size_t N>
basic_fixed_string(const CharT (&str)[N]) -> basic_fixed_string<CharT, N - 1>;

template<typename CharT, std::size_t N>
basic_fixed_string(std::from_range_t, std::array<CharT, N>) -> basic_fixed_string<CharT, N>;

// typedef-names
template<std::size_t N>
using fixed_string = basic_fixed_string<char, N>;
template<std::size_t N>
using fixed_u8string = basic_fixed_string<char8_t, N>;
template<std::size_t N>
using fixed_u16string = basic_fixed_string<char16_t, N>;
template<std::size_t N>
using fixed_u32string = basic_fixed_string<char32_t, N>;
template<std::size_t N>
using fixed_wstring = basic_fixed_string<wchar_t, N>;

}  // namespace mp_units

// hash support
template<std::size_t N>
struct std::hash<mp_units::fixed_string<N>> : std::hash<std::string_view> {};
template<std::size_t N>
struct std::hash<mp_units::fixed_u8string<N>> : std::hash<std::u8string_view> {};
template<std::size_t N>
struct std::hash<mp_units::fixed_u16string<N>> : std::hash<std::u16string_view> {};
template<std::size_t N>
struct std::hash<mp_units::fixed_u32string<N>> : std::hash<std::u32string_view> {};

// NOTE: We use std::basic_string_view<wchar_t> instead of std::wstring_view because LLVM only makes std::wstring_view
// available when the macro _LIBCPP_HAS_WIDE_CHARACTERS is set to 1. The `wchar_t` type can continue to be used
// without that macro definition. To avoid requiring an additional macro, we simply use std::basic_string_view<wchar_t>,
// which is the underlying definition of std::wstring_view.
template<std::size_t N>
struct std::hash<mp_units::fixed_wstring<N>> : std::hash<std::basic_string_view<wchar_t>> {};

#if MP_UNITS_HOSTED
// formatting support
template<typename CharT, std::size_t N>
struct MP_UNITS_STD_FMT::formatter<mp_units::basic_fixed_string<CharT, N>> : formatter<std::basic_string_view<CharT>> {
  template<typename FormatContext>
  auto format(const mp_units::basic_fixed_string<CharT, N>& str, FormatContext& ctx) const -> decltype(ctx.out())
  {
    return formatter<std::basic_string_view<CharT>>::format(std::basic_string_view<CharT>(str), ctx);
  }
};
#endif

// NOLINTEND(*-avoid-c-arrays)
