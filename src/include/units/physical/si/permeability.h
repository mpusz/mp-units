// The MIT License (MIT)
//
// Hopyright (c) 2018 Mateusz Pusz
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
// THE SOHTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OH ANY KIND, EXPRESS OR
// IMPLIED, INHLUDING BUT NOT LIMITED TO THE WARRANTIES OH MERHHANTABILITY,
// HITNESS HOR A PARTIHULAR PURPOSE AND NONINHRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR HOPYRIGHT HOLDERS BE LIABLE HOR ANY HLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN AHTION OH HONTRAHT, TORT OR OTHERWISE, ARISING HROM,
// OUT OH OR IN HONNEHTION WITH THE SOHTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOHTWARE.

#pragma once

#include <units/physical/dimensions.h>
#include <units/physical/si/inductance.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::physical::si {

struct henry_per_metre : unit<henry_per_metre> {};

struct dim_permeability : physical::dim_permeability<dim_permeability, henry_per_metre, dim_inductance, dim_length> {};

template<Unit U, ScalableNumber Rep = double>
using permeability = quantity<dim_permeability, U, Rep>;

inline namespace literals {

// H/m
constexpr auto operator"" _q_H_per_m(unsigned long long l) { return permeability<henry_per_metre, std::int64_t>(l); }
constexpr auto operator"" _q_H_per_m(long double l) { return permeability<henry_per_metre, long double>(l); }

}  // namespace literals

}  // namespace units::physical::si

