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

#include <mp-units/reference.h>
#include <mp-units/systems/isq/isq.h>
#include <mp-units/systems/si/unit_symbols.h>

namespace {

using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using enum mp_units::quantity_character;

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
static_assert(verify(isq::curvature, scalar, one / m));
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
static_assert(verify(isq::angular_velocity, vector, rad / s, one / s));
static_assert(verify(isq::angular_acceleration, vector, rad / s2, one / s2));
static_assert(verify(isq::period_duration, scalar, s));
static_assert(verify(isq::duration, scalar, s));
static_assert(verify(isq::time_constant, scalar, s));
static_assert(verify(isq::rotation, scalar, one));
static_assert(verify(isq::frequency, scalar, Hz, one / s));
static_assert(verify(isq::rotational_frequency, scalar, one / s));
static_assert(verify(isq::angular_frequency, scalar, rad / s, one / s));
static_assert(verify(isq::wavelength, scalar, m));
static_assert(verify(isq::repetency, scalar, one / m));
static_assert(verify(isq::wavenumber, scalar, one / m));
static_assert(verify(isq::wave_vector, vector, one / m));
static_assert(verify(isq::angular_repetency, scalar, one / m));
static_assert(verify(isq::angular_wavenumber, scalar, one / m));
static_assert(verify(isq::phase_velocity, scalar, m / s));
static_assert(verify(isq::phase_speed, scalar, m / s));
static_assert(verify(isq::group_velocity, scalar, m / s));
static_assert(verify(isq::group_speed, scalar, m / s));
static_assert(verify(isq::damping_coefficient, scalar, one / s));
static_assert(verify(isq::logarithmic_decrement, scalar, one));
static_assert(verify(isq::attenuation, scalar, one / m));
static_assert(verify(isq::extinction, scalar, one / m));
static_assert(verify(isq::phase_coefficient, scalar, rad / m, one / m));
static_assert(verify(isq::propagation_coefficient, scalar, one / m));

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
static_assert(verify(isq::torque, scalar, N* m, kg* m2 / s2));
static_assert(verify(isq::angular_impulse, vector, N* m* s, kg* m2 / s));
static_assert(verify(isq::pressure, scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::gauge_pressure, scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::stress, tensor, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::normal_stress, scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::shear_stress, scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::strain, tensor, one));
static_assert(verify(isq::relative_linear_strain, scalar, one));
static_assert(verify(isq::shear_strain, scalar, one));
static_assert(verify(isq::relative_volume_strain, scalar, one));
static_assert(verify(isq::Poisson_number, scalar, one));
static_assert(verify(isq::modulus_of_elasticity, scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::Young_modulus, scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::modulus_of_rigidity, scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::shear_modulus, scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::modulus_of_compression, scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::bulk_modulus, scalar, Pa, N / m2, kg / m / s2));
static_assert(verify(isq::compressibility, scalar, one / Pa, m* s2 / kg));
static_assert(verify(isq::second_axial_moment_of_area, scalar, m4));
static_assert(verify(isq::second_polar_moment_of_area, scalar, m4));
static_assert(verify(isq::section_modulus, scalar, m3));
static_assert(verify(isq::static_friction_coefficient, scalar, one));
static_assert(verify(isq::static_friction_factor, scalar, one));
static_assert(verify(isq::coefficient_of_static_friction, scalar, one));
static_assert(verify(isq::kinetic_friction_factor, scalar, one));
static_assert(verify(isq::dynamic_friction_factor, scalar, one));
static_assert(verify(isq::rolling_resistance_factor, scalar, one));
static_assert(verify(isq::drag_coefficient, scalar, one));
static_assert(verify(isq::drag_factor, scalar, one));
static_assert(verify(isq::dynamic_viscosity, scalar, Pa* s, kg / m / s));
static_assert(verify(isq::kinematic_viscosity, scalar, m2 / s));
static_assert(verify(isq::surface_tension, scalar, N / m, kg / s2));
static_assert(verify(isq::power, scalar, W, N* m / s, kg* m2 / s3));
static_assert(verify(isq::potential_energy, scalar, J, kg* m2 / s2));
static_assert(verify(isq::kinetic_energy, scalar, J, kg* m2 / s2));
static_assert(verify(isq::mechanical_energy, scalar, J, kg* m2 / s2));
static_assert(verify(isq::mechanical_work, scalar, J, kg* m2 / s2));
static_assert(verify(isq::work, scalar, J, kg* m2 / s2));
static_assert(verify(isq::mechanical_efficiency, scalar, one));
static_assert(verify(isq::mass_flow, vector, kg / m2 / s));
static_assert(verify(isq::mass_flow_rate, scalar, kg / s));
static_assert(verify(isq::mass_change_rate, scalar, kg / s));
static_assert(verify(isq::volume_flow_rate, scalar, m3 / s));
static_assert(verify(isq::action, scalar, J* s, kg* m2 / s));

// thermodynamics
static_assert(verify(isq::thermodynamic_temperature, scalar, K));
static_assert(verify(isq::Celsius_temperature, scalar, deg_C));
static_assert(verify(isq::linear_expansion_coefficient, scalar, one / K));
static_assert(verify(isq::cubic_expansion_coefficient, scalar, one / K));
static_assert(verify(isq::relative_pressure_coefficient, scalar, one / K));
static_assert(verify(isq::pressure_coefficient, scalar, Pa / K, kg / m / s2 / K));
static_assert(verify(isq::isothermal_compressibility, scalar, one / Pa, m* s2 / kg));
static_assert(verify(isq::isentropic_compressibility, scalar, one / Pa, m* s2 / kg));
static_assert(verify(isq::heat, scalar, J, kg* m2 / s2));
static_assert(verify(isq::amount_of_heat, scalar, J, kg* m2 / s2));
static_assert(verify(isq::latent_heat, scalar, J, kg* m2 / s2));
static_assert(verify(isq::heat_flow_rate, scalar, W, J / s, kg* m2 / s3));
static_assert(verify(isq::density_of_heat_flow_rate, scalar, W / m2, kg / s3));
static_assert(verify(isq::thermal_conductivity, scalar, W / (m * K), kg* m / s3 / K));
static_assert(verify(isq::coefficient_of_heat_transfer, scalar, W / (m2 * K), kg / s3 / K));
static_assert(verify(isq::surface_coefficient_of_heat_transfer, scalar, W / (m2 * K), kg / s3 / K));
static_assert(verify(isq::thermal_insulance, scalar, m2* K / W, s3* K / kg));
static_assert(verify(isq::thermal_resistance, scalar, K / W, s3* K / kg / m2));
static_assert(verify(isq::thermal_conductance, scalar, W / K, kg* m2 / s3 / K));
static_assert(verify(isq::thermal_diffusivity, scalar, m2 / s));
static_assert(verify(isq::heat_capacity, scalar, J / K, kg* m2 / s2 / K));
static_assert(verify(isq::specific_heat_capacity, scalar, J / (kg * K), m2 / s2 / K));
static_assert(verify(isq::specific_heat_capacity_at_constant_pressure, scalar, J / (kg * K), m2 / s2 / K));
static_assert(verify(isq::specific_heat_capacity_at_constant_volume, scalar, J / (kg * K), m2 / s2 / K));
static_assert(verify(isq::specific_heat_capacity_at_saturated_vapour_pressure, scalar, J / (kg * K), m2 / s2 / K));
static_assert(verify(isq::ratio_of_specific_heat_capacities, scalar, one));
static_assert(verify(isq::isentropic_exponent, scalar, one));
static_assert(verify(isq::isentropic_expansion_factor, scalar, one));
static_assert(verify(isq::entropy, scalar, J / K, kg* m2 / s2 / K));
static_assert(verify(isq::specific_entropy, scalar, J / (kg * K), m2 / s2 / K));
static_assert(verify(isq::energy, scalar, J, kg* m2 / s2));
static_assert(verify(isq::internal_energy, scalar, J, kg* m2 / s2));
static_assert(verify(isq::thermodynamic_energy, scalar, J, kg* m2 / s2));
static_assert(verify(isq::enthalpy, scalar, J, kg* m2 / s2));
static_assert(verify(isq::Helmholtz_energy, scalar, J, kg* m2 / s2));
static_assert(verify(isq::Helmholtz_function, scalar, J, kg* m2 / s2));
static_assert(verify(isq::Gibbs_energy, scalar, J, kg* m2 / s2));
static_assert(verify(isq::Gibbs_function, scalar, J, kg* m2 / s2));
static_assert(verify(isq::specific_energy, scalar, J / kg, m2 / s2));
static_assert(verify(isq::specific_internal_energy, scalar, J / kg, m2 / s2));
static_assert(verify(isq::specific_thermodynamic_energy, scalar, J / kg, m2 / s2));
static_assert(verify(isq::specific_enthalpy, scalar, J / kg, m2 / s2));
static_assert(verify(isq::specific_Helmholtz_energy, scalar, J / kg, m2 / s2));
static_assert(verify(isq::specific_Helmholtz_function, scalar, J / kg, m2 / s2));
static_assert(verify(isq::specific_Gibbs_energy, scalar, J / kg, m2 / s2));
static_assert(verify(isq::specific_Gibbs_function, scalar, J / kg, m2 / s2));
static_assert(verify(isq::Massieu_function, scalar, J / K, kg* m2 / s2 / K));
static_assert(verify(isq::Planck_function, scalar, J / K, kg* m2 / s2 / K));
static_assert(verify(isq::Joule_Thomson_coefficient, scalar, K / Pa, m* s2* K / kg));
static_assert(verify(isq::thermodynamic_efficiency, scalar, one));
static_assert(verify(isq::maximum_efficiency, scalar, one));
static_assert(verify(isq::specific_gas_constant, scalar, J / (kg * K), m2 / s2 / K));
static_assert(verify(isq::mass_concentration_of_water, scalar, kg / m3));
static_assert(verify(isq::mass_concentration_of_water_vapour, scalar, kg / m3));
static_assert(verify(isq::mass_ratio_of_water_to_dry_matter, scalar, one));
static_assert(verify(isq::mass_ratio_of_water_vapour_to_dry_gas, scalar, one));
static_assert(verify(isq::mass_fraction_of_water, scalar, one));
static_assert(verify(isq::mass_fraction_of_dry_matter, scalar, one));
static_assert(verify(isq::relative_humidity, scalar, one));
static_assert(verify(isq::relative_mass_concentration_of_vapour, scalar, one));
static_assert(verify(isq::relative_mass_ratio_of_vapour, scalar, one));
static_assert(verify(isq::dew_point_temperature, scalar, K));

// electromagnetism
static_assert(verify(isq::electric_current, scalar, A));
static_assert(verify(isq::electric_charge, scalar, C));
static_assert(verify(isq::electric_charge_density, scalar, C / m3));
static_assert(verify(isq::volume_electric_charge, scalar, C / m3));
static_assert(verify(isq::surface_density_of_electric_charge, scalar, C / m2));
static_assert(verify(isq::areic_electric_charge, scalar, C / m2));
static_assert(verify(isq::linear_density_of_electric_charge, scalar, C / m));
static_assert(verify(isq::lineic_electric_charge, scalar, C / m));
static_assert(verify(isq::electric_dipole_moment, vector, C* m));
static_assert(verify(isq::electric_polarization, vector, C / m2));
static_assert(verify(isq::electric_current_density, vector, A / m2));
static_assert(verify(isq::areic_electric_current, vector, A / m2));
static_assert(verify(isq::linear_electric_current_density, vector, A / m));
static_assert(verify(isq::lineic_electric_current, vector, A / m));
static_assert(verify(isq::electric_field_strength, vector, V / m, N / C));
static_assert(verify(isq::electric_potential, scalar, V));
static_assert(verify(isq::electric_potential_difference, scalar, V));
static_assert(verify(isq::voltage, scalar, V));
static_assert(verify(isq::electric_tension, scalar, V));
static_assert(verify(isq::electric_flux_density, vector, C / m2));
static_assert(verify(isq::electric_displacement, vector, C / m2));
static_assert(verify(isq::capacitance, scalar, F));
static_assert(verify(isq::electric_constant, scalar, F / m, C / (V * m)));
static_assert(verify(isq::permittivity_of_vacuum, scalar, F / m, C / (V * m)));
static_assert(verify(isq::permittivity, scalar, F / m, C / (V * m)));
static_assert(verify(isq::relative_permittivity, scalar, one));
static_assert(verify(isq::electric_susceptibility, scalar, one));
static_assert(verify(isq::electric_flux, scalar, C));
static_assert(verify(isq::displacement_current_density, vector, A / m2));
static_assert(verify(isq::displacement_current, scalar, A));
static_assert(verify(isq::total_current, scalar, A));
static_assert(verify(isq::total_current_density, vector, A / m2));
static_assert(verify(isq::magnetic_flux_density, vector, T, N / (A * m), Wb / m2));
static_assert(verify(isq::magnetic_flux, scalar, Wb));
static_assert(verify(isq::linked_flux, scalar, Wb));
static_assert(verify(isq::magnetic_moment, vector, A* m2));
static_assert(verify(isq::magnetic_area_moment, vector, A* m2));
static_assert(verify(isq::magnetization, vector, A / m));
static_assert(verify(isq::magnetic_field_strength, vector, A / m));
static_assert(verify(isq::magnetizing_field, vector, A / m));
static_assert(verify(isq::magnetic_constant, scalar, H / m, V* s / (A * m)));
static_assert(verify(isq::permeability_of_vacuum, scalar, H / m, V* s / (A * m)));
static_assert(verify(isq::permeability, scalar, H / m, V* s / (A * m)));
static_assert(verify(isq::relative_permeability, scalar, one));
static_assert(verify(isq::magnetic_susceptibility, scalar, one));
static_assert(verify(isq::magnetic_polarization, vector, T));
static_assert(verify(isq::magnetic_dipole_moment, vector, Wb* m));
static_assert(verify(isq::coercivity, scalar, A / m));
static_assert(verify(isq::magnetic_vector_potential, vector, Wb / m));
static_assert(verify(isq::electromagnetic_energy_density, scalar, J / m3));
static_assert(verify(isq::volumic_electromagnetic_energy, scalar, J / m3));
static_assert(verify(isq::Poynting_vector, vector, W / m2));
static_assert(verify(isq::phase_speed_of_electromagnetic_waves, scalar, m / s));
static_assert(verify(isq::speed_of_light_in_vacuum, scalar, m / s));
static_assert(verify(isq::light_speed_in_vacuum, scalar, m / s));
static_assert(verify(isq::source_voltage, scalar, V));
static_assert(verify(isq::source_tension, scalar, V));
static_assert(verify(isq::scalar_magnetic_potential, scalar, A));
static_assert(verify(isq::magnetic_tension, scalar, A));
static_assert(verify(isq::magnetomotive_force, scalar, A));
static_assert(verify(isq::current_linkage, scalar, A));
static_assert(verify(isq::number_of_turns_in_a_winding, scalar, one));
static_assert(verify(isq::reluctance, scalar, one / H));
static_assert(verify(isq::permeance, scalar, H));
static_assert(verify(isq::inductance, scalar, H));
static_assert(verify(isq::self_inductance, scalar, H));
static_assert(verify(isq::mutual_inductance, scalar, H));
static_assert(verify(isq::coupling_factor, scalar, one));
static_assert(verify(isq::leakage_factor, scalar, one));
static_assert(verify(isq::conductivity, scalar, S / m));
static_assert(verify(isq::resistivity, scalar, si::ohm* m));
static_assert(verify(isq::electromagnetism_power, scalar, W));
static_assert(verify(isq::instantaneous_power, scalar, W));
static_assert(verify(isq::resistance, scalar, si::ohm));
static_assert(verify(isq::conductance, scalar, S));
static_assert(verify(isq::phase_difference, scalar, rad));
static_assert(verify(isq::electric_current_phasor, scalar, A));
static_assert(verify(isq::voltage_phasor, scalar, V));
static_assert(verify(isq::impedance, scalar, si::ohm));
static_assert(verify(isq::complex_impedance, scalar, si::ohm));
static_assert(verify(isq::resistance_to_alternating_current, scalar, si::ohm));
static_assert(verify(isq::reactance, scalar, si::ohm));
static_assert(verify(isq::modulus_of_impedance, scalar, si::ohm));
static_assert(verify(isq::admittance, scalar, S));
static_assert(verify(isq::complex_admittance, scalar, S));
static_assert(verify(isq::conductance_for_alternating_current, scalar, S));
static_assert(verify(isq::susceptance, scalar, S));
static_assert(verify(isq::modulus_of_admittance, scalar, S));
static_assert(verify(isq::quality_factor, scalar, one));
static_assert(verify(isq::loss_factor, scalar, one));
static_assert(verify(isq::loss_angle, scalar, rad));
static_assert(verify(isq::active_power, scalar, W));
static_assert(verify(isq::apparent_power, scalar, V* A));
static_assert(verify(isq::power_factor, scalar, one));
static_assert(verify(isq::complex_power, scalar, V* A));
static_assert(verify(isq::reactive_power, scalar, V* A));
static_assert(verify(isq::non_active_power, scalar, V* A));
static_assert(verify(isq::active_energy, scalar, J, W* h));

// atomic and nuclear physics
static_assert(verify(isq::activity, scalar, Bq, one / s));
static_assert(verify(isq::absorbed_dose, scalar, Gy, J / kg, m2 / s2));
static_assert(verify(isq::quality_factor, scalar, one));
static_assert(verify(isq::dose_equivalent, scalar, Sv, J / kg, m2 / s2));

}  // namespace
