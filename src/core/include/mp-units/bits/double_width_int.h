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

#include <mp-units/bits/hacks.h>       // IWYU pragma: keep
#include <mp-units/ext/type_traits.h>  // IWYU pragma: keep

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <algorithm>
#include <bit>
#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <tuple>
#include <type_traits>
#include <version>
// <cmath> becomes partially freestanding in C++26
// before that, there is no guarantee about this header even existing
// (GCC 14 has it, but actively #error's out)
#if __STDC_HOSTED__
#include <cmath>
#endif
#endif
#endif

namespace mp_units::detail {

template<typename T>
constexpr std::size_t integer_rep_width_v = std::numeric_limits<std::make_unsigned_t<T>>::digits;

template<std::floating_point T>
[[nodiscard]] consteval T int_power(T base, int exponent)
{
#if __STDC_HOSTED__ && defined(__cpp_lib_constexpr_cmath) && __cpp_lib_constexpr_cmath >= 202202L
  return std::ldexp(base, exponent);
#else
  if (exponent < 0) {
    base = T{1} / base;
    exponent = -exponent;
  }
  T ret = 1;
  while (exponent) {
    if (exponent & 1) ret *= base;
    exponent >>= 1;
    base *= base;
  }
  return ret;

#endif
}

// A double-width integer synthesized from two base-width integers.  On platforms without a
// native `__int128` (notably MSVC), `int128_t` / `uint128_t` are aliases for this template
// instantiated on (u)int64_t; the operator set is therefore comprehensive enough for it to
// stand in for a real 128-bit integer.  See issue #795 for the original report from the
// MSVC team about missing operators.
template<std::integral T>
struct double_width_int {
  static constexpr bool is_signed = std::is_signed_v<T>;
  static constexpr std::size_t base_width = integer_rep_width_v<T>;
  static constexpr std::size_t width = 2 * base_width;

  using Th = T;
  using Tl = std::make_unsigned_t<T>;

  constexpr double_width_int() = default;

  constexpr double_width_int(Th hi, Tl lo) : hi_(hi), lo_(lo) {}

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

  // Conversion to floating-point. Reverses the long-double constructor above; needed by
  // generic numerical algorithms (e.g. constexpr_math.h root() does the initial bisection in
  // long double) so dwint can stand in for a built-in __int128 there. Negative values are
  // converted via their absolute value to avoid catastrophic cancellation on platforms
  // where long double has fewer than `width` bits of mantissa (e.g. MSVC, where long double
  // == double has only 53 bits, so `hi_ * 2^base_width + lo_` would round both terms before
  // they nearly cancel).
  explicit constexpr operator long double() const
  {
    if constexpr (is_signed) {
      if (hi_ < Th{0}) return -static_cast<long double>(-*this);
    }
    constexpr auto scale = int_power<long double>(2, base_width);
    return static_cast<long double>(hi_) * scale + static_cast<long double>(lo_);
  }
  explicit constexpr operator double() const { return static_cast<double>(static_cast<long double>(*this)); }
  explicit constexpr operator float() const { return static_cast<float>(static_cast<long double>(*this)); }

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
    Tl mid = (t01 & msk) + (static_cast<Tl>(t10) & msk) + (t00 >> half_width);
    Th o1 = t11 + static_cast<Th>(mid >> half_width) + (t10 >> half_width) + static_cast<Th>(t01 >> half_width);
    Tl o0 = (t00 & msk) | ((mid & msk) << half_width);
    return {o1, o0};
  }

  template<std::integral Rhs>
    requires(std::numeric_limits<Rhs>::digits <= base_width)
  [[nodiscard]] friend constexpr auto operator*(const double_width_int& lhs, Rhs rhs)
  {
    using RT = conditional<std::is_signed_v<Rhs>, std::make_signed_t<Tl>, Tl>;
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

  // Schoolbook multiplication truncated to `width` bits:
  //   (lhs.hi*B + lhs.lo) * (rhs.hi*B + rhs.lo) where B = 2^base_width;
  // the lhs.hi*rhs.hi*B^2 term overflows past `width` bits and is discarded.
  // The two cross-products contribute only to the hi half, so modular arithmetic in Tl is enough.
  [[nodiscard]] friend constexpr double_width_int operator*(const double_width_int& lhs, const double_width_int& rhs)
  {
    const auto lo_lo = double_width_int<Tl>::wide_product_of(lhs.lo_, rhs.lo_);
    const Tl mid = static_cast<Tl>(lhs.hi_) * rhs.lo_ + lhs.lo_ * static_cast<Tl>(rhs.hi_);
    return {static_cast<Th>(lo_lo.hi_ + mid), lo_lo.lo_};
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

  // Division between two double-width values. Falls back to the narrow operator/ when rhs fits
  // in T (with sign extension); otherwise reduces to an unsigned bit-by-bit long division.
  [[nodiscard]] friend constexpr double_width_int operator/(const double_width_int& lhs, const double_width_int& rhs)
  {
    if constexpr (is_signed) {
      // Fast path: rhs is representable in Th (positive or negative).
      if (rhs.hi_ == Th{0} && static_cast<Th>(rhs.lo_) >= Th{0}) return lhs / static_cast<Th>(rhs.lo_);
      if (rhs.hi_ == Th{-1} && static_cast<Th>(rhs.lo_) < Th{0}) return lhs / static_cast<Th>(rhs.lo_);
      // General signed path: reduce to unsigned via absolute values, then re-sign.
      using unsigned_t = double_width_int<Tl>;
      const bool neg_lhs = lhs.hi_ < 0;
      const bool neg_rhs = rhs.hi_ < 0;
      const auto abs_lhs = neg_lhs ? -lhs : lhs;
      const auto abs_rhs = neg_rhs ? -rhs : rhs;
      const auto uq = unsigned_t::from_hi_lo(static_cast<Tl>(abs_lhs.hi_), abs_lhs.lo_) /
                      unsigned_t::from_hi_lo(static_cast<Tl>(abs_rhs.hi_), abs_rhs.lo_);
      const auto q = double_width_int::from_hi_lo(static_cast<Th>(uq.hi_), uq.lo_);
      return (neg_lhs != neg_rhs) ? -q : q;
    } else {
      // Fast path: rhs is narrow.
      if (rhs.hi_ == Th{0}) return lhs / rhs.lo_;
      // Unsigned restoring long division.
      double_width_int quotient{};
      double_width_int remainder{};
      for (std::size_t i = 0; i < width; ++i) {
        const std::size_t bit_idx = width - 1 - i;
        const Tl bit =
          (bit_idx >= base_width) ? ((lhs.hi_ >> (bit_idx - base_width)) & Tl{1}) : ((lhs.lo_ >> bit_idx) & Tl{1});
        remainder = remainder << 1u;
        remainder.lo_ |= bit;
        quotient = quotient << 1u;
        if (!(remainder < rhs)) {
          remainder = remainder - rhs;
          quotient.lo_ |= Tl{1};
        }
      }
      return quotient;
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

  [[nodiscard]] friend constexpr double_width_int operator+(const double_width_int& lhs, const double_width_int& rhs)
  {
    const Tl rlo = lhs.lo_ + rhs.lo_;
    Th rhi = lhs.hi_ + rhs.hi_;
    if (rlo < lhs.lo_) ++rhi;  // carry
    return {rhi, rlo};
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

  [[nodiscard]] friend constexpr double_width_int operator-(const double_width_int& lhs, const double_width_int& rhs)
  {
    const Tl rlo = lhs.lo_ - rhs.lo_;
    Th rhi = lhs.hi_ - rhs.hi_;
    if (rlo > lhs.lo_) --rhi;  // borrow
    return {rhi, rlo};
  }

  [[nodiscard]] constexpr double_width_int operator-() const
  {
    return {(lo_ > 0 ? static_cast<Th>(-1) : Th{0}) - hi_, Tl{0} - lo_};
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

  // Bitwise operators (treat the value as a 2*base_width bit pattern).
  [[nodiscard]] constexpr double_width_int operator~() const { return {static_cast<Th>(~hi_), static_cast<Tl>(~lo_)}; }

  [[nodiscard]] friend constexpr double_width_int operator&(const double_width_int& lhs, const double_width_int& rhs)
  {
    return {static_cast<Th>(lhs.hi_ & rhs.hi_), static_cast<Tl>(lhs.lo_ & rhs.lo_)};
  }
  [[nodiscard]] friend constexpr double_width_int operator|(const double_width_int& lhs, const double_width_int& rhs)
  {
    return {static_cast<Th>(lhs.hi_ | rhs.hi_), static_cast<Tl>(lhs.lo_ | rhs.lo_)};
  }
  [[nodiscard]] friend constexpr double_width_int operator^(const double_width_int& lhs, const double_width_int& rhs)
  {
    return {static_cast<Th>(lhs.hi_ ^ rhs.hi_), static_cast<Tl>(lhs.lo_ ^ rhs.lo_)};
  }

  // Compound assignment operators, defined in terms of the binary forms above.
  constexpr double_width_int& operator+=(const double_width_int& rhs) { return *this = *this + rhs; }
  constexpr double_width_int& operator-=(const double_width_int& rhs) { return *this = *this - rhs; }
  constexpr double_width_int& operator*=(const double_width_int& rhs) { return *this = *this * rhs; }
  constexpr double_width_int& operator/=(const double_width_int& rhs) { return *this = *this / rhs; }
  constexpr double_width_int& operator&=(const double_width_int& rhs) { return *this = *this & rhs; }
  constexpr double_width_int& operator|=(const double_width_int& rhs) { return *this = *this | rhs; }
  constexpr double_width_int& operator^=(const double_width_int& rhs) { return *this = *this ^ rhs; }
  constexpr double_width_int& operator<<=(unsigned n) { return *this = *this << n; }
  constexpr double_width_int& operator>>=(unsigned n) { return *this = *this >> n; }

  // Pre/post increment and decrement.
  constexpr double_width_int& operator++()
  {
    ++lo_;
    if (lo_ == Tl{0}) ++hi_;
    return *this;
  }
  constexpr double_width_int operator++(int)
  {
    auto tmp = *this;
    ++*this;
    return tmp;
  }
  constexpr double_width_int& operator--()
  {
    if (lo_ == Tl{0}) --hi_;
    --lo_;
    return *this;
  }
  constexpr double_width_int operator--(int)
  {
    auto tmp = *this;
    --*this;
    return tmp;
  }

  static constexpr double_width_int max() { return {std::numeric_limits<Th>::max(), std::numeric_limits<Tl>::max()}; }
  static constexpr double_width_int min()
  {
    if constexpr (is_signed)
      return {std::numeric_limits<Th>::min(), 0};
    else
      return {0, 0};
  }

  Th hi_;
  Tl lo_;
};

template<typename T>
constexpr std::size_t integer_rep_width_v<double_width_int<T>> = double_width_int<T>::width;

}  // namespace mp_units::detail

namespace std {

// Specialize std::numeric_limits for the synthetic double-width integer.  Without this,
// generic algorithms that probe ::max(), ::min(), ::digits, etc. (e.g. checked_int_pow,
// magnitude-folding code in unit_magnitude.h) silently see the default-unspecialized
// limits (all zeros) and break.  On platforms with native __int128 the std-library
// specializations already exist; this specialization fills the gap on MSVC.
template<typename T>
  requires std::integral<T>
class numeric_limits<mp_units::detail::double_width_int<T>> {
  using dwi = mp_units::detail::double_width_int<T>;
public:
  static constexpr bool is_specialized = true;
  static constexpr bool is_signed = dwi::is_signed;
  static constexpr bool is_integer = true;
  static constexpr bool is_exact = true;
  static constexpr bool has_infinity = false;
  static constexpr bool has_quiet_NaN = false;
  static constexpr bool has_signaling_NaN = false;
  static constexpr float_round_style round_style = round_toward_zero;
  static constexpr bool is_iec559 = false;
  static constexpr bool is_bounded = true;
  static constexpr bool is_modulo = !dwi::is_signed;
  static constexpr int digits = static_cast<int>(dwi::width) - (dwi::is_signed ? 1 : 0);
  static constexpr int digits10 = static_cast<int>(digits * 3010 / 10000);  // floor(digits * log10(2))
  static constexpr int max_digits10 = 0;
  static constexpr int radix = 2;
  static constexpr int min_exponent = 0;
  static constexpr int min_exponent10 = 0;
  static constexpr int max_exponent = 0;
  static constexpr int max_exponent10 = 0;
  static constexpr bool traps = numeric_limits<T>::traps;
  static constexpr bool tinyness_before = false;

  static constexpr dwi min() noexcept { return dwi::min(); }
  static constexpr dwi lowest() noexcept { return dwi::min(); }
  static constexpr dwi max() noexcept { return dwi::max(); }
  static constexpr dwi epsilon() noexcept { return dwi{}; }
  static constexpr dwi round_error() noexcept { return dwi{}; }
  static constexpr dwi infinity() noexcept { return dwi{}; }
  static constexpr dwi quiet_NaN() noexcept { return dwi{}; }
  static constexpr dwi signaling_NaN() noexcept { return dwi{}; }
  static constexpr dwi denorm_min() noexcept { return dwi{}; }
};

}  // namespace std
