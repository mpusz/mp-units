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
#include <mp-units/bits/get_associated_quantity.h>
#include <mp-units/bits/hacks.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/bits/text_tools.h>
#include <mp-units/compat_macros.h>
#include <mp-units/ext/algorithm.h>
#include <mp-units/ext/fixed_string.h>
#include <mp-units/ext/inplace_vector.h>
#include <mp-units/ext/type_name.h>
#include <mp-units/ext/type_traits.h>
#include <mp-units/framework/expression_template.h>
#include <mp-units/framework/magnitude.h>
#include <mp-units/framework/quantity_point_concepts.h>
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/symbol_text.h>
#include <mp-units/framework/unit_concepts.h>
#include <mp-units/framework/unit_symbol_formatting.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/ext/contracts.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <array>
#include <cstdint>
#include <iterator>
#include <string_view>
#include <tuple>
#if MP_UNITS_HOSTED
#include <string>
#endif
#endif  // MP_UNITS_IMPORT_STD
#endif  // MP_UNITS_IN_MODULE_INTERFACE

namespace mp_units {

namespace detail {

template<Magnitude auto M, Unit U>
struct scaled_unit_impl;

template<typename T>
constexpr bool is_specialization_of_scaled_unit = false;

template<typename... Expr>
struct derived_unit_impl;

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

template<Unit T, symbol_text Symbol, auto... Args>
[[nodiscard]] consteval auto get_canonical_unit_impl(T t, const named_unit<Symbol, Args...>&);

template<Unit T, symbol_text Symbol, Unit auto U, auto... Args>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const named_unit<Symbol, U, Args...>&);

template<typename T, typename F, int Num, int... Den>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const power<F, Num, Den...>&);

template<Unit T, typename... Expr>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const derived_unit_impl<Expr...>&);

template<Unit T, auto M, typename U>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const scaled_unit_impl<M, U>&);

template<Unit T, typename... Us>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const common_unit<Us...>&);

template<Unit Lhs, Unit Rhs>
struct unit_less : std::bool_constant<type_name<Lhs>() < type_name<Rhs>()> {};

template<typename T1, typename T2>
using type_list_of_unit_less = expr_less<T1, T2, unit_less>;

template<typename From, typename To>
concept PotentiallyConvertibleTo = Unit<From> && Unit<To> &&
                                   ((AssociatedUnit<From> && AssociatedUnit<To> &&
                                     implicitly_convertible(get_quantity_spec(From{}), get_quantity_spec(To{}))) ||
                                    (!AssociatedUnit<From> && !AssociatedUnit<To>));

}  // namespace detail

// TODO this should really be in the `details` namespace but is used in `chrono.h` (a part of mp_units.systems)
// Even though it is not exported, it is visible to the other module via ADL
[[nodiscard]] consteval auto get_canonical_unit(Unit auto u) { return detail::get_canonical_unit_impl(u, u); }

// convertible
template<Unit From, Unit To>
[[nodiscard]] consteval bool convertible(From from, To to)
{
  if constexpr (is_same_v<From, To>)
    return true;
  else if constexpr (detail::PotentiallyConvertibleTo<From, To>)
    return is_same_v<decltype(get_canonical_unit(from).reference_unit),
                     decltype(get_canonical_unit(to).reference_unit)>;
  else
    return false;
}

template<typename... Expr>
struct derived_unit;

namespace detail {

struct unit_interface {
  /**
   * Multiplication by `1` returns the same unit, otherwise `scaled_unit` is being returned.
   */
  template<Magnitude M, Unit U>
  [[nodiscard]] friend MP_UNITS_CONSTEVAL Unit auto operator*(M, U u)
  {
    if constexpr (std::is_same_v<M, std::remove_const_t<decltype(mp_units::mag<1>)>>)
      return u;
    else if constexpr (is_specialization_of_scaled_unit<U>) {
      if constexpr (M{} * U::_mag_ == mag<1>)
        return U::_reference_unit_;
      else
        return scaled_unit<M{} * U::_mag_, std::remove_const_t<decltype(U::_reference_unit_)>>{};
    } else
      return scaled_unit<M{}, U>{};
  }

  [[nodiscard]] friend consteval Unit auto operator*(Unit auto, Magnitude auto)
#if __cpp_deleted_function
    = delete("To scale a unit use `mag * unit` syntax");
#else
    = delete;
#endif

  /**
   * Returns the result of multiplication with an inverse unit.
   */
  template<Magnitude M, Unit U>
  [[nodiscard]] friend MP_UNITS_CONSTEVAL Unit auto operator/(M mag, U u)
  {
    return mag * inverse(u);
  }

  /**
   * `scaled_unit` specializations have priority in this operation. This means that the library framework
   * prevents passing it as an element to the `derived_unit`. In such case only the reference unit is passed
   * to the derived unit and the magnitude remains outside forming another scaled unit as a result of the operation.
   */
  template<Unit Lhs, Unit Rhs>
  [[nodiscard]] friend MP_UNITS_CONSTEVAL Unit auto operator*(Lhs lhs, Rhs rhs)
  {
    return expr_multiply<derived_unit, struct one, type_list_of_unit_less>(lhs, rhs);
  }

  /**
   * `scaled_unit` specializations have priority in this operation. This means that the library framework
   * prevents passing it as an element to the `derived_unit`. In such case only the reference unit is passed
   * to the derived unit and the magnitude remains outside forming another scaled unit as a result of the operation.
   */
  template<Unit Lhs, Unit Rhs>
  [[nodiscard]] friend MP_UNITS_CONSTEVAL Unit auto operator/(Lhs lhs, Rhs rhs)
  {
    return expr_divide<derived_unit, struct one, type_list_of_unit_less>(lhs, rhs);
  }

  template<Unit Lhs, Unit Rhs>
  [[nodiscard]] friend consteval bool operator==(Lhs, Rhs)
  {
    return is_same_v<Lhs, Rhs>;
  }

  template<Unit Lhs, Unit Rhs>
    requires(convertible(Lhs{}, Rhs{}))
  [[nodiscard]] friend consteval bool equivalent(Lhs lhs, Rhs rhs)
  {
    return get_canonical_unit(lhs).mag == get_canonical_unit(rhs).mag;
  }
};

template<Unit U, bool = requires { U::_point_origin_; }>
struct propagate_point_origin {};

template<Unit U>
struct propagate_point_origin<U, true> {
  static constexpr auto _point_origin_ = U::_point_origin_;
};

template<Magnitude auto M, Unit U>
struct scaled_unit_impl : detail::unit_interface, detail::propagate_point_origin<U> {
  using _base_type_ = scaled_unit_impl;  // exposition only
  static constexpr Magnitude auto _mag_ = M;
  static constexpr U _reference_unit_{};
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
  requires(M != magnitude<>{} && M != mag<1>)
struct scaled_unit final : detail::scaled_unit_impl<M, U> {};

namespace detail {

template<auto M, Unit U>
constexpr bool is_specialization_of_scaled_unit<scaled_unit<M, U>> = true;

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
 * inline constexpr struct second final : named_unit<"s", kind_of<time>> {} second;
 * inline constexpr struct metre final : named_unit<"m", kind_of<length> {} metre;
 * inline constexpr struct hertz final : named_unit<"Hz", inverse(second), kind_of<frequency>> {} hertz;
 * inline constexpr struct newton final : named_unit<"N", kilogram * metre / square(second)> {} newton;
 * inline constexpr struct degree_Celsius final : named_unit<{u8"℃", "`C"}, kelvin, zeroth_degree_Celsius> {}
 * degree_Celsius; inline constexpr struct minute final : named_unit<"min", mag<60> * second> {} minute;
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
struct named_unit<Symbol, QS> : detail::unit_interface {
  using _base_type_ = named_unit;           // exposition only
  static constexpr auto _symbol_ = Symbol;  ///< Unique base unit identifier
  static constexpr auto _quantity_spec_ = QS;
};

template<symbol_text Symbol, detail::QuantityKindSpec auto QS, PointOrigin auto PO>
  requires(!Symbol.empty()) && detail::BaseDimension<std::remove_const_t<decltype(QS.dimension)>>
struct named_unit<Symbol, QS, PO> : detail::unit_interface {
  using _base_type_ = named_unit;           // exposition only
  static constexpr auto _symbol_ = Symbol;  ///< Unique base unit identifier
  static constexpr auto _quantity_spec_ = QS;
  static constexpr auto _point_origin_ = PO;
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
struct named_unit<Symbol> : detail::unit_interface {
  using _base_type_ = named_unit;           // exposition only
  static constexpr auto _symbol_ = Symbol;  ///< Unique base unit identifier
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
struct named_unit<Symbol, U> : decltype(U)::_base_type_ {
  using _base_type_ = named_unit;           // exposition only
  static constexpr auto _symbol_ = Symbol;  ///< Unique unit identifier
};

template<symbol_text Symbol, Unit auto U, PointOrigin auto PO>
  requires(!Symbol.empty())
struct named_unit<Symbol, U, PO> : decltype(U)::_base_type_ {
  using _base_type_ = named_unit;           // exposition only
  static constexpr auto _symbol_ = Symbol;  ///< Unique unit identifier
  static constexpr auto _point_origin_ = PO;
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
struct named_unit<Symbol, U, QS> : decltype(U)::_base_type_ {
  using _base_type_ = named_unit;           // exposition only
  static constexpr auto _symbol_ = Symbol;  ///< Unique unit identifier
  static constexpr auto _quantity_spec_ = QS;
};

template<symbol_text Symbol, AssociatedUnit auto U, detail::QuantityKindSpec auto QS, PointOrigin auto PO>
  requires(!Symbol.empty()) && (QS.dimension == detail::get_associated_quantity(U).dimension)
struct named_unit<Symbol, U, QS, PO> : decltype(U)::_base_type_ {
  using _base_type_ = named_unit;           // exposition only
  static constexpr auto _symbol_ = Symbol;  ///< Unique unit identifier
  static constexpr auto _quantity_spec_ = QS;
  static constexpr auto _point_origin_ = PO;
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
 * constexpr kilo_<U> kilo;
 *
 * inline constexpr auto kilogram = si::kilo<gram>;
 * @endcode
 *
 * @tparam Symbol a prefix text to prepend to a unit symbol
 * @tparam M scaling factor of the prefix
 * @tparam U a named unit to be prefixed
 */
MP_UNITS_EXPORT template<symbol_text Symbol, Magnitude auto M, PrefixableUnit auto U>
  requires(!Symbol.empty())
struct prefixed_unit : decltype(M * U)::_base_type_ {
  using _base_type_ = prefixed_unit;  // exposition only
  static constexpr auto _symbol_ = Symbol + U._symbol_;
};

namespace detail {

template<Unit U1, Unit U2>
  requires(convertible(U1{}, U2{}))
[[nodiscard]] consteval Unit auto get_common_scaled_unit(U1, U2)
{
  constexpr auto canonical_lhs = get_canonical_unit(U1{});
  constexpr auto canonical_rhs = get_canonical_unit(U2{});
  constexpr auto common_magnitude = _common_magnitude(canonical_lhs.mag, canonical_rhs.mag);
  if constexpr (common_magnitude == mag<1>)
    return canonical_lhs.reference_unit;
  else
    return scaled_unit<common_magnitude, std::remove_const_t<decltype(canonical_lhs.reference_unit)>>{};
}

[[nodiscard]] consteval Unit auto get_common_scaled_unit(Unit auto u1, Unit auto u2, Unit auto u3, Unit auto... rest)
  requires requires { get_common_scaled_unit(get_common_scaled_unit(u1, u2), u3, rest...); }
{
  return get_common_scaled_unit(get_common_scaled_unit(u1, u2), u3, rest...);
}

}  // namespace detail

/**
 * @brief Measurement unit for an accumulation of two quantities of different units
 *
 * While adding two quantities of different units we can often identify which of those unit should be used
 * to prevent data truncation. For example, adding `1 * m + 1 * mm` will end up in a quantity expressed in
 * millimeters. However, for some cases this is not possible. Choosing any of the units from the arguments
 * of the addition would result in a data truncation. For example, a common unit for `1 * km + 1 * mi` is
 * `[8/125] m`. Instead of returning such a complex unit type the library will return a `common_unit<mi, km>`.
 * This type is convertible to both `mi` and `km` without risking data truncation, but is not equal to any
 * of them.
 *
 * @note User should not instantiate this type! It is not exported from the C++ module. The library will
 *       instantiate this type automatically based on the unit arithmetic equation provided by the user.
 */
template<Unit U1, Unit U2, Unit... Rest>
struct common_unit final : decltype(detail::get_common_scaled_unit(U1{}, U2{}, Rest{}...))::_base_type_
{
  using _base_type_ = common_unit;  // exposition only
  static constexpr auto _common_unit_ = detail::get_common_scaled_unit(U1{}, U2{}, Rest{}...);
};

namespace detail {

template<typename... Expr>
struct derived_unit_impl : detail::unit_interface, detail::expr_fractions<one, Expr...> {
  using _base_type_ = derived_unit_impl;  // exposition only
};

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
template<typename... Expr>
struct derived_unit final : detail::derived_unit_impl<Expr...> {};

/**
 * @brief Unit one
 *
 * Unit of a dimensionless quantity.
 */
// clang-format off
MP_UNITS_EXPORT inline constexpr struct one final : detail::derived_unit_impl<> {} one;
// clang-format on

namespace detail {

template<Unit T, auto M, typename U>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const scaled_unit_impl<M, U>&)
{
  auto base = get_canonical_unit_impl(U{}, U{});
  return canonical_unit{M * base.mag, base.reference_unit};
}

template<Unit T, typename... Us>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const common_unit<Us...>& u)
{
  return get_canonical_unit_impl(u._common_unit_, u._common_unit_);
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
  auto mag = (mp_units::mag<1> * ... * _pow<Num, Den...>(get_canonical_unit_impl(Us{}, Us{}).mag));
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
    return canonical_unit{_pow<Num, Den...>(base.mag) * num.mag / den.mag, num.reference_unit / den.reference_unit};
  } else {
    return canonical_unit{_pow<Num, Den...>(base.mag),
                          derived_unit<power<decltype(base.reference_unit), Num, Den...>>{}};
  }
}

template<typename... Us>
[[nodiscard]] consteval auto get_canonical_unit_impl(const type_list<Us...>&)
{
  auto magnitude = (mp_units::mag<1> * ... * get_canonical_unit_impl(Us{}, Us{}).mag);
  auto u = (one * ... * get_canonical_unit_impl(Us{}, Us{}).reference_unit);
  return canonical_unit{magnitude, u};
}

template<Unit T, typename... Expr>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const derived_unit_impl<Expr...>&)
{
  auto num = get_canonical_unit_impl(typename derived_unit<Expr...>::_num_{});
  auto den = get_canonical_unit_impl(typename derived_unit<Expr...>::_den_{});
  return canonical_unit{num.mag / den.mag, num.reference_unit / den.reference_unit};
}

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

[[nodiscard]] MP_UNITS_CONSTEVAL Unit auto inverse(Unit auto u) { return one / u; }

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
  return detail::expr_pow<Num, Den, derived_unit, struct one, detail::type_list_of_unit_less>(u);
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
inline constexpr struct percent final : named_unit<"%", mag_ratio<1, 100> * one> {} percent;
inline constexpr struct per_mille final : named_unit<symbol_text{u8"‰" /* U+2030 PER MILLE SIGN */, "%o"}, mag_ratio<1, 1000> * one> {} per_mille;
inline constexpr struct parts_per_million final : named_unit<"ppm", mag_ratio<1, 1'000'000> * one> {} parts_per_million;
inline constexpr auto ppm = parts_per_million;
// clang-format on


// Common unit
[[nodiscard]] consteval Unit auto get_common_unit(Unit auto u) { return u; }

template<Unit U1, Unit U2>
  requires(convertible(U1{}, U2{}))
[[nodiscard]] consteval Unit auto get_common_unit(U1 u1, U2 u2)
{
  if constexpr (is_same_v<U1, U2>)
    return u1;
  else if constexpr (equivalent(U1{}, U2{})) {
    if constexpr (std::derived_from<U1, typename U2::_base_type_>)
      return u1;
    else if constexpr (std::derived_from<U2, typename U1::_base_type_>)
      return u2;
    else
      // TODO Check if there is a better choice here
      return detail::better_type_name(u1, u2);
  } else {
    constexpr auto canonical_lhs = get_canonical_unit(U1{});
    constexpr auto canonical_rhs = get_canonical_unit(U2{});

    if constexpr (_is_positive_integral_power(canonical_lhs.mag / canonical_rhs.mag))
      return u2;
    else if constexpr (_is_positive_integral_power(canonical_rhs.mag / canonical_lhs.mag))
      return u1;
    else {
      if constexpr (detail::unit_less<U1, U2>::value)
        return common_unit<U1, U2>{};
      else
        return common_unit<U2, U1>{};
    }
  }
}

namespace detail {

template<TypeList List, Unit NewUnit, bool Included, Unit... Us>
struct collapse_common_unit_impl;

template<TypeList List, Unit NewUnit, bool Included, Unit Front, Unit... Rest>
struct collapse_common_unit_impl<List, NewUnit, Included, Front, Rest...> {
  using cu = decltype(get_common_unit(NewUnit{}, Front{}));
  using type =
    conditional<is_specialization_of<cu, common_unit>,
                typename collapse_common_unit_impl<type_list_push_back<List, Front>, NewUnit, Included, Rest...>::type,
                typename collapse_common_unit_impl<type_list_push_back<List, cu>, NewUnit, true, Rest...>::type>;
};

template<TypeList List, Unit NewUnit>
struct collapse_common_unit_impl<List, NewUnit, false> {
  using type = type_list_push_back<List, NewUnit>;
};

template<TypeList List, Unit NewUnit>
struct collapse_common_unit_impl<List, NewUnit, true> {
  using type = List;
};

template<Unit NewUnit, Unit... Us>
using collapse_common_unit = type_list_unique<
  type_list_sort<typename collapse_common_unit_impl<type_list<>, NewUnit, false, Us...>::type, type_list_of_unit_less>>;

}  // namespace detail

template<Unit... Us, Unit NewUnit>
  requires(convertible(common_unit<Us...>{}, NewUnit{}))
[[nodiscard]] consteval Unit auto get_common_unit(common_unit<Us...>, NewUnit)
{
  using type = detail::collapse_common_unit<NewUnit, Us...>;
  if constexpr (detail::type_list_size<type> == 1)
    return detail::type_list_front<type>{};
  else
    return detail::type_list_map<type, common_unit>{};
}

template<Unit... Us, Unit NewUnit>
  requires(convertible(common_unit<Us...>{}, NewUnit{}))
[[nodiscard]] consteval Unit auto get_common_unit(NewUnit nu, common_unit<Us...> cu)
{
  return get_common_unit(cu, nu);
}

[[nodiscard]] consteval Unit auto get_common_unit(Unit auto u1, Unit auto u2, Unit auto u3, Unit auto... rest)
  requires requires { get_common_unit(get_common_unit(u1, u2), u3, rest...); }
{
  return get_common_unit(get_common_unit(u1, u2), u3, rest...);
}


/**
 * @brief Puts a space ' ' sign before a unit symbol
 *
 * Quantities of some units (e.g. degree, arcminute, arcsecond) should not be printed with the
 * space between a number and a unit. For those a partial specialization with the value `false` should
 * be provided.
 */
template<Unit auto U>
constexpr bool space_before_unit_symbol = true;

template<>
MP_UNITS_INLINE constexpr bool space_before_unit_symbol<one> = false;

MP_UNITS_EXPORT_END

namespace detail {

template<typename CharT, std::output_iterator<CharT> Out, Unit U>
  requires requires { U::_symbol_; }
constexpr Out unit_symbol_impl(Out out, U, const unit_symbol_formatting& fmt, bool negative_power)
{
  return copy_symbol<CharT>(U::_symbol_, fmt.encoding, negative_power, out);
}

template<typename CharT, std::output_iterator<CharT> Out, auto M, typename U>
constexpr Out unit_symbol_impl(Out out, const scaled_unit_impl<M, U>& u, const unit_symbol_formatting& fmt,
                               bool negative_power)
{
  *out++ = '[';
  _magnitude_symbol<CharT>(out, M, fmt);
  if constexpr (space_before_unit_symbol<scaled_unit<M, U>::_reference_unit_>) *out++ = ' ';
  unit_symbol_impl<CharT>(out, u._reference_unit_, fmt, negative_power);
  *out++ = ']';
  return out;
}

template<typename... Us, Unit U>
[[nodiscard]] consteval Unit auto get_common_unit_in(common_unit<Us...>, U u)
{
  auto get_magnitude = [&]() {
    if constexpr (requires { common_unit<Us...>::_mag_; })
      return common_unit<Us...>::_mag_;
    else
      return mag<1>;
  };
  constexpr auto canonical_u = get_canonical_unit(u);
  constexpr Magnitude auto cmag = get_magnitude() / canonical_u.mag;
  if constexpr (cmag == mag<1>)
    return u;
  else
    return scaled_unit<cmag, U>{};
}

template<typename CharT, std::output_iterator<CharT> Out, typename U, typename... Rest>
constexpr Out unit_symbol_impl(Out out, const common_unit<U, Rest...>&, const unit_symbol_formatting& fmt,
                               bool negative_power)
{
  constexpr std::string_view prefix("EQUIV{");
  constexpr std::string_view separator(", ");
  auto print_unit = [&]<Unit Arg>(Arg) {
    constexpr auto u = get_common_unit_in(common_unit<U, Rest...>{}, Arg{});
    unit_symbol_impl<CharT>(out, u, fmt, negative_power);
  };
  detail::copy(std::begin(prefix), std::end(prefix), out);
  print_unit(U{});
  for_each(std::tuple<Rest...>{}, [&]<Unit Arg>(Arg) {
    detail::copy(std::begin(separator), std::end(separator), out);
    print_unit(Arg{});
  });
  *out++ = '}';
  return out;
}

template<typename CharT, std::output_iterator<CharT> Out, typename F, int Num, int... Den>
constexpr auto unit_symbol_impl(Out out, const power<F, Num, Den...>&, const unit_symbol_formatting& fmt,
                                bool negative_power)
{
  out = unit_symbol_impl<CharT>(out, F{}, fmt, false);  // negative power component will be added below if needed
  return copy_symbol_exponent<CharT, Num, Den...>(fmt.encoding, negative_power, out);
}

template<typename CharT, std::output_iterator<CharT> Out, typename... Us>
constexpr Out unit_symbol_impl(Out out, const type_list<>&, const unit_symbol_formatting&, bool)
{
  return out;
}

template<typename CharT, std::output_iterator<CharT> Out, typename U, typename... Rest>
constexpr Out unit_symbol_impl(Out out, const type_list<U, Rest...>&, const unit_symbol_formatting& fmt,
                               bool negative_power)
{
  return ((out = unit_symbol_impl<CharT>(out, U{}, fmt, negative_power)), ...,
          (print_separator<CharT>(out, fmt), out = unit_symbol_impl<CharT>(out, Rest{}, fmt, negative_power)));
}

template<typename CharT, std::output_iterator<CharT> Out, typename... Nums, typename... Dens>
constexpr Out unit_symbol_impl(Out out, const type_list<Nums...>& nums, const type_list<Dens...>& dens,
                               const unit_symbol_formatting& fmt)
{
  if constexpr (sizeof...(Nums) == 0 && sizeof...(Dens) == 0) {
    // dimensionless quantity
    return out;
  } else if constexpr (sizeof...(Dens) == 0) {
    // no denominator
    return unit_symbol_impl<CharT>(out, nums, fmt, false);
  } else {
    using enum unit_symbol_solidus;
    if constexpr (sizeof...(Nums) > 0) out = unit_symbol_impl<CharT>(out, nums, fmt, false);

    if (fmt.solidus == always || (fmt.solidus == one_denominator && sizeof...(Dens) == 1)) {
      if constexpr (sizeof...(Nums) == 0) *out++ = '1';
      *out++ = '/';
      if (sizeof...(Dens) > 1) *out++ = '(';
    } else if constexpr (sizeof...(Nums) > 0) {
      out = print_separator<CharT>(out, fmt);
    }

    const bool negative_power = fmt.solidus == never || (fmt.solidus == one_denominator && sizeof...(Dens) > 1);
    out = unit_symbol_impl<CharT>(out, dens, fmt, negative_power);
    if (fmt.solidus == always && sizeof...(Dens) > 1) *out++ = ')';
    return out;
  }
}

template<typename CharT, std::output_iterator<CharT> Out, typename... Expr>
constexpr Out unit_symbol_impl(Out out, const derived_unit_impl<Expr...>&, const unit_symbol_formatting& fmt,
                               bool negative_power)
{
  (void)negative_power;
  MP_UNITS_EXPECTS(negative_power == false);
  return unit_symbol_impl<CharT>(out, typename derived_unit_impl<Expr...>::_num_{},
                                 typename derived_unit_impl<Expr...>::_den_{}, fmt);
}

}  // namespace detail

MP_UNITS_EXPORT template<typename CharT = char, std::output_iterator<CharT> Out, Unit U>
constexpr Out unit_symbol_to(Out out, U u, const unit_symbol_formatting& fmt = unit_symbol_formatting{})
{
  return detail::unit_symbol_impl<CharT>(out, u, fmt, false);
}

// TODO Refactor to `unit_symbol(U, fmt)` when P1045: constexpr Function Parameters is available
MP_UNITS_EXPORT template<unit_symbol_formatting fmt = unit_symbol_formatting{}, typename CharT = char, Unit U>
#if defined MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG <= 18
[[nodiscard]] constexpr auto unit_symbol(U)
#else
[[nodiscard]] consteval auto unit_symbol(U)
#endif
{
  constexpr auto oversized_symbol_text = []() consteval {
    // std::basic_string<CharT> text;  // TODO uncomment when https://wg21.link/P3032 is supported
    detail::inplace_vector<CharT, 128> text;
    unit_symbol_to<CharT>(std::back_inserter(text), U{}, fmt);
    return text;
  }();

#if MP_UNITS_API_STRING_VIEW_RET  // Permitting static constexpr variables in constexpr functions
  static constexpr basic_fixed_string<CharT, oversized_symbol_text.size()> storage(std::from_range,
                                                                                   oversized_symbol_text);
  return storage.view();
#else
  return basic_fixed_string<CharT, oversized_symbol_text.size()>(std::from_range, oversized_symbol_text);
#endif
}

}  // namespace mp_units
