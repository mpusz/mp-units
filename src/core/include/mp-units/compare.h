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

#include <mp-units/bits/module_macros.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <compare>
#endif

MP_UNITS_EXPORT
namespace mp_units {

template<typename T>
  requires requires {
    {
      T::zero()
    } -> std::equality_comparable_with<T>;
  }
[[nodiscard]] constexpr bool is_eq_zero(T v)
{
  return v == T::zero();
}

template<typename T>
  requires requires {
    {
      T::zero()
    } -> std::equality_comparable_with<T>;
  }
[[nodiscard]] constexpr bool is_neq_zero(T v)
{
  return v != T::zero();
}

template<typename T>
  requires requires {
    {
      T::zero()
    } -> std::three_way_comparable_with<T>;
  }
[[nodiscard]] constexpr bool is_lt_zero(T v)
{
  return v < T::zero();
}

template<typename T>
  requires requires {
    {
      T::zero()
    } -> std::three_way_comparable_with<T>;
  }
[[nodiscard]] constexpr bool is_gt_zero(T v)
{
  return v > T::zero();
}

template<typename T>
  requires requires {
    {
      T::zero()
    } -> std::three_way_comparable_with<T>;
  }
[[nodiscard]] constexpr bool is_lteq_zero(T v)
{
  return v <= T::zero();
}

template<typename T>
  requires requires {
    {
      T::zero()
    } -> std::three_way_comparable_with<T>;
  }
[[nodiscard]] constexpr bool is_gteq_zero(T v)
{
  return v >= T::zero();
}

}  // namespace mp_units
