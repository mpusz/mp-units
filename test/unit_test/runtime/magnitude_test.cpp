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

#include <units/magnitude.h>
#include <units/ratio.h>
#include <catch2/catch.hpp>
#include <type_traits>

using namespace units;
using namespace units::mag;

TEST_CASE("Magnitude is invertible")
{
  CHECK(std::is_same_v<inverse_t<magnitude<>>, magnitude<>>);
  CHECK(std::is_same_v<
      inverse_t<magnitude<int_base_power<2>>>, magnitude<int_base_power<2, -1>>>);
  CHECK(std::is_same_v<
      inverse_t<magnitude<int_base_power<3, 1, 2>, int_base_power<11, -5>>>,
      magnitude<int_base_power<3, -1, 2>, int_base_power<11, 5>>>);
}

TEST_CASE("Magnitude supports products")
{
  SECTION ("The nullary product gives the unit magnitude") {
    CHECK(std::is_same_v<product_t<>, magnitude<>>);
  }

  SECTION ("The unary product is the identity operation") {
    CHECK(std::is_same_v<
        product_t<magnitude<int_base_power<3, 4>>>,
        magnitude<int_base_power<3, 4>>>);

    CHECK(std::is_same_v<
        product_t<magnitude<int_base_power<2, -1, 3>, int_base_power<13, -2>>>,
        magnitude<int_base_power<2, -1, 3>, int_base_power<13, -2>>>);
  }

  SECTION ("Binary product with null magnitude is identity") {
    using arbitrary_mag = magnitude<int_base_power<11, 3, 2>>;
    CHECK(std::is_same_v<product_t<magnitude<>, magnitude<>>, magnitude<>>);
    CHECK(std::is_same_v<product_t<arbitrary_mag, magnitude<>>, arbitrary_mag>);
    CHECK(std::is_same_v<product_t<magnitude<>, arbitrary_mag>, arbitrary_mag>);
  }

  SECTION ("Binary product with distinct bases maintains sorted order") {
    CHECK(std::is_same_v<
        product_t<
          magnitude<int_base_power<2, 1, 3>, int_base_power<7, -2>>,
          magnitude<int_base_power<3>, int_base_power<5, 5>>>,
        magnitude<
          int_base_power<2, 1, 3>,
          int_base_power<3>,
          int_base_power<5, 5>,
          int_base_power<7, -2>>>);
  }

  SECTION ("Binary products add exponents for same bases") {
    CHECK(std::is_same_v<
        product_t<
          magnitude<int_base_power<2, 3>>,
          magnitude<int_base_power<2, -5>>>,
        magnitude<int_base_power<2, -2>>>);
    CHECK(std::is_same_v<
        product_t<
          magnitude<int_base_power<2, 3>, int_base_power<3, -1, 3>>,
          magnitude<int_base_power<2, -5>, int_base_power<5, 4>>>,
        magnitude<int_base_power<2, -2>, int_base_power<3, -1, 3>, int_base_power<5, 4>>>);
  }

  SECTION ("Binary products omit bases whose exponents cancel") {
    CHECK(std::is_same_v<
        product_t<
          magnitude<int_base_power<2, 1, 3>>, magnitude<int_base_power<2, -1, 3>>>,
        magnitude<>>);
    CHECK(std::is_same_v<
        product_t<
          magnitude<int_base_power<2, 1, 3>, int_base_power<7, -2>>,
          magnitude<int_base_power<2, -1, 3>, int_base_power<5, 5>>>,
        magnitude<int_base_power<5, 5>, int_base_power<7, -2>>>);
    CHECK(std::is_same_v<
        product_t<
          magnitude<int_base_power<2, 1, 3>, int_base_power<3, -2>, int_base_power<7, -2>>,
          magnitude<int_base_power<2, -1, 3>, int_base_power<5, 5>, int_base_power<7, 2>>>,
        magnitude<int_base_power<3, -2>, int_base_power<5, 5>>>);
  }

  SECTION ("N-ary products recurse") {
    CHECK(std::is_same_v<
        product_t<
          magnitude<int_base_power<2, 1, 3>>,
          magnitude<int_base_power<2, 2, 3>>,
          magnitude<int_base_power<3, -4>>,
          magnitude<int_base_power<5>>,
          magnitude<int_base_power<2, -1>>>,
        magnitude<int_base_power<3, -4>, int_base_power<5>>>);
  }
}

// TEST_CASE("Ratio shortcut performs prime factorization")
// {
//   // CHECK(std::is_same_v<ratio<>, magnitude<>>);
//   // CHECK(std::is_same_v<ratio<2>, magnitude<int_base_power<2>>>);
// }
// 
// TEST_CASE("Equality works for ratios")
// {
//   CHECK(ratio<>{} == ratio<>{});
//   CHECK(ratio<3>{} == ratio<3>{});
// 
//   // CHECK(ratio<3>{} != ratio<4>{});
// }
