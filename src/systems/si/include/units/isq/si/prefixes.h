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

#include <units/unit.h>

namespace units::isq::si {

template<NamedUnit auto U>
struct yocto : prefixed_unit<"y", mag_power<10, -24>, U> {};
template<NamedUnit auto U>
struct zepto : prefixed_unit<"z", mag_power<10, -21>, U> {};
template<NamedUnit auto U>
struct atto : prefixed_unit<"a", mag_power<10, -18>, U> {};
template<NamedUnit auto U>
struct femto : prefixed_unit<"f", mag_power<10, -15>, U> {};
template<NamedUnit auto U>
struct pico : prefixed_unit<"p", mag_power<10, -12>, U> {};
template<NamedUnit auto U>
struct nano : prefixed_unit<"n", mag_power<10, -9>, U> {};
template<NamedUnit auto U>
struct micro : prefixed_unit<basic_symbol_text{"\u00b5", "u"}, mag_power<10, -6>, U> {};
template<NamedUnit auto U>
struct milli : prefixed_unit<"m", mag_power<10, -3>, U> {};
template<NamedUnit auto U>
struct centi : prefixed_unit<"c", mag_power<10, -2>, U> {};
template<NamedUnit auto U>
struct deci : prefixed_unit<"d", mag_power<10, -1>, U> {};
template<NamedUnit auto U>
struct deca : prefixed_unit<"da", mag_power<10, 1>, U> {};
template<NamedUnit auto U>
struct hecto : prefixed_unit<"h", mag_power<10, 2>, U> {};
template<NamedUnit auto U>
struct kilo : prefixed_unit<"k", mag_power<10, 3>, U> {};
template<NamedUnit auto U>
struct mega : prefixed_unit<"M", mag_power<10, 6>, U> {};
template<NamedUnit auto U>
struct giga : prefixed_unit<"G", mag_power<10, 9>, U> {};
template<NamedUnit auto U>
struct tera : prefixed_unit<"T", mag_power<10, 12>, U> {};
template<NamedUnit auto U>
struct peta : prefixed_unit<"P", mag_power<10, 15>, U> {};
template<NamedUnit auto U>
struct exa : prefixed_unit<"E", mag_power<10, 18>, U> {};
template<NamedUnit auto U>
struct zetta : prefixed_unit<"Z", mag_power<10, 21>, U> {};
template<NamedUnit auto U>
struct yotta : prefixed_unit<"Y", mag_power<10, 24>, U> {};

}  // namespace units::isq::si
