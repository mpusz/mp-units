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

#ifdef MP_UNITS_MODULES
import mp_units;
#else
  #include <mp-units/cartesian_tensor.h>
  #include <mp-units/cartesian_vector.h>
  #if MP_UNITS_HOSTED
    #include <mp-units/ext/format.h>
    #include <sstream>
  #endif
#endif

using namespace mp_units;
using Catch::Matchers::WithinAbs;

TEST_CASE("cartesian_tensor construction and access", "[tensor]") {
  // 2x3 fill-ctor order is row-major
  cartesian_tensor<int, 2, 3> A{1,2,3, 4,5,6};
  REQUIRE(A(0,0) == 1);
  REQUIRE(A(0,1) == 2);
  REQUIRE(A(0,2) == 3);
  REQUIRE(A(1,0) == 4);
  REQUIRE(A(1,1) == 5);
  REQUIRE(A(1,2) == 6);
}

TEST_CASE("elementwise + and - with common_type", "[tensor]") {
  cartesian_tensor<int,    2,2> A{1,2, 3,4};
  cartesian_tensor<double, 2,2> B{0.5,1.5, 2.5,3.5};

  auto S = A + B;
  static_assert(std::is_same_v<decltype(S), cartesian_tensor<double,2,2>>);
  REQUIRE(S(0,0) == 1.5);
  REQUIRE(S(0,1) == 3.5);
  REQUIRE(S(1,0) == 5.5);
  REQUIRE(S(1,1) == 7.5);

  auto D = B - A;
REQUIRE_THAT(D(0,0), WithinAbs(-0.5, 1e-12));
REQUIRE_THAT(D(0,1), WithinAbs(-0.5, 1e-12));
REQUIRE_THAT(D(1,0), WithinAbs(-0.5, 1e-12));
REQUIRE_THAT(D(1,1), WithinAbs(-0.5, 1e-12));
}

TEST_CASE("elementwise modulo", "[tensor]") {
  SECTION("integral % integral") {
    cartesian_tensor<int, 2,3> A{10,11,12, 13,14,15};
    cartesian_tensor<int, 2,3> B{ 4, 5, 7,  4, 5, 7};
    auto R = A % B;
    REQUIRE(R(0,0) == 2); REQUIRE(R(0,1) == 1); REQUIRE(R(0,2) == 5);
    REQUIRE(R(1,0) == 1); REQUIRE(R(1,1) == 4); REQUIRE(R(1,2) == 1);
  }

  SECTION("floating uses fmod, result in common_type") {
    cartesian_tensor<double, 1,3> A{5.5, 7.25, 9.0};
    cartesian_tensor<double, 1,3> B{2.0, 2.50, 4.0};
    auto R = A % B;
    REQUIRE_THAT(R(0,0), WithinAbs(1.5, 1e-12));
    REQUIRE_THAT(R(0,1), WithinAbs(2.25, 1e-12));
    REQUIRE_THAT(R(0,2), WithinAbs(1.0, 1e-12));
  }
}

TEST_CASE("scalar multiply/divide", "[tensor]") {
  cartesian_tensor<double, 2,2> A{1,2,3,4};

  auto T1 = A * 2.0;
  REQUIRE(T1(0,0) == 2.0); REQUIRE(T1(0,1) == 4.0);
  REQUIRE(T1(1,0) == 6.0); REQUIRE(T1(1,1) == 8.0);

  auto T2 = 2.0 * A;
  REQUIRE(T2(0,0) == 2.0); REQUIRE(T2(0,1) == 4.0);
  REQUIRE(T2(1,0) == 6.0); REQUIRE(T2(1,1) == 8.0);

  auto T3 = A / 2.0;
  REQUIRE(T3(0,0) == 0.5); REQUIRE(T3(0,1) == 1.0);
  REQUIRE(T3(1,0) == 1.5); REQUIRE(T3(1,1) == 2.0);
}

TEST_CASE("matmul (R×K) * (K×C) -> (R×C)", "[tensor]") {
  cartesian_tensor<int, 2,3> A{1,2,3, 4,5,6};
  cartesian_tensor<int, 3,2> B{7,8, 9,10, 11,12};
  auto C = matmul(A, B); // 2x2

  static_assert(std::is_same_v<decltype(C), cartesian_tensor<int,2,2>>);
  REQUIRE(C(0,0) ==  58); REQUIRE(C(0,1) ==  64);
  REQUIRE(C(1,0) == 139); REQUIRE(C(1,1) == 154);
}

TEST_CASE("matvec (3x3) * vector -> vector", "[tensor][vector]") {
  cartesian_tensor<double,3,3> M{ 1,2,3,
                                  0,1,4,
                                  5,6,0 };
  cartesian_vector<double> x{1,2,3};
  auto y = matvec(M, x);
  REQUIRE_THAT(y[0], WithinAbs(14.0, 1e-12)); // 1*1 + 2*2 + 3*3
  REQUIRE_THAT(y[1], WithinAbs(14.0, 1e-12)); // 0*1 + 1*2 + 4*3
  REQUIRE_THAT(y[2], WithinAbs(17.0, 1e-12)); // 5*1 + 6*2 + 0*3
}

TEST_CASE("double contraction A : B", "[tensor]") {
  cartesian_tensor<int, 2,2> A{1,2, 3,4};
  cartesian_tensor<int, 2,2> B{5,6, 7,8};
  // 1*5 + 2*6 + 3*7 + 4*8 = 70
  auto s = double_contraction(A, B);
  REQUIRE(s == 70);
}

TEST_CASE("outer_numeric (vector ⊗ vector)", "[tensor][vector]") {
  cartesian_vector<int> a{1,2,3};
  cartesian_vector<int> b{4,5,6};
  auto T = outer_numeric(a, b); // 3x3
  REQUIRE(T(0,0) ==  4); REQUIRE(T(0,1) ==  5); REQUIRE(T(0,2) ==  6);
  REQUIRE(T(1,0) ==  8); REQUIRE(T(1,1) == 10); REQUIRE(T(1,2) == 12);
  REQUIRE(T(2,0) == 12); REQUIRE(T(2,1) == 15); REQUIRE(T(2,2) == 18);
}

#if MP_UNITS_HOSTED
TEST_CASE("text output (ostream + fmt)", "[tensor][fmt][ostream]") {
  cartesian_tensor<int,2,2> A{1,2,3,4};

  std::ostringstream os;
  os << A;
  CHECK(os.str() == "[[1, 2]\n [3, 4]]");

  CHECK(MP_UNITS_STD_FMT::format("{}", A) == os.str());
}
#endif

TEST_CASE("constexpr basics", "[tensor][constexpr]") {
  constexpr cartesian_tensor<int, 1,3> A{1,2,3};
  constexpr cartesian_tensor<int, 1,3> B{4,5,6};
  constexpr auto C = A + B;
  static_assert(C(0,0) == 5 && C(0,1) == 7 && C(0,2) == 9);
  (void)C;
}
