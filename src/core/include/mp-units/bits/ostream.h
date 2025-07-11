
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

#include <mp-units/bits/requires_hosted.h>
//
#include <mp-units/bits/module_macros.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cstdint>
#include <sstream>
#endif
#endif

namespace mp_units::detail {

template<typename CharT, class Traits, std::invocable<std::basic_ostream<CharT, Traits>&> F>
std::basic_ostream<CharT, Traits>& to_stream(std::basic_ostream<CharT, Traits>& os, const F& func)
{
  if (os.width()) {
    // std::setw() applies to the whole output so it has to be first put into std::string
    std::basic_ostringstream<CharT, Traits> oss;
    oss.flags(os.flags());
    oss.imbue(os.getloc());
    oss.precision(os.precision());
    func(oss);
    return os << std::move(oss).str();
  }

  func(os);
  return os;
}

}  // namespace mp_units::detail
