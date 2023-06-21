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

// Formatting library for C++ - the core API for char/UTF-8
//
// Copyright (c) 2012 - present, Victor Zverovich
// All rights reserved.
//
// For the license information refer to format.h.

#include <mp-units/bits/external/hacks.h>

#ifndef MP_UNITS_USE_LIBFMT
#define MP_UNITS_USE_LIBFMT 1
#endif

#if MP_UNITS_USE_LIBFMT

MP_UNITS_DIAGNOSTIC_PUSH
MP_UNITS_DIAGNOSTIC_IGNORE_UNREACHABLE
MP_UNITS_DIAGNOSTIC_IGNORE_SHADOW
#include <fmt/format.h>
MP_UNITS_DIAGNOSTIC_POP

#define MP_UNITS_STD_FMT fmt
#define MP_UNITS_FMT_LOCALE(loc) (loc).template get<std::locale>()
#define MP_UNITS_FMT_TO_ARG_ID(arg) static_cast<int>(arg)
#define MP_UNITS_FMT_FROM_ARG_ID(arg) static_cast<size_t>(arg)

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

#ifndef __cpp_lib_format
#error "std::formatting facility not supported"
#endif

#include <format>

#define MP_UNITS_STD_FMT std
#define MP_UNITS_FMT_LOCALE(loc) loc
#define MP_UNITS_FMT_TO_ARG_ID(arg) arg
#define MP_UNITS_FMT_FROM_ARG_ID(arg) arg
#define MP_UNITS_THROW(arg) throw arg

#endif
