// The MIT License (MIT)
//
// Copyright (c) 2021 Mateusz Pusz
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

// The HEP system of quantities is inspired by popular HEP projects:
// - https://gitlab.cern.ch/CLHEP/CLHEP/-/blob/develop/Units/Units/SystemOfUnits.h
// - https://gitlab.cern.ch/gaudi/Gaudi/-/blob/main/GaudiKernel/include/GaudiKernel/SystemOfUnits.h
// - https://gitlab.cern.ch/geant4/geant4/-/blob/master/source/externals/clhep/include/CLHEP/Units/SystemOfUnits.h
// - https://github.com/root-project/root/blob/master/geom/geom/inc/TGeoSystemOfUnits.h

#pragma once

#include <mp-units/bits/module_macros.h>
#include <mp-units/systems/angular.h>

// IWYU pragma: begin_exports
#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework/quantity_spec.h>
#endif
// IWYU pragma: end_exports

MP_UNITS_EXPORT
namespace mp_units::hep {

using namespace ::mp_units::angular;

// dimensions of base quantities
// clang-format off
inline constexpr struct dim_length final : base_dimension<"L"> {} dim_length;
inline constexpr struct dim_time final : base_dimension<"T"> {} dim_time;
inline constexpr struct dim_electric_charge final : base_dimension<"Q"> {} dim_electric_charge;
inline constexpr struct dim_energy final : base_dimension<"E"> {} dim_energy;
inline constexpr struct dim_temperature final : base_dimension<symbol_text{u8"Θ", "O"}> {} dim_temperature;
inline constexpr struct dim_amount_of_substance final : base_dimension<"N"> {} dim_amount_of_substance;
inline constexpr struct dim_luminous_intensity final : base_dimension<"I"> {} dim_luminous_intensity;
// clang-format on

// base quantities
QUANTITY_SPEC(length, dim_length, non_negative);
QUANTITY_SPEC(duration, dim_time, non_negative);
QUANTITY_SPEC(electric_charge, dim_electric_charge);
QUANTITY_SPEC(energy, dim_energy, non_negative);
QUANTITY_SPEC(temperature, dim_temperature, non_negative);
QUANTITY_SPEC(amount_of_substance, dim_amount_of_substance, non_negative);
QUANTITY_SPEC(luminous_intensity, dim_luminous_intensity, non_negative);

// derived quantites
// space and time
QUANTITY_SPEC(area, pow<2>(length));
QUANTITY_SPEC(volume, pow<3>(length));

// specialized length quantities - each represents distinct physical concepts
QUANTITY_SPEC(width, length);        // transverse dimension of a detector element or geometry
QUANTITY_SPEC(height, length);       // longitudinal dimension of a detector element or geometry
QUANTITY_SPEC(path_length, length);  // actual distance traveled along a trajectory
QUANTITY_SPEC(displacement, length,
              quantity_tensor_order::vector);        // shortest distance between two points (vector)
QUANTITY_SPEC(position_vector, displacement);        // position relative to origin (vector)
QUANTITY_SPEC(interaction_length, length, is_kind);  // mean distance before nuclear interaction with matter
QUANTITY_SPEC(radiation_length, length, is_kind);    // mean distance over which electron energy falls to 1/e
QUANTITY_SPEC(nuclear_interaction_length,
              interaction_length);        // mean distance before nuclear interaction
QUANTITY_SPEC(mean_free_path, length);    // average distance between collisions
QUANTITY_SPEC(impact_parameter, length);  // closest approach distance in collisions
QUANTITY_SPEC(decay_length, length);      // distance traveled before particle decay
QUANTITY_SPEC(vertex_position, length);   // position of collision/decay vertex
QUANTITY_SPEC(wavelength, length);        // electromagnetic or de Broglie wavelength
QUANTITY_SPEC(radius, length);            // radial distance from axis/center
QUANTITY_SPEC(range, length);             // distance a particle travels before stopping

// specialized time quantities
QUANTITY_SPEC(proper_time, duration, is_kind);      // time in particle's rest frame (Lorentz-invariant)
QUANTITY_SPEC(coordinate_time, duration, is_kind);  // time in lab/detector frame (frame-dependent)
QUANTITY_SPEC(lifetime, duration);                  // decay time quantities
QUANTITY_SPEC(half_life, lifetime);                 // time for 50% decay (t₁/₂)
QUANTITY_SPEC(mean_lifetime, lifetime);             // average decay time (τ = t₁/₂/ln(2))
QUANTITY_SPEC(time_of_flight, duration);            // time for particle to traverse distance

// electric
QUANTITY_SPEC(electric_current, electric_charge / duration);
QUANTITY_SPEC(electric_potential, energy / electric_charge);
QUANTITY_SPEC(electric_resistance, energy* duration / pow<2>(electric_charge));
QUANTITY_SPEC(electric_capacitance, pow<2>(electric_charge) / energy);

// magnetic
QUANTITY_SPEC(magnetic_flux, duration* energy / electric_charge);
QUANTITY_SPEC(magnetic_field, duration* energy / electric_charge / pow<2>(length));
QUANTITY_SPEC(inductance, pow<2>(duration) * energy / pow<2>(electric_charge));

// specialized energy quantities - fundamental measurements in HEP
// Note: total_energy is parent so that kinetic_energy + rest_mass_energy naturally gives total_energy
QUANTITY_SPEC(total_energy, energy);               // total relativistic energy: E = γmc²
QUANTITY_SPEC(kinetic_energy, total_energy);       // energy of motion: KE = (γ-1)mc²
QUANTITY_SPEC(rest_mass_energy, total_energy);     // rest energy: E₀ = mc²
QUANTITY_SPEC(center_of_mass_energy, energy);      // total energy in center-of-mass frame
QUANTITY_SPEC(binding_energy, energy);             // energy holding composite particles together
QUANTITY_SPEC(separation_energy, binding_energy);  // energy to remove one nucleon
QUANTITY_SPEC(Q_value, energy);                    // energy released/required in reaction or decay
QUANTITY_SPEC(excitation_energy, energy);          // energy above ground state
QUANTITY_SPEC(ionization_energy, energy);          // energy to remove electron from atom
QUANTITY_SPEC(threshold_energy, energy);           // minimum energy for reaction to occur
QUANTITY_SPEC(missing_energy, energy);             // energy carried by undetected particles
QUANTITY_SPEC(transverse_energy, energy);          // energy component perpendicular to beam axis

// energy, power, force, pressure
QUANTITY_SPEC(power, energy / duration);
QUANTITY_SPEC(force, energy / length);
QUANTITY_SPEC(pressure, energy / pow<3>(length));

// specialized mass and momentum quantities
QUANTITY_SPEC(mass, energy* pow<2>(duration) / pow<2>(length));  // general mass
QUANTITY_SPEC(rest_mass, mass);                                  // invariant mass of particle at rest
QUANTITY_SPEC(invariant_mass, mass);                             // mass calculated from 4-momentum
QUANTITY_SPEC(effective_mass, mass);                             // quasi-particle mass in medium
QUANTITY_SPEC(reduced_mass, mass);                               // effective mass in two-body problem
QUANTITY_SPEC(momentum, energy* duration / length);              // linear momentum
QUANTITY_SPEC(transverse_momentum, momentum);                    // momentum perpendicular to beam axis

// specialized angular quantities
QUANTITY_SPEC(scattering_angle, angle);        // deflection angle in collision
QUANTITY_SPEC(opening_angle, angle);           // angle between decay products/jets
QUANTITY_SPEC(azimuthal_angle, angle);         // angle in transverse plane (φ)
QUANTITY_SPEC(polar_angle, angle);             // angle from beam axis (θ)
QUANTITY_SPEC(phase, dimensionless, is_kind);  // quantum phase (cyclic, dimensionless, incompatible with angle)

// frequency and related
QUANTITY_SPEC(frequency, inverse(duration));

// derived quantities using specialized quantities for enhanced type safety
QUANTITY_SPEC(speed, path_length / duration);                                     // scalar speed along path
QUANTITY_SPEC(velocity, displacement / duration, quantity_tensor_order::vector);  // velocity  vector
QUANTITY_SPEC(decay_constant, frequency);  // λ = 1/τ (decay rate), a type of frequency
QUANTITY_SPEC(proper_velocity, displacement / proper_time,
              quantity_tensor_order::vector);  // four-velocity spatial part
// Note: lorentz_factor and relativistic_beta are dimensionless but have specific physical meaning
// They are distinct "kinds" to prevent accidental mixing with generic dimensionless values
QUANTITY_SPEC(lorentz_factor, dimensionless, is_kind, non_negative);  // γ = E/E₀ (relativistic time dilation factor)
QUANTITY_SPEC(relativistic_beta, dimensionless, is_kind,
              non_negative);  // β = v/c (velocity as fraction of light speed)

// interaction and collisions
QUANTITY_SPEC(cross_section, area);              // interaction probability measure (σ)
QUANTITY_SPEC(number_density, inverse(volume));  // particles per unit volume (n)

// radiometric
QUANTITY_SPEC(activity, inverse(duration));
QUANTITY_SPEC(absorbed_dose, pow<2>(length) / pow<2>(duration));

// photometric
QUANTITY_SPEC(luminous_flux, luminous_intensity* solid_angle);
QUANTITY_SPEC(illuminance, luminous_flux / pow<2>(length));

}  // namespace mp_units::hep
