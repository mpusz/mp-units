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

#include <mp-units/bits/constexpr_format.h>
#include <mp-units/bits/hacks.h>  // IWYU pragma: keep
#include <mp-units/ext/fixed_string.h>

#if MP_UNITS_API_THROWING_CONSTRAINTS
#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <stdexcept>
#include <string>
#endif
#endif
#endif

namespace mp_units::detail {

#if MP_UNITS_API_THROWING_CONSTRAINTS

#if __cpp_lib_constexpr_exceptions

struct unsatisfied_constraints : std::logic_error {
  consteval explicit unsatisfied_constraints(const std::string& msg) : std::logic_error(msg) {}
  [[nodiscard]] constexpr const char* what() const noexcept override { return std::logic_error::what(); }
};

#else

struct unsatisfied_constraints {
  std::string msg;
  [[nodiscard]] consteval const char* what() const noexcept { return msg.c_str(); }
};

#endif

#endif

template<mp_units::fixed_string Fmt, typename... Args>
[[nodiscard]] consteval bool unsatisfied([[maybe_unused]] Args&&... args)
{
#if MP_UNITS_API_THROWING_CONSTRAINTS
  throw unsatisfied_constraints{constexpr_format(FMT_COMPILE(Fmt.c_str()), std::forward<Args>(args)...)};
#endif
  return false;
}

}  // namespace mp_units::detail
