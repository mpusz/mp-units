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
#include <mp-units/systems/isq/mechanics.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si/prefixes.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/system_reference.h>
#include <mp-units/unit.h>
#endif

MP_UNITS_EXPORT
namespace mp_units::natural {

// clang-format off
// units
inline constexpr struct electronvolt : named_unit<"eV"> {} electronvolt;
#if MP_UNITS_COMP_MSVC
inline constexpr struct gigaelectronvolt : si::giga_<electronvolt> {} gigaelectronvolt;
#else
inline constexpr struct gigaelectronvolt : decltype(si::giga<electronvolt>) {} gigaelectronvolt;
#endif

// system references
inline constexpr struct time : system_reference<isq::time, inverse(gigaelectronvolt)> {} time;
inline constexpr struct length : system_reference<isq::length, inverse(gigaelectronvolt)> {} length;
inline constexpr struct mass : system_reference<isq::mass, gigaelectronvolt> {} mass;
inline constexpr struct velocity : system_reference<isq::velocity, one> {} velocity;
inline constexpr struct speed : system_reference<isq::speed, one> {} speed;
inline constexpr struct acceleration : system_reference<isq::acceleration, gigaelectronvolt> {} acceleration;
inline constexpr struct momentum : system_reference<isq::momentum, gigaelectronvolt> {} momentum;
inline constexpr struct force : system_reference<isq::force, square(gigaelectronvolt)> {} force;
inline constexpr struct energy : system_reference<isq::mechanical_energy, gigaelectronvolt> {} energy;
// clang-format on

// constants
inline constexpr auto speed_of_light = speed[one];

namespace unit_symbols {

inline constexpr auto GeV = gigaelectronvolt;
inline constexpr auto GeV2 = square(gigaelectronvolt);

}  // namespace unit_symbols

}  // namespace mp_units::natural
