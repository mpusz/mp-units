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
#include <mp-units/systems/isq/electromagnetism.h>
#include <mp-units/systems/isq/shared_quantities.h>
#include <mp-units/systems/isq/thermodynamics.h>

// IWYU pragma: begin_exports
#include <mp-units/systems/isq/si_quantities.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework/quantity_spec.h>
#endif
// IWYU pragma: end_exports

MP_UNITS_EXPORT
namespace mp_units::isq {

// crystal lattice (ISO 80000-12:2019, items 12-1 to 12-8)
QUANTITY_SPEC(lattice_vector, displacement);                // 12-1.1
QUANTITY_SPEC(fundamental_lattice_vector, lattice_vector);  // 12-1.2
// item 12-2.1 scales with 2π, and Bloch's theorem adds it to a wave vector, so it is a wave vector
QUANTITY_SPEC(angular_reciprocal_lattice_vector, wave_vector);                            // 12-2.1
QUANTITY_SPEC(fundamental_reciprocal_lattice_vector, angular_reciprocal_lattice_vector);  // 12-2.2
QUANTITY_SPEC(lattice_plane_spacing, distance);                                           // 12-3
QUANTITY_SPEC(Bragg_angle, angular_measure);                                              // 12-4
QUANTITY_SPEC(short_range_order_parameter, dimensionless);                                // 12-5.1
QUANTITY_SPEC(long_range_order_parameter, dimensionless);                                 // 12-5.2
// a scattering amplitude relative to that of a single electron, so not a ratio of like quantities
QUANTITY_SPEC(atomic_scattering_factor, dimensionless, is_kind, quantity_field::complex);  // 12-5.3
QUANTITY_SPEC(structure_factor, atomic_scattering_factor);                                 // 12-5.4
QUANTITY_SPEC(Burgers_vector, displacement);                                               // 12-6
QUANTITY_SPEC(particle_position_vector, position_vector);                                  // 12-7.1
QUANTITY_SPEC(equilibrium_position_vector, particle_position_vector);                      // 12-7.2
// ISO 80000 names item 12-7.3 "displacement vector <condensed matter physics>"
QUANTITY_SPEC(atomic_displacement_vector, displacement);          // 12-7.3
QUANTITY_SPEC(Debye_Waller_factor, dimensionless, non_negative);  // 12-8

// lattice dynamics (ISO 80000-12:2019, items 12-9 to 12-16)
// item 12-9.1 (angular wavenumber, k = p/ħ) is `angular_repetency` of ISO 80000-3 item 3-22
QUANTITY_SPEC(Fermi_angular_wavenumber, angular_repetency);  // 12-9.2
inline constexpr auto Fermi_angular_repetency = Fermi_angular_wavenumber;
QUANTITY_SPEC(Debye_angular_wavenumber, angular_repetency);  // 12-9.3
inline constexpr auto Debye_angular_repetency = Debye_angular_wavenumber;
QUANTITY_SPEC(Debye_angular_frequency, angular_frequency);    // 12-10
QUANTITY_SPEC(Debye_temperature, thermodynamic_temperature);  // 12-11
// not in ISO 80000; the counts that items 12-12 and 12-16 divide. Adding vibrational modes to
// electronic states is meaningless, so each roots its own kind below the shared count parent
QUANTITY_SPEC(number_of_vibrational_modes, number_of_entities, is_kind);
QUANTITY_SPEC(number_of_states, number_of_entities, is_kind);
QUANTITY_SPEC(density_of_vibrational_states, number_of_vibrational_modes / (volume * angular_frequency),
              non_negative);  // 12-12
QUANTITY_SPEC(thermodynamic_Gruneisen_parameter, dimensionless,
              cubic_expansion_coefficient /
                (isothermal_compressibility * specific_heat_capacity_at_constant_volume * mass_density));  // 12-13
// TODO item 12-14 is minus the logarithmic derivative of an angular frequency with respect to
// volume, which awaits the logarithmic-quantities framework, so it carries no defining equation yet
QUANTITY_SPEC(Gruneisen_parameter, dimensionless);  // 12-14
// not in ISO 80000; items 12-15.1 and 12-15.2 are both mean free paths and are compared with each other
QUANTITY_SPEC(mean_free_path, distance);
QUANTITY_SPEC(mean_free_path_of_phonons, mean_free_path);                                     // 12-15.1
QUANTITY_SPEC(mean_free_path_of_electrons, mean_free_path);                                   // 12-15.2
QUANTITY_SPEC(energy_density_of_states, number_of_states / (volume * energy), non_negative);  // 12-16

// electron transport (ISO 80000-12:2019, items 12-17 to 12-23)
QUANTITY_SPEC(residual_resistivity, resistivity);  // 12-17
QUANTITY_SPEC(Lorenz_coefficient,
              thermal_conductivity / (conductivity * thermodynamic_temperature));  // 12-18
// ISO 80000 defines item 12-19 for an isotropic conductor, where it relates two vectors as a scalar
QUANTITY_SPEC(Hall_coefficient, electric_field_strength / (magnetic_flux_density * electric_current_density),
              quantity_tensor_order::scalar);                                            // 12-19
QUANTITY_SPEC(thermoelectric_voltage, voltage);                                          // 12-20
QUANTITY_SPEC(Seebeck_coefficient, thermoelectric_voltage / thermodynamic_temperature);  // 12-21
inline constexpr auto thermoelectric_power = Seebeck_coefficient;
QUANTITY_SPEC(Peltier_coefficient, voltage, heat_flow_rate / electric_current);  // 12-22
QUANTITY_SPEC(Thomson_coefficient, Seebeck_coefficient,
              heat_flow_rate / (electric_current * thermodynamic_temperature));  // 12-23

// electronic structure (ISO 80000-12:2019, items 12-24 to 12-31)
// TODO ISO explicitly allows negative values for items 12-25, 12-27.1, 12-30, and 12-34 (an
// electron affinity of a repulsive state, a Fermi energy measured from a chosen reference, an
// effective mass near a band maximum, an antiferromagnetic exchange integral). They nevertheless
// inherit `non_negative` from `energy` and `mass`, because a signed child of a non-negative parent
// cannot be expressed yet. Do not construct these from `quantity_point`s until that exists.
QUANTITY_SPEC(work_function, energy);      // 12-24.1
QUANTITY_SPEC(ionization_energy, energy);  // 12-24.2
QUANTITY_SPEC(electron_affinity, energy);  // 12-25
QUANTITY_SPEC(Richardson_constant, electric_current_density / pow<2>(thermodynamic_temperature),
              quantity_tensor_order::scalar);                 // 12-26
QUANTITY_SPEC(Fermi_energy, energy);                          // 12-27.1
QUANTITY_SPEC(gap_energy, energy);                            // 12-27.2
QUANTITY_SPEC(Fermi_temperature, thermodynamic_temperature);  // 12-28
// not in ISO 80000; items 12-29.1 to 12-29.5 each divide a count of entities by a volume, and
// `n * p == pow<2>(n_i)` relates them across species, so they share one kind
QUANTITY_SPEC(number_density, number_of_entities / volume, non_negative);
QUANTITY_SPEC(electron_density, number_density);                                                      // 12-29.1
QUANTITY_SPEC(hole_density, number_density);                                                          // 12-29.2
QUANTITY_SPEC(intrinsic_carrier_density, number_density, pow<1, 2>(electron_density* hole_density));  // 12-29.3
QUANTITY_SPEC(donor_density, number_density);                                                         // 12-29.4
QUANTITY_SPEC(acceptor_density, number_density);                                                      // 12-29.5
QUANTITY_SPEC(effective_mass, mass);                                                                  // 12-30
// TODO item 12-31 (mobility ratio) divides two mobilities of ISO 80000-10 item 10-61, which is not
// modelled yet

// carrier dynamics (ISO 80000-12:2019, items 12-32 to 12-34)
QUANTITY_SPEC(relaxation_time, time_constant);   // 12-32.1
QUANTITY_SPEC(carrier_lifetime, time_constant);  // 12-32.2
// TODO item 12-33 (diffusion length) is the square root of a diffusion coefficient of ISO 80000-9
// item 9-39 times a carrier lifetime, and the diffusion coefficient is not modelled yet
QUANTITY_SPEC(exchange_integral, energy);  // 12-34

// magnetic and superconducting transitions (ISO 80000-12:2019, items 12-35 to 12-38)
// not in ISO 80000; the three transition temperatures below are each defined as a critical
// thermodynamic temperature, and a critical magnetic flux density likewise parents items 12-36.x
QUANTITY_SPEC(critical_thermodynamic_temperature, thermodynamic_temperature);
QUANTITY_SPEC(Curie_temperature, critical_thermodynamic_temperature);                       // 12-35.1
QUANTITY_SPEC(Neel_temperature, critical_thermodynamic_temperature);                        // 12-35.2
QUANTITY_SPEC(superconduction_transition_temperature, critical_thermodynamic_temperature);  // 12-35.3
QUANTITY_SPEC(critical_magnetic_flux_density, magnetic_flux_density, quantity_tensor_order::scalar);
QUANTITY_SPEC(thermodynamic_critical_magnetic_flux_density, critical_magnetic_flux_density,
              pow<1, 2>(magnetic_constant* Gibbs_energy / volume));                   // 12-36.1
QUANTITY_SPEC(lower_critical_magnetic_flux_density, critical_magnetic_flux_density);  // 12-36.2
QUANTITY_SPEC(upper_critical_magnetic_flux_density, critical_magnetic_flux_density);  // 12-36.3
QUANTITY_SPEC(superconductor_energy_gap, gap_energy);                                 // 12-37
QUANTITY_SPEC(London_penetration_depth, distance);                                    // 12-38.1
QUANTITY_SPEC(coherence_length, distance);                                            // 12-38.2

}  // namespace mp_units::isq
