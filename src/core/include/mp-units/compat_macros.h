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

// A contract belongs on the declaration. `MP_UNITS_PRE` and `MP_UNITS_POST` are the primary
// spelling and everything below them exists only to give the other backends, and a compiler whose
// declaration contracts do not work, the same check somewhere else.
#define MP_UNITS_PRE(...) pre(__VA_ARGS__)

// `MP_UNITS_PRE_DEFERRED` is for a position GCC 16 cannot compile a `pre` in. There are four, each
// reduced to a standalone reproducer and none of them matching an open GCC report:
//
//   1. a hidden friend defined inside a class template   -> "contract condition is not constant"
//   2. a function template with a deduced return type    -> ICE in extract_insn, at recog.cc:2892
//   3. a constructor reached by compile-time construction of a `symbol_text`  -> "not constant"
//   4. a function reached during constant evaluation from `div_round`         -> "not constant"
//
// The contract is still written on the declaration and merely disabled there, rather than moved
// into the body. That matters for more than tidiness: a predicate that only ever appears in a body
// stops being held to what a declaration requires - it may reach for locals, or for a member before
// the member initializers have run, or be well-formed for only some of the representations the
// template accepts - and then it can never move. Writing it in its proper place keeps the interface
// honest and lets a fixed compiler pick it up with no edit. `MP_UNITS_PRE_DEFERRED_COMPAT` in the
// body does the checking meanwhile.
#if MP_UNITS_COMP_GCC && MP_UNITS_COMP_GCC <= 16
#define MP_UNITS_PRE_DEFERRED(...)
#define MP_UNITS_PRE_DECL_COMPLETE 0
#else
#define MP_UNITS_PRE_DEFERRED(...) pre(__VA_ARGS__)
#define MP_UNITS_PRE_DECL_COMPLETE 1
#endif

// GCC rejects a `post` predicate that reads the result binding when the call initializes a
// `constexpr` variable: `constexpr int f(int x) post(r: r >= 0) { return x; }` makes
// `constexpr int v = f(3);` fail with "contract condition is not constant", while
// `static_assert(f(3) == 3);` on the same function compiles, as do `pre(x >= 0)` and
// `post(r: true)`. That `static_assert` passes is what makes it easy to miss in a probe.
// GCC PR 125587, open since 2026-06-03. A second, distinct ICE (`in tsubst, at cp/pt.cc:17105`)
// sits behind it, so this is not one bug away from working.
//
// Declarations carry `MP_UNITS_POST` regardless, so a fixed compiler picks it up on its own, and
// `MP_UNITS_POST_COMPAT` in the body does the checking meanwhile. The bound is a `<=` rather than a
// blanket GCC check on purpose: GCC 17 will attempt the real thing, and if the fix has not landed
// the build says so instead of staying silent because nobody edited this line.
#if MP_UNITS_COMP_GCC && MP_UNITS_COMP_GCC <= 16
#define MP_UNITS_POST(...)
#define MP_UNITS_POST_DECL_WORKS 0
#else
#define MP_UNITS_POST(...) post(__VA_ARGS__)
#define MP_UNITS_POST_DECL_WORKS 1
#endif

// The GSL vocabulary: in-body, and empty here because the declaration above already checks.
#define MP_UNITS_EXPECTS(expr) static_cast<void>(0)
#define MP_UNITS_EXPECTS_DEBUG(expr) static_cast<void>(0)
#define MP_UNITS_ENSURES(expr) static_cast<void>(0)
#define MP_UNITS_ENSURES_DEBUG(expr) static_cast<void>(0)

#define MP_UNITS_ASSERT(expr) MP_UNITS_CONTRACT_ASSERT(expr)
#if defined NDEBUG
#define MP_UNITS_ASSERT_DEBUG(expr) static_cast<void>(0)
#else
#define MP_UNITS_ASSERT_DEBUG(expr) MP_UNITS_CONTRACT_ASSERT(expr)
#endif

// In-body stand-ins, for the two cases where a declaration contract cannot be used at all: a
// predicate that is only well-formed for some of the representations a template accepts (a
// declaration contract cannot be wrapped in `if constexpr`), and a check that must not run in a
// release build (C++26 chooses its evaluation semantic per translation unit, so a contract cannot
// say "debug only" - that is what the `_DEBUG` spellings are for).
#define MP_UNITS_PRE_BODY(...) MP_UNITS_CONTRACT_ASSERT(__VA_ARGS__)
#define MP_UNITS_POST_BODY(...) MP_UNITS_CONTRACT_ASSERT(__VA_ARGS__)
#if defined NDEBUG
#define MP_UNITS_PRE_BODY_DEBUG(...) static_cast<void>(0)
#define MP_UNITS_POST_BODY_DEBUG(...) static_cast<void>(0)
#else
#define MP_UNITS_PRE_BODY_DEBUG(...) MP_UNITS_CONTRACT_ASSERT(__VA_ARGS__)
#define MP_UNITS_POST_BODY_DEBUG(...) MP_UNITS_CONTRACT_ASSERT(__VA_ARGS__)
#endif

// In-body checks. Each expands to exactly one statement, so none of them needs a
// `do`/`while (false)` wrapper to survive `if (c) M(x); else ...`. On this backend only the native
// spelling is ever real.
#define MP_UNITS_PRECONDITION(...) MP_UNITS_PRE_BODY(__VA_ARGS__)
#define MP_UNITS_PRECONDITION_DEBUG(...) MP_UNITS_PRE_BODY_DEBUG(__VA_ARGS__)
#define MP_UNITS_POSTCONDITION(...) MP_UNITS_POST_BODY(__VA_ARGS__)
#define MP_UNITS_POSTCONDITION_DEBUG(...) MP_UNITS_POST_BODY_DEBUG(__VA_ARGS__)
#if MP_UNITS_PRE_DECL_COMPLETE
#define MP_UNITS_PRE_DEFERRED_COMPAT(...) static_cast<void>(0)
#define MP_UNITS_PRE_DEFERRED_COMPAT_DEBUG(...) static_cast<void>(0)
#else
#define MP_UNITS_PRE_DEFERRED_COMPAT(...) MP_UNITS_PRE_BODY(__VA_ARGS__)
#define MP_UNITS_PRE_DEFERRED_COMPAT_DEBUG(...) MP_UNITS_PRE_BODY_DEBUG(__VA_ARGS__)
#endif
#if MP_UNITS_POST_DECL_WORKS
#define MP_UNITS_POST_COMPAT(...) static_cast<void>(0)
#define MP_UNITS_POST_COMPAT_DEBUG(...) static_cast<void>(0)
#else
#define MP_UNITS_POST_COMPAT(...) MP_UNITS_POST_BODY(__VA_ARGS__)
#define MP_UNITS_POST_COMPAT_DEBUG(...) MP_UNITS_POST_BODY_DEBUG(__VA_ARGS__)
#endif


#elif MP_UNITS_API_CONTRACTS == 2 || (!defined MP_UNITS_API_CONTRACTS && __has_include(<gsl-lite/gsl-lite.hpp>))

// No declaration contracts here, so the declaration macros are empty and the GSL vocabulary in the
// body does all of the work.
#define MP_UNITS_PRE(...)
#define MP_UNITS_PRE_DEFERRED(...)
#define MP_UNITS_PRE_DECL_COMPLETE 0
#define MP_UNITS_POST(...)
#define MP_UNITS_POST_DECL_WORKS 0
#define MP_UNITS_EXPECTS(expr) gsl_Expects(expr)
#define MP_UNITS_EXPECTS_DEBUG(expr) gsl_ExpectsDebug(expr)
#define MP_UNITS_ENSURES(expr) gsl_Ensures(expr)
#define MP_UNITS_ENSURES_DEBUG(expr) gsl_EnsuresDebug(expr)
#define MP_UNITS_ASSERT(expr) gsl_Assert(expr)
#define MP_UNITS_ASSERT_DEBUG(expr) gsl_AssertDebug(expr)
#define MP_UNITS_PRE_BODY(...) static_cast<void>(0)
#define MP_UNITS_POST_BODY(...) static_cast<void>(0)
#define MP_UNITS_PRE_BODY_DEBUG(...) static_cast<void>(0)
#define MP_UNITS_POST_BODY_DEBUG(...) static_cast<void>(0)

// In-body checks; on a GSL backend the GSL spelling is the only real one.
#define MP_UNITS_PRECONDITION(...) MP_UNITS_EXPECTS(__VA_ARGS__)
#define MP_UNITS_PRECONDITION_DEBUG(...) MP_UNITS_EXPECTS_DEBUG(__VA_ARGS__)
#define MP_UNITS_POSTCONDITION(...) MP_UNITS_ENSURES(__VA_ARGS__)
#define MP_UNITS_POSTCONDITION_DEBUG(...) MP_UNITS_ENSURES_DEBUG(__VA_ARGS__)
#define MP_UNITS_PRE_DEFERRED_COMPAT(...) MP_UNITS_EXPECTS(__VA_ARGS__)
#define MP_UNITS_PRE_DEFERRED_COMPAT_DEBUG(...) MP_UNITS_EXPECTS_DEBUG(__VA_ARGS__)
#define MP_UNITS_POST_COMPAT(...) MP_UNITS_ENSURES(__VA_ARGS__)
#define MP_UNITS_POST_COMPAT_DEBUG(...) MP_UNITS_ENSURES_DEBUG(__VA_ARGS__)


#elif MP_UNITS_API_CONTRACTS == 3 || (!defined MP_UNITS_API_CONTRACTS && __has_include(<gsl/gsl>))

// No declaration contracts here either; see the gsl-lite block above.
#define MP_UNITS_PRE(...)
#define MP_UNITS_PRE_DEFERRED(...)
#define MP_UNITS_PRE_DECL_COMPLETE 0
#define MP_UNITS_POST(...)
#define MP_UNITS_POST_DECL_WORKS 0
#define MP_UNITS_EXPECTS(expr) Expects(expr)
#define MP_UNITS_ENSURES(expr) Ensures(expr)
#if defined NDEBUG
#define MP_UNITS_EXPECTS_DEBUG(expr) static_cast<void>(0)
#define MP_UNITS_ENSURES_DEBUG(expr) static_cast<void>(0)
#else
#define MP_UNITS_EXPECTS_DEBUG(expr) Expects(expr)
#define MP_UNITS_ENSURES_DEBUG(expr) Ensures(expr)
#endif
#define MP_UNITS_ASSERT(expr) Expects(expr)
#define MP_UNITS_ASSERT_DEBUG(expr) assert(expr)
#define MP_UNITS_PRE_BODY(...) static_cast<void>(0)
#define MP_UNITS_POST_BODY(...) static_cast<void>(0)
#define MP_UNITS_PRE_BODY_DEBUG(...) static_cast<void>(0)
#define MP_UNITS_POST_BODY_DEBUG(...) static_cast<void>(0)

// In-body checks; on a GSL backend the GSL spelling is the only real one.
#define MP_UNITS_PRECONDITION(...) MP_UNITS_EXPECTS(__VA_ARGS__)
#define MP_UNITS_PRECONDITION_DEBUG(...) MP_UNITS_EXPECTS_DEBUG(__VA_ARGS__)
#define MP_UNITS_POSTCONDITION(...) MP_UNITS_ENSURES(__VA_ARGS__)
#define MP_UNITS_POSTCONDITION_DEBUG(...) MP_UNITS_ENSURES_DEBUG(__VA_ARGS__)
#define MP_UNITS_PRE_DEFERRED_COMPAT(...) MP_UNITS_EXPECTS(__VA_ARGS__)
#define MP_UNITS_PRE_DEFERRED_COMPAT_DEBUG(...) MP_UNITS_EXPECTS_DEBUG(__VA_ARGS__)
#define MP_UNITS_POST_COMPAT(...) MP_UNITS_ENSURES(__VA_ARGS__)
#define MP_UNITS_POST_COMPAT_DEBUG(...) MP_UNITS_ENSURES_DEBUG(__VA_ARGS__)


#else

// Contracts disabled: every spelling is inert.
#define MP_UNITS_PRE(...)
#define MP_UNITS_PRE_DEFERRED(...)
#define MP_UNITS_PRE_DECL_COMPLETE 0
#define MP_UNITS_POST(...)
#define MP_UNITS_POST_DECL_WORKS 0
#define MP_UNITS_EXPECTS(expr) static_cast<void>(0)
#define MP_UNITS_EXPECTS_DEBUG(expr) static_cast<void>(0)
#define MP_UNITS_ENSURES(expr) static_cast<void>(0)
#define MP_UNITS_ENSURES_DEBUG(expr) static_cast<void>(0)
#define MP_UNITS_ASSERT(expr) static_cast<void>(0)
#define MP_UNITS_ASSERT_DEBUG(expr) static_cast<void>(0)
#define MP_UNITS_PRE_BODY(...) static_cast<void>(0)
#define MP_UNITS_POST_BODY(...) static_cast<void>(0)
#define MP_UNITS_PRE_BODY_DEBUG(...) static_cast<void>(0)
#define MP_UNITS_POST_BODY_DEBUG(...) static_cast<void>(0)

// Contracts disabled: every in-body spelling is inert too.
#define MP_UNITS_PRECONDITION(...) static_cast<void>(0)
#define MP_UNITS_PRECONDITION_DEBUG(...) static_cast<void>(0)
#define MP_UNITS_POSTCONDITION(...) static_cast<void>(0)
#define MP_UNITS_POSTCONDITION_DEBUG(...) static_cast<void>(0)
#define MP_UNITS_PRE_DEFERRED_COMPAT(...) static_cast<void>(0)
#define MP_UNITS_PRE_DEFERRED_COMPAT_DEBUG(...) static_cast<void>(0)
#define MP_UNITS_POST_COMPAT(...) static_cast<void>(0)
#define MP_UNITS_POST_COMPAT_DEBUG(...) static_cast<void>(0)

#endif

// How the spellings pair up.
//
// A function whose declaration carries `MP_UNITS_PRE` writes `MP_UNITS_EXPECTS` in its body with
// the same predicate: that is the GSL vocabulary, and it is empty under the native backend, where
// the declaration already did the job. One of the two checks, never both.
//
// `MP_UNITS_PRE_DEFERRED_COMPAT` and `MP_UNITS_POST_COMPAT` are the same idea for a contract whose
// declaration form is disabled on this compiler: they additionally check in the body there, and go
// quiet as soon as the declaration works.
//
// `MP_UNITS_PRECONDITION` / `MP_UNITS_POSTCONDITION` are the last resort, for a predicate that
// cannot be put on a declaration at all - one that is well-formed for only some of the accepted
// representations, since a declaration contract cannot be wrapped in `if constexpr`. A use should
// say in a comment why the declaration was impossible.
//
// All of these are defined per backend above so that each expands to a single statement. That is
// why none of them carries a `do`/`while (false)`: the only wrapper left in this file is inside
// `MP_UNITS_CONTRACT_ASSERT`, which is an `if consteval` / `else` and cannot be anything but two
// statements.
// NOLINTEND(cppcoreguidelines-macro-usage)
