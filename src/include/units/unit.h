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

#include <units/bits/deduced_symbol_text.h>
#include <units/bits/deduced_unit.h>
#include <units/bits/external/downcasting.h>
#include <units/bits/external/fixed_string.h>
#include <units/bits/external/text_tools.h>
#include <units/bits/external/type_traits.h>
#include <units/derived_dimension.h>
#include <units/prefix.h>
#include <units/ratio.h>

namespace units {

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
 * @tparam U a unit to use as a reference for this dimension
 * @tparam R a ratio of a reference unit
 */
template<UnitRatio R, typename U>
struct scaled_unit : downcast_base<scaled_unit<R, U>> {
  using ratio = R;
  using reference = U;
};

template<Dimension D, UnitRatio R>
using downcast_unit = downcast<scaled_unit<R, typename dimension_unit<D>::reference>>;

template<Unit U1, Unit U2>
struct same_unit_reference : std::is_same<typename U1::reference, typename U2::reference> {};

/**
 * @brief An unnamed unit
 *
 * Defines a new unnamed (in most cases coherent) derived unit of a specific derived dimension
 * and it should be passed in this dimension's definition.
 *
 * @tparam Child inherited class type used by the downcasting facility (CRTP Idiom)
 */
template<typename Child>
struct unit : downcast_child<Child, scaled_unit<ratio<1>, Child>> {
  static constexpr bool is_named = false;
  using prefix_type = no_prefix;
};

/**
 * @brief Unknown unit
 * 
 * Used as a coherent unit of an unknown dimension.
 */
struct unknown_unit : unit<unknown_unit> {};

/**
 * @brief A named unit
 *
 * Defines a named (in most cases coherent) unit that is then passed to a dimension definition.
 * A named unit may be used by other units defined with the prefix of the same type, unless
 * no_prefix is provided for PT template parameter (in such a case it is impossible to define
 * a prefix unit based on this one).
 *
 * @tparam Child inherited class type used by the downcasting facility (CRTP Idiom)
 * @tparam Symbol a short text representation of the unit
 * @tparam PT no_prefix or a type of prefix family
 */
template<typename Child, basic_fixed_string Symbol, PrefixType PT>
struct named_unit : downcast_child<Child, scaled_unit<ratio<1>, Child>> {
  static constexpr bool is_named = true;
  static constexpr auto symbol = Symbol;
  using prefix_type = PT;
};

/**
 * @brief A scaled unit
 *
 * Defines a new named unit that is a scaled version of another unit. Such unit can be used by
 * other units defined with the prefix of the same type, unless no_prefix is provided for PT
 * template parameter (in such a case it is impossible to define a prefix unit based on this
 * one).
 *
 * @tparam Child inherited class type used by the downcasting facility (CRTP Idiom)
 * @tparam Symbol a short text representation of the unit
 * @tparam PT no_prefix or a type of prefix family
 * @tparam R a scale to apply to U
 * @tparam U a reference unit to scale
 */
template<typename Child, basic_fixed_string Symbol, PrefixType PT, UnitRatio R, Unit U>
struct named_scaled_unit : downcast_child<Child, scaled_unit<ratio_multiply<R, typename U::ratio>, typename U::reference>> {
  static constexpr bool is_named = true;
  static constexpr auto symbol = Symbol;
  using prefix_type = PT;
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
template<typename Child, Prefix P, Unit U>
  requires std::same_as<typename P::prefix_type, typename U::prefix_type>
// TODO replace with the below code when gcc will stop to crash on it ;-)
// struct prefixed_unit : named_scaled_unit<Child, P::symbol + U::symbol, typename P::prefix_type,
//                                    ratio_multiply<typename P::ratio, typename U::ratio>,
//                                    typename U::reference> {};
struct prefixed_unit :
    downcast_child<Child, scaled_unit<ratio_multiply<typename P::ratio, typename U::ratio>, typename U::reference>> {
  static constexpr bool is_named = true;
  static constexpr auto symbol = P::symbol + U::symbol;
  using prefix_type = P::prefix_type;
};

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
template<typename Child, DerivedDimension Dim, Unit U, Unit... URest>
  requires detail::same_scaled_units<typename Dim::recipe, U, URest...> &&
           (U::is_named && (URest::is_named && ... && true))
struct deduced_unit : downcast_child<Child, detail::deduced_unit<Dim, U, URest...>> {
  static constexpr bool is_named = false;
  static constexpr auto symbol = detail::deduced_symbol_text<Dim, U, URest...>();
  using prefix_type = no_prefix;
};

// template<typename Child, Dimension Dim, basic_fixed_string Symbol, PrefixType PT, Unit U, Unit... Us>
// struct named_deduced_derived_unit : downcast_child<Child, detail::deduced_derived_unit<Dim, U, Us...>> {
//   static constexpr bool is_named = true;
//   static constexpr auto symbol = Symbol;
//   using prefix_type = PT;
// };

}  // namespace units
