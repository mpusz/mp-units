// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#ifdef MP_UNITS_MODULES
import mp_units;
#else
  #include <mp-units/cartesian_vector.h>
  #if MP_UNITS_HOSTED
    #include <mp-units/ext/format.h>
    #include <sstream>
  #endif
#endif

using namespace mp_units;
using Catch::Matchers::WithinAbs;

TEST_CASE("cartesian_vector initialization and access", "[vector]") {
  SECTION("no arguments") {
    cartesian_vector<double> v;
    REQUIRE(v[0] == 0);
    REQUIRE(v[1] == 0);
    REQUIRE(v[2] == 0);
  }

  SECTION("zero arguments") {
    cartesian_vector<double> v{};
    REQUIRE(v[0] == 0);
    REQUIRE(v[1] == 0);
    REQUIRE(v[2] == 0);
  }

  SECTION("one argument") {
    cartesian_vector v{1.0};
    REQUIRE(v[0] == 1.0);
    REQUIRE(v[1] == 0);
    REQUIRE(v[2] == 0);
  }

  SECTION("two arguments") {
    cartesian_vector v{1.0, 2.0};
    REQUIRE(v[0] == 1.0);
    REQUIRE(v[1] == 2.0);
    REQUIRE(v[2] == 0);
  }

  SECTION("all arguments") {
    cartesian_vector v{1.0, 2.0, 3.0};
    REQUIRE(v[0] == 1.0);
    REQUIRE(v[1] == 2.0);
    REQUIRE(v[2] == 3.0);
  }

  SECTION("convertible arguments") {
    cartesian_vector<double> v{1, 2, 3};
    REQUIRE(v[0] == 1.0);
    REQUIRE(v[1] == 2.0);
    REQUIRE(v[2] == 3.0);
  }
}

TEST_CASE("convertibility from another vector", "[vector]") {
  cartesian_vector v1{1, 2, 3};

  SECTION("construction") {
    cartesian_vector<double> v2 = v1;
    REQUIRE(v2[0] == 1.0);
    REQUIRE(v2[1] == 2.0);
    REQUIRE(v2[2] == 3.0);
  }

  SECTION("assignment") {
    cartesian_vector<double> v2{3.0, 2.0, 1.0};
    v2 = v1;
    REQUIRE(v2[0] == 1.0);
    REQUIRE(v2[1] == 2.0);
    REQUIRE(v2[2] == 3.0);
  }
}

TEST_CASE("compound assignments", "[vector]") {
  SECTION("operator+=") {
    cartesian_vector v1{1.0, 2.0, 3.0};
    cartesian_vector v2{4.0, 5.0, 6.0};
    v1 += v2;
    REQUIRE(v1[0] == 5.0);
    REQUIRE(v1[1] == 7.0);
    REQUIRE(v1[2] == 9.0);
  }

  SECTION("operator-=") {
    cartesian_vector v1{4.0, 5.0, 6.0};
    cartesian_vector v2{1.0, 2.0, 3.0};
    v1 -= v2;
    REQUIRE(v1[0] == 3.0);
    REQUIRE(v1[1] == 3.0);
    REQUIRE(v1[2] == 3.0);
  }

  SECTION("operator*=") {
    cartesian_vector v{1.0, 2.0, 3.0};
    v *= 2.0;
    REQUIRE(v[0] == 2.0);
    REQUIRE(v[1] == 4.0);
    REQUIRE(v[2] == 6.0);
  }

  SECTION("operator/=") {
    cartesian_vector v{2.0, 4.0, 6.0};
    v /= 2.0;
    REQUIRE(v[0] == 1.0);
    REQUIRE(v[1] == 2.0);
    REQUIRE(v[2] == 3.0);
  }
}

TEST_CASE("binary ops + and -", "[vector]") {
  SECTION("double + double") {
    cartesian_vector v1{1.0, 2.0, 3.0};
    cartesian_vector v2{4.0, 5.0, 6.0};
    cartesian_vector result = v1 + v2;
    REQUIRE(result[0] == 5.0);
    REQUIRE(result[1] == 7.0);
    REQUIRE(result[2] == 9.0);
  }

  SECTION("int - int") {
    cartesian_vector v1{4, 5, 6};
    cartesian_vector v2{1, 2, 3};
    cartesian_vector result = v1 - v2;
    REQUIRE(result[0] == 3);
    REQUIRE(result[1] == 3);
    REQUIRE(result[2] == 3);
  }
}

TEST_CASE("elementwise modulo", "[vector]") {
  SECTION("integral % integral") {
    cartesian_vector<int> a{10, 11, 12};
    cartesian_vector<int> b{ 4,  5,  7};
    auto r = a % b;
    REQUIRE(r[0] == 2);
    REQUIRE(r[1] == 1);
    REQUIRE(r[2] == 5);
  }

  SECTION("floating uses fmod") {
    cartesian_vector<double> a{ 5.5, 7.25, 9.0};
    cartesian_vector<double> b{ 2.0, 2.50, 4.0};
    auto r = a % b;
    REQUIRE_THAT(r[0], WithinAbs(1.5, 1e-12));
    REQUIRE_THAT(r[1], WithinAbs(2.25, 1e-12));
    REQUIRE_THAT(r[2], WithinAbs(1.0, 1e-12));
  }
}

TEST_CASE("scalar multiply/divide", "[vector]") {
  cartesian_vector<double> v{1, 2, 3};

  SECTION("v * s") {
    auto r = v * 2.0;
    REQUIRE(r[0] == 2.0);
    REQUIRE(r[1] == 4.0);
    REQUIRE(r[2] == 6.0);
  }

  SECTION("s * v") {
    auto r = 2.0 * v;
    REQUIRE(r[0] == 2.0);
    REQUIRE(r[1] == 4.0);
    REQUIRE(r[2] == 6.0);
  }

  SECTION("v / s") {
    auto r = v / 2.0;
    REQUIRE(r[0] == 0.5);
    REQUIRE(r[1] == 1.0);
    REQUIRE(r[2] == 1.5);
  }
}

TEST_CASE("equality and inequality", "[vector]") {
  cartesian_vector v1{1.0, 2.0, 3.0};
  cartesian_vector v2{1, 2, 3};
  cartesian_vector v3{1.1, 2.0, 3.0};
  REQUIRE(v1 == v2);
  REQUIRE(v1 != v3);
}

TEST_CASE("dot and cross (numeric level)", "[vector]") {
  cartesian_vector a{1, 2, 3};
  cartesian_vector b{4, 5, 6};

  SECTION("scalar_product and alias dot") {
    REQUIRE(scalar_product(a, b) == 32);
    REQUIRE(dot(a, b) == 32);
  }

  SECTION("vector_product and alias cross") {
    auto r = vector_product(a, b);
    REQUIRE(r[0] == -3);
    REQUIRE(r[1] ==  6);
    REQUIRE(r[2] == -3);

    auto r2 = cross(a, b);
    REQUIRE(r2[0] == -3);
    REQUIRE(r2[1] ==  6);
    REQUIRE(r2[2] == -3);
  }
}

TEST_CASE("magnitude and unit (floating types)", "[vector]") {
  cartesian_vector<double> v{3.0, 4.0, 0.0};
  REQUIRE_THAT(v.magnitude(), WithinAbs(5.0, 1e-12));

  auto u = v.unit();
  REQUIRE_THAT(u.magnitude(), WithinAbs(1.0, 1e-12));
  REQUIRE_THAT(u[0], WithinAbs(3.0/5.0, 1e-12));
  REQUIRE_THAT(u[1], WithinAbs(4.0/5.0, 1e-12));
  REQUIRE_THAT(u[2], WithinAbs(0.0,       1e-12));
}

#if MP_UNITS_HOSTED
TEST_CASE("text output (ostream + fmt)", "[vector][fmt][ostream]") {
  cartesian_vector v{1, 2, 3};

  // iostream
  std::ostringstream os;
  os << v;
  CHECK(os.str() == "[1, 2, 3]");

  // fmt (via MP_UNITS_STD_FMT wrapper)
  CHECK(MP_UNITS_STD_FMT::format("{}", v) == os.str());
}
#endif

TEST_CASE("constexpr basics", "[vector][constexpr]") {
  constexpr cartesian_vector<int> a{1,2,3};
  constexpr cartesian_vector<int> b{4,5,6};
  constexpr auto c = a + b;
  static_assert(c._coordinates_[0] == 5 && c._coordinates_[1] == 7 && c._coordinates_[2] == 9);
  (void)c;
}
