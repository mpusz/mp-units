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

#include <units/dimensions/si_base_dimensions.h>
#include <units/dimensions/si_prefixes.h>
#include <units/dimensions/energy.h>

namespace units {

  struct power : derived_dimension<power, exp<energy, 1>, exp<base_dim_time, -1>> {};

  template<typename T>
  concept Power =  QuantityOf<T, power>;

  struct watt : named_coherent_derived_unit<watt, power, "W", si_prefix> {};
  struct milliwatt : prefixed_derived_unit<milliwatt, milli, watt> {};
  struct kilowatt : prefixed_derived_unit<kilowatt, kilo, watt> {};
  struct megawatt : prefixed_derived_unit<megawatt, mega, watt> {};
  struct gigawatt : prefixed_derived_unit<gigawatt, giga, watt> {};

  inline namespace literals {

    // W
    constexpr auto operator""W(unsigned long long l) { return quantity<watt, std::int64_t>(l); }
    constexpr auto operator""_W(long double l) { return quantity<watt, long double>(l); }

    // mW
    constexpr auto operator""mW(unsigned long long l) { return quantity<milliwatt, std::int64_t>(l); }
    constexpr auto operator""mW(long double l) { return quantity<milliwatt, long double>(l); }

    // kW
    constexpr auto operator""kW(unsigned long long l) { return quantity<kilowatt, std::int64_t>(l); }
    constexpr auto operator""kW(long double l) { return quantity<kilowatt, long double>(l); }

    // MW
    constexpr auto operator""MW(unsigned long long l) { return quantity<megawatt, std::int64_t>(l); }
    constexpr auto operator""MW(long double l) { return quantity<megawatt, long double>(l); }

    // GW
    constexpr auto operator""GW(unsigned long long l) { return quantity<gigawatt, std::int64_t>(l); }
    constexpr auto operator""GW(long double l) { return quantity<gigawatt, long double>(l); }

  }  // namespace literals

}  // namespace units
