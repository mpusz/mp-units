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

#include <units/physical/dimensions.h>
#include <units/physical/si/force.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

namespace units::physical::si {

struct joule : named_unit<joule, "J", prefix> {};
struct yoctojoule : prefixed_unit<yoctojoule, yocto, joule> {};
struct zeptojoule : prefixed_unit<zeptojoule, zepto, joule> {};
struct attojoule : prefixed_unit<attojoule, atto, joule> {};
struct femtojoule : prefixed_unit<femtojoule, femto, joule> {};
struct picojoule : prefixed_unit<picojoule, pico, joule> {};
struct nanojoule : prefixed_unit<nanojoule, nano, joule> {};
struct microjoule : prefixed_unit<microjoule, micro, joule> {};
struct millijoule : prefixed_unit<millijoule, milli, joule> {};
struct kilojoule : prefixed_unit<kilojoule, kilo, joule> {};
struct megajoule : prefixed_unit<megajoule, mega, joule> {};
struct gigajoule : prefixed_unit<gigajoule, giga, joule> {};
struct terajoule : prefixed_unit<terajoule, tera, joule> {};
struct petajoule : prefixed_unit<petajoule, peta, joule> {};
struct exajoule : prefixed_unit<exajoule, exa, joule> {};
struct zettajoule : prefixed_unit<zettajoule, zetta, joule> {};
struct yottajoule : prefixed_unit<yottajoule, yotta, joule> {};

struct electronvolt : named_scaled_unit<electronvolt, "eV", prefix, ratio(1'602'176'634, 1'000'000'000, -19), joule> {};
struct gigaelectronvolt : prefixed_unit<gigaelectronvolt, giga, electronvolt> {};

struct dim_energy : physical::dim_energy<dim_energy, joule, dim_force, dim_length> {};

template<Unit U, ScalableNumber Rep = double>
using energy = quantity<dim_energy, U, Rep>;

inline namespace literals {

// J
constexpr auto operator"" q_J(unsigned long long l) { return energy<joule, std::int64_t>(l); }
constexpr auto operator"" q_J(long double l) { return energy<joule, long double>(l); }

// yJ
constexpr auto operator"" q_yJ(unsigned long long l) { return energy<yoctojoule, std::int64_t>(l); }
constexpr auto operator"" q_yJ(long double l) { return energy<yoctojoule, long double>(l); }

// zJ
constexpr auto operator"" q_zJ(unsigned long long l) { return energy<zeptojoule, std::int64_t>(l); }
constexpr auto operator"" q_zJ(long double l) { return energy<zeptojoule, long double>(l); }

// aJ
constexpr auto operator"" q_aJ(unsigned long long l) { return energy<attojoule, std::int64_t>(l); }
constexpr auto operator"" q_aJ(long double l) { return energy<attojoule, long double>(l); }

// fJ
constexpr auto operator"" q_fJ(unsigned long long l) { return energy<femtojoule, std::int64_t>(l); }
constexpr auto operator"" q_fJ(long double l) { return energy<femtojoule, long double>(l); }

// pJ
constexpr auto operator"" q_pJ(unsigned long long l) { return energy<picojoule, std::int64_t>(l); }
constexpr auto operator"" q_pJ(long double l) { return energy<picojoule, long double>(l); }

// nJ
constexpr auto operator"" q_nJ(unsigned long long l) { return energy<nanojoule, std::int64_t>(l); }
constexpr auto operator"" q_nJ(long double l) { return energy<nanojoule, long double>(l); }

// uJ
constexpr auto operator"" q_uJ(unsigned long long l) { return energy<microjoule, std::int64_t>(l); }
constexpr auto operator"" q_uJ(long double l) { return energy<microjoule, long double>(l); }

// mJ
constexpr auto operator"" q_mJ(unsigned long long l) { return energy<millijoule, std::int64_t>(l); }
constexpr auto operator"" q_mJ(long double l) { return energy<millijoule, long double>(l); }

// kJ
constexpr auto operator"" q_kJ(unsigned long long l) { return energy<kilojoule, std::int64_t>(l); }
constexpr auto operator"" q_kJ(long double l) { return energy<kilojoule, long double>(l); }

// MJ
constexpr auto operator"" q_MJ(unsigned long long l) { return energy<megajoule, std::int64_t>(l); }
constexpr auto operator"" q_MJ(long double l) { return energy<megajoule, long double>(l); }

// GJ
constexpr auto operator"" q_GJ(unsigned long long l) { return energy<gigajoule, std::int64_t>(l); }
constexpr auto operator"" q_GJ(long double l) { return energy<gigajoule, long double>(l); }

// TJ
constexpr auto operator"" q_TJ(unsigned long long l) { return energy<terajoule, std::int64_t>(l); }
constexpr auto operator"" q_TJ(long double l) { return energy<terajoule, long double>(l); }

// PJ
constexpr auto operator"" q_PJ(unsigned long long l) { return energy<petajoule, std::int64_t>(l); }
constexpr auto operator"" q_PJ(long double l) { return energy<petajoule, long double>(l); }

// EJ
constexpr auto operator"" q_EJ(unsigned long long l) { return energy<exajoule, std::int64_t>(l); }
constexpr auto operator"" q_EJ(long double l) { return energy<exajoule, long double>(l); }

// ZJ
constexpr auto operator"" q_ZJ(unsigned long long l) { return energy<zettajoule, std::int64_t>(l); }
constexpr auto operator"" q_ZJ(long double l) { return energy<zettajoule, long double>(l); }

// YJ
constexpr auto operator"" q_YJ(unsigned long long l) { return energy<yottajoule, std::int64_t>(l); }
constexpr auto operator"" q_YJ(long double l) { return energy<yottajoule, long double>(l); }

// eV
constexpr auto operator"" q_eV(unsigned long long l) { return energy<electronvolt, std::int64_t>(l); }
constexpr auto operator"" q_eV(long double l) { return energy<electronvolt, long double>(l); }

// GeV
constexpr auto operator"" q_GeV(unsigned long long l) { return energy<gigaelectronvolt, std::int64_t>(l); }
constexpr auto operator"" q_GeV(long double l) { return energy<gigaelectronvolt, long double>(l); }

}  // namespace literals

}  // namespace units::physical::si
