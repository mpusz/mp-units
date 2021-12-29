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

#pragma once

#include <units/ratio.h>
#include <cstdint>

namespace units::mag
{

// A `magnitude` represents a positive real number in a format which optimizes taking products and
// rational powers.
template <typename... base_powers>
struct magnitude;

template <typename mag_t>
struct inverse;
template <typename mag_t>
using inverse_t = typename inverse<mag_t>::type;

template <typename... mags>
struct product;
template <typename... mags>
using product_t = typename product<mags...>::type;

template <typename mag1, typename mag2>
using quotient_t = product_t<mag1, inverse_t<mag2>>;

namespace detail
{
template <std::intmax_t n>
struct prime_factorization;
template <std::intmax_t n>
using prime_factorization_t = typename prime_factorization<n>::type;
} // namespace detail

template <typename... base_powers>
struct magnitude {
  bool operator==(magnitude) const { return true; }

  template <typename... other_base_powers>
  bool operator==(magnitude<other_base_powers...>) const { return false; }
};

template <std::intmax_t num = 1, std::intmax_t den = 1>
using ratio_t = quotient_t<detail::prime_factorization_t<num>, detail::prime_factorization_t<den>>;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation details below.
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Base powers.

template <typename base, ratio power = ratio{1}>
struct base_power;

template <std::intmax_t n>
struct int_base : std::integral_constant<std::intmax_t, n> {};
template <std::intmax_t n, std::intmax_t num = 1, std::intmax_t den = 1>
using int_base_power = base_power<int_base<n>, ratio{num, den}>;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Inverse implementation.

template <typename... bases, ratio... powers>
struct inverse<magnitude<base_power<bases, powers>...>> {
  using type = magnitude<base_power<bases, -powers>...>;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Product implementation.

// Convenience utility to prepend a base_power to a magnitude.
//
// Assumes that the prepended power has a smaller base than every base power in the magnitude.
template <typename new_base, typename mag_t>
struct prepend_base;
template <typename new_base, typename mag_t>
using prepend_base_t = typename prepend_base<new_base, mag_t>::type;
template <typename new_base, typename... old_bases>
struct prepend_base<new_base, magnitude<old_bases...>> {
  using type = magnitude<new_base, old_bases...>;
};

// Nullary case.
template <>
struct product<> { using type = magnitude<>; };

// Unary case.
template <typename mag_t>
struct product<mag_t> { using type = mag_t; };

// Binary case, where right argument is null magnitude.
template <typename mag_t>
struct product<mag_t, magnitude<>> { using type = mag_t; };

// Binary case, where left argument is null magnitude, and right is non-null.
template <typename head, typename... tail>
struct product<magnitude<>, magnitude<head, tail...>> { using type = magnitude<head, tail...>; };

// Binary case, with distinct heads.
template <
  typename base1,
  ratio pow1,
  typename... tail1,
  typename base2,
  ratio pow2,
  typename... tail2>
struct product<magnitude<base_power<base1, pow1>, tail1...>,
               magnitude<base_power<base2, pow2>, tail2...>>
{
  using mag1 = magnitude<base_power<base1, pow1>, tail1...>;
  using mag2 = magnitude<base_power<base2, pow2>, tail2...>;

  using type = std::conditional_t<
    (base1::value < base2::value),
    prepend_base_t<base_power<base1, pow1>, product_t<magnitude<tail1...>, mag2>>,
    prepend_base_t<base_power<base2, pow2>, product_t<mag1, magnitude<tail2...>>>>;
};

// Binary case, same head.
template <typename base, ratio pow1, ratio pow2, typename... tail1, typename... tail2>
struct product<magnitude<base_power<base, pow1>, tail1...>,
               magnitude<base_power<base, pow2>, tail2...>>
{
  using tail_product = product_t<magnitude<tail1...>, magnitude<tail2...>>;
  using type = std::conditional_t<
    ((pow1 + pow2).num == 0),
    tail_product,
    prepend_base_t<base_power<base, pow1 + pow2>, tail_product>>;
};

// N-ary case (N > 2).
template <typename mag_a, typename mag_b, typename... tail>
struct product<mag_a, mag_b, tail...>
{
  using type = product_t<product_t<mag_a, mag_b>, tail...>;
};

} // namespace units::mag
