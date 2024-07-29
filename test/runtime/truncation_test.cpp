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

#include "almost_equals.h"
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <mp-units/ext/format.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <limits>
#include <numbers>
#endif
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/ostream.h>  // IWYU pragma: keep
#include <mp-units/systems/angular.h>
#endif

using namespace mp_units;
using namespace mp_units::angular;
using namespace mp_units::angular::unit_symbols;

inline constexpr struct half_revolution final : named_unit<"hrev", mag_pi * radian> {
} half_revolution;
inline constexpr auto hrev = half_revolution;

TEST_CASE("value_cast should not truncate for valid inputs", "[value_cast]")
{
  SECTION("num > den > 1, irr = 1")
  {
    REQUIRE_THAT(value_cast<grad>(9 * deg), AlmostEquals(10 * grad));
    REQUIRE_THAT(value_cast<grad>(360 * deg), AlmostEquals(400 * grad));
  }

  SECTION("1 < num < den, irr = 1")
  {
    REQUIRE_THAT(value_cast<deg>(10 * grad), AlmostEquals(9 * deg));
    REQUIRE_THAT(value_cast<deg>(400 * grad), AlmostEquals(360 * deg));
  }


  SECTION("num > den = 1, irr > 1")
  {
    REQUIRE_THAT(value_cast<rad>(1 * rev), AlmostEquals(6 * rad));
    REQUIRE_THAT(value_cast<rad>(5 * rev), AlmostEquals(31 * rad));
    REQUIRE_THAT(value_cast<rad>(10 * rev), AlmostEquals(63 * rad));
    REQUIRE_THAT(value_cast<rad>(20 * rev), AlmostEquals(126 * rad));
  }

  SECTION("1 = num < den, irr < 1")
  {
    REQUIRE_THAT(value_cast<rev>(6 * rad), AlmostEquals(1 * rev));
    REQUIRE_THAT(value_cast<rev>(31 * rad), AlmostEquals(5 * rev));
    REQUIRE_THAT(value_cast<rev>(63 * rad), AlmostEquals(10 * rev));
    REQUIRE_THAT(value_cast<rev>(126 * rad), AlmostEquals(20 * rev));
  }

  SECTION("rational = 1, irrational > 1")
  {
    REQUIRE_THAT(value_cast<rad>(1 * hrev), AlmostEquals(3 * rad));
    REQUIRE_THAT(value_cast<rad>(10 * hrev), AlmostEquals(31 * rad));
    REQUIRE_THAT(value_cast<rad>(20 * hrev), AlmostEquals(63 * rad));
    REQUIRE_THAT(value_cast<rad>(40 * hrev), AlmostEquals(126 * rad));
  }

  SECTION("rational = 1, irrational < 1")
  {
    REQUIRE_THAT(value_cast<hrev>(3 * rad), AlmostEquals(1 * hrev));
    REQUIRE_THAT(value_cast<hrev>(31 * rad), AlmostEquals(10 * hrev));
    REQUIRE_THAT(value_cast<hrev>(63 * rad), AlmostEquals(20 * hrev));
    REQUIRE_THAT(value_cast<hrev>(126 * rad), AlmostEquals(40 * hrev));
  }
}


inline constexpr std::int64_t one_64bit = 1;
inline constexpr struct one_in_2to50 : named_unit<"oi2t50", mag_ratio<(one_64bit << 50) + 1, (one_64bit << 50)> * one> {
} one_in_2to50;
inline constexpr auto oi2t50 = one_in_2to50;

inline constexpr struct one_in_2to60 : named_unit<"oi2t60", mag_ratio<(one_64bit << 60) + 1, (one_64bit << 60)> * one> {
} one_in_2to60;
inline constexpr auto oi2t60 = one_in_2to60;


TEMPLATE_TEST_CASE("value_cast should not overflow internally for valid inputs", "[value_cast]", std::int8_t,
                   std::uint8_t, std::int16_t, std::uint16_t, std::int32_t, std::uint32_t, std::int64_t, std::uint64_t)
{
  // max()/20: small enough so that none of the tested factors are likely to cause overflow, but still be nonzero;
  // the "easy" test to verify the test itself is good.
  std::vector<TestType> test_values = {std::numeric_limits<TestType>::max() / 20,
                                       std::numeric_limits<TestType>::max() - 1};
  if (std::is_signed_v<TestType>) {
    test_values.push_back(std::numeric_limits<TestType>::min() + 1);
  }

  SECTION("grad <-> deg")
  {
    for (TestType tv : test_values) {
      // non-overflowing computation for b = 360/400 * a = (10 - 1)/10 * a = (1 - 1/10) * a = 1 - a/10
      auto deg_number = tv - tv / 10;
      // non-overflowing computation for b = 400/360 * a = (9 + 1)/9 * a = (1 + 1/9) * a = 1 + a/9
      auto grad_number = deg_number + deg_number / 9;
      INFO(MP_UNITS_STD_FMT::format("{} deg ~ {} grad", deg_number, grad_number));
      REQUIRE_THAT(value_cast<grad>(deg_number * deg), AlmostEquals(grad_number * grad));
      REQUIRE_THAT(value_cast<deg>(grad_number * grad), AlmostEquals(deg_number * deg));
    }
  }

  if constexpr (std::numeric_limits<TestType>::digits >= 60) {
    // ---- a couple of pathological conversion factors

    // this one can still be correctly calculated using a double-precision calculation
    SECTION("one <-> (1 + 2^-50) * one")
    {
      for (TestType tv : test_values) {
        auto n1 = tv - tv >> 50;
        auto n2 = n1 + n1 / ((one_64bit << 50) - 1);
        INFO(MP_UNITS_STD_FMT::format("{} (1 + 2^-50) * one ~ {} one", n1, n2));
        REQUIRE_THAT(value_cast<one>(n1 * oi2t50), AlmostEquals(n2 * one));
        REQUIRE_THAT(value_cast<oi2t50>(n2 * one), AlmostEquals(n1 * oi2t50));
      }
    }

    // this one cannot be correctly calculated in double-precision
    SECTION("one <-> (1 + 2^-60) * one")
    {
      for (TestType tv : test_values) {
        auto n1 = tv - tv >> 60;
        auto n2 = n1 + n1 / ((one_64bit << 60) - 1);
        INFO(MP_UNITS_STD_FMT::format("{} (1 + 2^-60) * one ~ {} one", n1, n2));
        REQUIRE_THAT(value_cast<one>(n1 * oi2t60), AlmostEquals(n2 * one));
        REQUIRE_THAT(value_cast<oi2t60>(n2 * one), AlmostEquals(n1 * oi2t60));
      }
    }
  } else {
    // skipping this oen for the 64 bit types; we don't know how to calculate the expected results to 64 bits
    // precision...
    SECTION("rad <-> rev")
    {
      for (TestType tv : test_values) {
        auto rev_number = static_cast<TestType>(std::trunc(0.5 * std::numbers::inv_pi * tv));
        auto rad_number = static_cast<TestType>(std::round(2 * std::numbers::pi * rev_number));
        INFO(MP_UNITS_STD_FMT::format("{} rev ~ {} rad", rev_number, rad_number));
        REQUIRE_THAT(value_cast<rad>(rev_number * rev), AlmostEquals(rad_number * rad));
        REQUIRE_THAT(value_cast<rev>(rad_number * rad), AlmostEquals(rev_number * rev));
      }
    }
  }
}
