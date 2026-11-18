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
#include <mp-units/systems/si/unit_symbols.h>
#include <mp-units/systems/si/units.h>

// IWYU pragma: begin_exports
#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework.h>
#endif
// IWYU pragma: end_exports

/**
 * The centimetre-gram-second system of units (CGS)
 *
 * The CGS base units and its named mechanical derived units, such as the gal, the dyne, the erg, and
 * the poise.
 *
 * @code{.cpp}
 * using cgs::unit_symbols::dyn;
 *
 * quantity force = 1. * dyn;
 * quantity in_newtons = force.in(N);  // 1e-05 N
 * @endcode
 *
 * @see [Centimetre-Gram-Second (CGS) System](docs:users_guide/systems/cgs) in the User's Guide
 * @see [CGS System](docs:reference/systems_reference/systems/cgs) for every entity it defines
 */
MP_UNITS_EXPORT
namespace mp_units::cgs {

// clang-format off
inline constexpr auto centimetre = si::centi<si::metre>;
using si::gram;
using si::second;
inline constexpr struct gal final : named_unit<"Gal", centimetre / square(second)> {} gal;
inline constexpr struct dyne final : named_unit<"dyn", gram * centimetre / square(second)> {} dyne;
inline constexpr struct erg final : named_unit<"erg", dyne * centimetre> {} erg;
inline constexpr struct barye final : named_unit<"Ba", gram / (centimetre * square(second))> {} barye;
inline constexpr struct poise final : named_unit<"P", gram / (centimetre * second)> {} poise;
inline constexpr struct stokes final : named_unit<"St", square(centimetre) / second> {} stokes;
inline constexpr struct kayser final : named_unit<"K", one / centimetre> {} kayser;
// clang-format on

namespace unit_symbols {

using si::unit_symbols::cm;
using si::unit_symbols::g;
using si::unit_symbols::s;
inline constexpr auto Gal = gal;
inline constexpr auto dyn = dyne;
using cgs::erg;
inline constexpr auto Ba = barye;
inline constexpr auto P = poise;
inline constexpr auto St = stokes;
inline constexpr auto K = kayser;

// commonly used squared and cubic units
inline constexpr auto cm2 = square(centimetre);
inline constexpr auto cm3 = cubic(centimetre);
using si::unit_symbols::s2;
using si::unit_symbols::s3;

}  // namespace unit_symbols

}  // namespace mp_units::cgs
