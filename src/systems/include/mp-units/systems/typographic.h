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
#include <mp-units/systems/si.h>
#include <mp-units/systems/yard_pound.h>

// IWYU pragma: begin_exports
#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework.h>
#endif
// IWYU pragma: end_exports

MP_UNITS_EXPORT
namespace mp_units::typographic {

// clang-format off
// https://en.wikipedia.org/wiki/Point_(typography)

// US point and pica: traditional American typographic units
// 1 pica(us) = 0.166044 inch, 1 point(us) = 1/12 pica(us)
inline constexpr struct pica_us final : named_unit<"pica(us)", mag_ratio<166'044, 1'000'000> * yard_pound::inch> {} pica_us;
inline constexpr struct point_us final : named_unit<"point(us)", mag_ratio<1, 12> * pica_us> {} point_us;

// DTP (PostScript) point and pica: desktop publishing standard
// 1 point(dtp) = 1/72 inch, 1 pica(dtp) = 12 point(dtp)
inline constexpr struct point_dtp final : named_unit<"point(dtp)", mag_ratio<1, 72> * yard_pound::inch> {} point_dtp;
inline constexpr struct pica_dtp final : named_unit<"pica(dtp)", mag<12> * point_dtp> {} pica_dtp;

// Didot point and Cicero: European typographic units
// 1 point(didot) = 0.3759 mm, 1 cicero = 12 point(didot)
inline constexpr struct didot_point final : named_unit<"point(didot)", mag_ratio<3759, 10'000'000> * si::metre> {} didot_point;
inline constexpr struct cicero final : named_unit<"cicero", mag<12> * didot_point> {} cicero;

// Q and H: Japanese typographic units
// 1 Q = 0.25 mm (also known as級 kyū)
// 1 H = 0.25 mm (used for line spacing, also known as 歯 ha)
inline constexpr struct q final : named_unit<"Q", mag_ratio<1, 4'000> * si::metre> {} q;
inline constexpr struct h final : named_unit<"H", mag_ratio<1, 4'000> * si::metre> {} h;

// TeX point: used in TeX and LaTeX typesetting systems
// 1 point(tex) = 1/72.27 inch
inline constexpr struct tex_point final : named_unit<"point(tex)", mag_ratio<1, 7227> * mag<100> * yard_pound::inch> {} tex_point;
// clang-format on

namespace unit_symbols {

inline constexpr auto pt_us = point_us;
inline constexpr auto pc_us = pica_us;
inline constexpr auto pt = point_dtp;
inline constexpr auto pc = pica_dtp;
inline constexpr auto pt_didot = didot_point;
inline constexpr auto cc = cicero;
inline constexpr auto Q = q;
inline constexpr auto H = h;
inline constexpr auto pt_tex = tex_point;

}  // namespace unit_symbols

}  // namespace mp_units::typographic
