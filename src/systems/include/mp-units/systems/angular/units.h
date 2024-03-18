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
#include <mp-units/dimension.h>
#include <mp-units/quantity_spec.h>
#include <mp-units/unit.h>
#endif

MP_UNITS_EXPORT
namespace mp_units::angular {

// clang-format off
inline constexpr struct dim_angle : base_dimension<"A"> {} dim_angle;
QUANTITY_SPEC(angle, dim_angle);
QUANTITY_SPEC(solid_angle, pow<2>(angle));

inline constexpr struct radian : named_unit<"rad", kind_of<angle>> {} radian;
inline constexpr struct revolution : named_unit<"rev", mag<2> * mag_pi * radian> {} revolution;
inline constexpr struct degree : named_unit<symbol_text{u8"°", "deg"}, mag<ratio{1, 360}> * revolution> {} degree;
inline constexpr struct gradian : named_unit<symbol_text{u8"ᵍ", "grad"}, mag<ratio{1, 400}> * revolution> {} gradian;
inline constexpr struct steradian : named_unit<"sr", square(radian)> {} steradian;
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

}  // namespace mp_units::angular
