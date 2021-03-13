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

#include <units/physical/dimensions/energy_density.h>
#include <units/physical/si/derived/energy.h>
#include <units/physical/si/derived/volume.h>
#include <units/quantity.h>

namespace units::physical::si {

struct joule_per_metre_cub : unit<joule_per_metre_cub> {};
struct dim_energy_density : physical::dim_energy_density<dim_energy_density, joule_per_metre_cub, dim_energy, dim_volume> {};

template<UnitOf<dim_energy_density> U, QuantityValue Rep = double>
using energy_density = quantity<dim_energy_density, U, Rep>;

inline namespace literals {

// N/m
constexpr auto operator"" _q_J_per_m3(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return energy_density<joule_per_metre_cub, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_J_per_m3(long double l) { return energy_density<joule_per_metre_cub, long double>(l); }

}  // namespace literals

}  // namespace units::physical::si
