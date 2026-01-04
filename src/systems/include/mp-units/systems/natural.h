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
#include <mp-units/framework/dimension.h>
#include <mp-units/framework/quantity_spec.h>
#include <mp-units/systems/si/prefixes.h>

// IWYU pragma: begin_exports
#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework.h>
#endif
// IWYU pragma: end_exports

MP_UNITS_EXPORT
namespace mp_units::natural {

// clang-format off
// dimension and base quantity for natural units
inline constexpr struct dim_energy final : base_dimension<"E"> {} dim_energy;
QUANTITY_SPEC(energy, dim_energy);

// Specialized quantities of kind energy (all have dimension energy but provide type safety)
// Functions expecting mass won't accept energy without explicit cast
QUANTITY_SPEC(mass, energy);
QUANTITY_SPEC(momentum, energy);
QUANTITY_SPEC(temperature, energy);
QUANTITY_SPEC(acceleration, energy);  // dv/dt where v is dimensionless and t is 1/energy

// Derived quantities - define base quantities first, then specializations
QUANTITY_SPEC(inverse_energy, inverse(energy));
QUANTITY_SPEC(time, inverse_energy);
QUANTITY_SPEC(length, inverse_energy);

QUANTITY_SPEC(energy_squared, pow<2>(energy));
QUANTITY_SPEC(force, energy_squared);

// Dimensionless quantities (all ratios with c = 1)
QUANTITY_SPEC(speed, dimensionless);
QUANTITY_SPEC(velocity, speed);
QUANTITY_SPEC(angular_measure, dimensionless);

// units
inline constexpr struct electronvolt final : named_unit<"eV", kind_of<energy>> {} electronvolt;
inline constexpr auto gigaelectronvolt = si::giga<electronvolt>;

// In natural units (ℏ = c = 1), all quantities are expressed in powers of GeV:
// - Energy, mass, momentum, temperature, acceleration: GeV (dimension: energy)
// - Time, length: GeV⁻¹ (dimension: inverse_energy)
// - Speed, velocity, angular_measure: dimensionless (v/c, θ)
// - Force: GeV² (dimension: energy_squared)
//
// The quantity_spec hierarchy provides type safety at function boundaries:
//   void compute(QuantityOf<natural::mass> auto m) { ... }           // accepts only mass
//   void compute(QuantityOf<natural::energy> auto e) { ... }         // accepts energy, mass, momentum, temperature, acceleration
//   void compute(QuantityOf<natural::time> auto t) { ... }           // accepts only time
//   void compute(QuantityOf<natural::inverse_energy> auto x) { ... } // accepts time, length
//   void compute(QuantityOf<natural::speed> auto v) { ... }          // accepts speed, velocity
//   void compute(QuantityOf<dimensionless> auto x) { ... }           // accepts all dimensionless quantities
//
// While all these quantities share related dimensions in natural units,
// the specialized quantity types prevent accidental misuse.

// Physical constants
// Speed of light is dimensionless 1 in natural units
inline constexpr auto speed_of_light = one;
// clang-format on

namespace unit_symbols {

inline constexpr auto GeV = gigaelectronvolt;
inline constexpr auto GeV2 = square(gigaelectronvolt);

}  // namespace unit_symbols

}  // namespace mp_units::natural
