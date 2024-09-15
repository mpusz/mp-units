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

// IWYU pragma: private, include <mp-units/framework.h>
#include <mp-units/framework/magnitude.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <bit>
#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <numbers>
#endif
#endif

namespace mp_units {
namespace detail {

// this class synthesizes a double-width integer from two base-width integers.
template<std::integral T>
struct double_width_int {
  static constexpr bool is_signed = std::is_signed_v<T>;
  static constexpr std::size_t base_width = std::numeric_limits<std::make_unsigned_t<T>>::digits;
  static constexpr std::size_t width = 2 * base_width;

  using Th = T;
  using Tl = std::make_unsigned_t<T>;

  constexpr double_width_int() = default;
  constexpr double_width_int(const double_width_int&) = default;

  constexpr double_width_int& operator=(const double_width_int&) = default;

  constexpr double_width_int(Th hi, Tl lo) : hi(hi), lo(lo) {}

  explicit(true) constexpr double_width_int(long double v)
  {
    constexpr auto scale = int_power<long double>(2, base_width);
    constexpr auto iscale = 1.l / scale;
    hi = static_cast<Th>(v * iscale);
    lo = static_cast<Tl>(v - (hi * scale));
  }
  template<std::integral U>
    requires(is_signed || !std::is_signed_v<U>)
  explicit(false) constexpr double_width_int(U v)
  {
    if constexpr (is_signed) {
      hi = v < 0 ? Th{-1} : Th{0};
    } else {
      hi = 0;
    }
    lo = static_cast<Tl>(v);
  }

  explicit(true) constexpr operator Th() const { return static_cast<Th>(lo); }

  constexpr auto operator<=>(const double_width_int&) const = default;

  // calculates the double-width product of two base-size integers
  static constexpr double_width_int wide_product_of(Th lhs, Tl rhs)
  {
    static constexpr std::size_t half_width = base_width / 2;
    static constexpr Tl msk = Tl(1) << half_width;
    Th l1 = lhs >> half_width;
    Tl l0 = static_cast<Tl>(lhs) & msk;
    Tl r1 = rhs >> half_width;
    Tl r0 = rhs & msk;
    Tl t00 = l0 * r0;
    Tl t01 = l0 * r1;
    Th t10 = l1 * r0;
    Th t11 = l1 * r1;
    Tl m = (t01 & msk) + (static_cast<Tl>(t10) & msk) + (t00 >> half_width);
    Th o1 = t11 + (m >> half_width) + (t10 >> half_width) + static_cast<Th>(t01 >> half_width);
    Tl o0 = (t00 & msk) | ((m & msk) << half_width);
    return {o1, o0};
  }

  template<std::integral Lhs>
  friend constexpr auto operator*(Lhs lhs, const double_width_int& rhs)
  {
    using ret_t = double_width_int<std::common_type_t<Lhs, Th>>;
    auto ret = ret_t::wide_product_of(lhs, rhs.lo);
    ret.hi += lhs * rhs.hi;
    return ret;
  };


  constexpr double_width_int operator+(Tl rhs) const
  {
    auto ret = lo + rhs;
    return {hi + (ret < lo ? 1 : 0), ret};
  }

  constexpr double_width_int operator>>(unsigned n) const
  {
    if (n >= base_width) {
      return {0, hi >> (n - base_width)};
    }
    return {hi >> n, (static_cast<Tl>(hi) << (base_width - n)) | (lo >> n)};
  }
  constexpr double_width_int operator<<(unsigned n) const
  {
    if (n >= base_width) {
      return {static_cast<Th>(lo >> (2 * base_width - n)), 0};
    }
    return {(hi << n) + static_cast<Th>(lo >> (base_width - n)), lo << n};
  }

  static constexpr double_width_int max() { return {std::numeric_limits<Th>::max(), std::numeric_limits<Tl>::max()}; }

  Th hi;
  Tl lo;
};

#ifdef __SIZEOF_INT128__
using int128_t = __int128;
using uint128_t = unsigned __int128;
inline constexpr std::size_t max_native_width = 128;
#else
using int128_t = double_width_int<std::int64_t>;
using uint128_t = double_width_int<std::uint64_t>;
inline constexpr std::size_t max_native_width = 64;
#endif

template<typename T>
inline constexpr std::size_t integer_rep_width_v = std::numeric_limits<std::make_unsigned_t<T>>::digits;
template<typename T>
inline constexpr std::size_t integer_rep_width_v<double_width_int<T>> = double_width_int<T>::width;

template<typename T>
inline constexpr bool is_signed_v = std::is_signed_v<T>;
template<typename T>
inline constexpr bool is_signed_v<double_width_int<T>> = double_width_int<T>::is_signed;

template<typename T>
using make_signed_t = std::make_signed_t<T>;

template<std::size_t N>
using min_width_uint_t =
  std::tuple_element_t<std::bit_width(N) - 4 + (N > (1u << (std::bit_width(N) - 1)) ? 1 : 0),
                       std::tuple<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t, uint128_t>>;

static_assert(std::is_same_v<min_width_uint_t<31>, std::uint32_t>);
static_assert(std::is_same_v<min_width_uint_t<32>, std::uint32_t>);
static_assert(std::is_same_v<min_width_uint_t<33>, std::uint64_t>);

template<std::size_t N>
using min_width_int_t = make_signed_t<min_width_uint_t<N>>;

template<typename T>
using double_width_int_for_t = std::conditional_t<is_signed_v<T>, min_width_int_t<integer_rep_width_v<T> * 2>,
                                                  min_width_uint_t<integer_rep_width_v<T> * 2>>;

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

// This class represents rational numbers using a fixed-point representation, with a symmetric number of digits (bits)
// on either side of the decimal point. The template argument `T` specifies the range of the integral part,
// thus this class uses twice as many bits as the provided type, but is able to precisely store exactly all integers
// from the declared type, as well as efficiently describe all rational factors that can be applied to that type
// and neither always cause underflow or overflow.
template<std::integral T>
struct fixed_point {
  using repr_t = double_width_int_for_t<T>;
  static constexpr std::size_t fractional_bits = integer_rep_width_v<T>;

  constexpr fixed_point() = default;
  constexpr fixed_point(const fixed_point&) = default;

  constexpr fixed_point& operator=(const fixed_point&) = default;

  explicit constexpr fixed_point(long double v) :
      int_repr_is_an_implementation_detail_(static_cast<repr_t>(v * int_power<long double>(2, fractional_bits)))
  {
  }

  template<std::integral U>
    requires(integer_rep_width_v<U> <= integer_rep_width_v<T>)
  auto scale(U v) const
  {
    auto res = v * int_repr_is_an_implementation_detail_;
    return static_cast<std::conditional_t<is_signed_v<decltype((res))>, std::make_signed_t<U>, U>>(res >>
                                                                                                   fractional_bits);
  }

  repr_t int_repr_is_an_implementation_detail_;
};

}  // namespace detail
}  // namespace mp_units
