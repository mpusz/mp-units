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
#include <mp-units/systems/si.h>
#include <cstdint>

// IWYU pragma: begin_exports
#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework.h>
#endif
// IWYU pragma: end_exports

MP_UNITS_EXPORT
namespace mp_units {

template<>
constexpr std::optional<std::intmax_t> known_first_factor<334'524'384'739> = 334'524'384'739;

namespace hep {

// energy
using si::electronvolt;

// clang-format off

// area
// effective cross-sectional area according to EU council directive 80/181/EEC
// https://eur-lex.europa.eu/legal-content/EN/TXT/PDF/?uri=CELEX:01980L0181-20090527#page=10
// https://www.fedlex.admin.ch/eli/cc/1994/3109_3109_3109/de
constexpr struct barn final : named_unit<"b", mag_power<10, -28> * square(si::metre)> {} barn;

// mass
constexpr struct electron_mass final : named_unit<"m_e", mag_ratio<9'109'383'701'528, 1'000'000'000'000> * mag_power<10, -31> * si::kilogram> {} electron_mass;
constexpr struct proton_mass final : named_unit<"m_p", mag_ratio<1'672'621'923'695, 1'000'000'000'000> * mag_power<10, -27> * si::kilogram> {} proton_mass;
constexpr struct neutron_mass final : named_unit<"m_n", mag_ratio<1'674'927'498'049, 1'000'000'000'000> * mag_power<10, -27> * si::kilogram> {} neutron_mass;

// speed
constexpr auto speed_of_light = si::si2019::speed_of_light_in_vacuum;
// clang-format on

namespace unit_symbols {

using si::unit_symbols::eV;

constexpr auto qeV = si::quecto<electronvolt>;
constexpr auto reV = si::ronto<electronvolt>;
constexpr auto yeV = si::yocto<electronvolt>;
constexpr auto zeV = si::zepto<electronvolt>;
constexpr auto aeV = si::atto<electronvolt>;
constexpr auto feV = si::femto<electronvolt>;
constexpr auto peV = si::pico<electronvolt>;
constexpr auto neV = si::nano<electronvolt>;
constexpr auto ueV = si::micro<electronvolt>;
constexpr auto meV = si::milli<electronvolt>;
constexpr auto ceV = si::centi<electronvolt>;
constexpr auto deV = si::deci<electronvolt>;
constexpr auto daeV = si::deca<electronvolt>;
constexpr auto heV = si::hecto<electronvolt>;
constexpr auto keV = si::kilo<electronvolt>;
constexpr auto MeV = si::mega<electronvolt>;
constexpr auto GeV = si::giga<electronvolt>;
constexpr auto TeV = si::tera<electronvolt>;
constexpr auto PeV = si::peta<electronvolt>;
constexpr auto EeV = si::exa<electronvolt>;
constexpr auto ZeV = si::zetta<electronvolt>;
constexpr auto YeV = si::yotta<electronvolt>;
constexpr auto ReV = si::ronna<electronvolt>;
constexpr auto QeV = si::quetta<electronvolt>;

constexpr auto qb = si::quecto<barn>;
constexpr auto rb = si::ronto<barn>;
constexpr auto yb = si::yocto<barn>;
constexpr auto zb = si::zepto<barn>;
constexpr auto ab = si::atto<barn>;
constexpr auto fb = si::femto<barn>;
constexpr auto pb = si::pico<barn>;
constexpr auto nb = si::nano<barn>;
constexpr auto ub = si::micro<barn>;
constexpr auto mb = si::milli<barn>;
constexpr auto b = barn;

constexpr auto m_e = electron_mass;
constexpr auto m_p = proton_mass;
constexpr auto m_n = neutron_mass;

constexpr auto c = speed_of_light;
constexpr auto c2 = square(speed_of_light);

}  // namespace unit_symbols
}  // namespace hep
}  // namespace mp_units
