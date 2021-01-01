// The MIT License (MIT)
//
// Copyright (c) 2021 Christoph Seitz
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

#include <units/physical/dimensions/frequency.h>
#include <units/physical/si/derived/frequency.h>
#include <units/quantity.h>

namespace units::data {

namespace si = units::physical::si;

struct baud : alias_unit<si::hertz, "Bd", si::prefix> {};
struct kilobaud : prefixed_alias_unit<si::kilohertz, si::kilo, baud> {};
struct megabaud : prefixed_alias_unit<si::megahertz, si::mega, baud> {};
struct gigabaud : prefixed_alias_unit<si::gigahertz, si::giga, baud> {};
struct terabaud : prefixed_alias_unit<si::terahertz, si::tera, baud> {};
struct petabaud : prefixed_alias_unit<si::petahertz, si::peta, baud> {};

template<UnitOf<si::dim_frequency> U, QuantityValue Rep = double>
using symbolrate = quantity<si::dim_frequency, U, Rep>;

inline namespace literals {

// Bd
constexpr auto operator"" _q_Bd(unsigned long long l) { return symbolrate<baud, std::int64_t>(l); }

// kBd
constexpr auto operator"" _q_kBd(unsigned long long l) { return symbolrate<kilobaud, std::int64_t>(l); }

// MBd
constexpr auto operator"" _q_MBd(unsigned long long l) { return symbolrate<megabaud, std::int64_t>(l); }

// GBd
constexpr auto operator"" _q_GBd(unsigned long long l) { return symbolrate<gigabaud, std::int64_t>(l); }

// TBd
constexpr auto operator"" _q_TBd(unsigned long long l) { return symbolrate<terabaud, std::int64_t>(l); }

// PBd
constexpr auto operator"" _q_PBd(unsigned long long l) { return symbolrate<petabaud, std::int64_t>(l); }

}  // namespace literals

namespace unit_constants {

inline constexpr auto Bd = symbolrate<baud, one_rep>{};
inline constexpr auto kBd = symbolrate<kilobaud, one_rep>{};
inline constexpr auto MBd = symbolrate<megabaud, one_rep>{};
inline constexpr auto GBd = symbolrate<gigabaud, one_rep>{};
inline constexpr auto TBd = symbolrate<terabaud, one_rep>{};
inline constexpr auto PBd = symbolrate<petabaud, one_rep>{};

}  // namespace unit_constants

}  // namespace units::data
