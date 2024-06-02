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

#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <units/bits/magnitude.h>
#include <units/bits/ratio.h>
#include <type_traits>
#endif

using namespace units;
using namespace units::detail;

template<>
inline constexpr std::optional<std::intmax_t> units::known_first_factor<9223372036854775783> = 9223372036854775783;

namespace {

// A set of non-standard bases for testing purposes.
// struct noninteger_base {
//   static constexpr long double value = 1.234L;
// };
// struct noncanonical_two_base {
//   static constexpr long double value = 2.0L;
// };
// struct other_noncanonical_two_base {
//   static constexpr long double value = 2.0L;
// };
// struct invalid_zero_base {
//   static constexpr long double value = 0.0L;
// };
// struct invalid_negative_base {
//   static constexpr long double value = -1.234L;
// };

// template<typename T, typename U>
// void check_same_type_and_value(T actual, U expected)
// {
//   CHECK(std::is_same_v<T, U>);
//   CHECK(actual == expected);
// }

// template<ratio R>
// void check_ratio_round_trip_is_identity()
// {
//   constexpr Magnitude auto m = mag<R>();
//   constexpr ratio round_trip = ratio{
//     get_value<std::intmax_t>(numerator(m)),
//     get_value<std::intmax_t>(denominator(m)),
//   };
//   CHECK(round_trip == R);
// }

inline constexpr struct mag_2_ : magnitude<2> {
} mag_2;
// inline constexpr struct mag_2_other : magnitude<2> {
// } mag_2_other;
// inline constexpr struct mag_3 : magnitude<2> {
// } mag_3;

// concepts verification
static_assert(Magnitude<decltype(mag<2>)>);
static_assert(Magnitude<mag_2_>);

// is_named_magnitude
static_assert(!is_named_magnitude<decltype(mag<2>)>);
static_assert(is_named_magnitude<mag_2_>);

// power_v
template<template<auto, int, int...> typename P>
concept invalid_power_v = requires {
  requires !requires { typename P<123, 0>; };
  requires !requires { typename P<123, 0, 2>; };
  requires !requires { typename P<123, 1, 0>; };
  requires !requires { typename P<123, 0, 0>; };
  requires !requires { typename P<123, 1>; };
  requires !requires { typename P<123, 1, 1>; };
  requires !requires { typename P<123, 5, 5>; };
};
static_assert(invalid_power_v<power_v>);

// get_base
// get_base retrieves value for integral base
static_assert(get_base(2) == 2);
static_assert(get_base(power_v<3, 5>{}) == 3);
static_assert(get_base(power_v<5, 1, 3>{}) == 5);

// get_base retrieves value for integral base
static_assert(get_base(2) == 2);
static_assert(get_base(power_v<3, 5>{}) == 3);
static_assert(get_base(power_v<5, 1, 3>{}) == 5);

// get_base retrieves named magnitude for named base
static_assert(std::is_same_v<decltype(get_base(mag_2)), mag_2_>);
static_assert(std::is_same_v<decltype(get_base(power_v<mag_2, 2>{})), mag_2_>);
static_assert(std::is_same_v<decltype(get_base(power_v<mag_2, 5, 8>{})), mag_2_>);


// equality
// static_assert(mag_2 == mag_2);
// static_assert(mag_2 != mag_3);
// static_assert(mag_2 != mag_2_other);

// {
//   const auto a = base_power<noncanonical_two_base>{};
//   const auto b = base_power{2};
//   const auto c = base_power<other_noncanonical_two_base>{};

//   REQUIRE(a.get_base() == b.get_base());
//   CHECK(a != b);

//   REQUIRE(a.get_base() == c.get_base());
//   CHECK(a != c);
// }

// SECTION("same-type values not equal if bases are different")
// {
//   CHECK(base_power{2} != base_power{3});
//   CHECK(base_power{2, ratio{5, 4}} != base_power{3, ratio{5, 4}});
// }

// SECTION("same-type, same-base values not equal if powers are different")
// {
//   CHECK(base_power{2} != base_power{2, 2});
//   CHECK(base_power<pi_base>{} != base_power<pi_base>{ratio{1, 3}});
// }

// SECTION("product with inverse equals identity")
// {
//   auto check_product_with_inverse_is_identity = [](auto x) { CHECK(x * pow<-1>(x) == mag<1>()); };

//   check_product_with_inverse_is_identity(mag<3>());
//   check_product_with_inverse_is_identity(mag_ratio<4, 17>());
//   check_product_with_inverse_is_identity(pi_to_the<ratio{-22, 7}>());
// }

// SECTION("pow() multiplies exponent")
// {
//   CHECK(pow(base_power{2}, 0) == base_power{2, 0});
//   CHECK(pow(base_power{2, 3}, ratio{-1, 2}) == base_power{2, ratio{-3, 2}});
//   CHECK(pow(base_power<pi_base>{ratio{3, 2}}, ratio{1, 3}) == base_power<pi_base>{ratio{1, 2}});
// }

// TEST_CASE("make_ratio performs prime factorization correctly")
// {
//   SECTION("Performs prime factorization when denominator is 1")
//   {
//     CHECK(mag<1>() == magnitude<>{});
//     CHECK(mag<2>() == magnitude<base_power{2}>{});
//     CHECK(mag<3>() == magnitude<base_power{3}>{});
//     CHECK(mag<4>() == magnitude<base_power{2, 2}>{});

//     CHECK(mag<792>() == magnitude<base_power{2, 3}, base_power{3, 2}, base_power{11}>{});
//   }

//   SECTION("Supports fractions") { CHECK(mag_ratio<5, 8>() == magnitude<base_power{2, -3}, base_power{5}>{}); }

//   SECTION("Can handle prime factor which would be large enough to overflow int")
//   {
//     // This was taken from a case which failed when we used `int` for our base to store prime numbers.
//     // The failure was due to a prime factor which is larger than 2^31.
//     mag_ratio<16'605'390'666'050, 10'000'000'000'000>();
//   }

//   SECTION("Can bypass computing primes by providing known_first_factor<N>")
//   {
//     // Sometimes, even wheel factorization isn't enough to handle the compilers' limits on constexpr steps and/or
//     // iterations.  To work around these cases, we can explicitly provide the correct answer directly to the
//     compiler.
//     //
//     // In this case, we test that we can represent the largest prime that fits in a signed 64-bit int.  The reason
//     this
//     // test can pass is that we have provided the answer, by specializing the `known_first_factor` variable template
//     // above in this file.
//     mag<9'223'372'036'854'775'783>();
//   }
// }

// TEST_CASE("magnitude converts to numerical value")
// {
//   SECTION("Positive integer powers of integer bases give integer values")
//   {
//     constexpr auto mag_412 = mag<412>();
//     check_same_type_and_value(get_value<int>(mag_412), 412);
//     check_same_type_and_value(get_value<std::size_t>(mag_412), std::size_t{412});
//     check_same_type_and_value(get_value<float>(mag_412), 412.0f);
//     check_same_type_and_value(get_value<double>(mag_412), 412.0);
//   }

//   SECTION("Negative integer powers of integer bases compute correct values")
//   {
//     constexpr auto mag_0p125 = mag_ratio<1, 8>();
//     check_same_type_and_value(get_value<float>(mag_0p125), 0.125f);
//     check_same_type_and_value(get_value<double>(mag_0p125), 0.125);
//   }

//   SECTION("pi to the 1 supplies correct values")
//   {
//     check_same_type_and_value(get_value<float>(mag_pi), std::numbers::pi_v<float>);
//     check_same_type_and_value(get_value<double>(mag_pi), std::numbers::pi_v<double>);
//     check_same_type_and_value(get_value<long double>(mag_pi), std::numbers::pi_v<long double>);
//   }

//   SECTION("pi to arbitrary power performs computations in most accurate type at compile time")
//   {
//     if constexpr (sizeof(float) < sizeof(long double)) {
//       constexpr auto pi_cubed = pi_to_the<3>();

//       auto cube = [](auto x) { return x * x * x; };
//       constexpr auto via_float = cube(std::numbers::pi_v<float>);
//       constexpr auto via_long_double = static_cast<float>(cube(std::numbers::pi_v<long double>));

//       constexpr auto pi_cubed_value = get_value<float>(pi_cubed);
//       REQUIRE(pi_cubed_value != via_float);
//       CHECK(pi_cubed_value == via_long_double);
//     }
//   }

//   SECTION("Impossible requests are prevented at compile time")
//   {
//     // Naturally, we cannot actually write a test to verify a compiler error.  But any of these can
//     // be uncommented if desired to verify that it breaks the build.

//     // get_value<int8_t>(mag<412>());

//     // Would work for pow<62>:
//     // get_value<int64_t>(pow<63>(mag<2>()));

//     // Would work for pow<63>:
//     // get_value<uint64_t>(pow<64>(mag<2>()));

//     get_value<double>(pow<308>(mag<10>()));  // Compiles, correctly.
//     // get_value<double>(pow<309>(mag<10>()));
//     // get_value<double>(pow<3099>(mag<10>()));
//     // get_value<double>(pow<3099999>(mag<10>()));

//     auto sqrt_2 = pow<ratio{1, 2}>(mag<2>());
//     CHECK(!is_integral(sqrt_2));
//     // get_value<int>(sqrt_2);
//   }
// }

// TEST_CASE("Equality works for magnitudes")
// {
//   SECTION("Equivalent ratios are equal")
//   {
//     CHECK(mag<1>() == mag<1>());
//     CHECK(mag<3>() == mag<3>());
//     CHECK(mag_ratio<3, 4>() == mag_ratio<9, 12>());
//   }

//   SECTION("Different ratios are unequal")
//   {
//     CHECK(mag<3>() != mag<5>());
//     CHECK(mag<3>() != mag_ratio<3, 2>());
//   }

//   SECTION("Supports constexpr")
//   {
//     constexpr auto eq = (mag_ratio<4, 5>() == mag_ratio<4, 3>());
//     CHECK(!eq);
//   }
// }

// TEST_CASE("Multiplication works for magnitudes")
// {
//   SECTION("Reciprocals reduce to null magnitude") { CHECK(mag_ratio<3, 4>() * mag_ratio<4, 3>() == mag<1>()); }

//   SECTION("Products work as expected") { CHECK(mag_ratio<4, 5>() * mag_ratio<4, 3>() == mag_ratio<16, 15>()); }

//   SECTION("Products handle pi correctly")
//   {
//     CHECK(pi_to_the<1>() * mag_ratio<2, 3>() * pi_to_the<ratio{-1, 2}>() ==
//           magnitude<base_power{2}, base_power{3, -1}, base_power<pi_base>{ratio{1, 2}}>{});
//   }

//   SECTION("Supports constexpr")
//   {
//     constexpr auto p = mag_ratio<4, 5>() * mag_ratio<4, 3>();
//     CHECK(p == mag_ratio<16, 15>());
//   }
// }

// TEST_CASE("Common Magnitude")
// {
//   SECTION("Identity for identical magnitudes")
//   {
//     CHECK(common_magnitude(mag<1>(), mag<1>()) == mag<1>());
//     CHECK(common_magnitude(mag<15>(), mag<15>()) == mag<15>());
//     CHECK(common_magnitude(pi_to_the<ratio{3, 4}>(), pi_to_the<ratio{3, 4}>()) == pi_to_the<ratio{3, 4}>());
//   }

//   SECTION("Greatest Common Factor for integers")
//   {
//     CHECK(common_magnitude(mag<24>(), mag<36>()) == mag<12>());
//     CHECK(common_magnitude(mag<24>(), mag<37>()) == mag<1>());
//   }

//   SECTION("Handles fractions")
//   {
//     CHECK(common_magnitude(mag_ratio<3, 8>(), mag_ratio<5, 6>()) == mag_ratio<1, 24>());
//   }
// }

// TEST_CASE("Division works for magnitudes")
// {
//   SECTION("Dividing anything by itself reduces to null magnitude")
//   {
//     CHECK(mag_ratio<3, 4>() / mag_ratio<3, 4>() == mag<1>());
//     CHECK(mag<15>() / mag<15>() == mag<1>());
//   }

//   SECTION("Quotients work as expected") { CHECK(mag_ratio<4, 5>() / mag_ratio<4, 3>() == mag_ratio<3, 5>()); }

//   SECTION("Supports constexpr")
//   {
//     constexpr auto q = mag_ratio<4, 5>() / mag_ratio<4, 3>();
//     CHECK(q == mag_ratio<3, 5>());
//   }
// }

// TEST_CASE("Can raise Magnitudes to rational powers")
// {
//   SECTION("Anything to the 0 is 1")
//   {
//     CHECK(pow<0>(mag<1>()) == mag<1>());
//     CHECK(pow<0>(mag<123>()) == mag<1>());
//     CHECK(pow<0>(mag_ratio<3, 4>()) == mag<1>());
//     CHECK(pow<0>(pi_to_the<ratio{-1, 2}>()) == mag<1>());
//   }

//   SECTION("Anything to the 1 is itself")
//   {
//     CHECK(pow<1>(mag<1>()) == mag<1>());
//     CHECK(pow<1>(mag<123>()) == mag<123>());
//     CHECK(pow<1>(mag_ratio<3, 4>()) == mag_ratio<3, 4>());
//     CHECK(pow<1>(pi_to_the<ratio{-1, 2}>()) == pi_to_the<ratio{-1, 2}>());
//   }

//   SECTION("Can raise to arbitrary rational power")
//   {
//     CHECK(pow<ratio{-8, 3}>(pi_to_the<ratio{-1, 2}>()) == pi_to_the<ratio{4, 3}>());
//   }
// }

// TEST_CASE("can distinguish integral, rational, and irrational magnitudes")
// {
//   SECTION("Integer magnitudes are integral and rational")
//   {
//     auto check_rational_and_integral = [](Magnitude auto m) {
//       CHECK(is_integral(m));
//       CHECK(is_rational(m));
//     };
//     check_rational_and_integral(magnitude<>{});
//     check_rational_and_integral(mag<1>());
//     check_rational_and_integral(mag<3>());
//     check_rational_and_integral(mag<8>());
//     check_rational_and_integral(mag<412>());
//     check_rational_and_integral(mag_ratio<1, 1>());
//   }

//   SECTION("Fractional magnitudes are rational, but not integral")
//   {
//     auto check_rational_but_not_integral = [](Magnitude auto m) {
//       CHECK(!is_integral(m));
//       CHECK(is_rational(m));
//     };
//     check_rational_but_not_integral(mag_ratio<1, 2>());
//     check_rational_but_not_integral(mag_ratio<5, 8>());
//   }
// }

// TEST_CASE("Constructing ratio from rational magnitude")
// {
//   SECTION("Round trip is identity")
//   {
//     // Note that not every Magnitude can be represented as a ratio.  However, if we _start_ with a
//     // ratio, we must guarantee to recover the same ratio in a round trip.
//     check_ratio_round_trip_is_identity<1>();
//     check_ratio_round_trip_is_identity<9>();
//     check_ratio_round_trip_is_identity<ratio{5, 8}>();
//   }

//   SECTION("Rational magnitude converts to ratio")
//   {
//     constexpr ratio r = as_ratio(mag_ratio<22, 7>());
//     CHECK(r == ratio{22, 7});
//   }

//   SECTION("Irrational magnitude does not convert to ratio")
//   {
//     // The following code should not compile.
//     // as_ratio(pow<ratio{1, 2}>(mag<2>()));

//     // The following code should not compile.
//     // as_ratio(mag<180>() / pi_to_the<1>());
//   }
// }

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// // Detail function tests below.

// TEST_CASE("int_power computes integer powers")
// {
//   SECTION("handles floating point")
//   {
//     check_same_type_and_value(int_power(0.123L, 0), 1.0L);
//     check_same_type_and_value(int_power(0.246f, 1), 0.246f);
//     check_same_type_and_value(int_power(0.5f, 3), 0.125f);
//     check_same_type_and_value(int_power(2.5, 4), 39.0625);

//     CHECK(std::is_same_v<long double, decltype(compute_base_power<double>(base_power{10, 20}))>);
//   }

//   SECTION("handles integral")
//   {
//     check_same_type_and_value(int_power(8, 0), 1);
//     check_same_type_and_value(int_power(9L, 1), 9L);
//     check_same_type_and_value(int_power(2, 10), 1024);
//   }
// }

// TEST_CASE("integer_part picks out integer part of single-basis magnitude")
// {
//   SECTION("integer_part of non-integer base is identity magnitude")
//   {
//     CHECK(integer_part(pi_to_the<1>()) == magnitude<>{});
//     CHECK(integer_part(pi_to_the<-8>()) == magnitude<>{});
//     CHECK(integer_part(pi_to_the<ratio{3, 4}>()) == magnitude<>{});
//   }

//   SECTION("integer_part of integer base to negative power is identity magnitude")
//   {
//     CHECK(integer_part(magnitude<base_power{2, -8}>{}) == magnitude<>{});
//     CHECK(integer_part(magnitude<base_power{11, -1}>{}) == magnitude<>{});
//   }

//   SECTION("integer_part of integer base to fractional power is identity magnitude")
//   {
//     CHECK(integer_part(magnitude<base_power{2, ratio{1, 2}}>{}) == magnitude<>{});
//   }

//   SECTION("integer_part of integer base to power at least one takes integer part")
//   {
//     CHECK(integer_part(magnitude<base_power{2, 1}>{}) == magnitude<base_power{2, 1}>{});
//     CHECK(integer_part(magnitude<base_power{2, ratio{19, 10}}>{}) == magnitude<base_power{2, 1}>{});
//     CHECK(integer_part(magnitude<base_power{11, ratio{97, 9}}>{}) == magnitude<base_power{11, 10}>{});
//   }
// }

// TEST_CASE("Prime helper functions")
// {
//   SECTION("multiplicity")
//   {
//     CHECK(multiplicity(2, 8) == 3);
//     CHECK(multiplicity(2, 1024) == 10);
//     CHECK(multiplicity(11, 6655) == 3);
//   }

//   SECTION("remove_power()")
//   {
//     CHECK(remove_power(17, 0, 5) == 5);
//     CHECK(remove_power(2, 3, 24) == 3);
//     CHECK(remove_power(11, 3, 6655) == 5);
//   }
// }

// TEST_CASE("Prime factorization")
// {
//   SECTION("1 factors into the null magnitude") { CHECK(prime_factorization_v<1> == magnitude<>{}); }

//   SECTION("Prime numbers factor into themselves")
//   {
//     CHECK(prime_factorization_v<2> == magnitude<base_power{2}>{});
//     CHECK(prime_factorization_v<3> == magnitude<base_power{3}>{});
//     CHECK(prime_factorization_v<5> == magnitude<base_power{5}>{});
//     CHECK(prime_factorization_v<7> == magnitude<base_power{7}>{});
//     CHECK(prime_factorization_v<11> == magnitude<base_power{11}>{});

//     CHECK(prime_factorization_v<41> == magnitude<base_power{41}>{});
//   }

//   SECTION("Prime factorization finds factors and multiplicities")
//   {
//     CHECK(prime_factorization_v<792> == magnitude<base_power{2, 3}, base_power{3, 2}, base_power{11}>{});
//   }
// }

// TEST_CASE("is_prime detects primes")
// {
//   SECTION("Non-positive numbers are not prime")
//   {
//     CHECK(!is_prime(-1328));
//     CHECK(!is_prime(-1));
//     CHECK(!is_prime(0));
//   }

//   SECTION("1 is not prime") { CHECK(!is_prime(1)); }

//   SECTION("Discriminates between primes and non-primes")
//   {
//     CHECK(is_prime(2));
//     CHECK(is_prime(3));
//     CHECK(!is_prime(4));
//     CHECK(is_prime(5));
//     CHECK(!is_prime(6));
//     CHECK(is_prime(7));
//     CHECK(!is_prime(8));
//     CHECK(!is_prime(9));

//     CHECK(is_prime(7919));
//   }
// }

// TEST_CASE("is_valid_base_power")
// {
//   SECTION("0 power is invalid")
//   {
//     REQUIRE(is_valid_base_power(base_power{2}));
//     CHECK(!is_valid_base_power(base_power{2, 0}));

//     REQUIRE(is_valid_base_power(base_power{41}));
//     CHECK(!is_valid_base_power(base_power{41, 0}));

//     REQUIRE(is_valid_base_power(base_power<pi_base>{}));
//     CHECK(!is_valid_base_power(base_power<pi_base>{0}));
//   }

//   SECTION("non-prime integers are invalid")
//   {
//     CHECK(!is_valid_base_power(base_power{-8}));
//     CHECK(!is_valid_base_power(base_power{0}));
//     CHECK(!is_valid_base_power(base_power{1}));

//     CHECK(is_valid_base_power(base_power{2}));
//     CHECK(is_valid_base_power(base_power{3}));

//     CHECK(!is_valid_base_power(base_power{4}));
//   }

//   SECTION("non-positive floating point bases are invalid")
//   {
//     CHECK(!is_valid_base_power(base_power<invalid_zero_base>{}));
//     CHECK(!is_valid_base_power(base_power<invalid_negative_base>{}));
//   }
// }

// TEST_CASE("pairwise_all evaluates all pairs")
// {
//   const auto all_pairs_return_true = pairwise_all{[](auto, auto) { return true; }};
//   const auto all_pairs_return_false = pairwise_all{[](auto, auto) { return false; }};
//   const auto all_increasing = pairwise_all{std::less{}};

//   SECTION("always true for empty tuples")
//   {
//     CHECK(all_pairs_return_true());
//     CHECK(all_pairs_return_false());
//   }

//   SECTION("always true for single-element tuples")
//   {
//     CHECK(all_pairs_return_true(1));
//     CHECK(all_pairs_return_false(3.14));
//     CHECK(all_pairs_return_true('x'));
//   }

//   SECTION("true for longer tuples iff true for all neighbouring pairs")
//   {
//     CHECK(all_increasing(1, 1.5));
//     CHECK(all_increasing(1, 1.5, 2));

//     CHECK(!all_increasing(1, 2.0, 2));
//     CHECK(!all_increasing(1, 2.5, 2));

//     CHECK(all_pairs_return_true('c', 1, 8.9, 42u));
//     CHECK(!all_pairs_return_false('c', 1, 8.9, 42u));
//   }
// }

// TEST_CASE("strictly_increasing")
// {
//   SECTION("Empty input is sorted") { CHECK(strictly_increasing()); }

//   SECTION("Single-element input is sorted")
//   {
//     CHECK(strictly_increasing(3));
//     CHECK(strictly_increasing(15.42));
//     CHECK(strictly_increasing('c'));
//   }

//   SECTION("Multi-value inputs compare correctly")
//   {
//     CHECK(strictly_increasing(3, 3.14));
//     CHECK(!strictly_increasing(3, 3.0));
//     CHECK(!strictly_increasing(4, 3.0));
//   }
// }

// TEST_CASE("extract_power_of_10")
// {
//   SECTION("Picks out positive powers")
//   {
//     CHECK(extract_power_of_10(mag<10>()) == 1);
//     CHECK(extract_power_of_10(mag<20>()) == 1);
//     CHECK(extract_power_of_10(mag<40>()) == 1);
//     CHECK(extract_power_of_10(mag<50>()) == 1);
//     CHECK(extract_power_of_10(mag<100>()) == 2);
//   }

//   SECTION("Picks out negative powers")
//   {
//     constexpr auto ONE = mag<1>();
//     CHECK(extract_power_of_10(ONE / mag<10>()) == -1);
//     CHECK(extract_power_of_10(ONE / mag<20>()) == -1);
//     CHECK(extract_power_of_10(ONE / mag<40>()) == -1);
//     CHECK(extract_power_of_10(ONE / mag<50>()) == -1);
//     CHECK(extract_power_of_10(ONE / mag<100>()) == -2);
//   }

//   SECTION("Zero if signs disagree") { CHECK(extract_power_of_10(mag<2>() / mag<5>()) == 0); }

//   SECTION("Handles rational powers") { CHECK(extract_power_of_10(sqrt(mag<1000>())) == 1); }
// }

}  // namespace


// mag<2> * Constant<2> * Constant<3> * Constant<2> * mag<3> * mag<2> * mag<2> * Constant<2> -> pow<Constant<2>, 3>,
// pow<mag<2>, 3>, Constant<3>, mag<3>

// mag_A * mag<2> * magA
// mag<2> * mag_A * magAA * magA -> the same value different name
