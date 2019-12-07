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
#include <units/bits/downcasting.h>
#include <units/bits/fixed_string.h>
#include <units/bits/type_list.h>
#include <units/ratio.h>
#include <ratio>

namespace units {

// Exponent
namespace detail {

template<typename T>
inline constexpr bool is_exp = false;

// partial specialization for an exp type provided below

}  // namespace detail

template<typename T>
concept Exponent = detail::is_exp<T>;

/**
 * @brief A derived dimension
 * 
 * There are 2 partial specializations of this primary class template. One of them is used by the library engine
 * and another one is the interface for the user to define a derived dimension.
 */
template<typename...>
struct derived_dimension;

/**
 * @brief Dimensionless quantity
 */
template<>
struct derived_dimension<> : downcast_base<derived_dimension<>> {};

/**
 * @brief A dimension of a derived quantity
 *
 * Expression of the dependence of a quantity on the base quantities (and their base dimensions) of a system of
 * quantities as a product of powers of factors corresponding to the base quantities, omitting any numerical factors.
 * A power of a factor is the factor raised to an exponent.
 * 
 * A derived dimension can be formed from multiple exponents (i.e. velocity is represented as "exp<L, 1>, exp<T, -1>").
 * It is also possible to form a derived dimension with only one exponent (i.e. frequency is represented as just
 * "exp<T, -1>").
 * 
 * @note This partial class template specialization is used by the library engine and should not be directly instantiated
 * by the user (see the other partial specialization).
 * 
 * @tparam E a first exponent of a derived dimension
 * @tparam ERest zero or more following exponents of a derived dimension
 */
template<Exponent E, Exponent... ERest>
struct derived_dimension<E, ERest...> : downcast_base<derived_dimension<E, ERest...>> {};

// DerivedDimension
template<typename T>
concept DerivedDimension = std::is_empty_v<T> && is_instantiation<downcast_base_t<T>, derived_dimension>;

// Dimension
template<typename T>
concept Dimension = BaseDimension<T> || DerivedDimension<T>;

/**
 * @brief A power of factor corresponding to the dimension of a quantity
 *
 * @tparam Dim component dimension of a derived quantity
 * @tparam Num numinator of the factor
 * @tparam Den denominator of the factor
 */
template<Dimension Dim, int Num, int Den = 1>
struct exp {
  using dimension = Dim;
  static constexpr int num = Num;
  static constexpr int den = Den;
};

// is_exp
namespace detail {

template<typename Dim, int Num, int Den>
inline constexpr bool is_exp<exp<Dim, Num, Den>> = true;

}  // namespace detail

// exp_less
template<Exponent E1, Exponent E2>
struct exp_less : base_dimension_less<typename E1::dimension, typename E2::dimension> {};

// exp_invert
namespace detail {

template<typename Dim, int Num, int Den>
constexpr exp<Dim, -Num, Den> exp_invert_impl(exp<Dim, Num, Den>);

}  // namespace detail

template<Exponent E>
using exp_invert = decltype(detail::exp_invert_impl(E()));

// exp_multiply
namespace detail {

template<Exponent E, int Num, int Den>
struct exp_multiply_impl {
  using r1 = ratio<E::num, E::den>;
  using r2 = ratio<Num, Den>;
  using r = ratio_multiply<r1, r2>;
  using type = exp<typename E::dimension, r::num, r::den>;
};

}  // namespace detail

template<Exponent E, int Num, int Den>
using exp_multiply = detail::exp_multiply_impl<E, Num, Den>::type;

// make_dimension
namespace detail {

/**
 * @brief Consolidates contiguous ranges of exponents of the same dimension
 * 
 * If there is more than one exponent with the same dimension they are aggregated into one exponent by adding
 * their exponents. If this accumulation will result with 0, such a dimension is removed from the list.
 * 
 * @tparam D derived dimension to consolidate
 */
template<DerivedDimension D>
struct dim_consolidate;

template<>
struct dim_consolidate<derived_dimension<>> {
  using type = derived_dimension<>;
};

template<typename E>
struct dim_consolidate<derived_dimension<E>> {
  using type = derived_dimension<E>;
};

template<typename E1, typename... ERest>
struct dim_consolidate<derived_dimension<E1, ERest...>> {
  using type = type_list_push_front<typename dim_consolidate<derived_dimension<ERest...>>::type, E1>;
};

template<BaseDimension Dim, int Num1, int Den1, int Num2, int Den2, typename... ERest>
struct dim_consolidate<derived_dimension<exp<Dim, Num1, Den1>, exp<Dim, Num2, Den2>, ERest...>> {
  // TODO: provide custom implementation for ratio_add
  using r1 = std::ratio<Num1, Den1>;
  using r2 = std::ratio<Num2, Den2>;
  using r = std::ratio_add<r1, r2>;
  using type = conditional<r::num == 0, typename dim_consolidate<derived_dimension<ERest...>>::type,
                           typename dim_consolidate<derived_dimension<exp<Dim, r::num, r::den>, ERest...>>::type>;
};

/**
 * @brief Extracts the list of potentially derived dimensions to a list containing only base dimensions
 * 
 * @tparam Es Exponents of potentially derived dimensions 
 */
template<Exponent... Es>
struct extract;

template<>
struct extract<> {
  using type = derived_dimension<>;
};

template<BaseDimension Dim, int Num, int Den, Exponent... ERest>
struct extract<exp<Dim, Num, Den>, ERest...> {
  using type = type_list_push_front<typename extract<ERest...>::type, exp<Dim, Num, Den>>;
};

template<DerivedDimension Dim, int Num, int Den, Exponent... ERest>
struct extract<exp<Dim, Num, Den>, ERest...> {
  using type = extract<exp<downcast_base_t<Dim>, Num, Den>, ERest...>::type;
};

template<Exponent... Es, int Num, int Den, Exponent... ERest>
struct extract<exp<derived_dimension<Es...>, Num, Den>, ERest...> {
  using type = type_list_push_front<typename extract<ERest...>::type, exp_multiply<Es, Num, Den>...>;
};

/**
 * @brief Converts user provided derived dimension specification into a valid units::derived_dimension definition
 * 
 * User provided definition of a derived dimension may contain the same base dimension repeated more than once on the
 * list possibly hidden in other derived units provided by the user. The process here should:
 * 1. Extract derived dimensions into exponents of base dimensions.
 * 2. Sort the exponents so the same dimensions are placed next to each other.
 * 3. Consolidate contiguous range of exponents of the same base dimensions to a one (or possibly zero) exponent for
 *    this base dimension.
 */
template<Exponent... Es>
using make_dimension = dim_consolidate<type_list_sort<typename extract<Es...>::type, exp_less>>::type;

}  // namespace detail

/**
 * @brief The list of exponents of dimensions (both base and derived) provided by the user
 * 
 * This is the primary interface to create derived dimensions. Exponents list can contain powers of factors of both
 * base and derived dimensions. This is called a "recipe" of the dimension and among others is used to print
 * unnamed coherent units of this dimension.
 * 
 * Coherent unit is a unit that, for a given system of quantities and for a chosen set of base units, is a product
 * of powers of base units with no other proportionality factor than one.
 *
 * The implementation is responsible for unpacking all of the dimensions into a list containing only base dimensions
 * and their factors and putting them to the other (private) units::derived_dimension class template partial
 * specialization.
 *  
 * @note User should always use only this partial specialization to create derived dimensions.
 * 
 * @tparam Child inherited class type used by the downcasting facility (CRTP Idiom)
 * @tparam E The list of exponents of ingredient dimensions
 * @tparam ERest The list of exponents of ingredient dimensions
 */
template<typename Child, Unit U, Exponent E, Exponent... ERest>
  requires (!Exponent<Child>)
struct derived_dimension<Child, U, E, ERest...> : downcast_child<Child, typename detail::make_dimension<E, ERest...>> {
  using recipe = derived_dimension<E, ERest...>;
  using coherent_unit = U;
};

// same_dim
template<Dimension D1, Dimension D2>
inline constexpr bool same_dim = false;

template<BaseDimension D1, BaseDimension D2>
inline constexpr bool same_dim<D1, D2> = std::is_same_v<D1, D2>;

template<DerivedDimension D1, DerivedDimension D2>
inline constexpr bool same_dim<D1, D2> = std::is_same_v<typename D1::downcast_base_type, typename D2::downcast_base_type>;

}  // namespace units
