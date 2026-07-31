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

// IWYU pragma: always_keep
// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#pragma once

#include <mp-units/bits/hacks.h>

#if MP_UNITS_API_NO_CRTP

#define QUANTITY_SPEC(name, ...)                                          \
  inline constexpr struct name : ::mp_units::quantity_spec<__VA_ARGS__> { \
  } name

#else

#define QUANTITY_SPEC(name, ...)                                                \
  inline constexpr struct name : ::mp_units::quantity_spec<name, __VA_ARGS__> { \
  } name

#endif

#if MP_UNITS_HOSTED && (defined __cpp_exceptions || defined __EXCEPTIONS || defined _CPPUNWIND)
#define MP_UNITS_THROW(expr) throw expr
#else
#include <cstdlib>
#define MP_UNITS_THROW(expr) std::abort()
#endif

#if MP_UNITS_HOSTED

#if defined MP_UNITS_API_STD_FORMAT && !MP_UNITS_API_STD_FORMAT

#define MP_UNITS_USE_FMTLIB 1

#elif !defined MP_UNITS_API_STD_FORMAT

#if !__has_include(<format>) && __has_include(<fmt/format.h>)
#define MP_UNITS_USE_FMTLIB 1
#endif

#endif

#if MP_UNITS_USE_FMTLIB

#define MP_UNITS_STD_FMT fmt
#define MP_UNITS_FMT_LOCALE(loc) (loc).template get<std::locale>()
#define MP_UNITS_FMT_TO_ARG_ID(arg) (arg)
#define MP_UNITS_FMT_FROM_ARG_ID(arg) (arg)

#else  // MP_UNITS_USE_FMTLIB

#if !defined __cpp_lib_format && !defined MP_UNITS_COMP_CLANG
#error "std::formatting facility not supported"
#endif

#define MP_UNITS_STD_FMT std
#define MP_UNITS_FMT_LOCALE(loc) loc
#define MP_UNITS_FMT_TO_ARG_ID(arg) static_cast<std::size_t>(arg)
#define MP_UNITS_FMT_FROM_ARG_ID(arg) static_cast<int>(arg)

#endif  // MP_UNITS_USE_FMTLIB

#ifndef MP_UNITS_IN_MODULE_INTERFACE

#endif

#endif  // MP_UNITS_HOSTED

#if MP_UNITS_API_CONTRACTS == 1 || (!defined MP_UNITS_API_CONTRACTS && __cpp_contracts >= 202502L)

// `contract_assert` is used only at runtime (behind a non-constexpr helper) - GCC 16 fails to
// constant-evaluate its predicates in many valid contexts (some snapshots even in the not-taken
// branch of `if consteval`), so during constant evaluation a violation is reported by making
// the constant expression invalid with a call to a non-constexpr function instead
#define MP_UNITS_CONTRACT_ASSERT(expr)                                     \
  do {                                                                     \
    if consteval {                                                         \
      if (!(expr)) ::mp_units::detail::contract_violation();               \
    } else {                                                               \
      ::mp_units::detail::runtime_contract_check(static_cast<bool>(expr)); \
    }                                                                      \
  } while (false)

// `MP_UNITS_EXPECTS*` are GSL-backend-only - a precondition's native form is `pre()` on the
// declaration (`MP_UNITS_PRE`), and `MP_UNITS_PRE_BODY` is its in-body stand-in where a
// declaration contract cannot be used
#define MP_UNITS_EXPECTS(expr) static_cast<void>(0)
#define MP_UNITS_EXPECTS_DEBUG(expr) static_cast<void>(0)
#define MP_UNITS_ASSERT(expr) MP_UNITS_CONTRACT_ASSERT(expr)
#if defined NDEBUG
#define MP_UNITS_ASSERT_DEBUG(expr) static_cast<void>(0)
#else
#define MP_UNITS_ASSERT_DEBUG(expr) MP_UNITS_CONTRACT_ASSERT(expr)
#endif
#define MP_UNITS_PRE(...) pre(__VA_ARGS__)
#define MP_UNITS_POST(...) post(__VA_ARGS__)
#define MP_UNITS_PRE_BODY(...) MP_UNITS_CONTRACT_ASSERT(__VA_ARGS__)
#if defined NDEBUG
#define MP_UNITS_PRE_BODY_DEBUG(...) static_cast<void>(0)
#else
#define MP_UNITS_PRE_BODY_DEBUG(...) MP_UNITS_CONTRACT_ASSERT(__VA_ARGS__)
#endif

#elif MP_UNITS_API_CONTRACTS == 2 || (!defined MP_UNITS_API_CONTRACTS && __has_include(<gsl-lite/gsl-lite.hpp>))

#define MP_UNITS_EXPECTS(expr) gsl_Expects(expr)
#define MP_UNITS_EXPECTS_DEBUG(expr) gsl_ExpectsDebug(expr)
#define MP_UNITS_ASSERT(expr) gsl_Assert(expr)
#define MP_UNITS_ASSERT_DEBUG(expr) gsl_AssertDebug(expr)
#define MP_UNITS_PRE(...)
#define MP_UNITS_POST(...)
#define MP_UNITS_PRE_BODY(...) static_cast<void>(0)
#define MP_UNITS_PRE_BODY_DEBUG(...) static_cast<void>(0)

#elif MP_UNITS_API_CONTRACTS == 3 || (!defined MP_UNITS_API_CONTRACTS && __has_include(<gsl/gsl>))

#define MP_UNITS_EXPECTS(expr) Expects(expr)
#if defined NDEBUG
#define MP_UNITS_EXPECTS_DEBUG(expr) static_cast<void>(0)
#else
#define MP_UNITS_EXPECTS_DEBUG(expr) Expects(expr)
#endif
#define MP_UNITS_ASSERT(expr) Expects(expr)
#define MP_UNITS_ASSERT_DEBUG(expr) assert(expr)
#define MP_UNITS_PRE(...)
#define MP_UNITS_POST(...)
#define MP_UNITS_PRE_BODY(...) static_cast<void>(0)
#define MP_UNITS_PRE_BODY_DEBUG(...) static_cast<void>(0)

#else

#define MP_UNITS_EXPECTS(expr) static_cast<void>(0)
#define MP_UNITS_EXPECTS_DEBUG(expr) static_cast<void>(0)
#define MP_UNITS_ASSERT(expr) static_cast<void>(0)
#define MP_UNITS_ASSERT_DEBUG(expr) static_cast<void>(0)
#define MP_UNITS_PRE(...)
#define MP_UNITS_POST(...)
#define MP_UNITS_PRE_BODY(...) static_cast<void>(0)
#define MP_UNITS_PRE_BODY_DEBUG(...) static_cast<void>(0)

#endif

// backend-independent precondition checks for functions that do not (yet) declare their
// precondition with `MP_UNITS_PRE` - exactly one of the two nested macros expands to a check
#define MP_UNITS_PRECONDITION(...)  \
  do {                              \
    MP_UNITS_EXPECTS(__VA_ARGS__);  \
    MP_UNITS_PRE_BODY(__VA_ARGS__); \
  } while (false)
#define MP_UNITS_PRECONDITION_DEBUG(...)  \
  do {                                    \
    MP_UNITS_EXPECTS_DEBUG(__VA_ARGS__);  \
    MP_UNITS_PRE_BODY_DEBUG(__VA_ARGS__); \
  } while (false)
// NOLINTEND(cppcoreguidelines-macro-usage)
