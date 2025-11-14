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

#include <mp-units/framework.h>
#include <mp-units/systems/iec.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>

namespace {

using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using namespace mp_units::iec::unit_symbols;
using enum mp_units::quantity_character;

[[nodiscard]] consteval bool verify(QuantitySpec auto q, quantity_character ch, Unit auto... units)
{
  return q.character == ch && (... && requires { q[units]; });
}

// space and time
static_assert(verify(isq::length, real_scalar, m));
static_assert(verify(isq::width, real_scalar, m));
static_assert(verify(isq::breadth, real_scalar, m));
static_assert(verify(isq::height, real_scalar, m));
static_assert(verify(isq::depth, real_scalar, m));
static_assert(verify(isq::altitude, real_scalar, m));
static_assert(verify(isq::thickness, real_scalar, m));
static_assert(verify(isq::diameter, real_scalar, m));
static_assert(verify(isq::radius, real_scalar, m));
static_assert(verify(isq::path_length, real_scalar, m));
static_assert(verify(isq::arc_length, real_scalar, m));
static_assert(verify(isq::distance, real_scalar, m));
static_assert(verify(isq::radial_distance, real_scalar, m));
static_assert(verify(isq::position_vector, vector, m));
static_assert(verify(isq::displacement, vector, m));
static_assert(verify(isq::radius_of_curvature, real_scalar, m));
static_assert(verify(isq::curvature, real_scalar, one / m));
static_assert(verify(isq::area, real_scalar, m2));
static_assert(verify(isq::volume, real_scalar, m3));
static_assert(verify(isq::angular_measure, real_scalar, rad, one));
static_assert(verify(isq::rotational_displacement, real_scalar, rad, one));
static_assert(verify(isq::angular_displacement, real_scalar, rad, one));
static_assert(verify(isq::phase_angle, real_scalar, rad, one));
static_assert(verify(isq::solid_angular_measure, real_scalar, sr, one));
static_assert(verify(isq::duration, real_scalar, s));
static_assert(verify(isq::velocity, vector, m / s));
static_assert(verify(isq::speed, real_scalar, m / s));
static_assert(verify(isq::acceleration, vector, m / s2));
static_assert(verify(isq::angular_velocity, vector, rad / s, one / s));
static_assert(verify(isq::angular_acceleration, vector, rad / s2, one / s2));
static_assert(verify(isq::period_duration, real_scalar, s));
static_assert(verify(isq::duration, real_scalar, s));
static_assert(verify(isq::time_constant, real_scalar, s));
static_assert(verify(isq::rotation, real_scalar, one));
static_assert(verify(isq::frequency, real_scalar, Hz, one / s));
static_assert(verify(isq::rotational_frequency, real_scalar, one / s));
static_assert(verify(isq::angular_frequency, real_scalar, rad / s, one / s));
static_assert(verify(isq::wavelength, real_scalar, m));
static_assert(verify(isq::repetency, real_scalar, one / m));
static_assert(verify(isq::wavenumber, real_scalar, one / m));
static_assert(verify(isq::wave_vector, vector, one / m));
static_assert(verify(isq::angular_repetency, real_scalar, one / m));
static_assert(verify(isq::angular_wavenumber, real_scalar, one / m));
static_assert(verify(isq::phase_speed, real_scalar, m / s));
static_assert(verify(isq::group_speed, real_scalar, m / s));
static_assert(verify(isq::damping_coefficient, real_scalar, one / s));
static_assert(verify(isq::logarithmic_decrement, real_scalar, one));
static_assert(verify(isq::attenuation, real_scalar, one / m));
static_assert(verify(isq::extinction, real_scalar, one / m));
static_assert(verify(isq::phase_coefficient, real_scalar, rad / m, one / m));
static_assert(verify(isq::propagation_coefficient, real_scalar, one / m));

// mechanics
static_assert(verify(isq::mass, real_scalar, kg));
static_assert(verify(isq::mass_density, real_scalar, kg / m3));
static_assert(verify(isq::density, real_scalar, kg / m3));
static_assert(verify(isq::specific_volume, real_scalar, m3 / kg));
static_assert(verify(isq::relative_mass_density, real_scalar, one));
static_assert(verify(isq::relative_density, real_scalar, one));
static_assert(verify(isq::surface_mass_density, real_scalar, kg / m2));
static_assert(verify(isq::surface_density, real_scalar, kg / m2));
static_assert(verify(isq::linear_mass_density, real_scalar, kg / m));
static_assert(verify(isq::linear_density, real_scalar, kg / m));
static_assert(verify(isq::moment_of_inertia, tensor, kg* m2));
static_assert(verify(isq::momentum, vector, kg* m / s));
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
static_assert(verify(isq::torque, real_scalar, N* m, kg* m2 / s2));
static_assert(verify(isq::angular_impulse, vector, N* m* s, kg* m2 / s));
static_assert(verify(isq::pressure, real_scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::gauge_pressure, real_scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::stress, tensor, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::normal_stress, real_scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::shear_stress, real_scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::strain, tensor, one));
static_assert(verify(isq::relative_linear_strain, real_scalar, one));
static_assert(verify(isq::shear_strain, real_scalar, one));
static_assert(verify(isq::relative_volume_strain, real_scalar, one));
static_assert(verify(isq::Poisson_number, real_scalar, one));
static_assert(verify(isq::modulus_of_elasticity, real_scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::Young_modulus, real_scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::modulus_of_rigidity, real_scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::shear_modulus, real_scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::modulus_of_compression, real_scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::bulk_modulus, real_scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::compressibility, real_scalar, one / Pa, m* s2 / kg));
static_assert(verify(isq::second_axial_moment_of_area, real_scalar, m4));
static_assert(verify(isq::second_polar_moment_of_area, real_scalar, m4));
static_assert(verify(isq::section_modulus, real_scalar, m3));
static_assert(verify(isq::static_friction_coefficient, real_scalar, one));
static_assert(verify(isq::static_friction_factor, real_scalar, one));
static_assert(verify(isq::coefficient_of_static_friction, real_scalar, one));
static_assert(verify(isq::kinetic_friction_factor, real_scalar, one));
static_assert(verify(isq::dynamic_friction_factor, real_scalar, one));
static_assert(verify(isq::rolling_resistance_factor, real_scalar, one));
static_assert(verify(isq::drag_coefficient, real_scalar, one));
static_assert(verify(isq::drag_factor, real_scalar, one));
static_assert(verify(isq::dynamic_viscosity, real_scalar, Pa* s, kg / m / s));
static_assert(verify(isq::kinematic_viscosity, real_scalar, m2 / s));
static_assert(verify(isq::surface_tension, real_scalar, N / m, kg / s2));
static_assert(verify(isq::power, real_scalar, W, N* m / s, kg* m2 / s3));
static_assert(verify(isq::potential_energy, real_scalar, J, kg* m2 / s2));
static_assert(verify(isq::kinetic_energy, real_scalar, J, kg* m2 / s2));
static_assert(verify(isq::mechanical_energy, real_scalar, J, kg* m2 / s2));
static_assert(verify(isq::mechanical_work, real_scalar, J, kg* m2 / s2));
static_assert(verify(isq::work, real_scalar, J, kg* m2 / s2));
static_assert(verify(isq::mechanical_efficiency, real_scalar, one));
static_assert(verify(isq::mass_flow, vector, kg / m2 / s));
static_assert(verify(isq::mass_flow_rate, real_scalar, kg / s));
static_assert(verify(isq::mass_change_rate, real_scalar, kg / s));
static_assert(verify(isq::volume_flow_rate, real_scalar, m3 / s));
static_assert(verify(isq::action, real_scalar, J* s, kg* m2 / s));

// thermodynamics
static_assert(verify(isq::thermodynamic_temperature, real_scalar, K));
static_assert(verify(isq::Celsius_temperature, real_scalar, deg_C));
static_assert(verify(isq::linear_expansion_coefficient, real_scalar, one / K));
static_assert(verify(isq::cubic_expansion_coefficient, real_scalar, one / K));
static_assert(verify(isq::relative_pressure_coefficient, real_scalar, one / K));
static_assert(verify(isq::pressure_coefficient, real_scalar, Pa / K, kg / m / s2 / K));
static_assert(verify(isq::isothermal_compressibility, real_scalar, one / Pa, m* s2 / kg));
static_assert(verify(isq::isentropic_compressibility, real_scalar, one / Pa, m* s2 / kg));
static_assert(verify(isq::heat, real_scalar, J, kg* m2 / s2));
static_assert(verify(isq::amount_of_heat, real_scalar, J, kg* m2 / s2));
static_assert(verify(isq::latent_heat, real_scalar, J, kg* m2 / s2));
static_assert(verify(isq::heat_flow_rate, real_scalar, W, J / s, kg* m2 / s3));
static_assert(verify(isq::density_of_heat_flow_rate, real_scalar, W / m2, kg / s3));
static_assert(verify(isq::thermal_conductivity, real_scalar, W / (m * K), kg* m / s3 / K));
static_assert(verify(isq::coefficient_of_heat_transfer, real_scalar, W / (m2 * K), kg / s3 / K));
static_assert(verify(isq::surface_coefficient_of_heat_transfer, real_scalar, W / (m2 * K), kg / s3 / K));
static_assert(verify(isq::thermal_insulance, real_scalar, m2* K / W, s3* K / kg));
static_assert(verify(isq::thermal_resistance, real_scalar, K / W, s3* K / kg / m2));
static_assert(verify(isq::thermal_conductance, real_scalar, W / K, kg* m2 / s3 / K));
static_assert(verify(isq::thermal_diffusivity, real_scalar, m2 / s));
static_assert(verify(isq::heat_capacity, real_scalar, J / K, kg* m2 / s2 / K));
static_assert(verify(isq::specific_heat_capacity, real_scalar, J / (kg * K), m2 / s2 / K));
static_assert(verify(isq::specific_heat_capacity_at_constant_pressure, real_scalar, J / (kg * K), m2 / s2 / K));
static_assert(verify(isq::specific_heat_capacity_at_constant_volume, real_scalar, J / (kg * K), m2 / s2 / K));
static_assert(verify(isq::specific_heat_capacity_at_saturated_vapour_pressure, real_scalar, J / (kg * K), m2 / s2 / K));
static_assert(verify(isq::ratio_of_specific_heat_capacities, real_scalar, one));
static_assert(verify(isq::isentropic_exponent, real_scalar, one));
static_assert(verify(isq::isentropic_expansion_factor, real_scalar, one));
static_assert(verify(isq::entropy, real_scalar, J / K, kg* m2 / s2 / K));
static_assert(verify(isq::specific_entropy, real_scalar, J / (kg * K), m2 / s2 / K));
static_assert(verify(isq::energy, real_scalar, J, kg* m2 / s2));
static_assert(verify(isq::internal_energy, real_scalar, J, kg* m2 / s2));
static_assert(verify(isq::thermodynamic_energy, real_scalar, J, kg* m2 / s2));
static_assert(verify(isq::enthalpy, real_scalar, J, kg* m2 / s2));
static_assert(verify(isq::Helmholtz_energy, real_scalar, J, kg* m2 / s2));
static_assert(verify(isq::Helmholtz_function, real_scalar, J, kg* m2 / s2));
static_assert(verify(isq::Gibbs_energy, real_scalar, J, kg* m2 / s2));
static_assert(verify(isq::Gibbs_function, real_scalar, J, kg* m2 / s2));
static_assert(verify(isq::specific_energy, real_scalar, J / kg, m2 / s2));
static_assert(verify(isq::specific_internal_energy, real_scalar, J / kg, m2 / s2));
static_assert(verify(isq::specific_thermodynamic_energy, real_scalar, J / kg, m2 / s2));
static_assert(verify(isq::specific_enthalpy, real_scalar, J / kg, m2 / s2));
static_assert(verify(isq::specific_Helmholtz_energy, real_scalar, J / kg, m2 / s2));
static_assert(verify(isq::specific_Helmholtz_function, real_scalar, J / kg, m2 / s2));
static_assert(verify(isq::specific_Gibbs_energy, real_scalar, J / kg, m2 / s2));
static_assert(verify(isq::specific_Gibbs_function, real_scalar, J / kg, m2 / s2));
static_assert(verify(isq::Massieu_function, real_scalar, J / K, kg* m2 / s2 / K));
static_assert(verify(isq::Planck_function, real_scalar, J / K, kg* m2 / s2 / K));
static_assert(verify(isq::Joule_Thomson_coefficient, real_scalar, K / Pa, m* s2* K / kg));
static_assert(verify(isq::thermodynamic_efficiency, real_scalar, one));
static_assert(verify(isq::maximum_efficiency, real_scalar, one));
static_assert(verify(isq::specific_gas_constant, real_scalar, J / (kg * K), m2 / s2 / K));
static_assert(verify(isq::mass_concentration_of_water, real_scalar, kg / m3));
static_assert(verify(isq::mass_concentration_of_water_vapour, real_scalar, kg / m3));
static_assert(verify(isq::mass_ratio_of_water_to_dry_matter, real_scalar, one));
static_assert(verify(isq::mass_ratio_of_water_vapour_to_dry_gas, real_scalar, one));
static_assert(verify(isq::mass_fraction_of_water, real_scalar, one));
static_assert(verify(isq::mass_fraction_of_dry_matter, real_scalar, one));
static_assert(verify(isq::relative_humidity, real_scalar, one));
static_assert(verify(isq::relative_mass_concentration_of_vapour, real_scalar, one));
static_assert(verify(isq::relative_mass_ratio_of_vapour, real_scalar, one));
static_assert(verify(isq::dew_point_temperature, real_scalar, K));

// electromagnetism
static_assert(verify(isq::electric_current, real_scalar, A));
static_assert(verify(isq::electric_charge, real_scalar, C, A* s));
static_assert(verify(isq::elementary_charge, real_scalar, C, A* s));
static_assert(verify(isq::electric_charge_density, real_scalar, C / m3, s* A / m3));
static_assert(verify(isq::volume_electric_charge, real_scalar, C / m3, s* A / m3));
static_assert(verify(isq::volumic_charge, real_scalar, C / m3, s* A / m3));
static_assert(verify(isq::surface_density_of_electric_charge, real_scalar, C / m2, s* A / m2));
static_assert(verify(isq::areic_electric_charge, real_scalar, C / m2, s* A / m2));
static_assert(verify(isq::areic_charge, real_scalar, C / m2, s* A / m2));
static_assert(verify(isq::linear_density_of_electric_charge, real_scalar, C / m, s* A / m));
static_assert(verify(isq::lineic_electric_charge, real_scalar, C / m, s* A / m));
static_assert(verify(isq::lineic_charge, real_scalar, C / m, s* A / m));
static_assert(verify(isq::electric_dipole_moment, vector, C* m, m* s* A));
static_assert(verify(isq::electric_polarization, vector, C / m2, s* A / m2));
static_assert(verify(isq::electric_current_density, vector, A / m2));
static_assert(verify(isq::linear_electric_current_density, vector, A / m));
static_assert(verify(isq::electric_field_strength, vector, V / m, kg* m / (s3 * A)));
static_assert(verify(isq::electric_potential, real_scalar, V, kg* m2 / (s3 * A)));
static_assert(verify(isq::electric_potential_difference, real_scalar, V, kg* m2 / (s3 * A)));
static_assert(verify(isq::voltage, real_scalar, V, kg* m2 / (s3 * A)));
static_assert(verify(isq::electric_tension, real_scalar, V, kg* m2 / (s3 * A)));
static_assert(verify(isq::induced_voltage, real_scalar, V, kg* m2 / (s3 * A)));
static_assert(verify(isq::electric_flux_density, vector, C / m2, s* A / m2));
static_assert(verify(isq::electric_displacement, vector, C / m2, s* A / m2));
static_assert(verify(isq::capacitance, real_scalar, F, pow<4>(s) * square(A) / (kg * m2)));
static_assert(verify(isq::electric_constant, real_scalar, F / m, pow<4>(s) * square(A) / (kg * m3)));
static_assert(verify(isq::permittivity_of_vacuum, real_scalar, F / m, pow<4>(s) * square(A) / (kg * m3)));
static_assert(verify(isq::permittivity, real_scalar, F / m, pow<4>(s) * square(A) / (kg * m3)));
static_assert(verify(isq::relative_permittivity, real_scalar, one));
static_assert(verify(isq::electric_susceptibility, real_scalar, one));
static_assert(verify(isq::electric_flux, real_scalar, C, s* A));
static_assert(verify(isq::displacement_current_density, vector, A / m2));
static_assert(verify(isq::displacement_current, real_scalar, A));
static_assert(verify(isq::total_current, real_scalar, A));
static_assert(verify(isq::total_current_density, vector, A / m2));
static_assert(verify(isq::magnetic_flux_density, vector, T, kg / (s2 * A)));
static_assert(verify(isq::magnetic_flux, real_scalar, Wb, kg* m2 / (s2 * A)));
static_assert(verify(isq::protoflux, real_scalar, Wb, kg* m2 / (s2 * A)));
static_assert(verify(isq::linked_magnetic_flux, real_scalar, Wb, kg* m2 / (s2 * A)));
static_assert(verify(isq::total_magnetic_flux, real_scalar, Wb, kg* m2 / (s2 * A)));
static_assert(verify(isq::magnetic_moment, vector, A* m2));
static_assert(verify(isq::magnetic_area_moment, vector, A* m2));
static_assert(verify(isq::magnetization, vector, A / m));
static_assert(verify(isq::magnetic_field_strength, vector, A / m));
static_assert(verify(isq::magnetizing_field, vector, A / m));
static_assert(verify(isq::magnetic_constant, real_scalar, H / m, kg* m / (s2 * square(A))));
static_assert(verify(isq::permeability_of_vacuum, real_scalar, H / m, kg* m / (s2 * square(A))));
static_assert(verify(isq::permeability, real_scalar, H / m, kg* m / (s2 * square(A))));
static_assert(verify(isq::relative_permeability, real_scalar, one));
static_assert(verify(isq::magnetic_susceptibility, real_scalar, one));
static_assert(verify(isq::magnetic_polarization, vector, T, Wb / m2, kg / (s2 * A)));
static_assert(verify(isq::magnetic_dipole_moment, vector, Wb* m, kg* m3 / (s2 * A)));
static_assert(verify(isq::coercivity, real_scalar, A / m));
static_assert(verify(isq::coercive_field_strength, real_scalar, A / m));
static_assert(verify(isq::magnetic_vector_potential, vector, J / (A * m), kg* m / (s2 * A)));
static_assert(verify(isq::electromagnetic_energy_density, real_scalar, J / m3, kg / (m * s2)));
static_assert(verify(isq::Poynting_vector, vector, W / m2, kg / s3));
static_assert(verify(isq::phase_speed_of_electromagnetic_waves, real_scalar, m / s));
static_assert(verify(isq::speed_of_light_in_vacuum, real_scalar, m / s));
static_assert(verify(isq::light_speed_in_vacuum, real_scalar, m / s));
static_assert(verify(isq::luminal_speed, real_scalar, m / s));
static_assert(verify(isq::source_voltage, real_scalar, V, kg* m2 / (s3 * A)));
static_assert(verify(isq::source_tension, real_scalar, V, kg* m2 / (s3 * A)));
static_assert(verify(isq::magnetic_potential, real_scalar, A));
static_assert(verify(isq::magnetic_tension, real_scalar, A));
static_assert(verify(isq::magnetomotive_force, real_scalar, A));
static_assert(verify(isq::number_of_turns_in_a_winding, real_scalar, one));
static_assert(verify(isq::reluctance, real_scalar, one / H, s2* square(A) / (kg * m2)));
static_assert(verify(isq::permeance, real_scalar, H, kg* m2 / (s2 * square(A))));
static_assert(verify(isq::inductance, real_scalar, H, kg* m2 / (s2 * square(A))));
static_assert(verify(isq::self_inductance, real_scalar, H, kg* m2 / (s2 * square(A))));
static_assert(verify(isq::mutual_inductance, real_scalar, H, kg* m2 / (s2 * square(A))));
static_assert(verify(isq::coupling_factor, real_scalar, one));
static_assert(verify(isq::leakage_factor, real_scalar, one));
static_assert(verify(isq::conductivity, real_scalar, S / m, s3* square(A) / (kg * m3)));
static_assert(verify(isq::resistivity, real_scalar, Ω* m, kg* m3 / (s3 * square(A))));
static_assert(verify(isq::electromagnetism_power, real_scalar, W, kg* m2 / s3));
static_assert(verify(isq::instantaneous_power, real_scalar, W, kg* m2 / s3));
static_assert(verify(isq::resistance, real_scalar, Ω, kg* m2 / (s3 * square(A))));
static_assert(verify(isq::conductance, real_scalar, S, s3* square(A) / (kg * m2)));
static_assert(verify(isq::phase_difference, real_scalar, rad, one));
static_assert(verify(isq::electric_current_phasor, complex_scalar, A));
static_assert(verify(isq::voltage_phasor, complex_scalar, V, kg* m2 / (s3 * A)));
static_assert(verify(isq::electric_tension_phasor, complex_scalar, V, kg* m2 / (s3 * A)));
static_assert(verify(isq::impedance, complex_scalar, Ω, kg* m2 / (s3 * square(A))));
static_assert(verify(isq::complex_impedance, complex_scalar, Ω, kg* m2 / (s3 * square(A))));
static_assert(verify(isq::impedance_of_vacuum, complex_scalar, V / A, kg* m2 / (s3 * square(A))));
static_assert(verify(isq::wave_impedance_in_vacuum, complex_scalar, V / A, kg* m2 / (s3 * square(A))));
static_assert(verify(isq::resistance_to_alternating_current, real_scalar, Ω, kg* m2 / (s3 * square(A))));
static_assert(verify(isq::reactance, real_scalar, Ω, kg* m2 / (s3 * square(A))));
static_assert(verify(isq::apparent_impedance, real_scalar, Ω, kg* m2 / (s3 * square(A))));
static_assert(verify(isq::admittance, complex_scalar, S, s3* square(A) / (kg * m2)));
static_assert(verify(isq::complex_admittance, complex_scalar, S, s3* square(A) / (kg * m2)));
static_assert(verify(isq::admittance_of_vacuum, complex_scalar, A / V, s3* square(A) / (kg * m2)));
static_assert(verify(isq::conductance_for_alternating_current, real_scalar, S, s3* square(A) / (kg * m2)));
static_assert(verify(isq::susceptance, real_scalar, S, s3* square(A) / (kg * m2)));
static_assert(verify(isq::apparent_admittance, real_scalar, S, s3* square(A) / (kg * m2)));
static_assert(verify(isq::quality_factor, real_scalar, one));
static_assert(verify(isq::loss_factor, real_scalar, one));
static_assert(verify(isq::loss_angle, real_scalar, rad, one));
static_assert(verify(isq::active_power, real_scalar, W, kg* m2 / s3));
static_assert(verify(isq::apparent_power, real_scalar, V* A, kg* m2 / s3));
static_assert(verify(isq::power_factor, real_scalar, one));
static_assert(verify(isq::complex_power, complex_scalar, V* A, kg* m2 / s3));
static_assert(verify(isq::reactive_power, real_scalar, var, V* A, kg* m2 / s3));
static_assert(verify(isq::non_active_power, real_scalar, V* A, kg* m2 / s3));
static_assert(verify(isq::active_energy, real_scalar, J, W* s, kg* m2 / s2));

// light and radiation
static_assert(verify(isq::speed_of_light_in_a_medium, real_scalar, m / s));
static_assert(verify(isq::refractive_index, real_scalar, one));
static_assert(verify(isq::radiant_energy, real_scalar, J, kg* m2 / s2));
static_assert(verify(isq::spectral_radiant_energy, real_scalar, J / nm, kg* m / s2));
static_assert(verify(isq::radiant_energy_density, real_scalar, J / m3, kg / m / s2));
static_assert(verify(isq::spectral_radiant_energy_density_in_terms_of_wavelength, real_scalar, J / (m3 * nm),
                     kg / m2 / s2));
static_assert(verify(isq::spectral_radiant_energy_density_in_terms_of_wavenumber, real_scalar, J / m2, kg / s2));
static_assert(verify(isq::radiant_flux, real_scalar, W, kg* m2 / s3));
static_assert(verify(isq::radiant_power, real_scalar, W, kg* m2 / s3));
static_assert(verify(isq::spectral_radiant_flux, real_scalar, W / nm, kg* m / s3));
static_assert(verify(isq::spectral_radiant_power, real_scalar, W / nm, kg* m / s3));
static_assert(verify(isq::radiant_intensity, real_scalar, W / sr, kg* m2 / s3 / sr));
static_assert(verify(isq::spectral_radiant_intensity, real_scalar, W / (sr * nm), kg* m / s3 / sr));
static_assert(verify(isq::radiance, real_scalar, W / (sr * m2), kg / s3 / sr));
static_assert(verify(isq::spectral_radiance, real_scalar, W / (sr * m2 * nm), kg / m / s3 / sr));
static_assert(verify(isq::irradiance, real_scalar, W / m2, kg / s3));
static_assert(verify(isq::spectral_irradiance, real_scalar, W / (m2 * nm), kg / m / s3));
static_assert(verify(isq::radiant_exitance, real_scalar, W / m2, kg / s3));
static_assert(verify(isq::spectral_radiant_exitance, real_scalar, W / (m2 * nm), kg / m / s3));
static_assert(verify(isq::radiant_exposure, real_scalar, J / m2, kg / s2));
static_assert(verify(isq::spectral_radiant_exposure, real_scalar, J / (m2 * nm), kg / m / s2));

// atomic and nuclear physics
static_assert(verify(isq::activity, real_scalar, Bq, one / s));
static_assert(verify(isq::absorbed_dose, real_scalar, Gy, J / kg, m2 / s2));
static_assert(verify(isq::quality_factor, real_scalar, one));
static_assert(verify(isq::dose_equivalent, real_scalar, Sv, J / kg, m2 / s2));

// information science and technology
static_assert(verify(isq::traffic_intensity, real_scalar, E));
static_assert(verify(isq::traffic_offered_intensity, real_scalar, E));
static_assert(verify(isq::traffic_carried_intensity, real_scalar, E));
static_assert(verify(isq::traffic_load, real_scalar, E));
static_assert(verify(isq::mean_queue_length, real_scalar, one));
static_assert(verify(isq::loss_probability, real_scalar, one));
static_assert(verify(isq::waiting_probability, real_scalar, one));
static_assert(verify(isq::call_intensity, real_scalar, one / s));
static_assert(verify(isq::calling_rate, real_scalar, one / s));
static_assert(verify(isq::completed_call_intensity, real_scalar, one / s));
static_assert(verify(isq::storage_capacity, real_scalar, one, bit, o, B));
static_assert(verify(isq::storage_size, real_scalar, one, bit, o, B));
static_assert(verify(isq::equivalent_binary_storage_capacity, real_scalar, one, bit));
static_assert(verify(isq::transfer_rate, real_scalar, one / s, o / s, B / s));
static_assert(verify(isq::period_of_data_elements, real_scalar, s));
static_assert(verify(isq::binary_digit_rate, real_scalar, one / s, bit / s));
static_assert(verify(isq::bit_rate, real_scalar, one / s, bit / s));
static_assert(verify(isq::period_of_binary_digits, real_scalar, s));
static_assert(verify(isq::bit_period, real_scalar, s));
static_assert(verify(isq::equivalent_binary_digit_rate, real_scalar, one / s, bit / s));
static_assert(verify(isq::equivalent_bit_rate, real_scalar, one / s, bit / s));
static_assert(verify(isq::modulation_rate, real_scalar, one / s, Bd));
static_assert(verify(isq::line_digit_rate, real_scalar, one / s, Bd));
static_assert(verify(isq::quantizing_distortion_power, real_scalar, W));
static_assert(verify(isq::carrier_power, real_scalar, W));
static_assert(verify(isq::signal_energy_per_binary_digit, real_scalar, J));
static_assert(verify(isq::error_probability, real_scalar, one));
static_assert(verify(isq::Hamming_distance, real_scalar, one));
static_assert(verify(isq::clock_frequency, real_scalar, Hz));
static_assert(verify(isq::clock_rate, real_scalar, Hz));
static_assert(verify(isq::decision_content, real_scalar, one));

}  // namespace
