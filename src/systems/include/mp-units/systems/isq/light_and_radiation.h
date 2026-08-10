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
#include <mp-units/systems/isq/shared_quantities.h>
#include <mp-units/systems/isq/space_and_time.h>

// IWYU pragma: begin_exports
#include <mp-units/systems/isq/base_quantities.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework/quantity_spec.h>
#endif
// IWYU pragma: end_exports

MP_UNITS_EXPORT
namespace mp_units::isq {

QUANTITY_SPEC(speed_of_light_in_a_medium, speed);
QUANTITY_SPEC(refractive_index, dimensionless, speed_of_light_in_vacuum / speed_of_light_in_a_medium);
QUANTITY_SPEC(radiant_energy, energy);
QUANTITY_SPEC(spectral_radiant_energy, radiant_energy / wavelength, non_negative);
QUANTITY_SPEC(radiant_energy_density, energy_density, radiant_energy / volume);
QUANTITY_SPEC(spectral_radiant_energy_density_in_terms_of_wavelength, radiant_energy_density / wavelength,
              non_negative);
QUANTITY_SPEC(spectral_radiant_energy_density_in_terms_of_wavenumber, radiant_energy_density / wavenumber,
              non_negative);
QUANTITY_SPEC(radiant_flux, power, radiant_energy / time);
inline constexpr auto radiant_power = radiant_flux;
QUANTITY_SPEC(spectral_radiant_flux, radiant_flux / wavelength, non_negative);
inline constexpr auto spectral_radiant_power = spectral_radiant_flux;
QUANTITY_SPEC(radiant_intensity, radiant_flux / solid_angular_measure, non_negative);
QUANTITY_SPEC(spectral_radiant_intensity, radiant_intensity / wavelength, non_negative);
QUANTITY_SPEC(radiance, radiant_intensity / area, non_negative);
QUANTITY_SPEC(spectral_radiance, radiance / wavelength, non_negative);
QUANTITY_SPEC(irradiance, radiant_flux / area, non_negative);
QUANTITY_SPEC(spectral_irradiance, irradiance / wavelength, non_negative);
QUANTITY_SPEC(radiant_exitance, radiant_flux / area, non_negative);
QUANTITY_SPEC(spectral_radiant_exitance, radiant_exitance / wavelength, non_negative);
QUANTITY_SPEC(radiant_exposure, radiant_energy / area, non_negative);
QUANTITY_SPEC(spectral_radiant_exposure, radiant_exposure / wavelength, non_negative);

// luminous quantities (ISO 80000-7:2019, items 7-10 to 7-18)
// the luminous counterparts of the radiometric quantities above with one kind tree per
// photometric condition; `luminous_flux` and `illuminance` themselves live in `si_quantities.h`

// ISO 80000 names both items "luminous efficacy" (7-11.1 <of radiation>, 7-11.4 <of a source>).
// They share the lumen per watt, yet must not be mixed: crossing between them takes the radiant
// efficiency of the source, so each is a distinct subkind of a common parent (like fluid and water head)
MP_UNITS_PHOTOMETRIC_QSPEC(luminous_efficacy, luminous_flux_of<Condition> / power, non_negative);
MP_UNITS_PHOTOMETRIC_QSPEC(luminous_efficacy_of_radiation, luminous_efficacy_of<Condition>,
                           luminous_flux_of<Condition> / radiant_flux, is_kind, non_negative);
MP_UNITS_PHOTOMETRIC_QSPEC(spectral_luminous_efficacy, luminous_efficacy_of_radiation_of<Condition>);
MP_UNITS_PHOTOMETRIC_QSPEC(maximum_luminous_efficacy, luminous_efficacy_of_radiation_of<Condition>);
MP_UNITS_PHOTOMETRIC_QSPEC(luminous_efficacy_of_source, luminous_efficacy_of<Condition>, is_kind, non_negative);
MP_UNITS_PHOTOMETRIC_QSPEC(luminous_efficiency,
                           luminous_efficacy_of_radiation_of<Condition> / luminous_efficacy_of_radiation_of<Condition>,
                           non_negative);
MP_UNITS_PHOTOMETRIC_QSPEC(spectral_luminous_efficiency, luminous_efficiency_of<Condition>);
MP_UNITS_PHOTOMETRIC_QSPEC(luminous_energy, luminous_flux_of<Condition>* time, non_negative);
MP_UNITS_PHOTOMETRIC_QSPEC(luminance, luminous_intensity_of<Condition> / area, non_negative);
MP_UNITS_PHOTOMETRIC_QSPEC(luminous_exitance, luminous_flux_of<Condition> / area, non_negative);
MP_UNITS_PHOTOMETRIC_QSPEC(luminous_exposure, illuminance_of<Condition>* time, non_negative);

// photon quantities (ISO 80000-7:2019, items 7-19 to 7-25)
QUANTITY_SPEC(photon_energy, energy);
QUANTITY_SPEC(photon_number, number_of_entities, radiant_energy / photon_energy, is_kind);
QUANTITY_SPEC(photon_flux, photon_number / duration, non_negative);
QUANTITY_SPEC(photon_intensity, photon_flux / solid_angular_measure, non_negative);
QUANTITY_SPEC(photon_radiance, photon_intensity / area, non_negative);
QUANTITY_SPEC(photon_irradiance, photon_flux / area, non_negative);
QUANTITY_SPEC(photon_exitance, photon_flux / area, non_negative);
QUANTITY_SPEC(photon_exposure, photon_number / area, non_negative);

// colorimetry (ISO 80000-7:2019, items 7-26 to 7-29)
// tristimulus values, colour-matching functions, and chromaticity coordinates
// (items 7-26 to 7-28) are triples of tabulated functions or coordinates rather than scalar
// quantities of a single kind, so they are not modelled as quantity specs
QUANTITY_SPEC(colour_temperature, thermodynamic_temperature);
QUANTITY_SPEC(correlated_colour_temperature, thermodynamic_temperature);

// material properties for radiation and light (ISO 80000-7:2019, items 7-30 to 7-37)
// TODO absorbance (items 7-32.1 and 7-32.2) is a logarithmic quantity and awaits the
// logarithmic-quantities framework
QUANTITY_SPEC(emissivity, radiant_exitance / radiant_exitance, non_negative);
QUANTITY_SPEC(spectral_emissivity, emissivity);  // named "emissivity at a specified wavelength" in ISO 80000
QUANTITY_SPEC(absorptance, radiant_flux / radiant_flux, non_negative);
MP_UNITS_PHOTOMETRIC_QSPEC(luminous_absorptance, luminous_flux_of<Condition> / luminous_flux_of<Condition>,
                           non_negative);
QUANTITY_SPEC(reflectance, radiant_flux / radiant_flux, non_negative);
MP_UNITS_PHOTOMETRIC_QSPEC(luminous_reflectance, luminous_flux_of<Condition> / luminous_flux_of<Condition>,
                           non_negative);
QUANTITY_SPEC(transmittance, radiant_flux / radiant_flux, non_negative);
MP_UNITS_PHOTOMETRIC_QSPEC(luminous_transmittance, luminous_flux_of<Condition> / luminous_flux_of<Condition>,
                           non_negative);
QUANTITY_SPEC(radiance_factor, radiance / radiance, non_negative);
MP_UNITS_PHOTOMETRIC_QSPEC(luminance_factor, luminance_of<Condition> / luminance_of<Condition>, non_negative);
QUANTITY_SPEC(reflectance_factor, radiant_flux / radiant_flux, non_negative);
QUANTITY_SPEC(linear_attenuation_coefficient, inverse(length), non_negative);
QUANTITY_SPEC(linear_absorption_coefficient, linear_attenuation_coefficient, non_negative);
QUANTITY_SPEC(mass_attenuation_coefficient, linear_attenuation_coefficient / mass_density, non_negative);
QUANTITY_SPEC(mass_absorption_coefficient, mass_attenuation_coefficient, linear_absorption_coefficient / mass_density,
              non_negative);
QUANTITY_SPEC(molar_absorption_coefficient, linear_absorption_coefficient* volume / amount_of_substance, non_negative);

}  // namespace mp_units::isq
