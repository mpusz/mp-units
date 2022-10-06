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
#include <units/reference.h>
#include <units/si/units.h>

namespace units::si::cgs {

// clang-format off
inline constexpr struct centimetre : si::centi_<si::metre> {} centimetre;
inline constexpr struct gram : si::gram {} gram;
inline constexpr struct second : si::second {} second;
inline constexpr struct gal : named_unit<"Gal", centimetre / square<second>> {} gal;
inline constexpr struct dyne : named_unit<"dyn", gram * centimetre / square<second>> {} dyne;
inline constexpr struct erg : named_unit<"erg", dyne / centimetre> {} erg;
inline constexpr struct barye : named_unit<"Ba", gram / (centimetre * square<second>)> {} barye;
inline constexpr struct poise : named_unit<"P", gram / (centimetre * second)> {} poise;
inline constexpr struct stokes : named_unit<"St", square<centimetre> / second> {} stokes;
inline constexpr struct kayser : decltype(1 / centimetre) {} kayser;

inline constexpr struct length : system_reference<isq::length_dim, centimetre> {} length;
inline constexpr struct mass : system_reference<isq::mass_dim, gram> {} mass;
inline constexpr struct time : system_reference<isq::time_dim, second> {} time;
inline constexpr struct speed : system_reference<isq::speed_dim, centimetre / second> {} speed;
inline constexpr struct acceleration : system_reference<isq::acceleration_dim, gal> {} acceleration;
inline constexpr struct force : system_reference<isq::force_dim, dyne> {} force;inline constexpr struct energy : system_reference<isq::energy_dim, erg> {} energy;
inline constexpr struct power : system_reference<isq::power_dim, erg / second> {} power;
inline constexpr struct dynamic_viscosity : system_reference<isq::dynamic_viscosity_dim, poise> {} dynamic_viscosity;
inline constexpr struct kinematic_viscosity : system_reference<isq::kinematic_viscosity_dim, stokes> {} kinematic_viscosity;
// inline constexpr struct wavenumber : system_reference<isq::wavenumber_dim, dyne> {} wavenumber;
// clang-format on

}  // namespace units::si::cgs
