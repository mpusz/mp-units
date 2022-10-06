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
#include <units/reference.h>
#include <units/si/units.h>

namespace units::si {

// clang-format off
inline constexpr struct mass : system_reference<isq::mass_dim, kilogram> {} mass;
inline constexpr struct mass_density : system_reference<isq::mass_density_dim, kilogram / cubic<metre>> {} mass_density;
inline constexpr struct specific_volume : system_reference<isq::specific_volume_dim, cubic<metre> / kilogram> {} specific_volume;
inline constexpr struct relative_mass_density : system_reference<isq::relative_mass_density_dim, one> {} relative_mass_density;
inline constexpr struct surface_mass_density : system_reference<isq::surface_mass_density_dim, kilogram / square<metre>> {} surface_mass_density;
inline constexpr struct linear_mass_density : system_reference<isq::linear_mass_density_dim, kilogram / metre> {} linear_mass_density;
inline constexpr struct momentum : system_reference<isq::momentum_dim, kilogram * metre / second> {} momentum;
inline constexpr struct force : system_reference<isq::force_dim, newton> {} force;
// inline constexpr struct weight : system_reference<isq::weight_dim, newton> {} weight;
inline constexpr struct impulse : system_reference<isq::impulse_dim, newton * second> {} impulse;
inline constexpr struct angular_momentum : system_reference<isq::angular_momentum_dim, kilogram * square<metre> / second> {} angular_momentum;  // TODO radian?
inline constexpr struct moment_of_inertia : system_reference<isq::moment_of_inertia_dim, kilogram * square<metre>> {} moment_of_inertia;
inline constexpr struct moment_of_force : system_reference<isq::moment_of_force_dim, newton * metre> {} moment_of_force;
inline constexpr struct torque : system_reference<isq::torque_dim, newton * metre> {} torque;
inline constexpr struct angular_impulse : system_reference<isq::angular_impulse_dim, newton * metre * second> {} angular_impulse;
inline constexpr struct pressure : system_reference<isq::pressure_dim, pascal> {} pressure;
inline constexpr struct stress : system_reference<isq::stress_dim, pascal> {} stress;
inline constexpr struct normal_stress : system_reference<isq::normal_stress_dim, pascal> {} normal_stress;
inline constexpr struct strain : system_reference<isq::strain_dim, one> {} strain;
inline constexpr struct poisson_number : system_reference<isq::poisson_number_dim, one> {} poisson_number;
inline constexpr struct compressibility : system_reference<isq::compressibility_dim, 1 / pascal> {} compressibility;
inline constexpr struct second_axial_moment_of_area : system_reference<isq::second_axial_moment_of_area_dim, square<metre> * square<metre>> {} second_axial_moment_of_area;
inline constexpr struct section_modulus : system_reference<isq::section_modulus_dim, cubic<metre>> {} section_modulus;
inline constexpr struct dynamic_viscosity : system_reference<isq::dynamic_viscosity_dim, pascal * second> {} dynamic_viscosity;
inline constexpr struct kinematic_viscosity : system_reference<isq::kinematic_viscosity_dim, square<metre> / second> {} kinematic_viscosity;
inline constexpr struct surface_tension : system_reference<isq::surface_tension_dim, newton / metre> {} surface_tension;
inline constexpr struct power : system_reference<isq::power_dim, watt> {} power;
inline constexpr struct efficiency : system_reference<isq::efficiency_dim, one> {} efficiency;
inline constexpr struct mass_flow : system_reference<isq::mass_flow_dim, kilogram / (square<metre> * second)> {} mass_flow;
inline constexpr struct mass_flow_rate : system_reference<isq::mass_flow_rate_dim, kilogram / second> {} mass_flow_rate;
inline constexpr struct mass_change_rate : system_reference<isq::mass_change_rate_dim, kilogram / second> {} mass_change_rate;
inline constexpr struct volume_flow_rate : system_reference<isq::volume_flow_rate_dim, cubic<metre> / second> {} volume_flow_rate;
// inline constexpr struct action : system_reference<isq::action_dim, joule* second> {} action;  // TODO make it compile
// clang-format on

}  // namespace units::si
