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
#include <units/quantity.h>

namespace units {

  struct length : derived_dimension<length, exp<base_dim_length, 1>> {};

  template<typename T>
  concept Length = QuantityOf<T, length>;

  // SI units
  struct metre : coherent_derived_unit<metre, "m", length, si_prefix> {};
  struct millimetre : prefixed_derived_unit<millimetre, milli, metre> {};
  struct centimetre : prefixed_derived_unit<centimetre, centi, metre> {};
  struct kilometre : prefixed_derived_unit<kilometre, kilo, metre> {};

  inline namespace literals {

    // m
    constexpr auto operator""m(unsigned long long l) { return quantity<metre, std::int64_t>(l); }
    constexpr auto operator""m(long double l) { return quantity<metre, long double>(l); }

    // mm
    constexpr auto operator""mm(unsigned long long l) { return quantity<millimetre, std::int64_t>(l); }
    constexpr auto operator""mm(long double l) { return quantity<millimetre, long double>(l); }

    // cm
    constexpr auto operator""cm(unsigned long long l) { return quantity<centimetre, std::int64_t>(l); }
    constexpr auto operator""cm(long double l) { return quantity<centimetre, long double>(l); }

    // km
    constexpr auto operator""km(unsigned long long l) { return quantity<kilometre, std::int64_t>(l); }
    constexpr auto operator""km(long double l) { return quantity<kilometre, long double>(l); }

  } // namespace literals

  // US customary units
  struct yard : derived_unit<yard, "yd", length, ratio<9'144, 10'000>> {};
  struct foot : derived_unit<foot, "ft", length, ratio_multiply<ratio<1, 3>, yard::ratio>> {};
  struct inch : derived_unit<inch, "in", length, ratio_multiply<ratio<1, 12>, foot::ratio>> {};
  struct mile : derived_unit<mile, "mi", length, ratio_multiply<ratio<1'760>, yard::ratio>> {};

  inline namespace literals {

    // yd
    constexpr auto operator""yd(unsigned long long l) { return quantity<yard, std::int64_t>(l); }
    constexpr auto operator""yd(long double l) { return quantity<yard, long double>(l); }

    // ft
    constexpr auto operator""ft(unsigned long long l) { return quantity<foot, std::int64_t>(l); }
    constexpr auto operator""ft(long double l) { return quantity<foot, long double>(l); }

    // in
    constexpr auto operator""in(unsigned long long l) { return quantity<inch, std::int64_t>(l); }
    constexpr auto operator""in(long double l) { return quantity<inch, long double>(l); }

    // mi
    constexpr auto operator""mi(unsigned long long l) { return quantity<mile, std::int64_t>(l); }
    constexpr auto operator""mi(long double l) { return quantity<mile, long double>(l); }

  }  // namespace literals

}  // namespace units
