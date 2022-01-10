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

namespace units {

// A set of non-standard bases for testing purposes.
struct noninteger_base { static constexpr long double value = 1.234L; };
struct noncanonical_two_base { static constexpr long double value = 2.0L; };
struct other_noncanonical_two_base { static constexpr long double value = 2.0L; };
struct invalid_zero_base { static constexpr long double value = 0.0L; };
struct invalid_negative_base { static constexpr long double value = -1.234L; };

TEST_CASE("base_power")
{
  SECTION("base rep deducible for integral base")
  {
    CHECK(base_power{2} == base_power<int>{2, ratio{1}});
    CHECK(base_power{2, 3} == base_power<int>{2, ratio{3}});
    CHECK(base_power{2, ratio{3, 4}} == base_power<int>{2, ratio{3, 4}});
  }

  SECTION("get_base retrieves base for integral base")
  {
    CHECK(base_power{2}.get_base() == 2);
    CHECK(base_power{3, 5}.get_base() == 3);
    CHECK(base_power{5, ratio{1, 3}}.get_base() == 5);
  }

  SECTION("get_base retrieves member value for non-integer base")
  {
    CHECK(base_power<noninteger_base>{}.get_base() == 1.234L);
    CHECK(base_power<noninteger_base>{2}.get_base() == 1.234L);
    CHECK(base_power<noninteger_base>{ratio{5, 8}}.get_base() == 1.234L);
  }

  SECTION("same-base values not equal if types are different")
  {
    const auto a = base_power<noncanonical_two_base>{};
    const auto b = base_power{2};
    const auto c = base_power<other_noncanonical_two_base>{};

    REQUIRE(a.get_base() == b.get_base());
    CHECK(a != b);

    REQUIRE(a.get_base() == c.get_base());
    CHECK(a != c);
  }

  SECTION("same-type values not equal if bases are different")
  {
    CHECK(base_power{2} != base_power{3});
    CHECK(base_power{2, ratio{5, 4}} != base_power{3, ratio{5, 4}});
  }

  SECTION("same-type, same-base values not equal if powers are different")
  {
    CHECK(base_power{2} != base_power{2, 2});
    CHECK(base_power<pi_base>{} != base_power<pi_base>{ratio{1, 3}});
  }

  SECTION("product with inverse equals identity")
  {
    auto check_product_with_inverse_is_identity = [] (auto x) {
      CHECK(x * pow<-1>(x) == as_magnitude<1>());
    };

    check_product_with_inverse_is_identity(as_magnitude<3>());
    check_product_with_inverse_is_identity(as_magnitude<ratio{4, 17}>());
    check_product_with_inverse_is_identity(pi_to_the<ratio{-22, 7}>());
  }

  SECTION("pow() multiplies exponent")
  {
    CHECK(pow<0>(base_power{2}) == base_power{2, 0});
    CHECK(pow<ratio{-1, 2}>(base_power{2, 3}) == base_power{2, ratio{-3, 2}});
    CHECK(pow<ratio{1, 3}>(base_power<pi_base>{ratio{3, 2}}) == base_power<pi_base>{ratio{1, 2}});
  }
}

TEST_CASE("make_ratio performs prime factorization correctly")
{
  SECTION("Performs prime factorization when denominator is 1")
  {
    CHECK(as_magnitude<1>() == magnitude<>{});
    CHECK(as_magnitude<2>() == magnitude<base_power{2}>{});
    CHECK(as_magnitude<3>() == magnitude<base_power{3}>{});
    CHECK(as_magnitude<4>() == magnitude<base_power{2, 2}>{});

    CHECK(as_magnitude<792>() == magnitude<base_power{2, 3}, base_power{3, 2}, base_power{11}>{});
  }

  SECTION("Supports fractions")
  {
    CHECK(as_magnitude<ratio{5, 8}>() == magnitude<base_power{2, -3}, base_power{5}>{});
  }

  SECTION("Supports nonzero exp")
  {
    constexpr ratio r{3, 1, 2};
    REQUIRE(r.exp == 2);
    CHECK(as_magnitude<r>() == as_magnitude<300>());
  }
}

TEST_CASE("Equality works for magnitudes")
{
  SECTION("Equivalent ratios are equal")
  {
    CHECK(as_magnitude<1>() == as_magnitude<1>());
    CHECK(as_magnitude<3>() == as_magnitude<3>());
    CHECK(as_magnitude<ratio{3, 4}>() == as_magnitude<ratio{9, 12}>());
  }

  SECTION("Different ratios are unequal")
  {
    CHECK(as_magnitude<3>() != as_magnitude<5>());
    CHECK(as_magnitude<3>() != as_magnitude<ratio{3, 2}>());
  }

  SECTION("Supports constexpr")
  {
    constexpr auto eq = (as_magnitude<ratio{4, 5}>() == as_magnitude<ratio{4, 3}>());
    CHECK(!eq);
  }
}

TEST_CASE("Multiplication works for magnitudes")
{
  SECTION("Reciprocals reduce to null magnitude")
  {
    CHECK(as_magnitude<ratio{3, 4}>() * as_magnitude<ratio{4, 3}>() == as_magnitude<1>());
  }

  SECTION("Products work as expected")
  {
    CHECK(as_magnitude<ratio{4, 5}>() * as_magnitude<ratio{4, 3}>() == as_magnitude<ratio{16, 15}>());
  }

  SECTION("Products handle pi correctly")
  {
     CHECK(
         pi_to_the<1>() * as_magnitude<ratio{2, 3}>() * pi_to_the<ratio{-1, 2}>() ==
         magnitude<base_power{2}, base_power{3, -1}, base_power<pi_base>{ratio{1, 2}}>{});
  }

  SECTION("Supports constexpr")
  {
    constexpr auto p = as_magnitude<ratio{4, 5}>() * as_magnitude<ratio{4, 3}>();
    CHECK(p == as_magnitude<ratio{16, 15}>());
  }
}

TEST_CASE("Division works for magnitudes")
{
  SECTION("Dividing anything by itself reduces to null magnitude")
  {
    CHECK(as_magnitude<ratio{3, 4}>() / as_magnitude<ratio{3, 4}>() == as_magnitude<1>());
    CHECK(as_magnitude<15>() / as_magnitude<15>() == as_magnitude<1>());
  }

  SECTION("Quotients work as expected")
  {
    CHECK(as_magnitude<ratio{4, 5}>() / as_magnitude<ratio{4, 3}>() == as_magnitude<ratio{3, 5}>());
  }

  SECTION("Supports constexpr")
  {
    constexpr auto q = as_magnitude<ratio{4, 5}>() / as_magnitude<ratio{4, 3}>();
    CHECK(q == as_magnitude<ratio{3, 5}>());
  }
}

TEST_CASE("Can raise Magnitudes to rational powers")
{
  SECTION("Anything to the 0 is 1") {
    CHECK(pow<0>(as_magnitude<1>()) == as_magnitude<1>());
    CHECK(pow<0>(as_magnitude<123>()) == as_magnitude<1>());
    CHECK(pow<0>(as_magnitude<ratio{3, 4}>()) == as_magnitude<1>());
    CHECK(pow<0>(pi_to_the<ratio{-1, 2}>()) == as_magnitude<1>());
  }

  SECTION("Anything to the 1 is itself") {
    CHECK(pow<1>(as_magnitude<1>()) == as_magnitude<1>());
    CHECK(pow<1>(as_magnitude<123>()) == as_magnitude<123>());
    CHECK(pow<1>(as_magnitude<ratio{3, 4}>()) == as_magnitude<ratio{3, 4}>());
    CHECK(pow<1>(pi_to_the<ratio{-1, 2}>()) == pi_to_the<ratio{-1, 2}>());
  }

  SECTION("Can raise to arbitrary rational power") {
    CHECK(pow<ratio{-8, 3}>(pi_to_the<ratio{-1, 2}>()) == pi_to_the<ratio{4, 3}>());
  }
}

namespace detail {

TEST_CASE("Prime helper functions")
{
  SECTION("find_first_factor()") {
    CHECK(find_first_factor(1) == 1);
    CHECK(find_first_factor(2) == 2);
    CHECK(find_first_factor(4) == 2);
    CHECK(find_first_factor(6) == 2);
    CHECK(find_first_factor(15) == 3);
    CHECK(find_first_factor(17) == 17);
  }

  SECTION("multiplicity") {
    CHECK(multiplicity(2, 8) == 3);
    CHECK(multiplicity(2, 1024) == 10);
    CHECK(multiplicity(11, 6655) == 3);
  }

  SECTION("remove_power()") {
    CHECK(remove_power(17, 0, 5) == 5);
    CHECK(remove_power(2, 3, 24) == 3);
    CHECK(remove_power(11, 3, 6655) == 5);
  }
}

TEST_CASE("Prime factorization")
{
  SECTION("1 factors into the null magnitude")
  {
    CHECK(prime_factorization_v<1> == magnitude<>{});
  }

  SECTION("Prime numbers factor into themselves")
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

TEST_CASE("is_valid_base_power")
{
  SECTION("0 power is invalid") {
    REQUIRE(is_valid_base_power(base_power{2}));
    CHECK(!is_valid_base_power(base_power{2, 0}));

    REQUIRE(is_valid_base_power(base_power{41}));
    CHECK(!is_valid_base_power(base_power{41, 0}));

    REQUIRE(is_valid_base_power(base_power<pi_base>{}));
    CHECK(!is_valid_base_power(base_power<pi_base>{0}));
  }

  SECTION("non-prime integers are invalid") {
    CHECK(!is_valid_base_power(base_power{-8}));
    CHECK(!is_valid_base_power(base_power{0}));
    CHECK(!is_valid_base_power(base_power{1}));

    CHECK(is_valid_base_power(base_power{2}));
    CHECK(is_valid_base_power(base_power{3}));

    CHECK(!is_valid_base_power(base_power{4}));
  }

  SECTION("non-positive floating point bases are invalid") {
    CHECK(!is_valid_base_power(base_power<invalid_zero_base>{}));
    CHECK(!is_valid_base_power(base_power<invalid_negative_base>{}));
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

TEST_CASE("strictly_increasing")
{
  SECTION("Empty input is sorted")
  {
    CHECK(strictly_increasing());
  }

  SECTION("Single-element input is sorted")
  {
    CHECK(strictly_increasing(3));
    CHECK(strictly_increasing(15.42));
    CHECK(strictly_increasing('c'));
  }

  SECTION("Multi-value inputs compare correctly")
  {
    CHECK(strictly_increasing(3, 3.14));
    CHECK(!strictly_increasing(3, 3.0));
    CHECK(!strictly_increasing(4, 3.0));
  }
}

} // namespace detail

} // namespace units
