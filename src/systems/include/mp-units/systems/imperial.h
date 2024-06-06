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

// IWYU pragma: begin_exports
#include <mp-units/systems/international.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework.h>
#endif
// IWYU pragma: end_exports

MP_UNITS_EXPORT
namespace mp_units::imperial {

using namespace international;

// clang-format off
// https://en.wikipedia.org/wiki/Imperial_units#Length
inline constexpr struct hand final : named_unit<"hh", mag_ratio<1, 3> * foot> {} hand;
inline constexpr struct barleycorn final : named_unit<"Bc", mag_ratio<1, 3> * inch> {} barleycorn;
inline constexpr struct thou final : named_unit<"th", mag_ratio<1, 12'000> * foot> {} thou;
inline constexpr struct chain final : named_unit<"ch", mag<22> * yard> {} chain;
inline constexpr struct furlong final : named_unit<"fur", mag<10> * chain> {} furlong;

// maritime units
inline constexpr struct cable final : named_unit<"cb", mag_ratio<1, 10> * nautical_mile> {} cable;
inline constexpr struct fathom final : named_unit<"ftm", mag_ratio<1, 1000> * nautical_mile> {} fathom;

// survey
inline constexpr struct link final : named_unit<"li", mag_ratio<1, 100> * chain> {} link;
inline constexpr struct rod final : named_unit<"rd", mag<25> * link> {} rod;

// https://en.wikipedia.org/wiki/Imperial_units#Area
inline constexpr struct perch final : named_unit<"perch", square(rod)> {} perch;
inline constexpr struct rood final : named_unit<"rood", mag<40> * perch> {} rood;
inline constexpr struct acre final : named_unit<"acre", mag<4> * rood> {} acre;

// https://en.wikipedia.org/wiki/Imperial_units#Volume
inline constexpr struct gallon final : named_unit<"gal", mag_ratio<454'609, 100'000> * si::litre> {} gallon;
inline constexpr struct quart final : named_unit<"qt", mag_ratio<1, 4> * gallon> {} quart;
inline constexpr struct pint final : named_unit<"pt", mag_ratio<1, 2> * quart> {} pint;
inline constexpr struct gill final : named_unit<"gi", mag_ratio<1, 4> * pint> {} gill;
inline constexpr struct fluid_ounce final : named_unit<"fl oz", mag_ratio<1, 5> * gill> {} fluid_ounce;

// https://en.wikipedia.org/wiki/Avoirdupois_system#Post-Elizabethan_units
inline constexpr auto drachm = dram;
inline constexpr struct stone final : named_unit<"st", mag<14> * pound> {} stone;
inline constexpr struct quarter final : named_unit<"qr", mag<2> * stone> {} quarter;
inline constexpr struct long_hundredweight final : named_unit<"cwt", mag<8> * stone> {} long_hundredweight;
inline constexpr struct ton final : named_unit<"t", mag<2'240> * pound> {} ton;
inline constexpr auto long_ton = ton;
// clang-format on

namespace unit_symbols {

using namespace international::unit_symbols;

inline constexpr auto hh = hand;
inline constexpr auto Bc = barleycorn;
inline constexpr auto th = thou;
inline constexpr auto ch = chain;
inline constexpr auto fur = furlong;

inline constexpr auto cb = cable;
inline constexpr auto ftm = fathom;

inline constexpr auto li = link;
inline constexpr auto rd = rod;

inline constexpr auto gal = gallon;
inline constexpr auto qt = quart;
inline constexpr auto pt = pint;
inline constexpr auto gi = gill;
inline constexpr auto fl_oz = fluid_ounce;

inline constexpr auto st = stone;
inline constexpr auto qr = quarter;
inline constexpr auto cwt = long_hundredweight;
inline constexpr auto t = ton;

}  // namespace unit_symbols

}  // namespace mp_units::imperial
