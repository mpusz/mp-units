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
QUANTITY_SPEC(energy, dim_energy, non_negative);

// Base energy kind — no defining equation beyond its parent
// mass: E₀ = mc²  →  with c = 1: E₀ = m
QUANTITY_SPEC(mass, energy);

// Base inverse-energy kind — no defining equation
// duration: τ = ℏ/Γ  →  with ℏ = 1: τ = 1/Γ
QUANTITY_SPEC(inverse_energy, inverse(energy));
QUANTITY_SPEC(duration, inverse_energy);

// length: ƛ = ℏ/(mc)  →  with ℏ = c = 1: length = 1/energy
//         (reduced Compton wavelength; equivalently, x and p are conjugate via Δx·Δp ≥ ℏ/2)
QUANTITY_SPEC(length, inverse_energy);

// Quantity with dimension energy²
QUANTITY_SPEC(energy_squared, pow<2>(energy));


// Dimensionless quantities. Only angular_measure is a kind (is_kind prevents implicit cross-conversion)
// speed: β = v/c, a plain ratio (not a kind) and the root of the velocity hierarchy
// velocity: directional speed
// angular_measure: θ = arc/radius; separate kind — angles are not speeds
QUANTITY_SPEC(speed,           dimensionless,  length / duration);            // v = l/t, a plain ratio (beta)
QUANTITY_SPEC(velocity,        speed);
QUANTITY_SPEC(angular_measure, dimensionless, is_kind);

// Derived quantities with explicit quantity equations
QUANTITY_SPEC(momentum,     energy,         mass * velocity);      // p = mv
QUANTITY_SPEC(acceleration, energy,         velocity / duration);      // a = dv/dt
QUANTITY_SPEC(force,        energy_squared, mass * acceleration);  // F = ma

// units
inline constexpr struct electronvolt final : named_unit<"eV", kind_of<energy>> {} electronvolt;
inline constexpr auto gigaelectronvolt = si::giga<electronvolt>;

// In natural units (ℏ = c = 1), all quantities are expressed in powers of GeV:
// - Energy, mass, momentum, acceleration: GeV  (dimension: energy)
// - Duration, length:                     GeV⁻¹ (dimension: inverse_energy)
// - Speed, velocity, angular_measure:     dimensionless (v/c, θ)
// - Force:                                GeV² (dimension: energy_squared)
//
// The quantity_spec hierarchy provides type safety at function boundaries.
//
// Non-template (fixes unit and rep; quantity-spec form adds kind constraint):
//   void compute(quantity<GeV, double> e) { ... }                             // accepts any quantity of energy kind (energy, mass, momentum, acceleration)
//   void compute(quantity<natural::energy[GeV], double> e) { ... }            // accepts any energy (energy, mass, momentum, acceleration)
//   void compute(quantity<natural::mass[GeV], double> m) { ... }              // accepts only mass
//   void compute(quantity<inverse(GeV), double> x) { ... }                    // accepts any inverse-energy quantity (duration, length)
//   void compute(quantity<natural::duration[inverse(GeV)], double> t) { ... } // accepts only duration
//   void compute(quantity<one, double> x) { ... }                             // accepts any dimensionless quantity (speed, velocity, angular_measure)
//   void compute(quantity<natural::speed[one], double> v) { ... }             // accepts speed and velocity (not angular_measure)
//
// Template (accepts any unit/rep of the right kind):
//   void compute(QuantityOf<natural::energy> auto e) { ... }         // accepts energy, mass, momentum, acceleration
//   void compute(QuantityOf<natural::mass> auto m) { ... }           // accepts only mass
//   void compute(QuantityOf<natural::inverse_energy> auto x) { ... } // accepts duration, length
//   void compute(QuantityOf<natural::duration> auto t) { ... }       // accepts only duration
//   void compute(QuantityOf<natural::speed> auto v) { ... }          // accepts speed, velocity (not angular_measure)
//
// While all these quantities share related dimensions in natural units,
// the specialized quantity types prevent accidental misuse.

// Physical constants
// Speed of light is dimensionless 1 in natural units
inline constexpr struct speed_of_light_in_vacuum final : named_constant<"c", one> {} speed_of_light_in_vacuum;
// Reduced Planck constant is dimensionless 1 in natural units
inline constexpr struct reduced_planck_constant final : named_constant<symbol_text{u8"\u210f", "hbar"}, one> {} reduced_planck_constant;
// clang-format on

namespace unit_symbols {

inline constexpr auto GeV = gigaelectronvolt;
inline constexpr auto GeV2 = square(gigaelectronvolt);
inline constexpr auto c = speed_of_light_in_vacuum;
inline constexpr auto hbar = reduced_planck_constant;

}  // namespace unit_symbols

}  // namespace mp_units::natural
