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
#include <mp-units/bits/unit_magnitude.h>
#include <mp-units/ext/inplace_vector.h>
#include <mp-units/framework/unit_magnitude.h>
#endif
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <type_traits>
#endif

using namespace mp_units;
using namespace mp_units::detail;

namespace {

// ============================================================
// Named (user-defined) magnitude types for testing.
// A named magnitude is a user-defined struct that inherits from unit_magnitude.
// ============================================================
inline constexpr struct named_mag_2_ : unit_magnitude<2> {
} named_mag_2;
// ============================================================
// UnitMagnitude concept
// ============================================================
// Direct specializations of unit_magnitude satisfy UnitMagnitude.
static_assert(UnitMagnitude<unit_magnitude<>>);
static_assert(UnitMagnitude<unit_magnitude<2>>);
static_assert(UnitMagnitude<unit_magnitude<2, 3>>);
// mag<N> variables are const-qualified; remove_cv_t strips the const.
static_assert(UnitMagnitude<std::remove_cv_t<decltype(mag<1>)>>);
static_assert(UnitMagnitude<std::remove_cv_t<decltype(mag<2>)>>);
static_assert(UnitMagnitude<std::remove_cv_t<decltype(mag<412>)>>);
// Derived types (named magnitudes) do not satisfy is_specialization_of_v.
static_assert(!UnitMagnitude<named_mag_2_>);
// Non-magnitude types do not satisfy UnitMagnitude.
static_assert(!UnitMagnitude<int>);
static_assert(!UnitMagnitude<double>);
// pi_c is a value; use decltype to obtain the type for concept testing.
static_assert(!UnitMagnitude<decltype(pi_c)>);  // pi_c type is mag_constant, not unit_magnitude

// ============================================================
// MagArg concept: integers, ratio, mag_constant subtypes
// ============================================================
static_assert(MagArg<int>);
static_assert(MagArg<long>);
static_assert(MagArg<std::intmax_t>);
static_assert(MagArg<ratio>);
// Note: pi_c structural type satisfies is_mag_constant; testing via get_base_value below
static_assert(!MagArg<double>);
static_assert(!MagArg<float>);
static_assert(!MagArg<unit_magnitude<2>>);

// ============================================================
// power_v: illegal instantiations
// ============================================================
// power_v<V, Num, Den...> requires valid_ratio && !ratio_one.
// ratio_one means the exponent reduces to 1/1 (which is excluded).
template<template<auto, int, int...> typename P>
concept invalid_power_v = requires {
  requires !requires { typename P<123, 0>; };     // 0/1: zero exponent invalid
  requires !requires { typename P<123, 0, 2>; };  // 0/2: zero exponent invalid
  requires !requires { typename P<123, 1, 0>; };  // 1/0: zero denominator invalid
  requires !requires { typename P<123, 0, 0>; };  // 0/0: both zero invalid
  requires !requires { typename P<123, 1>; };     // 1/1 = ratio_one, invalid
  requires !requires { typename P<123, 1, 1>; };  // 1/1 = ratio_one, invalid
  requires !requires { typename P<123, 5, 5>; };  // 5/5 = ratio_one, invalid
};
static_assert(invalid_power_v<power_v>);

// Valid power_v instantiations (non-one, non-zero ratios)
static_assert(std::is_same_v<decltype(power_v<2, 3>{}), power_v<2, 3>>);
static_assert(std::is_same_v<decltype(power_v<5, 1, 3>{}), power_v<5, 1, 3>>);
static_assert(std::is_same_v<decltype(power_v<3, -1>{}), power_v<3, -1>>);
static_assert(std::is_same_v<decltype(power_v<2, -3, 2>{}), power_v<2, -3, 2>>);

// ============================================================
// get_base
// ============================================================
// Bare values: get_base is identity
static_assert(get_base(2) == 2);
static_assert(get_base(3) == 3);
static_assert(get_base(std::intmax_t{5}) == 5);

// power_v: get_base extracts the base
static_assert(get_base(power_v<3, 5>{}) == 3);
static_assert(get_base(power_v<5, 1, 3>{}) == 5);
static_assert(get_base(power_v<7, -2>{}) == 7);

// Named magnitudes: get_base yields the named type
static_assert(std::is_same_v<decltype(get_base(named_mag_2)), named_mag_2_>);
static_assert(std::is_same_v<decltype(get_base(power_v<named_mag_2, 2>{})), named_mag_2_>);
static_assert(std::is_same_v<decltype(get_base(power_v<named_mag_2, 5, 8>{})), named_mag_2_>);

// ============================================================
// get_base_value
// ============================================================
static_assert(get_base_value(2) == 2);
static_assert(get_base_value(3) == 3);
static_assert(get_base_value(power_v<3, 5>{}) == 3);
static_assert(get_base_value(power_v<5, 1, 3>{}) == 5);
static_assert(get_base_value(negative_tag{}) == -1);

// ============================================================
// get_exponent
// ============================================================
// Bare value: exponent is implicitly 1
static_assert(get_exponent(2) == ratio{1});
static_assert(get_exponent(3) == ratio{1});

// power_v: get_exponent returns the stored exponent
static_assert(get_exponent(power_v<3, 5>{}) == ratio{5});
static_assert(get_exponent(power_v<5, 1, 3>{}) == ratio{1, 3});
static_assert(get_exponent(power_v<2, -1>{}) == ratio{-1});
static_assert(get_exponent(power_v<7, -3, 2>{}) == ratio{-3, 2});

// ============================================================
// is_nonzero_mag_arg (constexpr bool variable template)
// ============================================================
static_assert(is_nonzero_mag_arg<2>);
static_assert(is_nonzero_mag_arg<-2>);
static_assert(is_nonzero_mag_arg<100>);
static_assert(is_nonzero_mag_arg<ratio{3, 4}>);
static_assert(is_nonzero_mag_arg<ratio{-3, 4}>);
static_assert(!is_nonzero_mag_arg<0>);

// ============================================================
// is_positive_mag_arg (constexpr bool variable template)
// ============================================================
static_assert(is_positive_mag_arg<2>);
static_assert(is_positive_mag_arg<100>);
static_assert(is_positive_mag_arg<ratio{3, 4}>);
static_assert(!is_positive_mag_arg<-2>);
static_assert(!is_positive_mag_arg<0>);
static_assert(!is_positive_mag_arg<ratio{-3, 4}>);

// ============================================================
// mag<V>: positive integers
// ============================================================
// mag<1> is the dimensionless identity (empty magnitude pack)
static_assert(mag<1> == unit_magnitude<>{});
// Composites verify via arithmetic relationships (avoids NTTP type ambiguity with intmax_t)
static_assert(mag<4> == pow<2>(mag<2>));
static_assert(mag<6> == mag<2> * mag<3>);
static_assert(mag<8> == pow<3>(mag<2>));
static_assert(mag<9> == pow<2>(mag<3>));
static_assert(mag<12> == mag<4> * mag<3>);

// ============================================================
// mag<V>: negative integers (negative_tag sentinel)
// ============================================================
// mag<-N> strips the sign: abs_magnitude(mag<-N>) == mag<N>
static_assert(abs_magnitude(mag<-1>) == mag<1>);
static_assert(abs_magnitude(mag<-2>) == mag<2>);
static_assert(abs_magnitude(mag<-3>) == mag<3>);
static_assert(abs_magnitude(mag<-6>) == mag<6>);
static_assert(abs_magnitude(mag<-4>) == mag<4>);
// mag<-N> * mag<-1> == mag<N>
static_assert(mag<-1> * mag<-1> == mag<1>);
static_assert(mag<-2> * mag<-1> == mag<2>);
static_assert(mag<-6> * mag<-1> == mag<6>);
// mag<-N> is distinct from mag<N>
static_assert(mag<-1> != mag<1>);
static_assert(mag<-2> != mag<2>);
static_assert(mag<-6> != mag<6>);
// negative_tag sentinel: the unit_magnitude<negative_tag{}>{} has no positive factors
static_assert(mag<-1> == unit_magnitude<negative_tag{}>{});

// ============================================================
// mag_ratio<N, D>: positive and negative
// ============================================================
static_assert(UnitMagnitude<std::remove_cv_t<decltype(mag_ratio<3, 4>)>>);
static_assert(UnitMagnitude<std::remove_cv_t<decltype(mag_ratio<-3, 4>)>>);
static_assert(UnitMagnitude<std::remove_cv_t<decltype(mag_ratio<3, -4>)>>);
static_assert(UnitMagnitude<std::remove_cv_t<decltype(mag_ratio<-3, -4>)>>);

// Equivalent fractions are equal (ratio reduction in constructor)
static_assert(mag_ratio<3, 4> == mag_ratio<9, 12>);
static_assert(mag_ratio<-3, 4> == mag_ratio<-9, 12>);

// ratio{N,D} always normalises the denominator to be positive:
// negative denominator flips both signs, so mag_ratio<3,-4> == mag_ratio<-3,4>.
static_assert(mag_ratio<3, -4> == mag_ratio<-3, 4>);
static_assert(mag_ratio<6, -8> == mag_ratio<-3, 4>);  // also reduces

// Both negative: two sign flips cancel, result is positive.
static_assert(mag_ratio<-3, -4> == mag_ratio<3, 4>);
static_assert(mag_ratio<-9, -12> == mag_ratio<3, 4>);  // also reduces

// Sign classification
static_assert(!magnitude_is_positive<mag_ratio<3, -4>>);  // normalises to -3/4 → negative
static_assert(magnitude_is_positive<mag_ratio<-3, -4>>);  // normalises to  3/4 → positive

// ============================================================
// mag_power<Base, Num, Den>: constexpr alias for pow<Num,Den>(mag<Base>)
// ============================================================
static_assert(UnitMagnitude<std::remove_cv_t<decltype(mag_power<2, 2>)>>);
static_assert(UnitMagnitude<std::remove_cv_t<decltype(mag_power<2, 1, 2>)>>);  // sqrt(2)
static_assert(UnitMagnitude<std::remove_cv_t<decltype(mag_power<pi_c, 1>)>>);
static_assert(UnitMagnitude<std::remove_cv_t<decltype(mag_power<pi_c, -1>)>>);
static_assert(UnitMagnitude<std::remove_cv_t<decltype(mag_power<pi_c, 1, 2>)>>);  // sqrt(pi)

// mag_power requires is_positive_mag_arg<Base> (negative base is rejected at the requires clause)

// ============================================================
// mag<pi_c>: irrational magnitude
// ============================================================
static_assert(UnitMagnitude<std::remove_cv_t<decltype(mag<pi_c>)>>);
static_assert(!is_rational(mag<pi_c>));
static_assert(!is_integral(mag<pi_c>));
static_assert(is_positive_integral_power(mag<pi_c>));  // pi^1: positive integer exponent

// ============================================================
// Equality
// ============================================================
static_assert(mag<1> == mag<1>);
static_assert(mag<2> == mag<2>);
static_assert(mag<3> != mag<5>);
static_assert(mag<3> != mag_ratio<3, 2>);
static_assert(mag_ratio<4, 5> != mag_ratio<4, 3>);

// Positive vs. negative
static_assert(mag<-2> != mag<2>);
static_assert(mag<2> != mag<-2>);
static_assert(mag<-2> == mag<-2>);
static_assert(mag<-3> != mag<-5>);

// ============================================================
// Multiplication
// ============================================================
// Positive * positive
static_assert(mag_ratio<4, 5> * mag_ratio<4, 3> == mag_ratio<16, 15>);
// Reciprocal pair reduces to identity
static_assert(mag_ratio<3, 4> * mag_ratio<4, 3> == mag<1>);

// Positive * negative = negative
static_assert(mag<2> * mag<-3> == mag<-6>);
static_assert(mag<-3> * mag<2> == mag<-6>);

// Negative * negative = positive (two negative_tags cancel)
static_assert(mag<-2> * mag<-3> == mag<6>);
static_assert(mag<-1> * mag<-1> == mag<1>);

// ============================================================
// Division
// ============================================================
// Anything divided by itself reduces to identity
static_assert(mag_ratio<3, 4> / mag_ratio<3, 4> == mag<1>);
static_assert(mag<15> / mag<15> == mag<1>);

// Standard positive quotients
static_assert(mag_ratio<4, 5> / mag_ratio<4, 3> == mag_ratio<3, 5>);

// Division involving negatives
static_assert(mag<-6> / mag<2> == mag<-3>);  // (-6)/2 = -3
static_assert(mag<6> / mag<-2> == mag<-3>);  // 6/(-2) = -3
static_assert(mag<-6> / mag<-2> == mag<3>);  // (-6)/(-2) = 3

// ============================================================
// pow<Num, Den>(m)
// ============================================================
// Anything to the 0 is 1
static_assert(pow<0>(mag<1>) == mag<1>);
static_assert(pow<0>(mag<123>) == mag<1>);
static_assert(pow<0>(mag_ratio<3, 4>) == mag<1>);

// Anything to the 1 is itself
static_assert(pow<1>(mag<1>) == mag<1>);
static_assert(pow<1>(mag<123>) == mag<123>);
static_assert(pow<1>(mag_ratio<3, 4>) == mag_ratio<3, 4>);

// Integer powers
static_assert(pow<2>(mag<3>) == mag<9>);
static_assert(pow<3>(mag<2>) == mag<8>);

// Negative integer powers
static_assert(pow<-1>(mag<2>) == mag_ratio<1, 2>);
static_assert(pow<-2>(mag<2>) == mag_ratio<1, 4>);

// Rational powers: sqrt of perfect squares
static_assert(pow<1, 2>(mag<4>) == mag<2>);
static_assert(pow<1, 2>(mag<9>) == mag<3>);

// Negative magnitude raised to integer powers
static_assert(pow<2>(mag<-1>) == mag<1>);   // (-1)^2 = 1
static_assert(pow<3>(mag<-1>) == mag<-1>);  // (-1)^3 = -1
static_assert(pow<2>(mag<-2>) == mag<4>);   // (-2)^2 = 4
static_assert(pow<3>(mag<-2>) == mag<-8>);  // (-2)^3 = -8

// Negative magnitude raised to negative integer powers: sign follows the same parity rule.
static_assert(pow<-1>(mag<-2>) == mag_ratio<-1, 2>);  // (-2)^-1 = -1/2 (odd power → neg)
static_assert(pow<-1>(mag<-4>) == mag_ratio<-1, 4>);  // (-4)^-1 = -1/4
static_assert(pow<-2>(mag<-2>) == mag_ratio<1, 4>);   // (-2)^-2 = 1/4 (even power → pos)

// Even root of a negative magnitude triggers a static_assert (hard error) inside pow.

// ============================================================
// get_value<T>
// ============================================================
// Positive integer magnitudes
static_assert(get_value<int>(mag<1>) == 1);
static_assert(get_value<int>(mag<2>) == 2);
static_assert(get_value<int>(mag<412>) == 412);
static_assert(get_value<long>(mag<2>) == 2L);
static_assert(get_value<float>(mag<2>) == 2.0f);
static_assert(get_value<double>(mag<2>) == 2.0);

// Negative integer magnitudes
static_assert(get_value<int>(mag<-1>) == -1);
static_assert(get_value<int>(mag<-2>) == -2);
static_assert(get_value<int>(mag<-8>) == -8);
static_assert(get_value<double>(mag<-3>) == -3.0);

// Fractional magnitudes (require floating-point or will overflow)
static_assert(get_value<float>(mag_ratio<1, 8>) == 0.125f);
static_assert(get_value<double>(mag_ratio<1, 8>) == 0.125);
static_assert(get_value<double>(mag_ratio<1, 2>) == 0.5);

// Negative fractional magnitudes
static_assert(get_value<double>(mag_ratio<-1, 4>) == -0.25);
static_assert(get_value<double>(mag_ratio<-1, 2>) == -0.5);

// Negative denominator: normalised to negative numerator, so same value.
static_assert(get_value<double>(mag_ratio<3, -4>) == -0.75);
static_assert(get_value<double>(mag_ratio<1, -2>) == -0.5);

// Both negative: normalised to positive, so same value as the all-positive form.
static_assert(get_value<double>(mag_ratio<-1, -4>) == 0.25);
static_assert(get_value<double>(mag_ratio<-1, -2>) == 0.5);

// ============================================================
// numerator and denominator
// ============================================================
// Identity: numerator and denominator of mag<1> are both mag<1>
static_assert(numerator(mag<1>) == mag<1>);
static_assert(denominator(mag<1>) == mag<1>);

// Integer: numerator is the mag itself, denominator is 1
static_assert(numerator(mag<12>) == mag<12>);
static_assert(denominator(mag<12>) == mag<1>);

// Fraction: splits into numerator and denominator
static_assert(numerator(mag_ratio<3, 4>) == mag<3>);
static_assert(denominator(mag_ratio<3, 4>) == mag<4>);
static_assert(numerator(mag_ratio<5, 8>) == mag<5>);
static_assert(denominator(mag_ratio<5, 8>) == mag<8>);

// Negative integer: negative_tag is absorbed into the integer_part, so numerator carries the sign.
static_assert(numerator(mag<-1>) == mag<-1>);
static_assert(numerator(mag<-6>) == mag<-6>);
// The denominator is derived from abs_magnitude, so it is always positive.
static_assert(denominator(mag<-1>) == mag<1>);
static_assert(denominator(mag<-6>) == mag<1>);

// Negative fraction: numerator carries the sign; denominator is positive.
static_assert(numerator(mag_ratio<-3, 4>) == mag<-3>);
static_assert(denominator(mag_ratio<-3, 4>) == mag<4>);
static_assert(numerator(mag_ratio<-5, 8>) == mag<-5>);
static_assert(denominator(mag_ratio<-5, 8>) == mag<8>);

// Reconstruction: numerator(m) / denominator(m) == m for both positive and negative mags.
static_assert(numerator(mag_ratio<3, 4>) / denominator(mag_ratio<3, 4>) == mag_ratio<3, 4>);
static_assert(numerator(mag_ratio<-3, 4>) / denominator(mag_ratio<-3, 4>) == mag_ratio<-3, 4>);
static_assert(numerator(mag<-6>) / denominator(mag<-6>) == mag<-6>);

// Negative denominator: normalised at construction, so result equals the -num/+den form.
// mag_ratio<3,-4> == mag_ratio<-3,4>: numerator carries the sign, denominator is positive.
static_assert(numerator(mag_ratio<3, -4>) == mag<-3>);
static_assert(denominator(mag_ratio<3, -4>) == mag<4>);
static_assert(numerator(mag_ratio<3, -4>) / denominator(mag_ratio<3, -4>) == mag_ratio<3, -4>);

// Both negative: normalised to positive, so numerator is the positive integer part.
static_assert(numerator(mag_ratio<-3, -4>) == mag<3>);
static_assert(denominator(mag_ratio<-3, -4>) == mag<4>);
static_assert(numerator(mag_ratio<-3, -4>) / denominator(mag_ratio<-3, -4>) == mag_ratio<-3, -4>);

// ============================================================
// is_rational and is_integral
// ============================================================
// Identity (empty pack) is integral and rational
static_assert(is_integral(unit_magnitude<>{}));
static_assert(is_rational(unit_magnitude<>{}));

// Positive integers are integral and rational
static_assert(is_integral(mag<1>));
static_assert(is_integral(mag<3>));
static_assert(is_integral(mag<412>));
static_assert(is_rational(mag<1>));
static_assert(is_rational(mag<3>));

// Negative integers: treated as integral and rational (negative_tag represents the integer -1)
static_assert(is_integral(mag<-1>));
static_assert(is_integral(mag<-2>));
static_assert(is_integral(mag<-8>));
static_assert(is_rational(mag<-2>));

// Positive fractional magnitudes: rational but not integral
static_assert(!is_integral(mag_ratio<1, 2>));
static_assert(is_rational(mag_ratio<1, 2>));
static_assert(!is_integral(mag_ratio<5, 8>));
static_assert(is_rational(mag_ratio<5, 8>));

// Negative fractional magnitudes: rational but not integral
static_assert(!is_integral(mag_ratio<-1, 2>));
static_assert(is_rational(mag_ratio<-1, 2>));

// Irrational: neither rational nor integral
static_assert(!is_rational(mag<pi_c>));
static_assert(!is_integral(mag<pi_c>));

// Irrational fractional power: not rational, not integral
static_assert(!is_rational(mag_power<2, 1, 2>));  // sqrt(2)
static_assert(!is_integral(mag_power<2, 1, 2>));

// ============================================================
// is_positive_integral_power
// ============================================================
// Identity (empty pack) is vacuously true
static_assert(is_positive_integral_power(unit_magnitude<>{}));
// Positive integer bases with positive integer exponents → true
static_assert(is_positive_integral_power(mag<2>));
static_assert(is_positive_integral_power(mag<8>));
// Negative exponent → false
static_assert(!is_positive_integral_power(mag_ratio<1, 2>));
// Fractional exponent → false
static_assert(!is_positive_integral_power(mag_power<2, 1, 2>));
// Negative magnitude has negative_tag → false (negative_tag is not a positive power)
static_assert(!is_positive_integral_power(mag<-1>));
static_assert(!is_positive_integral_power(mag<-2>));

// ============================================================
// common_magnitude
// ============================================================
// Identity among equal magnitudes
static_assert(common_magnitude(mag<1>, mag<1>) == mag<1>);
static_assert(common_magnitude(mag<15>, mag<15>) == mag<15>);

// GCF for integers
static_assert(common_magnitude(mag<24>, mag<36>) == mag<12>);
static_assert(common_magnitude(mag<24>, mag<37>) == mag<1>);

// Fractional inputs
static_assert(common_magnitude(mag_ratio<3, 8>, mag_ratio<5, 6>) == mag_ratio<1, 24>);

// Both negative: negative_tag is shared, so it is preserved in the result.
// Semantically, common_magnitude(-2, -4) == -2: both -2/-2=1 and -4/-2=2 are integer multiples.
static_assert(common_magnitude(mag<-2>, mag<-4>) == mag<-2>);
static_assert(common_magnitude(mag<-24>, mag<-36>) == mag<-12>);
// Negative fractions: negative_tag shared → preserved; fractional part follows the same GCF rule.
static_assert(common_magnitude(mag_ratio<-3, 8>, mag_ratio<-5, 6>) == mag_ratio<-1, 24>);

// Mixed sign: negative_tag is present in one but not the other.
// remove_positive_power strips it, giving the same result as the all-positive case.
static_assert(common_magnitude(mag<2>, mag<-4>) == mag<2>);
static_assert(common_magnitude(mag<-2>, mag<4>) == mag<2>);
static_assert(common_magnitude(mag<-24>, mag<36>) == mag<12>);
static_assert(common_magnitude(mag<24>, mag<-36>) == mag<12>);

// ============================================================
// abs_magnitude
// ============================================================
// Positive magnitudes are unchanged
static_assert(abs_magnitude(unit_magnitude<>{}) == unit_magnitude<>{});
static_assert(abs_magnitude(mag<1>) == mag<1>);
static_assert(abs_magnitude(mag<2>) == mag<2>);
static_assert(abs_magnitude(mag_ratio<3, 4>) == mag_ratio<3, 4>);

// Negative magnitudes: strips negative_tag
static_assert(abs_magnitude(mag<-1>) == mag<1>);
static_assert(abs_magnitude(mag<-2>) == mag<2>);
static_assert(abs_magnitude(mag<-6>) == mag<6>);
static_assert(abs_magnitude(mag_ratio<-3, 4>) == mag_ratio<3, 4>);

// abs of abs is the same (idempotent on positive)
static_assert(abs_magnitude(abs_magnitude(mag<-4>)) == mag<4>);

// ============================================================
// magnitude_is_positive (constexpr bool variable template)
// ============================================================
// Positive magnitudes
static_assert(magnitude_is_positive<unit_magnitude<>{}>);
static_assert(magnitude_is_positive<mag<1>>);
static_assert(magnitude_is_positive<mag<2>>);
static_assert(magnitude_is_positive<mag<412>>);
static_assert(magnitude_is_positive<mag_ratio<3, 4>>);
static_assert(magnitude_is_positive<mag<pi_c>>);

// Negative magnitudes (have leading negative_tag)
static_assert(!magnitude_is_positive<mag<-1>>);
static_assert(!magnitude_is_positive<mag<-2>>);
static_assert(!magnitude_is_positive<mag<-6>>);
static_assert(!magnitude_is_positive<mag_ratio<-1, 4>>);
static_assert(!magnitude_is_positive<mag_ratio<3, -4>>);

// ============================================================
// check_magnitude_is_positive (consteval function)
// ============================================================
static_assert(check_magnitude_is_positive(mag<1>));
static_assert(check_magnitude_is_positive(mag<5>));
static_assert(check_magnitude_is_positive(mag_ratio<3, 4>));
static_assert(!check_magnitude_is_positive(mag<-1>));
static_assert(!check_magnitude_is_positive(mag<-5>));
static_assert(!check_magnitude_is_positive(mag_ratio<-3, 4>));

// ============================================================
// Negative magnitude arithmetic consistency
// ============================================================
// Result of multiplying two negatives is positive
static_assert(mag<-2> * mag<-3> == mag<6>);
static_assert(check_magnitude_is_positive(mag<-2> * mag<-3>));

// Result of multiplying positive by negative is negative
static_assert(mag<2> * mag<-3> == mag<-6>);
static_assert(!check_magnitude_is_positive(mag<2> * mag<-3>));

// Negative divided by negative is positive
static_assert(mag<-6> / mag<-2> == mag<3>);
static_assert(check_magnitude_is_positive(mag<-6> / mag<-2>));

// abs_magnitude strips sign; multiplying result by its own inverse gives identity
static_assert(abs_magnitude(mag<-6>) * pow<-1>(abs_magnitude(mag<-6>)) == mag<1>);

// ============================================================
// magnitude_symbol
// ============================================================
// Helper: evaluate magnitude_symbol to a fixed-length string at compile time.
// Mirrors the pattern of detail::unit_symbol_impl in unit.h.
using enum character_set;
using enum unit_symbol_solidus;
using usf = unit_symbol_formatting;

template<UnitMagnitude auto M, unit_symbol_formatting fmt = unit_symbol_formatting{}, typename CharT = char>
[[nodiscard]] consteval auto mag_symbol()
{
  constexpr auto buf = []() consteval {
    detail::inplace_vector<CharT, 128> text;
    (void)magnitude_symbol<CharT>(std::back_inserter(text), M, fmt);
    return text;
  }();
  return basic_fixed_string<CharT, buf.size()>(std::from_range, buf);
}

// Identity magnitude → empty string
static_assert(mag_symbol<mag<1>>() == "");

// Positive integers
static_assert(mag_symbol<mag<2>>() == "2");
static_assert(mag_symbol<mag<6>>() == "6");
static_assert(mag_symbol<mag<12>>() == "12");

// Large pure power-of-ten: exponent notation
static_assert(mag_symbol<mag<1000>>() == "10³");

// Negative integers: '-' prefix then absolute-magnitude symbol
static_assert(mag_symbol<mag<-1>>() == "-");  // abs = identity → sign only
static_assert(mag_symbol<mag<-2>>() == "-2");
static_assert(mag_symbol<mag<-6>>() == "-6");

// Positive ratios — default solidus = one_denominator → "num/den"
static_assert(mag_symbol<mag_ratio<3, 4>>() == "3/4");
static_assert(mag_symbol<mag_ratio<1, 2>>() == "1/2");

// Negative ratios
static_assert(mag_symbol<mag_ratio<-3, 4>>() == "-3/4");

// Irrational constant pi — UTF-8 (default char_set)
static_assert(mag_symbol<mag<pi_c>>() == "π");
static_assert(mag_symbol<pow<2>(mag<pi_c>)>() == "π²");

// pi singleton in the denominator: solidus=one_denominator, den_size=1 → "1/π"
static_assert(mag_symbol<pow<-1>(mag<pi_c>)>() == "1/π");

// Explicit solidus=never: denominator written as negative superscript
static_assert(mag_symbol<pow<-1>(mag<pi_c>), usf{.solidus = never}>() == "π⁻¹");

// Portable charset: ASCII symbols and exponents
static_assert(mag_symbol<mag<pi_c>, usf{.char_set = portable}>() == "pi");
static_assert(mag_symbol<pow<2>(mag<pi_c>), usf{.char_set = portable}>() == "pi^2");
static_assert(mag_symbol<pow<-1>(mag<pi_c>), usf{.char_set = portable, .solidus = never}>() == "pi^-1");

// Two-constant denominator: solidus=one_denominator, den_size>1 → negative powers (no slash)
// mag<1/(2*pi)> = pow<-1>(mag<2>) * pow<-1>(mag<pi_c>)
static_assert(mag_symbol<mag<1> / (mag<2> * mag<pi_c>)>() == "2⁻¹ π⁻¹");

}  // namespace
