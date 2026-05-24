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

#include <mp-units/bits/hacks.h>  // IWYU pragma: keep
#include <mp-units/bits/int_power.h>
#include <mp-units/ext/type_traits.h>  // IWYU pragma: keep

// `double_width_int` only emulates `int128_t` on platforms without native `__int128` (notably
// MSVC).  On every other platform `int128_t` is the builtin and dwint is unused in library code
#if !defined(__SIZEOF_INT128__)
#include <mp-units/bits/double_width_int.h>
#endif

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <algorithm>
#include <bit>
#include <concepts>
#include <cstdint>
#include <limits>
#include <tuple>
#include <type_traits>
#endif
#endif

namespace mp_units::detail {

#if defined(__SIZEOF_INT128__)

MP_UNITS_DIAGNOSTIC_PUSH
MP_UNITS_DIAGNOSTIC_IGNORE_PEDANTIC

using int128_t = __int128;
using uint128_t = unsigned __int128;

// Specialize integer_rep_width_v for __int128 types.
// std::numeric_limits<__int128> is NOT available in strict mode (-std=c++20 on GCC
// without -std=gnu++20) so we cannot rely on ::digits there.
template<>
inline constexpr std::size_t integer_rep_width_v<__int128> = 128;

template<>
inline constexpr std::size_t integer_rep_width_v<unsigned __int128> = 128;

MP_UNITS_DIAGNOSTIC_POP

inline constexpr std::size_t max_native_width = 128;

#else

using int128_t = double_width_int<std::int64_t>;
using uint128_t = double_width_int<std::uint64_t>;
constexpr std::size_t max_native_width = 64;

#endif

template<typename T>
constexpr bool is_signed_v = std::is_signed_v<T>;

#if defined(__SIZEOF_INT128__)

MP_UNITS_DIAGNOSTIC_PUSH
MP_UNITS_DIAGNOSTIC_IGNORE_PEDANTIC

// Specialize is_signed_v for __int128 types.
// std::is_signed<__int128> = false in GCC strict mode (-std=c++20) because
// std::is_signed requires std::is_arithmetic, which is not specialized for __int128
// in strict mode.  Hard-code the known signedness instead.
// These specializations must come after the primary is_signed_v template above.
template<>
inline constexpr bool is_signed_v<__int128> = true;

template<>
inline constexpr bool is_signed_v<unsigned __int128> = false;

MP_UNITS_DIAGNOSTIC_POP

#else

// Only needed when `double_width_int` doubles as `int128_t`; the dwint header was included
// for that purpose above.
template<typename T>
constexpr bool is_signed_v<double_width_int<T>> = double_width_int<T>::is_signed;

#endif

template<typename T>
using make_signed_t = conditional<!is_same_v<T, uint128_t>, std::make_signed<T>, std::type_identity<int128_t>>::type;

template<std::size_t N>
using min_width_uint_t =
  std::tuple_element_t<std::max<std::size_t>(4u, std::bit_width(N) + (std::has_single_bit(N) ? 0u : 1u)) - 4u,
                       std::tuple<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t, uint128_t>>;

template<std::size_t N>
using min_width_int_t = make_signed_t<min_width_uint_t<N>>;

template<typename T>
using double_width_int_for_t = conditional<is_signed_v<T>, min_width_int_t<integer_rep_width_v<T> * 2>,
                                           min_width_uint_t<integer_rep_width_v<T> * 2>>;

#if !defined(__SIZEOF_INT128__)
// On platforms with native `__int128` the widest type needing a double-width product is
// `int64_t`, whose double-width type is `__int128` — already handled by `double_width_int_for_t`.
// The `else` branch below is only reachable when `Lhs`/`Rhs` are themselves 128-bit, which only
// happens on MSVC where `int128_t == double_width_int<int64_t>` and `max_native_width == 64`.
template<typename Lhs, typename Rhs>
constexpr auto wide_product_of(Lhs lhs, Rhs rhs)
{
  if constexpr (integer_rep_width_v<Lhs> + integer_rep_width_v<Rhs> <= max_native_width) {
    using T = std::common_type_t<double_width_int_for_t<Lhs>, double_width_int_for_t<Rhs>>;
    return static_cast<T>(lhs) * static_cast<T>(rhs);
  } else {
    using T = double_width_int<std::common_type_t<Lhs, Rhs>>;
    return T::wide_product_of(lhs, rhs);
  }
}
#endif

// This class represents rational numbers using a fixed-point representation, with a symmetric number of digits (bits)
// on either side of the decimal point. The template argument `T` specifies the range of the integral part,
// thus this class uses twice as many bits as the provided type, but is able to precisely store exactly all integers
// from the declared type, as well as efficiently describe all rational factors that can be applied to that type
// and neither always cause underflow or overflow.
template<std::integral T>
struct fixed_point {
  using value_type = double_width_int_for_t<T>;
  static constexpr std::size_t fractional_bits = integer_rep_width_v<T>;

  constexpr fixed_point() = default;

  explicit constexpr fixed_point(value_type v) : int_repr_(v) {}

  explicit constexpr fixed_point(long double v)
  {
    long double scaled = v * int_power<long double>(2, fractional_bits);
    int_repr_ = static_cast<value_type>(scaled);
    // round away from zero; scaling will truncate towards zero, so we need to do the opposite to prevent
    // double rounding.
    if (int_repr_ >= 0) {
      if (scaled > static_cast<long double>(int_repr_)) int_repr_++;
    } else {
      if (scaled < static_cast<long double>(int_repr_)) int_repr_--;
    }
  }

  template<std::integral U>
    requires(integer_rep_width_v<U> <= integer_rep_width_v<T>)
  [[nodiscard]] constexpr auto scale(U v) const
  {
    auto res = v * int_repr_;
    return static_cast<conditional<is_signed_v<decltype((res))>, std::make_signed_t<U>, U>>(res >> fractional_bits);
  }
private:
  value_type int_repr_;
};

}  // namespace mp_units::detail
