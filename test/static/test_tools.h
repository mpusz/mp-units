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

#include <mp-units/bits/hacks.h>
#include <mp-units/framework/quantity_spec.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cmath>
#include <limits>
#include <type_traits>
#endif

namespace mp_units {

// Floating-point comparison helper for compile-time tests
constexpr bool approx_equal(Quantity auto q1, Quantity auto q2,
                            double rel_epsilon = 1000 * std::numeric_limits<double>::epsilon())
{
  const auto diff = q1 - q2;
  const auto abs_diff = diff.numerical_value_ref_in(diff.unit) >= 0 ? diff.numerical_value_ref_in(diff.unit)
                                                                    : -diff.numerical_value_ref_in(diff.unit);
  const auto abs_q1 =
    q1.numerical_value_ref_in(q1.unit) >= 0 ? q1.numerical_value_ref_in(q1.unit) : -q1.numerical_value_ref_in(q1.unit);
  const auto abs_q2 =
    q2.numerical_value_ref_in(q2.unit) >= 0 ? q2.numerical_value_ref_in(q2.unit) : -q2.numerical_value_ref_in(q2.unit);
  const auto max_abs = abs_q1 > abs_q2 ? abs_q1 : abs_q2;
  return abs_diff <= rel_epsilon * max_abs;
}

template<auto V, typename T>
constexpr bool is_of_type = std::is_same_v<MP_UNITS_REMOVE_CONST(decltype(V)), T>;

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#if MP_UNITS_API_NO_CRTP

#define QUANTITY_SPEC_(name, ...)                                      \
  inline constexpr struct name##_ final : quantity_spec<__VA_ARGS__> { \
  } name

#else

#define QUANTITY_SPEC_(name, ...)                                               \
  inline constexpr struct name##_ final : quantity_spec<name##_, __VA_ARGS__> { \
  } name

#endif
// NOLINTEND(cppcoreguidelines-macro-usage)

}  // namespace mp_units