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

template<Unit U, ScalableNumber Rep = double>
using information = quantity<dim_information, U, Rep>;

inline namespace literals {

// bits
constexpr auto operator"" q_b(unsigned long long l) { return information<bit, std::int64_t>(l); }
constexpr auto operator"" q_Kib(unsigned long long l) { return information<kibibit, std::int64_t>(l); }
constexpr auto operator"" q_Mib(unsigned long long l) { return information<mebibit, std::int64_t>(l); }
constexpr auto operator"" q_Gib(unsigned long long l) { return information<gibibit, std::int64_t>(l); }
constexpr auto operator"" q_Tib(unsigned long long l) { return information<tebibit, std::int64_t>(l); }
constexpr auto operator"" q_Pib(unsigned long long l) { return information<pebibit, std::int64_t>(l); }

// bytes
constexpr auto operator"" q_B(unsigned long long l) { return information<byte, std::int64_t>(l); }
constexpr auto operator"" q_KiB(unsigned long long l) { return information<kibibyte, std::int64_t>(l); }
constexpr auto operator"" q_MiB(unsigned long long l) { return information<mebibyte, std::int64_t>(l); }
constexpr auto operator"" q_GiB(unsigned long long l) { return information<gibibyte, std::int64_t>(l); }
constexpr auto operator"" q_TiB(unsigned long long l) { return information<tebibyte, std::int64_t>(l); }
constexpr auto operator"" q_PiB(unsigned long long l) { return information<pebibyte, std::int64_t>(l); }

}  // namespace literals

}  // namespace units::data
