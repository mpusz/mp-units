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

#include <mp-units/bits/expression_template.h>
#include <mp-units/bits/external/type_traits.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/bits/symbol_text.h>

namespace mp_units {

MP_UNITS_EXPORT template<symbol_text Symbol>
struct base_dimension;

namespace detail {

template<symbol_text Symbol>
void to_base_specialization_of_base_dimension(const volatile base_dimension<Symbol>*);

template<typename T>
inline constexpr bool is_derived_from_specialization_of_base_dimension =
  requires(T* t) { to_base_specialization_of_base_dimension(t); };

template<typename T>
inline constexpr bool is_specialization_of_base_dimension = false;

template<symbol_text Symbol>
inline constexpr bool is_specialization_of_base_dimension<base_dimension<Symbol>> = true;

/**
 * @brief A concept matching all named base dimensions in the library.
 *
 * Satisfied by all dimension types derived from a specialization of `base_dimension`.
 */
template<typename T>
concept BaseDimension =
  is_derived_from_specialization_of_base_dimension<T> && (!is_specialization_of_base_dimension<T>);

template<typename T>
struct is_dimension_one : std::false_type {};

template<typename T>
inline constexpr bool is_power_of_dim = requires {
  requires is_specialization_of_power<T> &&
             (BaseDimension<typename T::factor> || is_dimension_one<typename T::factor>::value);
};

template<typename T>
inline constexpr bool is_per_of_dims = false;

template<typename... Ts>
inline constexpr bool is_per_of_dims<per<Ts...>> =
  (... && (BaseDimension<Ts> || is_dimension_one<Ts>::value || is_power_of_dim<Ts>));

template<typename T>
concept DerivedDimensionExpr =
  BaseDimension<T> || is_dimension_one<T>::value || is_power_of_dim<T> || is_per_of_dims<T>;

}  // namespace detail

template<detail::DerivedDimensionExpr... Expr>
struct derived_dimension;

namespace detail {

/**
 * @brief A concept matching all derived dimensions in the library.
 *
 * Satisfied by all dimension types being a specialization of `derived_dimension` or
 * being the `dimension_one`.
 */
template<typename T>
concept DerivedDimension = is_specialization_of<T, derived_dimension> || is_dimension_one<T>::value;

}  // namespace detail

/**
 * @brief A concept matching all dimensions in the library.
 *
 * Satisfied by all dimension types for which either `BaseDimension<T>` or `DerivedDimension<T>` is `true`.
 */
MP_UNITS_EXPORT template<typename T>
concept Dimension = detail::BaseDimension<T> || detail::DerivedDimension<T>;

/**
 * @brief A concept checking if the argument is of the same dimension.
 *
 * Satisfied when both argument satisfy a `Dimension` concept and when they compare equal.
 */
MP_UNITS_EXPORT template<typename T, auto D>
concept DimensionOf = Dimension<T> && Dimension<std::remove_const_t<decltype(D)>> && (T{} == D);

}  // namespace mp_units
