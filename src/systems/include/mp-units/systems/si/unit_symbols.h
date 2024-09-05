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

constexpr auto qm = quecto<metre>;
constexpr auto rm = ronto<metre>;
constexpr auto ym = yocto<metre>;
constexpr auto zm = zepto<metre>;
constexpr auto am = atto<metre>;
constexpr auto fm = femto<metre>;
constexpr auto pm = pico<metre>;
constexpr auto nm = nano<metre>;
constexpr auto um = micro<metre>;
constexpr auto mm = milli<metre>;
constexpr auto cm = centi<metre>;
constexpr auto dm = deci<metre>;
constexpr auto m = metre;
constexpr auto dam = deca<metre>;
constexpr auto hm = hecto<metre>;
constexpr auto km = kilo<metre>;
constexpr auto Mm = mega<metre>;
constexpr auto Gm = giga<metre>;
constexpr auto Tm = tera<metre>;
constexpr auto Pm = peta<metre>;
constexpr auto Em = exa<metre>;
constexpr auto Zm = zetta<metre>;
constexpr auto Ym = yotta<metre>;
constexpr auto Rm = ronna<metre>;
constexpr auto Qm = quetta<metre>;

constexpr auto qs = quecto<second>;
constexpr auto rs = ronto<second>;
constexpr auto ys = yocto<second>;
constexpr auto zs = zepto<second>;
constexpr auto as = atto<second>;
constexpr auto fs = femto<second>;
constexpr auto ps = pico<second>;
constexpr auto ns = nano<second>;
constexpr auto us = micro<second>;
constexpr auto ms = milli<second>;
constexpr auto cs = centi<second>;
constexpr auto ds = deci<second>;
constexpr auto s = second;
// TODO Should the below multiples of second be provided?
constexpr auto das = deca<second>;
constexpr auto hs = hecto<second>;
constexpr auto ks = kilo<second>;
constexpr auto Ms = mega<second>;
constexpr auto Gs = giga<second>;
constexpr auto Ts = tera<second>;
constexpr auto Ps = peta<second>;
constexpr auto Es = exa<second>;
constexpr auto Zs = zetta<second>;
constexpr auto Ys = yotta<second>;
constexpr auto Rs = ronna<second>;
constexpr auto Qs = quetta<second>;

constexpr auto qg = quecto<gram>;
constexpr auto rg = ronto<gram>;
constexpr auto yg = yocto<gram>;
constexpr auto zg = zepto<gram>;
constexpr auto ag = atto<gram>;
constexpr auto fg = femto<gram>;
constexpr auto pg = pico<gram>;
constexpr auto ng = nano<gram>;
constexpr auto ug = micro<gram>;
constexpr auto mg = milli<gram>;
constexpr auto cg = centi<gram>;
constexpr auto dg = deci<gram>;
constexpr auto g = gram;
constexpr auto dag = deca<gram>;
constexpr auto hg = hecto<gram>;
constexpr auto kg = kilogram;
constexpr auto Mg = mega<gram>;
constexpr auto Gg = giga<gram>;
constexpr auto Tg = tera<gram>;
constexpr auto Pg = peta<gram>;
constexpr auto Eg = exa<gram>;
constexpr auto Zg = zetta<gram>;
constexpr auto Yg = yotta<gram>;
constexpr auto Rg = ronna<gram>;
constexpr auto Qg = quetta<gram>;

constexpr auto qA = quecto<ampere>;
constexpr auto rA = ronto<ampere>;
constexpr auto yA = yocto<ampere>;
constexpr auto zA = zepto<ampere>;
constexpr auto aA = atto<ampere>;
constexpr auto fA = femto<ampere>;
constexpr auto pA = pico<ampere>;
constexpr auto nA = nano<ampere>;
constexpr auto uA = micro<ampere>;
constexpr auto mA = milli<ampere>;
constexpr auto cA = centi<ampere>;
constexpr auto dA = deci<ampere>;
constexpr auto A = ampere;
constexpr auto daA = deca<ampere>;
constexpr auto hA = hecto<ampere>;
constexpr auto kA = kilo<ampere>;
constexpr auto MA = mega<ampere>;
constexpr auto GA = giga<ampere>;
constexpr auto TA = tera<ampere>;
constexpr auto PA = peta<ampere>;
constexpr auto EA = exa<ampere>;
constexpr auto ZA = zetta<ampere>;
constexpr auto YA = yotta<ampere>;
constexpr auto RA = ronna<ampere>;
constexpr auto QA = quetta<ampere>;

constexpr auto qK = quecto<kelvin>;
constexpr auto rK = ronto<kelvin>;
constexpr auto yK = yocto<kelvin>;
constexpr auto zK = zepto<kelvin>;
constexpr auto aK = atto<kelvin>;
constexpr auto fK = femto<kelvin>;
constexpr auto pK = pico<kelvin>;
constexpr auto nK = nano<kelvin>;
constexpr auto uK = micro<kelvin>;
constexpr auto mK = milli<kelvin>;
constexpr auto cK = centi<kelvin>;
constexpr auto dK = deci<kelvin>;
constexpr auto K = kelvin;
constexpr auto daK = deca<kelvin>;
constexpr auto hK = hecto<kelvin>;
constexpr auto kK = kilo<kelvin>;
constexpr auto MK = mega<kelvin>;
constexpr auto GK = giga<kelvin>;
constexpr auto TK = tera<kelvin>;
constexpr auto PK = peta<kelvin>;
constexpr auto EK = exa<kelvin>;
constexpr auto ZK = zetta<kelvin>;
constexpr auto YK = yotta<kelvin>;
constexpr auto RK = ronna<kelvin>;
constexpr auto QK = quetta<kelvin>;

constexpr auto qmol = quecto<mole>;
constexpr auto rmol = ronto<mole>;
constexpr auto ymol = yocto<mole>;
constexpr auto zmol = zepto<mole>;
constexpr auto amol = atto<mole>;
constexpr auto fmol = femto<mole>;
constexpr auto pmol = pico<mole>;
constexpr auto nmol = nano<mole>;
constexpr auto umol = micro<mole>;
constexpr auto mmol = milli<mole>;
constexpr auto cmol = centi<mole>;
constexpr auto dmol = deci<mole>;
constexpr auto mol = mole;
constexpr auto damol = deca<mole>;
constexpr auto hmol = hecto<mole>;
constexpr auto kmol = kilo<mole>;
constexpr auto Mmol = mega<mole>;
constexpr auto Gmol = giga<mole>;
constexpr auto Tmol = tera<mole>;
constexpr auto Pmol = peta<mole>;
constexpr auto Emol = exa<mole>;
constexpr auto Zmol = zetta<mole>;
constexpr auto Ymol = yotta<mole>;
constexpr auto Rmol = ronna<mole>;
constexpr auto Qmol = quetta<mole>;

constexpr auto qcd = quecto<candela>;
constexpr auto rcd = ronto<candela>;
constexpr auto ycd = yocto<candela>;
constexpr auto zcd = zepto<candela>;
constexpr auto acd = atto<candela>;
constexpr auto fcd = femto<candela>;
constexpr auto pcd = pico<candela>;
constexpr auto ncd = nano<candela>;
constexpr auto ucd = micro<candela>;
constexpr auto mcd = milli<candela>;
constexpr auto ccd = centi<candela>;
constexpr auto dcd = deci<candela>;
constexpr auto cd = candela;
constexpr auto dacd = deca<candela>;
constexpr auto hcd = hecto<candela>;
constexpr auto kcd = kilo<candela>;
constexpr auto Mcd = mega<candela>;
constexpr auto Gcd = giga<candela>;
constexpr auto Tcd = tera<candela>;
constexpr auto Pcd = peta<candela>;
constexpr auto Ecd = exa<candela>;
constexpr auto Zcd = zetta<candela>;
constexpr auto Ycd = yotta<candela>;
constexpr auto Rcd = ronna<candela>;
constexpr auto Qcd = quetta<candela>;

constexpr auto qrad = quecto<radian>;
constexpr auto rrad = ronto<radian>;
constexpr auto yrad = yocto<radian>;
constexpr auto zrad = zepto<radian>;
constexpr auto arad = atto<radian>;
constexpr auto frad = femto<radian>;
constexpr auto prad = pico<radian>;
constexpr auto nrad = nano<radian>;
constexpr auto urad = micro<radian>;
constexpr auto mrad = milli<radian>;
constexpr auto crad = centi<radian>;
constexpr auto drad = deci<radian>;
constexpr auto rad = radian;
constexpr auto darad = deca<radian>;
constexpr auto hrad = hecto<radian>;
constexpr auto krad = kilo<radian>;
constexpr auto Mrad = mega<radian>;
constexpr auto Grad = giga<radian>;
constexpr auto Trad = tera<radian>;
constexpr auto Prad = peta<radian>;
constexpr auto Erad = exa<radian>;
constexpr auto Zrad = zetta<radian>;
constexpr auto Yrad = yotta<radian>;
constexpr auto Rrad = ronna<radian>;
constexpr auto Qrad = quetta<radian>;

constexpr auto qsr = quecto<steradian>;
constexpr auto rsr = ronto<steradian>;
constexpr auto ysr = yocto<steradian>;
constexpr auto zsr = zepto<steradian>;
constexpr auto asr = atto<steradian>;
constexpr auto fsr = femto<steradian>;
constexpr auto psr = pico<steradian>;
constexpr auto nsr = nano<steradian>;
constexpr auto usr = micro<steradian>;
constexpr auto msr = milli<steradian>;
constexpr auto csr = centi<steradian>;
constexpr auto dsr = deci<steradian>;
constexpr auto sr = steradian;
constexpr auto dasr = deca<steradian>;
constexpr auto hsr = hecto<steradian>;
constexpr auto ksr = kilo<steradian>;
constexpr auto Msr = mega<steradian>;
constexpr auto Gsr = giga<steradian>;
constexpr auto Tsr = tera<steradian>;
constexpr auto Psr = peta<steradian>;
constexpr auto Esr = exa<steradian>;
constexpr auto Zsr = zetta<steradian>;
constexpr auto Ysr = yotta<steradian>;
constexpr auto Rsr = ronna<steradian>;
constexpr auto Qsr = quetta<steradian>;

constexpr auto qHz = quecto<hertz>;
constexpr auto rHz = ronto<hertz>;
constexpr auto yHz = yocto<hertz>;
constexpr auto zHz = zepto<hertz>;
constexpr auto aHz = atto<hertz>;
constexpr auto fHz = femto<hertz>;
constexpr auto pHz = pico<hertz>;
constexpr auto nHz = nano<hertz>;
constexpr auto uHz = micro<hertz>;
constexpr auto mHz = milli<hertz>;
constexpr auto cHz = centi<hertz>;
constexpr auto dHz = deci<hertz>;
constexpr auto Hz = hertz;
constexpr auto daHz = deca<hertz>;
constexpr auto hHz = hecto<hertz>;
constexpr auto kHz = kilo<hertz>;
constexpr auto MHz = mega<hertz>;
constexpr auto GHz = giga<hertz>;
constexpr auto THz = tera<hertz>;
constexpr auto PHz = peta<hertz>;
constexpr auto EHz = exa<hertz>;
constexpr auto ZHz = zetta<hertz>;
constexpr auto YHz = yotta<hertz>;
constexpr auto RHz = ronna<hertz>;
constexpr auto QHz = quetta<hertz>;

constexpr auto qN = quecto<newton>;
constexpr auto rN = ronto<newton>;
constexpr auto yN = yocto<newton>;
constexpr auto zN = zepto<newton>;
constexpr auto aN = atto<newton>;
constexpr auto fN = femto<newton>;
constexpr auto pN = pico<newton>;
constexpr auto nN = nano<newton>;
constexpr auto uN = micro<newton>;
constexpr auto mN = milli<newton>;
constexpr auto cN = centi<newton>;
constexpr auto dN = deci<newton>;
constexpr auto N = newton;
constexpr auto daN = deca<newton>;
constexpr auto hN = hecto<newton>;
constexpr auto kN = kilo<newton>;
constexpr auto MN = mega<newton>;
constexpr auto GN = giga<newton>;
constexpr auto TN = tera<newton>;
constexpr auto PN = peta<newton>;
constexpr auto EN = exa<newton>;
constexpr auto ZN = zetta<newton>;
constexpr auto YN = yotta<newton>;
constexpr auto RN = ronna<newton>;
constexpr auto QN = quetta<newton>;

#ifdef pascal
#pragma push_macro("pascal")
#undef pascal
#define MP_UNITS_REDEFINE_PASCAL
#endif
constexpr auto qPa = quecto<pascal>;
constexpr auto rPa = ronto<pascal>;
constexpr auto yPa = yocto<pascal>;
constexpr auto zPa = zepto<pascal>;
constexpr auto aPa = atto<pascal>;
constexpr auto fPa = femto<pascal>;
constexpr auto pPa = pico<pascal>;
constexpr auto nPa = nano<pascal>;
constexpr auto uPa = micro<pascal>;
constexpr auto mPa = milli<pascal>;
constexpr auto cPa = centi<pascal>;
constexpr auto dPa = deci<pascal>;
constexpr auto Pa = pascal;
constexpr auto daPa = deca<pascal>;
constexpr auto hPa = hecto<pascal>;
constexpr auto kPa = kilo<pascal>;
constexpr auto MPa = mega<pascal>;
constexpr auto GPa = giga<pascal>;
constexpr auto TPa = tera<pascal>;
constexpr auto PPa = peta<pascal>;
constexpr auto EPa = exa<pascal>;
constexpr auto ZPa = zetta<pascal>;
constexpr auto YPa = yotta<pascal>;
constexpr auto RPa = ronna<pascal>;
constexpr auto QPa = quetta<pascal>;
#ifdef MP_UNITS_REDEFINE_PASCAL
#pragma pop_macro("pascal")
#undef MP_UNITS_REDEFINE_PASCAL
#endif

constexpr auto qJ = quecto<joule>;
constexpr auto rJ = ronto<joule>;
constexpr auto yJ = yocto<joule>;
constexpr auto zJ = zepto<joule>;
constexpr auto aJ = atto<joule>;
constexpr auto fJ = femto<joule>;
constexpr auto pJ = pico<joule>;
constexpr auto nJ = nano<joule>;
constexpr auto uJ = micro<joule>;
constexpr auto mJ = milli<joule>;
constexpr auto cJ = centi<joule>;
constexpr auto dJ = deci<joule>;
constexpr auto J = joule;
constexpr auto daJ = deca<joule>;
constexpr auto hJ = hecto<joule>;
constexpr auto kJ = kilo<joule>;
constexpr auto MJ = mega<joule>;
constexpr auto GJ = giga<joule>;
constexpr auto TJ = tera<joule>;
constexpr auto PJ = peta<joule>;
constexpr auto EJ = exa<joule>;
constexpr auto ZJ = zetta<joule>;
constexpr auto YJ = yotta<joule>;
constexpr auto RJ = ronna<joule>;
constexpr auto QJ = quetta<joule>;

constexpr auto qW = quecto<watt>;
constexpr auto rW = ronto<watt>;
constexpr auto yW = yocto<watt>;
constexpr auto zW = zepto<watt>;
constexpr auto aW = atto<watt>;
constexpr auto fW = femto<watt>;
constexpr auto pW = pico<watt>;
constexpr auto nW = nano<watt>;
constexpr auto uW = micro<watt>;
constexpr auto mW = milli<watt>;
constexpr auto cW = centi<watt>;
constexpr auto dW = deci<watt>;
constexpr auto W = watt;
constexpr auto daW = deca<watt>;
constexpr auto hW = hecto<watt>;
constexpr auto kW = kilo<watt>;
constexpr auto MW = mega<watt>;
constexpr auto GW = giga<watt>;
constexpr auto TW = tera<watt>;
constexpr auto PW = peta<watt>;
constexpr auto EW = exa<watt>;
constexpr auto ZW = zetta<watt>;
constexpr auto YW = yotta<watt>;
constexpr auto RW = ronna<watt>;
constexpr auto QW = quetta<watt>;

constexpr auto qC = quecto<coulomb>;
constexpr auto rC = ronto<coulomb>;
constexpr auto yC = yocto<coulomb>;
constexpr auto zC = zepto<coulomb>;
constexpr auto aC = atto<coulomb>;
constexpr auto fC = femto<coulomb>;
constexpr auto pC = pico<coulomb>;
constexpr auto nC = nano<coulomb>;
constexpr auto uC = micro<coulomb>;
constexpr auto mC = milli<coulomb>;
constexpr auto cC = centi<coulomb>;
constexpr auto dC = deci<coulomb>;
constexpr auto C = coulomb;
constexpr auto daC = deca<coulomb>;
constexpr auto hC = hecto<coulomb>;
constexpr auto kC = kilo<coulomb>;
constexpr auto MC = mega<coulomb>;
constexpr auto GC = giga<coulomb>;
constexpr auto TC = tera<coulomb>;
constexpr auto PC = peta<coulomb>;
constexpr auto EC = exa<coulomb>;
constexpr auto ZC = zetta<coulomb>;
constexpr auto YC = yotta<coulomb>;
constexpr auto RC = ronna<coulomb>;
constexpr auto QC = quetta<coulomb>;

constexpr auto qV = quecto<volt>;
constexpr auto rV = ronto<volt>;
constexpr auto yV = yocto<volt>;
constexpr auto zV = zepto<volt>;
constexpr auto aV = atto<volt>;
constexpr auto fV = femto<volt>;
constexpr auto pV = pico<volt>;
constexpr auto nV = nano<volt>;
constexpr auto uV = micro<volt>;
constexpr auto mV = milli<volt>;
constexpr auto cV = centi<volt>;
constexpr auto dV = deci<volt>;
constexpr auto V = volt;
constexpr auto daV = deca<volt>;
constexpr auto hV = hecto<volt>;
constexpr auto kV = kilo<volt>;
constexpr auto MV = mega<volt>;
constexpr auto GV = giga<volt>;
constexpr auto TV = tera<volt>;
constexpr auto PV = peta<volt>;
constexpr auto EV = exa<volt>;
constexpr auto ZV = zetta<volt>;
constexpr auto YV = yotta<volt>;
constexpr auto RV = ronna<volt>;
constexpr auto QV = quetta<volt>;

constexpr auto qF = quecto<farad>;
constexpr auto rF = ronto<farad>;
constexpr auto yF = yocto<farad>;
constexpr auto zF = zepto<farad>;
constexpr auto aF = atto<farad>;
constexpr auto fF = femto<farad>;
constexpr auto pF = pico<farad>;
constexpr auto nF = nano<farad>;
constexpr auto uF = micro<farad>;
constexpr auto mF = milli<farad>;
constexpr auto cF = centi<farad>;
constexpr auto dF = deci<farad>;
constexpr auto F = farad;
constexpr auto daF = deca<farad>;
constexpr auto hF = hecto<farad>;
constexpr auto kF = kilo<farad>;
constexpr auto MF = mega<farad>;
constexpr auto GF = giga<farad>;
constexpr auto TF = tera<farad>;
constexpr auto PF = peta<farad>;
constexpr auto EF = exa<farad>;
constexpr auto ZF = zetta<farad>;
constexpr auto YF = yotta<farad>;
constexpr auto RF = ronna<farad>;
constexpr auto QF = quetta<farad>;

constexpr auto qohm = quecto<si::ohm>;
constexpr auto rohm = ronto<si::ohm>;
constexpr auto yohm = yocto<si::ohm>;
constexpr auto zohm = zepto<si::ohm>;
constexpr auto aohm = atto<si::ohm>;
constexpr auto fohm = femto<si::ohm>;
constexpr auto pohm = pico<si::ohm>;
constexpr auto nohm = nano<si::ohm>;
constexpr auto uohm = micro<si::ohm>;
constexpr auto mohm = milli<si::ohm>;
constexpr auto cohm = centi<si::ohm>;
constexpr auto dohm = deci<si::ohm>;
using si::ohm;
constexpr auto daohm = deca<si::ohm>;
constexpr auto hohm = hecto<si::ohm>;
constexpr auto kohm = kilo<si::ohm>;
constexpr auto Mohm = mega<si::ohm>;
constexpr auto Gohm = giga<si::ohm>;
constexpr auto Tohm = tera<si::ohm>;
constexpr auto Pohm = peta<si::ohm>;
constexpr auto Eohm = exa<si::ohm>;
constexpr auto Zohm = zetta<si::ohm>;
constexpr auto Yohm = yotta<si::ohm>;
constexpr auto Rohm = ronna<si::ohm>;
constexpr auto Qohm = quetta<si::ohm>;

constexpr auto qS = quecto<siemens>;
constexpr auto rS = ronto<siemens>;
constexpr auto yS = yocto<siemens>;
constexpr auto zS = zepto<siemens>;
constexpr auto aS = atto<siemens>;
constexpr auto fS = femto<siemens>;
constexpr auto pS = pico<siemens>;
constexpr auto nS = nano<siemens>;
constexpr auto uS = micro<siemens>;
constexpr auto mS = milli<siemens>;
constexpr auto cS = centi<siemens>;
constexpr auto dS = deci<siemens>;
constexpr auto S = siemens;
constexpr auto daS = deca<siemens>;
constexpr auto hS = hecto<siemens>;
constexpr auto kS = kilo<siemens>;
constexpr auto MS = mega<siemens>;
constexpr auto GS = giga<siemens>;
constexpr auto TS = tera<siemens>;
constexpr auto PS = peta<siemens>;
constexpr auto ES = exa<siemens>;
constexpr auto ZS = zetta<siemens>;
constexpr auto YS = yotta<siemens>;
constexpr auto RS = ronna<siemens>;
constexpr auto QS = quetta<siemens>;

constexpr auto qWb = quecto<weber>;
constexpr auto rWb = ronto<weber>;
constexpr auto yWb = yocto<weber>;
constexpr auto zWb = zepto<weber>;
constexpr auto aWb = atto<weber>;
constexpr auto fWb = femto<weber>;
constexpr auto pWb = pico<weber>;
constexpr auto nWb = nano<weber>;
constexpr auto uWb = micro<weber>;
constexpr auto mWb = milli<weber>;
constexpr auto cWb = centi<weber>;
constexpr auto dWb = deci<weber>;
constexpr auto Wb = weber;
constexpr auto daWb = deca<weber>;
constexpr auto hWb = hecto<weber>;
constexpr auto kWb = kilo<weber>;
constexpr auto MWb = mega<weber>;
constexpr auto GWb = giga<weber>;
constexpr auto TWb = tera<weber>;
constexpr auto PWb = peta<weber>;
constexpr auto EWb = exa<weber>;
constexpr auto ZWb = zetta<weber>;
constexpr auto YWb = yotta<weber>;
constexpr auto RWb = ronna<weber>;
constexpr auto QWb = quetta<weber>;

constexpr auto qT = quecto<tesla>;
constexpr auto rT = ronto<tesla>;
constexpr auto yT = yocto<tesla>;
constexpr auto zT = zepto<tesla>;
constexpr auto aT = atto<tesla>;
constexpr auto fT = femto<tesla>;
constexpr auto pT = pico<tesla>;
constexpr auto nT = nano<tesla>;
constexpr auto uT = micro<tesla>;
constexpr auto mT = milli<tesla>;
constexpr auto cT = centi<tesla>;
constexpr auto dT = deci<tesla>;
constexpr auto T = tesla;
constexpr auto daT = deca<tesla>;
constexpr auto hT = hecto<tesla>;
constexpr auto kT = kilo<tesla>;
constexpr auto MT = mega<tesla>;
constexpr auto GT = giga<tesla>;
constexpr auto TT = tera<tesla>;
constexpr auto PT = peta<tesla>;
constexpr auto ET = exa<tesla>;
constexpr auto ZT = zetta<tesla>;
constexpr auto YT = yotta<tesla>;
constexpr auto RT = ronna<tesla>;
constexpr auto QT = quetta<tesla>;

constexpr auto qH = quecto<henry>;
constexpr auto rH = ronto<henry>;
constexpr auto yH = yocto<henry>;
constexpr auto zH = zepto<henry>;
constexpr auto aH = atto<henry>;
constexpr auto fH = femto<henry>;
constexpr auto pH = pico<henry>;
constexpr auto nH = nano<henry>;
constexpr auto uH = micro<henry>;
constexpr auto mH = milli<henry>;
constexpr auto cH = centi<henry>;
constexpr auto dH = deci<henry>;
constexpr auto H = henry;
constexpr auto daH = deca<henry>;
constexpr auto hH = hecto<henry>;
constexpr auto kH = kilo<henry>;
constexpr auto MH = mega<henry>;
constexpr auto GH = giga<henry>;
constexpr auto TH = tera<henry>;
constexpr auto PH = peta<henry>;
constexpr auto EH = exa<henry>;
constexpr auto ZH = zetta<henry>;
constexpr auto YH = yotta<henry>;
constexpr auto RH = ronna<henry>;
constexpr auto QH = quetta<henry>;

constexpr auto qlm = quecto<lumen>;
constexpr auto rlm = ronto<lumen>;
constexpr auto ylm = yocto<lumen>;
constexpr auto zlm = zepto<lumen>;
constexpr auto alm = atto<lumen>;
constexpr auto flm = femto<lumen>;
constexpr auto plm = pico<lumen>;
constexpr auto nlm = nano<lumen>;
constexpr auto ulm = micro<lumen>;
constexpr auto mlm = milli<lumen>;
constexpr auto clm = centi<lumen>;
constexpr auto dlm = deci<lumen>;
constexpr auto lm = lumen;
constexpr auto dalm = deca<lumen>;
constexpr auto hlm = hecto<lumen>;
constexpr auto klm = kilo<lumen>;
constexpr auto Mlm = mega<lumen>;
constexpr auto Glm = giga<lumen>;
constexpr auto Tlm = tera<lumen>;
constexpr auto Plm = peta<lumen>;
constexpr auto Elm = exa<lumen>;
constexpr auto Zlm = zetta<lumen>;
constexpr auto Ylm = yotta<lumen>;
constexpr auto Rlm = ronna<lumen>;
constexpr auto Qlm = quetta<lumen>;

constexpr auto qlx = quecto<lux>;
constexpr auto rlx = ronto<lux>;
constexpr auto ylx = yocto<lux>;
constexpr auto zlx = zepto<lux>;
constexpr auto alx = atto<lux>;
constexpr auto flx = femto<lux>;
constexpr auto plx = pico<lux>;
constexpr auto nlx = nano<lux>;
constexpr auto ulx = micro<lux>;
constexpr auto mlx = milli<lux>;
constexpr auto clx = centi<lux>;
constexpr auto dlx = deci<lux>;
constexpr auto lx = lux;
constexpr auto dalx = deca<lux>;
constexpr auto hlx = hecto<lux>;
constexpr auto klx = kilo<lux>;
constexpr auto Mlx = mega<lux>;
constexpr auto Glx = giga<lux>;
constexpr auto Tlx = tera<lux>;
constexpr auto Plx = peta<lux>;
constexpr auto Elx = exa<lux>;
constexpr auto Zlx = zetta<lux>;
constexpr auto Ylx = yotta<lux>;
constexpr auto Rlx = ronna<lux>;
constexpr auto Qlx = quetta<lux>;

constexpr auto qBq = quecto<becquerel>;
constexpr auto rBq = ronto<becquerel>;
constexpr auto yBq = yocto<becquerel>;
constexpr auto zBq = zepto<becquerel>;
constexpr auto aBq = atto<becquerel>;
constexpr auto fBq = femto<becquerel>;
constexpr auto pBq = pico<becquerel>;
constexpr auto nBq = nano<becquerel>;
constexpr auto uBq = micro<becquerel>;
constexpr auto mBq = milli<becquerel>;
constexpr auto cBq = centi<becquerel>;
constexpr auto dBq = deci<becquerel>;
constexpr auto Bq = becquerel;
constexpr auto daBq = deca<becquerel>;
constexpr auto hBq = hecto<becquerel>;
constexpr auto kBq = kilo<becquerel>;
constexpr auto MBq = mega<becquerel>;
constexpr auto GBq = giga<becquerel>;
constexpr auto TBq = tera<becquerel>;
constexpr auto PBq = peta<becquerel>;
constexpr auto EBq = exa<becquerel>;
constexpr auto ZBq = zetta<becquerel>;
constexpr auto YBq = yotta<becquerel>;
constexpr auto RBq = ronna<becquerel>;
constexpr auto QBq = quetta<becquerel>;

constexpr auto qGy = quecto<gray>;
constexpr auto rGy = ronto<gray>;
constexpr auto yGy = yocto<gray>;
constexpr auto zGy = zepto<gray>;
constexpr auto aGy = atto<gray>;
constexpr auto fGy = femto<gray>;
constexpr auto pGy = pico<gray>;
constexpr auto nGy = nano<gray>;
constexpr auto uGy = micro<gray>;
constexpr auto mGy = milli<gray>;
constexpr auto cGy = centi<gray>;
constexpr auto dGy = deci<gray>;
constexpr auto Gy = gray;
constexpr auto daGy = deca<gray>;
constexpr auto hGy = hecto<gray>;
constexpr auto kGy = kilo<gray>;
constexpr auto MGy = mega<gray>;
constexpr auto GGy = giga<gray>;
constexpr auto TGy = tera<gray>;
constexpr auto PGy = peta<gray>;
constexpr auto EGy = exa<gray>;
constexpr auto ZGy = zetta<gray>;
constexpr auto YGy = yotta<gray>;
constexpr auto RGy = ronna<gray>;
constexpr auto QGy = quetta<gray>;

constexpr auto qSv = quecto<sievert>;
constexpr auto rSv = ronto<sievert>;
constexpr auto ySv = yocto<sievert>;
constexpr auto zSv = zepto<sievert>;
constexpr auto aSv = atto<sievert>;
constexpr auto fSv = femto<sievert>;
constexpr auto pSv = pico<sievert>;
constexpr auto nSv = nano<sievert>;
constexpr auto uSv = micro<sievert>;
constexpr auto mSv = milli<sievert>;
constexpr auto cSv = centi<sievert>;
constexpr auto dSv = deci<sievert>;
constexpr auto Sv = sievert;
constexpr auto daSv = deca<sievert>;
constexpr auto hSv = hecto<sievert>;
constexpr auto kSv = kilo<sievert>;
constexpr auto MSv = mega<sievert>;
constexpr auto GSv = giga<sievert>;
constexpr auto TSv = tera<sievert>;
constexpr auto PSv = peta<sievert>;
constexpr auto ESv = exa<sievert>;
constexpr auto ZSv = zetta<sievert>;
constexpr auto YSv = yotta<sievert>;
constexpr auto RSv = ronna<sievert>;
constexpr auto QSv = quetta<sievert>;

constexpr auto qkat = quecto<katal>;
constexpr auto rkat = ronto<katal>;
constexpr auto ykat = yocto<katal>;
constexpr auto zkat = zepto<katal>;
constexpr auto akat = atto<katal>;
constexpr auto fkat = femto<katal>;
constexpr auto pkat = pico<katal>;
constexpr auto nkat = nano<katal>;
constexpr auto ukat = micro<katal>;
constexpr auto mkat = milli<katal>;
constexpr auto ckat = centi<katal>;
constexpr auto dkat = deci<katal>;
constexpr auto kat = katal;
constexpr auto dakat = deca<katal>;
constexpr auto hkat = hecto<katal>;
constexpr auto kkat = kilo<katal>;
constexpr auto Mkat = mega<katal>;
constexpr auto Gkat = giga<katal>;
constexpr auto Tkat = tera<katal>;
constexpr auto Pkat = peta<katal>;
constexpr auto Ekat = exa<katal>;
constexpr auto Zkat = zetta<katal>;
constexpr auto Ykat = yotta<katal>;
constexpr auto Rkat = ronna<katal>;
constexpr auto Qkat = quetta<katal>;

// no prefixes should be provided for the below units
constexpr auto deg_C = degree_Celsius;

// commonly used squared and cubic units
constexpr auto m2 = square(metre);
constexpr auto m3 = cubic(metre);
constexpr auto m4 = pow<4>(metre);
constexpr auto s2 = square(second);
constexpr auto s3 = cubic(second);

}  // namespace si::unit_symbols

namespace non_si::unit_symbols {

// TODO Should the following non-SI units have prefixed symbols predefiend as well?
constexpr auto au = astronomical_unit;
constexpr auto deg = degree;
constexpr auto arcmin = arcminute;
constexpr auto arcsec = arcsecond;
constexpr auto a = are;
constexpr auto ha = hectare;
constexpr auto l = litre;
constexpr auto t = tonne;
constexpr auto Da = dalton;
constexpr auto eV = electronvolt;

// no prefixes should be provided for the below units
constexpr auto min = minute;
constexpr auto h = hour;
constexpr auto d = day;

}  // namespace non_si::unit_symbols

namespace si::unit_symbols {

using namespace non_si::unit_symbols;  // NOLINT(google-build-using-namespace)

}  // namespace si::unit_symbols

}  // namespace mp_units
