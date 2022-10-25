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

#include <units/isq/base_dimensions.h>
#include <units/si/prefixes.h>
#include <units/unit.h>

namespace units::si {

// clang-format off
// base units
inline constexpr struct second : named_unit<"s", isq::time> {} second;
inline constexpr struct metre : named_unit<"m", isq::length> {} metre;
inline constexpr struct gram : named_unit<"g", isq::mass> {} gram;
inline constexpr struct kilogram : decltype(kilo<gram>) {} kilogram;
inline constexpr struct ampere : named_unit<"A", isq::electric_current> {} ampere;
inline constexpr struct kelvin : named_unit<"K", isq::thermodynamic_temperature> {} kelvin;
inline constexpr struct mole : named_unit<"mol", isq::amount_of_substance> {} mole;
inline constexpr struct candela : named_unit<"cd", isq::luminous_intensity> {} candela;

// derived named units
inline constexpr struct radian : named_unit<"rad", metre / metre> {} radian;
inline constexpr struct steradian : named_unit<"sr", square<metre> / square<metre>> {} steradian;
inline constexpr struct hertz : named_unit<"Hz", 1 / second> {} hertz;
inline constexpr struct newton : named_unit<"N", kilogram * metre / square<second>> {} newton;
inline constexpr struct pascal : named_unit<"Pa", newton / square<metre>> {} pascal;
inline constexpr struct joule : named_unit<"J", newton * metre> {} joule;
inline constexpr struct watt : named_unit<"W", joule / second> {} watt;
inline constexpr struct coulomb : named_unit<"C", ampere * second> {} coulomb;
inline constexpr struct volt : named_unit<"V", watt / ampere> {} volt;
inline constexpr struct farad : named_unit<"F", coulomb / volt> {} farad;
inline constexpr struct ohm : named_unit<basic_symbol_text{"\u03A9", "ohm"}, volt / ampere> {} ohm;
inline constexpr struct siemens : named_unit<"S", 1 / ohm> {} siemens;
inline constexpr struct weber : named_unit<"Wb", volt * second> {} weber;
inline constexpr struct tesla : named_unit<"T", weber / square<metre>> {} tesla;
inline constexpr struct henry : named_unit<"H", weber / ampere> {} henry;
inline constexpr struct degree_Celsius : named_unit<basic_symbol_text{"\u00B0C", "`C"}, kelvin> {} degree_Celsius;
inline constexpr struct lumen : named_unit<"lm", candela * steradian> {} lumen;
inline constexpr struct lux : named_unit<"lx", lumen / square<metre>> {} lux;
inline constexpr struct becquerel : named_unit<"Bq", 1 / second> {} becquerel;
inline constexpr struct gray : named_unit<"Gy", joule / kilogram> {} gray;
inline constexpr struct sievert : named_unit<"Sv", joule / kilogram> {} sievert;
inline constexpr struct katal : named_unit<"kat", mole / second> {} katal;

// non-SI units accepted for use with the SI
inline constexpr struct minute : named_unit<"min", mag<60> * second> {} minute;
inline constexpr struct hour : named_unit<"h", mag<60> * minute> {} hour;
inline constexpr struct day : named_unit<"d", mag<24> * hour> {} day;
inline constexpr struct astronomical_unit : named_unit<"au", mag<149'597'870'700> * metre> {} astronomical_unit;
inline constexpr struct degree : named_unit<basic_symbol_text{"°", "deg"}, mag_pi / mag<180> * radian> {} degree;
// TODO how to disambiguate below angular units from time units
// inline constexpr struct minute : named_unit<basic_symbol_text{"′", "'"}, mag<ratio{1, 60}> * degree> {} minute;
// inline constexpr struct second : named_unit<basic_symbol_text{"″", "''"}, mag<ratio{1, 60}> * minute> {} second;
inline constexpr struct are : named_unit<"a", square<deca<metre>>> {} are;
inline constexpr struct hectare : decltype(hecto<are>) {} hectare;
inline constexpr struct litre : named_unit<"l", cubic<deci<metre>>> {} litre;
inline constexpr struct tonne : named_unit<"t", mag<1000> * kilogram> {} tonne;
inline constexpr struct dalton : named_unit<"Da", mag<ratio{16'605'390'666'050, 10'000'000'000'000}> * mag_power<10, -27> * kilogram> {} dalton;
// TODO A different value is provided in the SI Brochure and different in the ISO 80000
inline constexpr struct electronvolt : named_unit<"eV", mag<ratio{1'602'176'634, 1'000'000'000}> * mag_power<10, -19> * joule> {} electronvolt;
// TODO the below are logarithmic units - how to support those?
// neper
// bel
// decibel

// clang-format on

}  // namespace units::si

namespace units {

template<>
inline constexpr bool unit_can_be_prefixed<si::degree_Celsius> = false;  // TODO Is it true?
template<>
inline constexpr bool unit_can_be_prefixed<si::minute> = false;
template<>
inline constexpr bool unit_can_be_prefixed<si::hour> = false;
template<>
inline constexpr bool unit_can_be_prefixed<si::day> = false;

}  // namespace units
