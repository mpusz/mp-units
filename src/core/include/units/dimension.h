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
#include <units/bits/external/type_traits.h>
#include <units/bits/symbol_text.h>

namespace units {

/**
 * @brief A dimension of a base quantity
 *
 * Base quantity is a quantity in a conventionally chosen subset of a given system of quantities, where no quantity
 * in the subset can be expressed in terms of the other quantities within that subset. They are referred to as
 * being mutually independent since a base quantity cannot be expressed as a product of powers of the other base
 * quantities.
 *
 * `Symbol` template parameter is an unique identifier of the base dimension. The same identifiers can be multiplied
 * and divided which will result with an adjustment of its factor in an exponent of a `derived_dimension`
 * (in case of zero the dimension will be simplified and removed from further analysis of current expresion).
 *
 * User should derive a strong type from this class template rather than use it directly in the source code.
 * For example:
 *
 * @code{.cpp}
 * inline constexpr struct dim_length : base_dimension<"L"> {} dim_length;
 * inline constexpr struct dim_time : base_dimension<"T"> {} dim_time;
 * inline constexpr struct dim_mass : base_dimension<"M"> {} dim_mass;
 * @endcode
 *
 * @note A common convention in this library is to assign the same name for a type and an object of this type.
 *       Besides defining them user never works with the dimension types in the source code. All operations
 *       are done on the objects. Contrarily, the dimension types are the only one visible in the compilation
 *       errors. Having them of the same names improves user experience and somehow blurs those separate domains.
 *
 * @tparam Symbol an unique identifier of the base dimension used to provide dimensional analysis support
 */
template<basic_symbol_text Symbol>
struct base_dimension {
  static constexpr auto symbol = Symbol;  ///< Unique base dimension identifier
};

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
concept BaseDimension = requires(T* t) { detail::to_base_base_dimension(t); } &&
                        (!detail::is_specialization_of_base_dimension<T>);

namespace detail {

template<BaseDimension Lhs, BaseDimension Rhs>
struct base_dimension_less : std::bool_constant<(Lhs::symbol < Rhs::symbol)> {};

template<typename T1, typename T2>
using type_list_of_base_dimension_less = expr_less<T1, T2, base_dimension_less>;

template<typename T>
inline constexpr bool is_dimension_one = false;

template<typename T>
inline constexpr bool is_power_of_dim = requires {
  requires is_specialization_of_power<T> && (BaseDimension<typename T::factor> || is_dimension_one<typename T::factor>);
};

template<typename T>
inline constexpr bool is_per_of_dims = false;

template<typename... Ts>
inline constexpr bool is_per_of_dims<per<Ts...>> =
  (... && (BaseDimension<Ts> || is_dimension_one<Ts> || is_power_of_dim<Ts>));

}  // namespace detail

template<typename T>
concept DerivedDimensionExpr =
  BaseDimension<T> || detail::is_dimension_one<T> || detail::is_power_of_dim<T> || detail::is_per_of_dims<T>;

/**
 * @brief A dimension of a derived quantity
 *
 * Derived dimension is an expression of the dependence of a quantity on the base quantities of a system of quantities
 * as a product of powers of factors corresponding to the base quantities, omitting any numerical factors.
 *
 * Instead of using a raw list of exponents this library decided to use expression template syntax to make types
 * more digestable for the user. The positive exponents are ordered first and all negative exponents are put as a list
 * into the `per<...>` class template. If a power of exponent is different than `1` the dimension type is enclosed in
 * `power<Dim, Num, Den>` class template. Otherwise, it is just put directly in the list without any wrapper. There
 * is also one special case. In case all of the exponents are negative than the `dimension_one` being a dimension of
 * a dimensionless quantity is put in the front to increase the readability.
 *
 * For example:
 *
 * @code{.cpp}
 * using frequency = decltype(1 / dim_time);
 * using speed = decltype(dim_length / dim_time);
 * using acceleration = decltype(dim_speed / dim_time);
 * using force = decltype(dim_mass * dim_acceleration);
 * using energy = decltype(dim_force * dim_length);
 * using moment_of_force = decltype(dim_length * dim_force);
 * using torque = decltype(dim_moment_of_force);
 * @endcode
 *
 * - `frequency` will be derived from type `derived_dimension<dimension_one, per<dim_time>>`
 * - `speed` will be derived from type `derived_dimension<dim_length, per<dim_time>>`
 * - `acceleration` will be derived from type `derived_dimension<dim_length, per<power<dim_time, 2>>>`
 * - `force` will be derived from type `derived_dimension<dim_length, dim_mass, per<power<dim_time, 2>>>`
 * - `energy` will be derived from type `derived_dimension<power<dim_length, 2>, dim_mass, per<power<dim_time, 2>>>`
 *
 * @note A common convention in this library is to assign the same name for a type and an object of this type.
 *       Besides defining them user never works with the dimension types in the source code. All operations
 *       are done on the objects. Contrarily, the dimension types are the only one visible in the compilation
 *       errors. Having them of the same names improves user experience and somehow blurs those separate domains.
 *
 * @tparam Ds a parameter pack consisting tokens allowed in the dimension specification
 *         (base dimensions, `dimension_one`, `power<Dim, Num, Den>`, `per<...>`)
 *
 * @note User should not instantiate this type! It is not exported from the C++ module. The library will
 *       instantiate this type automatically based on the dimensional arithmetic equation provided by the user.
 */
template<DerivedDimensionExpr... Ds>
struct derived_dimension : detail::expr_fractions<derived_dimension<>, Ds...> {};

/**
 * @brief Dimension one
 *
 * Dimension for which all the exponents of the factors corresponding to the base
 * dimensions are zero. It is a dimension of a quantity of dimension one also known as
 * "dimensionless".
 */
inline constexpr struct dimension_one : derived_dimension<> {
} dimension_one;

namespace detail {

template<>
inline constexpr bool is_dimension_one<struct dimension_one> = true;

template<typename... Ds>
void to_base_specialization_of_derived_dimension(const volatile derived_dimension<Ds...>*);

template<typename T>
inline constexpr bool is_derived_from_specialization_of_derived_dimension =
  requires(T * t) { to_base_specialization_of_derived_dimension(t); };

}  // namespace detail

/**
 * @brief A concept matching all derived dimensions in the library.
 *
 * Satisfied by all dimension types either being a specialization of `derived_dimension`
 * or derived from it (inheritance needed to properly handle `dimension_one`).
 */
template<typename T>
concept DerivedDimension = detail::is_derived_from_specialization_of_derived_dimension<T>;

/**
 * @brief A concept matching all dimensions in the library.
 *
 * Satisfied by all dimension types for which either `BaseDimension<T>` or `DerivedDimension<T>` is `true`.
 */
template<typename T>
concept Dimension = BaseDimension<T> || DerivedDimension<T>;


// Operators

template<Dimension Lhs, Dimension Rhs>
[[nodiscard]] consteval Dimension auto operator*(Lhs, Rhs)
{
  return detail::expr_multiply<derived_dimension, struct dimension_one, detail::type_list_of_base_dimension_less>(
    Lhs{}, Rhs{});
}

template<Dimension Lhs, Dimension Rhs>
[[nodiscard]] consteval Dimension auto operator/(Lhs, Rhs)
{
  return detail::expr_divide<derived_dimension, struct dimension_one, detail::type_list_of_base_dimension_less>(Lhs{},
                                                                                                                Rhs{});
}

template<Dimension D>
[[nodiscard]] consteval Dimension auto operator/(int value, D)
{
  gsl_Expects(value == 1);
  return detail::expr_invert<derived_dimension, struct dimension_one>(D{});
}

template<Dimension D>
[[nodiscard]] consteval Dimension auto operator/(D, int) = delete;

template<Dimension Lhs, Dimension Rhs>
[[nodiscard]] consteval bool operator==(Lhs, Rhs)
{
  return is_same_v<Lhs, Rhs>;
}

/**
 * @brief Computes the value of a dimension raised to the `Num/Den` power
 *
 * @tparam Num Exponent numerator
 * @tparam Den Exponent denominator
 * @param d Dimension being the base of the operation
 *
 * @return Dimension The result of computation
 */
template<std::intmax_t Num, std::intmax_t Den = 1, Dimension D>
  requires detail::non_zero<Den>
[[nodiscard]] consteval Dimension auto pow(D d)
{
  if constexpr (BaseDimension<D>) {
    if constexpr (Den == 1)
      return derived_dimension<power<D, Num>>{};
    else
      return derived_dimension<power<D, Num, Den>>{};
  } else
    return detail::expr_pow<Num, Den, derived_dimension, struct dimension_one,
                            detail::type_list_of_base_dimension_less>(d);
}

// TODO consider adding the support for text output of the dimensional equation

}  // namespace units
