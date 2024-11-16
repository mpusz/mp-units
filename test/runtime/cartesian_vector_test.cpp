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
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <mp-units/compat_macros.h>
#include <mp-units/ext/format.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <sstream>
#endif
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/cartesian_vector.h>
#endif

using namespace mp_units;
using namespace Catch::Matchers;

TEST_CASE("cartesian_vector operations", "[vector]")
{
  SECTION("cartesian_vector initialization and access")
  {
    SECTION("no arguments")
    {
      cartesian_vector<double> v;
      REQUIRE(v[0] == 0);
      REQUIRE(v[1] == 0);
      REQUIRE(v[2] == 0);
    }

    SECTION("zero arguments")
    {
      cartesian_vector<double> v{};
      REQUIRE(v[0] == 0);
      REQUIRE(v[1] == 0);
      REQUIRE(v[2] == 0);
    }

    SECTION("one argument")
    {
      cartesian_vector v{1.0};
      REQUIRE(v[0] == 1.0);
      REQUIRE(v[1] == 0);
      REQUIRE(v[2] == 0);
    }

    SECTION("two arguments")
    {
      cartesian_vector v{1.0, 2.0};
      REQUIRE(v[0] == 1.0);
      REQUIRE(v[1] == 2.0);
      REQUIRE(v[2] == 0);
    }

    SECTION("all arguments")
    {
      cartesian_vector v{1.0, 2.0, 3.0};
      REQUIRE(v[0] == 1.0);
      REQUIRE(v[1] == 2.0);
      REQUIRE(v[2] == 3.0);
    }

    SECTION("convertible arguments")
    {
      cartesian_vector<double> v{1, 2, 3};
      REQUIRE(v[0] == 1.0);
      REQUIRE(v[1] == 2.0);
      REQUIRE(v[2] == 3.0);
    }
  }

  SECTION("convertibility from another vector")
  {
    cartesian_vector v1{1, 2, 3};

    SECTION("construction")
    {
      cartesian_vector<double> v2 = v1;
      REQUIRE(v2[0] == 1.0);
      REQUIRE(v2[1] == 2.0);
      REQUIRE(v2[2] == 3.0);
    }

    SECTION("assignment")
    {
      cartesian_vector<double> v2{3.0, 2.0, 1.0};
      v2 = v1;
      REQUIRE(v2[0] == 1.0);
      REQUIRE(v2[1] == 2.0);
      REQUIRE(v2[2] == 3.0);
    }
  }

  SECTION("cartesian_vector compound assignment addition")
  {
    cartesian_vector v1{1.0, 2.0, 3.0};
    cartesian_vector v2{4.0, 5.0, 6.0};
    v1 += v2;
    REQUIRE(v1[0] == 5.0);
    REQUIRE(v1[1] == 7.0);
    REQUIRE(v1[2] == 9.0);
  }

  SECTION("cartesian_vector compound assignment subtraction")
  {
    cartesian_vector v1{4.0, 5.0, 6.0};
    cartesian_vector v2{1.0, 2.0, 3.0};
    v1 -= v2;
    REQUIRE(v1[0] == 3.0);
    REQUIRE(v1[1] == 3.0);
    REQUIRE(v1[2] == 3.0);
  }

  SECTION("cartesian_vector compound assignment scalar multiplication")
  {
    cartesian_vector v{1.0, 2.0, 3.0};
    v *= 2.0;
    REQUIRE(v[0] == 2.0);
    REQUIRE(v[1] == 4.0);
    REQUIRE(v[2] == 6.0);
  }

  SECTION("cartesian_vector compound assignment scalar division")
  {
    cartesian_vector v{2.0, 4.0, 6.0};
    v /= 2.0;
    REQUIRE(v[0] == 1.0);
    REQUIRE(v[1] == 2.0);
    REQUIRE(v[2] == 3.0);
  }

  SECTION("cartesian_vector addition")
  {
    SECTION("double + double")
    {
      cartesian_vector v1{1.0, 2.0, 3.0};
      cartesian_vector v2{4.0, 5.0, 6.0};
      cartesian_vector result = v1 + v2;
      REQUIRE(result[0] == 5.0);
      REQUIRE(result[1] == 7.0);
      REQUIRE(result[2] == 9.0);
    }

    SECTION("double + int")
    {
      cartesian_vector v1{1.0, 2.0, 3.0};
      cartesian_vector v2{4, 5, 6};
      cartesian_vector result = v1 + v2;
      REQUIRE(result[0] == 5.0);
      REQUIRE(result[1] == 7.0);
      REQUIRE(result[2] == 9.0);
    }

    SECTION("int + double")
    {
      cartesian_vector v1{1, 2, 3};
      cartesian_vector v2{4.0, 5.0, 6.0};
      cartesian_vector result = v1 + v2;
      REQUIRE(result[0] == 5.0);
      REQUIRE(result[1] == 7.0);
      REQUIRE(result[2] == 9.0);
    }

    SECTION("int + int")
    {
      cartesian_vector v1{1, 2, 3};
      cartesian_vector v2{4, 5, 6};
      cartesian_vector result = v1 + v2;
      REQUIRE(result[0] == 5);
      REQUIRE(result[1] == 7);
      REQUIRE(result[2] == 9);
    }
  }

  SECTION("cartesian_vector subtraction")
  {
    SECTION("double - double")
    {
      cartesian_vector v1{4.0, 5.0, 6.0};
      cartesian_vector v2{1.0, 2.0, 3.0};
      cartesian_vector result = v1 - v2;
      REQUIRE(result[0] == 3.0);
      REQUIRE(result[1] == 3.0);
      REQUIRE(result[2] == 3.0);
    }

    SECTION("double - int")
    {
      cartesian_vector v1{4.0, 5.0, 6.0};
      cartesian_vector v2{1, 2, 3};
      cartesian_vector result = v1 - v2;
      REQUIRE(result[0] == 3.0);
      REQUIRE(result[1] == 3.0);
      REQUIRE(result[2] == 3.0);
    }

    SECTION("int - double")
    {
      cartesian_vector v1{4, 5, 6};
      cartesian_vector v2{1.0, 2.0, 3.0};
      cartesian_vector result = v1 - v2;
      REQUIRE(result[0] == 3.0);
      REQUIRE(result[1] == 3.0);
      REQUIRE(result[2] == 3.0);
    }

    SECTION("int - int")
    {
      cartesian_vector v1{4, 5, 6};
      cartesian_vector v2{1, 2, 3};
      cartesian_vector result = v1 - v2;
      REQUIRE(result[0] == 3);
      REQUIRE(result[1] == 3);
      REQUIRE(result[2] == 3);
    }
  }

  SECTION("cartesian_vector scalar multiplication")
  {
    SECTION("double * double")
    {
      cartesian_vector v{1.0, 2.0, 3.0};
      cartesian_vector result = v * 2.0;
      REQUIRE(result[0] == 2.0);
      REQUIRE(result[1] == 4.0);
      REQUIRE(result[2] == 6.0);
    }

    SECTION("double * int")
    {
      cartesian_vector v{1.0, 2.0, 3.0};
      cartesian_vector result = v * 2;
      REQUIRE(result[0] == 2.0);
      REQUIRE(result[1] == 4.0);
      REQUIRE(result[2] == 6.0);
    }

    SECTION("int * double")
    {
      cartesian_vector v{1, 2, 3};
      cartesian_vector result = v * 2.0;
      REQUIRE(result[0] == 2.0);
      REQUIRE(result[1] == 4.0);
      REQUIRE(result[2] == 6.0);
    }

    SECTION("int * int")
    {
      cartesian_vector v{1, 2, 3};
      cartesian_vector result = v * 2;
      REQUIRE(result[0] == 2);
      REQUIRE(result[1] == 4);
      REQUIRE(result[2] == 6);
    }
  }

  SECTION("cartesian_vector scalar division")
  {
    SECTION("double / double")
    {
      cartesian_vector v{2.0, 4.0, 6.0};
      cartesian_vector result = v / 2.0;
      REQUIRE(result[0] == 1.0);
      REQUIRE(result[1] == 2.0);
      REQUIRE(result[2] == 3.0);
    }

    SECTION("double / int")
    {
      cartesian_vector v{2.0, 4.0, 6.0};
      cartesian_vector result = v / 2;
      REQUIRE(result[0] == 1.0);
      REQUIRE(result[1] == 2.0);
      REQUIRE(result[2] == 3.0);
    }

    SECTION("int / double")
    {
      cartesian_vector v{2, 4, 6};
      cartesian_vector result = v / 2.0;
      REQUIRE(result[0] == 1.0);
      REQUIRE(result[1] == 2.0);
      REQUIRE(result[2] == 3.0);
    }

    SECTION("int / int")
    {
      cartesian_vector v{2, 4, 6};
      cartesian_vector result = v / 2;
      REQUIRE(result[0] == 1);
      REQUIRE(result[1] == 2);
      REQUIRE(result[2] == 3);
    }
  }

  SECTION("cartesian_vector magnitude")
  {
    cartesian_vector v1{3.0, 4.0, 0.0};
    cartesian_vector v2{2.0, 3.0, 6.0};
    REQUIRE(v1.magnitude() == 5.0);
    REQUIRE(v2.magnitude() == 7.0);
  }

  SECTION("cartesian_vector unit vector")
  {
    cartesian_vector v{3.0, 4.0, 0.0};
    cartesian_vector unit_v = v.unit();
    REQUIRE_THAT(unit_v.magnitude(), WithinULP(1.0, 1));
  }

  SECTION("cartesian_vector equality")
  {
    cartesian_vector v1{1.0, 2.0, 3.0};
    cartesian_vector v2{1, 2, 3};
    cartesian_vector v3{1.1, 2.0, 3.0};
    cartesian_vector v4{1.0, 2.1, 3.0};
    cartesian_vector v5{1.0, 2.0, 3.1};
    REQUIRE(v1 == v2);
    REQUIRE(v1 != v3);
    REQUIRE(v1 != v4);
    REQUIRE(v1 != v5);
  }

  SECTION("cartesian_vector scalar product")
  {
    SECTION("double * double")
    {
      cartesian_vector v1{1.0, 2.0, 3.0};
      cartesian_vector v2{4.0, 5.0, 6.0};
      REQUIRE(scalar_product(v1, v2) == 32.0);
    }

    SECTION("double * int")
    {
      cartesian_vector v1{1.0, 2.0, 3.0};
      cartesian_vector v2{4, 5, 6};
      REQUIRE(scalar_product(v1, v2) == 32.0);
    }

    SECTION("int * double")
    {
      cartesian_vector v1{1, 2, 3};
      cartesian_vector v2{4.0, 5.0, 6.0};
      REQUIRE(scalar_product(v1, v2) == 32.0);
    }

    SECTION("int * int")
    {
      cartesian_vector v1{1, 2, 3};
      cartesian_vector v2{4, 5, 6};
      REQUIRE(scalar_product(v1, v2) == 32);
    }
  }

  SECTION("cartesian_vector vector product")
  {
    SECTION("double * double")
    {
      cartesian_vector v1{1.0, 2.0, 3.0};
      cartesian_vector v2{4.0, 5.0, 6.0};
      cartesian_vector result = vector_product(v1, v2);
      REQUIRE(result[0] == -3.0);
      REQUIRE(result[1] == 6.0);
      REQUIRE(result[2] == -3.0);
    }

    SECTION("double * int")
    {
      cartesian_vector v1{1.0, 2.0, 3.0};
      cartesian_vector v2{4, 5, 6};
      cartesian_vector result = vector_product(v1, v2);
      REQUIRE(result[0] == -3.0);
      REQUIRE(result[1] == 6.0);
      REQUIRE(result[2] == -3.0);
    }

    SECTION("int * double")
    {
      cartesian_vector v1{1, 2, 3};
      cartesian_vector v2{4.0, 5.0, 6.0};
      cartesian_vector result = vector_product(v1, v2);
      REQUIRE(result[0] == -3.0);
      REQUIRE(result[1] == 6.0);
      REQUIRE(result[2] == -3.0);
    }

    SECTION("int * int")
    {
      cartesian_vector v1{1, 2, 3};
      cartesian_vector v2{4, 5, 6};
      cartesian_vector result = vector_product(v1, v2);
      REQUIRE(result[0] == -3);
      REQUIRE(result[1] == 6);
      REQUIRE(result[2] == -3);
    }
  }
}

TEST_CASE("cartesian_vector text output", "[vector][fmt][ostream]")
{
  std::ostringstream os;

  SECTION("integral representation")
  {
    cartesian_vector v{1, 2, 3};
    os << v;

    SECTION("iostream") { CHECK(os.str() == "[1, 2, 3]"); }
    SECTION("fmt with default format {}") { CHECK(MP_UNITS_STD_FMT::format("{}", v) == os.str()); }
  }

  SECTION("floating-point representation")
  {
    cartesian_vector v{1.2, 2.3, 3.4};
    os << v;

    SECTION("iostream") { CHECK(os.str() == "[1.2, 2.3, 3.4]"); }
    SECTION("fmt with default format {}") { CHECK(MP_UNITS_STD_FMT::format("{}", v) == os.str()); }
  }
}
