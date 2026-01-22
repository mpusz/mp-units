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

// Deprecated: The 'international' namespace has been renamed to 'yard_pound' for clarity.
// This header is provided for backward compatibility and will be removed in a future release.
// Please update your code to use <mp-units/systems/yard_pound.h> instead.

#warning \
  "2.6.0: The header <mp-units/systems/international.h> is deprecated. Use <mp-units/systems/yard_pound.h> instead."

// IWYU pragma: begin_exports
#include <mp-units/systems/yard_pound.h>
// IWYU pragma: end_exports

MP_UNITS_EXPORT
namespace mp_units {

// Deprecated namespace alias for backward compatibility
namespace international
  [[deprecated("2.6.0: The 'international' namespace has been renamed to 'yard_pound'. Please update your code.")]] {
using namespace yard_pound;
}

}  // namespace mp_units
