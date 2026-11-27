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

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework/dimension.h>
#include <mp-units/framework/quantity_spec.h>
#include <mp-units/framework/unit.h>
#endif

MP_UNITS_EXPORT
namespace mp_units {

/**
 * A system of units that treats _angle_ as a base dimension
 *
 * Experimental and standalone, without the ISQ quantity hierarchy. It provides angular units
 * such as the radian, the degree, and the steradian, and trigonometric functions that take
 * or return them.
 *
 * @code{.cpp}
 * using angular::unit_symbols::deg;
 * using angular::unit_symbols::rad;
 *
 * quantity angle = 90. * deg;
 * quantity in_radians = angle.in(rad);  // 1.5708 rad
 * @endcode
 *
 * @see [Strong Angular System](docs:users_guide/systems/strong_angular_system) in the User's Guide
 * @see [Angular System](docs:reference/systems_reference/systems/angular) for every entity it defines
 */
namespace angular {

// clang-format off
inline constexpr struct dim_angle final : base_dimension<symbol_text{u8"α", "a"}> {} dim_angle;
QUANTITY_SPEC(angle, dim_angle);
QUANTITY_SPEC(solid_angle, pow<2>(angle));

inline constexpr struct radian final : named_unit<"rad", kind_of<angle>> {} radian;
inline constexpr struct revolution final : named_unit<"rev", mag<2> * π * radian> {} revolution;
inline constexpr struct degree final : named_unit<symbol_text{u8"°", "deg"}, mag_ratio<1, 360> * revolution, no_space_before_unit_symbol> {} degree;
inline constexpr struct gradian final : named_unit<symbol_text{u8"ᵍ", "grad"}, mag_ratio<1, 400> * revolution, no_space_before_unit_symbol> {} gradian;
inline constexpr struct steradian final : named_unit<"sr", square(radian)> {} steradian;
// clang-format on

namespace unit_symbols {

inline constexpr auto rad = radian;
inline constexpr auto rev = revolution;
inline constexpr auto deg = degree;
inline constexpr auto grad = gradian;
inline constexpr auto sr = steradian;
inline constexpr auto rad2 = square(radian);
inline constexpr auto deg2 = square(degree);

}  // namespace unit_symbols

}  // namespace angular

}  // namespace mp_units
