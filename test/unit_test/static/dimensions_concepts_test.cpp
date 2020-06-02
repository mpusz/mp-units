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


#include <units/physical/si.h>

namespace {

using namespace units::physical;

static_assert(in_length<si::length<si::metre>>);
static_assert(!in_length<si::time<si::second>>);

static_assert(in_mass<si::mass<si::kilogram>>);
static_assert(!in_mass<si::time<si::second>>);

static_assert(in_time<si::time<si::second>>);
static_assert(!in_time<si::length<si::metre>>);

static_assert(in_current<si::current<si::ampere>>);
static_assert(!in_current<si::time<si::second>>);

static_assert(in_temperature<si::temperature<si::kelvin>>);
static_assert(!in_temperature<si::time<si::second>>);

static_assert(in_substance<si::substance<si::mole>>);
static_assert(!in_substance<si::time<si::second>>);

static_assert(in_luminous_intensity<si::luminous_intensity<si::candela>>);
static_assert(!in_luminous_intensity<si::time<si::second>>);

static_assert(in_frequency<si::frequency<si::hertz>>);
static_assert(!in_frequency<si::time<si::second>>);

static_assert(in_area<si::area<si::square_metre>>);
static_assert(!in_area<si::time<si::second>>);

static_assert(in_volume<si::volume<si::cubic_metre>>);
static_assert(!in_volume<si::time<si::second>>);

static_assert(in_speed<si::speed<si::metre_per_second>>);
static_assert(!in_speed<si::time<si::second>>);

static_assert(in_acceleration<si::acceleration<si::metre_per_second_sq>>);
static_assert(!in_acceleration<si::time<si::second>>);

static_assert(in_force<si::force<si::newton>>);
static_assert(!in_force<si::time<si::second>>);

static_assert(in_energy<si::energy<si::joule>>);
static_assert(!in_energy<si::time<si::second>>);

static_assert(in_power<si::power<si::watt>>);
static_assert(!in_power<si::time<si::second>>);

static_assert(in_voltage<si::voltage<si::volt>>);
static_assert(!in_voltage<si::time<si::second>>);

static_assert(in_electric_charge<si::electric_charge<si::coulomb>>);
static_assert(!in_electric_charge<si::time<si::second>>);

static_assert(in_capacitance<si::capacitance<si::farad>>);
static_assert(!in_capacitance<si::time<si::second>>);

static_assert(in_surface_tension<si::surface_tension<si::newton_per_metre>>);
static_assert(!in_surface_tension<si::time<si::second>>);

static_assert(in_pressure<si::pressure<si::pascal>>);
static_assert(!in_pressure<si::time<si::second>>);

static_assert(in_magnetic_induction<si::magnetic_induction<si::tesla>>);
static_assert(!in_magnetic_induction<si::time<si::second>>);

static_assert(in_magnetic_flux<si::magnetic_flux<si::weber>>);
static_assert(!in_magnetic_flux<si::time<si::second>>);

static_assert(in_inductance<si::inductance<si::henry>>);
static_assert(!in_inductance<si::time<si::second>>);

static_assert(in_conductance<si::conductance<si::siemens>>);
static_assert(!in_conductance<si::time<si::second>>);

// TODO Add when downcasting issue is solved
// static_assert(Radioactivity<si::radioactivity<si::siemens>>);
// static_assert(!Radioactivity<si::time<si::second>>);

static_assert(in_catalytic_activity<si::catalytic_activity<si::katal>>);
static_assert(!in_catalytic_activity<si::time<si::second>>);

static_assert(in_absorbed_dose<si::absorbed_dose<si::gray>>);
static_assert(!in_absorbed_dose<si::time<si::second>>);

static_assert(in_current_density<si::current_density<si::ampere_per_metre_sq>>);
static_assert(!in_current_density<si::time<si::second>>);

static_assert(in_concentration<si::concentration<si::mol_per_metre_cub>>);
static_assert(!in_concentration<si::time<si::second>>);

static_assert(in_luminance<si::luminance<si::candela_per_metre_sq>>);
static_assert(!in_luminance<si::time<si::second>>);

static_assert(in_dynamic_viscosity<si::dynamic_viscosity<si::pascal_second>>);
static_assert(!in_dynamic_viscosity<si::time<si::second>>);

static_assert(in_heat_capacity<si::heat_capacity<si::joule_per_kelvin>>);
static_assert(!in_heat_capacity<si::time<si::second>>);

static_assert(in_specific_heat_capacity<si::specific_heat_capacity<si::joule_per_kilogram_kelvin>>);
static_assert(!in_specific_heat_capacity<si::time<si::second>>);

static_assert(in_molar_heat_capacity<si::molar_heat_capacity<si::joule_per_mole_kelvin>>);
static_assert(!in_molar_heat_capacity<si::time<si::second>>);

static_assert(in_thermal_conductivity<si::thermal_conductivity<si::watt_per_metre_kelvin>>);
static_assert(!in_thermal_conductivity<si::time<si::second>>);

// TODO Add when downcasting issue is solved
// static_assert(EnergyDensity<si::energy_density<si::joule_per_metre_cub>>);
// static_assert(!EnergyDensity<si::time<si::second>>);

static_assert(in_electric_field_strength<si::electric_field_strength<si::volt_per_metre>>);
static_assert(!in_electric_field_strength<si::time<si::second>>);

static_assert(in_charge_density<si::charge_density<si::coulomb_per_metre_cub>>);
static_assert(!in_charge_density<si::time<si::second>>);

static_assert(in_surface_charge_density<si::surface_charge_density<si::coulomb_per_metre_sq>>);
static_assert(!in_surface_charge_density<si::time<si::second>>);

static_assert(in_permittivity<si::permittivity<si::farad_per_metre>>);
static_assert(!in_permittivity<si::time<si::second>>);

static_assert(in_permeability<si::permeability<si::henry_per_metre>>);
static_assert(!in_permeability<si::time<si::second>>);

static_assert(in_molar_energy<si::molar_energy<si::joule_per_mole>>);
static_assert(!in_molar_energy<si::time<si::second>>);

}
