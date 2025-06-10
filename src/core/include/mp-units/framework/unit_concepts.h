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
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/symbolic_expression.h>
#include <mp-units/framework/unit_magnitude.h>

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
concept Unit = std::derived_from<T, detail::unit_interface> && detail::SymbolicConstant<T>;

MP_UNITS_EXPORT template<symbol_text Symbol, auto...>
struct named_unit;

/**
 * @brief A concept to be used to define prefixes for a unit
 */
MP_UNITS_EXPORT template<typename T>
concept PrefixableUnit = Unit<T> && is_derived_from_specialization_of_v<T, named_unit>;

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
  if constexpr (requires { U::_quantity_spec_; }) return true;
  if constexpr (requires { U::_reference_unit_; })
    return has_associated_quantity(U::_reference_unit_);
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
 * Satisfied by all units for which an associated quantity spec is implicitly convertible to
 * the provided @c QS value.
 */
MP_UNITS_EXPORT template<typename U, auto QS>
concept UnitOf = AssociatedUnit<U> && QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(QS))> &&
                 (implicitly_convertible(get_quantity_spec(U{}), QS));

namespace detail {

template<typename U, auto QS>
concept WeakUnitOf =
  Unit<U> && QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(QS))> && ((!AssociatedUnit<U>) || UnitOf<U, QS>);

template<auto U1, auto U2>
concept UnitsOfCompatibleQuantities = explicitly_convertible(get_quantity_spec(U1), get_quantity_spec(U2));

template<auto U1, auto U2>
concept ConvertibleUnits = (get_canonical_unit(U1).reference_unit == get_canonical_unit(U2).reference_unit);

template<typename U1, auto U2>
concept UnitConvertibleTo =
  Unit<U1> && Unit<MP_UNITS_REMOVE_CONST(decltype(U2))> &&
  ((U1{} == U2) || ((!AssociatedUnit<U1> || !AssociatedUnit<MP_UNITS_REMOVE_CONST(decltype(U2))> ||
                     UnitsOfCompatibleQuantities<U1{}, U2>) &&
                    ConvertibleUnits<U1{}, U2>));

template<typename T>
concept OffsetUnit = Unit<T> && requires { T::_point_origin_; };

}  // namespace detail

}  // namespace mp_units
