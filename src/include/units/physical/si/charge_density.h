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
#include <units/physical/si/electric_charge.h>
#include <units/physical/si/length.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::physical::si {

struct coulomb_per_metre_cub : unit<coulomb_per_metre_cub> {};
struct coulomb_per_metre_sq : unit<coulomb_per_metre_sq> {};

struct dim_charge_density : physical::dim_charge_density<dim_charge_density, coulomb_per_metre_cub, dim_electric_charge, dim_length> {};
struct dim_surface_charge_density : physical::dim_surface_charge_density<dim_surface_charge_density, coulomb_per_metre_sq, dim_electric_charge, dim_length> {};

template<in_unit U, NumericValue Rep = double>
using charge_density = quantity<dim_charge_density, U, Rep>;

template<in_unit U, NumericValue Rep = double>
using surface_charge_density = quantity<dim_surface_charge_density, U, Rep>;

inline namespace literals {

// C/m³
constexpr auto operator"" q_C_per_m3(unsigned long long l) { return charge_density<coulomb_per_metre_cub, std::int64_t>(l); }
constexpr auto operator"" q_C_per_m3(long double l) { return charge_density<coulomb_per_metre_cub, long double>(l); }

// C/m²
constexpr auto operator"" q_C_per_m2(unsigned long long l) { return surface_charge_density<coulomb_per_metre_sq, std::int64_t>(l); }
constexpr auto operator"" q_C_per_m2(long double l) { return surface_charge_density<coulomb_per_metre_sq, long double>(l); }

}  // namespace literals

}  // namespace units::physical::si
