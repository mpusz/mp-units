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

#include <units/physical/dimensions/frequency.h>
#include <units/physical/si/base/time.h>
#include <units/quantity.h>

namespace units::physical::si {

struct hertz : named_unit<hertz, "Hz", prefix> {};
struct yoctohertz : prefixed_unit<yoctohertz, yocto, hertz> {};
struct zeptohertz : prefixed_unit<zeptohertz, zepto, hertz> {};
struct attohertz : prefixed_unit<attohertz, atto, hertz> {};
struct femtohertz : prefixed_unit<femtohertz, femto, hertz> {};
struct picohertz : prefixed_unit<picohertz, pico, hertz> {};
struct nanohertz : prefixed_unit<nanohertz, nano, hertz> {};
struct microhertz : prefixed_unit<microhertz, micro, hertz> {};
struct millihertz : prefixed_unit<millihertz, milli, hertz> {};
struct kilohertz : prefixed_unit<kilohertz, kilo, hertz> {};
struct megahertz : prefixed_unit<megahertz, mega, hertz> {};
struct gigahertz : prefixed_unit<gigahertz, giga, hertz> {};
struct terahertz : prefixed_unit<terahertz, tera, hertz> {};
struct petahertz : prefixed_unit<petahertz, peta, hertz> {};
struct exahertz : prefixed_unit<exahertz, exa, hertz> {};
struct zettahertz : prefixed_unit<zettahertz, zetta, hertz> {};
struct yottahertz : prefixed_unit<yottahertz, yotta, hertz> {};

struct dim_frequency : physical::dim_frequency<dim_frequency, hertz, dim_time> {};

template<Unit U, ScalableNumber Rep = double>
using frequency = quantity<dim_frequency, U, Rep>;

inline namespace literals {

// Hz
constexpr auto operator"" _q_Hz(unsigned long long l) { return frequency<hertz, std::int64_t>(l); }
constexpr auto operator"" _q_Hz(long double l) { return frequency<hertz, long double>(l); }

// yHz
constexpr auto operator"" _q_yHz(unsigned long long l) { return frequency<yoctohertz, std::int64_t>(l); }
constexpr auto operator"" _q_yHz(long double l) { return frequency<yoctohertz, long double>(l); }

// zHz
constexpr auto operator"" _q_zHz(unsigned long long l) { return frequency<zeptohertz, std::int64_t>(l); }
constexpr auto operator"" _q_zHz(long double l) { return frequency<zeptohertz, long double>(l); }

// aHz
constexpr auto operator"" _q_aHz(unsigned long long l) { return frequency<attohertz, std::int64_t>(l); }
constexpr auto operator"" _q_aHz(long double l) { return frequency<attohertz, long double>(l); }

// fHz
constexpr auto operator"" _q_fHz(unsigned long long l) { return frequency<femtohertz, std::int64_t>(l); }
constexpr auto operator"" _q_fHz(long double l) { return frequency<femtohertz, long double>(l); }

// pHz
constexpr auto operator"" _q_pHz(unsigned long long l) { return frequency<picohertz, std::int64_t>(l); }
constexpr auto operator"" _q_pHz(long double l) { return frequency<picohertz, long double>(l); }

// nHz
constexpr auto operator"" _q_nHz(unsigned long long l) { return frequency<nanohertz, std::int64_t>(l); }
constexpr auto operator"" _q_nHz(long double l) { return frequency<nanohertz, long double>(l); }

// uHz
constexpr auto operator"" _q_uHz(unsigned long long l) { return frequency<microhertz, std::int64_t>(l); }
constexpr auto operator"" _q_uHz(long double l) { return frequency<microhertz, long double>(l); }

// mHz
constexpr auto operator"" _q_mHz(unsigned long long l) { return frequency<millihertz, std::int64_t>(l); }
constexpr auto operator"" _q_mHz(long double l) { return frequency<millihertz, long double>(l); }

// kHz
constexpr auto operator"" _q_kHz(unsigned long long l) { return frequency<kilohertz, std::int64_t>(l); }
constexpr auto operator"" _q_kHz(long double l) { return frequency<kilohertz, long double>(l); }

// MHz
constexpr auto operator"" _q_MHz(unsigned long long l) { return frequency<megahertz, std::int64_t>(l); }
constexpr auto operator"" _q_MHz(long double l) { return frequency<megahertz, long double>(l); }

// GHz
constexpr auto operator"" _q_GHz(unsigned long long l) { return frequency<gigahertz, std::int64_t>(l); }
constexpr auto operator"" _q_GHz(long double l) { return frequency<gigahertz, long double>(l); }

// THz
constexpr auto operator"" _q_THz(unsigned long long l) { return frequency<terahertz, std::int64_t>(l); }
constexpr auto operator"" _q_THz(long double l) { return frequency<terahertz, long double>(l); }

// PHz
constexpr auto operator"" _q_PHz(unsigned long long l) { return frequency<petahertz, std::int64_t>(l); }
constexpr auto operator"" _q_PHz(long double l) { return frequency<petahertz, long double>(l); }

// EHz
constexpr auto operator"" _q_EHz(unsigned long long l) { return frequency<exahertz, std::int64_t>(l); }
constexpr auto operator"" _q_EHz(long double l) { return frequency<exahertz, long double>(l); }

// ZHz
constexpr auto operator"" _q_ZHz(unsigned long long l) { return frequency<zettahertz, std::int64_t>(l); }
constexpr auto operator"" _q_ZHz(long double l) { return frequency<zettahertz, long double>(l); }

// YHz
constexpr auto operator"" _q_YHz(unsigned long long l) { return frequency<yottahertz, std::int64_t>(l); }
constexpr auto operator"" _q_YHz(long double l) { return frequency<yottahertz, long double>(l); }

}  // namespace literals

}  // namespace units::physical::si
