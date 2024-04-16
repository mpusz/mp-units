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
#include <mp-units/bits/external/algorithm.h>
#include <mp-units/bits/external/fixed_string.h>
#include <mp-units/bits/external/hacks.h>
#include <mp-units/bits/external/type_name.h>
#include <mp-units/bits/external/type_traits.h>
#include <mp-units/bits/get_associated_quantity.h>
#include <mp-units/bits/magnitude.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/bits/quantity_point_concepts.h>
#include <mp-units/bits/quantity_spec_concepts.h>
#include <mp-units/bits/ratio.h>
#include <mp-units/bits/symbol_text.h>
#include <mp-units/bits/text_tools.h>
#include <mp-units/bits/unit_concepts.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <gsl/gsl-lite.hpp>
#include <array>
#include <iterator>
#include <string>
#include <string_view>
#endif

namespace mp_units {

namespace detail {

template<Unit U, bool = requires { U::point_origin; }>
struct propagate_point_origin {};

template<Unit U>
struct propagate_point_origin<U, true> {
  static constexpr auto point_origin = U::point_origin;
};

}  // namespace detail

/**
 * @brief Unit being a scaled version of another unit
 *
 * @tparam M magnitude describing the scale factor
 * @tparam U reference unit being scaled
 *
 * @note User should not instantiate this type! It is not exported from the C++ module. The library will
 *       instantiate this type automatically based on the unit arithmetic equation provided by the user.
 */
template<Magnitude auto M, Unit U>
struct scaled_unit : detail::propagate_point_origin<U> {
  static constexpr MP_UNITS_CONSTRAINED_AUTO_WORKAROUND(Magnitude) auto mag = M;
  static constexpr U reference_unit{};
};

namespace detail {

template<typename T>
inline constexpr bool is_specialization_of_scaled_unit = false;

template<auto M, Unit U>
inline constexpr bool is_specialization_of_scaled_unit<scaled_unit<M, U>> = true;

}  // namespace detail

/**
 * @brief A named unit
 *
 * Defines a unit with a special name. It may be used to provide a base unit in the system
 * of units (e.g. `metre`) or a name assigned to another scaled or derived unit
 * (e.g. `hour`, `joule`).
 * Most of the named units may be composed with a prefix to create a `prefixed_unit`.
 *
 * For example:
 *
 * @code{.cpp}
 * inline constexpr struct second : named_unit<"s", time> {} second;
 * inline constexpr struct metre : named_unit<"m", length> {} metre;
 * inline constexpr struct hertz : named_unit<"Hz", inverse(second)> {} hertz;
 * inline constexpr struct newton : named_unit<"N", kilogram * metre / square(second)> {} newton;
 * inline constexpr struct degree_Celsius : named_unit<{u8"°C", "`C"}, kelvin> {} degree_Celsius;
 * inline constexpr struct minute : named_unit<"min", mag<60> * second> {} minute;
 * @endcode
 *
 * @note A common convention in this library is to assign the same name for a type and an object of this type.
 *       Besides defining them user never works with the unit types in the source code. All operations
 *       are done on the objects. Contrarily, the unit types are the only one visible in the compilation
 *       errors. Having them of the same names improves user experience and somehow blurs those separate domains.
 *
 * @tparam Symbol a short text representation of the unit
 */
MP_UNITS_EXPORT template<symbol_text Symbol, auto...>
struct named_unit;

/**
 * @brief Specialization for unit of a specified base quantity
 *
 * Associates a unit with a specified base quantity.
 * For example `si::metre` is a unit to measure `isq::length` in the SI system.
 *
 * @note This is the preferred way to define a measurement unit for a specific base quantity.
 *
 * @note It does not have to (or sometimes even can't) be a proper system's base unit. For example
 *       a base unit of mass in the SI is `si::kilogram` but here you are about to provide an `si::gram`
 *       and it will work just fine as those two are convertible to each other. A similar case would be
 *       the `cgs::centimetre` that is a base unit for `isq::length` in the CGS system.
 *
 * @tparam Symbol a short text representation of the unit
 * @tparam QuantitySpec a specification of a base quantity to be measured with this unit
 */
template<symbol_text Symbol, detail::QuantityKindSpec auto QS>
  requires(!Symbol.empty()) && detail::BaseDimension<std::remove_const_t<decltype(QS.dimension)>>
struct named_unit<Symbol, QS> {
  static constexpr auto symbol = Symbol;  ///< Unique base unit identifier
  static constexpr auto quantity_spec = QS;
};

template<symbol_text Symbol, detail::QuantityKindSpec auto QS, PointOrigin auto PO>
  requires(!Symbol.empty()) && detail::BaseDimension<std::remove_const_t<decltype(QS.dimension)>>
struct named_unit<Symbol, QS, PO> {
  static constexpr auto symbol = Symbol;  ///< Unique base unit identifier
  static constexpr auto quantity_spec = QS;
  static constexpr auto point_origin = PO;
};

/**
 * @brief Specialization for a unit that can be reused by several base quantities
 *
 * This specialization is used in rare cases where more than one base quantity in a specific
 * system of units uses the same unit. For example in a hypothetical system of natural units
 * where  constant for speed of light `c = 1`, length and time could be measured in seconds.
 * In such cases `system_reference` has to be used to explicitly express such a binding.
 *
 * @tparam Symbol a short text representation of the unit
 */
template<symbol_text Symbol>
  requires(!Symbol.empty())
struct named_unit<Symbol> {
  static constexpr auto symbol = Symbol;  ///< Unique base unit identifier
};

/**
 * @brief Specialization for a unit with special name
 *
 * Allows assigning a special name to another scaled or derived unit (e.g. `hour`, `joule`).
 *
 * @tparam Symbol a short text representation of the unit
 * @tparam Unit a unit for which we provide a special name
 */
template<symbol_text Symbol, Unit auto U>
  requires(!Symbol.empty())
struct named_unit<Symbol, U> : std::remove_const_t<decltype(U)> {
  static constexpr auto symbol = Symbol;  ///< Unique unit identifier
};

template<symbol_text Symbol, Unit auto U, PointOrigin auto PO>
  requires(!Symbol.empty())
struct named_unit<Symbol, U, PO> : std::remove_const_t<decltype(U)> {
  static constexpr auto symbol = Symbol;  ///< Unique unit identifier
  static constexpr auto point_origin = PO;
};

/**
 * @brief Specialization for a unit with special name valid only for a specific quantity
 *
 * The same as the above but additionally limits the usage of this unit to provided quantities.
 *
 * @tparam Symbol a short text representation of the unit
 * @tparam Unit a unit for which we provide a special name
 * @tparam QuantitySpec a specification of a quantity to be measured with this unit
 */
template<symbol_text Symbol, AssociatedUnit auto U, detail::QuantityKindSpec auto QS>
  requires(!Symbol.empty()) && (QS.dimension == detail::get_associated_quantity(U).dimension)
struct named_unit<Symbol, U, QS> : std::remove_const_t<decltype(U)> {
  static constexpr auto symbol = Symbol;  ///< Unique unit identifier
  static constexpr auto quantity_spec = QS;
};

template<symbol_text Symbol, AssociatedUnit auto U, detail::QuantityKindSpec auto QS, PointOrigin auto PO>
  requires(!Symbol.empty()) && (QS.dimension == detail::get_associated_quantity(U).dimension)
struct named_unit<Symbol, U, QS, PO> : std::remove_const_t<decltype(U)> {
  static constexpr auto symbol = Symbol;  ///< Unique unit identifier
  static constexpr auto quantity_spec = QS;
  static constexpr auto point_origin = PO;
};

/**
 * @brief A prefixed unit
 *
 * Defines a new unit that is a scaled version of another unit with the scaling
 * factor specified by a predefined prefix.
 *
 * For example:
 *
 * @code{.cpp}
 * template<PrefixableUnit auto U>
 * struct kilo_ : prefixed_unit<"k", mag_power<10, 3>, U> {};
 *
 * template<PrefixableUnit auto U>
 * inline constexpr kilo_<U> kilo;
 *
 * inline constexpr struct kilogram : decltype(si::kilo<gram>) {} kilogram;
 * @endcode
 *
 * @tparam Symbol a prefix text to prepend to a unit symbol
 * @tparam M scaling factor of the prefix
 * @tparam U a named unit to be prefixed
 */
MP_UNITS_EXPORT template<symbol_text Symbol, Magnitude auto M, PrefixableUnit auto U>
  requires(!Symbol.empty())
struct prefixed_unit : std::remove_const_t<decltype(M * U)> {
  static constexpr auto symbol = Symbol + U.symbol;
};

namespace detail {

template<typename T>
struct is_one : std::false_type {};

}  // namespace detail

/**
 * @brief Measurement unit for a derived quantity
 *
 * Derived units are defined as products of powers of the base units.
 *
 * Instead of using a raw list of exponents this library decided to use expression template syntax to make types
 * more digestable for the user. The positive exponents are ordered first and all negative exponents are put as a list
 * into the `per<...>` class template. If a power of exponent is different than `1` the unit type is enclosed in
 * `power<Dim, Num, Den>` class template. Otherwise, it is just put directly in the list without any wrapper. There
 * is also one special case. In case all of the exponents are negative then the `one` being a coherent unit of
 * a dimensionless quantity is put in the front to increase the readability.
 *
 * For example:
 *
 * @code{.cpp}
 * static_assert(is_of_type<inverse(second), derived_unit<one, per<second>>>);
 * static_assert(is_of_type<one / inverse(second), second>);
 * static_assert(is_of_type<one * second, second>);
 * static_assert(is_of_type<metre * metre, derived_unit<power<metre, 2>>>);
 * static_assert(is_of_type<metre * second, derived_unit<metre, second>>);
 * static_assert(is_of_type<metre / second, derived_unit<metre, per<second>>>);
 * static_assert(is_of_type<metre / square(second), derived_unit<metre, per<power<second, 2>>>>);
 * static_assert(is_of_type<watt / joule, derived_unit<watt, per<joule>>>);
 * @endcode
 *
 * Every unit in the library has its internal canonical representation being the list of exponents of named base units
 * (with the exception of `kilogram` which is represented as `gram` here) and a scaling ratio represented with a
 * magnitude.
 *
 * Two units are deemed convertible if their canonical version has units of the same type.
 * Two units are equivalent when they are convertible and their canonical versions have the same scaling ratios.
 *
 * The above means that:
 * - `1/s` and `Hz` are both convertible and equal
 * - `m` and `km` are convertible but not equal
 * - `m` and `m²` ane not convertible and not equal
 *
 * @note This also means that units like `hertz` and `becquerel` are also considered convertible and equal.
 *
 * @tparam Us a parameter pack consisting tokens allowed in the unit specification
 *            (units, `power<U, Num, Den>`, `per<...>`)
 *
 * @note User should not instantiate this type! It is not exported from the C++ module. The library will
 *       instantiate this type automatically based on the unit arithmetic equation provided by the user.
 */
template<detail::DerivedUnitExpr... Expr>
struct derived_unit : detail::expr_fractions<detail::is_one, Expr...> {};

/**
 * @brief Unit one
 *
 * Unit of a dimensionless quantity.
 */
// clang-format off
MP_UNITS_EXPORT inline constexpr struct one : derived_unit<> {} one;
// clang-format on

namespace detail {

template<>
struct is_one<struct one> : std::true_type {};

/**
 * @brief A canonical representation of a unit
 *
 * A canonical representation of a unit consists of:
 * - a reference unit being the result of extraction of all the intermediate derived units,
 * - a magnitude being a product of all the prefixes and magnitudes of extracted scaled units.
 *
 * All units having the same canonical unit are deemed equal.
 * All units having the same reference unit are convertible (their magnitude may differ and
 * is used during conversion).
 *
 * @tparam U a unit to use as a `reference_unit`
 * @tparam M a Magnitude representing an absolute scaling factor of this unit
 */
template<Magnitude M, Unit U>
struct canonical_unit {
  M mag;
  U reference_unit;
};

#if MP_UNITS_COMP_CLANG

template<Magnitude M, Unit U>
canonical_unit(M, U) -> canonical_unit<M, U>;

#endif

template<Unit T, symbol_text Symbol, detail::QuantityKindSpec auto Q, auto... Args>
[[nodiscard]] consteval auto get_canonical_unit_impl(T t, const named_unit<Symbol, Q, Args...>&);

template<Unit T, symbol_text Symbol, auto... Args>
[[nodiscard]] consteval auto get_canonical_unit_impl(T t, const named_unit<Symbol, Args...>&);

template<Unit T, symbol_text Symbol, Unit auto U, auto... Args>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const named_unit<Symbol, U, Args...>&);

template<typename T, typename F, int Num, int... Den>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const power<F, Num, Den...>&);

template<Unit T, typename... Expr>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const derived_unit<Expr...>&);

template<Unit T, auto M, typename U>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const scaled_unit<M, U>&)
{
  auto base = get_canonical_unit_impl(U{}, U{});
  return canonical_unit{M * base.mag, base.reference_unit};
}

template<Unit T, symbol_text Symbol, detail::QuantityKindSpec auto Q, auto... Args>
[[nodiscard]] consteval auto get_canonical_unit_impl(T t, const named_unit<Symbol, Q, Args...>&)
{
  return canonical_unit{mag<1>, t};
}

template<Unit T, symbol_text Symbol, auto... Args>
[[nodiscard]] consteval auto get_canonical_unit_impl(T t, const named_unit<Symbol, Args...>&)
{
  return canonical_unit{mag<1>, t};
}

template<Unit T, symbol_text Symbol, Unit auto U, auto... Args>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const named_unit<Symbol, U, Args...>&)
{
  return get_canonical_unit_impl(U, U);
}

template<typename F, int Num, int... Den, typename... Us>
[[nodiscard]] consteval auto get_canonical_unit_impl(const power<F, Num, Den...>&, const type_list<Us...>&)
{
  auto mag = (mp_units::mag<1> * ... * pow<Num, Den...>(get_canonical_unit_impl(Us{}, Us{}).mag));
  auto u = (one * ... * pow<Num, Den...>(get_canonical_unit_impl(Us{}, Us{}).reference_unit));
  return canonical_unit{mag, u};
}

template<typename T, typename F, int Num, int... Den>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const power<F, Num, Den...>&)
{
  auto base = get_canonical_unit_impl(F{}, F{});
  if constexpr (requires { typename decltype(base.reference_unit)::_num_; }) {
    auto num = get_canonical_unit_impl(power<F, Num, Den...>{}, typename decltype(base.reference_unit)::_num_{});
    auto den = get_canonical_unit_impl(power<F, Num, Den...>{}, typename decltype(base.reference_unit)::_den_{});
    return canonical_unit{pow<Num, Den...>(base.mag) * num.mag / den.mag, num.reference_unit / den.reference_unit};
  } else {
    return canonical_unit{pow<Num, Den...>(base.mag),
                          derived_unit<power<std::remove_const_t<decltype(base.reference_unit)>, Num, Den...>>{}};
  }
}

template<typename... Us>
[[nodiscard]] consteval auto get_canonical_unit_impl(const type_list<Us...>&)
{
  auto m = (mp_units::mag<1> * ... * get_canonical_unit_impl(Us{}, Us{}).mag);
  auto u = (one * ... * get_canonical_unit_impl(Us{}, Us{}).reference_unit);
  return canonical_unit{m, u};
}

template<Unit T, typename... Expr>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const derived_unit<Expr...>&)
{
  auto num = get_canonical_unit_impl(typename derived_unit<Expr...>::_num_{});
  auto den = get_canonical_unit_impl(typename derived_unit<Expr...>::_den_{});
  return canonical_unit{num.mag / den.mag, num.reference_unit / den.reference_unit};
}

template<Unit Lhs, Unit Rhs>
struct unit_less : std::bool_constant<(Lhs::symbol < Rhs::symbol)> {};

template<typename T1, typename T2>
using type_list_of_unit_less = expr_less<T1, T2, unit_less>;

}  // namespace detail

// TODO this should really be in the `details` namespace but is used in `chrono.h` (a part of mp_units.systems)
// Even though it is not exported, it is visible to the other module via ADL
[[nodiscard]] consteval auto get_canonical_unit(Unit auto u) { return detail::get_canonical_unit_impl(u, u); }

MP_UNITS_EXPORT_BEGIN

// Operators

/**
 * Multiplication by `1` returns the same unit, otherwise `scaled_unit` is being returned.
 */
template<Magnitude M, Unit U>
[[nodiscard]] MP_UNITS_CONSTEVAL Unit auto operator*(M, const U u)
{
  if constexpr (std::is_same_v<M, std::remove_cvref_t<decltype(mp_units::mag<1>)>>)
    return u;
  else
    return scaled_unit<M{}, U>{};
}

[[nodiscard]] consteval Unit auto operator*(Unit auto, Magnitude auto) = delete;

/**
 * Returns the result of multiplication with an inverse unit.
 */
template<Magnitude M, Unit U>
[[nodiscard]] MP_UNITS_CONSTEVAL Unit auto operator/(M mag, const U u)
{
  return mag * inverse(u);
}

/**
 * `scaled_unit` specializations have priority in this operation. This means that the library framework
 * prevents passing it as an element to the `derived_unit`. In such case only the reference unit is passed
 * to the derived unit and the magnitude remains outside forming another scaled unit as a result of the operation.
 */
template<Unit Lhs, Unit Rhs>
[[nodiscard]] MP_UNITS_CONSTEVAL Unit auto operator*(Lhs lhs, Rhs rhs)
{
  if constexpr (detail::is_specialization_of_scaled_unit<Lhs> && detail::is_specialization_of_scaled_unit<Rhs>)
    return (Lhs::mag * Rhs::mag) * (Lhs::reference_unit * Rhs::reference_unit);
  else if constexpr (detail::is_specialization_of_scaled_unit<Lhs>)
    return Lhs::mag * (Lhs::reference_unit * rhs);
  else if constexpr (detail::is_specialization_of_scaled_unit<Rhs>)
    return Rhs::mag * (lhs * Rhs::reference_unit);
  else
    return detail::expr_multiply<derived_unit, struct one, detail::type_list_of_unit_less>(lhs, rhs);
}

/**
 * `scaled_unit` specializations have priority in this operation. This means that the library framework
 * prevents passing it as an element to the `derived_unit`. In such case only the reference unit is passed
 * to the derived unit and the magnitude remains outside forming another scaled unit as a result of the operation.
 */
template<Unit Lhs, Unit Rhs>
[[nodiscard]] MP_UNITS_CONSTEVAL Unit auto operator/(Lhs lhs, Rhs rhs)
{
  if constexpr (detail::is_specialization_of_scaled_unit<Lhs> && detail::is_specialization_of_scaled_unit<Rhs>)
    return (Lhs::mag / Rhs::mag) * (Lhs::reference_unit / Rhs::reference_unit);
  else if constexpr (detail::is_specialization_of_scaled_unit<Lhs>)
    return Lhs::mag * (Lhs::reference_unit / rhs);
  else if constexpr (detail::is_specialization_of_scaled_unit<Rhs>)
    return mag<1> / Rhs::mag * (lhs / Rhs::reference_unit);
  else
    return detail::expr_divide<derived_unit, struct one, detail::type_list_of_unit_less>(lhs, rhs);
}

[[nodiscard]] MP_UNITS_CONSTEVAL Unit auto inverse(Unit auto u) { return one / u; }

MP_UNITS_EXPORT_END

namespace detail {

[[nodiscard]] consteval bool have_same_canonical_reference_unit_impl(...) { return false; }

template<symbol_text Symbol, auto... D>
[[nodiscard]] consteval bool have_same_canonical_reference_unit_impl(const named_unit<Symbol, D...>&,
                                                                     const named_unit<Symbol, D...>&)
{
  return true;
}

template<typename F1, typename F2, auto... Vs>
[[nodiscard]] consteval bool have_same_canonical_reference_unit_impl(const power<F1, Vs...>&, const power<F2, Vs...>&)
{
  return have_same_canonical_reference_unit_impl(F1{}, F2{});
}

template<typename... Us1, typename... Us2>
  requires(sizeof...(Us1) == sizeof...(Us2))
[[nodiscard]] consteval bool have_same_canonical_reference_unit_impl(const type_list<Us1...>&, const type_list<Us2...>&)
{
  return (... && have_same_canonical_reference_unit_impl(Us1{}, Us2{}));
}

template<typename... Expr1, typename... Expr2>
[[nodiscard]] consteval bool have_same_canonical_reference_unit_impl(const derived_unit<Expr1...>&,
                                                                     const derived_unit<Expr2...>&)
{
  return have_same_canonical_reference_unit_impl(typename derived_unit<Expr1...>::_num_{},
                                                 typename derived_unit<Expr2...>::_num_{}) &&
         have_same_canonical_reference_unit_impl(typename derived_unit<Expr1...>::_den_{},
                                                 typename derived_unit<Expr2...>::_den_{});
}

[[nodiscard]] consteval bool have_same_canonical_reference_unit(Unit auto u1, Unit auto u2)
{
  auto canonical_lhs = get_canonical_unit(u1);
  auto canonical_rhs = get_canonical_unit(u2);
  return have_same_canonical_reference_unit_impl(canonical_lhs.reference_unit, canonical_rhs.reference_unit);
}

}  // namespace detail


MP_UNITS_EXPORT [[nodiscard]] consteval bool operator==(Unit auto lhs, Unit auto rhs)
{
  auto canonical_lhs = get_canonical_unit(lhs);
  auto canonical_rhs = get_canonical_unit(rhs);
  return detail::have_same_canonical_reference_unit(canonical_lhs.reference_unit, canonical_rhs.reference_unit) &&
         canonical_lhs.mag == canonical_rhs.mag;
}

namespace detail {

template<typename T>
inline constexpr bool is_specialization_of_derived_unit = false;

template<typename... Expr>
inline constexpr bool is_specialization_of_derived_unit<derived_unit<Expr...>> = true;

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

/**
 * @brief Computes the value of a unit raised to the `Num/Den` power
 *
 * @tparam Num Exponent numerator
 * @tparam Den Exponent denominator
 * @param u Unit being the base of the operation
 *
 * @return Unit The result of computation
 */
template<std::intmax_t Num, std::intmax_t Den = 1, Unit U>
  requires detail::non_zero<Den>
[[nodiscard]] consteval Unit auto pow(U u)
{
  if constexpr (Num == 0 || is_same_v<U, struct one>)
    return one;
  else if constexpr (ratio{Num, Den} == 1)
    return u;
  else if constexpr (detail::is_specialization_of_scaled_unit<U>)
    return scaled_unit<pow<Num, Den>(U::mag), std::remove_const_t<decltype(pow<Num, Den>(U::reference_unit))>>{};
  else if constexpr (detail::is_specialization_of_derived_unit<U>)
    return detail::expr_pow<Num, Den, derived_unit, struct one, detail::type_list_of_unit_less>(u);
  else if constexpr (Den == 1)
    return derived_unit<power<U, Num>>{};
  else
    return derived_unit<power<U, Num, Den>>{};
}

/**
 * @brief Computes the square root of a unit
 *
 * @param u Unit being the base of the operation
 *
 * @return Unit The result of computation
 */
[[nodiscard]] consteval Unit auto sqrt(Unit auto u) { return pow<1, 2>(u); }

/**
 * @brief Computes the cubic root of a unit
 *
 * @param u Unit being the base of the operation
 *
 * @return Unit The result of computation
 */
[[nodiscard]] consteval Unit auto cbrt(Unit auto u) { return pow<1, 3>(u); }

/**
 * @brief Computes the square power of a unit
 *
 * @param u Unit being the base of the operation
 *
 * @return Unit The result of computation
 */
[[nodiscard]] consteval Unit auto square(Unit auto u) { return pow<2>(u); }

/**
 * @brief Computes the cubic power of a unit
 *
 * @param u Unit being the base of the operation
 *
 * @return Unit The result of computation
 */
[[nodiscard]] consteval Unit auto cubic(Unit auto u) { return pow<3>(u); }


// common dimensionless units
// clang-format off
inline constexpr struct percent : named_unit<"%", mag<ratio{1, 100}> * one> {} percent;
inline constexpr struct per_mille : named_unit<symbol_text{u8"‰", "%o"}, mag<ratio(1, 1000)> * one> {} per_mille;
inline constexpr struct parts_per_million : named_unit<"ppm", mag<ratio(1, 1'000'000)> * one> {} parts_per_million;
inline constexpr auto ppm = parts_per_million;
// clang-format on


// convertible_to
[[nodiscard]] consteval bool convertible(Unit auto from, Unit auto to)
{
  return detail::have_same_canonical_reference_unit(from, to);
}

// Common unit
[[nodiscard]] consteval Unit auto common_unit(Unit auto u) { return u; }

template<Unit U1, Unit U2>
[[nodiscard]] consteval Unit auto common_unit(U1 u1, U2 u2)
  requires(detail::have_same_canonical_reference_unit(u1, u2))
{
  if constexpr (U1{} == U2{}) {
    if constexpr (std::derived_from<U1, U2>)
      return u1;
    else if constexpr (std::derived_from<U2, U1>)
      return u2;
    else
      // TODO Check if there is a better choice here
      return better_type_name(u1, u2);
  } else {
    constexpr auto canonical_lhs = get_canonical_unit(U1{});
    constexpr auto canonical_rhs = get_canonical_unit(U2{});

    if constexpr (is_integral(canonical_lhs.mag / canonical_rhs.mag))
      return u2;
    else if constexpr (is_integral(canonical_rhs.mag / canonical_lhs.mag))
      return u1;
    else {
      constexpr auto cm = detail::common_magnitude(canonical_lhs.mag, canonical_rhs.mag);
      return scaled_unit<cm, std::remove_const_t<decltype(canonical_lhs.reference_unit)>>{};
    }
  }
}

[[nodiscard]] consteval Unit auto common_unit(Unit auto u1, Unit auto u2, Unit auto u3, Unit auto... rest)
  requires requires { common_unit(common_unit(u1, u2), u3, rest...); }
{
  return common_unit(common_unit(u1, u2), u3, rest...);
}


/**
 * @brief Puts a space ' ' sign before a unit symbol
 *
 * Quantities of some units (e.g. degree, arcminute, arcsecond) should not be printed with the
 * space between a number and a unit. For those a partial specialization with the value `false` should
 * be provided.
 */
template<Unit auto U>
inline constexpr bool space_before_unit_symbol = true;

template<>
inline constexpr bool space_before_unit_symbol<one> = false;
template<>
inline constexpr bool space_before_unit_symbol<percent> = false;
template<>
inline constexpr bool space_before_unit_symbol<per_mille> = false;

// get_unit_symbol

enum class unit_symbol_solidus : std::int8_t {
  one_denominator,  // m/s;   kg m⁻¹ s⁻¹
  always,           // m/s;   kg/(m s)
  never,            // m s⁻¹; kg m⁻¹ s⁻¹
  default_denominator = one_denominator
};

enum class unit_symbol_separator : std::int8_t {
  space,          // kg m²/s²
  half_high_dot,  // kg⋅m²/s²  (valid only for unicode encoding)
  default_separator = space
};

struct unit_symbol_formatting {
  text_encoding encoding = text_encoding::default_encoding;
  unit_symbol_solidus solidus = unit_symbol_solidus::default_denominator;
  unit_symbol_separator separator = unit_symbol_separator::default_separator;
};

MP_UNITS_EXPORT_END

namespace detail {

template<typename CharT, std::output_iterator<CharT> Out>
constexpr Out print_separator(Out out, unit_symbol_formatting fmt)
{
  if (fmt.separator == unit_symbol_separator::half_high_dot) {
    if (fmt.encoding != text_encoding::unicode)
      throw std::invalid_argument(
        "'unit_symbol_separator::half_high_dot' can be only used with 'text_encoding::unicode'");
    out = copy(std::string_view("⋅"), out).out;
  } else {
    *out++ = ' ';
  }
  return out;
}

template<typename CharT, std::output_iterator<CharT> Out, Unit U>
  requires requires { U::symbol; }
constexpr Out unit_symbol_impl(Out out, U, unit_symbol_formatting fmt, bool negative_power)
{
  return copy_symbol<CharT>(U::symbol, fmt.encoding, negative_power, out);
}

template<typename CharT, std::output_iterator<CharT> Out, auto M, typename U>
constexpr Out unit_symbol_impl(Out out, const scaled_unit<M, U>& u, unit_symbol_formatting fmt, bool negative_power)
{
  if constexpr (M == mag<1>) {
    // no ratio/prefix
    return unit_symbol_impl<CharT>(out, u.reference_unit, fmt, negative_power);
  } else {
    constexpr auto mag_txt = magnitude_text<M>();
    out = copy<CharT>(mag_txt, fmt.encoding, out);

    if constexpr (space_before_unit_symbol<scaled_unit<M, U>::reference_unit>) *out++ = ' ';
    return unit_symbol_impl<CharT>(out, u.reference_unit, fmt, negative_power);
  }
}

template<typename CharT, std::output_iterator<CharT> Out, typename F, int Num, int... Den>
constexpr auto unit_symbol_impl(Out out, const power<F, Num, Den...>&, unit_symbol_formatting fmt, bool negative_power)
{
  out = unit_symbol_impl<CharT>(out, F{}, fmt, false);  // negative power component will be added below if needed
  return copy_symbol_exponent<CharT, Num, Den...>(fmt.encoding, negative_power, out);
}

template<typename CharT, std::output_iterator<CharT> Out, DerivedUnitExpr M>
constexpr Out unit_symbol_impl(Out out, M m, std::size_t Idx, unit_symbol_formatting fmt, bool negative_power)
{
  if (Idx > 0) out = print_separator<CharT>(out, fmt);
  return unit_symbol_impl<CharT>(out, m, fmt, negative_power);
}

template<typename CharT, std::output_iterator<CharT> Out, DerivedUnitExpr... Ms, std::size_t... Idxs>
constexpr Out unit_symbol_impl(Out out, const type_list<Ms...>&, std::index_sequence<Idxs...>,
                               unit_symbol_formatting fmt, bool negative_power)
{
  return (..., (out = unit_symbol_impl<CharT>(out, Ms{}, Idxs, fmt, negative_power)));
}

template<typename CharT, std::output_iterator<CharT> Out, DerivedUnitExpr... Nums, DerivedUnitExpr... Dens>
constexpr Out unit_symbol_impl(Out out, const type_list<Nums...>& nums, const type_list<Dens...>& dens,
                               unit_symbol_formatting fmt)
{
  if constexpr (sizeof...(Nums) == 0 && sizeof...(Dens) == 0) {
    // dimensionless quantity
    return out;
  } else if constexpr (sizeof...(Dens) == 0) {
    // no denominator
    return unit_symbol_impl<CharT>(out, nums, std::index_sequence_for<Nums...>(), fmt, false);
  } else {
    using enum unit_symbol_solidus;
    if constexpr (sizeof...(Nums) > 0) {
      out = unit_symbol_impl<CharT>(out, nums, std::index_sequence_for<Nums...>(), fmt, false);
    }

    if (fmt.solidus == always || (fmt.solidus == one_denominator && sizeof...(Dens) == 1)) {
      if constexpr (sizeof...(Nums) == 0) *out++ = '1';
      *out++ = '/';
    } else {
      out = print_separator<CharT>(out, fmt);
    }

    if (fmt.solidus == always && sizeof...(Dens) > 1) *out++ = '(';
    bool negative_power = fmt.solidus == never || (fmt.solidus == one_denominator && sizeof...(Dens) > 1);
    out = unit_symbol_impl<CharT>(out, dens, std::index_sequence_for<Dens...>(), fmt, negative_power);
    if (fmt.solidus == always && sizeof...(Dens) > 1) *out++ = ')';
    return out;
  }
}

template<typename CharT, std::output_iterator<CharT> Out, typename... Expr>
constexpr Out unit_symbol_impl(Out out, const derived_unit<Expr...>&, unit_symbol_formatting fmt, bool negative_power)
{
  gsl_Expects(negative_power == false);
  return unit_symbol_impl<CharT>(out, typename derived_unit<Expr...>::_num_{}, typename derived_unit<Expr...>::_den_{},
                                 fmt);
}

}  // namespace detail

MP_UNITS_EXPORT template<typename CharT = char, std::output_iterator<CharT> Out, Unit U>
constexpr Out unit_symbol_to(Out out, U u, unit_symbol_formatting fmt = unit_symbol_formatting{})
{
  return detail::unit_symbol_impl<CharT>(out, u, fmt, false);
}

namespace detail {

template<typename CharT, std::size_t N, unit_symbol_formatting fmt, Unit U>
[[nodiscard]] consteval std::array<CharT, N> get_symbol_buffer(U)
{
  std::array<CharT, N> buffer{};
  unit_symbol_to<CharT>(buffer.begin(), U{}, fmt);
  return buffer;
}

}  // namespace detail


// TODO Refactor to `unit_symbol(U, fmt)` when P1045: constexpr Function Parameters is available
MP_UNITS_EXPORT template<unit_symbol_formatting fmt = unit_symbol_formatting{}, typename CharT = char, Unit U>
#if defined MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG <= 18
[[nodiscard]] constexpr auto unit_symbol(U)
#else
[[nodiscard]] consteval auto unit_symbol(U)
#endif
{
  auto get_size = []() consteval {
    std::basic_string<CharT> buffer;
    unit_symbol_to<CharT>(std::back_inserter(buffer), U{}, fmt);
    return buffer.size();
  };

#if MP_UNITS_API_STRING_VIEW_RET  // Permitting static constexpr variables in constexpr functions
  static constexpr std::size_t size = get_size();
  static constexpr auto buffer = detail::get_symbol_buffer<CharT, size, fmt>(U{});
  return std::string_view(buffer.data(), size);
#else
  constexpr std::size_t size = get_size();
  constexpr auto buffer = detail::get_symbol_buffer<CharT, size, fmt>(U{});
  return basic_fixed_string<CharT, size>(buffer.begin(), buffer.end());
#endif
}

}  // namespace mp_units
