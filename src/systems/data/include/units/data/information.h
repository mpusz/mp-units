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

#include <units/reference.h>
#include <units/base_dimension.h>
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
constexpr auto operator"" _q_b(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return information<bit, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Kib(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return information<kibibit, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Mib(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return information<mebibit, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Gib(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return information<gibibit, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Tib(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return information<tebibit, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Pib(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return information<pebibit, std::int64_t>(static_cast<std::int64_t>(l)); }

// bytes
constexpr auto operator"" _q_B(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return information<byte, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_KiB(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return information<kibibyte, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_MiB(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return information<mebibyte, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_GiB(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return information<gibibyte, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_TiB(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return information<tebibyte, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_PiB(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return information<pebibyte, std::int64_t>(static_cast<std::int64_t>(l)); }

}  // namespace literals

namespace references {

inline constexpr auto b = reference<dim_information, bit>{};
inline constexpr auto Kib = reference<dim_information, kibibit>{};
inline constexpr auto Mib = reference<dim_information, mebibit>{};
inline constexpr auto Gib = reference<dim_information, gibibit>{};
inline constexpr auto Tib = reference<dim_information, tebibit>{};
inline constexpr auto Pib = reference<dim_information, pebibit>{};

inline constexpr auto B = reference<dim_information, byte>{};
inline constexpr auto KiB = reference<dim_information, kibibyte>{};
inline constexpr auto MiB = reference<dim_information, mebibyte>{};
inline constexpr auto GiB = reference<dim_information, gibibyte>{};
inline constexpr auto TiB = reference<dim_information, tebibyte>{};
inline constexpr auto PiB = reference<dim_information, pebibyte>{};

}  // namespace references

}  // namespace units::data
