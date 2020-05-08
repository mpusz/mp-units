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

#include <units/physical/dimensions.h>
#include <units/physical/si/temperature.h>
#include <units/physical/si/energy.h>
#include <units/physical/si/mass.h>
#include <units/physical/si/substance.h>
#include <units/quantity.h>

namespace units::physical::si {

struct joule_per_kelvin : unit<joule_per_kelvin> {};
struct joule_per_kilogram_kelvin : unit<joule_per_kilogram_kelvin> {};
struct joule_per_mole_kelvin : unit<joule_per_mole_kelvin> {};

struct dim_heat_capacity : physical::dim_heat_capacity<dim_heat_capacity, joule_per_kelvin, dim_energy, dim_thermodynamic_temperature> {};
struct dim_specific_heat_capacity : physical::dim_specific_heat_capacity<dim_specific_heat_capacity, joule_per_kilogram_kelvin, dim_heat_capacity, dim_mass> {};
struct dim_molar_heat_capacity : physical::dim_molar_heat_capacity<dim_molar_heat_capacity, joule_per_mole_kelvin, dim_heat_capacity, dim_substance> {};

template<Unit U, Scalar Rep = double>
using heat_capacity = quantity<dim_heat_capacity, U, Rep>;

template<Unit U, Scalar Rep = double>
using specific_heat_capacity = quantity<dim_specific_heat_capacity, U, Rep>;

template<Unit U, Scalar Rep = double>
using molar_heat_capacity = quantity<dim_molar_heat_capacity, U, Rep>;

inline namespace literals {

// J/K
constexpr auto operator"" q_J_per_K(unsigned long long l) { return heat_capacity<joule_per_kelvin, std::int64_t>(l); }
constexpr auto operator"" q_J_per_K(long double l) { return heat_capacity<joule_per_kelvin, long double>(l); }

// J/(kg·K)
constexpr auto operator"" q_J_per_kg_K(unsigned long long l) { return specific_heat_capacity<joule_per_kilogram_kelvin, std::int64_t>(l); }
constexpr auto operator"" q_J_per_kg_K(long double l) { return specific_heat_capacity<joule_per_kilogram_kelvin, long double>(l); }

// J/(mol·K)
constexpr auto operator"" q_J_per_mol_K(unsigned long long l) { return molar_heat_capacity<joule_per_mole_kelvin, std::int64_t>(l); }
constexpr auto operator"" q_J_per_mol_K(long double l) { return molar_heat_capacity<joule_per_mole_kelvin, long double>(l); }

}  // namespace literals

}  // namespace units::physical::si

