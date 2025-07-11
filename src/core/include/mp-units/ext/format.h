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

#include <mp-units/bits/requires_hosted.h>
//
#include <mp-units/bits/hacks.h>
#include <mp-units/compat_macros.h>

#if MP_UNITS_USE_FMTLIB
MP_UNITS_DIAGNOSTIC_PUSH
MP_UNITS_DIAGNOSTIC_IGNORE_UNREACHABLE
MP_UNITS_DIAGNOSTIC_IGNORE_SHADOW
#include <fmt/compile.h>
#include <fmt/format.h>
MP_UNITS_DIAGNOSTIC_POP
#else  // MP_UNITS_USE_FMTLIB
#ifdef MP_UNITS_IMPORT_STD
#ifndef MP_UNITS_IN_GMF
import std;
#endif
#else  // MP_UNITS_IMPORT_STD
#include <format>
#endif  // MP_UNITS_IMPORT_STD
#endif  // MP_UNITS_USE_FMTLIB

#endif
