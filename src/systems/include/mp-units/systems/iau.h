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
#include <mp-units/systems/si/unit_symbols.h>
#include <mp-units/systems/si/units.h>

// IWYU pragma: begin_exports
#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework.h>
#endif
// IWYU pragma: end_exports

MP_UNITS_EXPORT
namespace mp_units::iau {

// IAU 2015 Resolution B3 - Nominal Solar and Planetary Conversion Constants
// https://www.iau.org/Iau/Publications/List-of-Resolutions

// clang-format off
// Nominal solar conversion constants (exact by definition)
inline constexpr struct nominal_solar_radius final : named_unit<symbol_text{u8"R_☉ᴺ", "R_SUN_N"}, mag_ratio<6'957, 1'000> * mag_power<10, 8> * si::metre> {} nominal_solar_radius;
inline constexpr struct nominal_total_solar_irradiance final : named_unit<symbol_text{u8"S_☉ᴺ", "S_SUN_N"}, mag<1361> * si::watt / square(si::metre)> {} nominal_total_solar_irradiance;
inline constexpr struct nominal_solar_luminosity final : named_unit<symbol_text{u8"L_☉ᴺ", "L_SUN_N"}, mag_ratio<3'828, 1'000> * mag_power<10, 26> * si::watt> {} nominal_solar_luminosity;
inline constexpr struct nominal_solar_effective_temperature final : named_unit<symbol_text{u8"Tₑff,☉ᴺ", "T_EFF_SUN_N"}, mag<5772> * si::kelvin> {} nominal_solar_effective_temperature;
inline constexpr struct nominal_solar_mass_parameter final : named_unit<symbol_text{u8"(GM)_☉ᴺ", "(GM)_SUN_N"}, mag_ratio<13'271'244, 10'000'000> * mag_power<10, 20> * cubic(si::metre) / square(si::second)> {} nominal_solar_mass_parameter;

// Nominal planetary conversion constants (exact by definition)
inline constexpr struct nominal_terrestrial_equatorial_radius final : named_unit<symbol_text{u8"R_⊕ₑᴺ", "R_EARTH_E_N"}, mag_ratio<63'781, 10'000> * mag_power<10, 6> * si::metre> {} nominal_terrestrial_equatorial_radius;
inline constexpr struct nominal_terrestrial_polar_radius final : named_unit<symbol_text{u8"R_⊕ₚᴺ", "R_EARTH_P_N"}, mag_ratio<63'568, 10'000> * mag_power<10, 6> * si::metre> {} nominal_terrestrial_polar_radius;
inline constexpr struct nominal_jovian_equatorial_radius final : named_unit<symbol_text{u8"R_♃ₑᴺ", "R_JUP_E_N"}, mag_ratio<71'492, 10'000> * mag_power<10, 7> * si::metre> {} nominal_jovian_equatorial_radius;
inline constexpr struct nominal_jovian_polar_radius final : named_unit<symbol_text{u8"R_♃ₚᴺ", "R_JUP_P_N"}, mag_ratio<66'854, 10'000> * mag_power<10, 7> * si::metre> {} nominal_jovian_polar_radius;
inline constexpr struct nominal_terrestrial_mass_parameter final : named_unit<symbol_text{u8"(GM)_⊕ᴺ", "(GM)_EARTH_N"}, mag_ratio<3'986'004, 1'000'000> * mag_power<10, 14> * cubic(si::metre) / square(si::second)> {} nominal_terrestrial_mass_parameter;
inline constexpr struct nominal_jovian_mass_parameter final : named_unit<symbol_text{u8"(GM)_♃ᴺ", "(GM)_JUP_N"}, mag_ratio<12'668'653, 10'000'000> * mag_power<10, 17> * cubic(si::metre) / square(si::second)> {} nominal_jovian_mass_parameter;

// IAU 2012 Resolution B2 - astronomical unit
// https://www.iau.org/Iau/Publications/List-of-Resolutions
using non_si::astronomical_unit;

// Derived unitss
inline constexpr struct parsec final : named_unit<"pc", astronomical_unit / (mag_ratio<1, 3600> * si::degree)> {} parsec;

inline namespace codata2018 {

// Newtonian constant of gravitation (CODATA 2018)
// https://physics.nist.gov/cgi-bin/cuu/Value?bg
inline constexpr struct newtonian_constant_of_gravitation final : named_unit<"G", mag_ratio<667'430, 100'000> * mag_power<10, -11> * cubic(si::metre) / si::kilogram / square(si::second)> {} newtonian_constant_of_gravitation;

// Derived SI masses using (GM)ᴺ / G
inline constexpr struct solar_mass final : named_unit<symbol_text{u8"M_☉", "M_SUN"}, nominal_solar_mass_parameter / newtonian_constant_of_gravitation> {} solar_mass;
inline constexpr struct terrestrial_mass final : named_unit<symbol_text{u8"M_⊕", "M_EARTH"}, nominal_terrestrial_mass_parameter / newtonian_constant_of_gravitation> {} terrestrial_mass;
inline constexpr struct jovian_mass final : named_unit<symbol_text{u8"M_♃", "M_JUP"}, nominal_jovian_mass_parameter / newtonian_constant_of_gravitation> {} jovian_mass;

namespace unit_symbols {

inline constexpr auto G = newtonian_constant_of_gravitation;
inline constexpr auto M_SUN = solar_mass;
inline constexpr auto M_EARTH = terrestrial_mass;
inline constexpr auto M_JUP = jovian_mass;

}  // namespace unit_symbols

}  // namespace codata2018

// clang-format on

namespace unit_symbols {

// IAU 2015 Resolution B3 nominal values
inline constexpr auto R_SUN_N = nominal_solar_radius;
inline constexpr auto S_SUN_N = nominal_total_solar_irradiance;
inline constexpr auto L_SUN_N = nominal_solar_luminosity;
inline constexpr auto T_EFF_SUN_N = nominal_solar_effective_temperature;
inline constexpr auto GM_SUN_N = nominal_solar_mass_parameter;

inline constexpr auto R_EARTH_E_N = nominal_terrestrial_equatorial_radius;
inline constexpr auto R_EARTH_P_N = nominal_terrestrial_polar_radius;
inline constexpr auto R_JUP_E_N = nominal_jovian_equatorial_radius;
inline constexpr auto R_JUP_P_N = nominal_jovian_polar_radius;
inline constexpr auto GM_EARTH_N = nominal_terrestrial_mass_parameter;
inline constexpr auto GM_JUP_N = nominal_jovian_mass_parameter;

// IAU 2012 Resolution B1
using non_si::unit_symbols::au;

// Derived units
inline constexpr auto pc = parsec;

}  // namespace unit_symbols

}  // namespace mp_units::iau
