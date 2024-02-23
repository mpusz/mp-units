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
#include <mp-units/systems/isq/si_quantities.h>
#include <mp-units/systems/isq/space_and_time.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/quantity_spec.h>
#endif

MP_UNITS_EXPORT
namespace mp_units::isq {

QUANTITY_SPEC(mass_density, mass / volume);
inline constexpr auto density = mass_density;
QUANTITY_SPEC(specific_volume, inverse(mass_density));
QUANTITY_SPEC(relative_mass_density, mass_density / mass_density);
inline constexpr auto relative_density = relative_mass_density;
QUANTITY_SPEC(surface_mass_density, mass / area);
inline constexpr auto surface_density = surface_mass_density;
QUANTITY_SPEC(linear_mass_density, mass / length);
inline constexpr auto linear_density = linear_mass_density;
QUANTITY_SPEC(momentum, mass* velocity);                        // vector
QUANTITY_SPEC(force, mass* acceleration);                       // vector  // TODO what is a correct equation here?
QUANTITY_SPEC(weight, force, mass* acceleration_of_free_fall);  // vector  // differs from ISO 80000
QUANTITY_SPEC(static_friction_force, force);                    // vector
inline constexpr auto static_friction = static_friction_force;
QUANTITY_SPEC(kinetic_friction_force, force);  // vector
inline constexpr auto dynamic_friction_force = kinetic_friction_force;
QUANTITY_SPEC(rolling_resistance, force);  // vector
inline constexpr auto rolling_drag = rolling_resistance;
inline constexpr auto rolling_friction_force = rolling_resistance;
QUANTITY_SPEC(drag_force, force);                            // vector
QUANTITY_SPEC(impulse, force* time);                         // vector
QUANTITY_SPEC(angular_momentum, position_vector* momentum);  // vector
QUANTITY_SPEC(moment_of_inertia, angular_momentum / angular_velocity, quantity_character::tensor);
QUANTITY_SPEC(moment_of_force, position_vector* force);  // vector
QUANTITY_SPEC(torque, moment_of_force, quantity_character::scalar);
QUANTITY_SPEC(angular_impulse, moment_of_force* time);  // vector
QUANTITY_SPEC(pressure, force / area, quantity_character::scalar);
QUANTITY_SPEC(gauge_pressure, pressure);
QUANTITY_SPEC(stress, pressure, quantity_character::tensor);
QUANTITY_SPEC(normal_stress, pressure, quantity_character::scalar);
QUANTITY_SPEC(shear_stress, pressure, quantity_character::scalar);
QUANTITY_SPEC(strain, dimensionless, quantity_character::tensor);
QUANTITY_SPEC(relative_linear_strain, length / length);
QUANTITY_SPEC(shear_strain, dimensionless, displacement / thickness, quantity_character::scalar);
QUANTITY_SPEC(relative_volume_strain, volume / volume);
QUANTITY_SPEC(Poisson_number, dimensionless, width / length);
QUANTITY_SPEC(modulus_of_elasticity, normal_stress / relative_linear_strain);
inline constexpr auto Young_modulus = modulus_of_elasticity;
QUANTITY_SPEC(modulus_of_rigidity, shear_stress / shear_strain);
inline constexpr auto shear_modulus = modulus_of_rigidity;
QUANTITY_SPEC(modulus_of_compression, pressure / relative_volume_strain);
inline constexpr auto bulk_modulus = modulus_of_compression;
QUANTITY_SPEC(compressibility, inverse(volume) * (volume / pressure));
QUANTITY_SPEC(second_axial_moment_of_area, pow<2>(radial_distance) * area);
QUANTITY_SPEC(second_polar_moment_of_area, pow<2>(radial_distance) * area);
QUANTITY_SPEC(section_modulus, second_axial_moment_of_area / radial_distance);
QUANTITY_SPEC(static_friction_coefficient, dimensionless, static_friction_force / force, quantity_character::scalar);
inline constexpr auto static_friction_factor = static_friction_coefficient;
inline constexpr auto coefficient_of_static_friction = static_friction_coefficient;
QUANTITY_SPEC(kinetic_friction_factor, dimensionless, kinetic_friction_force / force, quantity_character::scalar);
inline constexpr auto dynamic_friction_factor = kinetic_friction_factor;
QUANTITY_SPEC(rolling_resistance_factor, force / force, quantity_character::scalar);
QUANTITY_SPEC(drag_coefficient, dimensionless, drag_force / (mass_density * pow<2>(speed) * area),
              quantity_character::scalar);
inline constexpr auto drag_factor = drag_coefficient;
QUANTITY_SPEC(dynamic_viscosity, shear_stress* length / velocity, quantity_character::scalar);
QUANTITY_SPEC(kinematic_viscosity, dynamic_viscosity / mass_density);
QUANTITY_SPEC(surface_tension, force / length, quantity_character::scalar);  // TODO what is a correct equation here?
QUANTITY_SPEC(power, mass* pow<2>(length) / pow<3>(time));                   // not in ISO 80000
QUANTITY_SPEC(mechanical_power, power, force* velocity, quantity_character::scalar);
QUANTITY_SPEC(mechanical_energy, energy);                               // differs from ISO 80000
QUANTITY_SPEC(potential_energy, mechanical_energy);                     // differs from ISO 80000
QUANTITY_SPEC(kinetic_energy, mechanical_energy, mass* pow<2>(speed));  // differs from ISO 80000
QUANTITY_SPEC(mechanical_work, force* displacement, quantity_character::scalar);
inline constexpr auto work = mechanical_work;
QUANTITY_SPEC(mechanical_efficiency, mechanical_power / mechanical_power);
QUANTITY_SPEC(mass_flow, mass_density* velocity);  // vector
QUANTITY_SPEC(mass_flow_rate, mass_flow* area, quantity_character::scalar);
QUANTITY_SPEC(mass_change_rate, mass / time);
QUANTITY_SPEC(volume_flow_rate, velocity* area, quantity_character::scalar);
QUANTITY_SPEC(action, energy* time);

}  // namespace mp_units::isq
