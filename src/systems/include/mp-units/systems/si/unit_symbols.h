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

// The complete 24-prefix matrix over every prefixable SI unit.
//
// The symbols in common use live in <mp-units/systems/si/unit_symbols_essential.h>, which
// this header includes and extends; see there for why the two are separate.  Include this
// header (or <mp-units/systems/si.h>) when you need an unusual prefix, and the essential
// one when you do not - the difference is measurable in every translation unit.

#include <mp-units/bits/module_macros.h>
#include <mp-units/systems/si/prefixes.h>
#include <mp-units/systems/si/unit_symbols_essential.h>
#include <mp-units/systems/si/units.h>

MP_UNITS_EXPORT
namespace mp_units {

namespace si::unit_symbols {

inline constexpr auto qm = quecto<metre>;
inline constexpr auto rm = ronto<metre>;
inline constexpr auto ym = yocto<metre>;
inline constexpr auto zm = zepto<metre>;
inline constexpr auto am = atto<metre>;
inline constexpr auto dam = deca<metre>;
inline constexpr auto hm = hecto<metre>;
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
inline constexpr auto cs = centi<second>;
inline constexpr auto ds = deci<second>;

// TODO Should the below multiples of second be provided?
inline constexpr auto das = deca<second>;
inline constexpr auto hs = hecto<second>;
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
inline constexpr auto cg = centi<gram>;
inline constexpr auto dg = deci<gram>;
inline constexpr auto dag = deca<gram>;
inline constexpr auto hg = hecto<gram>;
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
inline constexpr auto cA = centi<ampere>;
inline constexpr auto dA = deci<ampere>;
inline constexpr auto daA = deca<ampere>;
inline constexpr auto hA = hecto<ampere>;
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
inline constexpr auto cK = centi<kelvin>;
inline constexpr auto dK = deci<kelvin>;
inline constexpr auto daK = deca<kelvin>;
inline constexpr auto hK = hecto<kelvin>;
inline constexpr auto kK = kilo<kelvin>;
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
inline constexpr auto dmol = deci<mole>;
inline constexpr auto damol = deca<mole>;
inline constexpr auto hmol = hecto<mole>;
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
inline constexpr auto µcd = micro<candela>;
inline constexpr auto ccd = centi<candela>;
inline constexpr auto dcd = deci<candela>;
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
inline constexpr auto crad = centi<radian>;
inline constexpr auto drad = deci<radian>;
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
inline constexpr auto µsr = micro<steradian>;
inline constexpr auto csr = centi<steradian>;
inline constexpr auto dsr = deci<steradian>;
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
inline constexpr auto cHz = centi<hertz>;
inline constexpr auto dHz = deci<hertz>;
inline constexpr auto daHz = deca<hertz>;
inline constexpr auto hHz = hecto<hertz>;
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
inline constexpr auto dN = deci<newton>;
inline constexpr auto hN = hecto<newton>;
inline constexpr auto GN = giga<newton>;
inline constexpr auto TN = tera<newton>;
inline constexpr auto PN = peta<newton>;
inline constexpr auto EN = exa<newton>;
inline constexpr auto ZN = zetta<newton>;
inline constexpr auto YN = yotta<newton>;
inline constexpr auto RN = ronna<newton>;
inline constexpr auto QN = quetta<newton>;

#ifdef pascal
#pragma push_macro("pascal")
#undef pascal
#define MP_UNITS_REDEFINE_PASCAL
#endif
inline constexpr auto qPa = quecto<pascal>;
inline constexpr auto rPa = ronto<pascal>;
inline constexpr auto yPa = yocto<pascal>;
inline constexpr auto zPa = zepto<pascal>;
inline constexpr auto aPa = atto<pascal>;
inline constexpr auto fPa = femto<pascal>;
inline constexpr auto pPa = pico<pascal>;
inline constexpr auto nPa = nano<pascal>;
inline constexpr auto cPa = centi<pascal>;
inline constexpr auto dPa = deci<pascal>;
inline constexpr auto PPa = peta<pascal>;
inline constexpr auto EPa = exa<pascal>;
inline constexpr auto ZPa = zetta<pascal>;
inline constexpr auto YPa = yotta<pascal>;
inline constexpr auto RPa = ronna<pascal>;
inline constexpr auto QPa = quetta<pascal>;
#ifdef MP_UNITS_REDEFINE_PASCAL
#pragma pop_macro("pascal")
#undef MP_UNITS_REDEFINE_PASCAL
#endif

inline constexpr auto qJ = quecto<joule>;
inline constexpr auto rJ = ronto<joule>;
inline constexpr auto yJ = yocto<joule>;
inline constexpr auto zJ = zepto<joule>;
inline constexpr auto cJ = centi<joule>;
inline constexpr auto dJ = deci<joule>;
inline constexpr auto daJ = deca<joule>;
inline constexpr auto hJ = hecto<joule>;
inline constexpr auto YJ = yotta<joule>;
inline constexpr auto RJ = ronna<joule>;
inline constexpr auto QJ = quetta<joule>;

inline constexpr auto qW = quecto<watt>;
inline constexpr auto rW = ronto<watt>;
inline constexpr auto yW = yocto<watt>;
inline constexpr auto zW = zepto<watt>;
inline constexpr auto aW = atto<watt>;
inline constexpr auto fW = femto<watt>;
inline constexpr auto cW = centi<watt>;
inline constexpr auto dW = deci<watt>;
inline constexpr auto daW = deca<watt>;
inline constexpr auto hW = hecto<watt>;
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
inline constexpr auto cC = centi<coulomb>;
inline constexpr auto dC = deci<coulomb>;
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
inline constexpr auto cV = centi<volt>;
inline constexpr auto dV = deci<volt>;
inline constexpr auto daV = deca<volt>;
inline constexpr auto hV = hecto<volt>;
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
inline constexpr auto cF = centi<farad>;
inline constexpr auto dF = deci<farad>;
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

inline constexpr auto qohm = quecto<si::ohm>;
inline constexpr auto rohm = ronto<si::ohm>;
inline constexpr auto yohm = yocto<si::ohm>;
inline constexpr auto zohm = zepto<si::ohm>;
inline constexpr auto aohm = atto<si::ohm>;
inline constexpr auto fohm = femto<si::ohm>;
inline constexpr auto pohm = pico<si::ohm>;
inline constexpr auto nohm = nano<si::ohm>;
inline constexpr auto cohm = centi<si::ohm>;
inline constexpr auto dohm = deci<si::ohm>;
inline constexpr auto daohm = deca<si::ohm>;
inline constexpr auto hohm = hecto<si::ohm>;
inline constexpr auto Pohm = peta<si::ohm>;
inline constexpr auto Eohm = exa<si::ohm>;
inline constexpr auto Zohm = zetta<si::ohm>;
inline constexpr auto Yohm = yotta<si::ohm>;
inline constexpr auto Rohm = ronna<si::ohm>;
inline constexpr auto Qohm = quetta<si::ohm>;

inline constexpr auto qΩ = quecto<si::ohm>;
inline constexpr auto rΩ = ronto<si::ohm>;
inline constexpr auto yΩ = yocto<si::ohm>;
inline constexpr auto zΩ = zepto<si::ohm>;
inline constexpr auto aΩ = atto<si::ohm>;
inline constexpr auto fΩ = femto<si::ohm>;
inline constexpr auto pΩ = pico<si::ohm>;
inline constexpr auto nΩ = nano<si::ohm>;
inline constexpr auto cΩ = centi<si::ohm>;
inline constexpr auto dΩ = deci<si::ohm>;
inline constexpr auto daΩ = deca<si::ohm>;
inline constexpr auto hΩ = hecto<si::ohm>;
inline constexpr auto PΩ = peta<si::ohm>;
inline constexpr auto EΩ = exa<si::ohm>;
inline constexpr auto ZΩ = zetta<si::ohm>;
inline constexpr auto YΩ = yotta<si::ohm>;
inline constexpr auto RΩ = ronna<si::ohm>;
inline constexpr auto QΩ = quetta<si::ohm>;

inline constexpr auto qS = quecto<siemens>;
inline constexpr auto rS = ronto<siemens>;
inline constexpr auto yS = yocto<siemens>;
inline constexpr auto zS = zepto<siemens>;
inline constexpr auto aS = atto<siemens>;
inline constexpr auto fS = femto<siemens>;
inline constexpr auto cS = centi<siemens>;
inline constexpr auto daS = deca<siemens>;
inline constexpr auto hS = hecto<siemens>;
inline constexpr auto kS = kilo<siemens>;
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
inline constexpr auto µWb = micro<weber>;
inline constexpr auto mWb = milli<weber>;
inline constexpr auto cWb = centi<weber>;
inline constexpr auto dWb = deci<weber>;
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
inline constexpr auto cT = centi<tesla>;
inline constexpr auto dT = deci<tesla>;
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
inline constexpr auto cH = centi<henry>;
inline constexpr auto dH = deci<henry>;
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
inline constexpr auto µlm = micro<lumen>;
inline constexpr auto mlm = milli<lumen>;
inline constexpr auto clm = centi<lumen>;
inline constexpr auto dlm = deci<lumen>;
inline constexpr auto dalm = deca<lumen>;
inline constexpr auto hlm = hecto<lumen>;
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
inline constexpr auto µlx = micro<lux>;
inline constexpr auto clx = centi<lux>;
inline constexpr auto dlx = deci<lux>;
inline constexpr auto dalx = deca<lux>;
inline constexpr auto hlx = hecto<lux>;
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
inline constexpr auto µBq = micro<becquerel>;
inline constexpr auto cBq = centi<becquerel>;
inline constexpr auto dBq = deci<becquerel>;
inline constexpr auto daBq = deca<becquerel>;
inline constexpr auto hBq = hecto<becquerel>;
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
inline constexpr auto dGy = deci<gray>;
inline constexpr auto daGy = deca<gray>;
inline constexpr auto hGy = hecto<gray>;
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
inline constexpr auto cSv = centi<sievert>;
inline constexpr auto dSv = deci<sievert>;
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
inline constexpr auto mkat = milli<katal>;
inline constexpr auto ckat = centi<katal>;
inline constexpr auto dkat = deci<katal>;
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

}  // namespace si::unit_symbols

}  // namespace mp_units
