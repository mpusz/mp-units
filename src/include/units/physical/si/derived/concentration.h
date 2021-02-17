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

#include <units/physical/dimensions/concentration.h>
#include <units/physical/si/base/length.h>
#include <units/physical/si/base/amount_of_substance.h>
#include <units/quantity.h>

namespace units::physical::si {

struct mol_per_metre_cub : unit<mol_per_metre_cub> {};
struct dim_concentration : physical::dim_concentration<dim_concentration, mol_per_metre_cub, dim_amount_of_substance, dim_length> {};

template<UnitOf<dim_concentration> U, QuantityValue Rep = double>
using concentration = quantity<dim_concentration, U, Rep>;

inline namespace literals {

// mol/m³
consteval auto operator"" _q_mol_per_m3(unsigned long long l) { gsl_Expects(std::in_range<std::int64_t>(l)); return concentration<mol_per_metre_cub, std::int64_t>(static_cast<std::int64_t>(l)); }
consteval auto operator"" _q_mol_per_m3(long double l) { return concentration<mol_per_metre_cub, long double>(l); }

}  // namespace literals

}  // namespace units::physical::si

