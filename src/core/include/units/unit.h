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
#include <units/bits/external/fixed_string.h>
#include <units/bits/external/type_name.h>
#include <units/bits/external/type_traits.h>
#include <units/magnitude.h>
#include <units/symbol_text.h>

// #include <units/bits/derived_symbol_text.h>

// IWYU pragma: begin_exports
// #include <units/bits/absolute_magnitude.h>
// #include <units/ratio.h>
// IWYU pragma: end_exports

namespace units {

// namespace detail {

// template<typename>
// inline constexpr bool can_be_prefixed = false;

// }  // namespace detail

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
struct scaled_unit {
  static constexpr UNITS_MSVC_WORKAROUND(Magnitude) auto mag = M;
  using reference = U;
};

// TODO: Remove when P1985 accepted
namespace detail {

template<Magnitude auto M, typename U>
void to_base_scaled_unit(const volatile scaled_unit<M, U>*);

}  // namespace detail

/**
 * @brief A concept matching all unit types in the library
 *
 * Satisfied by all unit types derived from an specialization of :class:`scaled_unit`.
 */
template<typename T>
concept Unit = requires(T* t) { detail::to_base_scaled_unit(t); };

namespace detail {

template<typename>
inline constexpr bool is_named = false;

}

template<typename T>
concept NamedUnit = Unit<T> && detail::is_named<T>;

template<Unit auto U1, Unit auto U2>
struct same_unit_reference : is_same<typename decltype(U1)::reference, typename decltype(U2)::reference> {};

// TODO add checking for `per` and power elements as well
template<typename T>
concept UnitSpec = Unit<T> || is_specialization_of<T, per> || detail::is_specialization_of_power<T>;

template<UnitSpec... Us>
struct derived_unit : detail::expr_fractions<derived_unit<>, Us...>, scaled_unit<mag<1>(), derived_unit<Us...>> {};

// : detail::normalized_dimension<derived_dimension<>, Ds...> {};

/**
 * @brief Unit one
 *
 * Unit of a dimensionless quantity.
 */
inline constexpr struct one : derived_unit<> {
} one;

/**
 * @brief A named unit
 *
 * Defines a named (in most cases coherent) unit that is then passed to a dimension definition.
 * A named unit may be composed with a prefix to create a prefixed_unit.
 *
 * @tparam Symbol a short text representation of the unit
 */
template<basic_symbol_text Symbol, auto...>
struct named_unit;

template<basic_symbol_text Symbol>
struct named_unit<Symbol> : scaled_unit<mag<1>(), named_unit<Symbol>> {
  static constexpr auto symbol = Symbol;
};

template<basic_symbol_text Symbol, auto U>
  requires is_specialization_of<std::remove_const_t<decltype(U)>, derived_unit>
struct named_unit<Symbol, U> : decltype(U) {
  static constexpr auto symbol = Symbol;
};


/**
 * @brief A named scaled unit
 *
 * Defines a new named unit that is a scaled version of another unit.
 * A named unit may be composed with a prefix to create a prefixed_unit.
 *
 * @tparam Symbol a short text representation of the unit
 * @tparam M the Magnitude by which to scale U
 * @tparam U a reference unit to scale
 */
template<basic_symbol_text Symbol, Magnitude auto M, Unit auto U>
struct named_scaled_unit : scaled_unit<M* decltype(U)::mag, typename decltype(U)::reference> {
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
// template<typename Child, Prefix P, NamedUnit U>
//   requires detail::can_be_prefixed<U>
// struct prefixed_unit : downcast_dispatch<Child, scaled_unit<P::mag * U::mag, typename U::reference>> {
//   static constexpr auto symbol = P::symbol + U::symbol;
// };


template<basic_symbol_text Symbol, Magnitude auto M, NamedUnit auto U>
struct prefixed_unit : scaled_unit<M* decltype(U)::mag, typename decltype(U)::reference> {};

/**
 * @brief A coherent unit of a derived quantity
 *
 * Defines a new coherent unit of a derived quantity. It should be passed as a coherent unit
 * in the dimension's definition for such a quantity.
 *
 * @tparam Child inherited class type used by the downcasting facility (CRTP Idiom)
 */
// template<typename Child>
// struct derived_unit : downcast_dispatch<Child, scaled_unit<mag<1>(), Child>> {};

namespace detail {

template<basic_symbol_text Symbol>
void is_named_impl(const volatile named_unit<Symbol>*);

template<basic_symbol_text Symbol, Magnitude auto M, auto U>
void is_named_impl(const volatile named_scaled_unit<Symbol, M, U>*);

template<Unit U>
inline constexpr bool is_named<U> = requires(U * u) { is_named_impl(u); };

// template<typename Child, basic_symbol_text Symbol>
// void can_be_prefixed_impl(const volatile named_unit<Child, Symbol>*);

// template<typename Child, basic_symbol_text Symbol, Magnitude auto M, typename U>
// void can_be_prefixed_impl(const volatile named_scaled_unit<Child, Symbol, M, U>*);

// template<typename U, basic_symbol_text Symbol>
// void can_be_prefixed_impl(const volatile alias_unit<U, Symbol>*);

// template<Unit U>
// inline constexpr bool can_be_prefixed<U> = requires(U * u) { can_be_prefixed_impl(u); };

// template<Magnitude auto M, typename U>
// inline constexpr bool can_be_prefixed<scaled_unit<M, U>> = can_be_prefixed<typename U::reference>;


template<Unit U1, Unit U2>
struct unit_less : std::bool_constant<type_name<U1>() < type_name<U2>()> {};

template<typename T1, typename T2>
using type_list_of_unit_less = expr_less<T1, T2, unit_less>;

}  // namespace detail

template<Unit U1, Unit U2>
constexpr Unit auto operator*(U1, U2)
{
  return detail::expr_multiply<U1, U2, struct one, detail::type_list_of_unit_less, derived_unit>();
}

template<Unit U1, Unit U2>
constexpr Unit auto operator/(U1, U2)
{
  return detail::expr_divide<U1, U2, struct one, detail::type_list_of_unit_less, derived_unit>();
}

template<Unit U>
constexpr Unit auto operator/(int value, U)
{
  gsl_Assert(value == 1);
  return detail::expr_invert<U, struct one, derived_unit>();
}

template<Unit U1, Unit U2>
constexpr bool operator==(U1, U2)
{
  return false;
}

// template<BaseDimension D1, BaseDimension D2>
// constexpr bool operator==(D1, D2)
// {
//   return D1::symbol == D2::symbol;
// }

// template<BaseDimension D1, Dimension D2>
//   requires(type_list_size<typename D2::normalized_den> == 0) && (type_list_size<typename D2::normalized_num> == 1) &&
//           BaseDimension<type_list_front<typename D2::normalized_num>>
// constexpr bool operator==(D1, D2)
// {
//   return D1::symbol == type_list_front<typename D2::normalized_num>::symbol;
// }

// template<Dimension D1, BaseDimension D2>
//   requires(type_list_size<typename D1::normalized_den> == 0) && (type_list_size<typename D1::normalized_num> == 1) &&
//           BaseDimension<type_list_front<typename D1::normalized_num>>
// constexpr bool operator==(D1, D2)
// {
//   return type_list_front<typename D1::normalized_num>::symbol == D2::symbol;
// }

// template<DerivedDimension D1, DerivedDimension D2>
// constexpr bool operator==(D1, D2)
// {
//   return std::is_same_v<typename D1::normalized_num, typename D2::normalized_num> &&
//          std::is_same_v<typename D1::normalized_den, typename D2::normalized_den>;
// }

}  // namespace units
