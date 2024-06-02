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
#include <limits>
#include <numbers>
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/ostream.h>  // IWYU pragma: keep
#include <mp-units/systems/angular.h>
#endif

using namespace mp_units;
using namespace mp_units::angular;
using namespace mp_units::angular::unit_symbols;

inline constexpr struct half_revolution : named_unit<"hrev", mag_pi * radian> {
} half_revolution;
inline constexpr auto hrev = half_revolution;

// constexpr auto revb6 = mag_ratio<1,3> * mag_pi * rad;

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


TEMPLATE_TEST_CASE("value_cast should not overflow internally for valid inputs", "[value_cast]", std::int8_t,
                   std::uint8_t, std::int16_t, std::uint16_t, std::int32_t, std::uint32_t)
{
  // max()/20: small enough so that none of the tested factors are likely to cause overflow, but still be nonzero;
  // the "easy" test to verify the test itself is good.
  std::vector<TestType> test_values = {std::numeric_limits<TestType>::max() / 20,
                                       std::numeric_limits<TestType>::max() - 1};
  if (std::is_signed_v<TestType>) {
    test_values.push_back(std::numeric_limits<TestType>::min() + 1);
  }

  for (TestType tv : test_values) {
    SECTION("grad <-> deg")
    {
      auto deg_number = static_cast<TestType>(std::trunc(0.9 * tv));
      auto grad_number = static_cast<TestType>(std::round(deg_number / 0.9));
      INFO(MP_UNITS_STD_FMT::format("{} deg ~ {} grad", deg_number, grad_number));
      REQUIRE_THAT(value_cast<grad>(deg_number * deg), AlmostEquals(grad_number * grad));
      REQUIRE_THAT(value_cast<deg>(grad_number * grad), AlmostEquals(deg_number * deg));
    }


    SECTION("rad <-> rev")
    {
      auto rev_number = static_cast<TestType>(std::trunc(0.5 * std::numbers::inv_pi * tv));
      auto rad_number = static_cast<TestType>(std::round(2 * std::numbers::pi * rev_number));
      INFO(MP_UNITS_STD_FMT::format("{} rev ~ {} rad", rev_number, rad_number));
      REQUIRE_THAT(value_cast<rad>(rev_number * rev), AlmostEquals(rad_number * rad));
      REQUIRE_THAT(value_cast<rev>(rad_number * rad), AlmostEquals(rev_number * rev));
    }
  }
}
