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

#include <units/isq/mechanics.h>
#include <units/isq/space_and_time.h>
#include <units/isq/thermodynamics.h>
#include <units/quantity.h>
#include <units/si/units.h>

namespace units::si {

namespace si2019 {

// clang-format off
inline constexpr struct hyperfine_structure_transition_frequency_of_cs_unit :
  constant_unit<basic_symbol_text{"ΔνCs" "dvCs"}, mag<9'192'631'770> * hertz> {} hyperfine_structure_transition_frequency_of_cs_unit;
inline constexpr struct speed_of_light_in_vacuum_unit :
  constant_unit<"c", mag<299'792'458> * metre / second> {} speed_of_light_in_vacuum_unit;
inline constexpr struct planck_constant_unit :
  constant_unit<"h", mag<ratio{662'607'015, 100'000'000}> * mag_power<10, -34> * joule * second> {} planck_constant_unit;
inline constexpr struct elementary_charge_unit :
  constant_unit<"e", mag<ratio{1'602'176'634, 1'000'000'000}> * mag_power<10, -19> * coulomb> {} elementary_charge_unit;
inline constexpr struct boltzmann_constant_unit :
  constant_unit<"k", mag<ratio{1'380'649, 1'000'000}> * mag_power<10, -23> * joule / kelvin> {} boltzmann_constant_unit;
inline constexpr struct avogadro_constant_unit :
  constant_unit<"NA", mag<ratio{602'214'076, 100'000'000}> * mag_power<10, 23> * (1 / mole)> {} avogadro_constant_unit;
inline constexpr struct luminous_efficacy_unit :
  constant_unit<"Kcd", mag<683> * lumen / watt> {} luminous_efficacy_unit;
// clang-format on

inline constexpr auto hyperfine_structure_transition_frequency_of_cs =
  isq::frequency[hyperfine_structure_transition_frequency_of_cs_unit];
inline constexpr auto speed_of_light_in_vacuum = isq::speed[speed_of_light_in_vacuum_unit];
inline constexpr auto planck_constant = isq::action[planck_constant_unit];
// TODO uncomment the below when all the ISQ is provided
// inline constexpr auto elementary_charge = isq::electric_charge[elementary_charge_unit];
// inline constexpr auto boltzmann_constant = isq::heat_capacity[boltzmann_constant_unit];
inline constexpr auto avogadro_constant = (1 / isq::amount_of_substance)[avogadro_constant_unit];
// inline constexpr auto luminous_efficacy = isq::luminous_efficacy_of_radiation[luminous_efficacy_unit];

}  // namespace si2019

// clang-format off
inline constexpr struct standard_gravity_unit :
  constant_unit<"g", mag<ratio{980'665, 100'000}> * metre / square<second>> {} standard_gravity_unit;
// clang-format on

inline constexpr auto standard_gravity = isq::acceleration[standard_gravity_unit];

}  // namespace units::si
