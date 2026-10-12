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

// NOLINTEND(cppcoreguidelines-macro-usage)
