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

#include <units/bits/downcasting.h>
#include <units/bits/fixed_string.h>
#include <units/bits/type_traits.h>
#include <units/bits/unit_concept.h>
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
template<typename U, UnitRatio R>
struct scaled_unit : downcast_base<scaled_unit<U, R>> {
  using reference = U;
  using ratio = R;
};

template<Dimension D, UnitRatio R>
using downcast_unit = downcast<scaled_unit<typename dimension_unit<D>::reference, R>>;

template<Unit U1, Unit U2>
struct same_unit_reference : std::is_same<typename U1::reference, typename U2::reference> {};

/**
 * @brief A starting point for a new hierarchy of units
 *
 * Defines a new unnamed (in most cases coherent) derived unit of a specific derived dimension
 * and it should be passed in this dimension's definition.
 *
 * @tparam Child inherited class type used by the downcasting facility (CRTP Idiom)
 */
template<typename Child>
struct unit : downcast_child<Child, scaled_unit<Child, ratio<1>>> {
  static constexpr bool is_named = false;
  using prefix_type = no_prefix;
};

/**
 * @brief Unknown unit
 * 
 * Used as a reference unit of an unknown dimension.
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
struct named_unit : downcast_child<Child, scaled_unit<Child, ratio<1>>> {
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
struct named_scaled_unit : downcast_child<Child, scaled_unit<typename U::reference, ratio_multiply<R, typename U::ratio>>> {
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
    downcast_child<Child, scaled_unit<typename U::reference, ratio_multiply<typename P::ratio, typename U::ratio>>> {
  static constexpr bool is_named = true;
  static constexpr auto symbol = P::symbol + U::symbol;
  using prefix_type = P::prefix_type;
};

// UnitOf
template<typename U, typename D>
concept UnitOf =
  Unit<U> &&
  Dimension<D> &&
  std::same_as<typename U::reference, typename dimension_unit<D>::reference>;

namespace detail {

// same_scaled_units
template<DerivedDimension D, Unit... Us>
inline constexpr bool same_scaled_units = false;

template<typename... Es, Unit... Us>
inline constexpr bool same_scaled_units<derived_dimension<Es...>, Us...> = (UnitOf<Us, typename Es::dimension> && ...);

// deduced_unit
template<typename Result, int UnitExpNum, int UnitExpDen, typename UnitRatio>
struct ratio_op;

template<typename Result, int UnitExpDen, typename UnitRatio>
struct ratio_op<Result, 0, UnitExpDen, UnitRatio> {
  using ratio = Result;
};

template<typename Result, int UnitExpNum, int UnitExpDen, typename UnitRatio>
struct ratio_op {
  using calc_ratio =
      conditional<(UnitExpNum * UnitExpDen > 0), ratio_multiply<Result, UnitRatio>, ratio_divide<Result, UnitRatio>>;
  static constexpr int value = (UnitExpNum * UnitExpDen > 0) ? (UnitExpNum - UnitExpDen) : (UnitExpNum + UnitExpDen);
  using ratio = ratio_op<calc_ratio, value, UnitExpDen, UnitRatio>::ratio;
};

template<DerivedDimension D, Unit... Us>
struct derived_ratio;

template<Unit... Us>
struct derived_ratio<derived_dimension<>, Us...> {
  using ratio = ::units::ratio<1>;
};

template<typename E, typename... ERest, Unit U, Unit... URest>
struct derived_ratio<derived_dimension<E, ERest...>, U, URest...> {
  using rest_ratio = derived_ratio<derived_dimension<ERest...>, URest...>::ratio;
  using ratio = ratio_op<rest_ratio, E::num, E::den, typename U::ratio>::ratio;
};

template<DerivedDimension D, Unit... Us>
using deduced_unit =
    scaled_unit<typename D::coherent_unit::reference, typename detail::derived_ratio<typename D::recipe, Us...>::ratio>;

}  // namespace detail

/**
 * @brief A unit with a deduced ratio and symbol
 *
 * Defines a new unit with a deduced ratio and symbol based on the recipe from the provided
 * derived dimension. The number and order of provided units should match the recipe of
 * dimension.
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
  static constexpr auto symbol = basic_fixed_string{""};  // detail::deduced_symbol_text<Dim, U, Us...>(); // TODO implement this
  using prefix_type = no_prefix;
};

// template<typename Child, Dimension Dim, basic_fixed_string Symbol, PrefixType PT, Unit U, Unit... Us>
// struct named_deduced_derived_unit : downcast_child<Child, detail::deduced_derived_unit<Dim, U, Us...>> {
//   static constexpr bool is_named = true;
//   static constexpr auto symbol = Symbol;
//   using prefix_type = PT;
// };

}  // namespace units
