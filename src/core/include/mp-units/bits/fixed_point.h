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

namespace mp_units::detail {

template<typename T>
constexpr std::size_t integer_rep_width_v = std::numeric_limits<std::make_unsigned_t<T>>::digits;


// this class synthesizes a double-width integer from two base-width integers.
template<std::integral T>
struct double_width_int {
  static constexpr bool is_signed = std::is_signed_v<T>;
  static constexpr std::size_t base_width = integer_rep_width_v<T>;
  static constexpr std::size_t width = 2 * base_width;

  using Th = T;
  using Tl = std::make_unsigned_t<T>;

  constexpr double_width_int() = default;

#if !MP_UNITS_COMP_GCC || MP_UNITS_COMP_GCC > 12
private:
#endif
  constexpr double_width_int(Th hi, Tl lo) : hi_(hi), lo_(lo) {}

  friend struct double_width_int<std::conditional_t<is_signed, std::make_unsigned_t<T>, std::make_signed_t<T>>>;

public:
  static constexpr double_width_int from_hi_lo(Th hi, Tl lo) { return {hi, lo}; }

  explicit constexpr double_width_int(long double v)
  {
    constexpr auto scale = int_power<long double>(2, base_width);
    constexpr auto iscale = 1.l / scale;
    auto scaled = v * iscale;
    hi_ = static_cast<Th>(scaled);
    auto resid = (scaled - static_cast<long double>(hi_));
    if (resid < 0) {
      --hi_;
      resid += 1;
    }
    lo_ = static_cast<Tl>(resid * scale);
  }
  template<std::integral U>
    requires(is_signed || !std::is_signed_v<U>)
  explicit(false) constexpr double_width_int(U v)
  {
    if constexpr (is_signed) {
      hi_ = v < 0 ? Th{-1} : Th{0};
    } else {
      hi_ = 0;
    }
    lo_ = static_cast<Tl>(v);
  }

  template<std::integral U>
  explicit constexpr operator U() const
  {
    if constexpr (integer_rep_width_v<U> > base_width) {
      return (static_cast<U>(hi_) << base_width) + static_cast<U>(lo_);
    } else {
      return static_cast<U>(lo_);
    }
  }

  [[nodiscard]] constexpr auto operator<=>(const double_width_int&) const = default;

  // calculates the double-width product of two base-size integers; this implementation requires at least one of them to
  // be unsigned
  static constexpr double_width_int wide_product_of(Th lhs, Tl rhs)
  {
    constexpr std::size_t half_width = base_width / 2;
    constexpr Tl msk = (Tl(1) << half_width) - 1u;
    Th l1 = lhs >> half_width;
    Tl l0 = static_cast<Tl>(lhs) & msk;
    Tl r1 = rhs >> half_width;
    Tl r0 = rhs & msk;
    Tl t00 = l0 * r0;
    Tl t01 = l0 * r1;
    Th t10 = l1 * static_cast<Th>(r0);
    Th t11 = l1 * static_cast<Th>(r1);
    Tl m = (t01 & msk) + (static_cast<Tl>(t10) & msk) + (t00 >> half_width);
    Th o1 = t11 + static_cast<Th>(m >> half_width) + (t10 >> half_width) + static_cast<Th>(t01 >> half_width);
    Tl o0 = (t00 & msk) | ((m & msk) << half_width);
    return {o1, o0};
  }

  template<std::integral Rhs>
    requires(std::numeric_limits<Rhs>::digits <= base_width)
  [[nodiscard]] friend constexpr auto operator*(const double_width_int& lhs, Rhs rhs)
  {
    using RT = std::conditional_t<std::is_signed_v<Rhs>, std::make_signed_t<Tl>, Tl>;
    auto lo_prod = double_width_int<RT>::wide_product_of(rhs, lhs.lo_);
    // Normal C++ rules; with respect to signedness, the wider type always wins.
    using ret_t = double_width_int<Th>;
    return ret_t{static_cast<Th>(lo_prod.hi_) + lhs.hi_ * static_cast<Th>(rhs), lo_prod.lo_};
  }
  template<std::integral Lhs>
    requires(std::numeric_limits<Lhs>::digits <= base_width)
  [[nodiscard]] friend constexpr auto operator*(Lhs lhs, const double_width_int& rhs)
  {
    return rhs * lhs;
  }
  template<std::integral Rhs>
    requires(std::numeric_limits<Rhs>::digits <= base_width)
  [[nodiscard]] friend constexpr double_width_int operator/(const double_width_int& lhs, Rhs rhs)
  {
    // Normal C++ rules; with respect to signedness, the bigger type always wins.
    using ret_t = double_width_int;
    if constexpr (std::is_signed_v<Rhs>) {
      if (rhs < 0) {
        return (-lhs) / static_cast<Tl>(-rhs);
      } else {
        return lhs / static_cast<Tl>(rhs);
      }
    } else if constexpr (is_signed) {
      if (lhs.hi_ < 0) {
        return -((-lhs) / rhs);
      } else {
        using unsigned_t = double_width_int<Tl>;
        auto tmp = unsigned_t{static_cast<Tl>(lhs.hi_), lhs.lo_} / rhs;
        return ret_t{static_cast<Th>(tmp.hi_), tmp.lo_};
      }
    } else {
      Th res_hi = lhs.hi_ / rhs;
      // unfortunately, wide division is hard: https://en.wikipedia.org/wiki/Division_algorithm.
      // Here, we just provide a somewhat naive implementation of long division.
      Tl rem_hi = lhs.hi_ % rhs;
      Tl rem_lo = lhs.lo_;
      Tl res_lo = 0;
      for (std::size_t i = 0; i < base_width; ++i) {
        // shift in one bit
        rem_hi = (rem_hi << 1u) | (rem_lo >> (base_width - 1));
        rem_lo <<= 1u;
        res_lo <<= 1u;
        // perform one bit of long division
        if (rem_hi >= rhs) {
          rem_hi -= rhs;
          res_lo |= 1u;
        }
      }
      return ret_t{res_hi, res_lo};
    }
  }

  template<std::integral Rhs>
    requires(std::numeric_limits<Rhs>::digits <= base_width)
  [[nodiscard]] friend constexpr double_width_int operator+(const double_width_int& lhs, Rhs rhs)
  {
    Th rhi = lhs.hi_;
    Tl rlo = lhs.lo_;
    if constexpr (std::is_signed_v<Rhs>) {
      // sign extension; no matter if lhs is signed, negative rhs sign extend
      if (rhs < 0) --rhi;
    }
    rlo += static_cast<Tl>(rhs);
    if (rlo < lhs.lo_) {
      // carry bit
      ++rhi;
    }
    return {rhi, rlo};
  }
  template<std::integral Lhs>
  [[nodiscard]] friend constexpr double_width_int operator+(Lhs lhs, const double_width_int& rhs)
  {
    return rhs + lhs;
  }
  template<std::integral Rhs>
    requires(std::numeric_limits<Rhs>::digits <= base_width)
  [[nodiscard]] friend constexpr double_width_int operator-(const double_width_int& lhs, Rhs rhs)
  {
    Th rhi = lhs.hi_;
    Tl rlo = lhs.lo_;
    if constexpr (std::is_signed_v<Rhs>) {
      // sign extension; no matter if lhs is signed, negative rhs sign extend
      if (rhs < 0) ++rhi;
    }
    rlo -= static_cast<Tl>(rhs);
    if (rlo > lhs.lo_) {
      // carry bit
      --rhi;
    }
    return {rhi, rlo};
  }

  template<std::integral Lhs>
  [[nodiscard]] friend constexpr double_width_int operator-(Lhs lhs, const double_width_int& rhs)
  {
    Th rhi = 0;
    Tl rlo = static_cast<Tl>(lhs);
    if constexpr (std::is_signed_v<Lhs>) {
      // sign extension; no matter if rhs is signed, negative lhs sign extend
      if (lhs < 0) --rhi;
    }
    rhi -= rhs.hi_;
    if (rhs.lo_ > rlo) {
      // carry bit
      --rhi;
    }
    rlo -= rhs.lo_;
    return {rhi, rlo};
  }

  [[nodiscard]] constexpr double_width_int operator-() const
  {
    return {(lo_ > 0 ? static_cast<Th>(-1) : Th{0}) - hi_, -lo_};
  }

  [[nodiscard]] constexpr double_width_int operator>>(unsigned n) const
  {
    if (n >= base_width) {
      return {static_cast<Th>(hi_ < 0 ? -1 : 0), static_cast<Tl>(hi_ >> (n - base_width))};
    }
    return {hi_ >> n, (static_cast<Tl>(hi_) << (base_width - n)) | (lo_ >> n)};
  }
  [[nodiscard]] constexpr double_width_int operator<<(unsigned n) const
  {
    if (n >= base_width) {
      return {static_cast<Th>(lo_ << (n - base_width)), 0};
    }
    return {(hi_ << n) + static_cast<Th>(lo_ >> (base_width - n)), lo_ << n};
  }

  static constexpr double_width_int max() { return {std::numeric_limits<Th>::max(), std::numeric_limits<Tl>::max()}; }

#if !MP_UNITS_COMP_GCC || MP_UNITS_COMP_GCC > 12
private:
#endif
  Th hi_;
  Tl lo_;
};

#if defined(__SIZEOF_INT128__)
MP_UNITS_DIAGNOSTIC_PUSH
MP_UNITS_DIAGNOSTIC_IGNORE_PEDANTIC
using int128_t = __int128;
using uint128_t = unsigned __int128;
MP_UNITS_DIAGNOSTIC_POP
inline constexpr std::size_t max_native_width = 128;
#else
using int128_t = double_width_int<std::int64_t>;
using uint128_t = double_width_int<std::uint64_t>;
constexpr std::size_t max_native_width = 64;
#endif

template<typename T>
constexpr std::size_t integer_rep_width_v<double_width_int<T>> = double_width_int<T>::width;

template<typename T>
constexpr bool is_signed_v = std::is_signed_v<T>;
template<typename T>
constexpr bool is_signed_v<double_width_int<T>> = double_width_int<T>::is_signed;

template<typename T>
using make_signed_t =
  std::conditional_t<!std::is_same_v<T, uint128_t>, std::make_signed<T>, std::type_identity<int128_t>>::type;

template<std::size_t N>
using min_width_uint_t =
  std::tuple_element_t<std::max<std::size_t>(4u, std::bit_width(N) + (std::has_single_bit(N) ? 0u : 1u)) - 4u,
                       std::tuple<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t, uint128_t>>;

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
    return static_cast<std::conditional_t<is_signed_v<decltype((res))>, std::make_signed_t<U>, U>>(res >>
                                                                                                   fractional_bits);
  }
private:
  value_type int_repr_;
};

}  // namespace mp_units::detail
