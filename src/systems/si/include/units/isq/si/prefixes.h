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
struct yocto : prefixed_unit<"y", pow<-24>(mag<10>()), U> {};
template<NamedUnit auto U>
struct zepto : prefixed_unit<"z", pow<-21>(mag<10>()), U> {};
template<NamedUnit auto U>
struct atto : prefixed_unit<"a", pow<-18>(mag<10>()), U> {};
template<NamedUnit auto U>
struct femto : prefixed_unit<"f", pow<-15>(mag<10>()), U> {};
template<NamedUnit auto U>
struct pico : prefixed_unit<"p", pow<-12>(mag<10>()), U> {};
template<NamedUnit auto U>
struct nano : prefixed_unit<"n", pow<-9>(mag<10>()), U> {};
template<NamedUnit auto U>
struct micro : prefixed_unit<basic_symbol_text{"\u00b5", "u"}, pow<-6>(mag<10>()), U> {};
template<NamedUnit auto U>
struct milli : prefixed_unit<"m", pow<-3>(mag<10>()), U> {};
template<NamedUnit auto U>
struct centi : prefixed_unit<"c", pow<-2>(mag<10>()), U> {};
template<NamedUnit auto U>
struct deci : prefixed_unit<"d", pow<-1>(mag<10>()), U> {};
template<NamedUnit auto U>
struct deca : prefixed_unit<"da", pow<1>(mag<10>()), U> {};
template<NamedUnit auto U>
struct hecto : prefixed_unit<"h", pow<2>(mag<10>()), U> {};
template<NamedUnit auto U>
struct kilo : prefixed_unit<"k", pow<3>(mag<10>()), U> {};
template<NamedUnit auto U>
struct mega : prefixed_unit<"M", pow<6>(mag<10>()), U> {};
template<NamedUnit auto U>
struct giga : prefixed_unit<"G", pow<9>(mag<10>()), U> {};
template<NamedUnit auto U>
struct tera : prefixed_unit<"T", pow<12>(mag<10>()), U> {};
template<NamedUnit auto U>
struct peta : prefixed_unit<"P", pow<15>(mag<10>()), U> {};
template<NamedUnit auto U>
struct exa : prefixed_unit<"E", pow<18>(mag<10>()), U> {};
template<NamedUnit auto U>
struct zetta : prefixed_unit<"Z", pow<21>(mag<10>()), U> {};
template<NamedUnit auto U>
struct yotta : prefixed_unit<"Y", pow<24>(mag<10>()), U> {};

}  // namespace units::isq::si
