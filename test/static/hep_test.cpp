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

#include "test_tools.h"
#include <mp-units/systems/hep.h>

namespace {

using namespace mp_units;
using namespace mp_units::hep::unit_symbols;
inline constexpr quantity_character scalar{quantity_tensor_order::scalar};
inline constexpr quantity_character vector{quantity_tensor_order::vector};

[[nodiscard]] consteval bool verify(QuantitySpec auto q, quantity_character ch, Unit auto... units)
{
  return get_character(q) == ch && (... && requires { q[units]; });
}

// space and time
static_assert(verify(hep::length, scalar, mm, cm));  // Gaudi: mm, ROOT: cm
static_assert(verify(hep::area, scalar, mm2));
static_assert(verify(hep::volume, scalar, mm3));
static_assert(verify(hep::angle, scalar, hep::radian, hep::degree));  // Gaudi: radian, ROOT: degree
static_assert(verify(hep::solid_angle, scalar, hep::steradian));
static_assert(verify(hep::duration, scalar, ns, s));  // Gaudi: ns, ROOT: s

// electric
static_assert(verify(hep::electric_charge, scalar, hep::eplus));
static_assert(verify(hep::electric_current, scalar, hep::ampere));
static_assert(verify(hep::electric_potential, scalar, hep::volt));
static_assert(verify(hep::electric_resistance, scalar, hep::ohm));
static_assert(verify(hep::electric_capacitance, scalar, hep::farad));

// magnetic
static_assert(verify(hep::magnetic_flux, scalar, hep::weber));
static_assert(verify(hep::magnetic_field, scalar, hep::tesla));
static_assert(verify(hep::inductance, scalar, hep::henry));

// energy, power, force, pressure
static_assert(verify(hep::energy, scalar, MeV, GeV));  // Gaudi: MeV, ROOT: GeV
static_assert(verify(hep::power, scalar, hep::watt));
static_assert(verify(hep::force, scalar, hep::newton));
static_assert(verify(hep::pressure, scalar, hep::pascal));

// mechanical
static_assert(verify(hep::mass, scalar, hep::gram));
static_assert(verify(hep::frequency, scalar, hep::hertz));

// thermodynamic
static_assert(verify(hep::temperature, scalar, hep::kelvin));
static_assert(verify(hep::amount_of_substance, scalar, hep::mole));

// radiometric
static_assert(verify(hep::activity, scalar, hep::becquerel));
static_assert(verify(hep::absorbed_dose, scalar, hep::gray));

// photometric
static_assert(verify(hep::luminous_intensity, scalar, hep::candela));
static_assert(verify(hep::luminous_flux, scalar, hep::lumen));
static_assert(verify(hep::illuminance, scalar, hep::lux));

// specialized length quantities
static_assert(verify(hep::path_length, scalar, mm, cm));
static_assert(verify(hep::displacement, vector, mm, cm));
static_assert(verify(hep::position_vector, vector, mm, cm));
static_assert(verify(hep::interaction_length, scalar, cm, mm));
static_assert(verify(hep::radiation_length, scalar, cm, mm));
static_assert(verify(hep::nuclear_interaction_length, scalar, cm, mm));
static_assert(verify(hep::mean_free_path, scalar, mm, cm));
static_assert(verify(hep::impact_parameter, scalar, mm, cm));
static_assert(verify(hep::decay_length, scalar, mm, cm));
static_assert(verify(hep::vertex_position, scalar, mm, cm));
static_assert(verify(hep::wavelength, scalar, mm, nm));
static_assert(verify(hep::radius, scalar, mm, cm));
static_assert(verify(hep::range, scalar, mm, cm));

// specialized time quantities
static_assert(verify(hep::proper_time, scalar, ns, s));
static_assert(verify(hep::coordinate_time, scalar, ns, s));
static_assert(verify(hep::lifetime, scalar, ns, s));
static_assert(verify(hep::half_life, scalar, ns, s));
static_assert(verify(hep::mean_lifetime, scalar, ns, s));
static_assert(verify(hep::time_of_flight, scalar, ns, s));

// specialized energy quantities
static_assert(verify(hep::kinetic_energy, scalar, MeV, GeV));
static_assert(verify(hep::rest_mass_energy, scalar, MeV, GeV));
static_assert(verify(hep::total_energy, scalar, MeV, GeV));
static_assert(verify(hep::center_of_mass_energy, scalar, MeV, GeV));
static_assert(verify(hep::binding_energy, scalar, MeV, GeV));
static_assert(verify(hep::separation_energy, scalar, MeV, GeV));
static_assert(verify(hep::Q_value, scalar, MeV, GeV));
static_assert(verify(hep::excitation_energy, scalar, MeV, GeV));
static_assert(verify(hep::ionization_energy, scalar, MeV, eV));
static_assert(verify(hep::threshold_energy, scalar, MeV, GeV));
static_assert(verify(hep::missing_energy, scalar, MeV, GeV));
static_assert(verify(hep::transverse_energy, scalar, MeV, GeV));

// specialized mass and momentum quantities
static_assert(verify(hep::rest_mass, scalar, hep::gram, si::kilo<hep::gram>));
static_assert(verify(hep::invariant_mass, scalar, hep::gram, si::kilo<hep::gram>));
static_assert(verify(hep::effective_mass, scalar, hep::gram, si::kilo<hep::gram>));
static_assert(verify(hep::reduced_mass, scalar, hep::gram, si::kilo<hep::gram>));
static_assert(verify(hep::momentum, scalar, GeV / c));
static_assert(verify(hep::transverse_momentum, scalar, GeV / c));

// specialized angular quantities
static_assert(verify(hep::scattering_angle, scalar, hep::radian, hep::degree));
static_assert(verify(hep::opening_angle, scalar, hep::radian, hep::degree));
static_assert(verify(hep::azimuthal_angle, scalar, hep::radian, hep::degree));
static_assert(verify(hep::polar_angle, scalar, hep::radian, hep::degree));
static_assert(verify(hep::phase, scalar, one));  // phase is dimensionless (cyclic, not an angle)

// Test quantity hierarchy conversions
// All specialized lengths can be implicitly converted up the hierarchy to generic length
static_assert(implicitly_convertible(hep::decay_length, hep::length));
static_assert(implicitly_convertible(hep::nuclear_interaction_length, hep::interaction_length));

// is_kind quantities form subkinds
static_assert(!implicitly_convertible(hep::radiation_length, hep::length));
static_assert(explicitly_convertible(hep::radiation_length, hep::length));
static_assert(!implicitly_convertible(hep::interaction_length, hep::length));
static_assert(explicitly_convertible(hep::interaction_length, hep::length));

static_assert(!implicitly_convertible(hep::proper_time, hep::duration));
static_assert(explicitly_convertible(hep::proper_time, hep::duration));
static_assert(!implicitly_convertible(hep::coordinate_time, hep::duration));
static_assert(explicitly_convertible(hep::coordinate_time, hep::duration));

// But cannot be implicitly converted between each other (different physical concepts!)
static_assert(!implicitly_convertible(hep::radiation_length, hep::decay_length));
static_assert(!implicitly_convertible(hep::decay_length, hep::interaction_length));
static_assert(!implicitly_convertible(hep::impact_parameter, hep::vertex_position));

// Specialized times: proper_time vs coordinate_time are distinct
static_assert(!implicitly_convertible(hep::proper_time, hep::coordinate_time));
static_assert(!implicitly_convertible(hep::coordinate_time, hep::proper_time));

// Lifetime hierarchy
static_assert(implicitly_convertible(hep::half_life, hep::lifetime));
static_assert(implicitly_convertible(hep::mean_lifetime, hep::lifetime));
static_assert(!implicitly_convertible(hep::half_life, hep::mean_lifetime));

// Energy types are distinct
static_assert(implicitly_convertible(hep::kinetic_energy, hep::total_energy));
static_assert(implicitly_convertible(hep::rest_mass_energy, hep::total_energy));
static_assert(implicitly_convertible(hep::total_energy, hep::energy));
static_assert(!implicitly_convertible(hep::kinetic_energy, hep::rest_mass_energy));
static_assert(!implicitly_convertible(hep::rest_mass_energy, hep::kinetic_energy));
static_assert(!implicitly_convertible(hep::binding_energy, hep::kinetic_energy));

// Total energy hierarchy: KE and E0 are children of total_energy
// This ensures KE + E0 naturally gives total_energy
static_assert(std::same_as<decltype(quantity<hep::kinetic_energy[MeV]>{} + quantity<hep::rest_mass_energy[MeV]>{}),
                           quantity<hep::total_energy[MeV]>>);

// Binding energy hierarchy
static_assert(implicitly_convertible(hep::separation_energy, hep::binding_energy));
static_assert(implicitly_convertible(hep::binding_energy, hep::energy));

// Mass types are distinct
static_assert(implicitly_convertible(hep::rest_mass, hep::mass));
static_assert(implicitly_convertible(hep::invariant_mass, hep::mass));
static_assert(!implicitly_convertible(hep::rest_mass, hep::invariant_mass));

// Momentum hierarchy
static_assert(implicitly_convertible(hep::transverse_momentum, hep::momentum));

// Angular quantities
static_assert(implicitly_convertible(hep::scattering_angle, hep::angle));
static_assert(implicitly_convertible(hep::opening_angle, hep::angle));
static_assert(!implicitly_convertible(hep::scattering_angle, hep::opening_angle));

// Phase is a distinct kind (marked with is_kind) - cannot convert to/from angle
static_assert(!implicitly_convertible(hep::phase, hep::angle));
static_assert(!explicitly_convertible(hep::phase, hep::angle));
static_assert(!castable(hep::phase, hep::angle));
static_assert(!implicitly_convertible(hep::angle, hep::phase));

// Test that specialized quantities cannot implicitly convert to siblings
// (but they CAN be compared since they have same dimension - this is by design)
static_assert(!implicitly_convertible(hep::decay_length, hep::radiation_length));
static_assert(!implicitly_convertible(hep::kinetic_energy, hep::rest_mass_energy));
static_assert(!implicitly_convertible(hep::proper_time, hep::coordinate_time));

// Derived quantities using specialized quantities
static_assert(verify(hep::speed, scalar, m / s, cm / ns));
static_assert(verify(hep::velocity, vector, m / s, cm / ns));
static_assert(verify(hep::decay_constant, scalar, hep::hertz));
static_assert(verify(hep::proper_velocity, vector, m / s));

// Specialized dimensionless derived quantities with physical meaning
static_assert(verify(hep::lorentz_factor, scalar, one));     // γ = E/E₀
static_assert(verify(hep::relativistic_beta, scalar, one));  // β = v/c

// Interaction and scattering quantities
static_assert(verify(hep::cross_section, scalar, hep::barn, mb, pb));
static_assert(verify(hep::number_density, scalar, inverse(cm3)));

// Test that derived quantity expressions using specialized quantities implicitly convert properly
// speed is defined as path_length / duration
static_assert(implicitly_convertible(hep::path_length / hep::duration, hep::speed));

// velocity is defined as displacement / duration
static_assert(implicitly_convertible(hep::displacement / hep::duration, hep::velocity));

// decay_constant is a frequency (λ = 1/τ where τ is mean_lifetime)
static_assert(implicitly_convertible(hep::decay_constant, hep::frequency));  // child->parent
static_assert(implicitly_convertible(inverse(hep::mean_lifetime), hep::frequency));

// lorentz_factor and relativistic_beta are distinct dimensionless kinds
// They cannot be created from arbitrary dimensionless values (marked with is_kind)
static_assert(!implicitly_convertible(dimensionless, hep::lorentz_factor));
static_assert(!implicitly_convertible(dimensionless, hep::relativistic_beta));
static_assert(!implicitly_convertible(hep::lorentz_factor, hep::relativistic_beta));

// cross_section is a specialized area
static_assert(implicitly_convertible(hep::cross_section, hep::area));
// But general area cannot implicitly become cross_section (preserves physics meaning)
static_assert(!implicitly_convertible(hep::area, hep::cross_section));

// number_density is inverse(volume)
static_assert(implicitly_convertible(inverse(hep::volume), hep::number_density));
static_assert(!implicitly_convertible(hep::frequency, hep::number_density));  // both inverse dimensions but different

// unit prefix relationships
static_assert(1'000 * eV / c2 == 1 * keV / c2);
static_assert(1'000'000 * eV / c == 1 * MeV / c);

// barn definition: 1 b = 10⁻²⁸ m²
static_assert(1e28 * b == 1. * m2);

// ---- fundamental exact constants (CODATA-independent, post-2019 SI) --------

// Speed of light (exact since 1983): c = 299 792 458 m/s
static_assert(1 * c == 299'792'458 * m / s);

// Planck constant (exact since 2019): h = 6.62607015e-34 J·s
static_assert(approx_equal(1. * h, 6.626'070'15e-34 * hep::joule * s));

// Reduced Planck constant: ℏ = h/(2π) (exact by definition)
static_assert(1 * hbar == 1 * h / (mag<2> * π));

// Elementary charge (exact since 2019):  e = 1 e⁺ (exact by definition)
static_assert(1 * e == 1 * hep::eplus);

// Avogadro constant (exact since 2019): N_A = 6.02214076e23 mol⁻¹
static_assert(approx_equal(1. * N_A, 6.022'140'76e23 * inverse(hep::mole)));

// ---- CODATA 2018 constants (default via inline namespace) ------------------

// Boltzmann constant (exact in 2019 SI): k_B = 8.617333262e-11 MeV/K
static_assert(approx_equal(1. * k_B, 8.617'333'262e-11 * MeV / hep::kelvin));

// Particle masses
static_assert(approx_equal(1. * m_e, 0.510'998'950'00 * MeV / c2));
static_assert(approx_equal(1. * m_p, 938.272'088'16 * MeV / c2));
static_assert(approx_equal(1. * m_n, 939.565'420'52 * MeV / c2));
static_assert(approx_equal(1. * u, 931.494'102'42 * MeV / c2));

// Fine structure constant (dimensionless)
static_assert(approx_equal(1. * alpha, 7.297'352'569'3e-3 * one));

// Atomic length scales
static_assert(approx_equal(1. * r_e, 2.817'940'326'2e-15 * m));
static_assert(approx_equal(1. * lambda_C, 2.426'310'238'67e-12 * m));
static_assert(approx_equal(1. * a_0, 5.291'772'109'03e-11 * m));

// Magnetons
static_assert(approx_equal(1. * mu_B, 9.274'010'078'3e-24 * hep::joule / hep::tesla));
static_assert(approx_equal(1. * mu_N, 5.050'783'746'1e-27 * hep::joule / hep::tesla));

// ---- CODATA 2014 constants -------------------------------------------------

static_assert(approx_equal(1. * hep::codata2014::boltzmann_constant, 8.617'330'3e-11 * MeV / hep::kelvin));
static_assert(approx_equal(1. * hep::codata2014::electron_mass, 0.510'998'946'1 * MeV / c2));
static_assert(approx_equal(1. * hep::codata2014::proton_mass, 938.272'081'3 * MeV / c2));
static_assert(approx_equal(1. * hep::codata2014::neutron_mass, 939.565'413'3 * MeV / c2));
static_assert(approx_equal(1. * hep::codata2014::atomic_mass_unit, 931.494'095'4 * MeV / c2));
static_assert(approx_equal(1. * hep::codata2014::fine_structure_constant, 7.297'352'566'4e-3 * one));
static_assert(approx_equal(1. * hep::codata2014::classical_electron_radius, 2.817'940'322'7e-15 * m));
static_assert(approx_equal(1. * hep::codata2014::electron_compton_wavelength, 2.426'310'236'7e-12 * m));
static_assert(approx_equal(1. * hep::codata2014::bohr_radius, 5.291'772'106'7e-11 * m));
static_assert(approx_equal(1. * hep::codata2014::bohr_magneton, 9.274'009'994e-24 * hep::joule / hep::tesla));
static_assert(approx_equal(1. * hep::codata2014::nuclear_magneton, 5.050'783'699e-27 * hep::joule / hep::tesla));

// ---- CODATA 2022 constants -------------------------------------------------

// boltzmann_constant and electron_compton_wavelength are unchanged from CODATA 2018
static_assert(approx_equal(1. * hep::codata2022::electron_mass, 0.510'998'950'69 * MeV / c2));
static_assert(approx_equal(1. * hep::codata2022::proton_mass, 938.272'089'43 * MeV / c2));
static_assert(approx_equal(1. * hep::codata2022::neutron_mass, 939.565'421'94 * MeV / c2));
static_assert(approx_equal(1. * hep::codata2022::atomic_mass_unit, 931.494'103'72 * MeV / c2));
static_assert(approx_equal(1. * hep::codata2022::fine_structure_constant, 7.297'352'564'3e-3 * one));
static_assert(approx_equal(1. * hep::codata2022::classical_electron_radius, 2.817'940'320'5e-15 * m));
static_assert(approx_equal(1. * hep::codata2022::bohr_radius, 5.291'772'105'44e-11 * m));
static_assert(approx_equal(1. * hep::codata2022::bohr_magneton, 9.274'010'065'7e-24 * hep::joule / hep::tesla));
static_assert(approx_equal(1. * hep::codata2022::nuclear_magneton, 5.050'783'739'3e-27 * hep::joule / hep::tesla));

}  // namespace
