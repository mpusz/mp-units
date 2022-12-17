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

#include <units/isq/isq.h>
#include <units/si/unit_symbols.h>

namespace {

using namespace units;
using namespace units::si::unit_symbols;
using enum units::quantity_character;

[[nodiscard]] consteval bool verify(QuantitySpec auto q, quantity_character ch, Unit auto... units)
{
  return q.character == ch && (... && requires { q[units]; });
}

// space and time
static_assert(verify(isq::length, scalar, m));
static_assert(verify(isq::width, scalar, m));
static_assert(verify(isq::breadth, scalar, m));
static_assert(verify(isq::height, scalar, m));
static_assert(verify(isq::depth, scalar, m));
static_assert(verify(isq::altitude, scalar, m));
static_assert(verify(isq::thickness, scalar, m));
static_assert(verify(isq::diameter, scalar, m));
static_assert(verify(isq::radius, scalar, m));
static_assert(verify(isq::path_length, scalar, m));
static_assert(verify(isq::arc_length, scalar, m));
static_assert(verify(isq::distance, scalar, m));
static_assert(verify(isq::radial_distance, scalar, m));
static_assert(verify(isq::position_vector, vector, m));
static_assert(verify(isq::displacement, vector, m));
static_assert(verify(isq::radius_of_curvature, scalar, m));
static_assert(verify(isq::curvature, scalar, 1 / m));
static_assert(verify(isq::area, scalar, m2));
static_assert(verify(isq::volume, scalar, m3));
static_assert(verify(isq::angular_measure, scalar, rad, one));
static_assert(verify(isq::rotational_displacement, scalar, rad, one));
static_assert(verify(isq::angular_displacement, scalar, rad, one));
static_assert(verify(isq::phase_angle, scalar, rad, one));
static_assert(verify(isq::solid_angular_measure, scalar, sr, one));
static_assert(verify(isq::duration, scalar, s));
static_assert(verify(isq::velocity, vector, m / s));
static_assert(verify(isq::speed, scalar, m / s));
static_assert(verify(isq::acceleration, vector, m / s2));
static_assert(verify(isq::angular_velocity, vector, rad / s, 1 / s));
static_assert(verify(isq::angular_acceleration, vector, rad / s2, 1 / s2));
static_assert(verify(isq::period_duration, scalar, s));
static_assert(verify(isq::duration, scalar, s));
static_assert(verify(isq::time_constant, scalar, s));
static_assert(verify(isq::rotation, scalar, one));
static_assert(verify(isq::frequency, scalar, Hz, 1 / s));
static_assert(verify(isq::rotational_frequency, scalar, 1 / s));
static_assert(verify(isq::angular_frequency, scalar, rad / s, 1 / s));
static_assert(verify(isq::wavelength, scalar, m));
static_assert(verify(isq::repetency, scalar, 1 / m));
static_assert(verify(isq::wavenumber, scalar, 1 / m));
static_assert(verify(isq::wave_vector, vector, 1 / m));
static_assert(verify(isq::angular_repetency, scalar, 1 / m));
static_assert(verify(isq::angular_wavenumber, scalar, 1 / m));
static_assert(verify(isq::phase_velocity, scalar, m / s));
static_assert(verify(isq::phase_speed, scalar, m / s));
static_assert(verify(isq::group_velocity, scalar, m / s));
static_assert(verify(isq::group_speed, scalar, m / s));
static_assert(verify(isq::damping_coefficient, scalar, 1 / s));
static_assert(verify(isq::logarithmic_decrement, scalar, one));
static_assert(verify(isq::attenuation, scalar, 1 / m));
static_assert(verify(isq::extinction, scalar, 1 / m));
static_assert(verify(isq::phase_coefficient, scalar, rad / m, 1 / m));
static_assert(verify(isq::propagation_coefficient, scalar, 1 / m));

// mechanics
static_assert(verify(isq::mass, scalar, kg));
static_assert(verify(isq::mass_density, scalar, kg / m3));
static_assert(verify(isq::density, scalar, kg / m3));
static_assert(verify(isq::specific_volume, scalar, m3 / kg));
static_assert(verify(isq::relative_mass_density, scalar, one));
static_assert(verify(isq::relative_density, scalar, one));
static_assert(verify(isq::surface_mass_density, scalar, kg / m2));
static_assert(verify(isq::surface_density, scalar, kg / m2));
static_assert(verify(isq::linear_mass_density, scalar, kg / m));
static_assert(verify(isq::linear_density, scalar, kg / m));
static_assert(verify(isq::moment_of_inertia, tensor, kg* m2));
static_assert(verify(isq::momentum, vector, kg* m / s));  // TODO vector?
static_assert(verify(isq::force, vector, N, kg* m / s2));
static_assert(verify(isq::weight, vector, N, kg* m / s2));
static_assert(verify(isq::static_friction_force, vector, N, kg* m / s2));
static_assert(verify(isq::static_friction, vector, N, kg* m / s2));
static_assert(verify(isq::kinetic_friction_force, vector, N, kg* m / s2));
static_assert(verify(isq::dynamic_friction_force, vector, N, kg* m / s2));
static_assert(verify(isq::rolling_resistance, vector, N, kg* m / s2));
static_assert(verify(isq::rolling_drag, vector, N, kg* m / s2));
static_assert(verify(isq::rolling_friction_force, vector, N, kg* m / s2));
static_assert(verify(isq::drag_force, vector, N, kg* m / s2));
static_assert(verify(isq::impulse, vector, N* s, kg* m / s));
static_assert(verify(isq::angular_momentum, vector, kg* m2 / s));
static_assert(verify(isq::moment_of_force, vector, N* m, kg* m2 / s2));
static_assert(verify(isq::torque, scalar, N* m, kg* m2 / s2));
static_assert(verify(isq::angular_impulse, vector, N* m* s, kg* m2 / s));
static_assert(verify(isq::pressure, scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::gauge_pressure, scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::stress, tensor, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::normal_stress, scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::shear_stress, scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::strain, tensor, one));
static_assert(verify(isq::relative_linear_strain, scalar, one));
static_assert(verify(isq::shear_strain, vector, one));  // TODO scalar or vector?
static_assert(verify(isq::relative_volume_strain, scalar, one));
static_assert(verify(isq::Poisson_number, scalar, one));
static_assert(verify(isq::modulus_of_elasticity, scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::Young_modulus, scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::modulus_of_rigidity, scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::shear_modulus, scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::modulus_of_compression, scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::bulk_modulus, scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::compressibility, scalar, 1 / Pa, m* s2 / kg));
static_assert(verify(isq::second_axial_moment_of_area, scalar, m4));
static_assert(verify(isq::second_polar_moment_of_area, scalar, m4));
static_assert(verify(isq::section_modulus, scalar, m3));
static_assert(verify(isq::static_friction_coefficient, scalar, one));
static_assert(verify(isq::static_friction_factor, scalar, one));
static_assert(verify(isq::coefficient_of_static_friction, scalar, one));
static_assert(verify(isq::kinetic_friction_factor, scalar, one));
static_assert(verify(isq::dynamic_friction_factor, scalar, one));
static_assert(verify(isq::rolling_resistance_factor, scalar, one));
static_assert(verify(isq::drag_coefficient, vector, one));
static_assert(verify(isq::drag_factor, vector, one));
static_assert(verify(isq::dynamic_viscosity, scalar, Pa* s, kg / m / s));
static_assert(verify(isq::kinematic_viscosity, scalar, m2 / s));
static_assert(verify(isq::surface_tension, scalar, N / m, kg / s2));
static_assert(verify(isq::power, scalar, W, N* m / s, kg* m2 / s3));
static_assert(verify(isq::potential_energy, scalar, J, kg* m2 / s2));
static_assert(verify(isq::kinetic_energy, scalar, J, kg* m2 / s2));
static_assert(verify(isq::mechanical_energy, scalar, J, kg* m2 / s2));
static_assert(verify(isq::mechanical_work, scalar, J, kg* m2 / s2));
static_assert(verify(isq::work, scalar, J, kg* m2 / s2));
static_assert(verify(isq::efficiency, scalar, one));
static_assert(verify(isq::mass_flow, vector, kg / m2 / s));
static_assert(verify(isq::mass_flow_rate, scalar, kg / s));
static_assert(verify(isq::mass_change_rate, scalar, kg / s));
static_assert(verify(isq::volume_flow_rate, scalar, m3 / s));
static_assert(verify(isq::action, scalar, J* s, kg* m2 / s));

}  // namespace
