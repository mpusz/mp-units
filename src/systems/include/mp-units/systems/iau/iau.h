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
#include <mp-units/systems/si/constants.h>
#include <mp-units/systems/si/units.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/unit.h>
#endif

MP_UNITS_EXPORT
namespace mp_units::iau {

// https://en.wikipedia.org/wiki/Astronomical_system_of_units

// clang-format off
// time
inline constexpr struct day : named_unit<"D", si::day> {} day;
inline constexpr struct Julian_year : named_unit<"a", mag<ratio{365'25, 100}> * day> {} Julian_year;

// mass
// https://en.wikipedia.org/wiki/Solar_mass
// TODO What is the official mass of sun (every source in the Internet provides a different value)
inline constexpr struct solar_mass : named_unit<symbol_text{u8"M_☉", "M_SUN"}, mag<ratio{198'847, 100'000}> * mag_power<10, 30> * si::kilogram> {} solar_mass;
inline constexpr struct Jupiter_mass : named_unit<"M_JUP", mag<ratio{1'898, 1'000}> * mag_power<10, 27> * si::kilogram> {} Jupiter_mass;
inline constexpr struct Earth_mass : named_unit<"M_EARTH", mag<ratio{59'742, 10'000}> * mag_power<10, 24> * si::kilogram> {} Earth_mass;

// length
inline constexpr struct astronomical_unit : decltype(si::astronomical_unit) {} astronomical_unit;

// https://en.wikipedia.org/wiki/Lunar_distance_(astronomy)
inline constexpr struct lunar_distance : named_unit<"LD", mag<384'399> * si::kilo<si::metre>> {} lunar_distance;

// https://en.wikipedia.org/wiki/Light-year
inline constexpr struct light_year : named_unit<"ly", mag<9'460'730'472'580'800> * si::metre> {} light_year;

// https://en.wikipedia.org/wiki/Parsec
inline constexpr struct parsec : named_unit<"pc", astronomical_unit / (mag<ratio{1, 60 * 60}> * si::degree)> {} parsec;

// https://en.wikipedia.org/wiki/Angstrom
inline constexpr struct angstrom : named_unit<symbol_text{u8"Å", "A"}, mag_power<10, -10> * si::metre> {} angstrom;

// selected constants
// https://en.wikipedia.org/wiki/Astronomical_constant
inline constexpr struct gaussian_gravitational_constant :
    named_unit<"k", mag<ratio{1'720'209'895, 100'000'000'000}> * pow<3, 2>(astronomical_unit) / pow<1,2>(solar_mass) / day> {} gaussian_gravitational_constant;

inline constexpr struct speed_of_light :
    named_unit<symbol_text{u8"c₀", "c_0"}, si::si2019::speed_of_light_in_vacuum> {} speed_of_light;

inline constexpr struct constant_of_gravitation :
    named_unit<"G", mag<ratio{667'430, 100'000}> * mag_power<10, -11> * cubic(si::metre) / si::kilogram / square(si::second)> {} constant_of_gravitation;

inline constexpr struct hubble_constant :
    named_unit<symbol_text{u8"H₀", "H_0"}, mag<ratio{701, 10}> * si::kilo<si::metre> / si::second / si::mega<parsec>> {} hubble_constant;
// clang-format on

namespace unit_symbols {

inline constexpr auto D = day;
inline constexpr auto a = Julian_year;

inline constexpr auto M_SUN = solar_mass;
inline constexpr auto M_JUP = Jupiter_mass;
inline constexpr auto M_EARTH = Earth_mass;

inline constexpr auto au = astronomical_unit;
inline constexpr auto LD = lunar_distance;
inline constexpr auto ly = light_year;
inline constexpr auto pc = parsec;
inline constexpr auto A = angstrom;

inline constexpr auto k = gaussian_gravitational_constant;
inline constexpr auto c_0 = speed_of_light;
inline constexpr auto G = constant_of_gravitation;
inline constexpr auto H_0 = hubble_constant;

}  // namespace unit_symbols

}  // namespace mp_units::iau
