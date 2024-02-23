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

#pragma once

#include <mp-units/bits/module_macros.h>
#include <mp-units/systems/si/si.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/unit.h>
#endif

MP_UNITS_EXPORT
namespace mp_units {

template<>
inline constexpr std::optional<std::intmax_t> known_first_factor<334'524'384'739> = 334'524'384'739;

namespace hep {

// energy
using si::electronvolt;

// clang-format off

// area
// effective cross-sectional area according to EU council directive 80/181/EEC
// https://eur-lex.europa.eu/legal-content/EN/TXT/PDF/?uri=CELEX:01980L0181-20090527#page=10
// https://www.fedlex.admin.ch/eli/cc/1994/3109_3109_3109/de
inline constexpr struct barn : named_unit<"b", mag_power<10, -28> * square(si::metre)> {} barn;

// mass
inline constexpr struct electron_mass : named_unit<"m_e", mag<ratio{9'109'383'701'528, 1'000'000'000'000}> * mag_power<10, -31> * si::kilogram> {} electron_mass;
inline constexpr struct proton_mass : named_unit<"m_p", mag<ratio{1'672'621'923'695, 1'000'000'000'000}> * mag_power<10, -27> * si::kilogram> {} proton_mass;
inline constexpr struct neutron_mass : named_unit<"m_n", mag<ratio{1'674'927'498'049, 1'000'000'000'000}> * mag_power<10, -27> * si::kilogram> {} neutron_mass;

// speed
inline constexpr struct speed_of_light : decltype(si::si2019::speed_of_light_in_vacuum) {} speed_of_light;
// clang-format on

namespace unit_symbols {

using si::unit_symbols::eV;

inline constexpr auto qeV = si::quecto<electronvolt>;
inline constexpr auto reV = si::ronto<electronvolt>;
inline constexpr auto yeV = si::yocto<electronvolt>;
inline constexpr auto zeV = si::zepto<electronvolt>;
inline constexpr auto aeV = si::atto<electronvolt>;
inline constexpr auto feV = si::femto<electronvolt>;
inline constexpr auto peV = si::pico<electronvolt>;
inline constexpr auto neV = si::nano<electronvolt>;
inline constexpr auto ueV = si::micro<electronvolt>;
inline constexpr auto meV = si::milli<electronvolt>;
inline constexpr auto ceV = si::centi<electronvolt>;
inline constexpr auto deV = si::deci<electronvolt>;
inline constexpr auto daeV = si::deca<electronvolt>;
inline constexpr auto heV = si::hecto<electronvolt>;
inline constexpr auto keV = si::kilo<electronvolt>;
inline constexpr auto MeV = si::mega<electronvolt>;
inline constexpr auto GeV = si::giga<electronvolt>;
inline constexpr auto TeV = si::tera<electronvolt>;
inline constexpr auto PeV = si::peta<electronvolt>;
inline constexpr auto EeV = si::exa<electronvolt>;
inline constexpr auto ZeV = si::zetta<electronvolt>;
inline constexpr auto YeV = si::yotta<electronvolt>;
inline constexpr auto ReV = si::ronna<electronvolt>;
inline constexpr auto QeV = si::quetta<electronvolt>;

inline constexpr auto qb = si::quecto<barn>;
inline constexpr auto rb = si::ronto<barn>;
inline constexpr auto yb = si::yocto<barn>;
inline constexpr auto zb = si::zepto<barn>;
inline constexpr auto ab = si::atto<barn>;
inline constexpr auto fb = si::femto<barn>;
inline constexpr auto pb = si::pico<barn>;
inline constexpr auto nb = si::nano<barn>;
inline constexpr auto ub = si::micro<barn>;
inline constexpr auto mb = si::milli<barn>;
inline constexpr auto b = barn;

inline constexpr auto m_e = electron_mass;
inline constexpr auto m_p = proton_mass;
inline constexpr auto m_n = neutron_mass;

inline constexpr auto c = speed_of_light;
inline constexpr auto c2 = square(speed_of_light);

}  // namespace unit_symbols
}  // namespace hep
}  // namespace mp_units
