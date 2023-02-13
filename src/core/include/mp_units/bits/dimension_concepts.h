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

#include <mp_units/bits/expression_template.h>
#include <mp_units/bits/external/type_traits.h>
#include <mp_units/bits/symbol_text.h>

namespace mp_units {

template<basic_symbol_text Symbol>
struct base_dimension;

namespace detail {

template<basic_symbol_text Symbol>
void to_base_base_dimension(const volatile base_dimension<Symbol>*);

template<typename T>
inline constexpr bool is_specialization_of_base_dimension = false;

template<basic_symbol_text Symbol>
inline constexpr bool is_specialization_of_base_dimension<base_dimension<Symbol>> = true;

}  // namespace detail

/**
 * @brief A concept matching all named base dimensions in the library.
 *
 * Satisfied by all dimension types derived from a specialization of `base_dimension`.
 */
template<typename T>
concept BaseDimension =
  requires(T* t) { detail::to_base_base_dimension(t); } && (!detail::is_specialization_of_base_dimension<T>);

namespace detail {

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

}  // namespace detail

template<typename T>
concept DerivedDimensionExpr =
  BaseDimension<T> || detail::is_dimension_one<T>::value || detail::is_power_of_dim<T> || detail::is_per_of_dims<T>;

template<DerivedDimensionExpr... Expr>
struct derived_dimension;

namespace detail {

/**
 * @brief A concept matching all derived dimensions in the library.
 *
 * Satisfied by all dimension types either being a specialization of `derived_dimension`
 * or derived from it (inheritance needed to properly handle `dimension_one`).
 */
template<typename T>
concept DerivedDimension = is_derived_from_specialization_of<T, derived_dimension>;

}  // namespace detail

/**
 * @brief A concept matching all dimensions in the library.
 *
 * Satisfied by all dimension types for which either `BaseDimension<T>` or `DerivedDimension<T>` is `true`.
 */
template<typename T>
concept Dimension = BaseDimension<T> || detail::DerivedDimension<T>;

}  // namespace mp_units
