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
#include "units/physical/si/area.h"
#include "units/physical/si/volume.h"
#include <catch2/catch.hpp>

using namespace units;
using namespace units::si;

// classical

TEST_CASE("'pow<N>()' on quantity changes the value and the dimension accordingly", "[math][pow]")
{
  SECTION ("'pow<0>(q)' returns '1'") {
    CHECK(pow<0>(2q_m) == 1);
  }

  SECTION ("'pow<1>(q)' returns 'q'") {
    CHECK(pow<1>(2q_m) == 2q_m);
  }

  SECTION ("'pow<2>(q)' squares both the value and a dimension") {
    CHECK(pow<2>(2q_m) == 4q_m2);
  }

  SECTION ("'pow<3>(q)' cubes both the value and a dimension") {
    CHECK(pow<3>(2q_m) == 8q_m3);
  }
}

TEST_CASE("'sqrt()' on quantity changes the value and the dimension accordingly", "[math][sqrt]")
{
  REQUIRE(sqrt(4q_m2) == 2q_m);
}
