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
#include <units/dimensions/force.h>
#include <units/dimensions/pressure.h>
#include <units/bits/fmt.h>

namespace units {

  struct energy : derived_dimension<energy, exp<force, 1>, exp<length, 1>> {};

  template<typename T>
  concept Energy =  QuantityOf<T, energy>;

  struct joule : coherent_derived_unit<joule, "J", energy, si_prefix> {};
  struct millijoule : prefixed_derived_unit<millijoule, milli, joule> {};
  struct kilojoule : prefixed_derived_unit<kilojoule, kilo, joule> {};
  struct megajoule : prefixed_derived_unit<megajoule, mega, joule> {};
  struct gigajoule : prefixed_derived_unit<gigajoule, giga, joule> {};

  inline namespace literals {

    // J
    constexpr auto operator""_J(unsigned long long l) { return quantity<joule, std::int64_t>(l); }
    constexpr auto operator""_J(long double l) { return quantity<joule, long double>(l); }

    // mJ
    constexpr auto operator""mJ(unsigned long long l) { return quantity<millijoule, std::int64_t>(l); }
    constexpr auto operator""mJ(long double l) { return quantity<millijoule, long double>(l); }

    // kJ
    constexpr auto operator""kJ(unsigned long long l) { return quantity<kilojoule, std::int64_t>(l); }
    constexpr auto operator""kJ(long double l) { return quantity<kilojoule, long double>(l); }

    // MJ
    constexpr auto operator""MJ(unsigned long long l) { return quantity<megajoule, std::int64_t>(l); }
    constexpr auto operator""MJ(long double l) { return quantity<megajoule, long double>(l); }

    // GJ
    constexpr auto operator""GJ(unsigned long long l) { return quantity<gigajoule, std::int64_t>(l); }
    constexpr auto operator""GJ(long double l) { return quantity<gigajoule, long double>(l); }

  }  // namespace literals

  namespace details {

    template<>
    inline icu::MeasureUnit* create_icu_unit<joule>(UErrorCode& uc)
    {
      return icu::MeasureUnit::createJoule(uc);
    }

    template<>
    inline icu::MeasureUnit* create_icu_unit<kilojoule>(UErrorCode& uc)
    {
      return icu::MeasureUnit::createKilojoule(uc);
    }
  }  // namespace details

}  // namespace units
