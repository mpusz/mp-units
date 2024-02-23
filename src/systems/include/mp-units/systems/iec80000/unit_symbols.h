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
inline constexpr auto kbit = si::kilo<bit>;
inline constexpr auto Mbit = si::mega<bit>;
inline constexpr auto Gbit = si::giga<bit>;
inline constexpr auto Tbit = si::tera<bit>;
inline constexpr auto Pbit = si::peta<bit>;
inline constexpr auto Ebit = si::exa<bit>;
inline constexpr auto Zbit = si::zetta<bit>;
inline constexpr auto Ybit = si::yotta<bit>;
inline constexpr auto Rbit = si::ronna<bit>;
inline constexpr auto Qbit = si::quetta<bit>;

inline constexpr auto Kibit = kibi<bit>;
inline constexpr auto Mibit = mebi<bit>;
inline constexpr auto Gibit = gibi<bit>;
inline constexpr auto Tibit = tebi<bit>;
inline constexpr auto Pibit = pebi<bit>;
inline constexpr auto Eibit = exbi<bit>;

// octet
inline constexpr auto o = octet;

inline constexpr auto ko = si::kilo<octet>;
inline constexpr auto Mo = si::mega<octet>;
inline constexpr auto Go = si::giga<octet>;
inline constexpr auto To = si::tera<octet>;
inline constexpr auto Po = si::peta<octet>;
inline constexpr auto Eo = si::exa<octet>;
inline constexpr auto Zo = si::zetta<octet>;
inline constexpr auto Yo = si::yotta<octet>;
inline constexpr auto Ro = si::ronna<octet>;
inline constexpr auto Qo = si::quetta<octet>;

inline constexpr auto Kio = kibi<octet>;
inline constexpr auto Mio = mebi<octet>;
inline constexpr auto Gio = gibi<octet>;
inline constexpr auto Tio = tebi<octet>;
inline constexpr auto Pio = pebi<octet>;
inline constexpr auto Eio = exbi<octet>;

// byte
inline constexpr auto B = byte;

inline constexpr auto kB = si::kilo<byte>;
inline constexpr auto MB = si::mega<byte>;
inline constexpr auto GB = si::giga<byte>;
inline constexpr auto TB = si::tera<byte>;
inline constexpr auto PB = si::peta<byte>;
inline constexpr auto EB = si::exa<byte>;
inline constexpr auto ZB = si::zetta<byte>;
inline constexpr auto YB = si::yotta<byte>;
inline constexpr auto RB = si::ronna<byte>;
inline constexpr auto QB = si::quetta<byte>;

inline constexpr auto KiB = kibi<byte>;
inline constexpr auto MiB = mebi<byte>;
inline constexpr auto GiB = gibi<byte>;
inline constexpr auto TiB = tebi<byte>;
inline constexpr auto PiB = pebi<byte>;
inline constexpr auto EiB = exbi<byte>;

// baud
inline constexpr auto Bd = baud;
inline constexpr auto kBd = si::kilo<baud>;
inline constexpr auto MBd = si::mega<baud>;
inline constexpr auto GBd = si::giga<baud>;
inline constexpr auto TBd = si::tera<baud>;
inline constexpr auto PBd = si::peta<baud>;
inline constexpr auto EBd = si::exa<baud>;
inline constexpr auto ZBd = si::zetta<baud>;
inline constexpr auto YBd = si::yotta<baud>;
inline constexpr auto RBd = si::ronna<baud>;
inline constexpr auto QBd = si::quetta<baud>;

// erlang
// TODO do we need prefixed versions of Erlang?
inline constexpr auto E = erlang;

}  // namespace mp_units::iec80000::unit_symbols
