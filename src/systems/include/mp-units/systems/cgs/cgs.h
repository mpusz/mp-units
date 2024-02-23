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
#include <mp-units/systems/si/units.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/unit.h>
#endif

MP_UNITS_EXPORT
namespace mp_units::cgs {

// clang-format off
#if MP_UNITS_COMP_MSVC
inline constexpr struct centimetre : si::centi_<si::metre> {} centimetre;
#else
inline constexpr struct centimetre : decltype(si::centi<si::metre>) {} centimetre;
#endif
inline constexpr struct gram : decltype(si::gram) {} gram;
inline constexpr struct second : decltype(si::second) {} second;
inline constexpr struct gal : named_unit<"Gal", centimetre / square(second)> {} gal;
inline constexpr struct dyne : named_unit<"dyn", gram * centimetre / square(second)> {} dyne;
inline constexpr struct erg : named_unit<"erg", dyne * centimetre> {} erg;
inline constexpr struct barye : named_unit<"Ba", gram / (centimetre * square(second))> {} barye;
inline constexpr struct poise : named_unit<"P", gram / (centimetre * second)> {} poise;
inline constexpr struct stokes : named_unit<"St", square(centimetre) / second> {} stokes;
inline constexpr struct kayser : named_unit<"K", one / centimetre> {} kayser;
// clang-format on

namespace unit_symbols {

inline constexpr auto cm = centimetre;
inline constexpr auto g = gram;
inline constexpr auto s = second;
inline constexpr auto Gal = gal;
inline constexpr auto dyn = dyne;
inline constexpr auto Ba = barye;
inline constexpr auto P = poise;
inline constexpr auto St = stokes;
inline constexpr auto K = kayser;

// commonly used squared and cubic units
inline constexpr auto cm2 = square(centimetre);
inline constexpr auto cm3 = cubic(centimetre);
inline constexpr auto s2 = square(second);
inline constexpr auto s3 = cubic(second);

}  // namespace unit_symbols

}  // namespace mp_units::cgs
