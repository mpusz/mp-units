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

namespace units::si {

template<PrefixableUnit auto U>
struct yocto_ : prefixed_unit<"y", mag_power<10, -24>, U> {};
template<PrefixableUnit auto U>
struct zepto_ : prefixed_unit<"z", mag_power<10, -21>, U> {};
template<PrefixableUnit auto U>
struct atto_ : prefixed_unit<"a", mag_power<10, -18>, U> {};
template<PrefixableUnit auto U>
struct femto_ : prefixed_unit<"f", mag_power<10, -15>, U> {};
template<PrefixableUnit auto U>
struct pico_ : prefixed_unit<"p", mag_power<10, -12>, U> {};
template<PrefixableUnit auto U>
struct nano_ : prefixed_unit<"n", mag_power<10, -9>, U> {};
template<PrefixableUnit auto U>
struct micro_ : prefixed_unit<basic_symbol_text{"\u00b5", "u"}, mag_power<10, -6>, U> {};
template<PrefixableUnit auto U>
struct milli_ : prefixed_unit<"m", mag_power<10, -3>, U> {};
template<PrefixableUnit auto U>
struct centi_ : prefixed_unit<"c", mag_power<10, -2>, U> {};
template<PrefixableUnit auto U>
struct deci_ : prefixed_unit<"d", mag_power<10, -1>, U> {};
template<PrefixableUnit auto U>
struct deca_ : prefixed_unit<"da", mag_power<10, 1>, U> {};
template<PrefixableUnit auto U>
struct hecto_ : prefixed_unit<"h", mag_power<10, 2>, U> {};
template<PrefixableUnit auto U>
struct kilo_ : prefixed_unit<"k", mag_power<10, 3>, U> {};
template<PrefixableUnit auto U>
struct mega_ : prefixed_unit<"M", mag_power<10, 6>, U> {};
template<PrefixableUnit auto U>
struct giga_ : prefixed_unit<"G", mag_power<10, 9>, U> {};
template<PrefixableUnit auto U>
struct tera_ : prefixed_unit<"T", mag_power<10, 12>, U> {};
template<PrefixableUnit auto U>
struct peta_ : prefixed_unit<"P", mag_power<10, 15>, U> {};
template<PrefixableUnit auto U>
struct exa_ : prefixed_unit<"E", mag_power<10, 18>, U> {};
template<PrefixableUnit auto U>
struct zetta_ : prefixed_unit<"Z", mag_power<10, 21>, U> {};
template<PrefixableUnit auto U>
struct yotta_ : prefixed_unit<"Y", mag_power<10, 24>, U> {};

template<PrefixableUnit auto U>
inline constexpr yocto_<U> yocto;
template<PrefixableUnit auto U>
inline constexpr zepto_<U> zepto;
template<PrefixableUnit auto U>
inline constexpr atto_<U> atto;
template<PrefixableUnit auto U>
inline constexpr femto_<U> femto;
template<PrefixableUnit auto U>
inline constexpr pico_<U> pico;
template<PrefixableUnit auto U>
inline constexpr nano_<U> nano;
template<PrefixableUnit auto U>
inline constexpr micro_<U> micro;
template<PrefixableUnit auto U>
inline constexpr milli_<U> milli;
template<PrefixableUnit auto U>
inline constexpr centi_<U> centi;
template<PrefixableUnit auto U>
inline constexpr deci_<U> deci;
template<PrefixableUnit auto U>
inline constexpr deca_<U> deca;
template<PrefixableUnit auto U>
inline constexpr hecto_<U> hecto;
template<PrefixableUnit auto U>
inline constexpr kilo_<U> kilo;
template<PrefixableUnit auto U>
inline constexpr mega_<U> mega;
template<PrefixableUnit auto U>
inline constexpr giga_<U> giga;
template<PrefixableUnit auto U>
inline constexpr tera_<U> tera;
template<PrefixableUnit auto U>
inline constexpr peta_<U> peta;
template<PrefixableUnit auto U>
inline constexpr exa_<U> exa;
template<PrefixableUnit auto U>
inline constexpr zetta_<U> zetta;
template<PrefixableUnit auto U>
inline constexpr yotta_<U> yotta;

}  // namespace units::si
