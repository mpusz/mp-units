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
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <limits>
#endif
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/math.h>
#include <mp-units/systems/angular.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si.h>
#endif

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

inline constexpr struct mean_sea_level final : mp_units::absolute_point_origin<mp_units::isq::altitude> {
} mean_sea_level;

// classical

TEST_CASE("math operations", "[math]")
{
  SECTION("'pow<N>()' on quantity changes the value and the dimension accordingly")
  {
    SECTION("'pow<0>(q)' returns '1'") { CHECK(pow<0>(2 * isq::length[m]) == 1 * one); }

    SECTION("'pow<1>(q)' returns 'q'") { CHECK(pow<1>(2 * isq::length[m]) == 2 * isq::length[m]); }

    SECTION("'pow<2>(q)' squares both the value and a dimension")
    {
      CHECK(pow<2>(2 * isq::length[m]) == 4 * isq::area[m2]);
    }

    SECTION("'pow<3>(q)' cubes both the value and a dimension")
    {
      CHECK(pow<3>(2 * isq::length[m]) == 8 * isq::volume[m3]);
    }
  }

  SECTION("'sqrt()' on quantity changes the value and the dimension accordingly")
  {
    REQUIRE(sqrt(4 * isq::area[m2]) == 2 * isq::length[m]);
  }

  SECTION("'cbrt()' on quantity changes the value and the dimension accordingly")
  {
    REQUIRE(cbrt(8 * isq::volume[m3]) == 2 * isq::length[m]);
  }

  SECTION("'fma()' on quantity changes the value and the dimension accordingly")
  {
    REQUIRE(fma(1.0 * isq::length[m], 2.0 * one, 2.0 * isq::length[m]) == 4.0 * isq::length[m]);
    REQUIRE(fma(isq::speed(10.0 * m / s), isq::time(2.0 * s), isq::height(42.0 * m)) == isq::length(62.0 * m));
  }

  SECTION("fmod functions")
  {
    SECTION("fmod should work on the same quantities")
    {
      REQUIRE(fmod(4. * isq::length[km], 3. * isq::length[km]) == 1. * isq::length[km]);
      REQUIRE(fmod(-9. * isq::length[km], 3. * isq::length[km]) == -0. * isq::length[km]);
      REQUIRE(fmod(3 * isq::length[km], 2 * isq::length[km]) == 1 * isq::length[km]);
      REQUIRE(fmod(4 * isq::length[km], 2.5f * isq::length[km]) == 1.5 * isq::length[km]);
    }
    SECTION("fmod should work with different units of the same dimension")
    {
      REQUIRE(fmod(4. * isq::length[km], 3000. * isq::length[m]) == 1000. * isq::length[m]);
      REQUIRE(fmod(-9. * isq::length[km], 3000. * isq::length[m]) == -0. * isq::length[m]);
      REQUIRE(fmod(3. * isq::length[km], 2000. * isq::length[m]) == 1000 * isq::length[m]);
      REQUIRE(fmod(4 * isq::length[km], 2500 * isq::length[m]) == 1500 * isq::length[m]);
    }
  }

  SECTION("remainder functions")
  {
    SECTION("remainder should work on the same quantities")
    {
      REQUIRE(remainder(4. * isq::length[km], 3. * isq::length[km]) == 1. * isq::length[km]);
      REQUIRE(remainder(-9. * isq::length[km], 3. * isq::length[km]) == -0. * isq::length[km]);
      REQUIRE(remainder(3 * isq::length[km], 2 * isq::length[km]) == -1 * isq::length[km]);
      REQUIRE(remainder(4 * isq::length[km], 2.75f * isq::length[km]) == 1.25 * isq::length[km]);
    }
    SECTION("remainder should work with different units of the same dimension")
    {
      REQUIRE(remainder(4. * isq::length[km], 3000. * isq::length[m]) == 1000. * isq::length[m]);
      REQUIRE(remainder(-9. * isq::length[km], 3000. * isq::length[m]) == -0. * isq::length[m]);
      REQUIRE(remainder(3. * isq::length[km], 2000. * isq::length[m]) == -1000 * isq::length[m]);
      REQUIRE(remainder(4 * isq::length[km], 2750 * isq::length[m]) == 1250 * isq::length[m]);
    }
  }

  SECTION("'isfinite()' accepts dimensioned arguments") { REQUIRE(isfinite(4.0 * isq::length[m])); }

  SECTION("'isinf()' accepts dimensioned arguments") { REQUIRE(!isinf(4.0 * isq::length[m])); }

  SECTION("'isnan()' accepts dimensioned arguments") { REQUIRE(!isnan(4.0 * isq::length[m])); }


  SECTION("'pow<Num, Den>()' on quantity changes the value and the dimension accordingly")
  {
    REQUIRE(pow<1, 4>(16 * isq::area[m2]) == sqrt(4 * isq::length[m]));
  }

  // TODO add tests for exp()

  SECTION("absolute functions on quantity returns the absolute value")
  {
    SECTION("'abs()' on a negative quantity returns the abs")
    {
      SECTION("integral representation") { REQUIRE(abs(-1 * isq::length[m]) == 1 * isq::length[m]); }

      SECTION("floating-point representation") { REQUIRE(abs(-1. * isq::length[m]) == 1 * isq::length[m]); }
    }

    SECTION("'abs()' on a positive quantity returns the abs")
    {
      SECTION("integral representation") { REQUIRE(abs(1 * isq::length[m]) == 1 * isq::length[m]); }

      SECTION("floating-point representation") { REQUIRE(abs(1. * isq::length[m]) == 1 * isq::length[m]); }
    }
  }

  SECTION("floor functions")
  {
    SECTION("floor 1 second with target unit second should be 1 second")
    {
      REQUIRE(floor<si::second>(1 * isq::time[s]) == 1 * isq::time[s]);
    }
    SECTION("floor 1000 milliseconds with target unit second should be 1 second")
    {
      REQUIRE(floor<si::second>(1000 * isq::time[ms]) == 1 * isq::time[s]);
    }
    SECTION("floor 1001 milliseconds with target unit second should be 1 second")
    {
      REQUIRE(floor<si::second>(1001 * isq::time[ms]) == 1 * isq::time[s]);
    }
    SECTION("floor 1999 milliseconds with target unit second should be 1 second")
    {
      REQUIRE(floor<si::second>(1999 * isq::time[ms]) == 1 * isq::time[s]);
    }
    SECTION("floor -1000 milliseconds with target unit second should be -1 second")
    {
      REQUIRE(floor<si::second>(-1000 * isq::time[ms]) == -1 * isq::time[s]);
    }
    SECTION("floor -999 milliseconds with target unit second should be -1 second")
    {
      REQUIRE(floor<si::second>(-999 * isq::time[ms]) == -1 * isq::time[s]);
    }
    SECTION("floor 1.3 seconds with target unit second should be 1 second")
    {
      REQUIRE(floor<si::second>(1.3 * isq::time[s]) == 1 * isq::time[s]);
    }
    SECTION("floor -1.3 seconds with target unit second should be -2 seconds")
    {
      REQUIRE(floor<si::second>(-1.3 * isq::time[s]) == -2 * isq::time[s]);
    }
    SECTION("floor 1001. milliseconds with target unit second should be 1 second")
    {
      REQUIRE(floor<si::second>(1001. * isq::time[ms]) == 1 * isq::time[s]);
    }
    SECTION("floor 1999. milliseconds with target unit second should be 1 second")
    {
      REQUIRE(floor<si::second>(1999. * isq::time[ms]) == 1 * isq::time[s]);
    }
    SECTION("floor -1000. milliseconds with target unit second should be -1 second")
    {
      REQUIRE(floor<si::second>(-1000. * isq::time[ms]) == -1 * isq::time[s]);
    }
    SECTION("floor -999. milliseconds with target unit second should be -1 second")
    {
      REQUIRE(floor<si::second>(-999. * isq::time[ms]) == -1 * isq::time[s]);
    }

    // TODO Add tests for `N`, `kN` and `kg * m / s2` i `kg * km / s2`
  }

  SECTION("ceil functions")
  {
    SECTION("ceil 1 second with target unit second should be 1 second")
    {
      REQUIRE(ceil<si::second>(1 * isq::time[s]) == 1 * isq::time[s]);
    }
    SECTION("ceil 1000 milliseconds with target unit second should be 1 second")
    {
      REQUIRE(ceil<si::second>(1000 * isq::time[ms]) == 1 * isq::time[s]);
    }
    SECTION("ceil 1001 milliseconds with target unit second should be 2 seconds")
    {
      REQUIRE(ceil<si::second>(1001 * isq::time[ms]) == 2 * isq::time[s]);
    }
    SECTION("ceil 1999 milliseconds with target unit second should be 2 seconds")
    {
      REQUIRE(ceil<si::second>(1999 * isq::time[ms]) == 2 * isq::time[s]);
    }
    SECTION("ceil -1000 milliseconds with target unit second should be -1 second")
    {
      REQUIRE(ceil<si::second>(-1000 * isq::time[ms]) == -1 * isq::time[s]);
    }
    SECTION("ceil -999 milliseconds with target unit second should be 0 seconds")
    {
      REQUIRE(ceil<si::second>(-999 * isq::time[ms]) == 0 * isq::time[s]);
    }
    SECTION("ceil 1.3 seconds with target unit second should be 2 seconds")
    {
      REQUIRE(ceil<si::second>(1.3 * isq::time[s]) == 2 * isq::time[s]);
    }
    SECTION("ceil -1.3 seconds with target unit second should be -1 second")
    {
      REQUIRE(ceil<si::second>(-1.3 * isq::time[s]) == -1 * isq::time[s]);
    }
    SECTION("ceil 1001. milliseconds with target unit second should be 2 seconds")
    {
      REQUIRE(ceil<si::second>(1001. * isq::time[ms]) == 2 * isq::time[s]);
    }
    SECTION("ceil 1999. milliseconds with target unit second should be 2 seconds")
    {
      REQUIRE(ceil<si::second>(1999. * isq::time[ms]) == 2 * isq::time[s]);
    }
    SECTION("ceil -1000. milliseconds with target unit second should be -1 second")
    {
      REQUIRE(ceil<si::second>(-1000. * isq::time[ms]) == -1 * isq::time[s]);
    }
    SECTION("ceil -999. milliseconds with target unit second should be 0 seconds")
    {
      REQUIRE(ceil<si::second>(-999. * isq::time[ms]) == 0 * isq::time[s]);
    }
  }

  SECTION("round functions")
  {
    SECTION("round 1 second with target unit second should be 1 second")
    {
      REQUIRE(round<si::second>(1 * isq::time[s]) == 1 * isq::time[s]);
    }
    SECTION("round 1000 milliseconds with target unit second should be 1 second")
    {
      REQUIRE(round<si::second>(1000 * isq::time[ms]) == 1 * isq::time[s]);
    }
    SECTION("round 1001 milliseconds with target unit second should be 1 second")
    {
      REQUIRE(round<si::second>(1001 * isq::time[ms]) == 1 * isq::time[s]);
    }
    SECTION("round 1499 milliseconds with target unit second should be 1 second")
    {
      REQUIRE(round<si::second>(1499 * isq::time[ms]) == 1 * isq::time[s]);
    }
    SECTION("round 1500 milliseconds with target unit second should be 2 seconds")
    {
      REQUIRE(round<si::second>(1500 * isq::time[ms]) == 2 * isq::time[s]);
    }
    SECTION("round 1999 milliseconds with target unit second should be 2 seconds")
    {
      REQUIRE(round<si::second>(1999 * isq::time[ms]) == 2 * isq::time[s]);
    }
    SECTION("round -1000 milliseconds with target unit second should be -1 second")
    {
      REQUIRE(round<si::second>(-1000 * isq::time[ms]) == -1 * isq::time[s]);
    }
    SECTION("round -1001 milliseconds with target unit second should be -1 second")
    {
      REQUIRE(round<si::second>(-1001 * isq::time[ms]) == -1 * isq::time[s]);
    }
    SECTION("round -1499 milliseconds with target unit second should be -1 second")
    {
      REQUIRE(round<si::second>(-1499 * isq::time[ms]) == -1 * isq::time[s]);
    }
    SECTION("round -1500 milliseconds with target unit second should be -2 seconds")
    {
      REQUIRE(round<si::second>(-1500 * isq::time[ms]) == -2 * isq::time[s]);
    }
    SECTION("round -1999 milliseconds with target unit second should be -2 seconds")
    {
      REQUIRE(round<si::second>(-1999 * isq::time[ms]) == -2 * isq::time[s]);
    }
    SECTION("round 1000. milliseconds with target unit second should be 1 second")
    {
      REQUIRE(round<si::second>(1000. * isq::time[ms]) == 1 * isq::time[s]);
    }
    SECTION("round 1001. milliseconds with target unit second should be 1 second")
    {
      REQUIRE(round<si::second>(1001. * isq::time[ms]) == 1 * isq::time[s]);
    }
    SECTION("round 1499. milliseconds with target unit second should be 1 second")
    {
      REQUIRE(round<si::second>(1499. * isq::time[ms]) == 1 * isq::time[s]);
    }
    SECTION("round 1500. milliseconds with target unit second should be 2 seconds")
    {
      REQUIRE(round<si::second>(1500. * isq::time[ms]) == 2 * isq::time[s]);
    }
    SECTION("round 1999. milliseconds with target unit second should be 2 seconds")
    {
      REQUIRE(round<si::second>(1999. * isq::time[ms]) == 2 * isq::time[s]);
    }
    SECTION("round -1000. milliseconds with target unit second should be -1 second")
    {
      REQUIRE(round<si::second>(-1000. * isq::time[ms]) == -1 * isq::time[s]);
    }
    SECTION("round -1001. milliseconds with target unit second should be -1 second")
    {
      REQUIRE(round<si::second>(-1001. * isq::time[ms]) == -1 * isq::time[s]);
    }
    SECTION("round -1499. milliseconds with target unit second should be -1 second")
    {
      REQUIRE(round<si::second>(-1499. * isq::time[ms]) == -1 * isq::time[s]);
    }
    SECTION("round -1500. milliseconds with target unit second should be -2 seconds")
    {
      REQUIRE(round<si::second>(-1500. * isq::time[ms]) == -2 * isq::time[s]);
    }
    SECTION("round -1999. milliseconds with target unit second should be -2 seconds")
    {
      REQUIRE(round<si::second>(-1999. * isq::time[ms]) == -2 * isq::time[s]);
    }
  }

  SECTION("hypot functions")
  {
    SECTION("hypot should work on the same quantities")
    {
      REQUIRE(hypot(3. * isq::length[km], 4. * isq::length[km]) == 5. * isq::length[km]);
      REQUIRE(hypot(2. * isq::length[km], 3. * isq::length[km], 6. * isq::length[km]) == 7. * isq::length[km]);
    }
    SECTION("hypot should work with different units of the same dimension")
    {
      REQUIRE(hypot(3. * isq::length[km], 4000. * isq::length[m]) == 5. * isq::length[km]);
      REQUIRE(hypot(2. * isq::length[km], 3000. * isq::length[m], 6. * isq::length[km]) == 7. * isq::length[km]);
    }
  }

  SECTION("lerp functions")
  {
    SECTION("lerp should work on the same quantity points")
    {
      SECTION("default origins")
      {
        REQUIRE(lerp(point<isq::altitude[m]>(99.), point<isq::altitude[m]>(100.), 0.0) == point<isq::altitude[m]>(99.));
        REQUIRE(lerp(point<isq::altitude[m]>(99.), point<isq::altitude[m]>(100.), 0.5) ==
                point<isq::altitude[m]>(99.5));
        REQUIRE(lerp(point<isq::altitude[m]>(99.), point<isq::altitude[m]>(100.), 1.0) ==
                point<isq::altitude[m]>(100.));
        REQUIRE(lerp(point<isq::altitude[m]>(99.), point<isq::altitude[m]>(100.), 2.0) ==
                point<isq::altitude[m]>(101.));
      }

      SECTION("custom origins")
      {
        REQUIRE(lerp(mean_sea_level + isq::height(99. * m), mean_sea_level + isq::height(100. * m), 0.0) ==
                mean_sea_level + isq::height(99. * m));
        REQUIRE(lerp(mean_sea_level + isq::height(99. * m), mean_sea_level + isq::height(100. * m), 0.5) ==
                mean_sea_level + isq::height(99.5 * m));
        REQUIRE(lerp(mean_sea_level + isq::height(99. * m), mean_sea_level + isq::height(100. * m), 1.0) ==
                mean_sea_level + isq::height(100. * m));
        REQUIRE(lerp(mean_sea_level + isq::height(99. * m), mean_sea_level + isq::height(100. * m), 2.0) ==
                mean_sea_level + isq::height(101. * m));
      }
    }

    SECTION("lerp should work with different units of the same dimension")
    {
      SECTION("default origins")
      {
        REQUIRE(lerp(point<isq::altitude[m]>(99.), point<isq::altitude[cm]>(10'000.), 0.0) ==
                point<isq::altitude[m]>(99.));
        REQUIRE(lerp(point<isq::altitude[m]>(99.), point<isq::altitude[cm]>(10'000.), 0.5) ==
                point<isq::altitude[m]>(99.5));
        REQUIRE(lerp(point<isq::altitude[m]>(99.), point<isq::altitude[cm]>(10'000.), 1.0) ==
                point<isq::altitude[m]>(100.));
        REQUIRE(lerp(point<isq::altitude[m]>(99.), point<isq::altitude[cm]>(10'000.), 2.0) ==
                point<isq::altitude[m]>(101.));
      }

      SECTION("custom origins")
      {
        REQUIRE(lerp(mean_sea_level + isq::height(99. * m), mean_sea_level + isq::height(10'000. * cm), 0.0) ==
                mean_sea_level + isq::height(99. * m));
        REQUIRE(lerp(mean_sea_level + isq::height(99. * m), mean_sea_level + isq::height(10'000. * cm), 0.5) ==
                mean_sea_level + isq::height(99.5 * m));
        REQUIRE(lerp(mean_sea_level + isq::height(99. * m), mean_sea_level + isq::height(10'000. * cm), 1.0) ==
                mean_sea_level + isq::height(100. * m));
        REQUIRE(lerp(mean_sea_level + isq::height(99. * m), mean_sea_level + isq::height(10'000. * cm), 2.0) ==
                mean_sea_level + isq::height(101. * m));
      }
    }
  }

  SECTION("midpoint functions")
  {
    SECTION("midpoint should work on the same quantity points")
    {
      REQUIRE(midpoint(point<isq::altitude[m]>(99.), point<isq::altitude[m]>(100.)) == point<isq::altitude[m]>(99.5));
      REQUIRE(midpoint(mean_sea_level + isq::height(99. * m), mean_sea_level + isq::height(100. * m)) ==
              mean_sea_level + isq::height(99.5 * m));
    }

    SECTION("midpoint should work with different units of the same dimension")
    {
      REQUIRE(midpoint(point<isq::altitude[m]>(99.), point<isq::altitude[cm]>(10'000.)) ==
              point<isq::altitude[m]>(99.5));
      REQUIRE(midpoint(mean_sea_level + isq::height(99. * m), mean_sea_level + isq::height(10'000. * cm)) ==
              mean_sea_level + isq::height(99.5 * m));
    }
  }

  SECTION("SI trigonometric functions")
  {
    SECTION("sin")
    {
      REQUIRE_THAT(si::sin(0 * deg), AlmostEquals(0. * one));
      REQUIRE_THAT(si::sin(90 * deg), AlmostEquals(1. * one));
      REQUIRE_THAT(si::sin(180 * deg), AlmostEquals(0. * one));
      REQUIRE_THAT(si::sin(270 * deg), AlmostEquals(-1. * one));
    }

    SECTION("cos")
    {
      REQUIRE_THAT(si::cos(0 * deg), AlmostEquals(1. * one));
      REQUIRE_THAT(si::cos(90 * deg), AlmostEquals(0. * one));
      REQUIRE_THAT(si::cos(180 * deg), AlmostEquals(-1. * one));
      REQUIRE_THAT(si::cos(270 * deg), AlmostEquals(0. * one));
    }

    SECTION("tan")
    {
      REQUIRE_THAT(si::tan(0 * deg), AlmostEquals(0. * one));
      REQUIRE_THAT(si::tan(45. * deg), AlmostEquals(1. * one));
      REQUIRE_THAT(si::tan(135. * deg), AlmostEquals(-1. * one));
      REQUIRE_THAT(si::tan(180. * deg), AlmostEquals(0. * one));
    }
  }

  SECTION("SI inverse trigonometric functions")
  {
    SECTION("asin")
    {
      REQUIRE_THAT(si::asin(-1 * one), AlmostEquals(-90. * deg));
      REQUIRE_THAT(si::asin(0 * one), AlmostEquals(0. * deg));
      REQUIRE_THAT(si::asin(1 * one), AlmostEquals(90. * deg));
    }

    SECTION("acos")
    {
      REQUIRE_THAT(si::asin(-1 * one), AlmostEquals(-90. * deg));
      REQUIRE_THAT(si::asin(0 * one), AlmostEquals(0. * deg));
      REQUIRE_THAT(si::asin(1 * one), AlmostEquals(90. * deg));
    }

    SECTION("atan")
    {
      REQUIRE_THAT(si::atan(-1 * one), AlmostEquals(-45. * deg));
      REQUIRE_THAT(si::atan(0 * one), AlmostEquals(0. * deg));
      REQUIRE_THAT(si::atan(1 * one), AlmostEquals(45. * deg));
    }
  }

  SECTION("SI atan2 functions")
  {
    SECTION("atan2 should work on the same quantities")
    {
      REQUIRE_THAT(si::atan2(-1. * isq::length[km], 1. * isq::length[km]), AlmostEquals(-45. * deg));
      REQUIRE_THAT(si::atan2(0. * isq::length[km], 1. * isq::length[km]), AlmostEquals(0. * deg));
      REQUIRE_THAT(si::atan2(1. * isq::length[km], 1. * isq::length[km]), AlmostEquals(45. * deg));
    }
    SECTION("atan2 should work with different units of the same dimension")
    {
      REQUIRE_THAT(si::atan2(-1. * isq::length[km], 1000. * isq::length[m]), AlmostEquals(-45. * deg));
      REQUIRE_THAT(si::atan2(0. * isq::length[km], 1000. * isq::length[m]), AlmostEquals(0. * deg));
      REQUIRE_THAT(si::atan2(1. * isq::length[km], 1000. * isq::length[m]), AlmostEquals(45. * deg));
    }
  }

  SECTION("Angle trigonometric functions")
  {
    using namespace mp_units::angular;
    using namespace mp_units::angular::unit_symbols;
    using mp_units::angular::unit_symbols::deg;

    SECTION("sin")
    {
      REQUIRE_THAT(sin(0 * angle[deg]), AlmostEquals(0. * one));
      REQUIRE_THAT(sin(90 * angle[deg]), AlmostEquals(1. * one));
      REQUIRE_THAT(sin(180 * angle[deg]), AlmostEquals(0. * one));
      REQUIRE_THAT(sin(270 * angle[deg]), AlmostEquals(-1. * one));

      REQUIRE_THAT(sin(0 * angle[grad]), AlmostEquals(0. * one));
      REQUIRE_THAT(sin(100 * angle[grad]), AlmostEquals(1. * one));
      REQUIRE_THAT(sin(200 * angle[grad]), AlmostEquals(0. * one, 2));
      REQUIRE_THAT(sin(300 * angle[grad]), AlmostEquals(-1. * one));
    }

    SECTION("cos")
    {
      REQUIRE_THAT(cos(0 * angle[deg]), AlmostEquals(1. * one));
      REQUIRE_THAT(cos(90 * angle[deg]), AlmostEquals(0. * one));
      REQUIRE_THAT(cos(180 * angle[deg]), AlmostEquals(-1. * one));
      REQUIRE_THAT(cos(270 * angle[deg]), AlmostEquals(0. * one));

      REQUIRE_THAT(cos(0 * angle[grad]), AlmostEquals(1. * one));
      REQUIRE_THAT(cos(100 * angle[grad]), AlmostEquals(0. * one));
      REQUIRE_THAT(cos(200 * angle[grad]), AlmostEquals(-1. * one));
      REQUIRE_THAT(cos(300 * angle[grad]), AlmostEquals(0. * one));
    }

    SECTION("tan")
    {
      REQUIRE_THAT(tan(0 * angle[deg]), AlmostEquals(0. * one));
      REQUIRE_THAT(tan(45 * angle[deg]), AlmostEquals(1. * one));
      REQUIRE_THAT(tan(135 * angle[deg]), AlmostEquals(-1. * one));
      REQUIRE_THAT(tan(180 * angle[deg]), AlmostEquals(0. * one));

      REQUIRE_THAT(tan(0 * angle[grad]), AlmostEquals(0. * one));
      REQUIRE_THAT(tan(50 * angle[grad]), AlmostEquals(1. * one));
      REQUIRE_THAT(tan(150 * angle[grad]), AlmostEquals(-1. * one));
      REQUIRE_THAT(tan(200 * angle[grad]), AlmostEquals(0. * one, 2));
    }
  }

  SECTION("Angle inverse trigonometric functions")
  {
    using namespace mp_units::angular;
    using namespace mp_units::angular::unit_symbols;
    using mp_units::angular::unit_symbols::deg;

    SECTION("asin")
    {
      REQUIRE_THAT(asin(-1 * one), AlmostEquals(-90. * angle[deg]));
      REQUIRE_THAT(asin(0 * one), AlmostEquals(0. * angle[deg]));
      REQUIRE_THAT(asin(1 * one), AlmostEquals(90. * angle[deg]));
    }

    SECTION("acos")
    {
      REQUIRE_THAT(asin(-1 * one), AlmostEquals(-90. * angle[deg]));
      REQUIRE_THAT(asin(0 * one), AlmostEquals(0. * angle[deg]));
      REQUIRE_THAT(asin(1 * one), AlmostEquals(90. * angle[deg]));
    }

    SECTION("atan")
    {
      REQUIRE_THAT(atan(-1 * one), AlmostEquals(-45. * angle[deg]));
      REQUIRE_THAT(atan(0 * one), AlmostEquals(0. * angle[deg]));
      REQUIRE_THAT(atan(1 * one), AlmostEquals(45. * angle[deg]));
    }
  }

  SECTION("Angle atan2 functions")
  {
    using namespace mp_units::angular;
    using namespace mp_units::angular::unit_symbols;
    using mp_units::angular::unit_symbols::deg;

    SECTION("atan2 should work on the same quantities")
    {
      REQUIRE_THAT(atan2(-1. * isq::length[km], 1. * isq::length[km]), AlmostEquals(-45. * angle[deg]));
      REQUIRE_THAT(atan2(0. * isq::length[km], 1. * isq::length[km]), AlmostEquals(0. * angle[deg]));
      REQUIRE_THAT(atan2(1. * isq::length[km], 1. * isq::length[km]), AlmostEquals(45. * angle[deg]));
    }
    SECTION("atan2 should work with different units of the same dimension")
    {
      REQUIRE_THAT(atan2(-1. * isq::length[km], 1000. * isq::length[m]), AlmostEquals(-45. * angle[deg]));
      REQUIRE_THAT(atan2(0. * isq::length[km], 1000. * isq::length[m]), AlmostEquals(0. * angle[deg]));
      REQUIRE_THAT(atan2(1. * isq::length[km], 1000. * isq::length[m]), AlmostEquals(45. * angle[deg]));
    }
  }

  SECTION("inverse functions")
  {
    SECTION("inverse of time quantity returns frequency")
    {
      auto period = 2.0 * isq::time[s];
      auto frequency = inverse<si::hertz>(period);
      REQUIRE(frequency == 0.5 * isq::frequency[Hz]);
    }

    SECTION("inverse works with runtime values")
    {
      // Test the specific case that fails with consteval
      double runtime_value = 3.0;
      auto period = runtime_value * isq::time[s];
      auto frequency = inverse<si::hertz>(period);
      auto expected = (1.0 / 3.0) * isq::frequency[Hz];
      REQUIRE_THAT(frequency, AlmostEquals(expected));
    }

    SECTION("inverse with different input units")
    {
      auto period_ms = 500.0 * isq::time[ms];
      auto frequency = inverse<si::hertz>(period_ms);
      REQUIRE(frequency == 2.0 * isq::frequency[Hz]);
    }
  }
}
