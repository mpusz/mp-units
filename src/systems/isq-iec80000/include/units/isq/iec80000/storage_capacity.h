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

#include <units/base_dimension.h>
#include <units/isq/iec80000/binary_prefixes.h>
#include <units/isq/si/prefixes.h>
#include <units/unit.h>

// IWYU pragma: begin_exports
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

namespace units::isq::iec80000 {

struct bit : named_unit<bit, "bit", si::prefix> {};
struct kilobit : prefixed_unit<kilobit, si::kilo, bit> {};
struct megabit : prefixed_unit<megabit, si::mega, bit> {};
struct gigabit : prefixed_unit<gigabit, si::giga, bit> {};
struct terabit : prefixed_unit<terabit, si::tera, bit> {};
struct petabit : prefixed_unit<petabit, si::peta, bit> {};
struct exabit : prefixed_unit<exabit, si::exa, bit> {};
struct zettabit : prefixed_unit<zettabit, si::zetta, bit> {};
struct yottabit : prefixed_unit<yottabit, si::yotta, bit> {};

struct binary_prefix_bit : alias_unit<bit, "bit", binary_prefix> {};
struct kibibit : prefixed_unit<kibibit, kibi, binary_prefix_bit> {};
struct mebibit : prefixed_unit<mebibit, mebi, binary_prefix_bit> {};
struct gibibit : prefixed_unit<gibibit, gibi, binary_prefix_bit> {};
struct tebibit : prefixed_unit<tebibit, tebi, binary_prefix_bit> {};
struct pebibit : prefixed_unit<pebibit, pebi, binary_prefix_bit> {};
struct exbibit : prefixed_unit<exbibit, exbi, binary_prefix_bit> {};

struct byte : named_scaled_unit<byte, "B", si::prefix, ratio(8), bit> {};
struct kilobyte : prefixed_unit<kilobyte, si::kilo, byte> {};
struct megabyte : prefixed_unit<megabyte, si::mega, byte> {};
struct gigabyte : prefixed_unit<gigabyte, si::giga, byte> {};
struct terabyte : prefixed_unit<terabyte, si::tera, byte> {};
struct petabyte : prefixed_unit<petabyte, si::peta, byte> {};
struct exabyte : prefixed_unit<exabyte, si::exa, byte> {};
struct zettabyte : prefixed_unit<zettabyte, si::zetta, byte> {};
struct yottabyte : prefixed_unit<yottabyte, si::yotta, byte> {};

struct binary_prefix_byte : alias_unit<byte, "B", binary_prefix> {};
struct kibibyte : prefixed_unit<kibibyte, kibi, binary_prefix_byte> {};
struct mebibyte : prefixed_unit<mebibyte, mebi, binary_prefix_byte> {};
struct gibibyte : prefixed_unit<gibibyte, gibi, binary_prefix_byte> {};
struct tebibyte : prefixed_unit<tebibyte, tebi, binary_prefix_byte> {};
struct pebibyte : prefixed_unit<pebibyte, pebi, binary_prefix_byte> {};
// struct exbibyte : prefixed_unit<exbibyte, exbi, binary_prefix_byte> {};

struct dim_storage_capacity : base_dimension<"M", byte> {};

template<typename T>
concept StorageCapacity = QuantityOf<T, dim_storage_capacity>;

template<UnitOf<dim_storage_capacity> U, Representation Rep = double>
using storage_capacity = quantity<dim_storage_capacity, U, Rep>;

#ifdef UNITS_LITERALS

inline namespace literals {

// bits
constexpr auto operator"" _q_bit(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<bit, std::int64_t>(static_cast<std::int64_t>(l)); }

constexpr auto operator"" _q_kbit(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<kilobit, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Mbit(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<megabit, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Gbit(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<gigabit, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Tbit(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<terabit, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Pbit(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<petabit, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Ebit(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<exabit, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Zbit(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<zettabit, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Ybit(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<yottabit, std::int64_t>(static_cast<std::int64_t>(l)); }

constexpr auto operator"" _q_Kibit(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<kibibit, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Mibit(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<mebibit, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Gibit(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<gibibit, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Tibit(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<tebibit, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Pibit(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<pebibit, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Eibit(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<exbibit, std::int64_t>(static_cast<std::int64_t>(l)); }

// bytes
constexpr auto operator"" _q_B(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<byte, std::int64_t>(static_cast<std::int64_t>(l)); }

constexpr auto operator"" _q_kB(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<kilobyte, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_MB(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<megabyte, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_GB(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<gigabyte, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_TB(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<terabyte, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_PB(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<petabyte, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_EB(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<exabyte, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_ZB(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<zettabyte, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_YB(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<yottabyte, std::int64_t>(static_cast<std::int64_t>(l)); }

constexpr auto operator"" _q_KiB(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<kibibyte, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_MiB(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<mebibyte, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_GiB(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<gibibyte, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_TiB(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<tebibyte, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_PiB(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<pebibyte, std::int64_t>(static_cast<std::int64_t>(l)); }
// constexpr auto operator"" _q_EiB(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return storage_capacity<exbibyte, std::int64_t>(static_cast<std::int64_t>(l)); }

}  // namespace literals

#endif // UNITS_LITERALS

namespace references {

// bits
inline constexpr auto bit = reference<dim_storage_capacity, iec80000::bit>{};

inline constexpr auto kbit = reference<dim_storage_capacity, kilobit>{};
inline constexpr auto Mbit = reference<dim_storage_capacity, megabit>{};
inline constexpr auto Gbit = reference<dim_storage_capacity, gigabit>{};
inline constexpr auto Tbit = reference<dim_storage_capacity, terabit>{};
inline constexpr auto Pbit = reference<dim_storage_capacity, petabit>{};
inline constexpr auto Ebit = reference<dim_storage_capacity, exabit>{};
inline constexpr auto Zbit = reference<dim_storage_capacity, zettabit>{};
inline constexpr auto Ybit = reference<dim_storage_capacity, yottabit>{};

inline constexpr auto Kibit = reference<dim_storage_capacity, kibibit>{};
inline constexpr auto Mibit = reference<dim_storage_capacity, mebibit>{};
inline constexpr auto Gibit = reference<dim_storage_capacity, gibibit>{};
inline constexpr auto Tibit = reference<dim_storage_capacity, tebibit>{};
inline constexpr auto Pibit = reference<dim_storage_capacity, pebibit>{};
inline constexpr auto Eibit = reference<dim_storage_capacity, exbibit>{};

// bytes
inline constexpr auto B = reference<dim_storage_capacity, byte>{};

inline constexpr auto kB = reference<dim_storage_capacity, kilobyte>{};
inline constexpr auto MB = reference<dim_storage_capacity, megabyte>{};
inline constexpr auto GB = reference<dim_storage_capacity, gigabyte>{};
inline constexpr auto TB = reference<dim_storage_capacity, terabyte>{};
inline constexpr auto PB = reference<dim_storage_capacity, petabyte>{};
inline constexpr auto EB = reference<dim_storage_capacity, exabyte>{};
inline constexpr auto ZB = reference<dim_storage_capacity, zettabyte>{};
inline constexpr auto YB = reference<dim_storage_capacity, yottabyte>{};

inline constexpr auto KiB = reference<dim_storage_capacity, kibibyte>{};
inline constexpr auto MiB = reference<dim_storage_capacity, mebibyte>{};
inline constexpr auto GiB = reference<dim_storage_capacity, gibibyte>{};
inline constexpr auto TiB = reference<dim_storage_capacity, tebibyte>{};
inline constexpr auto PiB = reference<dim_storage_capacity, pebibyte>{};
// inline constexpr auto EiB = reference<dim_storage_capacity, exbibyte>{};

}  // namespace references

}  // namespace units::isq::iec80000
