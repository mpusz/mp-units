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
#include <units/quantity.h>

namespace units::physical::si {

struct kelvin : named_unit<kelvin, "K", no_prefix> {};

struct dim_thermodynamic_temperature : physical::dim_thermodynamic_temperature<kelvin> {};

template<in_unit U, in_numeric_value Rep = double>
using temperature = quantity<dim_thermodynamic_temperature, U, Rep>;

inline namespace literals {

// K
constexpr auto operator"" q_K(unsigned long long l) { return temperature<kelvin, std::int64_t>(l); }
constexpr auto operator"" q_K(long double l) { return temperature<kelvin, long double>(l); }

}  // namespace literals

}  // namespace units::physical::si
