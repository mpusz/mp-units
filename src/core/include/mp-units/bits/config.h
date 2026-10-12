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

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#pragma once

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <version>
#endif

#if __clang__
#define MP_UNITS_COMP_CLANG __clang_major__
#if defined __apple_build_version__
// Apple Clang numbers its versions independently of LLVM Clang, so a hack keyed to an LLVM Clang
// version has to exclude it, and a hack for Apple Clang has to name Apple's version
#define MP_UNITS_COMP_APPLE_CLANG __clang_major__
#endif
#elif __GNUC__
#define MP_UNITS_COMP_GCC __GNUC__
#define MP_UNITS_COMP_GCC_MINOR __GNUC_MINOR__
#elif _MSC_VER
#define MP_UNITS_COMP_MSVC _MSC_VER
#endif

// Adapted from https://github.com/ericniebler/range-v3/blob/master/include/range/v3/detail/config.hpp#L185.
#define MP_UNITS_PRAGMA(X) _Pragma(#X)

#if !defined MP_UNITS_HOSTED && defined __STDC_HOSTED__
#define MP_UNITS_HOSTED __STDC_HOSTED__
#endif

// A hosted build can still be compiled with exceptions disabled (e.g. -fno-exceptions)
#if MP_UNITS_HOSTED && (defined __cpp_exceptions || defined __EXCEPTIONS || defined _CPPUNWIND)
#define MP_UNITS_HAS_EXCEPTIONS 1
#else
#define MP_UNITS_HAS_EXCEPTIONS 0
#endif

// MSVC does not define `__has_builtin`. `#if` expands macros before evaluating, and `defined()`
// does not stop that expansion, so `defined(__has_builtin) && __has_builtin(X)` still leaves
// `__has_builtin (X)` in the expression; every remaining identifier becomes `0`, giving `0 (0)`
// and a "unexpected tokens following preprocessor directive" diagnostic. Going through a wrapper
// keeps the call site free of the bare macro name.
#ifdef __has_builtin
#define MP_UNITS_HAS_BUILTIN(...) __has_builtin(__VA_ARGS__)
#else
#define MP_UNITS_HAS_BUILTIN(...) 0
#endif

// `= delete("reason")` where the language mode provides it. Clang 19 and later advertise
// `__cpp_deleted_function` in every mode because they accept the syntax as an extension before
// C++26, and `-Wpedantic` rejects that extension (Clang 19 warns about it even without
// `-Wpedantic`, https://github.com/llvm/llvm-project/issues/109311).
#if __cpp_deleted_function && !(MP_UNITS_COMP_CLANG && __cplusplus <= 202302L)
#define MP_UNITS_HAS_DELETED_FUNCTION_MESSAGE 1
#else
#define MP_UNITS_HAS_DELETED_FUNCTION_MESSAGE 0
#endif

#if !defined MP_UNITS_API_NO_CRTP && __cpp_explicit_this_parameter

#define MP_UNITS_API_NO_CRTP 1

#endif

#if !defined MP_UNITS_API_THROWING_CONSTRAINTS && __cpp_constexpr_exceptions >= 202411L

#define MP_UNITS_API_THROWING_CONSTRAINTS 1

#endif

// NOLINTEND(cppcoreguidelines-macro-usage)
