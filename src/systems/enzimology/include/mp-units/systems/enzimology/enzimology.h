// The MIT License (MIT)
//
// Copyright (c) 2023 Audrius Meskauskas
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

// REFERENCES:
// Table 1 in Hans Bisswanger (2014). Enzyme assays. Perspectives in Science. Volume 1, Issues 1–6, May 2014, Pages 41-55
// https://www.sciencedirect.com/science/article/pii/S2213020914000068 or https://doi.org/10.1016/j.pisc.2014.02.005
// This article is under Creative Commons license, CC BY 3.0, this reference should be retained.

#pragma once

#include "mp-units/systems/si/units.h"
#include <mp-units/dimension.h>
#include <mp-units/quantity_spec.h>
#include <mp-units/systems/si/unit_symbols.h>
#include <mp-units/unit.h>

namespace mp_units::enzimology {

// clang-format off

// 1 unit (U) is the amount of enzyme that catalyses the reaction of 1 micro-mol of substrate per minute
// It is also marked as IU and named "international unit" in other sources so define these as compatible synonims.
inline constexpr struct unit : named_unit<"U", si::micro<si::mole> / non_si::minute> {} unit;
inline constexpr struct iu : named_unit<"IU", si::micro<si::mole> / non_si::minute> {} iu;

// Catal is more recently proposed another unit. One U is about 16.67 nkat.
inline constexpr struct katal : named_unit<"kat", si::mole / si::second> {} katal;

// katal is a very large unit unlikely to be used. As recommeded per article, define immediately  nanokatal as more practical
inline constexpr struct nanokatal : named_unit<"nkat", mag_power<10, -9> * katal> {} nanokatal;

// Activity is quoted in units per ml (U/ml),
inline constexpr struct activity : named_unit<"U/ml", unit / si::milli<non_si::litre>> {} activity;

//  ‘specific activity’ is the number of enzyme units per ml divided by the concentration of protein in mg/ml
//  Hence the actual unit of measurement is U/mg.
inline constexpr struct specific_activity : named_unit<"U/mg", unit / si::milli<si::gram> > {} specific_activity;

// clang-format on

namespace unit_symbols {

inline constexpr auto U = unit;
inline constexpr auto IU = iu;
inline constexpr auto A = activity;
inline constexpr auto kat = katal;

inline constexpr auto nkat = nanokatal;
inline constexpr auto mkat = si::micro<katal>;
inline constexpr auto pkat = si::pico<katal>;

}  // namespace unit_symbols

}  // namespace mp_units::enzimology

