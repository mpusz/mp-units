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
#include <units/physical/si/time.h>
#include <units/quantity.h>

namespace units::si {

struct hertz : named_unit<hertz, "Hz", prefix> {};
struct millihertz : prefixed_unit<millihertz, milli, hertz> {};
struct kilohertz : prefixed_unit<kilohertz, kilo, hertz> {};
struct megahertz : prefixed_unit<megahertz, mega, hertz> {};
struct gigahertz : prefixed_unit<gigahertz, giga, hertz> {};
struct terahertz : prefixed_unit<terahertz, tera, hertz> {};

struct dim_frequency : physical::dim_frequency<dim_frequency, hertz, dim_time> {};

template<Unit U, Scalar Rep = double>
using frequency = quantity<dim_frequency, U, Rep>;

inline namespace literals {

// Hz
constexpr auto operator"" Hz(unsigned long long l) { return frequency<hertz, std::int64_t>(l); }
constexpr auto operator"" Hz(long double l) { return frequency<hertz, long double>(l); }

// mHz
constexpr auto operator"" mHz(unsigned long long l) { return frequency<millihertz, std::int64_t>(l); }
constexpr auto operator"" mHz(long double l) { return frequency<millihertz, long double>(l); }

// kHz
constexpr auto operator"" kHz(unsigned long long l) { return frequency<kilohertz, std::int64_t>(l); }
constexpr auto operator"" kHz(long double l) { return frequency<kilohertz, long double>(l); }

// MHz
constexpr auto operator"" MHz(unsigned long long l) { return frequency<megahertz, std::int64_t>(l); }
constexpr auto operator"" MHz(long double l) { return frequency<megahertz, long double>(l); }

// GHz
constexpr auto operator"" GHz(unsigned long long l) { return frequency<gigahertz, std::int64_t>(l); }
constexpr auto operator"" GHz(long double l) { return frequency<gigahertz, long double>(l); }

// THz
constexpr auto operator"" THz(unsigned long long l) { return frequency<terahertz, std::int64_t>(l); }
constexpr auto operator"" THz(long double l) { return frequency<terahertz, long double>(l); }

}  // namespace literals

}  // namespace units::si
