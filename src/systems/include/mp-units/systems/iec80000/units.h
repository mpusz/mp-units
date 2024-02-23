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

#include <mp-units/bits/module_macros.h>
#include <mp-units/systems/iec80000/quantities.h>
#include <mp-units/systems/si/units.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/unit.h>
#endif

MP_UNITS_EXPORT
namespace mp_units::iec80000 {

// clang-format off
inline constexpr struct erlang : named_unit<"E", kind_of<traffic_intensity>> {} erlang;
inline constexpr struct bit : named_unit<"bit", one, kind_of<storage_capacity>> {} bit;
inline constexpr struct octet : named_unit<"o", mag<8> * bit> {} octet;
inline constexpr struct byte : named_unit<"B", mag<8> * bit> {} byte;
inline constexpr struct baud : named_unit<"Bd", one / si::second, kind_of<modulation_rate>> {} baud;
// clang-format on

}  // namespace mp_units::iec80000
