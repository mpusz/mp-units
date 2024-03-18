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
#include <mp-units/bits/magnitude.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/bits/quantity_spec_concepts.h>
#include <mp-units/bits/symbol_text.h>

namespace mp_units {

namespace detail {

// do not refactor below to a variable template - GCC-11 does not like it
template<typename T>
struct is_unit : std::false_type {};

}  // namespace detail

/**
 * @brief A concept matching all unit types in the library
 *
 * Satisfied by all unit types provided by the library.
 */
MP_UNITS_EXPORT template<typename T>
concept Unit = detail::is_unit<T>::value;

template<Magnitude auto M, Unit U>
struct scaled_unit;

MP_UNITS_EXPORT template<symbol_text Symbol, auto...>
struct named_unit;

namespace detail {

template<symbol_text Symbol, auto... Args>
void to_base_specialization_of_named_unit(const volatile named_unit<Symbol, Args...>*);

template<typename T>
inline constexpr bool is_derived_from_specialization_of_named_unit =
  requires(T* t) { to_base_specialization_of_named_unit(t); };

template<typename T>
inline constexpr bool is_specialization_of_named_unit = false;

template<symbol_text Symbol, auto... Args>
inline constexpr bool is_specialization_of_named_unit<named_unit<Symbol, Args...>> = true;

/**
 * @brief A concept matching all units with special names
 *
 * Satisfied by all unit types derived from the specialization of `named_unit`.
 */
template<typename T>
concept NamedUnit =
  Unit<T> && detail::is_derived_from_specialization_of_named_unit<T> && (!detail::is_specialization_of_named_unit<T>);

}  // namespace detail

/**
 * @brief Prevents assignment of a prefix to specific units
 *
 * By default all named units allow assigning a prefix for them. There are some notable exceptions like
 * `hour` or `degree_Celsius`. For those a partial specialization with the value `false` should be
 * provided.
 */
MP_UNITS_EXPORT template<Unit auto V>
inline constexpr bool unit_can_be_prefixed = true;

/**
 * @brief A concept to be used to define prefixes for a unit
 */
MP_UNITS_EXPORT template<typename T>
concept PrefixableUnit = detail::NamedUnit<T> && unit_can_be_prefixed<T{}>;

namespace detail {

template<typename T>
inline constexpr bool is_power_of_unit =
  requires { requires is_specialization_of_power<T> && Unit<typename T::factor>; };

template<typename T>
inline constexpr bool is_per_of_units = false;

template<typename... Ts>
inline constexpr bool is_per_of_units<per<Ts...>> = (... && (Unit<Ts> || is_power_of_unit<Ts>));

template<typename T>
concept DerivedUnitExpr = Unit<T> || detail::is_power_of_unit<T> || detail::is_per_of_units<T>;

}  // namespace detail

template<detail::DerivedUnitExpr... Expr>
struct derived_unit;

MP_UNITS_EXPORT template<symbol_text Symbol, Magnitude auto M, PrefixableUnit auto U>
  requires(!Symbol.empty())
struct prefixed_unit;

namespace detail {

template<auto M, typename U>
void is_unit_impl(const scaled_unit<M, U>*);

template<symbol_text Symbol, auto... Args>
void is_unit_impl(const named_unit<Symbol, Args...>*);

template<typename... Expr>
void is_unit_impl(const derived_unit<Expr...>*);

template<typename T>
inline constexpr bool is_specialization_of_unit = false;

template<symbol_text Symbol, auto... Args>
inline constexpr bool is_specialization_of_unit<named_unit<Symbol, Args...>> = true;

template<typename T>
inline constexpr bool is_specialization_of_prefixed_unit = false;

template<symbol_text Symbol, Magnitude auto M, PrefixableUnit auto U>
inline constexpr bool is_specialization_of_prefixed_unit<prefixed_unit<Symbol, M, U>> = true;

template<typename T>
  requires requires(T* t) { is_unit_impl(t); } && (!is_specialization_of_named_unit<T>) &&
           (!is_specialization_of_prefixed_unit<T>)
struct is_unit<T> : std::true_type {};

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
  if constexpr (requires { U::quantity_spec; }) return true;
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
MP_UNITS_EXPORT template<typename U>
concept AssociatedUnit = Unit<U> && detail::has_associated_quantity(U{});

/**
 * @brief A concept matching all units associated with the provided quantity spec
 *
 * Satisfied by all units associated with the quantity_spec being the instantiation derived from
 * the provided quantity_spec type.
 */
MP_UNITS_EXPORT template<typename U, auto QS>
concept UnitOf =
  AssociatedUnit<U> && QuantitySpec<std::remove_const_t<decltype(QS)>> &&
  implicitly_convertible(get_quantity_spec(U{}), QS) &&
  // the below is to make `dimensionless[radian]` invalid
  (get_kind(QS) == get_kind(get_quantity_spec(U{})) || !detail::NestedQuantityKindSpecOf<get_quantity_spec(U{}), QS>);

namespace detail {

[[nodiscard]] consteval bool have_same_canonical_reference_unit(Unit auto u1, Unit auto u2);

}

/**
 * @brief A concept matching all units compatible with the provided unit and quantity spec
 *
 * Satisfied by all units that have the same canonical reference as `U2` and in case they
 * have associated quantity specification it should satisfy `UnitOf<QS>`.
 */
MP_UNITS_EXPORT template<typename U, auto U2, auto QS>
concept UnitCompatibleWith =
  Unit<U> && Unit<std::remove_const_t<decltype(U2)>> && QuantitySpec<std::remove_const_t<decltype(QS)>> &&
  (!AssociatedUnit<U> || UnitOf<U, QS>)&&detail::have_same_canonical_reference_unit(U{}, U2);


}  // namespace mp_units
