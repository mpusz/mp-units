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

#include <units/constant.h>
#include <units/physical/si/electric_charge.h>
#include <units/physical/si/frequency.h>
#include <units/physical/si/energy.h>
#include <units/physical/si/power.h>
#include <units/physical/si/substance.h>
#include <units/physical/si/velocity.h>
#include <units/physical/si/temperature.h>

namespace units::si {

inline constexpr auto planck_constant = constant<ratio<662607015, 1, -42>, exp<dim_energy, 1>, exp<dim_time, 1>>;
inline constexpr auto elementary_charge = constant<ratio<1602176634, 1, -51>, exp<dim_electric_charge, 1>>;
inline constexpr auto boltzmann_constant =
    constant<ratio<1380649, 1, -29>, exp<dim_energy, 1>, exp<dim_thermodynamic_temperature, -1>>;
inline constexpr auto avogadro_constant = constant<ratio<602214076, 1, 15>, exp<dim_substance, -1>>;
inline constexpr auto speed_of_light = constant<ratio<299792458, 1, 15>, exp<dim_velocity, 1>>;
inline constexpr auto hyperfine_structure_transition_frequency = constant<ratio<9192631770>, exp<dim_frequency, 1>>;
// inline constexpr auto luminous_efficacy = constant<ratio<683>, exp<luminous_flux, 1>, exp<power, -1>>;

}  // namespace units::si
