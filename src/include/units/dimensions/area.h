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

#include <units/dimensions/length.h>
#include <units/bits/fmt.h>

namespace units {

  struct area : derived_dimension<area, exp<base_dim_length, 2>> {};

  template<typename T>
  concept Area = QuantityOf<T, area>;

  struct square_metre : coherent_derived_unit<square_metre, "m^2", area> {};
  struct square_millimetre : deduced_derived_unit<square_millimetre, "mm^2", area, millimetre> {};
  struct square_centimetre : deduced_derived_unit<square_centimetre, "cm^2", area, centimetre> {};
  struct square_kilometre : deduced_derived_unit<square_kilometre, "km^2", area, kilometre> {};
  struct square_foot : deduced_derived_unit<square_foot, "ft^2", area, foot> {};

  inline namespace literals {

    // sq_m
    constexpr auto operator""sq_m(unsigned long long l) { return quantity<square_metre, std::int64_t>(l); }
    constexpr auto operator""sq_m(long double l) { return quantity<square_metre, long double>(l); }

    // sq_mm
    constexpr auto operator""sq_mm(unsigned long long l) { return quantity<square_millimetre, std::int64_t>(l); }
    constexpr auto operator""sq_mm(long double l) { return quantity<square_millimetre, long double>(l); }

    // sq_cm
    constexpr auto operator""sq_cm(unsigned long long l) { return quantity<square_centimetre, std::int64_t>(l); }
    constexpr auto operator""sq_cm(long double l) { return quantity<square_centimetre, long double>(l); }

    // sq_km
    constexpr auto operator""sq_km(unsigned long long l) { return quantity<square_kilometre, std::int64_t>(l); }
    constexpr auto operator""sq_km(long double l) { return quantity<square_kilometre, long double>(l); }

    // sq_ft
    constexpr auto operator""sq_ft(unsigned long long l) { return quantity<square_foot, std::int64_t>(l); }
    constexpr auto operator""sq_ft(long double l) { return quantity<square_foot, long double>(l); }

  }  // namespace literals


  namespace details {

    template<>
    inline icu::MeasureUnit* create_icu_unit<square_metre>(UErrorCode& uc)
    {
      return icu::MeasureUnit::createSquareMeter(uc);
    }

    template<>
    inline icu::MeasureUnit* create_icu_unit<square_centimetre>(UErrorCode& uc)
    {
      return icu::MeasureUnit::createSquareCentimeter(uc);
    }

    template<>
    inline icu::MeasureUnit* create_icu_unit<square_kilometre>(UErrorCode& uc)
    {
      return icu::MeasureUnit::createSquareKilometer(uc);
    }

    template<>
    inline icu::MeasureUnit* create_icu_unit<square_foot>(UErrorCode& uc)
    {
      return icu::MeasureUnit::createSquareFoot(uc);
    }

  }  // namespace details

}  // namespace units