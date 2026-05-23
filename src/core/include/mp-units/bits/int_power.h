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
#include <cstddef>
#include <limits>
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

// Representation width (in bits) of an integral type, as the digit count of its unsigned
// counterpart.  Lives in its own header so that both `double_width_int.h` and
// `fixed_point.h` can use it without one having to include the other.
template<typename T>
constexpr std::size_t integer_rep_width_v = std::numeric_limits<std::make_unsigned_t<T>>::digits;

// `2^exponent` (or more precisely `base * 2^exponent`) as a compile-time floating-point
// constant.  Used by `double_width_int`'s long-double conversions and by `fixed_point`'s
// long-double constructor.
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

}  // namespace mp_units::detail
