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

#include <units/base_dimension.h>
#include <units/ratio.h>

namespace units {

/**
 * @brief A power of factor corresponding to the dimension of a quantity
 *
 * @tparam Dim component dimension of a derived quantity
 * @tparam Num numinator of the factor
 * @tparam Den denominator of the factor
 */
template<Dimension Dim, std::intmax_t Num, std::intmax_t Den = 1>
struct exponent {
  using dimension = Dim;
  static constexpr int num = Num;
  static constexpr int den = Den;
};

// is_exponent
namespace detail {

template<typename Dim, std::intmax_t Num, std::intmax_t Den>
inline constexpr bool is_exponent<exponent<Dim, Num, Den>> = true;

}  // namespace detail

// exp_less
template<Exponent E1, Exponent E2>
  requires BaseDimension<typename E1::dimension> && BaseDimension<typename E2::dimension>
struct exp_less : base_dimension_less<typename E1::dimension, typename E2::dimension> {};

// exp_invert
namespace detail {

template<typename Dim, std::intmax_t Num, std::intmax_t Den>
constexpr exponent<Dim, -Num, Den> exp_invert_impl(exponent<Dim, Num, Den>);

}  // namespace detail

template<Exponent E>
using exp_invert = decltype(detail::exp_invert_impl(E()));

// exp_multiply
namespace detail {

template<Exponent E, std::intmax_t Num, std::intmax_t Den>
struct exp_multiply_impl {
  static constexpr ratio r = ratio(E::num, E::den) * ratio(Num, Den);
  using type = exponent<typename E::dimension, r.num, r.den>;
};

}  // namespace detail

template<Exponent E, std::intmax_t Num, std::intmax_t Den>
using exp_multiply = TYPENAME detail::exp_multiply_impl<E, Num, Den>::type;

template<Exponent... Es>
struct exp_list {};

}  // namespace units
