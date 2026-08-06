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

// IWYU pragma: private, include <mp-units/framework/unit.h>
#include <mp-units/bits/get_associated_quantity.h>
#include <mp-units/bits/hacks.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/bits/unit_magnitude.h>
#include <mp-units/ext/type_traits.h>
#include <mp-units/framework/quantity_point_concepts.h>
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/quantity_spec_definitions.h>
#include <mp-units/framework/symbol_text.h>
#include <mp-units/framework/symbolic_expression.h>
#include <mp-units/framework/unit_concepts.h>
#include <mp-units/framework/unit_magnitude.h>
#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cstdint>
#include <type_traits>
#endif
#endif

namespace mp_units {

MP_UNITS_EXPORT template<symbol_text Symbol, Unit auto U, auto...>
struct named_constant;

namespace detail {

template<UnitMagnitude auto M, Unit U>
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
template<UnitMagnitude M, Unit U>
struct canonical_unit {
  M mag;
  U reference_unit;
};

#if MP_UNITS_COMP_CLANG

template<UnitMagnitude M, Unit U>
canonical_unit(M, U) -> canonical_unit<M, U>;

#endif

template<Unit T, symbol_text Symbol, auto... Args>
[[nodiscard]] consteval auto get_canonical_unit_impl(T t, const named_unit<Symbol, Args...>&);

template<Unit T, symbol_text Symbol, Unit auto U, auto... Args>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const named_unit<Symbol, U, Args...>&);

template<Unit T, symbol_text Symbol, Unit auto U, auto... Args>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const named_constant<Symbol, U, Args...>&);

template<typename T, typename F, int Num, int... Den>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const power<F, Num, Den...>&);

template<Unit T, typename... Expr>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const derived_unit_impl<Expr...>&);

template<Unit T, auto M, typename U>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const scaled_unit_impl<M, U>&);

template<Unit T, typename... Us>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const common_unit<Us...>&);

template<Unit U>
constexpr auto get_canonical_unit_result = get_canonical_unit_impl(U{}, U{});

template<symbol_text Symbol, auto... Args>
[[nodiscard]] consteval bool unit_mag_is_positive(const named_unit<Symbol, Args...>&);

template<symbol_text Symbol, Unit auto U, auto... Args>
[[nodiscard]] consteval bool unit_mag_is_positive(const named_unit<Symbol, U, Args...>&);

template<symbol_text Symbol, Unit auto U, auto... Args>
[[nodiscard]] consteval bool unit_mag_is_positive(const named_constant<Symbol, U, Args...>&);

template<typename F, int Num, int... Den>
[[nodiscard]] consteval bool unit_mag_is_positive(const power<F, Num, Den...>&);

template<typename... Expr>
[[nodiscard]] consteval bool unit_mag_is_positive(const derived_unit_impl<Expr...>&);

template<auto M, typename U>
[[nodiscard]] consteval bool unit_mag_is_positive(const scaled_unit_impl<M, U>&);

template<typename... Us>
[[nodiscard]] consteval bool unit_mag_is_positive(const common_unit<Us...>&);

}  // namespace detail

// TODO this should really be in the `details` namespace and not exported but is used in `chrono.h`
// (a part of mp_units.systems)
MP_UNITS_EXPORT [[nodiscard]] consteval auto get_canonical_unit(Unit auto u)
{
  return detail::get_canonical_unit_result<decltype(u)>;
}

namespace detail {

// This is evaluated for every named unit definition while system headers are being parsed, so it has to stay cheap.
// The magnitude sign is determined structurally (a `negative_tag` always sorts first in a magnitude pack), which
// avoids computing the canonical unit and prime-factorizing its magnitude only to read the sign.
template<Unit auto U>
constexpr bool is_positive_canonical_unit_mag = unit_mag_is_positive(U);

}  // namespace detail

template<UnitMagnitude auto M, Unit U>
  requires(M != mag<1>)  // `mag<1>` is the empty `unit_magnitude<>`, so a single check covers both spellings
struct scaled_unit;

template<detail::SymbolicConstant... Expr>
struct derived_unit;

MP_UNITS_EXPORT struct one;

namespace detail {

struct unit_interface {
  /**
   * Multiplication by `1` returns the same unit, otherwise `scaled_unit` is being returned.
   */
  template<UnitMagnitude M, Unit U>
  [[nodiscard]] friend MP_UNITS_CONSTEVAL Unit auto operator*(M, U u)
  {
    if constexpr (std::is_same_v<M, MP_UNITS_NONCONST_TYPE(mp_units::mag<1>)>)
      return u;
    else if constexpr (is_specialization_of_scaled_unit<U>) {
      if constexpr (M{} * U::_mag_ == mag<1>)
        return U::_reference_unit_;
      else
        return scaled_unit<M{} * U::_mag_, MP_UNITS_NONCONST_TYPE(U::_reference_unit_)>{};
    } else
      return scaled_unit<M{}, U>{};
  }

  [[nodiscard]] friend consteval Unit auto operator*(Unit auto, UnitMagnitude auto)
#if __cpp_deleted_function
    = delete ("To scale a unit use `mag * unit` syntax");
#else
    = delete;
#endif

  /**
   * Returns the result of multiplication with an inverse unit.
   */
  template<UnitMagnitude M, Unit U>
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
    return expr_multiply<derived_unit, one>(lhs, rhs);
  }

  /**
   * `scaled_unit` specializations have priority in this operation. This means that the library framework
   * prevents passing it as an element to the `derived_unit`. In such case only the reference unit is passed
   * to the derived unit and the magnitude remains outside forming another scaled unit as a result of the operation.
   */
  template<Unit Lhs, Unit Rhs>
  [[nodiscard]] friend MP_UNITS_CONSTEVAL Unit auto operator/(Lhs lhs, Rhs rhs)
  {
    return expr_divide<derived_unit, one>(lhs, rhs);
  }

  template<Unit Lhs, Unit Rhs>
  [[nodiscard]] friend consteval bool operator==(Lhs, Rhs)
  {
    return std::is_same_v<Lhs, Rhs>;
  }

  template<Unit Lhs, Unit Rhs>
  [[nodiscard]] friend consteval bool equivalent(Lhs lhs, Rhs rhs)
  {
    if constexpr (std::is_same_v<Lhs, Rhs>)
      return true;
    else {
      const auto lhs_canonical = mp_units::get_canonical_unit(lhs);
      const auto rhs_canonical = mp_units::get_canonical_unit(rhs);
      return lhs_canonical.mag == rhs_canonical.mag && lhs_canonical.reference_unit == rhs_canonical.reference_unit;
    }
  }

  template<Unit Self>
  [[nodiscard]] friend consteval Self get_unit(Self self)
  {
    return self;
  }

  template<Unit U>
  [[nodiscard]] friend consteval QuantitySpec auto get_quantity_spec(U)
  {
    return kind_of<detail::get_associated_quantity(U{})>;
  }

  template<Unit U>
    requires std::is_final_v<decltype(detail::get_associated_quantity(U{}))>
  [[deprecated(
    "2.6.0: `quantity_spec` definition of `U` should not be marked "
    "`final`")]] [[nodiscard]] friend consteval QuantitySpec auto get_quantity_spec(U)
  {
    return kind_of<detail::get_associated_quantity(U{})>;
  }

  // Clang <= 18 does not support default template arguments in friend function templates.
  // The two overloads below are a workaround. The intended library API (and the one that
  // should be proposed for ISO standardization) is a single function template:
  //
  //   template<std::intmax_t Num, std::intmax_t Den = 1, Unit U>
  //     requires(Den != 0)
  //   [[nodiscard]] friend consteval Unit auto pow(U u)
  //   {
  //     return detail::expr_pow<Num, Den, derived_unit, struct one>(u);
  //   }
  template<std::intmax_t Num, std::intmax_t Den, Unit U>
    requires(Den != 0)
  [[nodiscard]] friend consteval Unit auto pow(U u)
  {
    return detail::expr_pow<Num, Den, derived_unit, struct one>(u);
  }

  template<std::intmax_t Num, Unit U>
  [[nodiscard]] friend consteval Unit auto pow(U u)
  {
    return detail::expr_pow<Num, 1, derived_unit, struct one>(u);
  }

  [[nodiscard]] friend consteval Unit auto sqrt(Unit auto u) { return pow<1, 2>(u); }
  [[nodiscard]] friend consteval Unit auto cbrt(Unit auto u) { return pow<1, 3>(u); }
  [[nodiscard]] friend consteval Unit auto square(Unit auto u) { return pow<2>(u); }
  [[nodiscard]] friend consteval Unit auto cubic(Unit auto u) { return pow<3>(u); }
};

template<Unit U, bool = requires { U::_point_origin_; }>
struct propagate_point_origin {};

template<Unit U>
struct propagate_point_origin<U, true> {
  static constexpr auto _point_origin_ = U::_point_origin_;
};

template<UnitMagnitude auto M, Unit U>
struct scaled_unit_impl : detail::unit_interface, detail::propagate_point_origin<U> {
  using _base_type_ = scaled_unit_impl;  // exposition only
  static constexpr UnitMagnitude auto _mag_ = M;
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
template<UnitMagnitude auto M, Unit U>
  requires(M != mag<1>)  // `mag<1>` is the empty `unit_magnitude<>`, so a single check covers both spellings
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
 * inline constexpr struct second : named_unit<"s", kind_of<time>> {} second;
 * inline constexpr struct metre : named_unit<"m", kind_of<length> {} metre;
 * inline constexpr struct hertz : named_unit<"Hz", inverse(second), kind_of<frequency>> {} hertz;
 * inline constexpr struct newton : named_unit<"N", kilogram * metre / square(second)> {} newton;
 * inline constexpr struct degree_Celsius : named_unit<{u8"℃", "`C"}, kelvin, zeroth_degree_Celsius> {}
 * degree_Celsius; inline constexpr struct minute : named_unit<"min", mag<60> * second> {} minute;
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
  requires(!Symbol.empty()) &&
          (detail::BaseDimension<MP_UNITS_NONCONST_TYPE(get_dimension(QS))> || detail::remove_kind(QS) == dimensionless)
struct named_unit<Symbol, QS> : detail::unit_interface {
  using _base_type_ = named_unit;           // exposition only
  static constexpr auto _symbol_ = Symbol;  ///< Unique base unit identifier
  static constexpr auto _quantity_spec_ = QS;
};

template<symbol_text Symbol, detail::QuantityKindSpec auto QS, PointOrigin auto PO>
  requires(!Symbol.empty()) &&
          (detail::BaseDimension<MP_UNITS_NONCONST_TYPE(get_dimension(QS))> || detail::remove_kind(QS) == dimensionless)
struct named_unit<Symbol, QS, PO> : detail::unit_interface {
  using _base_type_ = named_unit;           // exposition only
  static constexpr auto _symbol_ = Symbol;  ///< Unique base unit identifier
  static constexpr auto _quantity_spec_ = QS;
  static constexpr auto _point_origin_ = PO;
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
  requires(!Symbol.empty()) && detail::is_positive_canonical_unit_mag<U>
struct named_unit<Symbol, U> : decltype(U)::_base_type_ {
  using _base_type_ = named_unit;           // exposition only
  static constexpr auto _symbol_ = Symbol;  ///< Unique unit identifier
};

template<symbol_text Symbol, Unit auto U, PointOrigin auto PO>
  requires(!Symbol.empty()) && detail::is_positive_canonical_unit_mag<U>
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
template<symbol_text Symbol, Unit auto U, detail::QuantityKindSpec auto QS>
  requires(!Symbol.empty()) && (get_dimension(QS) == get_dimension(detail::get_associated_quantity(U))) &&
          detail::is_positive_canonical_unit_mag<U>
struct named_unit<Symbol, U, QS> : decltype(U)::_base_type_ {
  using _base_type_ = named_unit;           // exposition only
  static constexpr auto _symbol_ = Symbol;  ///< Unique unit identifier
  static constexpr auto _quantity_spec_ = QS;
};

template<symbol_text Symbol, Unit auto U, detail::QuantityKindSpec auto QS, PointOrigin auto PO>
  requires(!Symbol.empty()) && (get_dimension(QS) == get_dimension(detail::get_associated_quantity(U))) &&
          detail::is_positive_canonical_unit_mag<U>
struct named_unit<Symbol, U, QS, PO> : decltype(U)::_base_type_ {
  using _base_type_ = named_unit;           // exposition only
  static constexpr auto _symbol_ = Symbol;  ///< Unique unit identifier
  static constexpr auto _quantity_spec_ = QS;
  static constexpr auto _point_origin_ = PO;
};

/**
 * @brief Relative standard uncertainty of a measured constant
 *
 * Wraps the relative standard uncertainty (GUM/CODATA `u_r`) of a measured constant as an exact
 * symbolic magnitude so that no representation type is imposed at the definition point. It is
 * metadata only - it never participates in unit equality, conversion factors, or symbolic
 * simplification.
 *
 * A measured constant declares exactly one of `relative_standard_uncertainty` and
 * `standard_uncertainty` - whichever form its source publishes. The published pair is mutually
 * rounded, so carrying both would embed a contradiction. This form is invariant under a change
 * of unit, so it is the right choice for a constant whose defining unit has no published
 * absolute uncertainty.
 *
 * For example:
 *
 * @code{.cpp}
 * inline constexpr struct newtonian_constant_of_gravitation final :
 *     named_constant<"G", mag_ratio<667'430, 100'000> * mag_power<10, -11> * m3 / kg / s2,
 *                    relative_standard_uncertainty{mag_ratio<22, 10> * mag_power<10, -5>}> {}
 * newtonian_constant_of_gravitation;
 * @endcode
 *
 * @tparam M an exact magnitude representing the relative standard uncertainty
 */
MP_UNITS_EXPORT template<UnitMagnitude M>
struct relative_standard_uncertainty {
  M magnitude;
};

#if MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17

template<UnitMagnitude M>
relative_standard_uncertainty(M) -> relative_standard_uncertainty<M>;

#endif

/**
 * @brief Absolute standard uncertainty of a measured constant
 *
 * Wraps the standard uncertainty (GUM/CODATA `u`) of a measured constant as an exact symbolic
 * expression of the same shape as the constant's value: a magnitude times a unit of the same
 * dimension. Spelling the unit out makes the expression self-contained - the relative form
 * derives as an exact ratio of two canonical magnitudes, with the units cancelling exactly.
 * Like `relative_standard_uncertainty`, it is metadata only.
 *
 * A measured constant declares exactly one of the two wrappers - whichever form its source
 * publishes. Metrology tables (e.g., CODATA) tabulate the absolute uncertainty next to the
 * value, both rounded consistently, so a definition transcribing a table row should use this
 * form; deriving it from a published `u_r` instead can be off by several percent in the last
 * digit.
 *
 * For example:
 *
 * @code{.cpp}
 * inline constexpr struct newtonian_constant_of_gravitation final :
 *     named_constant<"G", mag_ratio<667'430, 100'000> * mag_power<10, -11> * m3 / kg / s2,
 *                    standard_uncertainty{mag_ratio<15, 10> * mag_power<10, -15> * m3 / kg / s2}> {}
 * newtonian_constant_of_gravitation;
 * @endcode
 *
 * @tparam U a unit expression representing the standard uncertainty in the constant's dimension
 */
MP_UNITS_EXPORT template<Unit U>
struct standard_uncertainty {
  U unit;
};

#if MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17

template<Unit U>
standard_uncertainty(U) -> standard_uncertainty<U>;

#endif

/**
 * @brief Named constant definition
 *
 * It is very similar to `named_unit` but:
 * - allows negative exponents in the unit definition (TODO )
 * - does not allow the constant to be associated with a `quantity_spec`
 * - does not allow the constant to be prefixed
 *
 * @tparam Symbol a short text representation of the constant
 * @tparam Unit a unit that we use to define a constant
 */
template<symbol_text Symbol, Unit auto U>
  requires(!Symbol.empty())
struct named_constant<Symbol, U> : decltype(U)::_base_type_ {
  using _base_type_ = named_constant;       // exposition only
  static constexpr auto _symbol_ = Symbol;  ///< Unique constant identifier
};

/**
 * @brief Specialization for a measured constant declaring its relative standard uncertainty
 *
 * The same as the above but additionally stores the relative standard uncertainty of a constant
 * that is measured rather than exact by definition. The value transcribes the entry published in
 * the source metrology table (e.g., CODATA).
 *
 * @tparam Symbol a short text representation of the constant
 * @tparam Unit a unit that we use to define a constant
 * @tparam RSU the relative standard uncertainty of the measured value
 */
// The magnitude is deduced from the wrapper's type instead of being read from the `RSU` value
// (`decltype(RSU)` + member access in a constraint misfires on gcc-12), which also spares a
// detection trait for the wrapper.
template<symbol_text Symbol, Unit auto U, typename M, relative_standard_uncertainty<M> RSU>
  requires(!Symbol.empty()) && detail::magnitude_is_positive<M{}>
struct named_constant<Symbol, U, RSU> : decltype(U)::_base_type_ {
  using _base_type_ = named_constant;       // exposition only
  static constexpr auto _symbol_ = Symbol;  ///< Unique constant identifier
  static constexpr auto _relative_standard_uncertainty_ = M{};
};

/**
 * @brief Specialization for a measured constant declaring its absolute standard uncertainty
 *
 * The same as the above but stores the standard uncertainty in the form the source metrology
 * table publishes it: an absolute value in a unit of the constant's own dimension. The relative
 * form derives on demand as an exact ratio of canonical magnitudes.
 *
 * @tparam Symbol a short text representation of the constant
 * @tparam Unit a unit that we use to define a constant
 * @tparam SU the standard uncertainty of the measured value
 */
template<symbol_text Symbol, Unit auto U, typename SUU, standard_uncertainty<SUU> SU>
  requires(!Symbol.empty()) && detail::is_positive_canonical_unit_mag<SUU{}> && detail::ConvertibleUnits<SUU{}, U>
struct named_constant<Symbol, U, SU> : decltype(U)::_base_type_ {
  using _base_type_ = named_constant;       // exposition only
  static constexpr auto _symbol_ = Symbol;  ///< Unique constant identifier
  static constexpr auto _standard_uncertainty_ = SUU{};
};

/**
 * @brief Returns the relative standard uncertainty of a measured constant
 *
 * For a constant declared with `standard_uncertainty`, the result is derived as the exact ratio
 * of the uncertainty's canonical magnitude to the constant's one (`u_r = u(x)/|x|`, so the
 * result never carries a sign even for a negative-valued constant).
 *
 * There is no valid result for constants that are exact by definition. Asking for one would
 * conflate "exact by definition" with "measured infinitely precisely", so it is a compile-time
 * error rather than a zero magnitude.
 */
MP_UNITS_EXPORT template<MeasuredConstant U>
[[nodiscard]] consteval UnitMagnitude auto get_relative_standard_uncertainty(U u)
{
  if constexpr (requires { requires UnitMagnitude<MP_UNITS_NONCONST_TYPE(U::_relative_standard_uncertainty_)>; })
    return U::_relative_standard_uncertainty_;
  else
    // unqualified on purpose: `abs_magnitude` is a hidden friend of the magnitude, found via ADL
    return abs_magnitude(get_canonical_unit(U::_standard_uncertainty_).mag / get_canonical_unit(u).mag);
}

/**
 * @brief Returns the standard uncertainty of a measured constant
 *
 * The result is a unit expression of the constant's own dimension (constants are units in this
 * library, and so are their uncertainties). For a constant declared with
 * `relative_standard_uncertainty`, the result is derived by scaling the constant itself
 * (`u(x) = u_r * |x|`, so the result is positive even for a negative-valued constant).
 *
 * As for `get_relative_standard_uncertainty`, there is no valid result for constants that are
 * exact by definition.
 */
MP_UNITS_EXPORT template<MeasuredConstant U>
[[nodiscard]] consteval Unit auto get_standard_uncertainty(U u)
{
  if constexpr (requires { requires Unit<MP_UNITS_NONCONST_TYPE(U::_standard_uncertainty_)>; })
    return U::_standard_uncertainty_;
  else if constexpr (detail::is_positive_canonical_unit_mag<U{}>)
    return U::_relative_standard_uncertainty_ * u;
  else
    return (-U::_relative_standard_uncertainty_) * u;
}


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
MP_UNITS_EXPORT template<symbol_text Symbol, UnitMagnitude auto M, PrefixableUnit auto U>
  requires(!Symbol.empty())
// `PrefixableUnit` guarantees a named unit, so `M * U` always reduces to `scaled_unit<M, U>`; spelling the base
// directly avoids instantiating the `operator*` machinery for every prefix and unit combination (e.g. unit_symbols.h)
struct prefixed_unit : scaled_unit<M, MP_UNITS_REMOVE_CONST(decltype(U))>::_base_type_ {
  using _base_type_ = prefixed_unit;  // exposition only
  static constexpr auto _symbol_ = Symbol + U._symbol_;
};

namespace detail {

template<auto... Args>
constexpr bool is_specialization_of_prefixed_unit<prefixed_unit<Args...>> = true;

template<Unit U1, UnitConvertibleTo<U1{}> U2>
[[nodiscard]] consteval Unit auto get_common_scaled_unit(U1, U2)
{
  constexpr auto canonical_lhs = mp_units::get_canonical_unit(U1{});
  constexpr auto canonical_rhs = mp_units::get_canonical_unit(U2{});
  constexpr auto common_mag = common_magnitude(canonical_lhs.mag, canonical_rhs.mag);
  if constexpr (common_mag == mag<1>)
    return canonical_lhs.reference_unit;
  else
    return scaled_unit<common_mag, MP_UNITS_NONCONST_TYPE(canonical_lhs.reference_unit)>{};
}

[[nodiscard]] consteval Unit auto get_common_scaled_unit(Unit auto u1, Unit auto u2, Unit auto u3, Unit auto... rest)
  requires requires { get_common_scaled_unit(get_common_scaled_unit(u1, u2), u3, rest...); }
{
  return get_common_scaled_unit(get_common_scaled_unit(u1, u2), u3, rest...);
}

template<Unit U1, Unit U2, Unit... Rest>
constexpr auto common_unit_scaled_result = get_common_scaled_unit(U1{}, U2{}, Rest{}...);

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
struct common_unit final : decltype(detail::common_unit_scaled_result<U1, U2, Rest...>)::_base_type_ {
  using _base_type_ = common_unit;  // exposition only
  static constexpr auto _common_unit_ = detail::common_unit_scaled_result<U1, U2, Rest...>;
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
template<detail::SymbolicConstant... Expr>
struct derived_unit final : detail::derived_unit_impl<Expr...> {};

/**
 * @brief Unit one
 *
 * Unit of a dimensionless quantity.
 */
// clang-format off
MP_UNITS_EXPORT inline constexpr struct one final : detail::derived_unit_impl<> {} one;
// clang-format on

}  // namespace mp_units
