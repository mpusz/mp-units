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
#include <mp-units/systems/iau.h>
#include <mp-units/systems/si/units.h>

// IWYU pragma: begin_exports
#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework.h>
#endif
// IWYU pragma: end_exports

MP_UNITS_EXPORT
namespace mp_units::astronomy {

// Commonly used astronomy units not standardized by IAU

// clang-format off
// time
inline constexpr struct day final : named_unit<"D", non_si::day> {} day;
inline constexpr struct sidereal_day final : named_unit<"D_sid", mag_ratio<86'164'0905, 1'000'000> * si::second> {} sidereal_day;
inline constexpr struct Julian_year final : named_unit<"a", mag_ratio<365'25, 100> * day> {} Julian_year;
inline constexpr struct tropical_year final : named_unit<"a_trop", mag_ratio<365'242189, 1'000'000> * day> {} tropical_year;
inline constexpr struct century final : named_unit<"c", mag<100> * Julian_year> {} century;
inline constexpr struct millennium final : named_unit<"ka", mag<1000> * Julian_year> {} millennium;

// length
// https://en.wikipedia.org/wiki/Lunar_distance_(astronomy)
inline constexpr struct lunar_distance final : named_unit<"LD", mag<384'399> * si::kilo<si::metre>> {} lunar_distance;

// https://en.wikipedia.org/wiki/Light-year
inline constexpr struct light_year final : named_unit<"ly", mag<9'460'730'472'580'800> * si::metre> {} light_year;

// spectral flux density
// https://en.wikipedia.org/wiki/Jansky
inline constexpr struct jansky final : named_unit<"Jy", mag_power<10, -26> * si::watt / square(si::metre) / si::hertz> {} jansky;

// Legacy astronomical constant (obsolete, use IAU nominal values instead)
// https://en.wikipedia.org/wiki/Astronomical_constant
inline constexpr struct gaussian_gravitational_constant final :
    named_unit<"k", mag_ratio<1'720'209'895, 100'000'000'000> * pow<3, 2>(iau::astronomical_unit) / pow<1,2>(iau::solar_mass) / day> {} gaussian_gravitational_constant;
// clang-format on

namespace unit_symbols {

inline constexpr auto D = day;
inline constexpr auto D_sid = sidereal_day;
inline constexpr auto a = Julian_year;
inline constexpr auto a_trop = tropical_year;
inline constexpr auto c = century;
inline constexpr auto ka = millennium;

inline constexpr auto LD = lunar_distance;
inline constexpr auto ly = light_year;

inline constexpr auto Jy = jansky;

inline constexpr auto k = gaussian_gravitational_constant;

}  // namespace unit_symbols

}  // namespace mp_units::astronomy
