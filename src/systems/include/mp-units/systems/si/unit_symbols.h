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
#include <mp-units/systems/si/prefixes.h>
#include <mp-units/systems/si/units.h>

MP_UNITS_EXPORT
namespace mp_units {

namespace si::unit_symbols {

inline constexpr auto qm = quecto<metre>;
inline constexpr auto rm = ronto<metre>;
inline constexpr auto ym = yocto<metre>;
inline constexpr auto zm = zepto<metre>;
inline constexpr auto am = atto<metre>;
inline constexpr auto fm = femto<metre>;
inline constexpr auto pm = pico<metre>;
inline constexpr auto nm = nano<metre>;
inline constexpr auto um = micro<metre>;
inline constexpr auto mm = milli<metre>;
inline constexpr auto cm = centi<metre>;
inline constexpr auto dm = deci<metre>;
inline constexpr auto m = metre;
inline constexpr auto dam = deca<metre>;
inline constexpr auto hm = hecto<metre>;
inline constexpr auto km = kilo<metre>;
inline constexpr auto Mm = mega<metre>;
inline constexpr auto Gm = giga<metre>;
inline constexpr auto Tm = tera<metre>;
inline constexpr auto Pm = peta<metre>;
inline constexpr auto Em = exa<metre>;
inline constexpr auto Zm = zetta<metre>;
inline constexpr auto Ym = yotta<metre>;
inline constexpr auto Rm = ronna<metre>;
inline constexpr auto Qm = quetta<metre>;

inline constexpr auto qs = quecto<second>;
inline constexpr auto rs = ronto<second>;
inline constexpr auto ys = yocto<second>;
inline constexpr auto zs = zepto<second>;
inline constexpr auto as = atto<second>;
inline constexpr auto fs = femto<second>;
inline constexpr auto ps = pico<second>;
inline constexpr auto ns = nano<second>;
inline constexpr auto us = micro<second>;
inline constexpr auto ms = milli<second>;
inline constexpr auto cs = centi<second>;
inline constexpr auto ds = deci<second>;
inline constexpr auto s = second;
// TODO Should the below multiples of second be provided?
inline constexpr auto das = deca<second>;
inline constexpr auto hs = hecto<second>;
inline constexpr auto ks = kilo<second>;
inline constexpr auto Ms = mega<second>;
inline constexpr auto Gs = giga<second>;
inline constexpr auto Ts = tera<second>;
inline constexpr auto Ps = peta<second>;
inline constexpr auto Es = exa<second>;
inline constexpr auto Zs = zetta<second>;
inline constexpr auto Ys = yotta<second>;
inline constexpr auto Rs = ronna<second>;
inline constexpr auto Qs = quetta<second>;

inline constexpr auto qg = quecto<gram>;
inline constexpr auto rg = ronto<gram>;
inline constexpr auto yg = yocto<gram>;
inline constexpr auto zg = zepto<gram>;
inline constexpr auto ag = atto<gram>;
inline constexpr auto fg = femto<gram>;
inline constexpr auto pg = pico<gram>;
inline constexpr auto ng = nano<gram>;
inline constexpr auto ug = micro<gram>;
inline constexpr auto mg = milli<gram>;
inline constexpr auto cg = centi<gram>;
inline constexpr auto dg = deci<gram>;
inline constexpr auto g = gram;
inline constexpr auto dag = deca<gram>;
inline constexpr auto hg = hecto<gram>;
inline constexpr auto kg = kilogram;
inline constexpr auto Mg = mega<gram>;
inline constexpr auto Gg = giga<gram>;
inline constexpr auto Tg = tera<gram>;
inline constexpr auto Pg = peta<gram>;
inline constexpr auto Eg = exa<gram>;
inline constexpr auto Zg = zetta<gram>;
inline constexpr auto Yg = yotta<gram>;
inline constexpr auto Rg = ronna<gram>;
inline constexpr auto Qg = quetta<gram>;

inline constexpr auto qA = quecto<ampere>;
inline constexpr auto rA = ronto<ampere>;
inline constexpr auto yA = yocto<ampere>;
inline constexpr auto zA = zepto<ampere>;
inline constexpr auto aA = atto<ampere>;
inline constexpr auto fA = femto<ampere>;
inline constexpr auto pA = pico<ampere>;
inline constexpr auto nA = nano<ampere>;
inline constexpr auto uA = micro<ampere>;
inline constexpr auto mA = milli<ampere>;
inline constexpr auto cA = centi<ampere>;
inline constexpr auto dA = deci<ampere>;
inline constexpr auto A = ampere;
inline constexpr auto daA = deca<ampere>;
inline constexpr auto hA = hecto<ampere>;
inline constexpr auto kA = kilo<ampere>;
inline constexpr auto MA = mega<ampere>;
inline constexpr auto GA = giga<ampere>;
inline constexpr auto TA = tera<ampere>;
inline constexpr auto PA = peta<ampere>;
inline constexpr auto EA = exa<ampere>;
inline constexpr auto ZA = zetta<ampere>;
inline constexpr auto YA = yotta<ampere>;
inline constexpr auto RA = ronna<ampere>;
inline constexpr auto QA = quetta<ampere>;

inline constexpr auto qK = quecto<kelvin>;
inline constexpr auto rK = ronto<kelvin>;
inline constexpr auto yK = yocto<kelvin>;
inline constexpr auto zK = zepto<kelvin>;
inline constexpr auto aK = atto<kelvin>;
inline constexpr auto fK = femto<kelvin>;
inline constexpr auto pK = pico<kelvin>;
inline constexpr auto nK = nano<kelvin>;
inline constexpr auto uK = micro<kelvin>;
inline constexpr auto mK = milli<kelvin>;
inline constexpr auto cK = centi<kelvin>;
inline constexpr auto dK = deci<kelvin>;
inline constexpr auto K = kelvin;
inline constexpr auto daK = deca<kelvin>;
inline constexpr auto hK = hecto<kelvin>;
inline constexpr auto kK = kilo<kelvin>;
inline constexpr auto MK = mega<kelvin>;
inline constexpr auto GK = giga<kelvin>;
inline constexpr auto TK = tera<kelvin>;
inline constexpr auto PK = peta<kelvin>;
inline constexpr auto EK = exa<kelvin>;
inline constexpr auto ZK = zetta<kelvin>;
inline constexpr auto YK = yotta<kelvin>;
inline constexpr auto RK = ronna<kelvin>;
inline constexpr auto QK = quetta<kelvin>;

inline constexpr auto qmol = quecto<mole>;
inline constexpr auto rmol = ronto<mole>;
inline constexpr auto ymol = yocto<mole>;
inline constexpr auto zmol = zepto<mole>;
inline constexpr auto amol = atto<mole>;
inline constexpr auto fmol = femto<mole>;
inline constexpr auto pmol = pico<mole>;
inline constexpr auto nmol = nano<mole>;
inline constexpr auto umol = micro<mole>;
inline constexpr auto mmol = milli<mole>;
inline constexpr auto cmol = centi<mole>;
inline constexpr auto dmol = deci<mole>;
inline constexpr auto mol = mole;
inline constexpr auto damol = deca<mole>;
inline constexpr auto hmol = hecto<mole>;
inline constexpr auto kmol = kilo<mole>;
inline constexpr auto Mmol = mega<mole>;
inline constexpr auto Gmol = giga<mole>;
inline constexpr auto Tmol = tera<mole>;
inline constexpr auto Pmol = peta<mole>;
inline constexpr auto Emol = exa<mole>;
inline constexpr auto Zmol = zetta<mole>;
inline constexpr auto Ymol = yotta<mole>;
inline constexpr auto Rmol = ronna<mole>;
inline constexpr auto Qmol = quetta<mole>;

inline constexpr auto qcd = quecto<candela>;
inline constexpr auto rcd = ronto<candela>;
inline constexpr auto ycd = yocto<candela>;
inline constexpr auto zcd = zepto<candela>;
inline constexpr auto acd = atto<candela>;
inline constexpr auto fcd = femto<candela>;
inline constexpr auto pcd = pico<candela>;
inline constexpr auto ncd = nano<candela>;
inline constexpr auto ucd = micro<candela>;
inline constexpr auto mcd = milli<candela>;
inline constexpr auto ccd = centi<candela>;
inline constexpr auto dcd = deci<candela>;
inline constexpr auto cd = candela;
inline constexpr auto dacd = deca<candela>;
inline constexpr auto hcd = hecto<candela>;
inline constexpr auto kcd = kilo<candela>;
inline constexpr auto Mcd = mega<candela>;
inline constexpr auto Gcd = giga<candela>;
inline constexpr auto Tcd = tera<candela>;
inline constexpr auto Pcd = peta<candela>;
inline constexpr auto Ecd = exa<candela>;
inline constexpr auto Zcd = zetta<candela>;
inline constexpr auto Ycd = yotta<candela>;
inline constexpr auto Rcd = ronna<candela>;
inline constexpr auto Qcd = quetta<candela>;

inline constexpr auto qrad = quecto<radian>;
inline constexpr auto rrad = ronto<radian>;
inline constexpr auto yrad = yocto<radian>;
inline constexpr auto zrad = zepto<radian>;
inline constexpr auto arad = atto<radian>;
inline constexpr auto frad = femto<radian>;
inline constexpr auto prad = pico<radian>;
inline constexpr auto nrad = nano<radian>;
inline constexpr auto urad = micro<radian>;
inline constexpr auto mrad = milli<radian>;
inline constexpr auto crad = centi<radian>;
inline constexpr auto drad = deci<radian>;
inline constexpr auto rad = radian;
inline constexpr auto darad = deca<radian>;
inline constexpr auto hrad = hecto<radian>;
inline constexpr auto krad = kilo<radian>;
inline constexpr auto Mrad = mega<radian>;
inline constexpr auto Grad = giga<radian>;
inline constexpr auto Trad = tera<radian>;
inline constexpr auto Prad = peta<radian>;
inline constexpr auto Erad = exa<radian>;
inline constexpr auto Zrad = zetta<radian>;
inline constexpr auto Yrad = yotta<radian>;
inline constexpr auto Rrad = ronna<radian>;
inline constexpr auto Qrad = quetta<radian>;

inline constexpr auto qsr = quecto<steradian>;
inline constexpr auto rsr = ronto<steradian>;
inline constexpr auto ysr = yocto<steradian>;
inline constexpr auto zsr = zepto<steradian>;
inline constexpr auto asr = atto<steradian>;
inline constexpr auto fsr = femto<steradian>;
inline constexpr auto psr = pico<steradian>;
inline constexpr auto nsr = nano<steradian>;
inline constexpr auto usr = micro<steradian>;
inline constexpr auto msr = milli<steradian>;
inline constexpr auto csr = centi<steradian>;
inline constexpr auto dsr = deci<steradian>;
inline constexpr auto sr = steradian;
inline constexpr auto dasr = deca<steradian>;
inline constexpr auto hsr = hecto<steradian>;
inline constexpr auto ksr = kilo<steradian>;
inline constexpr auto Msr = mega<steradian>;
inline constexpr auto Gsr = giga<steradian>;
inline constexpr auto Tsr = tera<steradian>;
inline constexpr auto Psr = peta<steradian>;
inline constexpr auto Esr = exa<steradian>;
inline constexpr auto Zsr = zetta<steradian>;
inline constexpr auto Ysr = yotta<steradian>;
inline constexpr auto Rsr = ronna<steradian>;
inline constexpr auto Qsr = quetta<steradian>;

inline constexpr auto qHz = quecto<hertz>;
inline constexpr auto rHz = ronto<hertz>;
inline constexpr auto yHz = yocto<hertz>;
inline constexpr auto zHz = zepto<hertz>;
inline constexpr auto aHz = atto<hertz>;
inline constexpr auto fHz = femto<hertz>;
inline constexpr auto pHz = pico<hertz>;
inline constexpr auto nHz = nano<hertz>;
inline constexpr auto uHz = micro<hertz>;
inline constexpr auto mHz = milli<hertz>;
inline constexpr auto cHz = centi<hertz>;
inline constexpr auto dHz = deci<hertz>;
inline constexpr auto Hz = hertz;
inline constexpr auto daHz = deca<hertz>;
inline constexpr auto hHz = hecto<hertz>;
inline constexpr auto kHz = kilo<hertz>;
inline constexpr auto MHz = mega<hertz>;
inline constexpr auto GHz = giga<hertz>;
inline constexpr auto THz = tera<hertz>;
inline constexpr auto PHz = peta<hertz>;
inline constexpr auto EHz = exa<hertz>;
inline constexpr auto ZHz = zetta<hertz>;
inline constexpr auto YHz = yotta<hertz>;
inline constexpr auto RHz = ronna<hertz>;
inline constexpr auto QHz = quetta<hertz>;

inline constexpr auto qN = quecto<newton>;
inline constexpr auto rN = ronto<newton>;
inline constexpr auto yN = yocto<newton>;
inline constexpr auto zN = zepto<newton>;
inline constexpr auto aN = atto<newton>;
inline constexpr auto fN = femto<newton>;
inline constexpr auto pN = pico<newton>;
inline constexpr auto nN = nano<newton>;
inline constexpr auto uN = micro<newton>;
inline constexpr auto mN = milli<newton>;
inline constexpr auto cN = centi<newton>;
inline constexpr auto dN = deci<newton>;
inline constexpr auto N = newton;
inline constexpr auto daN = deca<newton>;
inline constexpr auto hN = hecto<newton>;
inline constexpr auto kN = kilo<newton>;
inline constexpr auto MN = mega<newton>;
inline constexpr auto GN = giga<newton>;
inline constexpr auto TN = tera<newton>;
inline constexpr auto PN = peta<newton>;
inline constexpr auto EN = exa<newton>;
inline constexpr auto ZN = zetta<newton>;
inline constexpr auto YN = yotta<newton>;
inline constexpr auto RN = ronna<newton>;
inline constexpr auto QN = quetta<newton>;

inline constexpr auto qPa = quecto<pascal>;
inline constexpr auto rPa = ronto<pascal>;
inline constexpr auto yPa = yocto<pascal>;
inline constexpr auto zPa = zepto<pascal>;
inline constexpr auto aPa = atto<pascal>;
inline constexpr auto fPa = femto<pascal>;
inline constexpr auto pPa = pico<pascal>;
inline constexpr auto nPa = nano<pascal>;
inline constexpr auto uPa = micro<pascal>;
inline constexpr auto mPa = milli<pascal>;
inline constexpr auto cPa = centi<pascal>;
inline constexpr auto dPa = deci<pascal>;
inline constexpr auto Pa = pascal;
inline constexpr auto daPa = deca<pascal>;
inline constexpr auto hPa = hecto<pascal>;
inline constexpr auto kPa = kilo<pascal>;
inline constexpr auto MPa = mega<pascal>;
inline constexpr auto GPa = giga<pascal>;
inline constexpr auto TPa = tera<pascal>;
inline constexpr auto PPa = peta<pascal>;
inline constexpr auto EPa = exa<pascal>;
inline constexpr auto ZPa = zetta<pascal>;
inline constexpr auto YPa = yotta<pascal>;
inline constexpr auto RPa = ronna<pascal>;
inline constexpr auto QPa = quetta<pascal>;

inline constexpr auto qJ = quecto<joule>;
inline constexpr auto rJ = ronto<joule>;
inline constexpr auto yJ = yocto<joule>;
inline constexpr auto zJ = zepto<joule>;
inline constexpr auto aJ = atto<joule>;
inline constexpr auto fJ = femto<joule>;
inline constexpr auto pJ = pico<joule>;
inline constexpr auto nJ = nano<joule>;
inline constexpr auto uJ = micro<joule>;
inline constexpr auto mJ = milli<joule>;
inline constexpr auto cJ = centi<joule>;
inline constexpr auto dJ = deci<joule>;
inline constexpr auto J = joule;
inline constexpr auto daJ = deca<joule>;
inline constexpr auto hJ = hecto<joule>;
inline constexpr auto kJ = kilo<joule>;
inline constexpr auto MJ = mega<joule>;
inline constexpr auto GJ = giga<joule>;
inline constexpr auto TJ = tera<joule>;
inline constexpr auto PJ = peta<joule>;
inline constexpr auto EJ = exa<joule>;
inline constexpr auto ZJ = zetta<joule>;
inline constexpr auto YJ = yotta<joule>;
inline constexpr auto RJ = ronna<joule>;
inline constexpr auto QJ = quetta<joule>;

inline constexpr auto qW = quecto<watt>;
inline constexpr auto rW = ronto<watt>;
inline constexpr auto yW = yocto<watt>;
inline constexpr auto zW = zepto<watt>;
inline constexpr auto aW = atto<watt>;
inline constexpr auto fW = femto<watt>;
inline constexpr auto pW = pico<watt>;
inline constexpr auto nW = nano<watt>;
inline constexpr auto uW = micro<watt>;
inline constexpr auto mW = milli<watt>;
inline constexpr auto cW = centi<watt>;
inline constexpr auto dW = deci<watt>;
inline constexpr auto W = watt;
inline constexpr auto daW = deca<watt>;
inline constexpr auto hW = hecto<watt>;
inline constexpr auto kW = kilo<watt>;
inline constexpr auto MW = mega<watt>;
inline constexpr auto GW = giga<watt>;
inline constexpr auto TW = tera<watt>;
inline constexpr auto PW = peta<watt>;
inline constexpr auto EW = exa<watt>;
inline constexpr auto ZW = zetta<watt>;
inline constexpr auto YW = yotta<watt>;
inline constexpr auto RW = ronna<watt>;
inline constexpr auto QW = quetta<watt>;

inline constexpr auto qC = quecto<coulomb>;
inline constexpr auto rC = ronto<coulomb>;
inline constexpr auto yC = yocto<coulomb>;
inline constexpr auto zC = zepto<coulomb>;
inline constexpr auto aC = atto<coulomb>;
inline constexpr auto fC = femto<coulomb>;
inline constexpr auto pC = pico<coulomb>;
inline constexpr auto nC = nano<coulomb>;
inline constexpr auto uC = micro<coulomb>;
inline constexpr auto mC = milli<coulomb>;
inline constexpr auto cC = centi<coulomb>;
inline constexpr auto dC = deci<coulomb>;
inline constexpr auto C = coulomb;
inline constexpr auto daC = deca<coulomb>;
inline constexpr auto hC = hecto<coulomb>;
inline constexpr auto kC = kilo<coulomb>;
inline constexpr auto MC = mega<coulomb>;
inline constexpr auto GC = giga<coulomb>;
inline constexpr auto TC = tera<coulomb>;
inline constexpr auto PC = peta<coulomb>;
inline constexpr auto EC = exa<coulomb>;
inline constexpr auto ZC = zetta<coulomb>;
inline constexpr auto YC = yotta<coulomb>;
inline constexpr auto RC = ronna<coulomb>;
inline constexpr auto QC = quetta<coulomb>;

inline constexpr auto qV = quecto<volt>;
inline constexpr auto rV = ronto<volt>;
inline constexpr auto yV = yocto<volt>;
inline constexpr auto zV = zepto<volt>;
inline constexpr auto aV = atto<volt>;
inline constexpr auto fV = femto<volt>;
inline constexpr auto pV = pico<volt>;
inline constexpr auto nV = nano<volt>;
inline constexpr auto uV = micro<volt>;
inline constexpr auto mV = milli<volt>;
inline constexpr auto cV = centi<volt>;
inline constexpr auto dV = deci<volt>;
inline constexpr auto V = volt;
inline constexpr auto daV = deca<volt>;
inline constexpr auto hV = hecto<volt>;
inline constexpr auto kV = kilo<volt>;
inline constexpr auto MV = mega<volt>;
inline constexpr auto GV = giga<volt>;
inline constexpr auto TV = tera<volt>;
inline constexpr auto PV = peta<volt>;
inline constexpr auto EV = exa<volt>;
inline constexpr auto ZV = zetta<volt>;
inline constexpr auto YV = yotta<volt>;
inline constexpr auto RV = ronna<volt>;
inline constexpr auto QV = quetta<volt>;

inline constexpr auto qF = quecto<farad>;
inline constexpr auto rF = ronto<farad>;
inline constexpr auto yF = yocto<farad>;
inline constexpr auto zF = zepto<farad>;
inline constexpr auto aF = atto<farad>;
inline constexpr auto fF = femto<farad>;
inline constexpr auto pF = pico<farad>;
inline constexpr auto nF = nano<farad>;
inline constexpr auto uF = micro<farad>;
inline constexpr auto mF = milli<farad>;
inline constexpr auto cF = centi<farad>;
inline constexpr auto dF = deci<farad>;
inline constexpr auto F = farad;
inline constexpr auto daF = deca<farad>;
inline constexpr auto hF = hecto<farad>;
inline constexpr auto kF = kilo<farad>;
inline constexpr auto MF = mega<farad>;
inline constexpr auto GF = giga<farad>;
inline constexpr auto TF = tera<farad>;
inline constexpr auto PF = peta<farad>;
inline constexpr auto EF = exa<farad>;
inline constexpr auto ZF = zetta<farad>;
inline constexpr auto YF = yotta<farad>;
inline constexpr auto RF = ronna<farad>;
inline constexpr auto QF = quetta<farad>;

inline constexpr auto qS = quecto<siemens>;
inline constexpr auto rS = ronto<siemens>;
inline constexpr auto yS = yocto<siemens>;
inline constexpr auto zS = zepto<siemens>;
inline constexpr auto aS = atto<siemens>;
inline constexpr auto fS = femto<siemens>;
inline constexpr auto pS = pico<siemens>;
inline constexpr auto nS = nano<siemens>;
inline constexpr auto uS = micro<siemens>;
inline constexpr auto mS = milli<siemens>;
inline constexpr auto cS = centi<siemens>;
inline constexpr auto dS = deci<siemens>;
inline constexpr auto S = siemens;
inline constexpr auto daS = deca<siemens>;
inline constexpr auto hS = hecto<siemens>;
inline constexpr auto kS = kilo<siemens>;
inline constexpr auto MS = mega<siemens>;
inline constexpr auto GS = giga<siemens>;
inline constexpr auto TS = tera<siemens>;
inline constexpr auto PS = peta<siemens>;
inline constexpr auto ES = exa<siemens>;
inline constexpr auto ZS = zetta<siemens>;
inline constexpr auto YS = yotta<siemens>;
inline constexpr auto RS = ronna<siemens>;
inline constexpr auto QS = quetta<siemens>;

inline constexpr auto qWb = quecto<weber>;
inline constexpr auto rWb = ronto<weber>;
inline constexpr auto yWb = yocto<weber>;
inline constexpr auto zWb = zepto<weber>;
inline constexpr auto aWb = atto<weber>;
inline constexpr auto fWb = femto<weber>;
inline constexpr auto pWb = pico<weber>;
inline constexpr auto nWb = nano<weber>;
inline constexpr auto uWb = micro<weber>;
inline constexpr auto mWb = milli<weber>;
inline constexpr auto cWb = centi<weber>;
inline constexpr auto dWb = deci<weber>;
inline constexpr auto Wb = weber;
inline constexpr auto daWb = deca<weber>;
inline constexpr auto hWb = hecto<weber>;
inline constexpr auto kWb = kilo<weber>;
inline constexpr auto MWb = mega<weber>;
inline constexpr auto GWb = giga<weber>;
inline constexpr auto TWb = tera<weber>;
inline constexpr auto PWb = peta<weber>;
inline constexpr auto EWb = exa<weber>;
inline constexpr auto ZWb = zetta<weber>;
inline constexpr auto YWb = yotta<weber>;
inline constexpr auto RWb = ronna<weber>;
inline constexpr auto QWb = quetta<weber>;

inline constexpr auto qT = quecto<tesla>;
inline constexpr auto rT = ronto<tesla>;
inline constexpr auto yT = yocto<tesla>;
inline constexpr auto zT = zepto<tesla>;
inline constexpr auto aT = atto<tesla>;
inline constexpr auto fT = femto<tesla>;
inline constexpr auto pT = pico<tesla>;
inline constexpr auto nT = nano<tesla>;
inline constexpr auto uT = micro<tesla>;
inline constexpr auto mT = milli<tesla>;
inline constexpr auto cT = centi<tesla>;
inline constexpr auto dT = deci<tesla>;
inline constexpr auto T = tesla;
inline constexpr auto daT = deca<tesla>;
inline constexpr auto hT = hecto<tesla>;
inline constexpr auto kT = kilo<tesla>;
inline constexpr auto MT = mega<tesla>;
inline constexpr auto GT = giga<tesla>;
inline constexpr auto TT = tera<tesla>;
inline constexpr auto PT = peta<tesla>;
inline constexpr auto ET = exa<tesla>;
inline constexpr auto ZT = zetta<tesla>;
inline constexpr auto YT = yotta<tesla>;
inline constexpr auto RT = ronna<tesla>;
inline constexpr auto QT = quetta<tesla>;

inline constexpr auto qH = quecto<henry>;
inline constexpr auto rH = ronto<henry>;
inline constexpr auto yH = yocto<henry>;
inline constexpr auto zH = zepto<henry>;
inline constexpr auto aH = atto<henry>;
inline constexpr auto fH = femto<henry>;
inline constexpr auto pH = pico<henry>;
inline constexpr auto nH = nano<henry>;
inline constexpr auto uH = micro<henry>;
inline constexpr auto mH = milli<henry>;
inline constexpr auto cH = centi<henry>;
inline constexpr auto dH = deci<henry>;
inline constexpr auto H = henry;
inline constexpr auto daH = deca<henry>;
inline constexpr auto hH = hecto<henry>;
inline constexpr auto kH = kilo<henry>;
inline constexpr auto MH = mega<henry>;
inline constexpr auto GH = giga<henry>;
inline constexpr auto TH = tera<henry>;
inline constexpr auto PH = peta<henry>;
inline constexpr auto EH = exa<henry>;
inline constexpr auto ZH = zetta<henry>;
inline constexpr auto YH = yotta<henry>;
inline constexpr auto RH = ronna<henry>;
inline constexpr auto QH = quetta<henry>;

inline constexpr auto qlm = quecto<lumen>;
inline constexpr auto rlm = ronto<lumen>;
inline constexpr auto ylm = yocto<lumen>;
inline constexpr auto zlm = zepto<lumen>;
inline constexpr auto alm = atto<lumen>;
inline constexpr auto flm = femto<lumen>;
inline constexpr auto plm = pico<lumen>;
inline constexpr auto nlm = nano<lumen>;
inline constexpr auto ulm = micro<lumen>;
inline constexpr auto mlm = milli<lumen>;
inline constexpr auto clm = centi<lumen>;
inline constexpr auto dlm = deci<lumen>;
inline constexpr auto lm = lumen;
inline constexpr auto dalm = deca<lumen>;
inline constexpr auto hlm = hecto<lumen>;
inline constexpr auto klm = kilo<lumen>;
inline constexpr auto Mlm = mega<lumen>;
inline constexpr auto Glm = giga<lumen>;
inline constexpr auto Tlm = tera<lumen>;
inline constexpr auto Plm = peta<lumen>;
inline constexpr auto Elm = exa<lumen>;
inline constexpr auto Zlm = zetta<lumen>;
inline constexpr auto Ylm = yotta<lumen>;
inline constexpr auto Rlm = ronna<lumen>;
inline constexpr auto Qlm = quetta<lumen>;

inline constexpr auto qlx = quecto<lux>;
inline constexpr auto rlx = ronto<lux>;
inline constexpr auto ylx = yocto<lux>;
inline constexpr auto zlx = zepto<lux>;
inline constexpr auto alx = atto<lux>;
inline constexpr auto flx = femto<lux>;
inline constexpr auto plx = pico<lux>;
inline constexpr auto nlx = nano<lux>;
inline constexpr auto ulx = micro<lux>;
inline constexpr auto mlx = milli<lux>;
inline constexpr auto clx = centi<lux>;
inline constexpr auto dlx = deci<lux>;
inline constexpr auto lx = lux;
inline constexpr auto dalx = deca<lux>;
inline constexpr auto hlx = hecto<lux>;
inline constexpr auto klx = kilo<lux>;
inline constexpr auto Mlx = mega<lux>;
inline constexpr auto Glx = giga<lux>;
inline constexpr auto Tlx = tera<lux>;
inline constexpr auto Plx = peta<lux>;
inline constexpr auto Elx = exa<lux>;
inline constexpr auto Zlx = zetta<lux>;
inline constexpr auto Ylx = yotta<lux>;
inline constexpr auto Rlx = ronna<lux>;
inline constexpr auto Qlx = quetta<lux>;

inline constexpr auto qBq = quecto<becquerel>;
inline constexpr auto rBq = ronto<becquerel>;
inline constexpr auto yBq = yocto<becquerel>;
inline constexpr auto zBq = zepto<becquerel>;
inline constexpr auto aBq = atto<becquerel>;
inline constexpr auto fBq = femto<becquerel>;
inline constexpr auto pBq = pico<becquerel>;
inline constexpr auto nBq = nano<becquerel>;
inline constexpr auto uBq = micro<becquerel>;
inline constexpr auto mBq = milli<becquerel>;
inline constexpr auto cBq = centi<becquerel>;
inline constexpr auto dBq = deci<becquerel>;
inline constexpr auto Bq = becquerel;
inline constexpr auto daBq = deca<becquerel>;
inline constexpr auto hBq = hecto<becquerel>;
inline constexpr auto kBq = kilo<becquerel>;
inline constexpr auto MBq = mega<becquerel>;
inline constexpr auto GBq = giga<becquerel>;
inline constexpr auto TBq = tera<becquerel>;
inline constexpr auto PBq = peta<becquerel>;
inline constexpr auto EBq = exa<becquerel>;
inline constexpr auto ZBq = zetta<becquerel>;
inline constexpr auto YBq = yotta<becquerel>;
inline constexpr auto RBq = ronna<becquerel>;
inline constexpr auto QBq = quetta<becquerel>;

inline constexpr auto qGy = quecto<gray>;
inline constexpr auto rGy = ronto<gray>;
inline constexpr auto yGy = yocto<gray>;
inline constexpr auto zGy = zepto<gray>;
inline constexpr auto aGy = atto<gray>;
inline constexpr auto fGy = femto<gray>;
inline constexpr auto pGy = pico<gray>;
inline constexpr auto nGy = nano<gray>;
inline constexpr auto uGy = micro<gray>;
inline constexpr auto mGy = milli<gray>;
inline constexpr auto cGy = centi<gray>;
inline constexpr auto dGy = deci<gray>;
inline constexpr auto Gy = gray;
inline constexpr auto daGy = deca<gray>;
inline constexpr auto hGy = hecto<gray>;
inline constexpr auto kGy = kilo<gray>;
inline constexpr auto MGy = mega<gray>;
inline constexpr auto GGy = giga<gray>;
inline constexpr auto TGy = tera<gray>;
inline constexpr auto PGy = peta<gray>;
inline constexpr auto EGy = exa<gray>;
inline constexpr auto ZGy = zetta<gray>;
inline constexpr auto YGy = yotta<gray>;
inline constexpr auto RGy = ronna<gray>;
inline constexpr auto QGy = quetta<gray>;

inline constexpr auto qSv = quecto<sievert>;
inline constexpr auto rSv = ronto<sievert>;
inline constexpr auto ySv = yocto<sievert>;
inline constexpr auto zSv = zepto<sievert>;
inline constexpr auto aSv = atto<sievert>;
inline constexpr auto fSv = femto<sievert>;
inline constexpr auto pSv = pico<sievert>;
inline constexpr auto nSv = nano<sievert>;
inline constexpr auto uSv = micro<sievert>;
inline constexpr auto mSv = milli<sievert>;
inline constexpr auto cSv = centi<sievert>;
inline constexpr auto dSv = deci<sievert>;
inline constexpr auto Sv = sievert;
inline constexpr auto daSv = deca<sievert>;
inline constexpr auto hSv = hecto<sievert>;
inline constexpr auto kSv = kilo<sievert>;
inline constexpr auto MSv = mega<sievert>;
inline constexpr auto GSv = giga<sievert>;
inline constexpr auto TSv = tera<sievert>;
inline constexpr auto PSv = peta<sievert>;
inline constexpr auto ESv = exa<sievert>;
inline constexpr auto ZSv = zetta<sievert>;
inline constexpr auto YSv = yotta<sievert>;
inline constexpr auto RSv = ronna<sievert>;
inline constexpr auto QSv = quetta<sievert>;

inline constexpr auto qkat = quecto<katal>;
inline constexpr auto rkat = ronto<katal>;
inline constexpr auto ykat = yocto<katal>;
inline constexpr auto zkat = zepto<katal>;
inline constexpr auto akat = atto<katal>;
inline constexpr auto fkat = femto<katal>;
inline constexpr auto pkat = pico<katal>;
inline constexpr auto nkat = nano<katal>;
inline constexpr auto ukat = micro<katal>;
inline constexpr auto mkat = milli<katal>;
inline constexpr auto ckat = centi<katal>;
inline constexpr auto dkat = deci<katal>;
inline constexpr auto kat = katal;
inline constexpr auto dakat = deca<katal>;
inline constexpr auto hkat = hecto<katal>;
inline constexpr auto kkat = kilo<katal>;
inline constexpr auto Mkat = mega<katal>;
inline constexpr auto Gkat = giga<katal>;
inline constexpr auto Tkat = tera<katal>;
inline constexpr auto Pkat = peta<katal>;
inline constexpr auto Ekat = exa<katal>;
inline constexpr auto Zkat = zetta<katal>;
inline constexpr auto Ykat = yotta<katal>;
inline constexpr auto Rkat = ronna<katal>;
inline constexpr auto Qkat = quetta<katal>;

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
inline constexpr auto t = tonne;
inline constexpr auto Da = dalton;
inline constexpr auto eV = electronvolt;

// no prefixes should be provided for the below units
inline constexpr auto min = minute;
inline constexpr auto h = hour;
inline constexpr auto d = day;

}  // namespace non_si::unit_symbols

namespace si::unit_symbols {

using namespace non_si::unit_symbols;

}  // namespace si::unit_symbols

}  // namespace mp_units
