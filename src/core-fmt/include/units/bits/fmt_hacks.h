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

#include <units/bits/external/hacks.h>

#ifndef UNITS_USE_LIBFMT
#define UNITS_USE_LIBFMT 1
#endif

#if UNITS_USE_LIBFMT

UNITS_DIAGNOSTIC_PUSH
UNITS_DIAGNOSTIC_IGNORE_UNREACHABLE
UNITS_DIAGNOSTIC_IGNORE_SHADOW
#include <fmt/format.h>
UNITS_DIAGNOSTIC_POP

#define STD_FMT fmt
#define FMT_LOCALE(loc) (loc).template get<std::locale>()
#define FMT_TO_ARG_ID(arg) static_cast<int>(arg)
#define FMT_FROM_ARG_ID(arg) static_cast<size_t>(arg)

#else

#ifndef __cpp_lib_format
#error "std::formatting facility not supported"
#endif

#include <format>

#define STD_FMT std
#define FMT_LOCALE(loc) loc
#define FMT_TO_ARG_ID(arg) arg
#define FMT_FROM_ARG_ID(arg) arg
#define FMT_THROW(arg) throw arg

#endif
