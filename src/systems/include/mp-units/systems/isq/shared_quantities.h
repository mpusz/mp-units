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

// IWYU pragma: begin_exports
#include <mp-units/systems/isq/space_and_time.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework/quantity_spec.h>
#endif
// IWYU pragma: end_exports

// Quantities that several ISO 80000 parts need, kept here so that a part does not have to include
// another part's whole header to reach one definition. The motivation is compile time: every
// `QUANTITY_SPEC` with a defining equation checks it against its parent at parse time, used or not,
// so moving two definitions out of parts 4 and 6 halved the cost of `light_and_radiation.h`.

MP_UNITS_EXPORT
namespace mp_units::isq {

// not in ISO 80000; parents of the mechanical, electromagnetic, radiant, and sound variants. Given
// dimensionally rather than as `energy / time` and `energy / volume` because the children use
// unrelated equations (ISO 80000-8 item 8-7 gives sound energy density as ½ρu² + ½p²/(ρc²))
// not non_negative: a load absorbs power while a source delivers it, so the sign records direction
QUANTITY_SPEC(power, mass* pow<2>(length) / pow<3>(time), possibly_negative);
QUANTITY_SPEC(energy_density, mass / (length * pow<2>(time)), non_negative);

// ISO 80000-4 item 4-2; parents the mass concentrations of part 5, divides those of part 7
QUANTITY_SPEC(mass_density, mass / volume, non_negative);
inline constexpr auto density = mass_density;

// ISO 80000-6 item 6-35.2; part 7 needs it for the refractive index (item 7-1.2)
QUANTITY_SPEC(speed_of_light_in_vacuum, speed);
inline constexpr auto light_speed_in_vacuum = speed_of_light_in_vacuum;
inline constexpr auto luminal_speed = speed_of_light_in_vacuum;

// ISO 80000-9 item 9-1; parent of every count of discrete entities. A count is not a plain ratio, so
// it is a kind of its own, and counts that must not be added root their own kind below it
QUANTITY_SPEC(number_of_entities, dimensionless, is_kind, non_negative);

}  // namespace mp_units::isq
