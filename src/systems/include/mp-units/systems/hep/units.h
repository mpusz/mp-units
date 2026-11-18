// The MIT License (MIT)
//
// Copyright (c) 2021 Mateusz Pusz
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

// The HEP system of units is inspired by popular HEP projects:
// - https://gitlab.cern.ch/CLHEP/CLHEP/-/blob/develop/Units/Units/SystemOfUnits.h
// - https://gitlab.cern.ch/gaudi/Gaudi/-/blob/main/GaudiKernel/include/GaudiKernel/SystemOfUnits.h
// - https://gitlab.cern.ch/geant4/geant4/-/blob/master/source/externals/clhep/include/CLHEP/Units/SystemOfUnits.h
// - https://github.com/root-project/root/blob/master/geom/geom/inc/TGeoSystemOfUnits.h

#pragma once

#include <mp-units/bits/module_macros.h>
#include <mp-units/systems/hep/quantities.h>
#include <mp-units/systems/si/prefixes.h>

// IWYU pragma: begin_exports
#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework/unit.h>
#endif
// IWYU pragma: end_exports

/**
 * The system of quantities and units of high energy physics (HEP)
 *
 * _Energy_ and _electric charge_ are base quantities instead of _mass_ and _electric current_.
 * Its conventions follow CLHEP, Gaudi, Geant4, and ROOT, which express _length_ in millimetres,
 * _time_ in nanoseconds, _energy_ in megaelectronvolts, and _electric charge_ in positron
 * charges. The physical constants default to the CODATA 2018 values, and @ref codata2014 and
 * @ref codata2022 hold the 2014 and 2022 values.
 *
 * @code{.cpp}
 * using hep::unit_symbols::GeV;
 * using hep::unit_symbols::MeV;
 *
 * quantity higgs_energy = 125.1 * GeV;
 * quantity in_megaelectronvolts = higgs_energy.in(MeV);  // 125100 MeV
 * @endcode
 *
 * @see [High Energy Physics (HEP) System](docs:users_guide/systems/hep) in the User's Guide
 * @see [HEP System](docs:reference/systems_reference/systems/hep) for every entity it defines
 */
MP_UNITS_EXPORT
namespace mp_units::hep {

// dimensionless units
inline constexpr auto pi = ::mp_units::pi;
inline constexpr auto twopi = mag<2> * pi;
inline constexpr auto halfpi = mag_ratio<1, 2> * pi;
inline constexpr auto pi2 = square(pi);
inline constexpr auto perCent = ::mp_units::percent;
inline constexpr auto perThousand = ::mp_units::per_mille;
inline constexpr auto perMillion = ::mp_units::parts_per_million;

// named units
// clang-format off
inline constexpr struct meter final : named_unit<"m", kind_of<length>> {} meter;
inline constexpr struct angstrom final : named_unit<symbol_text{u8"Å", "A"}, mag_power<10, -10> * meter> {} angstrom;
inline constexpr auto fermi = si::femto<meter>;
inline constexpr struct astronomical_unit final : named_unit<"au", mag<149'597'870'700> * meter> {} astronomical_unit;
inline constexpr struct barn final : named_unit<"b", mag_power<10, -28> * square(meter)> {} barn;
inline constexpr struct liter final : named_unit<"L", cubic(si::deci<meter>)> {} liter;
inline constexpr struct parsec final : named_unit<"pc", astronomical_unit / (mag_ratio<1, 3600> * degree)> {} parsec;
inline constexpr struct second final : named_unit<"s", kind_of<duration>> {} second;
inline constexpr struct hertz final : named_unit<"Hz", one / second, kind_of<frequency>> {} hertz;
inline constexpr struct eplus final : named_unit<symbol_text{u8"e⁺", "e+"}, kind_of<electric_charge>> {} eplus;
inline constexpr struct coulomb final : named_unit<"C", mag<6'241'509'074> * mag_power<10, 9> * eplus> {} coulomb;
inline constexpr struct electronvolt final : named_unit<"eV", kind_of<energy>> {} electronvolt;
inline constexpr struct joule final : named_unit<"J", electronvolt * coulomb / eplus> {} joule;
inline constexpr struct gram final : named_unit<"g", mag_ratio<1, 1000> * joule * square(second) / square(meter)> {} gram;
inline constexpr struct watt final : named_unit<"W", joule / second> {} watt;
inline constexpr struct newton final : named_unit<"N", joule / meter> {} newton;
#ifdef pascal
#pragma push_macro("pascal")
#undef pascal
#define MP_UNITS_REDEFINE_PASCAL
#endif
inline constexpr struct pascal final : named_unit<"Pa", newton / square(meter)> {} pascal;
#ifdef MP_UNITS_REDEFINE_PASCAL
#pragma pop_macro("pascal")
#undef MP_UNITS_REDEFINE_PASCAL
#endif
inline constexpr struct bar final : named_unit<"bar", mag_power<10, 5> * pascal> {} bar;
inline constexpr struct atmosphere final : named_unit<"atm", mag<101'325> * pascal> {} atmosphere;
inline constexpr struct ampere final : named_unit<"A", coulomb / second> {} ampere;
inline constexpr struct volt final : named_unit<"V", electronvolt / eplus> {} volt;
inline constexpr struct ohm final : named_unit<symbol_text{u8"Ω", "ohm"}, volt / ampere> {} ohm;
inline constexpr struct farad final : named_unit<"F", coulomb / volt> {} farad;
inline constexpr struct weber final : named_unit<"Wb", volt * second> {} weber;
inline constexpr struct tesla final : named_unit<"T", weber / square(meter)> {} tesla;
inline constexpr struct gauss final : named_unit<"G", mag_power<10, -4> * tesla> {} gauss;
inline constexpr struct henry final : named_unit<"H", weber / ampere> {} henry;
inline constexpr struct kelvin final : named_unit<"K", kind_of<temperature>> {} kelvin;
inline constexpr struct mole final : named_unit<"mol", kind_of<amount_of_substance>> {} mole;
inline constexpr struct becquerel final : named_unit<"Bq", one / second, kind_of<activity>> {} becquerel;
inline constexpr struct curie final : named_unit<"Ci", mag_ratio<37, 10> * mag_power<10, 10> * becquerel> {} curie;
inline constexpr struct gray final : named_unit<"Gy", joule / si::kilo<gram>, kind_of<absorbed_dose>> {} gray;
inline constexpr struct candela final : named_unit<"cd", kind_of<luminous_intensity>> {} candela;
inline constexpr struct lumen final : named_unit<"lm", candela * steradian> {} lumen;
inline constexpr struct lux final : named_unit<"lx", lumen / square(meter)> {} lux;
// clang-format on

namespace unit_symbols {

inline constexpr auto nm = si::nano<meter>;
inline constexpr auto um = si::micro<meter>;

inline constexpr auto mm = si::milli<meter>;
inline constexpr auto mm2 = square(mm);
inline constexpr auto mm3 = cubic(mm);

inline constexpr auto cm = si::centi<meter>;
inline constexpr auto cm2 = square(cm);
inline constexpr auto cm3 = cubic(cm);

inline constexpr auto L = liter;
inline constexpr auto dL = si::deci<liter>;
inline constexpr auto cL = si::centi<liter>;
inline constexpr auto mL = si::milli<liter>;

inline constexpr auto m = meter;
inline constexpr auto m2 = square(meter);
inline constexpr auto m3 = cubic(meter);

inline constexpr auto km = si::kilo<meter>;
inline constexpr auto km2 = square(km);
inline constexpr auto km3 = cubic(km);

inline constexpr auto pc = parsec;

inline constexpr auto rad = radian;
inline constexpr auto mrad = si::milli<radian>;
inline constexpr auto sr = steradian;
inline constexpr auto deg = degree;

inline constexpr auto ns = si::nano<second>;
inline constexpr auto s = second;
inline constexpr auto ms = si::milli<second>;
inline constexpr auto us = si::micro<second>;
inline constexpr auto ps = si::pico<second>;

inline constexpr auto MeV = si::mega<electronvolt>;
inline constexpr auto eV = electronvolt;
inline constexpr auto keV = si::kilo<electronvolt>;
inline constexpr auto GeV = si::giga<electronvolt>;
inline constexpr auto TeV = si::tera<electronvolt>;
inline constexpr auto PeV = si::peta<electronvolt>;

inline constexpr auto kg = si::kilo<gram>;
inline constexpr auto g = gram;
inline constexpr auto mg = si::milli<gram>;

inline constexpr auto b = barn;
inline constexpr auto mb = si::milli<barn>;
inline constexpr auto ub = si::micro<barn>;
inline constexpr auto nb = si::nano<barn>;
inline constexpr auto pb = si::pico<barn>;
inline constexpr auto fb = si::femto<barn>;

inline constexpr auto Bq = becquerel;
inline constexpr auto kBq = si::kilo<becquerel>;
inline constexpr auto MBq = si::mega<becquerel>;
inline constexpr auto GBq = si::giga<becquerel>;
inline constexpr auto Ci = curie;
inline constexpr auto mCi = si::milli<curie>;
inline constexpr auto uCi = si::micro<curie>;

}  // namespace unit_symbols
}  // namespace mp_units::hep
