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
#include <units/physical/si/energy.h>
#include <units/physical/si/mass.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::si {

struct gray : named_unit<gray, "Gy", prefix> {};
struct milligray : prefixed_unit<milligray, milli, gray> {};
struct kilogray : prefixed_unit<kilogray, kilo, gray> {};

struct dim_absorbed_dose : physical::dim_absorbed_dose<dim_absorbed_dose, gray, dim_energy, dim_mass> {};

template<Unit U, Scalar Rep = double>
using absorbed_dose = quantity<dim_absorbed_dose, U, Rep>;

inline namespace literals {

// G
constexpr auto operator"" q_Gy(unsigned long long l) { return absorbed_dose<gray, std::int64_t>(l); }
constexpr auto operator"" q_Gy(long double l) { return absorbed_dose<gray, long double>(l); }

// mGy
constexpr auto operator"" q_mGy(unsigned long long l) { return absorbed_dose<milligray, std::int64_t>(l); }
constexpr auto operator"" q_mGy(long double l) { return absorbed_dose<milligray, long double>(l); }

// kGy
constexpr auto operator"" q_kGy(unsigned long long l) { return absorbed_dose<kilogray, std::int64_t>(l); }
constexpr auto operator"" q_kGy(long double l) { return absorbed_dose<kilogray, long double>(l); }

}  // namespace literals

}  // namespace units::si
