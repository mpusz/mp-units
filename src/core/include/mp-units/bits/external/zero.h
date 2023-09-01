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

namespace mp_units {

/**
 * @brief Universal abstraction for a value zero
 *
 * This type and value is meant to be used universally as a value zero for any numeric-like type.
 * It is really useful in comparison and sign checking, but can also be handy during initialization
 * (for example when a type does not support value initialization and does not want to zero
 * initialize its members during default construction) and assignment.
 *
 * Moreover, as its value is known at compile-time, this type may enable some optimizations in
 * numerical libraries which will result in a faster runtime code.
 */
struct zero_t {
  consteval zero_t& operator+=(zero_t) { return *this; }
  consteval zero_t& operator-=(zero_t) { return *this; }
  consteval zero_t& operator*=(zero_t) { return *this; }
  consteval zero_t& operator/=(zero_t) = delete;
  consteval zero_t& operator%=(zero_t) = delete;

  [[nodiscard]] friend consteval zero_t operator+(zero_t, zero_t) { return zero_t{}; }
  [[nodiscard]] friend consteval zero_t operator-(zero_t, zero_t) { return zero_t{}; }
  [[nodiscard]] friend consteval zero_t operator*(zero_t, zero_t) { return zero_t{}; }
  friend consteval zero_t operator/(zero_t, zero_t) = delete;
  friend consteval zero_t operator%(zero_t, zero_t) = delete;

  [[nodiscard]] consteval bool operator==(zero_t) const { return true; }
  [[nodiscard]] consteval auto operator<=>(zero_t) const { return std::partial_ordering::equivalent; }
};
inline constexpr zero_t zero;

}  // namespace mp_units
