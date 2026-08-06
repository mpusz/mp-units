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

// The `essential` tier of the SI unit symbols.
//
// It holds every unprefixed symbol - the base units, the named derived units, and the
// non-SI units accepted for use with the SI - plus the prefixed spellings that are
// standard practice in some domain (fm and nm for physics, us and ns for electronics,
// hPa for meteorology, MPa for materials, kHz..THz for signals, the pF..mF capacitor and
// mOhm..GOhm resistor decades, uSv for radiation protection, and so on).  What is left
// for the full <mp-units/systems/si/unit_symbols.h> is the rest of the 24x28 prefix
// matrix: spellings like `Qkat` or `dasr` that no domain writes.
//
// The split exists because that matrix is not free.  Every prefixed symbol is a distinct
// class type, and instantiating all 671 of them costs roughly two thirds as much as the
// entire SI unit set does - while this header costs about a seventh of that.
//
// The full header includes this one, so both spell the very same types and may be mixed
// freely; `si/core.h` includes this one, so a lean include already gives you the symbols
// you are likely to reach for.

#include <mp-units/bits/module_macros.h>
#include <mp-units/systems/si/prefixes.h>
#include <mp-units/systems/si/units.h>

MP_UNITS_EXPORT
namespace mp_units {

namespace si::unit_symbols {

inline constexpr auto fm = femto<metre>;
inline constexpr auto pm = pico<metre>;
inline constexpr auto nm = nano<metre>;
inline constexpr auto um = micro<metre>;
inline constexpr auto µm = micro<metre>;
inline constexpr auto mm = milli<metre>;
inline constexpr auto cm = centi<metre>;
inline constexpr auto dm = deci<metre>;
inline constexpr auto m = metre;
inline constexpr auto km = kilo<metre>;

inline constexpr auto as = atto<second>;
inline constexpr auto fs = femto<second>;
inline constexpr auto ps = pico<second>;
inline constexpr auto ns = nano<second>;
inline constexpr auto us = micro<second>;
inline constexpr auto µs = micro<second>;
inline constexpr auto ms = milli<second>;
inline constexpr auto s = second;
inline constexpr auto ks = kilo<second>;

inline constexpr auto fg = femto<gram>;
inline constexpr auto pg = pico<gram>;
inline constexpr auto ng = nano<gram>;
inline constexpr auto ug = micro<gram>;
inline constexpr auto µg = micro<gram>;
inline constexpr auto mg = milli<gram>;
inline constexpr auto g = gram;
inline constexpr auto kg = kilogram;

inline constexpr auto Mg = mega<gram>;

inline constexpr auto fA = femto<ampere>;
inline constexpr auto pA = pico<ampere>;
inline constexpr auto nA = nano<ampere>;
inline constexpr auto uA = micro<ampere>;
inline constexpr auto µA = micro<ampere>;
inline constexpr auto mA = milli<ampere>;
inline constexpr auto A = ampere;
inline constexpr auto kA = kilo<ampere>;

inline constexpr auto pK = pico<kelvin>;
inline constexpr auto nK = nano<kelvin>;
inline constexpr auto uK = micro<kelvin>;
inline constexpr auto µK = micro<kelvin>;
inline constexpr auto mK = milli<kelvin>;
inline constexpr auto K = kelvin;
inline constexpr auto MK = mega<kelvin>;

inline constexpr auto fmol = femto<mole>;
inline constexpr auto pmol = pico<mole>;
inline constexpr auto nmol = nano<mole>;
inline constexpr auto umol = micro<mole>;
inline constexpr auto µmol = micro<mole>;
inline constexpr auto mmol = milli<mole>;
inline constexpr auto cmol = centi<mole>;
inline constexpr auto mol = mole;
inline constexpr auto kmol = kilo<mole>;

inline constexpr auto mcd = milli<candela>;
inline constexpr auto cd = candela;

inline constexpr auto nrad = nano<radian>;
inline constexpr auto urad = micro<radian>;
inline constexpr auto µrad = micro<radian>;
inline constexpr auto mrad = milli<radian>;
inline constexpr auto rad = radian;

inline constexpr auto msr = milli<steradian>;
inline constexpr auto sr = steradian;

inline constexpr auto nHz = nano<hertz>;
inline constexpr auto uHz = micro<hertz>;
inline constexpr auto µHz = micro<hertz>;
inline constexpr auto mHz = milli<hertz>;
inline constexpr auto Hz = hertz;
inline constexpr auto kHz = kilo<hertz>;
inline constexpr auto MHz = mega<hertz>;
inline constexpr auto GHz = giga<hertz>;
inline constexpr auto THz = tera<hertz>;
inline constexpr auto PHz = peta<hertz>;

inline constexpr auto pN = pico<newton>;
inline constexpr auto nN = nano<newton>;
inline constexpr auto uN = micro<newton>;
inline constexpr auto µN = micro<newton>;
inline constexpr auto mN = milli<newton>;
inline constexpr auto cN = centi<newton>;
inline constexpr auto N = newton;
inline constexpr auto daN = deca<newton>;
inline constexpr auto kN = kilo<newton>;
inline constexpr auto MN = mega<newton>;

#ifdef pascal
#pragma push_macro("pascal")
#undef pascal
#define MP_UNITS_REDEFINE_PASCAL
#endif
inline constexpr auto uPa = micro<pascal>;
inline constexpr auto µPa = micro<pascal>;
inline constexpr auto mPa = milli<pascal>;
inline constexpr auto Pa = pascal;
inline constexpr auto daPa = deca<pascal>;
inline constexpr auto hPa = hecto<pascal>;
inline constexpr auto kPa = kilo<pascal>;
inline constexpr auto MPa = mega<pascal>;
inline constexpr auto GPa = giga<pascal>;
inline constexpr auto TPa = tera<pascal>;
#ifdef MP_UNITS_REDEFINE_PASCAL
#pragma pop_macro("pascal")
#undef MP_UNITS_REDEFINE_PASCAL
#endif

inline constexpr auto aJ = atto<joule>;
inline constexpr auto fJ = femto<joule>;
inline constexpr auto pJ = pico<joule>;
inline constexpr auto nJ = nano<joule>;
inline constexpr auto uJ = micro<joule>;
inline constexpr auto µJ = micro<joule>;
inline constexpr auto mJ = milli<joule>;
inline constexpr auto J = joule;
inline constexpr auto kJ = kilo<joule>;
inline constexpr auto MJ = mega<joule>;
inline constexpr auto GJ = giga<joule>;
inline constexpr auto TJ = tera<joule>;
inline constexpr auto PJ = peta<joule>;
inline constexpr auto EJ = exa<joule>;
inline constexpr auto ZJ = zetta<joule>;

inline constexpr auto pW = pico<watt>;
inline constexpr auto nW = nano<watt>;
inline constexpr auto uW = micro<watt>;
inline constexpr auto µW = micro<watt>;
inline constexpr auto mW = milli<watt>;
inline constexpr auto W = watt;
inline constexpr auto kW = kilo<watt>;
inline constexpr auto MW = mega<watt>;
inline constexpr auto GW = giga<watt>;
inline constexpr auto TW = tera<watt>;
inline constexpr auto PW = peta<watt>;

inline constexpr auto pC = pico<coulomb>;
inline constexpr auto nC = nano<coulomb>;
inline constexpr auto uC = micro<coulomb>;
inline constexpr auto µC = micro<coulomb>;
inline constexpr auto mC = milli<coulomb>;
inline constexpr auto C = coulomb;

inline constexpr auto nV = nano<volt>;
inline constexpr auto uV = micro<volt>;
inline constexpr auto µV = micro<volt>;
inline constexpr auto mV = milli<volt>;
inline constexpr auto V = volt;
inline constexpr auto kV = kilo<volt>;
inline constexpr auto MV = mega<volt>;

inline constexpr auto fF = femto<farad>;
inline constexpr auto pF = pico<farad>;
inline constexpr auto nF = nano<farad>;
inline constexpr auto uF = micro<farad>;
inline constexpr auto µF = micro<farad>;
inline constexpr auto mF = milli<farad>;
inline constexpr auto F = farad;

inline constexpr auto uohm = micro<si::ohm>;
inline constexpr auto mohm = milli<si::ohm>;
using si::ohm;
inline constexpr auto kohm = kilo<si::ohm>;
inline constexpr auto Mohm = mega<si::ohm>;
inline constexpr auto Gohm = giga<si::ohm>;
inline constexpr auto Tohm = tera<si::ohm>;
inline constexpr auto µΩ = micro<si::ohm>;
inline constexpr auto mΩ = milli<si::ohm>;
inline constexpr auto Ω = si::ohm;
inline constexpr auto kΩ = kilo<si::ohm>;
inline constexpr auto MΩ = mega<si::ohm>;
inline constexpr auto GΩ = giga<si::ohm>;
inline constexpr auto TΩ = tera<si::ohm>;

inline constexpr auto pS = pico<siemens>;
inline constexpr auto nS = nano<siemens>;
inline constexpr auto uS = micro<siemens>;
inline constexpr auto µS = micro<siemens>;
inline constexpr auto mS = milli<siemens>;
inline constexpr auto dS = deci<siemens>;
inline constexpr auto S = siemens;
inline constexpr auto MS = mega<siemens>;

inline constexpr auto Wb = weber;

inline constexpr auto fT = femto<tesla>;
inline constexpr auto pT = pico<tesla>;
inline constexpr auto nT = nano<tesla>;
inline constexpr auto uT = micro<tesla>;
inline constexpr auto µT = micro<tesla>;
inline constexpr auto mT = milli<tesla>;
inline constexpr auto T = tesla;

inline constexpr auto nH = nano<henry>;
inline constexpr auto uH = micro<henry>;
inline constexpr auto µH = micro<henry>;
inline constexpr auto mH = milli<henry>;
inline constexpr auto H = henry;
inline constexpr auto lm = lumen;
inline constexpr auto klm = kilo<lumen>;

inline constexpr auto mlx = milli<lux>;
inline constexpr auto lx = lux;
inline constexpr auto klx = kilo<lux>;

inline constexpr auto mBq = milli<becquerel>;
inline constexpr auto Bq = becquerel;
inline constexpr auto kBq = kilo<becquerel>;
inline constexpr auto MBq = mega<becquerel>;
inline constexpr auto GBq = giga<becquerel>;
inline constexpr auto TBq = tera<becquerel>;
inline constexpr auto PBq = peta<becquerel>;

inline constexpr auto uGy = micro<gray>;
inline constexpr auto µGy = micro<gray>;
inline constexpr auto mGy = milli<gray>;
inline constexpr auto cGy = centi<gray>;
inline constexpr auto Gy = gray;
inline constexpr auto kGy = kilo<gray>;

inline constexpr auto nSv = nano<sievert>;
inline constexpr auto uSv = micro<sievert>;
inline constexpr auto µSv = micro<sievert>;
inline constexpr auto mSv = milli<sievert>;
inline constexpr auto Sv = sievert;

inline constexpr auto nkat = nano<katal>;
inline constexpr auto ukat = micro<katal>;
inline constexpr auto µkat = micro<katal>;
inline constexpr auto kat = katal;

// no prefixes should be provided for the below units
inline constexpr auto deg_C = degree_Celsius;

// commonly used squared and cubic units
inline constexpr auto m2 = square(metre);
inline constexpr auto m3 = cubic(metre);
inline constexpr auto m4 = pow<4>(metre);
inline constexpr auto s2 = square(second);
inline constexpr auto s3 = cubic(second);

}  // namespace si::unit_symbols

namespace non_si::unit_symbols {

// TODO Should the following non-SI units have prefixed symbols predefiend as well?
inline constexpr auto au = astronomical_unit;
inline constexpr auto deg = degree;
inline constexpr auto arcmin = arcminute;
inline constexpr auto arcsec = arcsecond;
inline constexpr auto a = are;
inline constexpr auto ha = hectare;
inline constexpr auto l = litre;
inline constexpr auto L = litre;  // both versions of litre are allowed
inline constexpr auto t = tonne;
inline constexpr auto Da = dalton;
inline constexpr auto eV = electronvolt;

// no prefixes should be provided for the below units
inline constexpr auto min = minute;
inline constexpr auto h = hour;
inline constexpr auto d = day;

}  // namespace non_si::unit_symbols

namespace si::unit_symbols {

using namespace non_si::unit_symbols;  // NOLINT(google-build-using-namespace)

}  // namespace si::unit_symbols

}  // namespace mp_units
