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
#include <mp-units/systems/isq/base_quantities.h>
#include <mp-units/systems/isq/electromagnetism.h>
#include <mp-units/systems/isq/space_and_time.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/quantity_spec.h>
#endif

MP_UNITS_EXPORT
namespace mp_units::isq {

QUANTITY_SPEC(speed_of_light_in_a_medium, speed);
QUANTITY_SPEC(refractive_index, dimensionless, speed_of_light_in_vacuum / speed_of_light_in_a_medium);
QUANTITY_SPEC(radiant_energy, energy);
QUANTITY_SPEC(spectral_radiant_energy, radiant_energy / wavelength);
QUANTITY_SPEC(radiant_energy_density, radiant_energy / volume);
QUANTITY_SPEC(spectral_radiant_energy_density_in_terms_of_wavelength, radiant_energy_density / wavelength);
QUANTITY_SPEC(spectral_radiant_energy_density_in_terms_of_wavenumber, radiant_energy_density / wavenumber);
QUANTITY_SPEC(radiant_flux, power, radiant_energy / time);
inline constexpr auto radiant_power = radiant_flux;
QUANTITY_SPEC(spectral_radiant_flux, radiant_flux / wavelength);
inline constexpr auto spectral_radiant_power = spectral_radiant_flux;
QUANTITY_SPEC(radiant_intensity, radiant_flux / solid_angular_measure);
QUANTITY_SPEC(spectral_radiant_intensity, radiant_intensity / wavelength);
QUANTITY_SPEC(radiance, radiant_intensity / area);
QUANTITY_SPEC(spectral_radiance, radiance / wavelength);
QUANTITY_SPEC(irradiance, radiant_flux / area);
QUANTITY_SPEC(spectral_irradiance, irradiance / wavelength);
QUANTITY_SPEC(radiant_exitance, radiant_flux / area);
QUANTITY_SPEC(spectral_radiant_exitance, radiant_exitance / wavelength);
QUANTITY_SPEC(radiant_exposure, radiant_energy / area);
QUANTITY_SPEC(spectral_radiant_exposure, radiant_exposure / wavelength);

}  // namespace mp_units::isq
