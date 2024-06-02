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
#include <type_traits>

template<auto V, typename T>
inline constexpr bool is_of_type = std::is_same_v<MP_UNITS_REMOVE_CONST(decltype(V)), T>;

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#ifdef MP_UNITS_API_NO_CRTP

#define QUANTITY_SPEC_(name, ...)                                \
  inline constexpr struct name##_ : quantity_spec<__VA_ARGS__> { \
  } name

#else

#define QUANTITY_SPEC_(name, ...)                                         \
  inline constexpr struct name##_ : quantity_spec<name##_, __VA_ARGS__> { \
  } name

#endif
// NOLINTEND(cppcoreguidelines-macro-usage)
