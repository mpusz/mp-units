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

#include <units/bits/derived_symbol_text.h>
#include <units/bits/external/downcasting.h>
#include <units/bits/external/type_traits.h>

// IWYU pragma: begin_exports
#include <units/bits/derived_scaled_unit.h>
#include <units/bits/external/fixed_string.h>
#include <units/magnitude.h>
#include <units/prefix.h>
#include <units/ratio.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

namespace units {

namespace detail {

template<typename>
inline constexpr bool can_be_prefixed = false;

}  // namespace detail

/**
 * @brief A common point for a hierarchy of units
 *
 * A unit is an entity defined and adopted by convention, with which any other quantity of
 * the same kind can be compared to express the ratio of the second quantity to the first
 * one as a number.
 *
 * All units of the same dimension can be convereted between each other. To allow this all of
 * them are expressed as different ratios of the same one proprietary chosen reference unit
 * (i.e. all length units are expressed in terms of meter, all mass units are expressed in
 * terms of gram, ...)
 *
 * @tparam M a Magnitude representing the (relative) size of this unit
 * @tparam U a unit to use as a reference for this dimension
 *
 * @note U cannot be constrained with Unit as for some specializations (i.e. named_unit)
 *       it gets the incomplete child's type with the CRTP idiom.
 */
template<Magnitude auto M, typename U>
struct scaled_unit : downcast_base<scaled_unit<M, U>> {
  static constexpr UNITS_CONSTRAINED_AUTO_WORKAROUND(Magnitude) auto mag = M;
  using reference = U;
};

template<Dimension D, Magnitude auto M>
using downcast_unit = downcast<scaled_unit<M, typename dimension_unit<D>::reference>>;

template<Unit U1, Unit U2>
struct same_unit_reference : is_same<typename U1::reference, typename U2::reference> {};

/**
 * @brief A named unit
 *
 * Defines a named (in most cases coherent) unit that is then passed to a dimension definition.
 * A named unit may be composed with a prefix to create a prefixed_unit.
 *
 * @tparam Child inherited class type used by the downcasting facility (CRTP Idiom)
 * @tparam Symbol a short text representation of the unit
 */
template<typename Child, basic_symbol_text Symbol>
struct named_unit : downcast_dispatch<Child, scaled_unit<mag<1>(), Child>> {
  static constexpr auto symbol = Symbol;
};

/**
 * @brief A named scaled unit
 *
 * Defines a new named unit that is a scaled version of another unit.
 * A named unit may be composed with a prefix to create a prefixed_unit.
 *
 * @tparam Child inherited class type used by the downcasting facility (CRTP Idiom)
 * @tparam Symbol a short text representation of the unit
 * @tparam M the Magnitude by which to scale U
 * @tparam U a reference unit to scale
 */
template<typename Child, basic_symbol_text Symbol, Magnitude auto M, Unit U>
struct named_scaled_unit : downcast_dispatch<Child, scaled_unit<M * U::mag, typename U::reference>> {
  static constexpr auto symbol = Symbol;
};

/**
 * @brief A prefixed unit
 *
 * Defines a new unit that is a scaled version of another unit by the provided prefix. It is
 * only possible to create such a unit if the given prefix type matches the one defined in a
 * reference unit.
 *
 * @tparam Child inherited class type used by the downcasting facility (CRTP Idiom)
 * @tparam P prefix to be appied to the reference unit
 * @tparam U reference unit
 */
template<typename Child, Prefix P, NamedUnit U>
  requires detail::can_be_prefixed<U>
struct prefixed_unit : downcast_dispatch<Child, scaled_unit<P::mag * U::mag, typename U::reference>> {
  static constexpr auto symbol = P::symbol + U::symbol;
};

/**
 * @brief A coherent unit of a derived quantity
 *
 * Defines a new coherent unit of a derived quantity. It should be passed as a coherent unit
 * in the dimension's definition for such a quantity.
 *
 * @tparam Child inherited class type used by the downcasting facility (CRTP Idiom)
 */
template<typename Child>
struct derived_unit : downcast_dispatch<Child, scaled_unit<mag<1>(), Child>> {};

/**
 * @brief A unit with a deduced ratio and symbol
 *
 * Defines a new unit with a deduced ratio and symbol based on the recipe from the provided
 * derived dimension. The number and order of provided units should match the recipe of the
 * derived dimension. All of the units provided should also be a named ones so it is possible
 * to create a deduced symbol text.
 *
 * @tparam Child inherited class type used by the downcasting facility (CRTP Idiom)
 * @tparam Dim a derived dimension recipe to use for deduction
 * @tparam U the unit of the first composite dimension from provided derived dimension's recipe
 * @tparam URest the units for the rest of dimensions from the recipe
 */
template<typename Child, DerivedDimension Dim, NamedUnit U, NamedUnit... URest>
  requires detail::compatible_units<typename Dim::recipe, U, URest...>
struct derived_scaled_unit : downcast_dispatch<Child, detail::derived_scaled_unit<Dim, U, URest...>> {
  static constexpr auto symbol = detail::derived_symbol_text<Dim, U, URest...>();
};

/**
 * @brief An aliased named unit
 *
 * Defines a named alias for another unit. It is useful to assign alternative names and symbols
 * to the already predefined units (i.e. "tonne" for "megagram").
 * A alias unit may be composed with a prefix to create a prefixed_alias_unit.
 *
 * @tparam U Unit for which an alias is defined
 * @tparam Symbol a short text representation of the unit
 */
template<Unit U, basic_symbol_text Symbol>
struct alias_unit : U {
  static constexpr auto symbol = Symbol;
};

/**
 * @brief A prefixed alias unit
 *
 * Defines a new unit that is an alias for a scaled version of another unit by the provided
 * prefix. It is only possible to create such a unit if the given prefix type matches the one
 * defined in a reference unit.
 *
 * @tparam U Unit for which an alias is defined
 * @tparam P prefix to be appied to the reference unit
 * @tparam AU reference alias unit
 */
template<Unit U, Prefix P, AliasUnit AU>
  requires(!AliasUnit<U>)
struct prefixed_alias_unit : U {
  static constexpr auto symbol = P::symbol + AU::symbol;
};

/**
 * @brief Unknown coherent unit
 *
 * Used as a coherent unit of an unknown dimension.
 */
template<Exponent... Es>
struct unknown_coherent_unit :
    downcast_dispatch<unknown_coherent_unit<Es...>,
                      scaled_unit<detail::absolute_magnitude(exponent_list<Es...>()), unknown_coherent_unit<Es...>>> {};

namespace detail {

template<typename Child, basic_symbol_text Symbol>
void is_named_impl(const volatile named_unit<Child, Symbol>*);

template<typename Child, basic_symbol_text Symbol, Magnitude auto M, typename U>
void is_named_impl(const volatile named_scaled_unit<Child, Symbol, M, U>*);

template<typename Child, typename P, typename U>
void is_named_impl(const volatile prefixed_unit<Child, P, U>*);

template<typename U, basic_symbol_text Symbol>
void is_named_impl(const volatile alias_unit<U, Symbol>*);

template<typename U, typename P, typename AU>
void is_named_impl(const volatile prefixed_alias_unit<U, P, AU>*);

template<Unit U>
inline constexpr bool is_named<U> = requires(U* u) { is_named_impl(u); };

template<typename Child, basic_symbol_text Symbol>
void can_be_prefixed_impl(const volatile named_unit<Child, Symbol>*);

template<typename Child, basic_symbol_text Symbol, Magnitude auto M, typename U>
void can_be_prefixed_impl(const volatile named_scaled_unit<Child, Symbol, M, U>*);

template<typename U, basic_symbol_text Symbol>
void can_be_prefixed_impl(const volatile alias_unit<U, Symbol>*);

template<Unit U>
inline constexpr bool can_be_prefixed<U> = requires(U* u) { can_be_prefixed_impl(u); };

template<Magnitude auto M, typename U>
inline constexpr bool can_be_prefixed<scaled_unit<M, U>> = can_be_prefixed<typename U::reference>;

}  // namespace detail

}  // namespace units
