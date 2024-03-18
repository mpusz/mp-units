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
#include <mp-units/systems/isq/si_quantities.h>
#include <mp-units/systems/si/prefixes.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/quantity_point.h>
#include <mp-units/unit.h>
#endif

MP_UNITS_EXPORT
namespace mp_units {

namespace si {

// clang-format off
// base units
inline constexpr struct second : named_unit<"s", kind_of<isq::time>> {} second;
inline constexpr struct metre : named_unit<"m", kind_of<isq::length>> {} metre;
inline constexpr struct gram : named_unit<"g", kind_of<isq::mass>> {} gram;
#if MP_UNITS_COMP_MSVC
inline constexpr struct kilogram : kilo_<gram> {} kilogram;
#else
inline constexpr struct kilogram : decltype(kilo<gram>) {} kilogram;
#endif
inline constexpr struct ampere : named_unit<"A", kind_of<isq::electric_current>> {} ampere;

inline constexpr struct absolute_zero : absolute_point_origin<absolute_zero, isq::thermodynamic_temperature> {} absolute_zero;
inline constexpr struct zeroth_kelvin : decltype(absolute_zero) {} zeroth_kelvin;
inline constexpr struct kelvin : named_unit<"K", kind_of<isq::thermodynamic_temperature>, zeroth_kelvin> {} kelvin;

inline constexpr struct mole : named_unit<"mol", kind_of<isq::amount_of_substance>> {} mole;
inline constexpr struct candela : named_unit<"cd", kind_of<isq::luminous_intensity>> {} candela;

// derived named units
inline constexpr struct radian : named_unit<"rad", metre / metre, kind_of<isq::angular_measure>> {} radian;
inline constexpr struct steradian : named_unit<"sr", square(metre) / square(metre), kind_of<isq::solid_angular_measure>> {} steradian;
inline constexpr struct hertz : named_unit<"Hz", one / second, kind_of<isq::frequency>> {} hertz;
inline constexpr struct newton : named_unit<"N", kilogram * metre / square(second)> {} newton;
#ifdef pascal
#pragma push_macro("pascal")
#undef pascal
#define MP_UNITS_REDEFINE_PASCAL
#endif
inline constexpr struct pascal : named_unit<"Pa", newton / square(metre)> {} pascal;
#ifdef MP_UNITS_REDEFINE_PASCAL
#pragma pop_macro("pascal")
#undef MP_UNITS_REDEFINE_PASCAL
#endif
inline constexpr struct joule : named_unit<"J", newton * metre> {} joule;
inline constexpr struct watt : named_unit<"W", joule / second> {} watt;
inline constexpr struct coulomb : named_unit<"C", ampere * second> {} coulomb;
inline constexpr struct volt : named_unit<"V", watt / ampere> {} volt;
inline constexpr struct farad : named_unit<"F", coulomb / volt> {} farad;
inline constexpr struct ohm : named_unit<symbol_text{u8"Ω", "ohm"}, volt / ampere> {} ohm;
inline constexpr struct siemens : named_unit<"S", one / ohm> {} siemens;
inline constexpr struct weber : named_unit<"Wb", volt * second> {} weber;
inline constexpr struct tesla : named_unit<"T", weber / square(metre)> {} tesla;
inline constexpr struct henry : named_unit<"H", weber / ampere> {} henry;

inline constexpr struct ice_point : relative_point_origin<quantity_point{273'150 * milli<kelvin>}> {} ice_point;
inline constexpr struct zeroth_degree_Celsius : decltype(ice_point) {} zeroth_degree_Celsius;
inline constexpr struct degree_Celsius : named_unit<symbol_text{u8"°C", "`C"}, kelvin, zeroth_degree_Celsius> {} degree_Celsius;

inline constexpr struct lumen : named_unit<"lm", candela * steradian> {} lumen;
inline constexpr struct lux : named_unit<"lx", lumen / square(metre)> {} lux;
inline constexpr struct becquerel : named_unit<"Bq", one / second, kind_of<isq::activity>> {} becquerel;
inline constexpr struct gray : named_unit<"Gy", joule / kilogram, kind_of<isq::absorbed_dose>> {} gray;
inline constexpr struct sievert : named_unit<"Sv", joule / kilogram, kind_of<isq::dose_equivalent>> {} sievert;
inline constexpr struct katal : named_unit<"kat", mole / second> {} katal;
// clang-format on

}  // namespace si

namespace non_si {

// clang-format off
// non-SI units accepted for use with the SI
inline constexpr struct minute : named_unit<"min", mag<60> * si::second> {} minute;
inline constexpr struct hour : named_unit<"h", mag<60> * minute> {} hour;
inline constexpr struct day : named_unit<"d", mag<24> * hour> {} day;
inline constexpr struct astronomical_unit : named_unit<"au", mag<149'597'870'700> * si::metre> {} astronomical_unit;
inline constexpr struct degree : named_unit<symbol_text{u8"°", "deg"}, mag_pi / mag<180> * si::radian> {} degree;
inline constexpr struct arcminute : named_unit<symbol_text{u8"′", "'"}, mag<ratio{1, 60}> * degree> {} arcminute;
inline constexpr struct arcsecond : named_unit<symbol_text{u8"″", "''"}, mag<ratio{1, 60}> * arcminute> {} arcsecond;
inline constexpr struct are : named_unit<"a", square(si::deca<si::metre>)> {} are;
#if MP_UNITS_COMP_MSVC
inline constexpr struct hectare : si::hecto_<are> {} hectare;
#else
inline constexpr struct hectare : decltype(si::hecto<are>) {} hectare;
#endif
inline constexpr struct litre : named_unit<"l", cubic(si::deci<si::metre>)> {} litre;
inline constexpr struct tonne : named_unit<"t", mag<1000> * si::kilogram> {} tonne;
inline constexpr struct dalton : named_unit<"Da", mag<ratio{16'605'390'666'050, 10'000'000'000'000}> * mag_power<10, -27> * si::kilogram> {} dalton;
// TODO A different value is provided in the SI Brochure and different in the ISO 80000
inline constexpr struct electronvolt : named_unit<"eV", mag<ratio{1'602'176'634, 1'000'000'000}> * mag_power<10, -19> * si::joule> {} electronvolt;
// TODO the below are logarithmic units - how to support those?
// neper
// bel
// decibel
// clang-format on

}  // namespace non_si

namespace si {

// Non-SI units are accepted for use with SI
using namespace non_si;

}  // namespace si

template<>
inline constexpr bool unit_can_be_prefixed<si::degree_Celsius> = false;
template<>
inline constexpr bool unit_can_be_prefixed<non_si::minute> = false;
template<>
inline constexpr bool unit_can_be_prefixed<non_si::hour> = false;
template<>
inline constexpr bool unit_can_be_prefixed<non_si::day> = false;

template<>
inline constexpr bool space_before_unit_symbol<non_si::degree> = false;
template<>
inline constexpr bool space_before_unit_symbol<non_si::arcminute> = false;
template<>
inline constexpr bool space_before_unit_symbol<non_si::arcsecond> = false;

}  // namespace mp_units
