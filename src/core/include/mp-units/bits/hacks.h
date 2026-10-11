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

// NOLINTBEGIN(bugprone-reserved-identifier, cppcoreguidelines-macro-usage)
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

// `#warning` is C++23, and MSVC emits a hard error for it in earlier modes, so deprecated headers
// use a compiler-specific pragma instead
#if MP_UNITS_COMP_MSVC
#define MP_UNITS_DEPRECATED_HEADER(Msg) MP_UNITS_PRAGMA(message(Msg))
#else
#define MP_UNITS_DEPRECATED_HEADER(Msg) MP_UNITS_PRAGMA(GCC warning Msg)
#endif
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
#define MP_UNITS_DIAGNOSTIC_IGNORE_UNARY_MINUS_UNSIGNED
#define MP_UNITS_DIAGNOSTIC_IGNORE_UNREACHABLE
#define MP_UNITS_DIAGNOSTIC_IGNORE_ZERO_AS_NULLPOINTER_CONSTANT \
  MP_UNITS_DIAGNOSTIC_IGNORE("-Wzero-as-nullpointer-constant")
#define MP_UNITS_DIAGNOSTIC_IGNORE_DEPRECATED MP_UNITS_DIAGNOSTIC_IGNORE("-Wdeprecated-declarations")
#define MP_UNITS_DIAGNOSTIC_IGNORE_PEDANTIC MP_UNITS_DIAGNOSTIC_IGNORE("-Wpedantic")
#define MP_UNITS_DIAGNOSTIC_IGNORE_SIGN_CONVERSION MP_UNITS_DIAGNOSTIC_IGNORE("-Wsign-conversion")
#define MP_UNITS_DIAGNOSTIC_IGNORE_CONVERSION MP_UNITS_DIAGNOSTIC_IGNORE("-Wconversion")
#if MP_UNITS_COMP_CLANG
#define MP_UNITS_DIAGNOSTIC_IGNORE_FLOAT_CONVERSION MP_UNITS_DIAGNOSTIC_IGNORE("-Wimplicit-float-conversion")
#define MP_UNITS_DIAGNOSTIC_IGNORE_INT_CONVERSION MP_UNITS_DIAGNOSTIC_IGNORE("-Wimplicit-int-conversion")
#else
#define MP_UNITS_DIAGNOSTIC_IGNORE_FLOAT_CONVERSION MP_UNITS_DIAGNOSTIC_IGNORE("-Wfloat-conversion")
// GCC has no C++ counterpart of Clang's `-Wimplicit-int-conversion`: its `-Wint-conversion` is
// C-only, and `-Wconversion` already covers integer conversions
#define MP_UNITS_DIAGNOSTIC_IGNORE_INT_CONVERSION
#endif
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
#define MP_UNITS_DIAGNOSTIC_IGNORE_UNARY_MINUS_UNSIGNED MP_UNITS_DIAGNOSTIC_IGNORE(4146)
#define MP_UNITS_DIAGNOSTIC_IGNORE_UNREACHABLE MP_UNITS_DIAGNOSTIC_IGNORE(4702)
#define MP_UNITS_DIAGNOSTIC_IGNORE_ZERO_AS_NULLPOINTER_CONSTANT
#define MP_UNITS_DIAGNOSTIC_IGNORE_DEPRECATED MP_UNITS_DIAGNOSTIC_IGNORE(4996)
#define MP_UNITS_DIAGNOSTIC_IGNORE_PEDANTIC
#define MP_UNITS_DIAGNOSTIC_IGNORE_SIGN_CONVERSION
#define MP_UNITS_DIAGNOSTIC_IGNORE_CONVERSION MP_UNITS_DIAGNOSTIC_IGNORE(4242)
#define MP_UNITS_DIAGNOSTIC_IGNORE_FLOAT_CONVERSION
#define MP_UNITS_DIAGNOSTIC_IGNORE_INT_CONVERSION
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

// Hack for compiler bugs: `decltype(V)` of a class-type non-type template parameter should be
// the parameter's type, but two compilers get it wrong. GCC 12 gives `const T` (GCC bug 98820, fixed
// in GCC 13). Clang 23 gives `const T&` once a constraint passes `V` on to another concept
// (https://github.com/llvm/llvm-project/issues/230945). Wrap such a `decltype(V)` in this macro: it
// adds the trait that undoes the bug on those two compilers and leaves the type alone everywhere else.
// Newer GCC still yields `const T` for a `V` mapped from a static member such as `T::_quantity_spec_`,
// which needs no hack because the classification concepts accept a const-qualified type.
// Extend a branch to a newer version only after that version is confirmed to have the same bug.
#if MP_UNITS_COMP_CLANG == 23
#define MP_UNITS_NTTP_HACK(...) std::remove_cvref_t<__VA_ARGS__>
#elif MP_UNITS_COMP_GCC == 12
#define MP_UNITS_NTTP_HACK(...) std::remove_const_t<__VA_ARGS__>
#else
#define MP_UNITS_NTTP_HACK(...) __VA_ARGS__
#endif

#if !defined __cpp_lib_ranges_to_container

namespace std {

struct from_range_t {
  explicit from_range_t() = default;
};
inline constexpr from_range_t from_range{};

}  // namespace std

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

// Hack for a compiler bug: Clang 16 rejects some valid immediate invocations, e.g. a `consteval`
// operator called in the initializer of a static data member of a class template ("cannot take
// address of consteval function ... outside of an immediate invocation"). Clang 17 accepts them.
// Extend the branch to a newer version only after that version is confirmed to have the same bug.
#if MP_UNITS_COMP_CLANG == 16
#define MP_UNITS_CONSTEVAL_HACK constexpr
#else
#define MP_UNITS_CONSTEVAL_HACK consteval
#endif

#if !defined MP_UNITS_API_NO_CRTP && __cpp_explicit_this_parameter

#define MP_UNITS_API_NO_CRTP 1

#endif

#if !defined MP_UNITS_API_THROWING_CONSTRAINTS && __cpp_constexpr_exceptions >= 202411L

#define MP_UNITS_API_THROWING_CONSTRAINTS 1

#endif

#if defined(__clang__) && defined(__apple_build_version__) && __apple_build_version__ < 16000026
#define MP_UNITS_XCODE15_HACKS
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

// Hack for a library bug: libstdc++ 15.1 in C++23 mode derives some `std::formatter`
// specializations from `std::formatter<__format::__disabled, wchar_t>` for an incomplete
// `__disabled`. Matching that against a constrained program-defined `std::formatter` partial
// specialization checks its constraint on the incomplete type, which is a hard error for a
// concept such as `Dimension` (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=120625, fixed in
// 15.2). Wrap such a constraint in this macro: on the affected library it first checks that
// `T` is complete.
#if _GLIBCXX_RELEASE == 15 && __GLIBCXX__ < 20250808

namespace mp_units::detail {

template<typename T>
inline constexpr bool is_complete = requires { sizeof(T) > 0; };

}  // namespace mp_units::detail

#define MP_UNITS_FORMATTER_HACK(T, ...) mp_units::detail::is_complete<T>&& __VA_ARGS__

#else

#define MP_UNITS_FORMATTER_HACK(T, ...) __VA_ARGS__

#endif

// NOLINTEND(bugprone-reserved-identifier, cppcoreguidelines-macro-usage)
