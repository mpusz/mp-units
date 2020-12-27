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

#include <units/bits/one_rep.h>
#include <units/physical/dimensions/amount_of_substance.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::physical::si {

struct mole : named_unit<mole, "mol", prefix> {};

struct dim_amount_of_substance : physical::dim_amount_of_substance<mole> {};

template<UnitOf<dim_amount_of_substance> U, QuantityValue Rep = double>
using amount_of_substance = quantity<dim_amount_of_substance, U, Rep>;

inline namespace literals {

// mol
constexpr auto operator"" _q_mol(unsigned long long l) { return amount_of_substance<mole, std::int64_t>(l); }
constexpr auto operator"" _q_mol(long double l) { return amount_of_substance<mole, long double>(l); }

}  // namespace literals

namespace unit_constants {

inline constexpr auto mol = amount_of_substance<mole, detail::one_rep>{};

}  // namespace unit_constants

}  // namespace units::physical::si
