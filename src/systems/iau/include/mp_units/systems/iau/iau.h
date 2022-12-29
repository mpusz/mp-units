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

#include <mp_units/systems/si/si.h>
#include <mp_units/unit.h>

namespace mp_units::iau {

// https://en.wikipedia.org/wiki/Astronomical_system_of_units

// clang-format off
// time
inline constexpr struct day : named_unit<"D", si::day> {} day;
inline constexpr struct Julian_year : named_unit<"a", mag<86400> * si::second> {} Julian_year;

// mass
inline constexpr struct solar_mass : named_unit<basic_symbol_text{"M_☉", "M_SUN"}, mag<ratio{198'892, 100'000}> * mag_power<10, 30> * si::kilogram> {} solar_mass;
inline constexpr struct Jupiter_mass : named_unit<"M_JUP", mag<ratio{1'898, 1'000}> * mag_power<10, 27> * si::kilogram> {} Jupiter_mass;
inline constexpr struct Earth_mass : named_unit<"M_EARTH", mag<ratio{59'742, 10'000}> * mag_power<10, 24> * si::kilogram> {} Earth_mass;

// length
using si::astronomical_unit;

// https://en.wikipedia.org/wiki/Lunar_distance_(astronomy)
inline constexpr struct lunar_distance : named_unit<"LD", mag<384'399> * si::kilo<si::metre>> {} lunar_distance;

// https://en.wikipedia.org/wiki/Light-year
inline constexpr struct light_year : named_unit<"ly", mag<9'460'730'472'580'800> * si::metre> {} light_year;

// https://en.wikipedia.org/wiki/Parsec
inline constexpr struct parsec : named_unit<"pc", mag<30'856'775'814'913'673> * si::metre> {} parsec;

// https://en.wikipedia.org/wiki/Angstrom
inline constexpr struct angstrom : named_unit<basic_symbol_text{"Å", "A"}, mag_power<10, -10> * si::metre> {} angstrom;

// speed
inline constexpr struct speed_of_light_unit : constant_unit<"c_0", si::si2019::speed_of_light_in_vacuum_unit> {} speed_of_light_unit;
inline constexpr auto speed_of_light = isq::speed_of_light[speed_of_light_unit];

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

inline constexpr auto c_0 = speed_of_light_unit;

}  // namespace unit_symbols

}  // namespace mp_units::iau
