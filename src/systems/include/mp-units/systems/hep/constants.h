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

#pragma once

#include <mp-units/bits/module_macros.h>
#include <mp-units/systems/hep/units.h>

// IWYU pragma: begin_exports
#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework/unit.h>
#endif
// IWYU pragma: end_exports

MP_UNITS_EXPORT
namespace mp_units::hep {

// Physical constants for High Energy Physics
//
// This file provides fundamental physical constants organized by CODATA release:
// - codata2014: CODATA 2014 values (used by some older HEP frameworks)
// - codata2018: CODATA 2018 values (inline namespace, default - used by CLHEP, Gaudi, Geant4)
// - codata2022: CODATA 2022 values (used by ROOT and most recent software)
//
// Usage:
//   using namespace hep;           // Uses CODATA 2018 by default (inline namespace)
//   auto E = m * c2;               // Uses CODATA 2018 constants
//
//   using namespace hep::codata2022;  // Opt into CODATA 2022 values
//   auto E = m * c2;                  // Uses CODATA 2022 constants
//
// Key differences between CODATA releases:
// - Boltzmann constant: 8.6173303e-11 (2014) vs 8.617333262e-11 (2018/2022) MeV/K
// - Electron mass:      0.5109989461 (2014) vs 0.51099895000 (2018) vs 0.51099895069 (2022) MeV
// - Proton mass:        938.2720813 (2014) vs 938.27208816 (2018) vs 938.27208943 (2022) MeV
// - Fine structure:     7.2973525664e-3 (2014) vs 7.2973525693e-3 (2018) vs 7.2973525643e-3 (2022)
// - Vacuum permeability: exactly 4π × 10⁻⁷ (2014) vs measured (2018/2022), see below
//
// References:
// - CODATA 2014: https://physics.nist.gov/cuu/Constants/archive2014.html
// - CODATA 2018: https://physics.nist.gov/cuu/Constants/archive2018.html
// - CODATA 2022: https://physics.nist.gov/cuu/Constants/ (current)

// Constants that are exact and identical across all CODATA releases
// clang-format off

// Avogadro constant (exact since 2019 SI redefinition)
inline constexpr struct avogadro_constant final :
    named_constant<"N_A", mag_ratio<602'214'076, 100'000'000> * mag_power<10, 23> / mole> {} avogadro_constant;

// Speed of light (exact since 1983)
inline constexpr struct speed_of_light_in_vacuum final :
    named_constant<"c", mag<299'792'458> * meter / second> {} speed_of_light_in_vacuum;

// Planck constant (exact since 2019 SI redefinition)
inline constexpr struct planck_constant final :
    named_constant<"h", mag_ratio<662'607'015, 100'000'000> * mag_power<10, -34> * joule * second> {} planck_constant;

// Elementary charge (exact since 2019 SI redefinition)
inline constexpr struct elementary_charge final :
    named_constant<"e", eplus> {} elementary_charge;

// Standard conditions
inline constexpr struct standard_temperature final :
    named_constant<"T_STP", mag_ratio<27'315, 100> * kelvin> {} standard_temperature;

inline constexpr struct standard_pressure final :
    named_constant<"P_STP", atmosphere> {} standard_pressure;

// Gas threshold (CLHEP convention)
inline constexpr struct gas_threshold final :
    named_constant<"gas_threshold", mag<10> * si::milli<gram> / cubic(si::centi<meter>)> {} gas_threshold;

// Universe mean density (cosmological)
inline constexpr struct universe_mean_density final :
    named_constant<symbol_text{u8"ρ_universe", "rho_universe"}, mag_power<10, -25> * gram / cubic(si::centi<meter>)> {} universe_mean_density;

// clang-format on

// CODATA 2014 values (used by some older HEP frameworks)
namespace codata2014 {

// clang-format off

// Vacuum magnetic permeability (CODATA 2014: exactly 4π × 10⁻⁷ N/A²)
// Before the 2019 SI redefinition the ampere was defined through this constant, which made it
// exact by definition. It became a measured quantity when the ampere was redefined via `e`.
inline constexpr struct permeability_of_vacuum final :
    named_constant<symbol_text{u8"μ₀", "mu_0"}, mag<4> * mag_power<10, -7> * π * henry / meter> {} permeability_of_vacuum;

// Boltzmann constant (CODATA 2014: 8.6173303e-11 MeV/K)
inline constexpr struct boltzmann_constant final :
    named_constant<"k_B", mag_ratio<86'173'303, 10'000'000> * mag_power<10, -11> * si::mega<electronvolt> / kelvin, relative_standard_uncertainty{mag_ratio<57, 10> * mag_power<10, -7>}> {} boltzmann_constant;

// Particle masses (CODATA 2014)
inline constexpr struct electron_mass final :
    named_constant<"m_e", mag_ratio<5'109'989'461, 10'000'000'000> * si::mega<electronvolt> / square(speed_of_light_in_vacuum), relative_standard_uncertainty{mag_ratio<62, 10> * mag_power<10, -9>}> {} electron_mass;

inline constexpr struct proton_mass final :
    named_constant<"m_p", mag_ratio<9'382'720'813, 10'000'000> * si::mega<electronvolt> / square(speed_of_light_in_vacuum), relative_standard_uncertainty{mag_ratio<62, 10> * mag_power<10, -9>}> {} proton_mass;

inline constexpr struct neutron_mass final :
    named_constant<"m_n", mag_ratio<9'395'654'133, 10'000'000> * si::mega<electronvolt> / square(speed_of_light_in_vacuum), relative_standard_uncertainty{mag_ratio<62, 10> * mag_power<10, -9>}> {} neutron_mass;

// Atomic mass unit (CODATA 2014)
inline constexpr struct atomic_mass_unit final :
    named_constant<"u", mag_ratio<9'314'940'954, 10'000'000> * si::mega<electronvolt> / square(speed_of_light_in_vacuum), relative_standard_uncertainty{mag_ratio<62, 10> * mag_power<10, -9>}> {} atomic_mass_unit;

// Fine structure constant (CODATA 2014: 7.2973525664e-3)
inline constexpr struct fine_structure_constant final :
    named_constant<symbol_text{u8"α", "alpha"}, mag_ratio<72'973'525'664, 10'000'000'000> * mag_power<10, -3> * one, relative_standard_uncertainty{mag_ratio<23, 10> * mag_power<10, -10>}> {} fine_structure_constant;

// Classical electron radius (CODATA 2014: 2.8179403227e-15 m)
inline constexpr struct classical_electron_radius final :
    named_constant<"r_e", mag_ratio<28'179'403'227, 10'000'000'000> * mag_power<10, -15> * meter, relative_standard_uncertainty{mag_ratio<68, 10> * mag_power<10, -10>}> {} classical_electron_radius;

// Electron Compton wavelength (CODATA 2014: 2.4263102367e-12 m)
inline constexpr struct electron_compton_wavelength final :
    named_constant<symbol_text{u8"λ_C", "lambda_C"}, mag_ratio<24'263'102'367, 10'000'000'000> * mag_power<10, -12> * meter, relative_standard_uncertainty{mag_ratio<45, 10> * mag_power<10, -10>}> {} electron_compton_wavelength;

// Bohr radius (CODATA 2014: 5.2917721067e-11 m)
inline constexpr struct bohr_radius final :
    named_constant<"a_0", mag_ratio<52'917'721'067, 10'000'000'000> * mag_power<10, -11> * meter, relative_standard_uncertainty{mag_ratio<23, 10> * mag_power<10, -10>}> {} bohr_radius;

// Bohr magneton (CODATA 2014: 9.274009994e-24 J/T)
inline constexpr struct bohr_magneton final :
    named_constant<symbol_text{u8"μ_B", "mu_B"}, mag_ratio<9'274'009'994, 1'000'000'000> * mag_power<10, -24> * joule / tesla, relative_standard_uncertainty{mag_ratio<62, 10> * mag_power<10, -9>}> {} bohr_magneton;

// Nuclear magneton (CODATA 2014: 5.050783699e-27 J/T)
inline constexpr struct nuclear_magneton final :
    named_constant<symbol_text{u8"μ_N", "mu_N"}, mag_ratio<5'050'783'699, 1'000'000'000> * mag_power<10, -27> * joule / tesla, relative_standard_uncertainty{mag_ratio<62, 10> * mag_power<10, -9>}> {} nuclear_magneton;

// clang-format on

}  // namespace codata2014

// CODATA 2018 values (used by more recent HEP frameworks like CLHEP, Gaudi, Geant4)
inline namespace codata2018 {

// clang-format off

// Vacuum magnetic permeability (CODATA 2018: 1.256 637 062 12(19) × 10⁻⁶ N/A², u_r = 1.5 × 10⁻¹⁰)
// No longer exact: the 2019 SI redefinition fixed `e` instead, so μ₀ is now measured.
inline constexpr struct permeability_of_vacuum final :
    named_constant<symbol_text{u8"μ₀", "mu_0"}, mag_ratio<125'663'706'212, 100'000'000'000> * mag_power<10, -6> * henry / meter, relative_standard_uncertainty{mag_ratio<15, 10> * mag_power<10, -10>}> {} permeability_of_vacuum;

// Boltzmann constant (CODATA 2018: 8.617333262e-11 MeV/K)
// No `relative_standard_uncertainty`: since the 2019 SI redefinition both k_B and the elementary
// charge are exact, so k_B in MeV/K is an exact ratio and the digits above are merely truncated.
// Its imprecision is decimal rounding, not measurement uncertainty. The CODATA 2014 value above,
// from before the redefinition, was genuinely measured and is annotated accordingly.
inline constexpr struct boltzmann_constant final :
    named_constant<"k_B", mag_ratio<8'617'333'262, 1'000'000'000> * mag_power<10, -11> * si::mega<electronvolt> / kelvin> {} boltzmann_constant;

// Particle masses (CODATA 2018)
inline constexpr struct electron_mass final :
    named_constant<"m_e", mag_ratio<51'099'895, 100'000'000> * si::mega<electronvolt> / square(speed_of_light_in_vacuum), relative_standard_uncertainty{mag_ratio<30, 10> * mag_power<10, -10>}> {} electron_mass;

inline constexpr struct proton_mass final :
    named_constant<"m_p", mag_ratio<938'272'08816, 100'000'000> * si::mega<electronvolt> / square(speed_of_light_in_vacuum), relative_standard_uncertainty{mag_ratio<31, 10> * mag_power<10, -10>}> {} proton_mass;

inline constexpr struct neutron_mass final :
    named_constant<"m_n", mag_ratio<93'956'542'052, 100'000'000> * si::mega<electronvolt> / square(speed_of_light_in_vacuum), relative_standard_uncertainty{mag_ratio<57, 10> * mag_power<10, -10>}> {} neutron_mass;

// Atomic mass unit (CODATA 2018)
inline constexpr struct atomic_mass_unit final :
    named_constant<"u", mag_ratio<93'149'410'242, 100'000'000> * si::mega<electronvolt> / square(speed_of_light_in_vacuum), relative_standard_uncertainty{mag_ratio<30, 10> * mag_power<10, -10>}> {} atomic_mass_unit;

// Fine structure constant (CODATA 2018: 7.2973525693e-3)
inline constexpr struct fine_structure_constant final :
    named_constant<symbol_text{u8"α", "alpha"}, mag_ratio<72'973'525'693, 10'000'000'000> * mag_power<10, -3> * one, relative_standard_uncertainty{mag_ratio<15, 10> * mag_power<10, -10>}> {} fine_structure_constant;

// Classical electron radius (CODATA 2018: 2.8179403262e-15 m)
inline constexpr struct classical_electron_radius final :
    named_constant<"r_e", mag_ratio<28'179'403'262, 10'000'000'000> * mag_power<10, -15> * meter, relative_standard_uncertainty{mag_ratio<45, 10> * mag_power<10, -10>}> {} classical_electron_radius;

// Electron Compton wavelength (CODATA 2018: 2.42631023867e-12 m)
inline constexpr struct electron_compton_wavelength final :
    named_constant<symbol_text{u8"λ_C", "lambda_C"}, mag_ratio<242'631'023'867, 100'000'000'000> * mag_power<10, -12> * meter, relative_standard_uncertainty{mag_ratio<30, 10> * mag_power<10, -10>}> {} electron_compton_wavelength;

// Bohr radius (CODATA 2018: 5.29177210903e-11 m)
inline constexpr struct bohr_radius final :
    named_constant<"a_0", mag_ratio<529'177'210'903, 100'000'000'000> * mag_power<10, -11> * meter, relative_standard_uncertainty{mag_ratio<15, 10> * mag_power<10, -10>}> {} bohr_radius;

// Bohr magneton (CODATA 2018: 9.2740100783e-24 J/T)
inline constexpr struct bohr_magneton final :
    named_constant<symbol_text{u8"μ_B", "mu_B"}, mag_ratio<92'740'100'783, 10'000'000'000> * mag_power<10, -24> * joule / tesla, relative_standard_uncertainty{mag_ratio<30, 10> * mag_power<10, -10>}> {} bohr_magneton;

// Nuclear magneton (CODATA 2018: 5.0507837461e-27 J/T)
inline constexpr struct nuclear_magneton final :
    named_constant<symbol_text{u8"μ_N", "mu_N"}, mag_ratio<50'507'837'461, 10'000'000'000> * mag_power<10, -27> * joule / tesla, relative_standard_uncertainty{mag_ratio<31, 10> * mag_power<10, -10>}> {} nuclear_magneton;

// clang-format on

}  // namespace codata2018

// CODATA 2022 values (used by ROOT and most recent HEP software)
namespace codata2022 {

// clang-format off

// Boltzmann constant (CODATA 2022: 8.617333262e-11 MeV/K, exact - same as 2018)
using codata2018::boltzmann_constant;

// Vacuum magnetic permeability (CODATA 2022: 1.256 637 061 27(20) × 10⁻⁶ N/A², u_r = 1.6 × 10⁻¹⁰)
inline constexpr struct permeability_of_vacuum final :
    named_constant<symbol_text{u8"μ₀", "mu_0"}, mag_ratio<125'663'706'127, 100'000'000'000> * mag_power<10, -6> * henry / meter, relative_standard_uncertainty{mag_ratio<16, 10> * mag_power<10, -10>}> {} permeability_of_vacuum;

// Particle masses (CODATA 2022)
inline constexpr struct electron_mass final :
    named_constant<"m_e", mag_ratio<51'099'895'069, 100'000'000'000> * si::mega<electronvolt> / square(speed_of_light_in_vacuum), relative_standard_uncertainty{mag_ratio<31, 10> * mag_power<10, -10>}> {} electron_mass;

inline constexpr struct proton_mass final :
    named_constant<"m_p", mag_ratio<93'827'208'943, 100'000'000> * si::mega<electronvolt> / square(speed_of_light_in_vacuum), relative_standard_uncertainty{mag_ratio<31, 10> * mag_power<10, -10>}> {} proton_mass;

inline constexpr struct neutron_mass final :
    named_constant<"m_n", mag_ratio<93'956'542'194, 100'000'000> * si::mega<electronvolt> / square(speed_of_light_in_vacuum), relative_standard_uncertainty{mag_ratio<51, 10> * mag_power<10, -10>}> {} neutron_mass;

// Atomic mass unit (CODATA 2022)
inline constexpr struct atomic_mass_unit final :
    named_constant<"u", mag_ratio<93'149'410'372, 100'000'000> * si::mega<electronvolt> / square(speed_of_light_in_vacuum), relative_standard_uncertainty{mag_ratio<31, 10> * mag_power<10, -10>}> {} atomic_mass_unit;

// Fine structure constant (CODATA 2022: 7.2973525643e-3)
inline constexpr struct fine_structure_constant final :
    named_constant<symbol_text{u8"α", "alpha"}, mag_ratio<72'973'525'643, 10'000'000'000> * mag_power<10, -3> * one, relative_standard_uncertainty{mag_ratio<16, 10> * mag_power<10, -10>}> {} fine_structure_constant;

// Classical electron radius (CODATA 2022: 2.8179403205e-15 m)
inline constexpr struct classical_electron_radius final :
    named_constant<"r_e", mag_ratio<28'179'403'205, 10'000'000'000> * mag_power<10, -15> * meter, relative_standard_uncertainty{mag_ratio<47, 10> * mag_power<10, -10>}> {} classical_electron_radius;

// Electron Compton wavelength (CODATA 2022: 2.42631023867e-12 m - same as 2018)
using codata2018::electron_compton_wavelength;

// Bohr radius (CODATA 2022: 5.29177210544e-11 m)
inline constexpr struct bohr_radius final :
    named_constant<"a_0", mag_ratio<529'177'210'544, 100'000'000'000> * mag_power<10, -11> * meter, relative_standard_uncertainty{mag_ratio<16, 10> * mag_power<10, -10>}> {} bohr_radius;

// Bohr magneton (CODATA 2022: 9.2740100657e-24 J/T)
inline constexpr struct bohr_magneton final :
    named_constant<symbol_text{u8"μ_B", "mu_B"}, mag_ratio<92'740'100'657, 10'000'000'000> * mag_power<10, -24> * joule / tesla, relative_standard_uncertainty{mag_ratio<31, 10> * mag_power<10, -10>}> {} bohr_magneton;

// Nuclear magneton (CODATA 2022: 5.0507837393e-27 J/T)
inline constexpr struct nuclear_magneton final :
    named_constant<symbol_text{u8"μ_N", "mu_N"}, mag_ratio<50'507'837'393, 10'000'000'000> * mag_power<10, -27> * joule / tesla, relative_standard_uncertainty{mag_ratio<31, 10> * mag_power<10, -10>}> {} nuclear_magneton;

// clang-format on

}  // namespace codata2022

// Convenient symbols namespace (uses CODATA 2018 values by default)
// This matches the values in CLHEP, Gaudi, and Geant4
namespace unit_symbols {

// Speed of light symbols
inline constexpr auto c = speed_of_light_in_vacuum;
inline constexpr auto c2 = square(speed_of_light_in_vacuum);

// Planck constant symbols
inline constexpr auto h = planck_constant;
inline constexpr auto hbar = planck_constant / (mag<2> * π);

// Elementary charge symbol
inline constexpr auto e = elementary_charge;

// Particle mass symbols
inline constexpr auto m_e = electron_mass;
inline constexpr auto m_p = proton_mass;
inline constexpr auto m_n = neutron_mass;
inline constexpr auto u = atomic_mass_unit;

// Other constants
inline constexpr auto N_A = avogadro_constant;
inline constexpr auto k_B = boltzmann_constant;
inline constexpr auto mu_0 = permeability_of_vacuum;
inline constexpr auto alpha = fine_structure_constant;
inline constexpr auto r_e = classical_electron_radius;
inline constexpr auto lambda_C = electron_compton_wavelength;
inline constexpr auto a_0 = bohr_radius;
inline constexpr auto mu_B = bohr_magneton;
inline constexpr auto mu_N = nuclear_magneton;

}  // namespace unit_symbols
}  // namespace mp_units::hep
