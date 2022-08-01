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

#include <units/bits/derived_dimension_base.h>
#include <units/bits/dim_consolidate.h>
#include <units/bits/external/downcasting.h>
#include <units/bits/external/hacks.h>
#include <units/bits/external/type_list.h>
#include <units/derived_dimension.h>
#include <units/unit.h>

namespace units {

/**
 * @brief Unknown dimension
 *
 * Sometimes a temporary partial result of a complex calculation may not result in a predefined
 * dimension. In such a case an `unknown_dimension` is created with a coherent unit of `unknown_coherent_unit`
 * with a magnitude being the absolute one of all the exponents of such a dimension.
 *
 * @tparam Es the list of exponents of ingredient dimensions
 */
template<Exponent... Es>
struct unknown_dimension : derived_dimension<unknown_dimension<Es...>, unknown_coherent_unit<Es...>, Es...> {};

namespace detail {

template<DerivedDimension D>
struct check_unknown {
  using type = D;
};

// downcast did not find a user predefined type
template<typename... Es>
struct check_unknown<derived_dimension_base<Es...>> {
  using type = unknown_dimension<Es...>;
};

template<Dimension D>
struct downcast_dimension_impl;

template<BaseDimension D>
struct downcast_dimension_impl<D> {
  using type = D;
};

template<DerivedDimension D>
struct downcast_dimension_impl<D> {
  using type = TYPENAME check_unknown<downcast<D>>::type;
};

}  // namespace detail

template<Dimension D>
using downcast_dimension = TYPENAME detail::downcast_dimension_impl<D>::type;

// dim_invert
namespace detail {

template<Dimension D>
struct dim_invert_impl;

template<BaseDimension D>
struct dim_invert_impl<D> {
  using type = downcast_dimension<derived_dimension_base<exponent<D, -1>>>;
};

template<BaseDimension D>
struct dim_invert_impl<derived_dimension_base<exponent<D, -1>>> {
  using type = D;
};

template<typename... Es>
struct dim_invert_impl<derived_dimension_base<Es...>> {
  using type = downcast_dimension<derived_dimension_base<exponent_invert<Es>...>>;
};

template<DerivedDimension D>
struct dim_invert_impl<D> : dim_invert_impl<downcast_base_t<D>> {};

}  // namespace detail

template<Dimension D>
using dim_invert = TYPENAME detail::dim_invert_impl<D>::type;

// dimension_multiply
namespace detail {

template<typename T>
struct to_dimension;

template<Exponent... Es>
struct to_dimension<exponent_list<Es...>> {
  using type = derived_dimension_base<Es...>;
};

template<BaseDimension D>
struct to_dimension<exponent_list<exponent<D, 1>>> {
  using type = D;
};

/**
 * @brief Merges 2 sorted derived dimensions into one units::derived_dimension_base
 *
 * A result of a dimensional calculation may result with many exponents of the same base dimension orginated
 * from different parts of the equation. As the exponents lists of both operands it is enough to merge them
 * into one list and consolidate duplicates. Also it is possible that final exponents list will contain only
 * one element being a base dimension with exponent 1. In such a case the final dimension should be the base
 * dimension itself.
 */
template<Dimension D1, Dimension D2>
using merge_dimension = TYPENAME to_dimension<typename dim_consolidate<
  type_list_merge_sorted<typename D1::exponents, typename D2::exponents, exponent_less>>::type>::type;

template<Dimension D1, Dimension D2>
struct dimension_multiply_impl;

template<BaseDimension D1, BaseDimension D2>
struct dimension_multiply_impl<D1, D2> {
  using type = downcast_dimension<
    merge_dimension<derived_dimension_base<exponent<D1, 1>>, derived_dimension_base<exponent<D2, 1>>>>;
};

template<BaseDimension D1, DerivedDimension D2>
struct dimension_multiply_impl<D1, D2> {
  using type =
    downcast_dimension<merge_dimension<derived_dimension_base<exponent<D1, 1>>, typename D2::downcast_base_type>>;
};

template<DerivedDimension D1, BaseDimension D2>
struct dimension_multiply_impl<D1, D2> {
  using type = TYPENAME dimension_multiply_impl<D2, D1>::type;
};

template<DerivedDimension D1, DerivedDimension D2>
struct dimension_multiply_impl<D1, D2> {
  using type = downcast_dimension<merge_dimension<typename D1::downcast_base_type, typename D2::downcast_base_type>>;
};

}  // namespace detail

template<Dimension D1, Dimension D2>
using dimension_multiply = TYPENAME detail::dimension_multiply_impl<D1, D2>::type;

template<Dimension D1, Dimension D2>
using dimension_divide = TYPENAME detail::dimension_multiply_impl<D1, dim_invert<D2>>::type;

// dimension_pow
namespace detail {

template<Dimension D, std::intmax_t Num, std::intmax_t Den = 1>
struct dimension_pow_impl;

template<BaseDimension D, std::intmax_t Num, std::intmax_t Den>
struct dimension_pow_impl<D, Num, Den> {
  using type = downcast_dimension<derived_dimension_base<exponent<D, Num, Den>>>;
};

template<BaseDimension D>
struct dimension_pow_impl<D, 1, 1> {
  using type = D;
};

template<BaseDimension D, std::intmax_t Num, std::intmax_t Den>
struct dimension_pow_impl<derived_dimension_base<exponent<D, Den, Num>>, Num, Den> {
  using type = D;
};

template<DerivedDimension D, std::intmax_t Num, std::intmax_t Den>
struct dimension_pow_impl<D, Num, Den> {
  using type = TYPENAME dimension_pow_impl<downcast_base_t<D>, Num, Den>::type;
};

template<typename... Es, std::intmax_t Num, std::intmax_t Den>
struct dimension_pow_impl<derived_dimension_base<Es...>, Num, Den> {
  using type = downcast_dimension<derived_dimension_base<exponent_multiply<Es, Num, Den>...>>;
};

}  // namespace detail

template<Dimension D, std::intmax_t Num, std::intmax_t Den = 1>
using dimension_pow = TYPENAME detail::dimension_pow_impl<D, Num, Den>::type;

template<Dimension D>
using dimension_sqrt = TYPENAME detail::dimension_pow_impl<D, 1, 2>::type;

}  // namespace units
