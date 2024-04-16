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

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <version>
#endif

#if __clang__
#define MP_UNITS_COMP_CLANG __clang_major__
#elif __GNUC__
#define MP_UNITS_COMP_GCC __GNUC__
#define MP_UNITS_COMP_GCC_MINOR __GNUC_MINOR__
#elif _MSC_VER
#define MP_UNITS_COMP_MSVC _MSC_VER
#endif

// Adapted from https://github.com/ericniebler/range-v3/blob/master/include/range/v3/detail/config.hpp#L185.
#define MP_UNITS_PRAGMA(X) _Pragma(#X)
#if !MP_UNITS_COMP_MSVC
#define MP_UNITS_DIAGNOSTIC_PUSH MP_UNITS_PRAGMA(GCC diagnostic push)
#define MP_UNITS_DIAGNOSTIC_POP MP_UNITS_PRAGMA(GCC diagnostic pop)
#define MP_UNITS_DIAGNOSTIC_IGNORE_PRAGMAS MP_UNITS_PRAGMA(GCC diagnostic ignored "-Wpragmas")
#define MP_UNITS_DIAGNOSTIC_IGNORE(X)                                \
  MP_UNITS_DIAGNOSTIC_IGNORE_PRAGMAS                                 \
  MP_UNITS_PRAGMA(GCC diagnostic ignored "-Wunknown-pragmas")        \
  MP_UNITS_PRAGMA(GCC diagnostic ignored "-Wunknown-warning-option") \
  MP_UNITS_PRAGMA(GCC diagnostic ignored X)
#define MP_UNITS_DIAGNOSTIC_IGNORE_EXPR_ALWAYS_TF
#define MP_UNITS_DIAGNOSTIC_IGNORE_FLOAT_EQUAL MP_UNITS_DIAGNOSTIC_IGNORE("-Wfloat-equal")
#define MP_UNITS_DIAGNOSTIC_IGNORE_LOSS_OF_DATA
#define MP_UNITS_DIAGNOSTIC_IGNORE_MISSING_BRACES MP_UNITS_DIAGNOSTIC_IGNORE("-Wmissing-braces")
#define MP_UNITS_DIAGNOSTIC_IGNORE_NON_TEMPLATE_FRIEND MP_UNITS_DIAGNOSTIC_IGNORE("-Wnon-template-friend")
#define MP_UNITS_DIAGNOSTIC_IGNORE_SHADOW MP_UNITS_DIAGNOSTIC_IGNORE("-Wshadow")
#define MP_UNITS_DIAGNOSTIC_IGNORE_UNREACHABLE
#define MP_UNITS_DIAGNOSTIC_IGNORE_ZERO_AS_NULLPOINTER_CONSTANT \
  MP_UNITS_DIAGNOSTIC_IGNORE("-Wzero-as-nullpointer-constant")
#define MP_UNITS_DIAGNOSTIC_IGNORE_DEPRECATED MP_UNITS_DIAGNOSTIC_IGNORE("-Wdeprecated-declarations")
#else
#define MP_UNITS_DIAGNOSTIC_PUSH MP_UNITS_PRAGMA(warning(push))
#define MP_UNITS_DIAGNOSTIC_POP MP_UNITS_PRAGMA(warning(pop))
#define MP_UNITS_DIAGNOSTIC_IGNORE_PRAGMAS MP_UNITS_PRAGMA(warning(disable : 4068))
#define MP_UNITS_DIAGNOSTIC_IGNORE(X) MP_UNITS_DIAGNOSTIC_IGNORE_PRAGMAS MP_UNITS_PRAGMA(warning(disable : X))
#define MP_UNITS_DIAGNOSTIC_IGNORE_EXPR_ALWAYS_TF MP_UNITS_DIAGNOSTIC_IGNORE(4296)
#define MP_UNITS_DIAGNOSTIC_IGNORE_FLOAT_EQUAL
#define MP_UNITS_DIAGNOSTIC_IGNORE_LOSS_OF_DATA MP_UNITS_DIAGNOSTIC_IGNORE(4244)
#define MP_UNITS_DIAGNOSTIC_IGNORE_MISSING_BRACES
#define MP_UNITS_DIAGNOSTIC_IGNORE_NON_TEMPLATE_FRIEND
#define MP_UNITS_DIAGNOSTIC_IGNORE_SHADOW MP_UNITS_DIAGNOSTIC_IGNORE(4459)
#define MP_UNITS_DIAGNOSTIC_IGNORE_UNREACHABLE MP_UNITS_DIAGNOSTIC_IGNORE(4702)
#define MP_UNITS_DIAGNOSTIC_IGNORE_ZERO_AS_NULLPOINTER_CONSTANT
#define MP_UNITS_DIAGNOSTIC_IGNORE_DEPRECATED
#endif

#if MP_UNITS_COMP_MSVC

#define MP_UNITS_TYPENAME typename

#else

#define MP_UNITS_TYPENAME

#endif

// TODO revise the below when clang-18 is released
#if MP_UNITS_COMP_CLANG >= 18 && __cplusplus >= 202300L && !defined __cpp_explicit_this_parameter

#define __cpp_explicit_this_parameter 202110L

#endif

#if defined MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17

#define MP_UNITS_CONSTEVAL constexpr

#else

#define MP_UNITS_CONSTEVAL consteval

#endif

#if MP_UNITS_COMP_MSVC

#define MP_UNITS_CONSTRAINED_AUTO_WORKAROUND(X)
#define MP_UNITS_CONSTRAINED_NTTP_WORKAROUND(X)

#else

#define MP_UNITS_CONSTRAINED_AUTO_WORKAROUND(X) X
#define MP_UNITS_CONSTRAINED_NTTP_WORKAROUND(X) X

#endif

#if !defined MP_UNITS_API_STRING_VIEW_RET && __cpp_constexpr >= 202211L

#define MP_UNITS_API_STRING_VIEW_RET 1

#endif

#if !defined MP_UNITS_API_NO_CRTP && __cpp_explicit_this_parameter

#define MP_UNITS_API_NO_CRTP 1

#endif
