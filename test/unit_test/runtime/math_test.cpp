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

#include "units/math.h"
#include "units/physical/si/si.h"
#include <catch2/catch.hpp>

using namespace units;
using namespace units::physical::si;

// classical

TEST_CASE("'pow<N>()' on quantity changes the value and the dimension accordingly", "[math][pow]")
{
  SECTION ("'pow<0>(q)' returns '1'") {
    CHECK(pow<0>(2_q_m) == 1);
  }

  SECTION ("'pow<1>(q)' returns 'q'") {
    CHECK(pow<1>(2_q_m) == 2_q_m);
  }

  SECTION ("'pow<2>(q)' squares both the value and a dimension") {
    CHECK(pow<2>(2_q_m) == 4_q_m2);
  }

  SECTION ("'pow<3>(q)' cubes both the value and a dimension") {
    CHECK(pow<3>(2_q_m) == 8_q_m3);
  }
}

TEST_CASE("'sqrt()' on quantity changes the value and the dimension accordingly", "[math][sqrt]")
{
  REQUIRE(sqrt(4_q_m2) == 2_q_m);
}

TEST_CASE("absolute functions on quantity returns the absolute value", "[math][abs][fabs]")
{
  SECTION ("'abs()' on a negative quantity returns the abs")
  {
    SECTION ("integral representation")
    {
      REQUIRE(abs(-1_q_m) == 1_q_m);
    }

#ifndef COMP_MSVC
    SECTION ("floating-point representation")
    {
      REQUIRE(abs(-1._q_m) == 1_q_m);
    }
#endif
  }

  SECTION ("'abs()' on a positive quantity returns the abs")
  {
    SECTION ("integral representation")
    {
      REQUIRE(abs(1_q_m) == 1_q_m);
    }

#ifndef COMP_MSVC
    SECTION ("floating-point representation")
    {
      REQUIRE(abs(1._q_m) == 1_q_m);
    }
#endif
  }
}

TEST_CASE("numeric_limits functions", "[limits]")
{
  SECTION ("'epsilon' works as expected using default floating type") {
    REQUIRE(epsilon<decltype(1._q_m)>().count() == std::numeric_limits<decltype(1._q_m)::rep>::epsilon());
  }
  SECTION ("'epsilon' works as expected using integers") {
    REQUIRE(epsilon<decltype(1_q_m)>().count() == std::numeric_limits<decltype(1_q_m)::rep>::epsilon());
  }
  SECTION ("'epsilon' works as expected using mixed Rep types") {
    REQUIRE(epsilon<decltype(1_q_m)>().count() != std::numeric_limits<decltype(1._q_m)::rep>::epsilon());
  }
}
