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

// IWYU pragma: begin_exports
#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework.h>
#endif
// IWYU pragma: end_exports

MP_UNITS_EXPORT
namespace mp_units::cgs {

// clang-format off
constexpr auto centimetre = si::centi<si::metre>;
constexpr auto gram = si::gram;
constexpr auto second = si::second;
constexpr struct gal final : named_unit<"Gal", centimetre / square(second)> {} gal;
constexpr struct dyne final : named_unit<"dyn", gram * centimetre / square(second)> {} dyne;
constexpr struct erg final : named_unit<"erg", dyne * centimetre> {} erg;
constexpr struct barye final : named_unit<"Ba", gram / (centimetre * square(second))> {} barye;
constexpr struct poise final : named_unit<"P", gram / (centimetre * second)> {} poise;
constexpr struct stokes final : named_unit<"St", square(centimetre) / second> {} stokes;
constexpr struct kayser final : named_unit<"K", one / centimetre> {} kayser;
// clang-format on

namespace unit_symbols {

constexpr auto cm = centimetre;
constexpr auto g = gram;
constexpr auto s = second;
constexpr auto Gal = gal;
constexpr auto dyn = dyne;
constexpr auto Ba = barye;
constexpr auto P = poise;
constexpr auto St = stokes;
constexpr auto K = kayser;

// commonly used squared and cubic units
constexpr auto cm2 = square(centimetre);
constexpr auto cm3 = cubic(centimetre);
constexpr auto s2 = square(second);
constexpr auto s3 = cubic(second);

}  // namespace unit_symbols

}  // namespace mp_units::cgs
