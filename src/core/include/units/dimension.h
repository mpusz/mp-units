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

#include <units/bits/expression_template.h>
#include <units/bits/external/fixed_string.h>
#include <units/bits/external/type_traits.h>

namespace units {

/**
 * @brief A dimension of a base quantity
 *
 * Base quantity is a quantity in a conventionally chosen subset of a given system of quantities, where no quantity
 * in the subset can be expressed in terms of the other quantities within that subset. They are referred to as
 * being mutually independent since a base quantity cannot be expressed as a product of powers of the other base
 * quantities.
 *
 * Symbol template parameters is an unique identifier of the base dimension. The same identifiers can be multiplied
 * and divided which will result with an adjustment of its factor in an Exponent of a DerivedDimension
 * (in case of zero the dimension will be simplified and removed from further analysis of current expresion).
 *
 * @tparam Symbol an unique identifier of the base dimension used to provide dimensional analysis support
 */
template<basic_fixed_string Symbol>
struct base_dimension {
  static constexpr auto symbol = Symbol;  ///< Unique base dimension identifier
};

namespace detail {

template<basic_fixed_string Symbol>
void to_base_base_dimension(const volatile base_dimension<Symbol>*);

}  // namespace detail

/**
 * @brief A concept matching all base dimensions in the library.
 *
 * Satisfied by all dimension types derived from an specialization of `base_dimension`.
 */
template<typename T>
concept BaseDimension = requires(T* t) { detail::to_base_base_dimension(t); };

template<BaseDimension D1, BaseDimension D2>
struct base_dimension_less : std::bool_constant<(D1::symbol < D2::symbol)> {};

// TODO Can we provide a smarter implementation?
std::false_type is_derived_dimension(...);

/**
 * @brief A concept matching all derived dimensions in the library.
 *
 * Satisfied by all dimension types derived from an specialization of `derived_dimension`.
 */
template<typename T>
concept DerivedDimension = decltype(is_derived_dimension(std::declval<T*>()))::value;

/**
 * @brief A concept matching all dimensions in the library.
 *
 * Satisfied by all dimension types for which either `BaseDimension<T>` or `DerivedDimension<T>` is `true`.
 */
template<typename T>
concept Dimension = BaseDimension<T> || DerivedDimension<T>;

namespace detail {


template<typename T1, typename T2>
using type_list_of_base_dimension_less = expr_less<T1, T2, base_dimension_less>;

template<typename T>
inline constexpr bool is_one_dim = false;

}  // namespace detail

// TODO add checking for `per` and power elements as well
template<typename T>
concept DimensionSpec =
  BaseDimension<T> || detail::is_one_dim<T> || is_specialization_of<T, per> || detail::is_specialization_of_power<T>;

// User should not instantiate this type!!!
// It should not be exported from the module
template<DimensionSpec... Ds>
struct derived_dimension : detail::expr_fractions<derived_dimension<>, Ds...> {
  using type = derived_dimension<Ds...>;
};

template<typename... Args>
std::true_type is_derived_dimension(const volatile derived_dimension<Args...>*);

/**
 * @brief Dimension one
 *
 * Dimension for which all the exponents of the factors corresponding to the base
 * dimensions are zero. Also commonly named as "dimensionless".
 */
inline constexpr struct one_dim : derived_dimension<> {
} one_dim;

namespace detail {

template<>
inline constexpr bool is_one_dim<struct one_dim> = true;

template<Dimension T>
struct dim_type_impl {
  using type = T;
};

template<DerivedDimension T>
struct dim_type_impl<T> {
  using type = T::type;
};

template<Dimension T>
using dim_type = dim_type_impl<T>::type;

}  // namespace detail

template<Dimension D1, Dimension D2>
[[nodiscard]] constexpr Dimension auto operator*(D1, D2)
{
  return detail::expr_multiply<detail::dim_type<D1>, detail::dim_type<D2>, struct one_dim,
                               detail::type_list_of_base_dimension_less, derived_dimension>();
}

template<Dimension D1, Dimension D2>
[[nodiscard]] constexpr Dimension auto operator/(D1, D2)
{
  return detail::expr_divide<detail::dim_type<D1>, detail::dim_type<D2>, struct one_dim,
                             detail::type_list_of_base_dimension_less, derived_dimension>();
}

template<Dimension D>
[[nodiscard]] constexpr Dimension auto operator/(int value, D)
{
  gsl_Assert(value == 1);
  return detail::expr_invert<detail::dim_type<D>, struct one_dim, derived_dimension>();
}

template<Dimension D1, Dimension D2>
[[nodiscard]] constexpr bool operator==(D1, D2)
{
  return is_same_v<detail::dim_type<D1>, detail::dim_type<D2>>;
}

// TODO consider adding the support for text output of the dimensional equation

}  // namespace units
