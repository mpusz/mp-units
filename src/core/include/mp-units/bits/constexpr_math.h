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

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <optional>
#include <utility>
#endif
#endif

namespace mp_units::detail {

template<typename T>
[[nodiscard]] MP_UNITS_CONSTEVAL T abs(T v) noexcept
{
  return v < 0 ? -v : v;
}

// Raise an arbitrary arithmetic type to a positive integer power at compile time.
template<typename T>
[[nodiscard]] consteval T int_power(T base, std::integral auto exp)
{
  // As this function should only be called at compile time, the terminations herein function as
  // "parameter-compatible static_asserts", and should not result in terminations at runtime.
  if (exp < 0) {
    std::abort();  // int_power only supports positive integer powers
  }

  constexpr auto checked_multiply = [](auto a, auto b) {
    const auto result = a * b;
    MP_UNITS_DIAGNOSTIC_PUSH
    MP_UNITS_DIAGNOSTIC_IGNORE_FLOAT_EQUAL
    if (result / a != b) {
      std::abort();  // Wraparound detected
    }
    MP_UNITS_DIAGNOSTIC_POP
    return result;
  };

  constexpr auto checked_square = [checked_multiply](auto a) { return checked_multiply(a, a); };

  if (exp == 0) return T{1};
  if (exp % 2 == 1) return checked_multiply(base, int_power(base, exp - 1));
  return checked_square(int_power(base, exp / 2));
}

template<typename T>
[[nodiscard]] consteval std::optional<T> checked_int_pow(T base, std::uintmax_t exp)
{
  T result = T{1};
  while (exp > 0u) {
    if (exp % 2u == 1u) {
      if (base > std::numeric_limits<T>::max() / result) {
        return std::nullopt;
      }
      result *= base;
    }

    exp /= 2u;

    if (base > std::numeric_limits<T>::max() / base) {
      return (exp == 0u) ? std::make_optional(result) : std::nullopt;
    }
    base *= base;
  }
  return result;
}

template<typename T>
[[nodiscard]] consteval std::optional<T> root(T x, std::uintmax_t n)
{
  // The "zeroth root" would be mathematically undefined.
  if (n == 0) {
    return std::nullopt;
  }

  // The "first root" is trivial.
  if (n == 1) {
    return x;
  }

  // We only support nontrivial roots of floating point types.
  if (!std::is_floating_point<T>::value) {
    return std::nullopt;
  }

  // Handle negative numbers: only odd roots are allowed.
  if (x < 0) {
    if (n % 2 == 0) {
      return std::nullopt;
    } else {
      const auto negative_result = root(-x, n);
      if (!negative_result.has_value()) {
        return std::nullopt;
      }
      return static_cast<T>(-negative_result.value());
    }
  }

  // Handle special cases of zero and one.
  if (x == 0 || x == 1) {
    return x;
  }

  // Handle numbers bewtween 0 and 1.
  if (x < 1) {
    const auto inverse_result = root(T{1} / x, n);
    if (!inverse_result.has_value()) {
      return std::nullopt;
    }
    return static_cast<T>(T{1} / inverse_result.value());
  }

  //
  // At this point, error conditions are finished, and we can proceed with the "core" algorithm.
  //

  // Always use `long double` for intermediate computations.  We don't ever expect people to be
  // calling this at runtime, so we want maximum accuracy.
  long double xld = static_cast<long double>(x);
  long double lo = 1.0;
  long double hi = xld;

  // Do a binary search to find the closest value such that `checked_int_pow` recovers the input.
  //
  // Because we know `n > 1`, and `x > 1`, and x^n is monotonically increasing, we know that
  // `checked_int_pow(lo, n) < x < checked_int_pow(hi, n)`.  We will preserve this as an
  // invariant.
  while (lo < hi) {
    long double mid = lo + (hi - lo) / 2;

    auto result = checked_int_pow(mid, n);

    if (!result.has_value()) {
      return std::nullopt;
    }

    // Early return if we get lucky with an exact answer.
    if (result.value() == xld) {
      return static_cast<T>(mid);
    }

    // Check for stagnation.
    if (mid == lo || mid == hi) {
      break;
    }

    // Preserve the invariant that `checked_int_pow(lo, n) < x < checked_int_pow(hi, n)`.
    if (result.value() < xld) {
      lo = mid;
    } else {
      hi = mid;
    }
  }

  // Pick whichever one gets closer to the target.
  const auto lo_diff = xld - checked_int_pow(lo, n).value();
  const auto hi_diff = checked_int_pow(hi, n).value() - xld;
  return static_cast<T>(lo_diff < hi_diff ? lo : hi);
}

// A converter for the value member variable of magnitude (below).
//
// The input is the desired result, but in a (wider) intermediate type.  The point of this function
// is to cast to the desired type, but avoid overflow in doing so.
template<typename To, typename From>
  requires(!std::is_integral_v<To> || std::is_integral_v<From>)
[[nodiscard]] consteval To checked_static_cast(From x)
{
  // This function should only ever be called at compile time.  The purpose of these terminations is
  // to produce compiler errors, because we cannot `static_assert` on function arguments.
  if constexpr (std::is_integral_v<To>) {
    if (!std::in_range<To>(x)) {
      std::abort();  // Cannot represent magnitude in this type
    }
  }

  return static_cast<To>(x);
}

// The exponent of `factor` in the prime factorization of `n`.
[[nodiscard]] consteval std::intmax_t multiplicity(std::intmax_t factor, std::intmax_t n)
{
  std::intmax_t m = 0;
  while (n % factor == 0) {
    n /= factor;
    ++m;
  }
  return m;
}

// Divide a number by a given base raised to some power.
//
// Undefined unless base > 1, pow >= 0, and (base ^ pow) evenly divides n.
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
[[nodiscard]] consteval std::intmax_t remove_power(std::intmax_t base, std::intmax_t pow, std::intmax_t n)
{
  while (pow-- > 0) {
    n /= base;
  }
  return n;
}

}  // namespace mp_units::detail
