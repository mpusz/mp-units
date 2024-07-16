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

#define MP_UNITS_IN_GMF

#include <mp-units/bits/hacks.h>
#include <mp-units/compat_macros.h>
#include <mp-units/ext/contracts.h>

#ifndef MP_UNITS_IMPORT_STD
#include <array>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <numbers>
#include <numeric>
#include <optional>
#include <ranges>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#if __cpp_lib_text_encoding
#include <text_encoding>
#endif
#endif

#if MP_UNITS_HOSTED
#include <mp-units/ext/format.h>
#ifndef MP_UNITS_IMPORT_STD
#include <cmath>
#include <locale>
#include <ostream>
#include <random>
#include <sstream>
#include <string>
#endif
#endif

#undef MP_UNITS_IN_GMF
