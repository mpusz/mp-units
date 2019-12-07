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

#include <units/derived_dimension.h>
#include <units/unit.h>

namespace units {

/**
 * @brief Unknown dimension
 * 
 * Sometimes a temporary partial result of a complex calculation may not result in a predefined
 * dimension. In such a case an `unknown_dimension` is created with a coherent unit of `unknown_unit`
 * and ratio<1>.
 * 
 * @tparam Es zero or more exponents of a derived dimension
 */
template<Exponent... Es>
struct unknown_dimension : derived_dimension<unknown_dimension<Es...>, scaled_unit<unknown_unit, ratio<1>>, Es...> {
  using coherent_unit = scaled_unit<unknown_unit, ratio<1>>;
};

namespace detail {

template<Dimension D>
struct downcast_dimension_impl {
  using type = D;
};

// downcast did not find user predefined type
template<typename... Es>
struct downcast_dimension_impl<derived_dimension<Es...>> {
  using type = unknown_dimension<Es...>;
};

} // namespace detail

template<Dimension D>
using downcast_dimension = detail::downcast_dimension_impl<downcast<D>>::type;

// dim_invert
namespace detail {

template<Dimension D>
struct dim_invert_impl;

template<BaseDimension D>
struct dim_invert_impl<D> {
  using type = downcast_dimension<derived_dimension<exp<D, -1>>>;
};

template<BaseDimension D>
struct dim_invert_impl<derived_dimension<exp<D, -1>>> {
  using type = D;
};

template<typename... Es>
struct dim_invert_impl<derived_dimension<Es...>> {
  using type = downcast_dimension<derived_dimension<exp_invert<Es>...>>;
};

template<DerivedDimension D>
struct dim_invert_impl<D> : dim_invert_impl<downcast_base_t<D>> {
};

}  // namespace detail

template<Dimension D>
using dim_invert = detail::dim_invert_impl<D>::type;

// dimension_multiply
namespace detail {

template<Dimension D>
struct dim_unpack {
  using type = D;
};

template<BaseDimension D>
struct dim_unpack<derived_dimension<exp<D, 1>>> {
  using type = D;
};

/**
 * @brief Merges 2 sorted derived dimensions into one units::derived_dimension
 * 
 * A result of a dimensional calculation may result with many exponents of the same base dimension orginated
 * from different parts of the equation. As the exponents lists of both operands it is enough to merge them
 * into one list and consolidate duplicates. Also it is possible that final exponents list will contain only
 * one element being a base dimension with exponent 1. In such a case the final dimension should be the base
 * dimension itself.
 */
template<Dimension D1, Dimension D2>
using merge_dimension = dim_unpack<typename dim_consolidate<type_list_merge_sorted<D1, D2, exp_less>>::type>::type;

template<Dimension D1, Dimension D2>
struct dimension_multiply_impl;

template<BaseDimension D1, BaseDimension D2>
struct dimension_multiply_impl<D1, D2> {
  using type = downcast_dimension<merge_dimension<derived_dimension<exp<D1, 1>>, derived_dimension<exp<D2, 1>>>>;
};

template<BaseDimension D1, DerivedDimension D2>
struct dimension_multiply_impl<D1, D2> {
  using type = downcast_dimension<merge_dimension<derived_dimension<exp<D1, 1>>, typename D2::downcast_base_type>>;
};

template<DerivedDimension D1, BaseDimension D2>
struct dimension_multiply_impl<D1, D2> {
  using type = dimension_multiply_impl<D2, D1>::type;
};

template<DerivedDimension D1, DerivedDimension D2>
struct dimension_multiply_impl<D1, D2> {
  using type = downcast_dimension<merge_dimension<typename D1::downcast_base_type, typename D2::downcast_base_type>>;
};

}  // namespace detail

template<Dimension D1, Dimension D2>
using dimension_multiply = detail::dimension_multiply_impl<D1, D2>::type;

template<Dimension D1, Dimension D2>
using dimension_divide = detail::dimension_multiply_impl<D1, dim_invert<D2>>::type;

// dimension_sqrt
namespace detail {

template<Dimension D>
struct dimension_sqrt_impl;

template<BaseDimension D>
struct dimension_sqrt_impl<D> {
  using type = derived_dimension<exp<D, 1, 2>>;
};

template<BaseDimension D>
struct dimension_sqrt_impl<derived_dimension<exp<D, 2>>> {
  using type = D;
};

template<DerivedDimension D>
struct dimension_sqrt_impl<D> {
  using type = dimension_sqrt_impl<typename D::downcast_base_type>;
};

template<typename... Es>
struct dimension_sqrt_impl<derived_dimension<Es...>> {
  using type = downcast_dimension<derived_dimension<exp_multiply<Es, 1, 2>...>>;
};

}  // namespace detail

template<Dimension D>
using dimension_sqrt = detail::dimension_sqrt_impl<typename D::downcast_base_type>::type;

// dimension_pow
namespace detail {

template<Dimension D, std::size_t N>
struct dimension_pow_impl;

template<BaseDimension D, std::size_t N>
struct dimension_pow_impl<D, N> {
  using type = downcast_dimension<derived_dimension<exp<D, N>>>;
};

template<BaseDimension D>
struct dimension_pow_impl<D, 1> {
  using type = D;
};

template<BaseDimension D, std::size_t N>
struct dimension_pow_impl<derived_dimension<exp<D, 1, N>>, N> {
  using type = D;
};

template<DerivedDimension D, std::size_t N>
struct dimension_pow_impl<D, N> {
  using type = dimension_pow_impl<downcast_base_t<D>, N>::type;
};

template<typename... Es, std::size_t N>
struct dimension_pow_impl<derived_dimension<Es...>, N> {
  using type = downcast_dimension<derived_dimension<exp_multiply<Es, N, 1>...>>;
}; 

}  // namespace detail

template<Dimension D, std::size_t N>
using dimension_pow = detail::dimension_pow_impl<D, N>::type;

}  // namespace units
