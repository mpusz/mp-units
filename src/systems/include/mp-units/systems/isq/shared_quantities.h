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
#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework/quantity_spec.h>
#endif
// IWYU pragma: end_exports

// Parents shared by quantities of more than one ISO 80000 part, for the case when the parts must
// not depend on each other. A photon count (part 7) and an entity count (part 9) need the same
// parent, yet light and radiation is not a client of physical chemistry, so the parent cannot live
// in either part's header. This one belongs to no part in particular and depends on the framework
// alone, so any part may include it.
//
// This is the last resort. A quantity that one part owns belongs in that part's header, and when a
// part already depends on another, the shared parent belongs in that existing dependency (this is
// why `energy_density` sits in `mechanics.h`, which parts 6, 7, and 8 already include).

MP_UNITS_EXPORT
namespace mp_units::isq {

// the shared parent for every count of discrete entities (photons, particles, vibrational modes,
// ...), named after ISO 80000-9 item 9-1. A count is not a plain ratio, so it is a kind of its own,
// and counts whose cross-addition is meaningless root their own kind below it
QUANTITY_SPEC(number_of_entities, dimensionless, is_kind, non_negative);

}  // namespace mp_units::isq
