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

#include <mp_units/quantity_spec.h>
#include <mp_units/systems/isq/base_quantities.h>
#include <mp_units/systems/isq/mechanics.h>
#include <mp_units/systems/isq/space_and_time.h>

namespace mp_units::isq {

QUANTITY_SPEC(electric_charge, electric_current* time);
QUANTITY_SPEC(electric_charge_density, electric_charge / volume);
inline constexpr auto volume_electric_charge = electric_charge_density;
QUANTITY_SPEC(surface_density_of_electric_charge, electric_charge / area);
inline constexpr auto areic_electric_charge = surface_density_of_electric_charge;
QUANTITY_SPEC(linear_density_of_electric_charge, electric_charge / length);
inline constexpr auto lineic_electric_charge = linear_density_of_electric_charge;
QUANTITY_SPEC(electric_dipole_moment, electric_charge* position_vector);     // vector
QUANTITY_SPEC(electric_polarization, electric_dipole_moment / volume);       // vector
QUANTITY_SPEC(electric_current_density, electric_charge_density* velocity);  // vector
inline constexpr auto areic_electric_current = electric_current_density;
QUANTITY_SPEC(linear_electric_current_density, surface_density_of_electric_charge* velocity);  // vector
inline constexpr auto lineic_electric_current = linear_electric_current_density;
QUANTITY_SPEC(electric_field_strength, force / electric_charge);  // vector
QUANTITY_SPEC(electric_potential, electric_field_strength* length,
              quantity_character::scalar);  // TODO what is a correct equation here?
QUANTITY_SPEC(electric_potential_difference, electric_potential, quantity_character::scalar);
QUANTITY_SPEC(voltage, electric_potential);
inline constexpr auto electric_tension = voltage;
QUANTITY_SPEC(electric_flux_density, electric_polarization);  // vector
inline constexpr auto electric_displacement = electric_flux_density;
QUANTITY_SPEC(capacitance, electric_charge / voltage);
// TODO how to calculate an argument of a vector product?
QUANTITY_SPEC(magnetic_flux_density, force / (electric_charge * velocity));  // vector
QUANTITY_SPEC(magnetic_vector_potential,
              magnetic_flux_density* length);  // vector // TODO what is a correct equation here?
QUANTITY_SPEC(linked_flux, magnetic_vector_potential* displacement, quantity_character::scalar);
QUANTITY_SPEC(magnetic_constant,
              electric_potential* time / (electric_current * length));  // TODO what is a correct equation here?
inline constexpr auto permeability_of_vacuum = magnetic_constant;
QUANTITY_SPEC(phase_speed_of_electromagnetic_waves, angular_frequency / angular_wavenumber);
QUANTITY_SPEC(speed_of_light, speed);
inline constexpr auto light_speed = speed_of_light;
QUANTITY_SPEC(electric_constant, 1 / (magnetic_constant * pow<2>(speed_of_light)));
inline constexpr auto permittivity_of_vacuum = electric_constant;
QUANTITY_SPEC(permittivity, electric_flux_density / electric_field_strength, quantity_character::scalar);
QUANTITY_SPEC(relative_permittivity, permittivity / electric_constant);
QUANTITY_SPEC(electric_susceptibility, electric_polarization / electric_constant / electric_field_strength,
              quantity_character::scalar);
QUANTITY_SPEC(electric_flux, electric_flux_density* area, quantity_character::scalar);
QUANTITY_SPEC(displacement_current_density, electric_flux_density / time);  // vector
QUANTITY_SPEC(displacement_current, displacement_current_density* area, quantity_character::scalar);
QUANTITY_SPEC(total_current, electric_current);
QUANTITY_SPEC(total_current_density, electric_current_density);  // vector
QUANTITY_SPEC(magnetic_flux, magnetic_flux_density* area, quantity_character::scalar);
QUANTITY_SPEC(magnetic_moment, electric_current* area, quantity_character::vector);
inline constexpr auto magnetic_area_moment = magnetic_moment;
QUANTITY_SPEC(magnetization, magnetic_moment / volume);  // vector
QUANTITY_SPEC(magnetic_field_strength, magnetization);   // vector
inline constexpr auto magnetizing_field = magnetic_field_strength;
QUANTITY_SPEC(permeability, magnetic_flux_density / magnetic_field_strength, quantity_character::scalar);
QUANTITY_SPEC(relative_permeability, permeability / magnetic_constant);
QUANTITY_SPEC(magnetic_susceptibility, magnetization / magnetic_field_strength, quantity_character::scalar);
QUANTITY_SPEC(magnetic_polarization, magnetic_constant* magnetization);     // vector
QUANTITY_SPEC(magnetic_dipole_moment, magnetic_constant* magnetic_moment);  // vector
QUANTITY_SPEC(coercivity, magnetic_field_strength, quantity_character::scalar);
QUANTITY_SPEC(electromagnetic_energy_density, electric_field_strength* electric_flux_density,
              quantity_character::scalar);
inline constexpr auto volumic_electromagnetic_energy = electromagnetic_energy_density;
QUANTITY_SPEC(Poynting_vector, electric_field_strength* magnetic_field_strength);  // vector
QUANTITY_SPEC(source_voltage, voltage);
inline constexpr auto source_tension = source_voltage;
QUANTITY_SPEC(scalar_magnetic_potential, magnetic_field_strength* length,
              quantity_character::scalar);  // TODO what is a correct equation here?
QUANTITY_SPEC(magnetic_tension, magnetic_field_strength* position_vector, quantity_character::scalar);
QUANTITY_SPEC(magnetomotive_force, magnetic_field_strength* position_vector, quantity_character::scalar);
QUANTITY_SPEC(current_linkage, electric_current);
QUANTITY_SPEC(number_of_turns_in_a_winding, dimensionless);
QUANTITY_SPEC(reluctance, magnetic_tension / magnetic_flux);
QUANTITY_SPEC(permeance, 1 / reluctance);
QUANTITY_SPEC(inductance, linked_flux / electric_current);
inline constexpr auto self_inductance = inductance;
QUANTITY_SPEC(mutual_inductance, linked_flux / electric_current);
QUANTITY_SPEC(coupling_factor, mutual_inductance / pow<1, 2>(pow<2>(self_inductance)));
QUANTITY_SPEC(leakage_factor, pow<2>(coupling_factor));
QUANTITY_SPEC(conductivity, electric_current_density / electric_field_strength, quantity_character::scalar);
QUANTITY_SPEC(resistivity, 1 / conductivity);
// QUANTITY_SPEC(power, voltage* electric_current);  // TODO conflicts with mechanical power
// inline constexpr auto instantaneous_power = power;
QUANTITY_SPEC(instantaneous_power, voltage* electric_current);
QUANTITY_SPEC(resistance, voltage / electric_current);
QUANTITY_SPEC(conductance, 1 / resistance);
QUANTITY_SPEC(phase_difference, phase_angle);
QUANTITY_SPEC(electric_current_phasor, electric_current);
QUANTITY_SPEC(voltage_phasor, voltage);
QUANTITY_SPEC(impedance, voltage_phasor / electric_current_phasor);
inline constexpr auto complex_impedance = impedance;
QUANTITY_SPEC(resistance_to_alternating_current, impedance);
QUANTITY_SPEC(reactance, impedance);
QUANTITY_SPEC(modulus_of_impedance, impedance);
QUANTITY_SPEC(admittance, 1 / impedance);
inline constexpr auto complex_admittance = admittance;
QUANTITY_SPEC(conductance_for_alternating_current, admittance);
QUANTITY_SPEC(susceptance, admittance);
QUANTITY_SPEC(modulus_of_admittance, admittance);
QUANTITY_SPEC(quality_factor, reactance / resistance);
QUANTITY_SPEC(loss_factor, 1 / quality_factor);
QUANTITY_SPEC(loss_angle, angular_measure);
QUANTITY_SPEC(active_power, 1 / period * (instantaneous_power * time));
QUANTITY_SPEC(apparent_power, voltage* electric_current);
QUANTITY_SPEC(power_factor, active_power / apparent_power);
QUANTITY_SPEC(complex_power, voltage_phasor* electric_current_phasor);
QUANTITY_SPEC(reactive_power, complex_power);
QUANTITY_SPEC(non_active_power, pow<1, 2>(pow<2>(apparent_power)));
QUANTITY_SPEC(active_energy, instantaneous_power* time);

}  // namespace mp_units::isq
