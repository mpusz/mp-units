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
#include <mp-units/bits/hacks.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/bits/quantity_spec_hierarchy.h>
#include <mp-units/bits/ratio.h>
#include <mp-units/compat_macros.h>
#include <mp-units/ext/algorithm.h>
#include <mp-units/ext/type_name.h>
#include <mp-units/ext/type_traits.h>
#include <mp-units/framework/dimension.h>
#include <mp-units/framework/quantity_concepts.h>
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/reference_concepts.h>
#include <mp-units/framework/representation_concepts.h>
#include <mp-units/framework/symbolic_expression.h>
#include <mp-units/framework/unit_concepts.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#include <cstdint>
#include <optional>
#include <tuple>
#include <type_traits>
#endif
#endif

namespace mp_units {

MP_UNITS_EXPORT struct dimensionless;

MP_UNITS_EXPORT
#if MP_UNITS_API_NO_CRTP
template<auto...>
#else
template<typename, auto...>
#endif
struct quantity_spec;

template<detail::SymbolicConstant... Expr>
struct derived_quantity_spec;

MP_UNITS_EXPORT template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval bool explicitly_convertible(From from, To to);

namespace detail {

#if MP_UNITS_API_NO_CRTP
template<auto... Args>
void to_base_specialization_of_quantity_spec(const volatile quantity_spec<Args...>*);
#else
template<typename T, auto... Args>
void to_base_specialization_of_quantity_spec(const volatile quantity_spec<T, Args...>*);
#endif

template<typename T>
constexpr bool is_derived_from_specialization_of_quantity_spec =
  requires(T* type) { detail::to_base_specialization_of_quantity_spec(type); };

/**
 * @brief Concept matching all named quantity specification types
 *
 * Satisfied by all types that derive from `quantity_spec`.
 */
template<typename T>
concept NamedQuantitySpec =
  QuantitySpec<T> && is_derived_from_specialization_of_quantity_spec<T> && (!QuantityKindSpec<T>);

/**
 * @brief Concept matching all derived quantity specification types
 *
 * Satisfied by all `derived_quantity_spec` specializations.
 *
 * @note Deriving a strong type from it is considered a logic error and thus is
 * explicitly not supported here.
 */
template<typename T>
concept DerivedQuantitySpec =
  QuantitySpec<T> &&
  (is_specialization_of<T, derived_quantity_spec> ||
   (QuantityKindSpec<T> && is_specialization_of<MP_UNITS_NONCONST_TYPE(T::_quantity_spec_), derived_quantity_spec>));


template<QuantitySpec QS, detail::WeakUnitOf<QS{}> U>
[[nodiscard]] consteval Reference auto make_reference(QS, U u)
{
  if constexpr (requires { requires(mp_units::get_quantity_spec(U{}) == QS{}); })
    return u;
  else
    return reference<QS, U>{};
}

// TODO revise the note in the below comment
/**
 * @brief Returns the most restrictive character from the list
 *
 * @note `vector * vector` returns vector (not tensor)
 */
template<std::same_as<quantity_character>... Ts>
[[nodiscard]] consteval quantity_character common_quantity_character(Ts... args)
{
  return detail::max({args...});
}

template<typename... Qs1, typename... Qs2>
[[nodiscard]] consteval quantity_character derived_quantity_character(const type_list<Qs1...>&,
                                                                      const type_list<Qs2...>&)
{
  constexpr quantity_character num =
    detail::common_quantity_character(quantity_character::scalar, expr_type<Qs1>::character...);
  constexpr quantity_character den =
    detail::common_quantity_character(quantity_character::scalar, expr_type<Qs2>::character...);
  if constexpr (num == den)
    return quantity_character::scalar;
  else
    return detail::common_quantity_character(num, den);
}

/**
 * @brief Initializes quantity character
 *
 * If a quantity character value is present in template parameters, this value will be used.
 * Otherwise, an inherited/derived value provided through the function argument is returned.
 */
template<auto... Args>
[[nodiscard]] consteval quantity_character quantity_character_init(quantity_character ch)
{
  if constexpr (mp_units::contains<quantity_character, Args...>())
    return mp_units::get<quantity_character, Args...>();
  else
    return ch;
}

template<QuantitySpec auto... From, QuantitySpec Q>
[[nodiscard]] consteval QuantitySpec auto clone_kind_of(Q q);

template<QuantitySpec Q>
[[nodiscard]] consteval auto remove_kind(Q q);

template<QuantitySpec Q>
[[nodiscard]] consteval QuantitySpec auto get_kind_tree_root(Q);

template<NamedQuantitySpec Lhs, NamedQuantitySpec Rhs, NamedQuantitySpec LhsRoot, NamedQuantitySpec RhsRoot,
         NamedQuantitySpec LhsKindRoot, NamedQuantitySpec RhsKindRoot>
struct quantity_spec_less_impl :
    std::bool_constant<(detail::type_name<LhsRoot>() < detail::type_name<RhsRoot>()) ||
                       (detail::type_name<LhsRoot>() == detail::type_name<RhsRoot>() &&
                        detail::type_name<LhsKindRoot>() < detail::type_name<RhsKindRoot>()) ||
                       (detail::type_name<LhsRoot>() == detail::type_name<RhsRoot>() &&
                        detail::type_name<LhsKindRoot>() == detail::type_name<RhsKindRoot>() &&
                        detail::type_name<Lhs>() < detail::type_name<Rhs>())> {};

template<NamedQuantitySpec Lhs, NamedQuantitySpec Rhs>
struct quantity_spec_less :
    quantity_spec_less_impl<Lhs, Rhs, decltype(detail::get_hierarchy_root(Lhs{})),
                            decltype(detail::get_hierarchy_root(Rhs{})), decltype(detail::get_kind_tree_root(Lhs{})),
                            decltype(detail::get_kind_tree_root(Rhs{}))> {};

template<typename T1, typename T2>
using type_list_of_quantity_spec_less = expr_less<T1, T2, quantity_spec_less>;

struct quantity_spec_interface_base {
  template<QuantitySpec Lhs, QuantitySpec Rhs>
  [[nodiscard]] friend consteval QuantitySpec auto operator*(Lhs lhs, Rhs rhs)
  {
    return detail::clone_kind_of<Lhs{}, Rhs{}>(
      detail::expr_multiply<derived_quantity_spec, struct dimensionless, type_list_of_quantity_spec_less>(
        detail::remove_kind(lhs), detail::remove_kind(rhs)));
  }

  template<QuantitySpec Lhs, QuantitySpec Rhs>
  [[nodiscard]] friend consteval QuantitySpec auto operator/(Lhs lhs, Rhs rhs)
  {
    return detail::clone_kind_of<Lhs{}, Rhs{}>(
      detail::expr_divide<derived_quantity_spec, struct dimensionless, type_list_of_quantity_spec_less>(
        detail::remove_kind(lhs), detail::remove_kind(rhs)));
  }

  template<QuantitySpec Lhs, QuantitySpec Rhs>
  [[nodiscard]] friend consteval bool operator==(Lhs, Rhs)
  {
    return is_same_v<Lhs, Rhs>;
  }
};

#if !MP_UNITS_API_NO_CRTP
template<typename Self>
#endif
struct quantity_spec_interface : quantity_spec_interface_base {
#if MP_UNITS_API_NO_CRTP
  template<typename Self, UnitOf<Self{}> U>
  [[nodiscard]] consteval Reference auto operator[](this Self self, U u)
  {
    return detail::make_reference(self, u);
  }

  template<typename Self, typename FwdQ, Quantity Q = std::remove_cvref_t<FwdQ>>
    requires(mp_units::explicitly_convertible(Q::quantity_spec, Self{}))
  [[nodiscard]] constexpr Quantity auto operator()(this Self self, FwdQ&& q)
  {
    return quantity{std::forward<FwdQ>(q).numerical_value_is_an_implementation_detail_,
                    detail::make_reference(self, Q::unit)};
  }
#else
  template<typename Self_ = Self, UnitOf<Self_{}> U>
  [[nodiscard]] MP_UNITS_CONSTEVAL Reference auto operator[](U u) const
  {
    return detail::make_reference(Self{}, u);
  }

  template<typename FwdQ, Quantity Q = std::remove_cvref_t<FwdQ>, typename Self_ = Self>
    requires(mp_units::explicitly_convertible(Q::quantity_spec, Self_{}))
  [[nodiscard]] constexpr Quantity auto operator()(FwdQ&& q) const
  {
    return quantity{std::forward<FwdQ>(q).numerical_value_is_an_implementation_detail_,
                    detail::make_reference(Self{}, Q::unit)};
  }
#endif
};

template<typename T>
concept QSProperty = (!QuantitySpec<T>);

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

inline constexpr struct is_kind {
} is_kind;

/**
 * @brief Quantity Specification
 *
 * This type specifies all the properties of a quantity and allow modeling most of the quantities in the ISO 80000.
 * It serves to define base and derived quantities as well as quantity kinds. Each quantity specification
 * provides an information on how this quantity relates to other quantities, specifies its dimension, and character.
 *
 * Quantity character can be derived from other quantities or explicitly overriden through a template parameter.
 *
 * Binding a proper unit to a quantity specification via an indexing operator (`operator[]`) results
 * in a quantity reference.
 *
 * Call operator may be used to change the type of a provided quantity.
 *
 * Two quantity specifications are deemed equal when they are of the same type. With that, both strong
 * types `speed` and `velocity` are considered not equal to `derived_dimension<length, per<time>>` or
 * to each other.
 */
#if MP_UNITS_API_NO_CRTP
template<auto...>
#else
template<typename, auto...>
#endif
struct quantity_spec;

MP_UNITS_EXPORT_END

/**
 * @brief Specialization defining a base quantity
 *
 * Base quantity is a quantity in a conventionally chosen subset of a given system of quantities, where no quantity
 * in the subset can be expressed in terms of the other quantities within that subset. They are referred to as
 * being mutually independent since a base quantity cannot be expressed as a product of powers of the other base
 * quantities.
 *
 * This quantity serves as a root/kind for a new hierarchy of quantities of the same kind.
 *
 * Base quantities have scalar character by default.
 *
 * User should derive a strong type from this class template rather than use it directly in the source code.
 * For example:
 *
 * @code{.cpp}
 * inline constexpr struct dim_length final : base_dimension<"L"> {} dim_length;
 * inline constexpr struct dim_mass final : base_dimension<"M"> {} dim_mass;
 * inline constexpr struct dim_time final : base_dimension<"T"> {} dim_time;
 *
 * inline constexpr struct length final : quantity_spec<dim_length> {} length;
 * inline constexpr struct mass final : quantity_spec<dim_mass> {} mass;
 * inline constexpr struct time final : quantity_spec<dim_time> {} time;
 * @endcode
 *
 * @note A common convention in this library is to assign the same name for a type and an object of this type.
 *       Besides defining them user never works with the types in the source code. All operations
 *       are done on the objects. Contrarily, the types are the only one visible in the compilation
 *       errors. Having them of the same names improves user experience and somehow blurs those separate domains.
 *
 * @tparam BaseDimension base dimension for which a base quantity is being defined
 * @tparam Args optionally a value of a `quantity_character` in case the base quantity should not be scalar
 */
#if MP_UNITS_API_NO_CRTP
template<detail::BaseDimension auto Dim, detail::QSProperty auto... Args>
struct quantity_spec<Dim, Args...> : detail::quantity_spec_interface {
#else
template<typename Self, detail::BaseDimension auto Dim, detail::QSProperty auto... Args>
struct quantity_spec<Self, Dim, Args...> : detail::quantity_spec_interface<Self> {
#endif
  using _base_type_ = quantity_spec;
  static constexpr detail::BaseDimension auto dimension = Dim;
  static constexpr quantity_character character = detail::quantity_character_init<Args...>(quantity_character::scalar);
};

/**
 * @brief Specialization defining a named quantity being the result of a quantity calculus
 *
 * Derived quantity is a quantity, in a system of quantities, defined in terms of other quantities
 * of that system.
 *
 * This quantity serves as a root/kind for a new hierarchy of quantities of the same kind.
 *
 * Such quantities by default derive the character from the derived quantity definition.
 *
 * User should derive a strong type from this class template rather than use it directly in the source code.
 * For example:
 *
 * @code{.cpp}
 * inline constexpr struct area final : quantity_spec<pow<2>(length)> {} area;
 * inline constexpr struct volume final : quantity_spec<pow<3>(length)> {} volume;
 * inline constexpr struct velocity final : quantity_spec<displacement / duration> {} velocity;
 * inline constexpr struct speed final : quantity_spec<length / time> {} speed;
 * inline constexpr struct force final : quantity_spec<mass * acceleration, quantity_character::vector> {} force;
 * inline constexpr struct power final : quantity_spec<force * velocity, quantity_character::scalar> {} power;
 * @endcode
 *
 * @note A common convention in this library is to assign the same name for a type and an object of this type.
 *       Besides defining them user never works with the types in the source code. All operations
 *       are done on the objects. Contrarily, the types are the only one visible in the compilation
 *       errors. Having them of the same names improves user experience and somehow blurs those separate domains.
 *
 * @tparam Eq quantity equation specification of a derived quantity
 * @tparam Args optionally a value of a `quantity_character` in case the base quantity should not be scalar
 */
#if MP_UNITS_API_NO_CRTP
template<detail::DerivedQuantitySpec auto Eq, detail::QSProperty auto... Args>
struct quantity_spec<Eq, Args...> : detail::quantity_spec_interface {
#else
template<typename Self, detail::DerivedQuantitySpec auto Eq, detail::QSProperty auto... Args>
struct quantity_spec<Self, Eq, Args...> : detail::quantity_spec_interface<Self> {
#endif
  using _base_type_ = quantity_spec;
  static constexpr auto _equation_ = Eq;
  static constexpr Dimension auto dimension = Eq.dimension;
  static constexpr quantity_character character = detail::quantity_character_init<Args...>(Eq.character);
};

namespace detail {

template<QuantitySpec auto Q, bool = requires { Q._equation_; }>
struct propagate_equation {};

template<QuantitySpec auto Q>
struct propagate_equation<Q, true> {
  static constexpr auto _equation_ = Q._equation_;
};

}  // namespace detail

/**
 * @brief Specialization defining a leaf quantity in the hierarchy
 *
 * Quantities of the same kind form a hierarchy. This specialization adds new leaf to such a tree which
 * can later be used as a parent by other quantities.
 *
 * The character of those quantities by default is derived from the parent quantity.
 *
 * User should derive a strong type from this class template rather than use it directly in the source code.
 * For example:
 *
 * @code{.cpp}
 * inline constexpr struct width final : quantity_spec<length> {} width;
 * inline constexpr struct height final : quantity_spec<length> {} height;
 * inline constexpr struct diameter final : quantity_spec<width> {} diameter;
 * inline constexpr struct displacement final : quantity_spec<length, quantity_character::vector> {} displacement;
 * @endcode
 *
 * @note A common convention in this library is to assign the same name for a type and an object of this type.
 *       Besides defining them user never works with the types in the source code. All operations
 *       are done on the objects. Contrarily, the types are the only one visible in the compilation
 *       errors. Having them of the same names improves user experience and somehow blurs those separate domains.
 *
 * @tparam Q quantity specification of a parent quantity
 * @tparam Args optionally a value of a `quantity_character` in case the base quantity should not be scalar
 *              or `is_kind` in case the quantity starts a new hierarchy tree of a kind
 */
#if MP_UNITS_API_NO_CRTP
template<detail::NamedQuantitySpec auto QS, detail::QSProperty auto... Args>
struct quantity_spec<QS, Args...> : detail::propagate_equation<QS>, detail::quantity_spec_interface {
#else
template<typename Self, detail::NamedQuantitySpec auto QS, detail::QSProperty auto... Args>
struct quantity_spec<Self, QS, Args...> : detail::propagate_equation<QS>, detail::quantity_spec_interface<Self> {
#endif
  using _base_type_ = quantity_spec;
  static constexpr auto _parent_ = QS;
  static constexpr Dimension auto dimension = _parent_.dimension;
  static constexpr quantity_character character = detail::quantity_character_init<Args...>(QS.character);

#if !MP_UNITS_API_NO_CRTP
  template<typename Self_ = Self, UnitOf<Self_{}> U>
  [[nodiscard]] MP_UNITS_CONSTEVAL Reference auto operator[](U u) const
  {
    return detail::make_reference(Self{}, u);
  }

  template<typename FwdQ, Quantity Q = std::remove_cvref_t<FwdQ>, typename Self_ = Self>
    requires detail::QuantitySpecExplicitlyConvertibleTo<Q::quantity_spec, Self_{}>
  [[nodiscard]] constexpr Quantity auto operator()(FwdQ&& q) const
  {
    return quantity{std::forward<FwdQ>(q).numerical_value_is_an_implementation_detail_,
                    detail::make_reference(Self{}, Q::unit)};
  }
#endif
};

// clang-format off
/**
 * @brief Specialization defining a leaf derived quantity in the hierarchy and refining paren't equation
 *
 * Quantities of the same kind form a hierarchy. This specialization adds new leaf to such a tree which
 * can later be used as a parent by other quantities. Additionally, this defintion adds additional
 * constraints on the derived quantity's equation.
 *
 * The character of those quantities by default is derived from the parent quantity.
 *
 * User should derive a strong type from this class template rather than use it directly in the source code.
 * For example:
 *
 * @code{.cpp}
 * inline constexpr struct angular_measure final : quantity_spec<dimensionless, arc_length / radius, is_kind> {} angular_measure;
 * inline constexpr struct velocity final : quantity_spec<speed, displacement / duration> {} velocity;
 * inline constexpr struct weight final : quantity_spec<force, mass * acceleration_of_free_fall> {} weight;
 * inline constexpr struct kinetic_energy final : quantity_spec<mechanical_energy, mass * pow<2>(speed)> {} kinetic_energy;
 * @endcode
 *
 * @note A common convention in this library is to assign the same name for a type and an object of this type.
 *       Besides defining them user never works with the types in the source code. All operations
 *       are done on the objects. Contrarily, the types are the only one visible in the compilation
 *       errors. Having them of the same names improves user experience and somehow blurs those separate domains.
 *
 * @tparam Q quantity specification of a parent quantity
 * @tparam Args optionally a value of a `quantity_character` in case the base quantity should not be scalar
 *              or `is_kind` in case the quantity starts a new hierarchy tree of a kind
 */
// clang-format on
#if MP_UNITS_API_NO_CRTP
template<detail::NamedQuantitySpec auto QS, detail::DerivedQuantitySpec auto Eq, detail::QSProperty auto... Args>
  requires(detail::QuantitySpecExplicitlyConvertibleTo<Eq, QS>)
struct quantity_spec<QS, Eq, Args...> : detail::quantity_spec_interface {
#else
template<typename Self, detail::NamedQuantitySpec auto QS, detail::DerivedQuantitySpec auto Eq,
         detail::QSProperty auto... Args>
  requires(detail::QuantitySpecExplicitlyConvertibleTo<Eq, QS>)
struct quantity_spec<Self, QS, Eq, Args...> : detail::quantity_spec_interface<Self> {
#endif
  using _base_type_ = quantity_spec;
  static constexpr auto _parent_ = QS;
  static constexpr auto _equation_ = Eq;
  static constexpr Dimension auto dimension = _parent_.dimension;
  static constexpr quantity_character character = detail::quantity_character_init<Args...>(Eq.character);
};

namespace detail {

template<NamedQuantitySpec Q>
  requires requires { Q::dimension; }
using to_dimension = MP_UNITS_NONCONST_TYPE(Q::dimension);

template<typename... Expr>
struct derived_quantity_spec_impl :
#if MP_UNITS_API_NO_CRTP
    quantity_spec_interface,
#else
    quantity_spec_interface<derived_quantity_spec<Expr...>>,
#endif
    expr_fractions<dimensionless, Expr...> {
  using _base_type_ = derived_quantity_spec_impl;
  using _base_ = expr_fractions<dimensionless, Expr...>;

  static constexpr Dimension auto dimension =
    detail::expr_map<to_dimension, derived_dimension, struct dimension_one>(_base_{});
  static constexpr quantity_character character =
    detail::derived_quantity_character(typename _base_::_num_{}, typename _base_::_den_{});
};

}  // namespace detail

/**
 * @brief A specification of a derived quantity
 *
 * Derived quantity is a quantity, in a system of quantities, defined in terms of other quantities of that system.
 * Its dimension is an expression of the dependence of a quantity on the base quantities of a system of
 * quantities as a product of powers of factors corresponding to the base quantities, omitting any numerical factors.
 *
 * Instead of using a raw list of exponents this library decided to use symbolic expression syntax to make types
 * more digestable for the user both for quantity specification and its dimension. The positive exponents are ordered
 * first and all negative exponents are put as a list into the `per<...>` class template. If a power of exponent
 * is different than `1` the quantity type is enclosed in `power<Q, Num, Den>` class template. Otherwise, it is
 * just put directly in the list without any wrapper. In case all of the exponents are negative than the
 * `dimensionless`/`dimension_one` is put in the front to increase the readability.
 *
 * The character of those quantities is derived from ingredients or overriden with a template parameter.
 *
 * For example:
 *
 * @code{.cpp}
 * auto frequency = inverse(period_duration);
 * auto area = pow<2>(length);
 * auto speed = distance / duration;
 * auto velocity = displacement / duration;
 * auto acceleration = velocity / duration;
 * @endcode
 *
 * - the type of `frequency` is `derived_quantity_spec<dimensionless, per<period_duration>>`
 * - the dimension type of `frequency` is `derived_dimension<dimension_one, per<dim_time>>`
 * - the type of `area` is `derived_quantity_spec<power<length, 2>>`
 * - the dimension type of `area` is `derived_dimension<power<dim_length, 2>>`
 * - the type of `speed` is `derived_quantity_spec<distance, per<duration>>`
 * - the dimension type of `speed` is `derived_dimension<dim_length, per<dim_time>>`
 * - the type of `velocity` is `derived_quantity_spec<displacement, per<duration>>`
 * - the dimension type of `velocity` is `derived_dimension<dim_length, per<dim_time>>`
 * - the type of `acceleration` is `derived_quantity_spec<velocity, per<duration>>`
 * - the dimension type of `acceleration` is `derived_dimension<dim_length, per<power<dim_time, 2>>>`
 *
 * @tparam Expr a parameter pack consisting tokens allowed in the quantity specification
 *         (named quantity specification, `dimensionless`, `power<Q, Num, Den>`, `per<...>`)
 *
 * @note User should not instantiate this type! It is not exported from the C++ module. The library will
 *       instantiate this type automatically based on the dimensional arithmetic equation provided by the user.
 */
template<detail::SymbolicConstant... Expr>
struct derived_quantity_spec final : detail::derived_quantity_spec_impl<Expr...> {};

/**
 * @brief Quantity of dimension one
 *
 * Quantity of dimension one also commonly named as "dimensionless" is a quantity with a dimension
 * for which all the exponents of the factors corresponding to the base dimensions are zero.
 */
MP_UNITS_EXPORT QUANTITY_SPEC(dimensionless, derived_quantity_spec<>{});

/**
 * @brief Quantity kind specifier
 *
 * Specifies that the provided `Q` should be treated as a quantity kind.
 */
namespace detail {

template<QuantitySpec Q>
[[nodiscard]] consteval QuantitySpec auto get_kind_tree_root(Q q);

}  // namespace detail

template<QuantitySpec Q>
  requires(!detail::QuantityKindSpec<Q>) && (detail::get_kind_tree_root(Q{}) == Q{})
#if MP_UNITS_API_NO_CRTP
struct kind_of_<Q> final : Q::_base_type_ {
#else
struct kind_of_<Q> final : quantity_spec<kind_of_<Q>, Q{}>::_base_type_ {
#endif
  using _base_type_ = kind_of_;
  static constexpr auto _quantity_spec_ = Q{};
};

MP_UNITS_EXPORT template<QuantitySpec auto Q>
  requires requires { typename kind_of_<decltype(Q)>; }
constexpr kind_of_<MP_UNITS_REMOVE_CONST(decltype(Q))> kind_of;

namespace detail {

template<QuantitySpec auto... From, QuantitySpec Q>
[[nodiscard]] consteval QuantitySpec auto clone_kind_of(Q q)
{
  if constexpr ((... && QuantityKindSpec<MP_UNITS_REMOVE_CONST(decltype(From))>))
    return kind_of<Q{}>;
  else
    return q;
}

template<QuantitySpec Q>
[[nodiscard]] consteval auto remove_kind(Q q)
{
  if constexpr (QuantityKindSpec<Q>)
    return Q::_quantity_spec_;
  else
    return q;
}

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

[[nodiscard]] consteval QuantitySpec auto inverse(QuantitySpec auto q) { return dimensionless / q; }


/**
 * @brief Computes the value of a quantity specification raised to the `Num/Den` power
 *
 * @tparam Num Exponent numerator
 * @tparam Den Exponent denominator
 * @param q Quantity specification being the base of the operation
 *
 * @return QuantitySpec The result of computation
 */
template<std::intmax_t Num, std::intmax_t Den = 1, QuantitySpec Q>
  requires detail::non_zero<Den>
[[nodiscard]] consteval QuantitySpec auto pow(Q q)
{
  return detail::clone_kind_of<Q{}>(
    detail::expr_pow<Num, Den, derived_quantity_spec, struct dimensionless, detail::type_list_of_quantity_spec_less>(
      detail::remove_kind(q)));
}


/**
 * @brief Computes the square root of a quantity specification
 *
 * @param q Quantity specification being the base of the operation
 *
 * @return QuantitySpec The result of computation
 */
[[nodiscard]] consteval QuantitySpec auto sqrt(QuantitySpec auto q) { return mp_units::pow<1, 2>(q); }


/**
 * @brief Computes the cubic root of a quantity specification
 *
 * @param q Quantity specification being the base of the operation
 *
 * @return QuantitySpec The result of computation
 */
[[nodiscard]] consteval QuantitySpec auto cbrt(QuantitySpec auto q) { return mp_units::pow<1, 3>(q); }

MP_UNITS_EXPORT_END

namespace detail {

/**
 * @brief @c get_complexity specifies how many type explosions can be done on a quantity
 */
template<QuantitySpec Q>
[[nodiscard]] consteval int get_complexity(Q);

template<QuantitySpec Q, int... Ints>
[[nodiscard]] consteval int get_complexity_impl(power<Q, Ints...>);

template<typename Q>
[[nodiscard]] consteval int get_complexity_impl(kind_of_<Q>);

template<QuantitySpec Q>
[[nodiscard]] consteval int get_complexity_impl(Q);

template<typename... Ts>
[[nodiscard]] consteval int get_complexity_impl(type_list<Ts...>)
{
  if constexpr (sizeof...(Ts) == 0)
    return 0;
  else
    return max({detail::get_complexity_impl(Ts{})...});
}

template<QuantitySpec Q, int... Ints>
[[nodiscard]] consteval int get_complexity_impl(power<Q, Ints...>)
{
  return detail::get_complexity(Q{});
}

template<typename Q>
[[nodiscard]] consteval int get_complexity_impl(kind_of_<Q>)
{
  return detail::get_complexity(Q{});
}

template<QuantitySpec Q>
[[nodiscard]] consteval int get_complexity_impl(Q)
{
  if constexpr (DerivedQuantitySpec<Q>)
    return max(detail::get_complexity_impl(typename Q::_num_{}), detail::get_complexity_impl(typename Q::_den_{}));
  else if constexpr (requires { Q::_equation_; } && Q{} != dimensionless)
    return 1 + detail::get_complexity(Q::_equation_);
  else
    return 0;
}

template<QuantitySpec Q>
constexpr auto get_complexity_result = detail::get_complexity_impl(Q{});

/**
 * @brief Get the complexity of a quantity spec
 *
 * Complexity of the quantity spec specifies how many times a given quantity can be exploded
 * to the underlying equation. For the derived quantities, the largest complexity of the
 * ingredients is returned.
 *
 * @tparam Q Quantity specification to be analyzed
 * @return int Complexity of the quantity spec
 */
template<QuantitySpec Q>
[[nodiscard]] consteval int get_complexity(Q)
{
  return get_complexity_result<Q>;
}

template<QuantitySpec Q>
  requires requires { Q::_equation_; }
[[nodiscard]] consteval bool defines_equation(Q)
{
  if constexpr (requires { Q::_parent_._equation_; })
    return Q::_parent_._equation_ != Q::_equation_;
  else
    return true;
}

enum class [[nodiscard]] specs_convertible_result : std::int8_t {
  no,
  cast,
  explicit_conversion_beyond_kind,
  explicit_conversion,
  yes
};

template<QuantitySpec Q>
struct [[nodiscard]] explode_result {
  Q equation;
  specs_convertible_result result;
};

#if MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17

template<QuantitySpec Q>
explode_result(Q, specs_convertible_result) -> explode_result<Q>;

#endif

template<QuantitySpec Q>
  requires requires { Q::_equation_; }
[[nodiscard]] consteval auto explode(Q q)
{
  return explode_result{Q::_equation_, detail::defines_equation(q) ? specs_convertible_result::yes
                                                                   : specs_convertible_result::explicit_conversion};
}

template<QuantitySpec Q, int... Ints>
  requires requires { Q::_equation_; }
[[nodiscard]] consteval auto explode(power<Q, Ints...>)
{
  constexpr ratio exp = power<Q, Ints...>::_exponent_;
  return explode_result{pow<exp.num, exp.den>(Q::_equation_), defines_equation(Q{})
                                                                ? specs_convertible_result::yes
                                                                : specs_convertible_result::explicit_conversion};
}

template<TypeList NumFrom, TypeList DenFrom, TypeList NumTo, TypeList DenTo>
[[nodiscard]] constexpr specs_convertible_result are_ingredients_convertible(NumFrom num_from, DenFrom den_from,
                                                                             NumTo num_to, DenTo den_to);


template<NamedQuantitySpec From, NamedQuantitySpec To>
[[nodiscard]] consteval specs_convertible_result convertible_common_base(From from, To to)
{
  using enum specs_convertible_result;
  if constexpr (is_same_v<From, To>) return yes;
  if constexpr (detail::is_child_of(From{}, To{}))
    return (detail::get_kind_tree_root(from) == detail::get_kind_tree_root(to)) ? yes : explicit_conversion_beyond_kind;
  if constexpr (detail::is_child_of(To{}, From{})) return explicit_conversion;
  if constexpr (detail::get_kind_tree_root(From{}) == detail::get_kind_tree_root(To{})) return cast;
  return no;
}

template<TypeList RestFrom = type_list<>, TypeList RestTo = type_list<>>
struct [[nodiscard]] extract_common_base_result {
  specs_convertible_result result;
  RestFrom rest_from{};
  RestTo rest_to{};
};

#if MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17

extract_common_base_result(specs_convertible_result) -> extract_common_base_result<type_list<>, type_list<>>;

template<TypeList RestFrom, TypeList RestTo>
extract_common_base_result(specs_convertible_result, RestFrom, RestTo) -> extract_common_base_result<RestFrom, RestTo>;

#endif

template<TypeList ProcessedFrom, TypeList RemainingFrom, TypeList ProcessedTo, TypeList RemainingTo>
[[nodiscard]] consteval auto try_extract_common_base(ProcessedFrom, RemainingFrom, ProcessedTo, RemainingTo)
{
  return std::optional<extract_common_base_result<>>(std::nullopt);
}

template<typename... ProcessedFrom, typename From, typename... RemainingFrom, typename... ProcessedTo, typename To,
         typename... RemainingTo>
[[nodiscard]] consteval auto try_extract_common_base(type_list<ProcessedFrom...>, type_list<From, RemainingFrom...>,
                                                     type_list<ProcessedTo...>, type_list<To, RemainingTo...>)
{
  constexpr QuantitySpec auto from_factor = detail::get_factor(From{});
  constexpr QuantitySpec auto to_factor = detail::get_factor(To{});
  constexpr QuantitySpec auto from_root = get_hierarchy_root(from_factor);
  constexpr QuantitySpec auto to_root = get_hierarchy_root(to_factor);
  constexpr std::string_view from_root_name = detail::type_name<MP_UNITS_NONCONST_TYPE(from_root)>();
  constexpr std::string_view to_root_name = detail::type_name<MP_UNITS_NONCONST_TYPE(to_root)>();

  // Type lists are sorted by the root name. This allows us to easily progress through both lists to find the quantities
  // belonging to the same hierarchy tree.
  if constexpr (from_root_name < to_root_name)
    return detail::try_extract_common_base(type_list<ProcessedFrom..., From>{}, type_list<RemainingFrom...>{},
                                           type_list<ProcessedTo...>{}, type_list<To, RemainingTo...>{});
  else if constexpr (from_root_name > to_root_name)
    return detail::try_extract_common_base(type_list<ProcessedFrom...>{}, type_list<From, RemainingFrom...>{},
                                           type_list<ProcessedTo..., To>{}, type_list<RemainingTo...>{});
  else {
    // We are dealing with quantities from the same hierarchy tree.
    constexpr specs_convertible_result res = detail::convertible_common_base(from_factor, to_factor);
    if constexpr (res == specs_convertible_result::no)
      // We do not need to analyze the rest of the lists as the quantities are not convertible when at least one element
      // is not convertible.
      return std::optional(extract_common_base_result{specs_convertible_result::no});
    else {
      // We might deal with different powers of quantities. Let's find the biggest common exponent and extract it.
      // The rest of the quantities must be reinserted into the list.
      constexpr ratio from_exp = detail::get_exponent(From{});
      constexpr ratio to_exp = detail::get_exponent(To{});
      if constexpr (from_exp == to_exp)
        return std::optional(extract_common_base_result{res, type_list<ProcessedFrom..., RemainingFrom...>{},
                                                        type_list<ProcessedTo..., RemainingTo...>{}});
      else if constexpr (from_exp > to_exp) {
        using rest = power_or_T<MP_UNITS_NONCONST_TYPE(from_factor), from_exp - to_exp>;
        return std::optional(extract_common_base_result{res, type_list<ProcessedFrom..., rest, RemainingFrom...>{},
                                                        type_list<ProcessedTo..., RemainingTo...>{}});
      } else {
        using rest = power_or_T<MP_UNITS_NONCONST_TYPE(to_factor), to_exp - from_exp>;
        return std::optional(extract_common_base_result{res, type_list<ProcessedFrom..., RemainingFrom...>{},
                                                        type_list<ProcessedTo..., rest, RemainingTo...>{}});
      }
    }
  }
}

template<TypeList From, TypeList To>
[[nodiscard]] consteval auto try_extract_common_base(From from, To to)
{
  return detail::try_extract_common_base(type_list<>{}, from, type_list<>{}, to);
}

template<typename... Elems>
[[nodiscard]] consteval std::tuple<int, int> get_max_complexity(type_list<Elems...>)
{
  if constexpr (sizeof...(Elems) == 0)
    return {-1, -1};
  else {
    const std::initializer_list<int> list{detail::get_complexity_impl(Elems{})...};
    const auto it = max_element(list.begin(), list.end());
    return {*it, it - list.begin()};
  }
}

enum class ingredient_type : std::int8_t { numerator_from, denominator_from, numerator_to, denominator_to };

template<TypeList NumFrom, TypeList DenFrom, TypeList NumTo, TypeList DenTo>
[[nodiscard]] consteval std::tuple<int, std::size_t, ingredient_type> get_max_complexity(NumFrom num_from,
                                                                                         DenFrom den_from, NumTo num_to,
                                                                                         DenTo den_to)
{
  const std::initializer_list<std::tuple<int, int>> list{
    detail::get_max_complexity(num_from), detail::get_max_complexity(den_from), detail::get_max_complexity(num_to),
    detail::get_max_complexity(den_to)};
  const auto it =
    max_element(list.begin(), list.end(), [](auto lhs, auto rhs) { return std::get<0>(lhs) < std::get<0>(rhs); });
  return std::tuple_cat(*it, std::tuple{static_cast<ingredient_type>(it - list.begin())});
}

template<QuantitySpec Equation, TypeList Num, TypeList Den>
[[nodiscard]] consteval auto merge_with_equation(Equation, Num, Den)
{
  constexpr QuantitySpec auto merged_qs = [&]() {
    if constexpr (DerivedQuantitySpec<Equation>)
      return detail::get_optimized_expression<
        type_list_merge_sorted<typename Equation::_num_, Num, type_list_of_quantity_spec_less>,
        type_list_merge_sorted<typename Equation::_den_, Den, type_list_of_quantity_spec_less>, struct dimensionless,
        type_list_of_quantity_spec_less, derived_quantity_spec>();
    else
      return detail::get_optimized_expression<
        type_list_merge_sorted<Num, type_list<Equation>, type_list_of_quantity_spec_less>, Den, struct dimensionless,
        type_list_of_quantity_spec_less, derived_quantity_spec>();
  }();
  using qs_type = MP_UNITS_NONCONST_TYPE(merged_qs);
  if constexpr (DerivedQuantitySpec<qs_type>)
    return std::pair(typename qs_type::_num_{}, typename qs_type::_den_{});
  else
    return std::pair(type_list<qs_type>{}, type_list<>{});
}

template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval specs_convertible_result convertible(From, To);

template<TypeList NumFrom, TypeList DenFrom, TypeList NumTo, TypeList DenTo>
[[nodiscard]] constexpr specs_convertible_result are_ingredients_convertible(NumFrom num_from, DenFrom den_from,
                                                                             NumTo num_to, DenTo den_to)
{
  constexpr std::size_t num_from_size = type_list_size<NumFrom>;
  constexpr std::size_t den_from_size = type_list_size<DenFrom>;
  constexpr std::size_t num_to_size = type_list_size<NumTo>;
  constexpr std::size_t den_to_size = type_list_size<DenTo>;

  // if at least one of the sides is empty then we compare with dimensionless
  if constexpr (num_from_size == den_from_size && num_to_size + den_to_size == 0)
    return specs_convertible_result::yes;
  else if constexpr (num_from_size + den_from_size == 0 && num_to_size == den_to_size && num_to_size >= 1)
    return specs_convertible_result::explicit_conversion;
  else {
    // otherwise, check if the pairwise ingredients are convertible
    if constexpr (constexpr auto nums = detail::try_extract_common_base(NumFrom{}, NumTo{})) {
      if (nums->result == specs_convertible_result::no) return specs_convertible_result::no;
      return detail::min(nums->result,
                         detail::are_ingredients_convertible(nums->rest_from, den_from, nums->rest_to, den_to));
    } else if constexpr (constexpr auto dens = detail::try_extract_common_base(DenFrom{}, DenTo{})) {
      if (dens->result == specs_convertible_result::no) return specs_convertible_result::no;
      return detail::min(dens->result,
                         detail::are_ingredients_convertible(num_from, dens->rest_from, num_to, dens->rest_to));
    } else {
      // otherwise, get the ingredient with the highest complexity
      constexpr auto max_compl_res = detail::get_max_complexity(NumFrom{}, DenFrom{}, NumTo{}, DenTo{});
      constexpr int max_compl = std::get<0>(max_compl_res);
      constexpr std::size_t idx = std::get<1>(max_compl_res);
      constexpr ingredient_type type = std::get<2>(max_compl_res);

      constexpr auto list = [&] {
        if constexpr (type == ingredient_type::numerator_from)
          return NumFrom{};
        else if constexpr (type == ingredient_type::denominator_from)
          return DenFrom{};
        else if constexpr (type == ingredient_type::numerator_to)
          return NumTo{};
        else
          return DenTo{};
      }();
      using extracted = type_list_extract<MP_UNITS_NONCONST_TYPE(list), idx>;

      // before exploding check if the ingredient is convertible with dimensionless
      if constexpr (type == ingredient_type::numerator_from && num_to_size + den_to_size == 0) {
        constexpr specs_convertible_result res =
          detail::convertible(detail::get_factor(typename extracted::element{}), dimensionless);
        if constexpr (res != specs_convertible_result::no)
          return detail::min(res,
                             detail::are_ingredients_convertible(typename extracted::rest{}, den_from, num_to, den_to));
      } else if constexpr (type == ingredient_type::numerator_to && num_from_size + den_from_size == 0) {
        constexpr specs_convertible_result res =
          detail::convertible(dimensionless, detail::get_factor(typename extracted::element{}));
        if constexpr (res != specs_convertible_result::no)
          return detail::min(
            res, detail::are_ingredients_convertible(num_from, den_from, typename extracted::rest{}, den_to));
      }

      if constexpr (max_compl > 0) {
        // explode the ingredient to the underlying equation
        constexpr auto explode_res = detail::explode(typename extracted::element{});
        if constexpr (type == ingredient_type::numerator_from) {
          const auto [num, den] =
            detail::merge_with_equation(explode_res.equation, typename extracted::rest{}, den_from);
          return detail::are_ingredients_convertible(num, den, num_to, den_to);
        } else if constexpr (type == ingredient_type::denominator_from) {
          const auto [num, den] =
            detail::merge_with_equation(dimensionless / explode_res.equation, num_from, typename extracted::rest{});
          return detail::min(explode_res.result, detail::are_ingredients_convertible(num, den, num_to, den_to));
        } else if constexpr (type == ingredient_type::numerator_to) {
          const auto [num, den] = detail::merge_with_equation(explode_res.equation, typename extracted::rest{}, den_to);
          return detail::min(explode_res.result, detail::are_ingredients_convertible(num_from, den_from, num, den));
        } else {
          const auto [num, den] =
            detail::merge_with_equation(dimensionless / explode_res.equation, num_to, typename extracted::rest{});
          return detail::min(explode_res.result, detail::are_ingredients_convertible(num_from, den_from, num, den));
        }
      } else
        // this should never happen
        return specs_convertible_result::no;
    }
  }
}

template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(From, To)
{
  if constexpr (DerivedQuantitySpec<From> && DerivedQuantitySpec<To>)
    return detail::are_ingredients_convertible(typename From::_num_{}, typename From::_den_{}, typename To::_num_{},
                                               typename To::_den_{});
  else if constexpr (DerivedQuantitySpec<From> && NamedQuantitySpec<To>) {
    if constexpr (To{} == dimensionless)
      return detail::are_ingredients_convertible(typename From::_num_{}, typename From::_den_{}, type_list<>{},
                                                 type_list<>{});
    else
      return detail::are_ingredients_convertible(typename From::_num_{}, typename From::_den_{}, type_list<To>{},
                                                 type_list<>{});
  } else {
    if constexpr (From{} == dimensionless)
      return detail::are_ingredients_convertible(type_list<>{}, type_list<>{}, typename To::_num_{},
                                                 typename To::_den_{});
    else
      return detail::are_ingredients_convertible(type_list<From>{}, type_list<>{}, typename To::_num_{},
                                                 typename To::_den_{});
  }
}

template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval specs_convertible_result convertible_kinds(From from, To to)
{
  constexpr auto from_root = detail::get_kind_tree_root(from);
  constexpr auto to_root = detail::get_kind_tree_root(to);
  using enum specs_convertible_result;

  if constexpr (QuantityKindSpec<From>) {
    auto res = detail::convertible(from_root, to_root);
    // the below does not account for `explicit_conversion_beyond_kind`
    return (res == explicit_conversion) ? yes : res;
  } else
    return detail::convertible(from, to_root);
}

template<NamedQuantitySpec From, NamedQuantitySpec To>
[[nodiscard]] consteval specs_convertible_result convertible_named(From from, To to)
{
  using enum specs_convertible_result;

  if constexpr (detail::have_common_base(From{}, To{}))
    return detail::convertible_common_base(From{}, To{});
  else if constexpr (detail::get_kind_tree_root(From{}) != detail::get_kind_tree_root(To{}))
    return no;
  else if constexpr (detail::get_complexity(From{}) != detail::get_complexity(To{})) {
    if constexpr (detail::get_complexity(From{}) > detail::get_complexity(To{}))
      return detail::convertible(explode<get_complexity(To{})>(from).quantity, to);
    else {
      auto res = detail::explode<get_complexity(From{})>(to);
      return detail::min(res.result, detail::convertible(from, res.quantity));
    }
  }
}


template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval specs_convertible_result convertible_impl(From from, To to)
{
  using enum specs_convertible_result;

  // NOLINTBEGIN(bugprone-branch-clone)
  if constexpr (From{} == To{})
    return yes;
  else if constexpr (From::dimension != To::dimension)
    return no;
  else if constexpr (QuantityKindSpec<From> || QuantityKindSpec<To>)
    return detail::convertible_kinds(from, to);
  else if constexpr (NamedQuantitySpec<From> && NamedQuantitySpec<To>)
    return detail::convertible_named(from, to);
  else
    return detail::are_ingredients_convertible(from, to);
}

template<QuantitySpec From, QuantitySpec To>
constexpr specs_convertible_result convertible_result = detail::convertible_impl(From{}, To{});

template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval specs_convertible_result convertible(From, To)
{
  return convertible_result<From, To>;
}

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval bool implicitly_convertible(From from, To to)
{
  return detail::convertible(from, to) == detail::specs_convertible_result::yes;
}

template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval bool explicitly_convertible(From from, To to)
{
  return detail::convertible(from, to) >= detail::specs_convertible_result::explicit_conversion_beyond_kind;
}

template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval bool castable(From from, To to)
{
  return detail::convertible(from, to) >= detail::specs_convertible_result::cast;
}

template<QuantitySpec QS1, QuantitySpec QS2>
[[nodiscard]] consteval bool interconvertible(QS1 qs1, QS2 qs2)
{
  return mp_units::implicitly_convertible(qs1, qs2) && mp_units::implicitly_convertible(qs2, qs1);
}

MP_UNITS_EXPORT_END

namespace detail {

template<QuantitySpec Q>
  requires requires(Q q) { detail::get_kind_tree_root(q); }
using to_kind = decltype(detail::get_kind_tree_root(Q{}));

#if MP_UNITS_API_NO_CRTP
template<NamedQuantitySpec auto QS, auto... Args>
[[nodiscard]] consteval bool defined_as_kind_impl(quantity_spec<QS, Args...>)
#else
template<typename Self, NamedQuantitySpec auto QS, auto... Args>
[[nodiscard]] consteval bool defined_as_kind_impl(quantity_spec<Self, QS, Args...>)
#endif
{
  return mp_units::contains<struct is_kind, Args...>();
}

template<QuantitySpec Q>
[[nodiscard]] consteval QuantitySpec auto get_kind_tree_root_impl(Q q)
{
  auto defined_as_kind = []<typename QQ>(QQ qq) {
    if constexpr (requires { detail::defined_as_kind_impl(qq); })
      return detail::defined_as_kind_impl(QQ{});
    else
      return false;
  };

  // NOLINTBEGIN(bugprone-branch-clone)
  if constexpr (QuantityKindSpec<Q>) {
    return detail::remove_kind(q);
  } else if constexpr (defined_as_kind(Q{})) {
    return q;
  } else if constexpr (requires { Q::_parent_; }) {
    return detail::get_kind_tree_root(Q::_parent_);
  } else if constexpr (DerivedQuantitySpec<Q>) {
    return detail::expr_map<to_kind, derived_quantity_spec, struct dimensionless, type_list_of_quantity_spec_less>(q);
  } else {
    // root quantity
    return q;
  }
  // NOLINTEND(bugprone-branch-clone)
}

template<QuantitySpec Q>
constexpr QuantitySpec auto get_kind_tree_root_result = detail::get_kind_tree_root_impl(Q{});

template<QuantitySpec Q>
[[nodiscard]] consteval QuantitySpec auto get_kind_tree_root(Q)
{
  return get_kind_tree_root_result<Q>;
}

}  // namespace detail


MP_UNITS_EXPORT template<QuantitySpec Q>
[[nodiscard]] consteval detail::QuantityKindSpec auto get_kind(Q)
{
  return kind_of<detail::get_kind_tree_root(Q{})>;
}

namespace detail {

struct no_common_quantity_spec {};

template<QuantitySpec Q1, QuantitySpec Q2>
[[nodiscard]] consteval auto get_common_quantity_spec_impl(Q1 q1, Q2 q2)
{
  if constexpr (is_same_v<Q1, Q2>)
    return q1;
  else {
    constexpr bool q1q2 = mp_units::implicitly_convertible(Q1{}, Q2{});
    constexpr bool q2q1 = mp_units::implicitly_convertible(Q2{}, Q1{});

    // in case of interconvertible quantities we try to figure out which one is a better choice
    if constexpr (q1q2 && q2q1) {
      // quantity kinds have lower priority
      if constexpr (detail::QuantityKindSpec<Q1> && !detail::QuantityKindSpec<Q2>)
        return q2;
      else if constexpr (!detail::QuantityKindSpec<Q1> && detail::QuantityKindSpec<Q2>)
        return q1;
      else {
        using q1_type = decltype(detail::remove_kind(q1));
        using q2_type = decltype(detail::remove_kind(q2));
        // derived quantities have a lower priority
        if constexpr (detail::NamedQuantitySpec<q1_type> && detail::DerivedQuantitySpec<q2_type>)
          return q1;
        else if constexpr (detail::DerivedQuantitySpec<q1_type> && detail::NamedQuantitySpec<q2_type>)
          return q2;
        else
          // TODO Check if there is a better choice here
          return detail::better_type_name(q1, q2);
      }
    }
    // we prefer a quantity to which we can convert the other quantity
    else if constexpr (q1q2)
      return q2;
    else if constexpr (q2q1)
      return q1;
    // if quantities can't be converted in any direction check if they have a common base in the tree
    else if constexpr (detail::have_common_base(Q1{}, Q2{})) {
      constexpr auto base = detail::get_common_base(Q1{}, Q2{});
      if constexpr (mp_units::implicitly_convertible(q1, base) && mp_units::implicitly_convertible(q2, base))
        return base;
      else
        return no_common_quantity_spec{};
    } else {
      // verify kind tree roots as the last resort check
      constexpr auto q1_root = detail::get_kind_tree_root(Q1{});
      constexpr auto q2_root = detail::get_kind_tree_root(Q2{});
      if constexpr (mp_units::implicitly_convertible(q1_root, q2_root))
        return q2_root;
      else if constexpr (mp_units::implicitly_convertible(q2_root, q1_root))
        return q1_root;
      else
        return no_common_quantity_spec{};
    }
  }
}

template<QuantitySpec Q1, QuantitySpec Q2>
constexpr auto get_common_quantity_spec_result = detail::get_common_quantity_spec_impl(Q1{}, Q2{});

template<QuantitySpec Q1, QuantitySpec Q2>
[[nodiscard]] consteval bool have_common_quantity_spec(Q1, Q2)
{
  return !is_same_v<decltype(detail::get_common_quantity_spec_result<Q1, Q2>), const no_common_quantity_spec>;
}

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

[[nodiscard]] consteval QuantitySpec auto get_common_quantity_spec(QuantitySpec auto q) { return q; }

template<QuantitySpec Q1, QuantitySpec Q2>
  requires(detail::have_common_quantity_spec(Q1{}, Q2{}))
[[nodiscard]] consteval QuantitySpec auto get_common_quantity_spec(Q1, Q2)
{
  return detail::get_common_quantity_spec_result<Q1, Q2>;
}

[[nodiscard]] consteval QuantitySpec auto get_common_quantity_spec(QuantitySpec auto q1, QuantitySpec auto q2,
                                                                   QuantitySpec auto q3, QuantitySpec auto... rest)
  requires requires { mp_units::get_common_quantity_spec(mp_units::get_common_quantity_spec(q1, q2), q3, rest...); }
{
  return mp_units::get_common_quantity_spec(mp_units::get_common_quantity_spec(q1, q2), q3, rest...);
}

MP_UNITS_EXPORT_END

}  // namespace mp_units
