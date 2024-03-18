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

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/unit.h>
#endif

namespace mp_units::si {

// clang-format off
template<PrefixableUnit U> struct quecto_ : prefixed_unit<"q", mag_power<10, -30>, U{}> {};
template<PrefixableUnit U> struct ronto_ : prefixed_unit<"r", mag_power<10, -27>, U{}> {};
template<PrefixableUnit U> struct yocto_ : prefixed_unit<"y", mag_power<10, -24>, U{}> {};
template<PrefixableUnit U> struct zepto_ : prefixed_unit<"z", mag_power<10, -21>, U{}> {};
template<PrefixableUnit U> struct atto_  : prefixed_unit<"a", mag_power<10, -18>, U{}> {};
template<PrefixableUnit U> struct femto_ : prefixed_unit<"f", mag_power<10, -15>, U{}> {};
template<PrefixableUnit U> struct pico_  : prefixed_unit<"p", mag_power<10, -12>, U{}> {};
template<PrefixableUnit U> struct nano_  : prefixed_unit<"n", mag_power<10, -9>, U{}> {};
template<PrefixableUnit U> struct micro_ : prefixed_unit<symbol_text{u8"µ", "u"}, mag_power<10, -6>, U{}> {};
template<PrefixableUnit U> struct milli_ : prefixed_unit<"m", mag_power<10, -3>, U{}> {};
template<PrefixableUnit U> struct centi_ : prefixed_unit<"c", mag_power<10, -2>, U{}> {};
template<PrefixableUnit U> struct deci_  : prefixed_unit<"d", mag_power<10, -1>, U{}> {};
template<PrefixableUnit U> struct deca_  : prefixed_unit<"da", mag_power<10, 1>, U{}> {};
template<PrefixableUnit U> struct hecto_ : prefixed_unit<"h", mag_power<10, 2>, U{}> {};
template<PrefixableUnit U> struct kilo_  : prefixed_unit<"k", mag_power<10, 3>, U{}> {};
template<PrefixableUnit U> struct mega_  : prefixed_unit<"M", mag_power<10, 6>, U{}> {};
template<PrefixableUnit U> struct giga_  : prefixed_unit<"G", mag_power<10, 9>, U{}> {};
template<PrefixableUnit U> struct tera_  : prefixed_unit<"T", mag_power<10, 12>, U{}> {};
template<PrefixableUnit U> struct peta_  : prefixed_unit<"P", mag_power<10, 15>, U{}> {};
template<PrefixableUnit U> struct exa_   : prefixed_unit<"E", mag_power<10, 18>, U{}> {};
template<PrefixableUnit U> struct zetta_ : prefixed_unit<"Z", mag_power<10, 21>, U{}> {};
template<PrefixableUnit U> struct yotta_ : prefixed_unit<"Y", mag_power<10, 24>, U{}> {};
template<PrefixableUnit U> struct ronna_ : prefixed_unit<"R", mag_power<10, 27>, U{}> {};
template<PrefixableUnit U> struct quetta_ : prefixed_unit<"Q", mag_power<10, 30>, U{}> {};

MP_UNITS_EXPORT_BEGIN

template<PrefixableUnit auto U> inline constexpr quecto_<std::remove_const_t<decltype(U)>> quecto;
template<PrefixableUnit auto U> inline constexpr ronto_<std::remove_const_t<decltype(U)>> ronto;
template<PrefixableUnit auto U> inline constexpr yocto_<std::remove_const_t<decltype(U)>> yocto;
template<PrefixableUnit auto U> inline constexpr zepto_<std::remove_const_t<decltype(U)>> zepto;
template<PrefixableUnit auto U> inline constexpr atto_<std::remove_const_t<decltype(U)>> atto;
template<PrefixableUnit auto U> inline constexpr femto_<std::remove_const_t<decltype(U)>> femto;
template<PrefixableUnit auto U> inline constexpr pico_<std::remove_const_t<decltype(U)>> pico;
template<PrefixableUnit auto U> inline constexpr nano_<std::remove_const_t<decltype(U)>> nano;
template<PrefixableUnit auto U> inline constexpr micro_<std::remove_const_t<decltype(U)>> micro;
template<PrefixableUnit auto U> inline constexpr milli_<std::remove_const_t<decltype(U)>> milli;
template<PrefixableUnit auto U> inline constexpr centi_<std::remove_const_t<decltype(U)>> centi;
template<PrefixableUnit auto U> inline constexpr deci_<std::remove_const_t<decltype(U)>> deci;
template<PrefixableUnit auto U> inline constexpr deca_<std::remove_const_t<decltype(U)>> deca;
template<PrefixableUnit auto U> inline constexpr hecto_<std::remove_const_t<decltype(U)>> hecto;
template<PrefixableUnit auto U> inline constexpr kilo_<std::remove_const_t<decltype(U)>> kilo;
template<PrefixableUnit auto U> inline constexpr mega_<std::remove_const_t<decltype(U)>> mega;
template<PrefixableUnit auto U> inline constexpr giga_<std::remove_const_t<decltype(U)>> giga;
template<PrefixableUnit auto U> inline constexpr tera_<std::remove_const_t<decltype(U)>> tera;
template<PrefixableUnit auto U> inline constexpr peta_<std::remove_const_t<decltype(U)>> peta;
template<PrefixableUnit auto U> inline constexpr exa_<std::remove_const_t<decltype(U)>> exa;
template<PrefixableUnit auto U> inline constexpr zetta_<std::remove_const_t<decltype(U)>> zetta;
template<PrefixableUnit auto U> inline constexpr yotta_<std::remove_const_t<decltype(U)>> yotta;
template<PrefixableUnit auto U> inline constexpr ronna_<std::remove_const_t<decltype(U)>> ronna;
template<PrefixableUnit auto U> inline constexpr quetta_<std::remove_const_t<decltype(U)>> quetta;
// clang-format on

MP_UNITS_EXPORT_END

}  // namespace mp_units::si
