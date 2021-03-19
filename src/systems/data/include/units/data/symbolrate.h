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

#include <units/isq/si/frequency.h>
#include <units/quantity.h>

namespace units::data {

struct baud : alias_unit<isq::si::hertz, "Bd", isq::si::prefix> {};
struct kilobaud : prefixed_alias_unit<isq::si::kilohertz, isq::si::kilo, baud> {};
struct megabaud : prefixed_alias_unit<isq::si::megahertz, isq::si::mega, baud> {};
struct gigabaud : prefixed_alias_unit<isq::si::gigahertz, isq::si::giga, baud> {};
struct terabaud : prefixed_alias_unit<isq::si::terahertz, isq::si::tera, baud> {};
struct petabaud : prefixed_alias_unit<isq::si::petahertz, isq::si::peta, baud> {};

template<UnitOf<isq::si::dim_frequency> U, QuantityValue Rep = double>
using symbolrate = quantity<isq::si::dim_frequency, U, Rep>;

inline namespace literals {

constexpr auto operator"" _q_Bd(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return symbolrate<baud, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_kBd(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return symbolrate<kilobaud, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_MBd(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return symbolrate<megabaud, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_GBd(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return symbolrate<gigabaud, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_TBd(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return symbolrate<terabaud, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_PBd(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return symbolrate<petabaud, std::int64_t>(static_cast<std::int64_t>(l)); }

}  // namespace literals

namespace references {

inline constexpr auto Bd = reference<isq::si::dim_frequency, baud>{};
inline constexpr auto kBd = reference<isq::si::dim_frequency, kilobaud>{};
inline constexpr auto MBd = reference<isq::si::dim_frequency, megabaud>{};
inline constexpr auto GBd = reference<isq::si::dim_frequency, gigabaud>{};
inline constexpr auto TBd = reference<isq::si::dim_frequency, terabaud>{};
inline constexpr auto PBd = reference<isq::si::dim_frequency, petabaud>{};

}  // namespace references

}  // namespace units::data
