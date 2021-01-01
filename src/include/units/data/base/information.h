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

#include <units/one_rep.h>
#include <units/base_dimension.h>
#include <units/physical/si/prefixes.h>
#include <units/data/prefixes.h>
#include <units/unit.h>
#include <units/quantity.h>

namespace units::data {

struct bit : named_unit<bit, "b", prefix> {};
struct kibibit : prefixed_unit<kibibit, kibi, bit> {};
struct mebibit : prefixed_unit<mebibit, mebi, bit> {};
struct gibibit : prefixed_unit<gibibit, gibi, bit> {};
struct tebibit : prefixed_unit<tebibit, tebi, bit> {};
struct pebibit : prefixed_unit<pebibit, pebi, bit> {};

struct si_bit : alias_unit<bit, "b", units::physical::si::prefix> {};
struct kilobit : prefixed_unit<kilobit, units::physical::si::kilo, si_bit> {};
struct megabit : prefixed_unit<megabit, units::physical::si::mega, si_bit> {};
struct gigabit : prefixed_unit<gigabit, units::physical::si::giga, si_bit> {};
struct terabit : prefixed_unit<terabit, units::physical::si::tera, si_bit> {};
struct petabit : prefixed_unit<petabit, units::physical::si::peta, si_bit> {};

struct byte : named_scaled_unit<byte, "B", prefix, ratio(8), bit> {};
struct kibibyte : prefixed_unit<kibibyte, kibi, byte> {};
struct mebibyte : prefixed_unit<mebibyte, mebi, byte> {};
struct gibibyte : prefixed_unit<gibibyte, gibi, byte> {};
struct tebibyte : prefixed_unit<tebibyte, tebi, byte> {};
struct pebibyte : prefixed_unit<pebibyte, pebi, byte> {};

struct dim_information : base_dimension<"information", bit> {};

template<typename T>
concept Information = QuantityOf<T, dim_information>;

template<UnitOf<dim_information> U, QuantityValue Rep = double>
using information = quantity<dim_information, U, Rep>;

inline namespace literals {

// bits
constexpr auto operator"" _q_b(unsigned long long l) { return information<bit, std::int64_t>(l); }
constexpr auto operator"" _q_Kib(unsigned long long l) { return information<kibibit, std::int64_t>(l); }
constexpr auto operator"" _q_Mib(unsigned long long l) { return information<mebibit, std::int64_t>(l); }
constexpr auto operator"" _q_Gib(unsigned long long l) { return information<gibibit, std::int64_t>(l); }
constexpr auto operator"" _q_Tib(unsigned long long l) { return information<tebibit, std::int64_t>(l); }
constexpr auto operator"" _q_Pib(unsigned long long l) { return information<pebibit, std::int64_t>(l); }

// bits (SI)
constexpr auto operator"" _q_kb(unsigned long long l) { return information<kilobit, std::int64_t>(l); }
constexpr auto operator"" _q_Mb(unsigned long long l) { return information<megabit, std::int64_t>(l); }
constexpr auto operator"" _q_Gb(unsigned long long l) { return information<gigabit, std::int64_t>(l); }
constexpr auto operator"" _q_Tb(unsigned long long l) { return information<terabit, std::int64_t>(l); }
constexpr auto operator"" _q_Pb(unsigned long long l) { return information<petabit, std::int64_t>(l); }

// bytes
constexpr auto operator"" _q_B(unsigned long long l) { return information<byte, std::int64_t>(l); }
constexpr auto operator"" _q_KiB(unsigned long long l) { return information<kibibyte, std::int64_t>(l); }
constexpr auto operator"" _q_MiB(unsigned long long l) { return information<mebibyte, std::int64_t>(l); }
constexpr auto operator"" _q_GiB(unsigned long long l) { return information<gibibyte, std::int64_t>(l); }
constexpr auto operator"" _q_TiB(unsigned long long l) { return information<tebibyte, std::int64_t>(l); }
constexpr auto operator"" _q_PiB(unsigned long long l) { return information<pebibyte, std::int64_t>(l); }

}  // namespace literals

namespace unit_constants {

inline constexpr auto b = information<bit, one_rep>{};
inline constexpr auto Kib = information<kibibit, one_rep>{};
inline constexpr auto Mib = information<mebibit, one_rep>{};
inline constexpr auto Gib = information<gibibit, one_rep>{};
inline constexpr auto Tib = information<tebibit, one_rep>{};
inline constexpr auto Pib = information<pebibit, one_rep>{};

inline constexpr auto kb = information<kilobit, one_rep>{};
inline constexpr auto Mb = information<megabit, one_rep>{};
inline constexpr auto Gb = information<gigabit, one_rep>{};
inline constexpr auto Tb = information<terabit, one_rep>{};
inline constexpr auto Pb = information<petabit, one_rep>{};

inline constexpr auto B = information<byte, one_rep>{};
inline constexpr auto KiB = information<kibibyte, one_rep>{};
inline constexpr auto MiB = information<mebibyte, one_rep>{};
inline constexpr auto GiB = information<gibibyte, one_rep>{};
inline constexpr auto TiB = information<tebibyte, one_rep>{};
inline constexpr auto PiB = information<pebibyte, one_rep>{};

}  // namespace unit_constants

}  // namespace units::data
