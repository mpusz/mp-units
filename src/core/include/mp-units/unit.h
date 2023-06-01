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

#include <mp-units/bits/algorithm.h>
#include <mp-units/bits/expression_template.h>
#include <mp-units/bits/external/fixed_string.h>
#include <mp-units/bits/external/type_name.h>
#include <mp-units/bits/external/type_traits.h>
#include <mp-units/bits/get_associated_quantity.h>
#include <mp-units/bits/magnitude.h>
#include <mp-units/bits/quantity_spec_concepts.h>
#include <mp-units/bits/ratio.h>
#include <mp-units/bits/symbol_text.h>
#include <mp-units/bits/text_tools.h>
#include <mp-units/bits/unit_concepts.h>
#include <iterator>
#include <string>

namespace mp_units {

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
struct scaled_unit {
  static constexpr UNITS_CONSTRAINED_AUTO_WORKAROUND(Magnitude) auto mag = M;
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
 * of units (i.e. `metre`) or a name assigned to another scaled or derived unit
 * (i.e. `hour`, `joule`).
 * Most of the named units may be composed with a prefix to create a `prefixed_unit`.
 *
 * For example:
 *
 * @code{.cpp}
 * inline constexpr struct second : named_unit<"s", time> {} second;
 * inline constexpr struct metre : named_unit<"m", length> {} metre;
 * inline constexpr struct hertz : named_unit<"Hz", 1 / second> {} hertz;
 * inline constexpr struct newton : named_unit<"N", kilogram * metre / square(second)> {} newton;
 * inline constexpr struct degree_Celsius : named_unit<basic_symbol_text{"°C", "`C"}, kelvin> {} degree_Celsius;
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
template<basic_symbol_text Symbol, auto...>
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
template<basic_symbol_text Symbol, detail::QuantityKindSpec auto QS>
  requires(!Symbol.empty()) && detail::BaseDimension<std::remove_const_t<decltype(QS.dimension)>>
struct named_unit<Symbol, QS> {
  static constexpr auto symbol = Symbol;  ///< Unique base unit identifier
  static constexpr auto quantity_spec = QS;
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
template<basic_symbol_text Symbol>
  requires(!Symbol.empty())
struct named_unit<Symbol> {
  static constexpr auto symbol = Symbol;  ///< Unique base unit identifier
};

/**
 * @brief Specialization for a unit with special name
 *
 * Allows assigning a special name to another scaled or derived unit (i.e. `hour`, `joule`).
 *
 * @tparam Symbol a short text representation of the unit
 * @tparam Unit a unit for which we provide a special name
 */
template<basic_symbol_text Symbol, Unit auto U>
  requires(!Symbol.empty())
struct named_unit<Symbol, U> : std::remove_const_t<decltype(U)> {
  static constexpr auto symbol = Symbol;  ///< Unique unit identifier
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
template<basic_symbol_text Symbol, AssociatedUnit auto U, detail::QuantityKindSpec auto QS>
  requires(!Symbol.empty()) && (QS.dimension == detail::get_associated_quantity(U).dimension)
struct named_unit<Symbol, U, QS> : std::remove_const_t<decltype(U)> {
  static constexpr auto symbol = Symbol;  ///< Unique unit identifier
  static constexpr auto quantity_spec = QS;
};

/**
 * @brief A unit of a physical constant
 *
 * Defines a unit of a physical constant together with its value encoded as a unit ratio.
 *
 * This allows moving all of the constant-related ratio manipulation to the compile-time domain
 * (i.e. multiplying and then dividing by the same constant will eliminate the item from the final
 * type).
 * As a result we have faster runtime performance and no precision loss due to eager floating-point
 * operations. Also, if the user prefers integral types for a quantity representation, this will
 * not force the user to convert to a floating-point type right away. Only when a final quantity
 * number needs to actually account for the constant value, the floating-point operation (if any)
 * can be triggered lazily with the `value_cast<U>()`.
 *
 * For example:
 *
 * @code{.cpp}
 * inline constexpr struct standard_gravity_unit :
 *   constant_unit<"g", mag<ratio{980'665, 100'000}> * metre / square(second)> {} standard_gravity_unit;
 * @endcode
 *
 * @note A common convention in this library is to assign the same name for a type and an object of this type.
 *       Besides defining them user never works with the unit types in the source code. All operations
 *       are done on the objects. Contrarily, the unit types are the only one visible in the compilation
 *       errors. Having them of the same names improves user experience and somehow blurs those separate domains.
 *
 * @tparam Symbol a short text representation of the constant
 *
 * @note Constant symbol is printed in the text output encapsulated inside square brackets `[]`
 *       and before any regular units
 */
template<basic_symbol_text Symbol, Unit auto U>
  requires(!Symbol.empty())
struct constant_unit : named_unit<'[' + Symbol + ']', U> {};

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
template<basic_symbol_text Symbol, Magnitude auto M, PrefixableUnit auto U>
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
 * static_assert(is_of_type<1 / second, derived_unit<one, per<second>>>);
 * static_assert(is_of_type<1 / (1 / second), second>);
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
inline constexpr struct one : derived_unit<> {} one;
// clang-format on

namespace detail {

template<>
struct is_one<struct one> : std::true_type {};

/**
 * @brief A canonical representation of a unit
 *
 * A canonical representation of a unit consists of a `reference_unit` and its scaling
 * factor represented by the magnitude `mag`.
 *
 * `reference_unit` is a unit (possibly derived one) that consists only named base units.
 * All of the intermediate derived units are extracted, prefixes and magnitudes of scaled
 * units are stripped from them and accounted in the `mag`.
 *
 * All units having the same canonical unit are deemed equal.
 * All units having the same `reference_unit` are convertible (their `mag` may differ
 * and is the subject of conversion).
 *
 * @tparam U a unit to use as a `reference_unit`
 * @tparam M a Magnitude representing an absolute scaling factor of this unit
 */
template<Magnitude M, Unit U>
struct canonical_unit {
  M mag;
  U reference_unit;
};

template<Unit T, basic_symbol_text Symbol, detail::QuantityKindSpec auto Q>
[[nodiscard]] consteval auto get_canonical_unit_impl(T t, const named_unit<Symbol, Q>&);

template<Unit T, basic_symbol_text Symbol>
[[nodiscard]] consteval auto get_canonical_unit_impl(T t, const named_unit<Symbol>&);

template<Unit T, basic_symbol_text Symbol, Unit auto U>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const named_unit<Symbol, U>&);

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

template<Unit T, basic_symbol_text Symbol, detail::QuantityKindSpec auto Q>
[[nodiscard]] consteval auto get_canonical_unit_impl(T t, const named_unit<Symbol, Q>&)
{
  return canonical_unit{mag<1>, t};
}

template<Unit T, basic_symbol_text Symbol>
[[nodiscard]] consteval auto get_canonical_unit_impl(T t, const named_unit<Symbol>&)
{
  return canonical_unit{mag<1>, t};
}

template<Unit T, basic_symbol_text Symbol, Unit auto U>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const named_unit<Symbol, U>&)
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
  auto mag = (mp_units::mag<1> * ... * get_canonical_unit_impl(Us{}, Us{}).mag);
  auto u = (one * ... * get_canonical_unit_impl(Us{}, Us{}).reference_unit);
  return canonical_unit{mag, u};
}

template<Unit T, typename... Expr>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const derived_unit<Expr...>&)
{
  auto num = get_canonical_unit_impl(typename derived_unit<Expr...>::_num_{});
  auto den = get_canonical_unit_impl(typename derived_unit<Expr...>::_den_{});
  return canonical_unit{num.mag / den.mag, num.reference_unit / den.reference_unit};
}

[[nodiscard]] consteval auto get_canonical_unit(Unit auto u) { return get_canonical_unit_impl(u, u); }

template<Unit Lhs, Unit Rhs>
[[nodiscard]] consteval bool less(Lhs, Rhs)
{
  if constexpr ((is_derived_from_specialization_of_constant_unit<Lhs> &&
                 is_derived_from_specialization_of_constant_unit<Rhs>) ||
                (!is_derived_from_specialization_of_constant_unit<Lhs> &&
                 !is_derived_from_specialization_of_constant_unit<Rhs>)) {
    if constexpr (requires {
                    Lhs::symbol;
                    Rhs::symbol;
                  })
      // prefer symbols comparison if possible as it gives typically better results
      // i.e. it puts upper case in from so `N m` is correct
      return Lhs::symbol < Rhs::symbol;
    else
      return type_name<Lhs>() < type_name<Rhs>();
  } else {
    // put constants at the front of units list in the expression
    return is_derived_from_specialization_of_constant_unit<Lhs>;
  }
}


// TODO What if the same unit will have different types (i.e. user will inherit its own type from `metre`)?
// Is there a better way to sort units here? Some of them may not have symbol at all (like all units of
// dimensionless quantities).
template<Unit Lhs, Unit Rhs>
struct unit_less : std::bool_constant<less(Lhs{}, Rhs{})> {};

template<typename T1, typename T2>
using type_list_of_unit_less = expr_less<T1, T2, unit_less>;

}  // namespace detail


// Operators

/**
 * Multiplication by `1` returns the same unit, otherwise `scaled_unit` is being returned.
 */
template<Magnitude M, Unit U>
[[nodiscard]] consteval Unit auto operator*(M mag, const U u)
{
  if constexpr (mag == mp_units::mag<1>)
    return u;
  else
    return scaled_unit<mag, U>{};
}

[[nodiscard]] consteval Unit auto operator*(Unit auto, Magnitude auto) = delete;

/**
 * `scaled_unit` specializations have priority in this operation. This means that the library framework
 * prevents passing it as an element to the `derived_unit`. In such case only the reference unit is passed
 * to the derived unit and the magnitude remains outside forming another scaled unit as a result of the operation.
 */
template<Unit Lhs, Unit Rhs>
[[nodiscard]] consteval Unit auto operator*(Lhs lhs, Rhs rhs)
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
[[nodiscard]] consteval Unit auto operator/(Lhs lhs, Rhs rhs)
{
  if constexpr (detail::is_specialization_of_scaled_unit<Lhs> && detail::is_specialization_of_scaled_unit<Rhs>)
    return (Lhs::mag / Rhs::mag) * (Lhs::reference_unit / Rhs::reference_unit);
  else if constexpr (detail::is_specialization_of_scaled_unit<Lhs>)
    return Lhs::mag * (Lhs::reference_unit / rhs);
  else if constexpr (detail::is_specialization_of_scaled_unit<Rhs>)
    return Rhs::mag * (lhs / Rhs::reference_unit);
  else
    return detail::expr_divide<derived_unit, struct one, detail::type_list_of_unit_less>(lhs, rhs);
}

[[nodiscard]] consteval Unit auto operator/(int value, Unit auto u)
{
  gsl_Expects(value == 1);
  return detail::expr_invert<derived_unit, struct one>(u);
}

[[nodiscard]] consteval Unit auto operator/(Unit auto, int) = delete;

namespace detail {

[[nodiscard]] consteval bool have_same_canonical_reference_unit_impl(...) { return false; }

template<basic_symbol_text Symbol, auto... D>
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
  auto canonical_lhs = detail::get_canonical_unit(u1);
  auto canonical_rhs = detail::get_canonical_unit(u2);
  return have_same_canonical_reference_unit_impl(canonical_lhs.reference_unit, canonical_rhs.reference_unit);
}

}  // namespace detail


[[nodiscard]] consteval bool operator==(Unit auto lhs, Unit auto rhs)
{
  auto canonical_lhs = detail::get_canonical_unit(lhs);
  auto canonical_rhs = detail::get_canonical_unit(rhs);
  return detail::have_same_canonical_reference_unit(canonical_lhs.reference_unit, canonical_rhs.reference_unit) &&
         canonical_lhs.mag == canonical_rhs.mag;
}

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
  if constexpr (requires { U::symbol; }) {
    if constexpr (Den == 1)
      return derived_unit<power<U, Num>>{};
    else
      return derived_unit<power<U, Num, Den>>{};
  } else if constexpr (detail::is_specialization_of_scaled_unit<U>) {
    return scaled_unit<pow<Num, Den>(U::mag), std::remove_const_t<decltype(pow<Num, Den>(U::reference_unit))>>{};
  } else {
    return detail::expr_pow<Num, Den, derived_unit, struct one, detail::type_list_of_unit_less>(u);
  }
}

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
inline constexpr struct per_mille : named_unit<basic_symbol_text{"‰", "%o"}, mag<ratio(1, 1000)> * one> {} per_mille;
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
    else
      return u2;
  } else {
    constexpr auto canonical_lhs = detail::get_canonical_unit(U1{});
    constexpr auto canonical_rhs = detail::get_canonical_unit(U2{});

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


// get_unit_symbol

enum class text_encoding {
  unicode,  // m³;  µs
  ascii,    // m^3; us
  default_encoding = unicode
};

enum class unit_symbol_solidus {
  one_denominator,  // m/s;   kg m-1 s-1
  always,           // m/s;   kg/(m s)
  never,            // m s-1; kg m-1 s-1
  default_denominator = one_denominator
};

enum class unit_symbol_separator {
  space,  // kg m²/s²
  dot,    // kg⋅m²/s²  (valid only for unicode encoding)
  default_separator = space
};

struct unit_symbol_formatting {
  text_encoding encoding = text_encoding::default_encoding;
  unit_symbol_solidus solidus = unit_symbol_solidus::default_denominator;
  unit_symbol_separator separator = unit_symbol_separator::default_separator;
};

namespace detail {

// TODO Should `basic_symbol_text` be fixed to use `char` type for both encodings?
template<typename CharT, typename UnicodeCharT, std::size_t N, std::size_t M, std::output_iterator<CharT> Out>
constexpr Out copy(const basic_symbol_text<UnicodeCharT, N, M>& txt, text_encoding encoding, Out out)
{
  if (encoding == text_encoding::unicode) {
    if (is_same_v<CharT, UnicodeCharT>)
      return copy(txt.unicode(), out).out;
    else
      throw std::invalid_argument("Unicode text can't be copied to CharT output");
  } else {
    if (is_same_v<CharT, char>)
      return copy(txt.ascii(), out).out;
    else
      throw std::invalid_argument("ASCII text can't be copied to CharT output");
  }
}

template<typename CharT, std::output_iterator<CharT> Out>
constexpr Out print_separator(Out out, unit_symbol_formatting fmt)
{
  if (fmt.separator == unit_symbol_separator::dot) {
    if (fmt.encoding != text_encoding::unicode)
      throw std::invalid_argument("'unit_symbol_separator::dot' can be only used with 'text_encoding::unicode'");
    copy(std::string_view("⋅"), out);
  } else {
    *out++ = ' ';
  }
  return out;
}

template<typename CharT, std::output_iterator<CharT> Out, Unit U>
  requires requires { U::symbol; }
constexpr Out unit_symbol_impl(Out out, U, unit_symbol_formatting fmt, bool negative_power)
{
  out = copy<CharT>(U::symbol, fmt.encoding, out);
  if (negative_power) {
    constexpr auto txt = superscript<-1>();
    out = copy<CharT>(txt, fmt.encoding, out);
  }
  return out;
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

    if constexpr (std::derived_from<std::remove_const_t<decltype(u.reference_unit)>, derived_unit<>>)
      return out;
    else {
      *out++ = ' ';
      return unit_symbol_impl<CharT>(out, u.reference_unit, fmt, negative_power);
    }
  }
}

template<typename CharT, std::output_iterator<CharT> Out, typename F, int Num, int... Den>
constexpr auto unit_symbol_impl(Out out, const power<F, Num, Den...>&, unit_symbol_formatting fmt, bool negative_power)
{
  out = unit_symbol_impl<CharT>(out, F{}, fmt, false);  // negative power component will be added below if needed

  constexpr ratio r = power<F, Num, Den...>::exponent;
  if constexpr (r.den != 1) {
    // add root part
    constexpr auto txt = basic_fixed_string("^(") + regular<r.num>() + basic_fixed_string("/") + regular<r.den>() +
                         basic_fixed_string(")");
    return copy<CharT>(txt, fmt.encoding, out);
  } else if constexpr (r.num != 1) {
    // add exponent part
    if (negative_power) {
      constexpr auto txt = superscript<-r.num>();
      return copy<CharT>(txt, fmt.encoding, out);
    } else {
      constexpr auto txt = superscript<r.num>();
      return copy<CharT>(txt, fmt.encoding, out);
    }
  }
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
      unit_symbol_impl<CharT>(out, nums, std::index_sequence_for<Nums...>(), fmt, false);
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


template<typename CharT = char, std::output_iterator<CharT> Out, Unit U>
constexpr Out unit_symbol_to(Out out, U u, unit_symbol_formatting fmt = unit_symbol_formatting{})
{
  return detail::unit_symbol_impl<CharT>(out, u, fmt, false);
}

template<typename CharT = char, Unit U>
[[nodiscard]] constexpr std::basic_string<CharT> unit_symbol(U u, unit_symbol_formatting fmt = unit_symbol_formatting{})
{
  std::basic_string<CharT> buffer;
  unit_symbol_to<CharT>(std::back_inserter(buffer), u, fmt);
  return buffer;
}

}  // namespace mp_units
