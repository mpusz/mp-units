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
#include "print_helpers.h"
#include "units/dimensions/area.h"
#include "units/dimensions/volume.h"
#include <catch2/catch.hpp>

using namespace units;

// classical

TEST_CASE("pow<N>() on quantity changes the value and the dimension accordingly", "[math][pow]")
{
  //  CHECK(pow<0>(2m) == 2);
  CHECK(pow<1>(2m) == 2m);
  CHECK(pow<2>(2m) == 4sq_m);
  CHECK(pow<3>(2m) == 8cub_m);
}

TEST_CASE("sqrt() on quantity changes the value and the dimension accordingly", "[math][sqrt]")
{
  REQUIRE(sqrt(4sq_m) == 2m);
}

// BDD style

SCENARIO("quantities should work with pow<N>()", "[math][pow]")
{
  GIVEN("A quantity q") {
    auto q = 2m;

    REQUIRE(q.count() == 2);

    WHEN("pow<1>(q) is called") {
      auto res = pow<1>(q);

      THEN("the same quantity is received") {
        REQUIRE(res == q);
      }
    }
    WHEN("pow<2>(q) is called") {
      auto res = pow<2>(q);

      THEN("both the value and dimension is raised to the exponent 2") {
        REQUIRE(res == 4sq_m);
      }
    }
    WHEN("pow<3>(q) is called") {
      auto res = pow<3>(q);

      THEN("both the value and dimension is raised to the exponent 3") {
        REQUIRE(res == 8cub_m);
      }
    }
  }
}

SCENARIO("quantities should work with sqrt()", "[math][sqrt]")
{
  GIVEN("A quantity q") {
    auto q = 4sq_m;

    REQUIRE(q.count() == 4);

    WHEN("sqrt(q) is called") {
      auto res = sqrt(q);

      THEN("both the value and dimension are square rooted") {
        REQUIRE(res == 2m);
      }
    }
  }
}
