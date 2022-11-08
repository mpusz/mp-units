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

// inline constexpr struct mass : base_dimension<"M"> {} mass;
DERIVED_DIMENSION(mass_density, decltype(mass / volume));
DERIVED_DIMENSION(specific_volume, decltype(1 / mass_density));
DERIVED_DIMENSION(relative_mass_density, decltype(mass_density / mass_density));
DERIVED_DIMENSION(surface_mass_density, decltype(mass / area));
DERIVED_DIMENSION(linear_mass_density, decltype(mass / length));
DERIVED_DIMENSION(momentum, decltype(mass * speed));      // TODO velocity?
DERIVED_DIMENSION(force, decltype(mass * acceleration));  // TODO what is a correct equation here?
// DERIVED_DIMENSION(weight, decltype(mass * acceleration));  // TODO should we add it as a quantity or should it be a
// quantity_kind?
// TODO Should we add other forces as well: static_friction_force, kinematic_friction_force, rolling_resistance,
// drag_force
DERIVED_DIMENSION(impulse, decltype(force / time));
DERIVED_DIMENSION(angular_momentum, decltype(length * momentum));  // TODO position_vector
DERIVED_DIMENSION(moment_of_inertia, decltype(angular_momentum * angular_velocity));
DERIVED_DIMENSION(moment_of_force, decltype(length * force));  // TODO position_vector
DERIVED_DIMENSION(torque, decltype(moment_of_force));          // TODO angle?
DERIVED_DIMENSION(angular_impulse, decltype(moment_of_force * time));
DERIVED_DIMENSION(pressure, decltype(force / area));
DERIVED_DIMENSION(stress, decltype(pressure));  // TODO tensor?
DERIVED_DIMENSION(normal_stress, decltype(force / area));
DERIVED_DIMENSION(strain, decltype(stress / stress));          // TODO what is a correct equation here?
DERIVED_DIMENSION(poisson_number, decltype(length / length));  // TODO width?
// TODO modulus quantities
DERIVED_DIMENSION(compressibility, decltype(volume / volume / pressure));
DERIVED_DIMENSION(second_axial_moment_of_area, decltype(area * area));  // TODO what is a correct equation here?
DERIVED_DIMENSION(section_modulus, decltype(second_axial_moment_of_area / length));  // TODO radial distance
// TODO friction coefficients?
DERIVED_DIMENSION(dynamic_viscosity, decltype(stress * length / speed));  // TODO shear stress, velocity
DERIVED_DIMENSION(kinematic_viscosity, decltype(dynamic_viscosity / mass_density));
DERIVED_DIMENSION(surface_tension, decltype(force / length));  // TODO what is a correct equation here?
DERIVED_DIMENSION(power, decltype(force * speed));
// TODO what about energy (potential and kinetic as separate quantities will prevent an equation for mechanical one, is
// it expected?)
DERIVED_DIMENSION(efficiency, decltype(power / power));
DERIVED_DIMENSION(mass_flow, decltype(mass_density * speed));  // TODO velocity
DERIVED_DIMENSION(mass_flow_rate, decltype(mass_flow * area));
DERIVED_DIMENSION(mass_change_rate, decltype(mass / time));
DERIVED_DIMENSION(volume_flow_rate, decltype(speed * area));  // TODO velocity
// DERIVED_DIMENSION(action, decltype(energy * time));  // TODO make it compile

}  // namespace units::isq
