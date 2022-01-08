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

namespace units::mag
{

TEST_CASE("strictly_increasing")
{
  SECTION ("Empty input is sorted")
  {
    CHECK(strictly_increasing());
  }

  SECTION ("Single-element input is sorted")
  {
    CHECK(strictly_increasing(3));
    CHECK(strictly_increasing(15.42));
    CHECK(strictly_increasing('c'));
  }

  SECTION ("Multi-value inputs compare correctly")
  {
    CHECK(strictly_increasing(3, 3.14));
    CHECK(!strictly_increasing(3, 3.0));
    CHECK(!strictly_increasing(4, 3.0));
  }
}

// TEST_CASE("make_ratio performs prime factorization correctly")
// {
//   SECTION("Performs prime factorization when denominator is 1")
//   {
//     CHECK(std::is_same_v<decltype(make_ratio<1>()), magnitude<>>);
//     CHECK(std::is_same_v<decltype(make_ratio<2>()), magnitude<base_power{2}>>);
//     CHECK(std::is_same_v<decltype(make_ratio<3>()), magnitude<base_power{3}>>);
//     CHECK(std::is_same_v<decltype(make_ratio<4>()), magnitude<base_power{2, 2}>>);
// 
//     CHECK(std::is_same_v<
//         decltype(make_ratio<792>()),
//         magnitude<base_power{2, 3}, base_power{3, 2}, base_power{11}>>);
//   }
// 
//   SECTION("Reduces fractions to lowest terms")
//   {
//     CHECK(std::is_same_v<decltype(make_ratio<8, 8>()), magnitude<>>);
//     CHECK(std::is_same_v<
//         decltype(make_ratio<50, 80>()), magnitude<base_power{2, -3}, base_power{5}>>);
//   }
// }

// TEST_CASE("Equality works for magnitudes")
// {
//   SECTION("Equivalent ratios are equal")
//   {
//     CHECK(make_ratio<1>() == make_ratio<1>());
//     CHECK(make_ratio<3>() == make_ratio<3>());
//     CHECK(make_ratio<3, 4>() == make_ratio<9, 12>());
//   }
// 
//   SECTION("Different ratios are unequal")
//   {
//     CHECK(make_ratio<3>() != make_ratio<5>());
//     CHECK(make_ratio<3>() != make_ratio<3, 2>());
//   }
// 
//   SECTION("Supports constexpr")
//   {
//     constexpr auto eq = (make_ratio<4, 5>() == make_ratio<4, 3>());
//     CHECK(!eq);
//   }
// }

TEST_CASE("Multiplication works for magnitudes")
{
  SECTION("Reciprocals reduce to null magnitude")
  {
    CHECK(make_ratio<3, 4>() * make_ratio<4, 3>() == make_ratio<1>());
  }

  SECTION("Products work as expected")
  {
    CHECK(make_ratio<4, 5>() * make_ratio<4, 3>() == make_ratio<16, 15>());
  }

  //SECTION("Products handle pi correctly")
  //{
  //   CHECK(
  //       pi_to_the<1>() * make_ratio<2, 3>() * pi_to_the<ratio{-1, 2}>() ==
  //       magnitude<base_power{2}, base_power{3, -1}, pi_power<ratio{1, 2}>()>{});
  //}

  SECTION("Supports constexpr")
  {
    constexpr auto p = make_ratio<4, 5>() * make_ratio<4, 3>();
    CHECK(p == make_ratio<16, 15>());
  }
}

TEST_CASE("Division works for magnitudes")
{
  SECTION("Dividing anything by itself reduces to null magnitude")
  {
    CHECK(make_ratio<3, 4>() / make_ratio<3, 4>() == make_ratio<1>());
    CHECK(make_ratio<15>() / make_ratio<15>() == make_ratio<1>());
  }

  SECTION("Quotients work as expected")
  {
    CHECK(make_ratio<4, 5>() / make_ratio<4, 3>() == make_ratio<3, 5>());
  }

  SECTION("Supports constexpr")
  {
    constexpr auto q = make_ratio<4, 5>() / make_ratio<4, 3>();
    CHECK(q == make_ratio<3, 5>());
  }
}

namespace detail
{

TEST_CASE("Prime factorization")
{
  SECTION ("1 factors into the null magnitude")
  {
    CHECK(prime_factorization_v<1> == magnitude<>{});
  }

  SECTION ("Prime numbers factor into themselves")
  {
    CHECK(prime_factorization_v<2> == magnitude<base_power{2}>{});
    CHECK(prime_factorization_v<3> == magnitude<base_power{3}>{});
    CHECK(prime_factorization_v<5> == magnitude<base_power{5}>{});
    CHECK(prime_factorization_v<7> == magnitude<base_power{7}>{});
    CHECK(prime_factorization_v<11> == magnitude<base_power{11}>{});

    CHECK(prime_factorization_v<41> == magnitude<base_power{41}>{});
  }

  SECTION("Prime factorization finds factors and multiplicities")
  {
    CHECK(prime_factorization_v<792> ==
          magnitude<base_power{2, 3}, base_power{3, 2}, base_power{11}>{});
  }
}

TEST_CASE("is_prime detects primes")
{
  SECTION("Non-positive numbers are not prime")
  {
    CHECK(!is_prime(-1328));
    CHECK(!is_prime(-1));
    CHECK(!is_prime(0));
  }

  SECTION("1 is not prime")
  {
    CHECK(!is_prime(1));
  }

  SECTION("Discriminates between primes and non-primes")
  {
    CHECK(is_prime(2));
    CHECK(is_prime(3));
    CHECK(!is_prime(4));
    CHECK(is_prime(5));
    CHECK(!is_prime(6));
    CHECK(is_prime(7));
    CHECK(!is_prime(8));
    CHECK(!is_prime(9));

    CHECK(is_prime(7919));
  }
}

TEST_CASE("pairwise_all evaluates all pairs")
{
  SECTION("always true for empty tuples")
  {
    CHECK(pairwise_all{[](auto, auto){ return true; }}());
    CHECK(pairwise_all{[](auto, auto){ return false; }}());
  }

  SECTION("always true for single-element tuples")
  {
    CHECK(pairwise_all{[](auto, auto){ return true; }}(1));
    CHECK(pairwise_all{[](auto, auto){ return false; }}(3.14));
    CHECK(pairwise_all{[](auto, auto){ return true; }}('x'));
  }

  SECTION("true for longer tuples iff true for all neighbouring pairs")
  {
    CHECK(pairwise_all{std::less{}}(1, 1.5));
    CHECK(pairwise_all{std::less{}}(1, 1.5, 2));
    CHECK(!pairwise_all{std::less{}}(1, 2.0, 2));
    CHECK(!pairwise_all{std::less{}}(1, 2.5, 2));
  }
}

} // namespace detail

} // namespace units::mag
