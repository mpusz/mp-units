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
#include <mp-units/systems/isq/si_quantities.h>
#include <mp-units/systems/isq/space_and_time.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/quantity_spec.h>
#endif

MP_UNITS_EXPORT
namespace mp_units::isq {

QUANTITY_SPEC(Celsius_temperature, thermodynamic_temperature);  // TODO should we account for T0 here?
QUANTITY_SPEC(linear_expansion_coefficient, inverse(length) * (length / thermodynamic_temperature));
QUANTITY_SPEC(cubic_expansion_coefficient, inverse(volume) * (volume / thermodynamic_temperature));
QUANTITY_SPEC(relative_pressure_coefficient, inverse(pressure) * (pressure / thermodynamic_temperature));
QUANTITY_SPEC(pressure_coefficient, pressure / thermodynamic_temperature);
QUANTITY_SPEC(isothermal_compressibility, inverse(volume) * (volume / pressure));  // TODO how to handle "negative" part
QUANTITY_SPEC(isentropic_compressibility, inverse(volume) * (volume / pressure));  // TODO how to handle "negative" part
// energy definition moved to mechanics
QUANTITY_SPEC(heat, energy);
inline constexpr auto amount_of_heat = heat;
QUANTITY_SPEC(latent_heat, heat);  // TODO what is a correct equation here?
QUANTITY_SPEC(heat_flow_rate, heat / time);
QUANTITY_SPEC(density_of_heat_flow_rate, heat_flow_rate / area);
QUANTITY_SPEC(thermal_conductivity, density_of_heat_flow_rate*(length / thermodynamic_temperature));
QUANTITY_SPEC(coefficient_of_heat_transfer, density_of_heat_flow_rate / thermodynamic_temperature);
QUANTITY_SPEC(surface_coefficient_of_heat_transfer, density_of_heat_flow_rate / thermodynamic_temperature);
QUANTITY_SPEC(thermal_insulance, inverse(coefficient_of_heat_transfer));
inline constexpr auto coefficient_of_thermal_insulance = thermal_insulance;
QUANTITY_SPEC(thermal_resistance, thermodynamic_temperature / heat_flow_rate);
QUANTITY_SPEC(thermal_conductance, inverse(thermal_resistance));
QUANTITY_SPEC(heat_capacity, heat / thermodynamic_temperature);
QUANTITY_SPEC(specific_heat_capacity, heat_capacity / mass);
QUANTITY_SPEC(specific_heat_capacity_at_constant_pressure, specific_heat_capacity);
QUANTITY_SPEC(specific_heat_capacity_at_constant_volume, specific_heat_capacity);
QUANTITY_SPEC(specific_heat_capacity_at_saturated_vapour_pressure, specific_heat_capacity);
QUANTITY_SPEC(thermal_diffusivity, thermal_conductivity / (mass_density * specific_heat_capacity_at_constant_pressure));
QUANTITY_SPEC(ratio_of_specific_heat_capacities, dimensionless,
              specific_heat_capacity_at_constant_pressure / specific_heat_capacity_at_constant_volume);
QUANTITY_SPEC(isentropic_exponent,
              volume / pressure * (pressure / volume));  // TODO how to handle "negative" part
inline constexpr auto isentropic_expansion_factor = isentropic_exponent;
QUANTITY_SPEC(entropy, kinetic_energy / thermodynamic_temperature);
QUANTITY_SPEC(specific_entropy, entropy / mass);
QUANTITY_SPEC(enthalpy, energy);           // differs from ISO 80000
QUANTITY_SPEC(internal_energy, enthalpy);  // differs from ISO 80000
inline constexpr auto thermodynamic_energy = internal_energy;
QUANTITY_SPEC(Helmholtz_energy, internal_energy);
inline constexpr auto Helmholtz_function = Helmholtz_energy;
QUANTITY_SPEC(Gibbs_energy, enthalpy);
inline constexpr auto Gibbs_function = Gibbs_energy;
QUANTITY_SPEC(specific_energy, energy / mass);
QUANTITY_SPEC(specific_internal_energy, internal_energy / mass);
inline constexpr auto specific_thermodynamic_energy = specific_internal_energy;
QUANTITY_SPEC(specific_enthalpy, enthalpy / mass);
QUANTITY_SPEC(specific_Helmholtz_energy, Helmholtz_energy / mass);
inline constexpr auto specific_Helmholtz_function = specific_Helmholtz_energy;
QUANTITY_SPEC(specific_Gibbs_energy, Gibbs_energy / mass);
inline constexpr auto specific_Gibbs_function = specific_Gibbs_energy;
QUANTITY_SPEC(Massieu_function, Helmholtz_energy / thermodynamic_temperature);  // TODO how to handle "negative" part
QUANTITY_SPEC(Planck_function, Gibbs_energy / thermodynamic_temperature);       // TODO how to handle "negative" part
QUANTITY_SPEC(Joule_Thomson_coefficient, thermodynamic_temperature / pressure);
QUANTITY_SPEC(thermodynamic_efficiency, dimensionless, work / heat);
QUANTITY_SPEC(maximum_efficiency, thermodynamic_temperature / thermodynamic_temperature);
QUANTITY_SPEC(specific_gas_constant, entropy / mass);
QUANTITY_SPEC(mass_concentration_of_water, mass / volume);
QUANTITY_SPEC(mass_concentration_of_water_vapour, mass / volume);
QUANTITY_SPEC(mass_ratio_of_water_to_dry_matter, mass / mass);
QUANTITY_SPEC(mass_ratio_of_water_vapour_to_dry_gas, mass / mass);
QUANTITY_SPEC(mass_fraction_of_water, mass_ratio_of_water_to_dry_matter / mass_ratio_of_water_to_dry_matter);
QUANTITY_SPEC(mass_fraction_of_dry_matter, mass_fraction_of_water);
QUANTITY_SPEC(relative_humidity, pressure / pressure);
QUANTITY_SPEC(relative_mass_concentration_of_vapour,
              mass_concentration_of_water_vapour / mass_concentration_of_water_vapour);
QUANTITY_SPEC(relative_mass_ratio_of_vapour,
              mass_ratio_of_water_vapour_to_dry_gas / mass_ratio_of_water_vapour_to_dry_gas);
QUANTITY_SPEC(dew_point_temperature, thermodynamic_temperature);

}  // namespace mp_units::isq
