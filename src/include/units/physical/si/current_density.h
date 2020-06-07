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
#include <units/physical/si/current.h>
#include <units/physical/si/length.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::physical::si {

struct ampere_per_metre_sq : unit<ampere_per_metre_sq> {};

struct dim_current_density : physical::dim_current_density<dim_current_density, ampere_per_metre_sq, dim_electric_current, dim_length> {};

template<in_unit U, in_numeric_value Rep = double>
using current_density = quantity<dim_current_density, U, Rep>;

inline namespace literals {

constexpr auto operator"" q_A_per_m2(unsigned long long l) { return current_density<ampere_per_metre_sq, std::int64_t>(l); }
constexpr auto operator"" q_A_per_m2(long double l) { return current_density<ampere_per_metre_sq, long double>(l); }

}  // namespace literals

}  // namespace units::physical::si
