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

#include <units/generic/angle.h>
#include <units/isq/si/amount_of_substance.h>
#include <units/isq/si/electric_charge.h>
#include <units/isq/si/energy.h>
#include <units/isq/si/frequency.h>
#include <units/isq/si/luminous_flux.h>
#include <units/isq/si/power.h>
#include <units/isq/si/speed.h>
#include <units/isq/si/thermodynamic_temperature.h>

namespace units::isq::si {

namespace si2019 {

// defining constants
template<Representation Rep = double>
inline constexpr auto hyperfine_structure_transition_frequency = frequency<hertz, Rep>(Rep{9'192'631'770});

template<Representation Rep = double>
inline constexpr auto speed_of_light = speed<metre_per_second, Rep>(299'792'458);

template<Representation Rep = double>
inline constexpr auto planck_constant = energy<joule, Rep>(6.62607015e-34) * time<second, Rep>(1);

template<Representation Rep = double>
inline constexpr auto elementary_charge = electric_charge<coulomb, Rep>(1.602176634e-19);

template<Representation Rep = double>
inline constexpr auto boltzmann_constant = energy<joule, Rep>(1.380649e-23) / thermodynamic_temperature<kelvin, Rep>(1);

template<Representation Rep = double>
inline constexpr auto avogadro_constant = Rep(6.02214076e23) / amount_of_substance<mole, Rep>(1);

template<Representation Rep = double>
inline constexpr auto luminous_efficacy = luminous_flux<lumen, Rep>(683) / power<watt, Rep>(1);

}  // namespace si2019

template<Representation Rep = double>
inline constexpr auto standard_gravity = acceleration<metre_per_second_sq, Rep>(9.80665);

}  // namespace units::isq::si
