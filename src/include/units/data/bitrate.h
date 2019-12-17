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

#include <units/data/information.h>
#include <units/derived_dimension.h>
#include <units/physical/si/time.h>
#include <units/quantity.h>

namespace units::data {

struct bit_per_second : unit<bit_per_second> {};
struct dim_bitrate : derived_dimension<dim_bitrate, bit_per_second, exp<dim_information, 1>, exp<si::dim_time, -1>> {};

struct kibibit_per_second : deduced_unit<kibibit_per_second, dim_bitrate, kibibit, si::second> {};
struct mebibit_per_second : deduced_unit<mebibit_per_second, dim_bitrate, mebibit, si::second> {};
struct gibibit_per_second : deduced_unit<gibibit_per_second, dim_bitrate, gibibit, si::second> {};
struct tebibit_per_second : deduced_unit<tebibit_per_second, dim_bitrate, tebibit, si::second> {};
struct pebibit_per_second : deduced_unit<pebibit_per_second, dim_bitrate, pebibit, si::second> {};

template<typename T>
concept Bitrate = QuantityOf<T, dim_bitrate>;

template<Unit U, Scalar Rep = double>
using bitrate = quantity<dim_bitrate, U, Rep>;

inline namespace literals {

// bits
constexpr auto operator""_bps(unsigned long long l) { return bitrate<bit_per_second, std::int64_t>(l); }
constexpr auto operator""_Kibps(unsigned long long l) { return bitrate<kibibit_per_second, std::int64_t>(l); }
constexpr auto operator""_Mibps(unsigned long long l) { return bitrate<mebibit_per_second, std::int64_t>(l); }
constexpr auto operator""_Gibps(unsigned long long l) { return bitrate<gibibit_per_second, std::int64_t>(l); }
constexpr auto operator""_Tibps(unsigned long long l) { return bitrate<tebibit_per_second, std::int64_t>(l); }
constexpr auto operator""_Pibps(unsigned long long l) { return bitrate<pebibit_per_second, std::int64_t>(l); }

}  // namespace literals

}  // namespace units::data
