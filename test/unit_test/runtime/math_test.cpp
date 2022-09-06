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
#include <catch2/catch_all.hpp>
#include <units/isq/si/area.h>
#include <units/isq/si/cgs/length.h>
#include <units/isq/si/length.h>
#include <units/isq/si/time.h>
#include <units/isq/si/volume.h>
#include <units/math.h>
#include <units/quantity_io.h>
#include <limits>

using namespace units;
using namespace units::isq;
using namespace units::isq::si::literals;

// classical

TEST_CASE("'pow<N>()' on quantity changes the value and the dimension accordingly", "[math][pow]")
{
  SECTION("'pow<0>(q)' returns '1'") { CHECK(pow<0>(2_q_m) == 1); }

  SECTION("'pow<1>(q)' returns 'q'") { CHECK(pow<1>(2_q_m) == 2_q_m); }

  SECTION("'pow<2>(q)' squares both the value and a dimension") { CHECK(pow<2>(2_q_m) == 4_q_m2); }

  SECTION("'pow<3>(q)' cubes both the value and a dimension") { CHECK(pow<3>(2_q_m) == 8_q_m3); }
}

TEST_CASE("'sqrt()' on quantity changes the value and the dimension accordingly", "[math][sqrt]")
{
  REQUIRE(sqrt(4_q_m2) == 2_q_m);
}

TEST_CASE("'cbrt()' on quantity changes the value and the dimension accordingly", "[math][cbrt]")
{
  REQUIRE(cbrt(8_q_m3) == 2_q_m);
}

TEST_CASE("'pow<Num, Den>()' on quantity changes the value and the dimension accordingly", "[math][pow]")
{
  REQUIRE(pow<1, 4>(16_q_m2) == sqrt(4_q_m));
}

TEST_CASE("absolute functions on quantity returns the absolute value", "[math][abs][fabs]")
{
  SECTION("'abs()' on a negative quantity returns the abs")
  {
    SECTION("integral representation") { REQUIRE(abs(-1_q_m) == 1_q_m); }

    SECTION("floating-point representation") { REQUIRE(abs(-1._q_m) == 1_q_m); }
  }

  SECTION("'abs()' on a positive quantity returns the abs")
  {
    SECTION("integral representation") { REQUIRE(abs(1_q_m) == 1_q_m); }

    SECTION("floating-point representation") { REQUIRE(abs(1._q_m) == 1_q_m); }
  }
}

TEST_CASE("numeric_limits functions", "[limits]")
{
  SECTION("'epsilon' works as expected using default floating type")
  {
    REQUIRE(epsilon<decltype(1._q_m)>().number() == std::numeric_limits<decltype(1._q_m)::rep>::epsilon());
  }
  SECTION("'epsilon' works as expected using integers")
  {
    REQUIRE(epsilon<decltype(1_q_m)>().number() == std::numeric_limits<decltype(1_q_m)::rep>::epsilon());
  }
  SECTION("'epsilon' works as expected using mixed Rep types")
  {
    REQUIRE(epsilon<decltype(1_q_m)>().number() != std::numeric_limits<decltype(1._q_m)::rep>::epsilon());
  }
}

TEST_CASE("floor functions", "[floor]")
{
  SECTION("floor 1 second with target unit second should be 1 second") { REQUIRE(floor<si::second>(1_q_s) == 1_q_s); }
  SECTION("floor 1000 milliseconds with target unit second should be 1 second")
  {
    REQUIRE(floor<si::second>(1000_q_ms) == 1_q_s);
  }
  SECTION("floor 1001 milliseconds with target unit second should be 1 second")
  {
    REQUIRE(floor<si::second>(1001_q_ms) == 1_q_s);
  }
  SECTION("floor 1999 milliseconds with target unit second should be 1 second")
  {
    REQUIRE(floor<si::second>(1999_q_ms) == 1_q_s);
  }
  SECTION("floor -1000 milliseconds with target unit second should be -1 second")
  {
    REQUIRE(floor<si::second>(-1000_q_ms) == -1_q_s);
  }
  SECTION("floor -999 milliseconds with target unit second should be -1 second")
  {
    REQUIRE(floor<si::second>(-999_q_ms) == -1_q_s);
  }
  SECTION("floor 1.3 seconds with target unit second should be 1 second")
  {
    REQUIRE(floor<si::second>(1.3_q_s) == 1_q_s);
  }
  SECTION("floor -1.3 seconds with target unit second should be -2 seconds")
  {
    REQUIRE(floor<si::second>(-1.3_q_s) == -2_q_s);
  }
  SECTION("floor 1001. milliseconds with target unit second should be 1 second")
  {
    REQUIRE(floor<si::second>(1001._q_ms) == 1_q_s);
  }
  SECTION("floor 1999. milliseconds with target unit second should be 1 second")
  {
    REQUIRE(floor<si::second>(1999._q_ms) == 1_q_s);
  }
  SECTION("floor -1000. milliseconds with target unit second should be -1 second")
  {
    REQUIRE(floor<si::second>(-1000._q_ms) == -1_q_s);
  }
  SECTION("floor -999. milliseconds with target unit second should be -1 second")
  {
    REQUIRE(floor<si::second>(-999._q_ms) == -1_q_s);
  }
  SECTION("floor 1 second with target quantity with unit type second should be 1 second")
  {
    using showtime = si::time<si::second, int>;
    REQUIRE(floor<showtime>(showtime::one()) == showtime::one());
  }
}

TEST_CASE("ceil functions", "[ceil]")
{
  SECTION("ceil 1 second with target unit second should be 1 second") { REQUIRE(ceil<si::second>(1_q_s) == 1_q_s); }
  SECTION("ceil 1000 milliseconds with target unit second should be 1 second")
  {
    REQUIRE(ceil<si::second>(1000_q_ms) == 1_q_s);
  }
  SECTION("ceil 1001 milliseconds with target unit second should be 2 seconds")
  {
    REQUIRE(ceil<si::second>(1001_q_ms) == 2_q_s);
  }
  SECTION("ceil 1999 milliseconds with target unit second should be 2 seconds")
  {
    REQUIRE(ceil<si::second>(1999_q_ms) == 2_q_s);
  }
  SECTION("ceil -1000 milliseconds with target unit second should be -1 second")
  {
    REQUIRE(ceil<si::second>(-1000_q_ms) == -1_q_s);
  }
  SECTION("ceil -999 milliseconds with target unit second should be 0 seconds")
  {
    REQUIRE(ceil<si::second>(-999_q_ms) == 0_q_s);
  }
  SECTION("ceil 1.3 seconds with target unit second should be 2 seconds")
  {
    REQUIRE(ceil<si::second>(1.3_q_s) == 2_q_s);
  }
  SECTION("ceil -1.3 seconds with target unit second should be -1 second")
  {
    REQUIRE(ceil<si::second>(-1.3_q_s) == -1_q_s);
  }
  SECTION("ceil 1001. milliseconds with target unit second should be 2 seconds")
  {
    REQUIRE(ceil<si::second>(1001._q_ms) == 2_q_s);
  }
  SECTION("ceil 1999. milliseconds with target unit second should be 2 seconds")
  {
    REQUIRE(ceil<si::second>(1999._q_ms) == 2_q_s);
  }
  SECTION("ceil -1000. milliseconds with target unit second should be -1 second")
  {
    REQUIRE(ceil<si::second>(-1000._q_ms) == -1_q_s);
  }
  SECTION("ceil -999. milliseconds with target unit second should be 0 seconds")
  {
    REQUIRE(ceil<si::second>(-999._q_ms) == 0_q_s);
  }
  SECTION("ceil 1 second with target quantity with unit type second should be 1 second")
  {
    using showtime = si::time<si::second, int>;
    REQUIRE(ceil<showtime>(showtime::one()) == showtime::one());
  }
}

TEST_CASE("round functions", "[round]")
{
  SECTION("round 1 second with target unit second should be 1 second") { REQUIRE(round<si::second>(1_q_s) == 1_q_s); }
  SECTION("round 1000 milliseconds with target unit second should be 1 second")
  {
    REQUIRE(round<si::second>(1000_q_ms) == 1_q_s);
  }
  SECTION("round 1001 milliseconds with target unit second should be 1 second")
  {
    REQUIRE(round<si::second>(1001_q_ms) == 1_q_s);
  }
  SECTION("round 1499 milliseconds with target unit second should be 1 second")
  {
    REQUIRE(round<si::second>(1499_q_ms) == 1_q_s);
  }
  SECTION("round 1500 milliseconds with target unit second should be 2 seconds")
  {
    REQUIRE(round<si::second>(1500_q_ms) == 2_q_s);
  }
  SECTION("round 1999 milliseconds with target unit second should be 2 seconds")
  {
    REQUIRE(round<si::second>(1999_q_ms) == 2_q_s);
  }
  SECTION("round -1000 milliseconds with target unit second should be -1 second")
  {
    REQUIRE(round<si::second>(-1000_q_ms) == -1_q_s);
  }
  SECTION("round -1001 milliseconds with target unit second should be -1 second")
  {
    REQUIRE(round<si::second>(-1001_q_ms) == -1_q_s);
  }
  SECTION("round -1499 milliseconds with target unit second should be -1 second")
  {
    REQUIRE(round<si::second>(-1499_q_ms) == -1_q_s);
  }
  SECTION("round -1500 milliseconds with target unit second should be -2 seconds")
  {
    REQUIRE(round<si::second>(-1500_q_ms) == -2_q_s);
  }
  SECTION("round -1999 milliseconds with target unit second should be -2 seconds")
  {
    REQUIRE(round<si::second>(-1999_q_ms) == -2_q_s);
  }
  SECTION("round 1000. milliseconds with target unit second should be 1 second")
  {
    REQUIRE(round<si::second>(1000._q_ms) == 1_q_s);
  }
  SECTION("round 1001. milliseconds with target unit second should be 1 second")
  {
    REQUIRE(round<si::second>(1001._q_ms) == 1_q_s);
  }
  SECTION("round 1499. milliseconds with target unit second should be 1 second")
  {
    REQUIRE(round<si::second>(1499._q_ms) == 1_q_s);
  }
  SECTION("round 1500. milliseconds with target unit second should be 2 seconds")
  {
    REQUIRE(round<si::second>(1500._q_ms) == 2_q_s);
  }
  SECTION("round 1999. milliseconds with target unit second should be 2 seconds")
  {
    REQUIRE(round<si::second>(1999._q_ms) == 2_q_s);
  }
  SECTION("round -1000. milliseconds with target unit second should be -1 second")
  {
    REQUIRE(round<si::second>(-1000._q_ms) == -1_q_s);
  }
  SECTION("round -1001. milliseconds with target unit second should be -1 second")
  {
    REQUIRE(round<si::second>(-1001._q_ms) == -1_q_s);
  }
  SECTION("round -1499. milliseconds with target unit second should be -1 second")
  {
    REQUIRE(round<si::second>(-1499._q_ms) == -1_q_s);
  }
  SECTION("round -1500. milliseconds with target unit second should be -2 seconds")
  {
    REQUIRE(round<si::second>(-1500._q_ms) == -2_q_s);
  }
  SECTION("round -1999. milliseconds with target unit second should be -2 seconds")
  {
    REQUIRE(round<si::second>(-1999._q_ms) == -2_q_s);
  }
  SECTION("round 1 second with target quantity with unit type second should be 1 second")
  {
    using showtime = si::time<si::second, int>;
    REQUIRE(round<showtime>(showtime::one()) == showtime::one());
  }
}

TEST_CASE("hypot functions", "[hypot]")
{
  using namespace units::aliases::isq::si;

  SECTION("hypot should work on the same quantities")
  {
    REQUIRE(hypot(km<>(3.), km<>(4.)) == km<>(5.));
    REQUIRE(hypot(km<>(2.), km<>(3.), km<>(6.)) == km<>(7.));
  }
  SECTION("hypot should work with different units of the same dimension")
  {
    REQUIRE(hypot(km<>(3.), m<>(4000.)) == km<>(5.));
    REQUIRE(hypot(km<>(2.), m<>(3000.), km<>(6.)) == km<>(7.));
  }
  SECTION("hypot should work with different but equivalent dimensions")
  {
    REQUIRE(hypot(km<>(3.), cgs::length::cm<>(400'000.)) == km<>(5.));
    REQUIRE(hypot(km<>(2.), cgs::length::cm<>(300'000.), km<>(6.)) == km<>(7.));
  }
}

TEST_CASE("trigonometric functions", "[trig]")
{
  using namespace units::aliases;

  SECTION("sin")
  {
    REQUIRE_THAT(sin(deg<>(0.)), AlmostEquals(quantity{0.}));
    REQUIRE_THAT(sin(deg<>(90.)), AlmostEquals(quantity{1.}));
    REQUIRE_THAT(sin(deg<>(180.)), AlmostEquals(quantity{0.}));
    REQUIRE_THAT(sin(deg<>(270.)), AlmostEquals(quantity{-1.}));

    REQUIRE_THAT(sin(grad<>(0.)), AlmostEquals(quantity{0.}));
    REQUIRE_THAT(sin(grad<>(100.)), AlmostEquals(quantity{1.}));
    REQUIRE_THAT(sin(grad<>(200.)), AlmostEquals(quantity{0.}));
    REQUIRE_THAT(sin(grad<>(300.)), AlmostEquals(quantity{-1.}));
  }

  SECTION("cos")
  {
    REQUIRE_THAT(cos(deg<>(0.)), AlmostEquals(quantity{1.}));
    REQUIRE_THAT(cos(deg<>(90.)), AlmostEquals(quantity{0.}));
    REQUIRE_THAT(cos(deg<>(180.)), AlmostEquals(quantity{-1.}));
    REQUIRE_THAT(cos(deg<>(270.)), AlmostEquals(quantity{0.}));

    REQUIRE_THAT(cos(grad<>(0.)), AlmostEquals(quantity{1.}));
    REQUIRE_THAT(cos(grad<>(100.)), AlmostEquals(quantity{0.}));
    REQUIRE_THAT(cos(grad<>(200.)), AlmostEquals(quantity{-1.}));
    REQUIRE_THAT(cos(grad<>(300.)), AlmostEquals(quantity{0.}));
  }

  SECTION("tan")
  {
    REQUIRE_THAT(tan(deg<>(0.)), AlmostEquals(quantity{0.}));
    REQUIRE_THAT(tan(deg<>(45.)), AlmostEquals(quantity{1.}));
    REQUIRE_THAT(tan(deg<>(135.)), AlmostEquals(quantity{-1.}));
    REQUIRE_THAT(tan(deg<>(180.)), AlmostEquals(quantity{0.}));

    REQUIRE_THAT(tan(grad<>(0.)), AlmostEquals(quantity{0.}));
    REQUIRE_THAT(tan(grad<>(50.)), AlmostEquals(quantity{1.}));
    REQUIRE_THAT(tan(grad<>(150.)), AlmostEquals(quantity{-1.}));
    REQUIRE_THAT(tan(grad<>(200.)), AlmostEquals(quantity{0.}));
  }
}

TEST_CASE("inverse trigonometric functions", "[inv trig]")
{
  using namespace units::aliases;

  SECTION("asin")
  {
    REQUIRE_THAT(asin(quantity{-1.}), AlmostEquals(deg<>(-90.)));
    REQUIRE_THAT(asin(quantity{0.}), AlmostEquals(deg<>(0.)));
    REQUIRE_THAT(asin(quantity{1.}), AlmostEquals(deg<>(90.)));
  }

  SECTION("acos")
  {
    REQUIRE_THAT(asin(quantity{-1.}), AlmostEquals(deg<>(-90.)));
    REQUIRE_THAT(asin(quantity{0.}), AlmostEquals(deg<>(0.)));
    REQUIRE_THAT(asin(quantity{1.}), AlmostEquals(deg<>(90.)));
  }

  SECTION("atan")
  {
    REQUIRE_THAT(atan(quantity{-1.}), AlmostEquals(deg<>(-45.)));
    REQUIRE_THAT(atan(quantity{0.}), AlmostEquals(deg<>(0.)));
    REQUIRE_THAT(atan(quantity{1.}), AlmostEquals(deg<>(45.)));
  }
}
