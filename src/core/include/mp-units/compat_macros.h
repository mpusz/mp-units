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

#include <mp-units/bits/external/hacks.h>

#ifdef MP_UNITS_API_NO_CRTP

#define QUANTITY_SPEC(name, ...)                                          \
  inline constexpr struct name : ::mp_units::quantity_spec<__VA_ARGS__> { \
  } name

#else

#define QUANTITY_SPEC(name, ...)                                                \
  inline constexpr struct name : ::mp_units::quantity_spec<name, __VA_ARGS__> { \
  } name

#endif

#if !defined MP_UNITS_API_STD_FORMAT || !MP_UNITS_API_STD_FORMAT

#if __has_include(<fmt/format.h>)
#define MP_UNITS_USE_FMTLIB 1
#endif

#endif

#if MP_UNITS_USE_FMTLIB

MP_UNITS_DIAGNOSTIC_PUSH
MP_UNITS_DIAGNOSTIC_IGNORE_UNREACHABLE
MP_UNITS_DIAGNOSTIC_IGNORE_SHADOW
#include <fmt/format.h>
MP_UNITS_DIAGNOSTIC_POP

#define MP_UNITS_STD_FMT fmt
#define MP_UNITS_FMT_LOCALE(loc) (loc).template get<std::locale>()
#define MP_UNITS_FMT_TO_ARG_ID(arg) (arg)
#define MP_UNITS_FMT_FROM_ARG_ID(arg) (arg)

// This re-uses code from fmt;
#if FMT_EXCEPTIONS
#if FMT_MSC_VERSION || defined(__NVCC__)
#define MP_UNITS_THROW(x) ::fmt::detail::do_throw(x)
#else
#define MP_UNITS_THROW(x) throw x
#endif
#else
#define MP_UNITS_THROW(x)          \
  do {                             \
    FMT_ASSERT(false, (x).what()); \
  } while (false)
#endif

#else

#if !defined __cpp_lib_format && !defined MP_UNITS_COMP_CLANG
#error "std::formatting facility not supported"
#endif

#include <format>

#define MP_UNITS_STD_FMT std
#define MP_UNITS_FMT_LOCALE(loc) loc
#define MP_UNITS_FMT_TO_ARG_ID(arg) static_cast<std::size_t>(arg)
#define MP_UNITS_FMT_FROM_ARG_ID(arg) static_cast<int>(arg)
#define MP_UNITS_THROW(arg) throw arg

#endif
