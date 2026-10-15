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

#include <mp-units/bits/compiler.h>
#include <mp-units/bits/hacks.h>

// Library feature-test macros below need <version>. In an `import std;` build it is preloaded.
#ifndef MP_UNITS_IMPORT_STD
#include <version>
#endif

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

// A capability below is set only where the library can actually use the feature: the language mode
// provides it, and no compiler bug in `hacks.h` gets in the way. Code checks the capability alone,
// so a hack cannot be forgotten at one of its sites.

// Pack indexing (`Ts...[I]`) where the language mode provides it. Clang 19 and later advertise
// `__cpp_pack_indexing` in every mode because they accept the syntax as an extension before C++26,
// and `-Wpedantic` rejects that extension.
#if __cpp_pack_indexing && !(MP_UNITS_COMP_CLANG && __cplusplus <= 202302L) && !MP_UNITS_PACK_INDEXING_HACK
#define MP_UNITS_HAS_PACK_INDEXING 1
#else
#define MP_UNITS_HAS_PACK_INDEXING 0
#endif

// The C++23 multidimensional subscript (`t[i, j]`).
#if __cpp_multidimensional_subscript && !MP_UNITS_MULTIDIM_SUBSCRIPT_HACK
#define MP_UNITS_HAS_MULTIDIM_SUBSCRIPT 1
#else
#define MP_UNITS_HAS_MULTIDIM_SUBSCRIPT 0
#endif

// `auto(expr)` and `auto{expr}` (P0849).
#if __cpp_auto_cast >= 202110L && !MP_UNITS_AUTO_CAST_HACK
#define MP_UNITS_HAS_AUTO_CAST 1
#else
#define MP_UNITS_HAS_AUTO_CAST 0
#endif

// Class template argument deduction for aggregates (P1816, P2082), which LLVM Clang implements from
// version 17. Where it is missing, aggregates provide explicit deduction guides. Apple Clang below
// 17 keeps those guides as it always had them.
#if (MP_UNITS_COMP_CLANG == 16 && !MP_UNITS_COMP_APPLE_CLANG) || \
  (MP_UNITS_COMP_APPLE_CLANG && MP_UNITS_COMP_APPLE_CLANG < 17)
#define MP_UNITS_HAS_AGGREGATE_CTAD 0
#else
#define MP_UNITS_HAS_AGGREGATE_CTAD 1
#endif

// Non-type template parameters of floating-point type, which LLVM Clang implements from version 18
// ("sorry, non-type template argument of type 'long double' is not yet supported" before that).
// Clang reports `__cpp_nontype_template_args` as 201411 even when it supports them, so the macro
// cannot be used. Apple Clang below 18 keeps the form it always had.
#if ((MP_UNITS_COMP_CLANG == 16 || MP_UNITS_COMP_CLANG == 17) && !MP_UNITS_COMP_APPLE_CLANG) || \
  (MP_UNITS_COMP_APPLE_CLANG && MP_UNITS_COMP_APPLE_CLANG < 18)
#define MP_UNITS_HAS_FLOATING_POINT_NTTP 0
#else
#define MP_UNITS_HAS_FLOATING_POINT_NTTP 1
#endif

#if !defined MP_UNITS_API_NO_CRTP && __cpp_explicit_this_parameter

#define MP_UNITS_API_NO_CRTP 1

#endif

#if !defined MP_UNITS_API_THROWING_CONSTRAINTS && __cpp_constexpr_exceptions >= 202411L

#define MP_UNITS_API_THROWING_CONSTRAINTS 1

#endif

// `std::expected`, which `try_make` returns.
#if __cpp_lib_expected >= 202202L
#define MP_UNITS_HAS_EXPECTED 1
#else
#define MP_UNITS_HAS_EXPECTED 0
#endif

// NOLINTEND(cppcoreguidelines-macro-usage)
