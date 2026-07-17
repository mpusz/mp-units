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

// This header is force-included (via `-include`) at the very top of every translation unit that
// consumes mp-units on GCC header-mode `import std;` builds. It works around a libstdc++ bug: an
// mp-units header textually includes `<version>` (for library feature-test macros, which
// `import std;` does not provide), and GCC rejects that include as a redefinition when it appears
// *after* the consumer's `import std;`. Forcing `<version>` (and thus `<bits/c++config.h>`) to the
// top of the TU guarantees the libstdc++ config headers precede `import std;`.
//
// The include is gated on `MP_UNITS_IMPORT_STD` on purpose. CMake compiles the standard library
// module (`std.cc`) with the consumer target's `COMPILE_OPTIONS` -- including this forced include --
// but inherits `COMPILE_DEFINITIONS` from the module provider, so `MP_UNITS_IMPORT_STD` is *not*
// defined there (see cmake-cxxmodules(7), "synthetic target"). The gate therefore makes this header
// expand to nothing when `std.cc` is compiled, leaving its leading `module;` declaration intact,
// while still pulling in `<version>` for genuine consumer translation units.
#ifdef MP_UNITS_IMPORT_STD
#include <version>
#endif
