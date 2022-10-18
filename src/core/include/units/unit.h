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

namespace detail {

template<typename T>
inline constexpr bool is_unit = false;

}

/**
 * @brief A concept matching all unit types in the library
 *
 * Satisfied by all unit types derived from an specialization of :class:`scaled_unit`.
 */
template<typename T>
concept Unit = detail::is_unit<T>;

// User should not instantiate this type!!!
// It should not be exported from the module
template<Magnitude auto M, Unit U>
struct scaled_unit {};

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
struct named_unit<Symbol> {
  static constexpr auto symbol = Symbol;
};

template<basic_symbol_text Symbol, Unit auto U>
struct named_unit<Symbol, U> {
  static constexpr auto symbol = Symbol;
};

namespace detail {

template<basic_symbol_text Symbol, auto... Args>
void to_base_specialization_of_named_unit(const volatile named_unit<Symbol, Args...>*);

}  // namespace detail

template<typename T>
concept NamedUnit = Unit<T> && requires(T* t) { detail::to_base_specialization_of_named_unit(t); };

template<Unit auto V>
inline constexpr bool unit_can_be_prefixed = NamedUnit<std::remove_const_t<decltype(V)>>;

/**
 * @brief A prefixed unit
 *
 * Defines a new unit that is a scaled version of another unit by the provided prefix. It is
 * only possible to create such a unit if the given prefix type matches the one defined in a
 * coherent_unit unit.
 *
 * @tparam Child inherited class type used by the downcasting facility (CRTP Idiom)
 * @tparam P prefix to be appied to the coherent_unit unit
 * @tparam U coherent_unit unit
 */
template<basic_symbol_text Symbol, Magnitude auto M, NamedUnit auto U>
  requires unit_can_be_prefixed<U>
struct prefixed_unit : std::remove_const_t<decltype(M * U)> {
  static constexpr auto symbol = Symbol + U.symbol;
};

namespace detail {

template<typename T>
inline constexpr bool is_power_of_unit =
  requires { requires is_specialization_of_power<T> && Unit<typename T::factor>; };

template<typename T>
concept UnitLike = Unit<T> || is_power_of_unit<T>;

template<typename T>
inline constexpr bool is_per_of_units = false;

template<typename... Ts>
inline constexpr bool is_per_of_units<per<Ts...>> = (... && UnitLike<Ts>);

}  // namespace detail

template<typename T>
concept UnitSpec = detail::UnitLike<T> || detail::is_per_of_units<T>;

// User should not instantiate this type!!!
// It should not be exported from the module
template<UnitSpec... Us>
struct derived_unit : detail::expr_fractions<derived_unit<>, Us...> {};

/**
 * @brief Unit one
 *
 * Unit of a dimensionless quantity.
 */
inline constexpr struct one : derived_unit<> {
} one;


namespace detail {

template<auto M, typename U>
void is_unit_impl(const volatile scaled_unit<M, U>*);

template<basic_symbol_text Symbol, auto... Args>
void is_unit_impl(const volatile named_unit<Symbol, Args...>*);

template<typename... Us>
void is_unit_impl(const volatile derived_unit<Us...>*);

template<typename T>
  requires requires(T* t) { is_unit_impl(t); }
inline constexpr bool is_unit<T> = true;

/**
 * @brief A common point for a hierarchy of units
 *
 * A unit is an entity defined and adopted by convention, with which any other quantity of
 * the same kind can be compared to express the ratio of the second quantity to the first
 * one as a number.
 *
 * All units of the same dimension can be convereted between each other. To allow this all of
 * them are expressed as different ratios of the same one proprietary chosen coherent_unit unit
 * (i.e. all length units are expressed in terms of meter, all mass units are expressed in
 * terms of gram, ...)
 *
 * @tparam M a Magnitude representing the (relative) size of this unit
 * @tparam U a unit to use as a coherent_unit for this dimension
 *
 * @note U cannot be constrained with Unit as for some specializations (i.e. named_unit)
 *       it gets the incomplete child's type with the CRTP idiom.
 */
template<UnitLike U, Magnitude M>
struct canonical_unit {
  U reference_unit;
  M mag;
};

[[nodiscard]] constexpr auto get_canonical_unit(UnitLike auto u);

template<UnitLike T, auto M, typename U>
[[nodiscard]] constexpr auto get_canonical_unit_impl(T, const volatile scaled_unit<M, U>&)
{
  auto base = get_canonical_unit(U{});
  return canonical_unit{base.reference_unit, M * base.mag};
}

template<UnitLike T, basic_symbol_text Symbol>
[[nodiscard]] constexpr auto get_canonical_unit_impl(T t, const volatile named_unit<Symbol>&)
{
  return canonical_unit{t, mag<1>};
}

template<UnitLike T, basic_symbol_text Symbol, Unit auto U>
[[nodiscard]] constexpr auto get_canonical_unit_impl(T, const volatile named_unit<Symbol, U>&)
{
  return get_canonical_unit(U);
}

template<UnitLike T, typename F, int Num, int... Den>
[[nodiscard]] constexpr auto get_canonical_unit_impl(T, const volatile power<F, Num, Den...>&)
{
  auto base = get_canonical_unit(F{});
  return canonical_unit{
    derived_unit<power_or_T<std::remove_const_t<decltype(base.reference_unit)>, power<F, Num, Den...>::exponent>>{},
    pow<power<F, Num, Den...>::exponent>(base.mag)};
}

template<UnitLike T, UnitSpec... Us>
[[nodiscard]] constexpr auto get_canonical_unit_impl(T, const volatile derived_unit<Us...>&)
{
  if constexpr (type_list_size<typename derived_unit<Us...>::_den_> != 0) {
    auto num = get_canonical_unit(type_list_map<typename derived_unit<Us...>::_num_, derived_unit>{});
    auto den = get_canonical_unit(type_list_map<typename derived_unit<Us...>::_den_, derived_unit>{});
    return canonical_unit{num.reference_unit / den.reference_unit, num.mag / den.mag};
  } else {
    auto num = (one * ... * get_canonical_unit(Us{}).reference_unit);
    auto mag = (units::mag<1> * ... * get_canonical_unit(Us{}).mag);
    return canonical_unit{num, mag};
  }
}

[[nodiscard]] constexpr auto get_canonical_unit(UnitLike auto u) { return get_canonical_unit_impl(u, u); }


template<Unit U1, Unit U2>
struct unit_less : std::bool_constant<type_name<U1>() < type_name<U2>()> {};

template<typename T1, typename T2>
using type_list_of_unit_less = expr_less<T1, T2, unit_less>;

}  // namespace detail


// Operators

template<Magnitude M, Unit U>
[[nodiscard]] consteval Unit auto operator*(M mag, U)
{
  // TODO Try passing magnitude parameters rather than magnitude type itself in case of a trivial magnitude
  // (single integer, ratio...)
  return scaled_unit<mag, std::remove_const_t<U>>{};
}

template<Magnitude M, Unit U>
[[nodiscard]] consteval Unit auto operator*(U, M) = delete;

template<Unit U1, Unit U2>
[[nodiscard]] consteval Unit auto operator*(U1, U2)
{
  return detail::expr_multiply<U1, U2, struct one, detail::type_list_of_unit_less, derived_unit>();
}

template<Unit U1, Unit U2>
[[nodiscard]] consteval Unit auto operator/(U1, U2)
{
  return detail::expr_divide<U1, U2, struct one, detail::type_list_of_unit_less, derived_unit>();
}

template<Unit U>
[[nodiscard]] consteval Unit auto operator/(int value, U)
{
  gsl_Assert(value == 1);
  return detail::expr_invert<U, struct one, derived_unit>();
}

template<Unit U>
[[nodiscard]] consteval Unit auto operator/(U, int) = delete;

template<Unit U1, Unit U2>
[[nodiscard]] consteval bool operator==(U1 lhs, U2 rhs)
{
  auto canonical_lhs = detail::get_canonical_unit(lhs);
  auto canonical_rhs = detail::get_canonical_unit(rhs);
  return is_same_v<decltype(canonical_lhs.reference_unit), decltype(canonical_rhs.reference_unit)> &&
         canonical_lhs.mag == canonical_rhs.mag;
}


// Convertible
template<Unit U1, Unit U2>
[[nodiscard]] consteval bool convertible(U1 lhs, U2 rhs)
{
  auto canonical_lhs = detail::get_canonical_unit(lhs);
  auto canonical_rhs = detail::get_canonical_unit(rhs);
  return is_same_v<decltype(canonical_lhs.reference_unit), decltype(canonical_rhs.reference_unit)>;
}

// Helper types and variable factories
template<Unit U>
struct square_ : decltype(U{} * U{}) {};

template<Unit U>
struct cubic_ : decltype(U{} * U{} * U{}) {};

// it is not allowed to use the same name for a variable and class template
// (even though it works for objects of regular class types)
template<Unit auto U>
inline constexpr square_<std::remove_const_t<decltype(U)>> square;

template<Unit auto U>
inline constexpr cubic_<std::remove_const_t<decltype(U)>> cubic;

}  // namespace units

namespace std {
// TODO implement this
template<units::Unit U1, units::Unit U2>
  requires(units::convertible(U1{}, U2{}))
struct common_type<U1, U2> {
  using type = ::units::conditional<std::derived_from<std::remove_const_t<U1>, std::remove_const_t<U2>>,
                                    std::remove_const_t<U1>, std::remove_const_t<U2>>;
};

}  // namespace std
