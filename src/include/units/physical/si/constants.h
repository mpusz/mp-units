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

#include <units/physical/si/electric_charge.h>
#include <units/physical/si/energy.h>
#include <units/physical/si/frequency.h>
#include <units/physical/si/power.h>
#include <units/physical/si/substance.h>
#include <units/physical/si/temperature.h>
#include <units/physical/si/speed.h>

namespace units::physical::si::si2019 {

template<Scalar Rep = double>
inline constexpr auto planck_constant = energy<joule, Rep>(6.62607015e-34) * time<second, Rep>(1);

template<Scalar Rep = double>
inline constexpr auto reduced_planck_constant = energy<gigaelectronvolt, Rep>(6.582119569e-10) * time<second, Rep>(1);

template<Scalar Rep = double>
inline constexpr auto elementary_charge = electric_charge<coulomb, Rep>(1.602176634e-19);

template<Scalar Rep = double>
inline constexpr auto boltzmann_constant = energy<joule, Rep>(1.380649e-23) / temperature<kelvin, Rep>(1);

template<Scalar Rep = double>
inline constexpr auto avogadro_constant = Rep(6.02214076e23) / substance<mole, Rep>(1);

template<Scalar Rep = double>
inline constexpr auto speed_of_light = speed<metre_per_second, Rep>(299'792'458);

template<Scalar Rep = double>
inline constexpr auto hyperfine_structure_transition_frequency = frequency<hertz, Rep>(9'192'631'770);

// template<Scalar Rep = double>
// inline constexpr auto luminous_efficacy = 683q_lm / 1q_W;

template<Scalar Rep = double>
inline constexpr auto standard_gravity = acceleration<metre_per_second_sq, Rep>(9.80665);

}  // namespace units::physical::si::si2019
