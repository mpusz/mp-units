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
#include <limits>
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/math.h>
#include <mp-units/ostream.h>  // IWYU pragma: keep
#include <mp-units/systems/angular.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si.h>
#endif

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

// classical

TEST_CASE("'pow<N>()' on quantity changes the value and the dimension accordingly", "[math][pow]")
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

TEST_CASE("'sqrt()' on quantity changes the value and the dimension accordingly", "[math][sqrt]")
{
  REQUIRE(sqrt(4 * isq::area[m2]) == 2 * isq::length[m]);
}

TEST_CASE("'cbrt()' on quantity changes the value and the dimension accordingly", "[math][cbrt]")
{
  REQUIRE(cbrt(8 * isq::volume[m3]) == 2 * isq::length[m]);
}

TEST_CASE("'fma()' on quantity changes the value and the dimension accordingly", "[math][fma]")
{
  REQUIRE(fma(1.0 * isq::length[m], 2.0 * one, 2.0 * isq::length[m]) == 4.0 * isq::length[m]);
  REQUIRE(fma(isq::speed(10.0 * m / s), isq::time(2.0 * s), isq::height(42.0 * m)) == isq::length(62.0 * m));
}

TEST_CASE("fmod functions", "[math][fmod]")
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

TEST_CASE("remainder functions", "[math][remainder]")
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

TEST_CASE("'isfinite()' accepts dimensioned arguments", "[math][isfinite]") { REQUIRE(isfinite(4.0 * isq::length[m])); }

TEST_CASE("'isinf()' accepts dimensioned arguments", "[math][isinf]") { REQUIRE(!isinf(4.0 * isq::length[m])); }

TEST_CASE("'isnan()' accepts dimensioned arguments", "[math][isnan]") { REQUIRE(!isnan(4.0 * isq::length[m])); }


TEST_CASE("'pow<Num, Den>()' on quantity changes the value and the dimension accordingly", "[math][pow]")
{
  REQUIRE(pow<1, 4>(16 * isq::area[m2]) == sqrt(4 * isq::length[m]));
}

// TODO add tests for exp()

TEST_CASE("absolute functions on quantity returns the absolute value", "[math][abs][fabs]")
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

TEST_CASE("numeric_limits functions", "[limits]")
{
  SECTION("'epsilon' works as expected using default floating type")
  {
    REQUIRE(epsilon<double>(isq::length[m]).numerical_value_in(m) ==
            std::numeric_limits<decltype(1. * isq::length[m])::rep>::epsilon());
  }
  SECTION("'epsilon' works as expected using integers")
  {
    REQUIRE(epsilon<int>(isq::length[m]).numerical_value_in(m) ==
            std::numeric_limits<decltype(1 * isq::length[m])::rep>::epsilon());
  }
}

TEST_CASE("floor functions", "[floor]")
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

TEST_CASE("ceil functions", "[ceil]")
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

TEST_CASE("round functions", "[round]")
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

TEST_CASE("hypot functions", "[hypot]")
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

TEST_CASE("SI trigonometric functions", "[trig][si]")
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

TEST_CASE("SI inverse trigonometric functions", "[inv trig][si]")
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

TEST_CASE("SI atan2 functions", "[atan2][si]")
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


TEST_CASE("Angle trigonometric functions", "[trig][angle]")
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
    REQUIRE_THAT(sin(200 * angle[grad]), AlmostEquals(0. * one));
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
    REQUIRE_THAT(tan(200 * angle[grad]), AlmostEquals(0. * one));
  }
}

TEST_CASE("Angle inverse trigonometric functions", "[inv trig][angle]")
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

TEST_CASE("Angle atan2 functions", "[atan2][angle]")
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
