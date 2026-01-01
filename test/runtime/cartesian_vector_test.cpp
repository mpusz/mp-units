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

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
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

TEST_CASE("cartesian_vector", "[vector]")
{
  SECTION("construction & access")
  {
    SECTION("value-initialization yields zeros")
    {
      cartesian_vector<double> v{};
      REQUIRE(v[0] == 0);
      REQUIRE(v[1] == 0);
      REQUIRE(v[2] == 0);
    }
    SECTION("one argument -> [x,0,0]")
    {
      cartesian_vector v{1.0};
      REQUIRE(v[0] == 1.0);
      REQUIRE(v[1] == 0.0);
      REQUIRE(v[2] == 0.0);
    }
    SECTION("two arguments -> [x,y,0]")
    {
      cartesian_vector v{1.0, 2.0};
      REQUIRE(v[0] == 1.0);
      REQUIRE(v[1] == 2.0);
      REQUIRE(v[2] == 0.0);
    }
    SECTION("three arguments")
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

  SECTION("aggregate assignment")
  {
    SECTION("aggregate copy assignment")
    {
      cartesian_vector v1{1.0, 2.0, 3.0};
      cartesian_vector<double> v2 = v1;
      REQUIRE(v2[0] == 1.0);
      REQUIRE(v2[1] == 2.0);
      REQUIRE(v2[2] == 3.0);
    }
    SECTION("aggregate assignment from different rep")
    {
      cartesian_vector v1{1, 2, 3};
      cartesian_vector<double> v2{static_cast<double>(v1[0]), static_cast<double>(v1[1]), static_cast<double>(v1[2])};
      REQUIRE(v2[0] == 1.0);
      REQUIRE(v2[1] == 2.0);
      REQUIRE(v2[2] == 3.0);
    }
  }

  SECTION("compound assignments")
  {
    SECTION("operator+=")
    {
      cartesian_vector v1{1.0, 2.0, 3.0};
      cartesian_vector v2{4.0, 5.0, 6.0};
      v1 += v2;
      REQUIRE(v1[0] == 5.0);
      REQUIRE(v1[1] == 7.0);
      REQUIRE(v1[2] == 9.0);
    }
    SECTION("operator-=")
    {
      cartesian_vector v1{4.0, 5.0, 6.0};
      cartesian_vector v2{1.0, 2.0, 3.0};
      v1 -= v2;
      REQUIRE(v1[0] == 3.0);
      REQUIRE(v1[1] == 3.0);
      REQUIRE(v1[2] == 3.0);
    }
    SECTION("operator*=")
    {
      cartesian_vector v{1.0, 2.0, 3.0};
      v *= 2.0;
      REQUIRE(v[0] == 2.0);
      REQUIRE(v[1] == 4.0);
      REQUIRE(v[2] == 6.0);
    }
    SECTION("operator/=")
    {
      cartesian_vector v{2.0, 4.0, 6.0};
      v /= 2.0;
      REQUIRE(v[0] == 1.0);
      REQUIRE(v[1] == 2.0);
      REQUIRE(v[2] == 3.0);
    }
  }

  SECTION("binary + and -")
  {
    SECTION("double + double")
    {
      cartesian_vector v1{1.0, 2.0, 3.0};
      cartesian_vector v2{4.0, 5.0, 6.0};
      auto r = v1 + v2;
      REQUIRE(r[0] == 5.0);
      REQUIRE(r[1] == 7.0);
      REQUIRE(r[2] == 9.0);
    }
    SECTION("int - int")
    {
      cartesian_vector v1{4, 5, 6};
      cartesian_vector v2{1, 2, 3};
      auto r = v1 - v2;
      REQUIRE(r[0] == 3);
      REQUIRE(r[1] == 3);
      REQUIRE(r[2] == 3);
    }
  }

  SECTION("elementwise modulo (integral only)")
  {
    cartesian_vector<int> a{10, 11, 12};
    cartesian_vector<int> b{4, 5, 7};
    auto r = a % b;
    REQUIRE(r[0] == 2);
    REQUIRE(r[1] == 1);
    REQUIRE(r[2] == 5);
  }

  SECTION("scalar multiply/divide")
  {
    cartesian_vector<double> v{1, 2, 3};
    SECTION("v * s")
    {
      auto r = v * 2.0;
      REQUIRE(r[0] == 2.0);
      REQUIRE(r[1] == 4.0);
      REQUIRE(r[2] == 6.0);
    }
    SECTION("s * v")
    {
      auto r = 2.0 * v;
      REQUIRE(r[0] == 2.0);
      REQUIRE(r[1] == 4.0);
      REQUIRE(r[2] == 6.0);
    }
    SECTION("v / s")
    {
      auto r = v / 2.0;
      REQUIRE(r[0] == 0.5);
      REQUIRE(r[1] == 1.0);
      REQUIRE(r[2] == 1.5);
    }
  }

  SECTION("equality and inequality")
  {
    cartesian_vector v1{1.0, 2.0, 3.0};
    cartesian_vector v2{1, 2, 3};
    cartesian_vector v3{1.1, 2.0, 3.0};
    REQUIRE(v1 == v2);
    REQUIRE(v1 != v3);
  }

  SECTION("scalar_product and vector_product")
  {
    cartesian_vector a{1, 2, 3};
    cartesian_vector b{4, 5, 6};
    REQUIRE(scalar_product(a, b) == 32);
    auto r = vector_product(a, b);
    REQUIRE(r[0] == -3);
    REQUIRE(r[1] == 6);
    REQUIRE(r[2] == -3);
  }

  SECTION("magnitude and unit (floating types)")
  {
    cartesian_vector<double> v{3.0, 4.0, 0.0};
    REQUIRE_THAT(v.magnitude(), Catch::Matchers::WithinULP(5.0, 2));
    auto u = v.unit();
    REQUIRE_THAT(u.magnitude(), Catch::Matchers::WithinULP(1.0, 2));
    REQUIRE_THAT(u[0], Catch::Matchers::WithinULP(3.0 / 5.0, 2));
    REQUIRE_THAT(u[1], Catch::Matchers::WithinULP(4.0 / 5.0, 2));
    REQUIRE_THAT(u[2], Catch::Matchers::WithinULP(0.0, 2));
  }

  SECTION("constexpr basics")
  {
    constexpr cartesian_vector<int> a{1, 2, 3};
    constexpr cartesian_vector<int> b{4, 5, 6};
    constexpr auto c = a + b;
    static_assert(c._coordinates_[0] == 5 && c._coordinates_[1] == 7 && c._coordinates_[2] == 9);
    (void)c;
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
