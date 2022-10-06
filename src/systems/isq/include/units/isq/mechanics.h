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

#include <units/dimension.h>
#include <units/isq/base_dimensions.h>
#include <units/isq/space_and_time.h>

namespace units::isq {

// clang-format off
// inline constexpr struct mass_dim : base_dimension<"M"> {} mass_dim;
inline constexpr struct mass_density_dim : decltype(mass_dim / volume_dim) {} mass_density_dim;
inline constexpr struct specific_volume_dim : decltype(1 / mass_density_dim) {} specific_volume_dim;
inline constexpr struct relative_mass_density_dim : decltype(mass_density_dim / mass_density_dim) {} relative_mass_density_dim;
inline constexpr struct surface_mass_density_dim : decltype(mass_dim / area_dim) {} surface_mass_density_dim;
inline constexpr struct linear_mass_density_dim : decltype(mass_dim / length_dim) {} linear_mass_density_dim;
inline constexpr struct momentum_dim : decltype(mass_dim * speed_dim) {} momentum_dim;  // TODO velocity_dim?
inline constexpr struct force_dim : decltype(mass_dim * acceleration_dim) {} force_dim;  // TODO what is a correct equation here?
// inline constexpr struct weight_dim : decltype(mass_dim * acceleration_dim) {} weight_dim;  // TODO should we add it as a quantity or should it be a quantity_kind?
// TODO Should we add other forces as well: static_friction_force, kinematic_friction_force, rolling_resistance, drag_force
inline constexpr struct impulse_dim : decltype(force_dim / time_dim) {} impulse_dim;
inline constexpr struct angular_momentum_dim : decltype(length_dim * momentum_dim) {} angular_momentum_dim; // TODO position_vector
inline constexpr struct moment_of_inertia_dim : decltype(angular_momentum_dim * angular_velocity_dim) {} moment_of_inertia_dim;
inline constexpr struct moment_of_force_dim : decltype(length_dim * force_dim) {} moment_of_force_dim; // TODO position_vector
inline constexpr struct torque_dim : decltype(moment_of_force_dim) {} torque_dim; // TODO angle?
inline constexpr struct angular_impulse_dim : decltype(moment_of_force_dim * time_dim) {} angular_impulse_dim;
inline constexpr struct pressure_dim : decltype(force_dim / area_dim) {} pressure_dim;
inline constexpr struct stress_dim : decltype(pressure_dim) {} stress_dim; // TODO tensor?
inline constexpr struct normal_stress_dim : decltype(force_dim / area_dim) {} normal_stress_dim;
inline constexpr struct strain_dim : decltype(stress_dim / stress_dim) {} strain_dim; // TODO what is a correct equation here?
inline constexpr struct poisson_number_dim : decltype(length_dim / length_dim) {} poisson_number_dim; // TODO width?
// TODO modulus quantities
inline constexpr struct compressibility_dim : decltype(volume_dim / pressure_dim) {} compressibility_dim;
inline constexpr struct second_axial_moment_of_area_dim : decltype(area_dim * area_dim) {} second_axial_moment_of_area_dim;  // TODO what is a correct equation here?
inline constexpr struct section_modulus_dim : decltype(second_axial_moment_of_area_dim / length_dim) {} section_modulus_dim;  // TODO radial distance
// TODO friction coefficients?
inline constexpr struct dynamic_viscosity_dim : decltype(stress_dim * length_dim / speed_dim) {} dynamic_viscosity_dim;  // TODO shear stress, velocity
inline constexpr struct kinematic_viscosity_dim : decltype(dynamic_viscosity_dim / mass_density_dim) {} kinematic_viscosity_dim;
inline constexpr struct surface_tension_dim : decltype(force_dim / length_dim) {} surface_tension_dim;  // TODO what is a correct equation here?
inline constexpr struct power_dim : decltype(force_dim * speed_dim) {} power_dim;
// TODO what about energy (potential and kinetic as separate quantities will prevent an equation for mechanical one, is it expected?)
inline constexpr struct efficiency_dim : decltype(power_dim / power_dim) {} efficiency_dim;
inline constexpr struct mass_flow_dim : decltype(mass_density_dim * speed_dim) {} mass_flow_dim; // TODO velocity
inline constexpr struct mass_flow_rate_dim : decltype(mass_flow_dim * area_dim) {} mass_flow_rate_dim;
inline constexpr struct mass_change_rate_dim : decltype(mass_dim / time_dim) {} mass_change_rate_dim;
inline constexpr struct volume_flow_rate_dim : decltype(speed_dim * area_dim) {} volume_flow_rate_dim;  // TODO velocity
// inline constexpr struct action_dim : decltype(energy_dim * time_dim) {} action_dim;  // TODO make it compile

// clang-format on

}  // namespace units::isq
