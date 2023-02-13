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
#include <mp_units/bits/magnitude.h>
#include <mp_units/bits/symbol_text.h>

namespace mp_units {

namespace detail {

template<typename T>
inline constexpr bool is_unit = false;

}  // namespace detail

/**
 * @brief A concept matching all unit types in the library
 *
 * Satisfied by all unit types provided by the library.
 */
template<typename T>
concept Unit = detail::is_unit<T>;

template<Magnitude auto M, Unit U>
struct scaled_unit;

template<basic_symbol_text Symbol, auto...>
struct named_unit;

namespace detail {

template<basic_symbol_text Symbol, auto... Args>
void to_base_specialization_of_named_unit(const volatile named_unit<Symbol, Args...>*);

template<typename T>
inline constexpr bool is_specialization_of_named_unit = false;

template<basic_symbol_text Symbol, auto... Args>
inline constexpr bool is_specialization_of_named_unit<named_unit<Symbol, Args...>> = true;

}  // namespace detail

/**
 * @brief A concept matching all units with special names
 *
 * Satisfied by all unit types derived from the specialization of `named_unit`.
 */
template<typename T>
concept NamedUnit = Unit<T> && requires(T* t) { detail::to_base_specialization_of_named_unit(t); } &&
                    (!detail::is_specialization_of_named_unit<T>);

/**
 * @brief Prevents assignment of a prefix to specific units
 *
 * By default all named units allow assigning a prefix for them. There are some notable exceptions like
 * `hour` or `degree_Celsius`. For those a partial specialization with the value `false` should be
 * provided.
 */
template<NamedUnit auto V>
inline constexpr bool unit_can_be_prefixed = true;

/**
 * @brief A concept to be used to define prefixes for a unit
 */
template<typename T>
concept PrefixableUnit = NamedUnit<T> && unit_can_be_prefixed<T{}>;

namespace detail {

template<typename T>
inline constexpr bool is_power_of_unit =
  requires { requires is_specialization_of_power<T> && Unit<typename T::factor>; };

template<typename T>
inline constexpr bool is_per_of_units = false;

template<typename... Ts>
inline constexpr bool is_per_of_units<per<Ts...>> = (... && (Unit<Ts> || is_power_of_unit<Ts>));

}  // namespace detail

template<typename T>
concept DerivedUnitExpr = Unit<T> || detail::is_power_of_unit<T> || detail::is_per_of_units<T>;

template<DerivedUnitExpr... Expr>
struct derived_unit;

namespace detail {

template<auto M, typename U>
void is_unit_impl(const scaled_unit<M, U>*);

template<basic_symbol_text Symbol, auto... Args>
void is_unit_impl(const named_unit<Symbol, Args...>*);

template<typename... Expr>
void is_unit_impl(const derived_unit<Expr...>*);

template<typename T>
  requires requires(T* t) { is_unit_impl(t); }
inline constexpr bool is_unit<T> = true;

template<Unit U>
[[nodiscard]] consteval bool has_associated_quantity(U);

template<typename U, auto... Vs>
[[nodiscard]] consteval bool has_associated_quantity(power<U, Vs...>)
{
  return has_associated_quantity(U{});
}

template<typename... Us>
[[nodiscard]] consteval bool has_associated_quantity(type_list<Us...>)
{
  return (... && has_associated_quantity(Us{}));
}

template<Unit U>
[[nodiscard]] consteval bool has_associated_quantity(U)
{
  if constexpr (requires { U::quantity_spec; })
      return true;
  if constexpr (requires { U::reference_unit; })
    return has_associated_quantity(U::reference_unit);
  else if constexpr (requires { typename U::_num_; })
    return has_associated_quantity(typename U::_num_{}) && has_associated_quantity(typename U::_den_{});
  else
    return false;
}

}  // namespace detail

/**
 * @brief A concept matching all units that can be used as quantity references
 */
template<typename U>
concept AssociatedUnit = Unit<U> && detail::has_associated_quantity(U{});

}  // namespace mp_units
