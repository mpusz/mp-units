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
#include <mp-units/systems/iec80000/binary_prefixes.h>
#include <mp-units/systems/iec80000/units.h>
#include <mp-units/systems/si/prefixes.h>

MP_UNITS_EXPORT
namespace mp_units::iec80000::unit_symbols {

// bit
constexpr auto kbit = si::kilo<bit>;
constexpr auto Mbit = si::mega<bit>;
constexpr auto Gbit = si::giga<bit>;
constexpr auto Tbit = si::tera<bit>;
constexpr auto Pbit = si::peta<bit>;
constexpr auto Ebit = si::exa<bit>;
constexpr auto Zbit = si::zetta<bit>;
constexpr auto Ybit = si::yotta<bit>;
constexpr auto Rbit = si::ronna<bit>;
constexpr auto Qbit = si::quetta<bit>;

constexpr auto Kibit = kibi<bit>;
constexpr auto Mibit = mebi<bit>;
constexpr auto Gibit = gibi<bit>;
constexpr auto Tibit = tebi<bit>;
constexpr auto Pibit = pebi<bit>;
constexpr auto Eibit = exbi<bit>;

// octet
constexpr auto o = octet;

constexpr auto ko = si::kilo<octet>;
constexpr auto Mo = si::mega<octet>;
constexpr auto Go = si::giga<octet>;
constexpr auto To = si::tera<octet>;
constexpr auto Po = si::peta<octet>;
constexpr auto Eo = si::exa<octet>;
constexpr auto Zo = si::zetta<octet>;
constexpr auto Yo = si::yotta<octet>;
constexpr auto Ro = si::ronna<octet>;
constexpr auto Qo = si::quetta<octet>;

constexpr auto Kio = kibi<octet>;
constexpr auto Mio = mebi<octet>;
constexpr auto Gio = gibi<octet>;
constexpr auto Tio = tebi<octet>;
constexpr auto Pio = pebi<octet>;
constexpr auto Eio = exbi<octet>;

// byte
constexpr auto B = byte;

constexpr auto kB = si::kilo<byte>;
constexpr auto MB = si::mega<byte>;
constexpr auto GB = si::giga<byte>;
constexpr auto TB = si::tera<byte>;
constexpr auto PB = si::peta<byte>;
constexpr auto EB = si::exa<byte>;
constexpr auto ZB = si::zetta<byte>;
constexpr auto YB = si::yotta<byte>;
constexpr auto RB = si::ronna<byte>;
constexpr auto QB = si::quetta<byte>;

constexpr auto KiB = kibi<byte>;
constexpr auto MiB = mebi<byte>;
constexpr auto GiB = gibi<byte>;
constexpr auto TiB = tebi<byte>;
constexpr auto PiB = pebi<byte>;
constexpr auto EiB = exbi<byte>;

// baud
constexpr auto Bd = baud;
constexpr auto kBd = si::kilo<baud>;
constexpr auto MBd = si::mega<baud>;
constexpr auto GBd = si::giga<baud>;
constexpr auto TBd = si::tera<baud>;
constexpr auto PBd = si::peta<baud>;
constexpr auto EBd = si::exa<baud>;
constexpr auto ZBd = si::zetta<baud>;
constexpr auto YBd = si::yotta<baud>;
constexpr auto RBd = si::ronna<baud>;
constexpr auto QBd = si::quetta<baud>;

// erlang
// TODO do we need prefixed versions of Erlang?
constexpr auto E = erlang;

}  // namespace mp_units::iec80000::unit_symbols
