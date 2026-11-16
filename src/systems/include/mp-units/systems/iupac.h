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

#include <mp-units/bits/module_macros.h>
#include <mp-units/systems/codata/adopted_values.h>
#include <mp-units/systems/si/prefixes.h>
#include <mp-units/systems/si/units.h>

// IWYU pragma: begin_exports
#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework.h>
#endif
// IWYU pragma: end_exports

MP_UNITS_EXPORT
namespace mp_units::iupac {

// IUPAC "Quantities, Units and Symbols in Physical Chemistry" (the Green Book), 3rd edition,
// section 7.2 "Conversion tables for units". Only the units that no other system header provides
// are defined here, and each of them is exact by definition.

// clang-format off
// pressure
// The standard atmosphere is the CGPM 1954 adopted value and already exists as
// `codata::standard_atmosphere`, so it is not redefined here; `unit_symbols::atm` names it.

// exactly 1/760 of the standard atmosphere; not the same unit as the millimetre of mercury
inline constexpr struct torr final : named_unit<"Torr", mag_ratio<1, 760> * codata::standard_atmosphere> {} torr;

// The conventional millimetre of mercury: the pressure of a 1 mm column of a fluid of density
// 13 595.1 kg/m³ under the standard gravity, exactly 133.322 387 415 Pa. It is not a measurement
// of any actual mercury column.
inline constexpr struct millimetre_of_mercury final : named_unit<"mmHg", mag_ratio<135'951, 10> * si::kilogram / cubic(si::metre) * codata::standard_gravity * si::milli<si::metre>> {} millimetre_of_mercury;

// energy
inline constexpr struct thermochemical_calorie final : named_unit<"cal_th", mag_ratio<4'184, 1'000> * si::joule> {} thermochemical_calorie;
inline constexpr struct international_table_calorie final : named_unit<"cal_IT", mag_ratio<41'868, 10'000> * si::joule> {} international_table_calorie;
// clang-format on

namespace unit_symbols {

inline constexpr auto atm = codata::standard_atmosphere;
inline constexpr auto Torr = torr;
inline constexpr auto mmHg = millimetre_of_mercury;

inline constexpr auto cal_th = thermochemical_calorie;
inline constexpr auto cal_IT = international_table_calorie;

}  // namespace unit_symbols

}  // namespace mp_units::iupac
