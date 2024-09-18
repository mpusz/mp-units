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

// IWYU pragma: private, include <mp-units/framework.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/framework/expression_template.h>
#include <mp-units/framework/magnitude.h>
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/symbol_text.h>

namespace mp_units {

namespace detail {

struct unit_interface;

}  // namespace detail

/**
 * @brief A concept matching all unit types in the library
 *
 * Satisfied by all unit types provided by the library.
 */
MP_UNITS_EXPORT template<typename T>
concept Unit = std::derived_from<T, detail::unit_interface> && std::is_final_v<T>;

template<Magnitude auto M, Unit U>
struct scaled_unit;

MP_UNITS_EXPORT template<symbol_text Symbol, auto...>
struct named_unit;

MP_UNITS_EXPORT struct one;

namespace detail {

template<symbol_text Symbol, auto... Args>
void to_base_specialization_of_named_unit(const volatile named_unit<Symbol, Args...>*);

template<typename T>
constexpr bool is_derived_from_specialization_of_named_unit =
  requires(T* t) { to_base_specialization_of_named_unit(t); };

}  // namespace detail

/**
 * @brief A concept to be used to define prefixes for a unit
 */
MP_UNITS_EXPORT template<typename T>
concept PrefixableUnit = Unit<T> && detail::is_derived_from_specialization_of_named_unit<T>;

namespace detail {

template<typename T>
constexpr bool is_power_of_unit = requires { requires is_specialization_of_power<T> && Unit<typename T::factor>; };

template<typename T>
constexpr bool is_per_of_units = false;

template<typename... Ts>
constexpr bool is_per_of_units<per<Ts...>> = (... && (Unit<Ts> || is_power_of_unit<Ts>));

template<typename T>
concept DerivedUnitExpr = Unit<T> || detail::is_power_of_unit<T> || detail::is_per_of_units<T>;

}  // namespace detail

template<detail::DerivedUnitExpr... Expr>
struct derived_unit;

MP_UNITS_EXPORT template<symbol_text Symbol, Magnitude auto M, PrefixableUnit auto U>
  requires(!Symbol.empty())
struct prefixed_unit;

namespace detail {

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
concept UnitOf = AssociatedUnit<U> && QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(QS))> &&
                 detail::QuantitySpecConvertibleTo<get_quantity_spec(U{}), QS> &&
                 // the below is to make `dimensionless[radian]` invalid
                 (detail::SameQuantitySpec<get_kind(QS), get_kind(get_quantity_spec(U{}))> ||
                  !detail::NestedQuantityKindSpecOf<get_quantity_spec(U{}), QS>);

MP_UNITS_EXPORT template<Unit From, Unit To>
[[nodiscard]] consteval bool convertible(From from, To to);

namespace detail {

template<auto From, auto To>
concept UnitConvertibleTo =
  Unit<MP_UNITS_REMOVE_CONST(decltype(From))> && Unit<MP_UNITS_REMOVE_CONST(decltype(To))> && (convertible(From, To));

/**
 * @brief A concept matching all units compatible with the provided unit and quantity spec
 *
 * Satisfied by all units that have the same canonical reference as `U2` and in case they
 * have associated quantity specification it should satisfy `UnitOf<QS>`.
 */
MP_UNITS_EXPORT template<typename U, auto FromU, auto QS>
concept UnitCompatibleWith =
  Unit<U> && Unit<MP_UNITS_REMOVE_CONST(decltype(FromU))> && QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(QS))> &&
  (!AssociatedUnit<U> || UnitOf<U, QS>) && detail::UnitConvertibleTo<FromU, U{}>;

template<typename T>
concept OffsetUnit = Unit<T> && requires { T::point_origin; };

}  // namespace detail

}  // namespace mp_units
