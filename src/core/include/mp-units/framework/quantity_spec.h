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
#include <mp-units/compat_macros.h>
#include <mp-units/ext/algorithm.h>
#include <mp-units/ext/type_name.h>
#include <mp-units/ext/type_traits.h>
#include <mp-units/framework/dimension.h>
#include <mp-units/framework/expression_template.h>
#include <mp-units/framework/quantity_concepts.h>
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/reference_concepts.h>
#include <mp-units/framework/representation_concepts.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#include <cstdint>
#include <tuple>
#include <type_traits>
#endif
#endif

namespace mp_units {

MP_UNITS_EXPORT struct dimensionless;

namespace detail {

template<QuantitySpec QS, Unit U>
  requires(!AssociatedUnit<U>) || UnitOf<U, QS{}>
[[nodiscard]] consteval Reference auto make_reference(QS, U u)
{
  if constexpr (QuantityKindSpec<QS>)
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
  return max({args...});
}

template<typename... Qs1, typename... Qs2>
[[nodiscard]] consteval quantity_character derived_quantity_character(const type_list<Qs1...>&,
                                                                      const type_list<Qs2...>&)
{
  constexpr quantity_character num =
    common_quantity_character(quantity_character::scalar, expr_type<Qs1>::character...);
  constexpr quantity_character den =
    common_quantity_character(quantity_character::scalar, expr_type<Qs2>::character...);
  if constexpr (num == den)
    return quantity_character::scalar;
  else
    return common_quantity_character(num, den);
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
  if constexpr (contains<quantity_character, Args...>())
    return get<quantity_character, Args...>();
  else
    return ch;
}

template<NamedQuantitySpec Lhs, NamedQuantitySpec Rhs>
struct quantity_spec_less : std::bool_constant<type_name<Lhs>() < type_name<Rhs>()> {};

template<typename T1, typename T2>
using type_list_of_quantity_spec_less = expr_less<T1, T2, quantity_spec_less>;

template<NamedQuantitySpec Q>
  requires requires { Q::dimension; }
using to_dimension = std::remove_const_t<decltype(Q::dimension)>;

template<AssociatedUnit U>
[[nodiscard]] consteval auto get_associated_quantity(U);

template<QuantitySpec auto... From, QuantitySpec Q>
[[nodiscard]] consteval QuantitySpec auto clone_kind_of(Q q);

template<QuantitySpec Q>
[[nodiscard]] consteval auto remove_kind(Q q);

struct quantity_spec_interface_base {
  template<QuantitySpec Lhs, QuantitySpec Rhs>
  [[nodiscard]] friend consteval QuantitySpec auto operator*(Lhs lhs, Rhs rhs)
  {
    return clone_kind_of<Lhs{}, Rhs{}>(
      expr_multiply<derived_quantity_spec, struct dimensionless, type_list_of_quantity_spec_less>(remove_kind(lhs),
                                                                                                  remove_kind(rhs)));
  }

  template<QuantitySpec Lhs, QuantitySpec Rhs>
  [[nodiscard]] friend consteval QuantitySpec auto operator/(Lhs lhs, Rhs rhs)
  {
    return clone_kind_of<Lhs{}, Rhs{}>(
      expr_divide<derived_quantity_spec, struct dimensionless, type_list_of_quantity_spec_less>(remove_kind(lhs),
                                                                                                remove_kind(rhs)));
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
    return make_reference(self, u);
  }

  template<typename Self, typename FwdQ, Quantity Q = std::remove_cvref_t<FwdQ>>
    requires QuantitySpecExplicitlyConvertibleTo<Q::quantity_spec, Self{}>
  [[nodiscard]] constexpr Quantity auto operator()(this Self self, FwdQ&& q)
  {
    return quantity{std::forward<FwdQ>(q).numerical_value_is_an_implementation_detail_, make_reference(self, Q::unit)};
  }
#else
  template<typename Self_ = Self, UnitOf<Self_{}> U>
  [[nodiscard]] MP_UNITS_CONSTEVAL Reference auto operator[](U u) const
  {
    return make_reference(Self{}, u);
  }

  template<typename FwdQ, Quantity Q = std::remove_cvref_t<FwdQ>, typename Self_ = Self>
    requires QuantitySpecExplicitlyConvertibleTo<Q::quantity_spec, Self_{}>
  [[nodiscard]] constexpr Quantity auto operator()(FwdQ&& q) const
  {
    return quantity{std::forward<FwdQ>(q).numerical_value_is_an_implementation_detail_,
                    make_reference(Self{}, Q::unit)};
  }
#endif
};

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
template<detail::BaseDimension auto Dim, one_of<quantity_character> auto... Args>
  requires(... && !QuantitySpec<decltype(Args)>)
struct quantity_spec<Dim, Args...> : detail::quantity_spec_interface {
#else
template<typename Self, detail::BaseDimension auto Dim, one_of<quantity_character> auto... Args>
  requires(... && !QuantitySpec<decltype(Args)>)
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
 * inline constexpr struct velocity final : quantity_spec<position_vector / duration> {} velocity;
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
template<detail::DerivedQuantitySpec auto Eq, one_of<quantity_character> auto... Args>
  requires(... && !QuantitySpec<decltype(Args)>)
struct quantity_spec<Eq, Args...> : detail::quantity_spec_interface {
#else
template<typename Self, detail::DerivedQuantitySpec auto Eq, one_of<quantity_character> auto... Args>
  requires(... && !QuantitySpec<decltype(Args)>)
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
 * inline constexpr struct position_vector final : quantity_spec<length, quantity_character::vector> {} position_vector;
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
template<detail::NamedQuantitySpec auto QS, one_of<quantity_character, struct is_kind> auto... Args>
  requires(... && !QuantitySpec<decltype(Args)>)
struct quantity_spec<QS, Args...> : detail::propagate_equation<QS>, detail::quantity_spec_interface {
#else
template<typename Self, detail::NamedQuantitySpec auto QS, one_of<quantity_character, struct is_kind> auto... Args>
  requires(... && !QuantitySpec<decltype(Args)>)
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
 * inline constexpr struct velocity final : quantity_spec<speed, position_vector / duration> {} velocity;
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
template<detail::NamedQuantitySpec auto QS, detail::DerivedQuantitySpec auto Eq,
         one_of<quantity_character, struct is_kind> auto... Args>
  requires(!requires { QS._equation_; } ||
           ((requires { QS._equation_; }) && detail::QuantitySpecExplicitlyConvertibleTo<Eq, QS._equation_>)) &&
          (... && !QuantitySpec<decltype(Args)>)
struct quantity_spec<QS, Eq, Args...> : detail::quantity_spec_interface {
#else
template<typename Self, detail::NamedQuantitySpec auto QS, detail::DerivedQuantitySpec auto Eq,
         one_of<quantity_character, struct is_kind> auto... Args>
  requires(!requires { QS._equation_; } ||
           ((requires { QS._equation_; }) && detail::QuantitySpecExplicitlyConvertibleTo<Eq, QS._equation_>)) &&
          (... && !QuantitySpec<decltype(Args)>)
struct quantity_spec<Self, QS, Eq, Args...> : detail::quantity_spec_interface<Self> {
#endif
  using _base_type_ = quantity_spec;
  static constexpr auto _parent_ = QS;
  static constexpr auto _equation_ = Eq;
  static constexpr Dimension auto dimension = _parent_.dimension;
  static constexpr quantity_character character = detail::quantity_character_init<Args...>(Eq.character);
};

namespace detail {

template<detail::DerivedQuantitySpecExpr... Expr>
struct derived_quantity_spec_impl :
#if MP_UNITS_API_NO_CRTP
    detail::quantity_spec_interface,
#else
    detail::quantity_spec_interface<derived_quantity_spec<Expr...>>,
#endif
    detail::expr_fractions<detail::is_dimensionless, Expr...> {
  using _base_type_ = derived_quantity_spec_impl;
  using _base_ = detail::expr_fractions<detail::is_dimensionless, Expr...>;

  static constexpr Dimension auto dimension =
    detail::expr_map<detail::to_dimension, derived_dimension, struct dimension_one,
                     detail::type_list_of_base_dimension_less>(_base_{});
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
 * Instead of using a raw list of exponents this library decided to use expression template syntax to make types
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
 * auto velocity = position_vector / duration;
 * auto acceleration = velocity / duration;
 * @endcode
 *
 * - the type of `frequency` is `derived_quantity_spec<dimensionless, per<period_duration>>`
 * - the dimension type of `frequency` is `derived_dimension<dimension_one, per<dim_time>>`
 * - the type of `area` is `derived_quantity_spec<power<length, 2>>`
 * - the dimension type of `area` is `derived_dimension<power<dim_length, 2>>`
 * - the type of `speed` is `derived_quantity_spec<distance, per<duration>>`
 * - the dimension type of `speed` is `derived_dimension<dim_length, per<dim_time>>`
 * - the type of `velocity` is `derived_quantity_spec<position_vector, per<duration>>`
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
template<detail::DerivedQuantitySpecExpr... Expr>
struct derived_quantity_spec final : detail::derived_quantity_spec_impl<Expr...> {};

/**
 * @brief Quantity of dimension one
 *
 * Quantity of dimension one also commonly named as "dimensionless" is a quantity with a dimension
 * for which all the exponents of the factors corresponding to the base dimensions are zero.
 */
MP_UNITS_EXPORT QUANTITY_SPEC(dimensionless, derived_quantity_spec<>{});

namespace detail {

template<>
struct is_dimensionless<struct dimensionless> : std::true_type {};

}  // namespace detail

/**
 * @brief Quantity kind specifier
 *
 * Specifies that the provided `Q` should be treated as a quantity kind.
 */
namespace detail {

template<typename T>
concept QuantitySpecWithNoSpecifiers = detail::NamedQuantitySpec<T> || detail::DerivedQuantitySpec<T>;

template<QuantitySpec Q>
[[nodiscard]] consteval QuantitySpec auto get_kind_tree_root(Q q);

}  // namespace detail

#if MP_UNITS_API_NO_CRTP
template<typename Q>
  requires detail::QuantitySpecWithNoSpecifiers<Q> && detail::SameQuantitySpec<detail::get_kind_tree_root(Q{}), Q{}>
struct kind_of_<Q> final : Q::_base_type_ {
  using _base_type_ = kind_of_;
  static constexpr auto _quantity_spec_ = Q{};
};
#else

#if MP_UNITS_COMP_CLANG
template<typename Q>
  requires detail::QuantitySpecWithNoSpecifiers<Q> && detail::SameQuantitySpec<detail::get_kind_tree_root(Q{}), Q{}>
#else
template<detail::QuantitySpecWithNoSpecifiers Q>
  requires detail::SameQuantitySpec<detail::get_kind_tree_root(Q{}), Q{}>
#endif
struct kind_of_<Q> final : quantity_spec<kind_of_<Q>, Q{}>::_base_type_ {
  using _base_type_ = kind_of_;
  static constexpr auto _quantity_spec_ = Q{};
};
#endif

MP_UNITS_EXPORT template<detail::QuantitySpecWithNoSpecifiers auto Q>
  requires detail::SameQuantitySpec<detail::get_kind_tree_root(Q), Q>
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
  if constexpr (detail::QuantityKindSpec<Q>)
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
  if constexpr (Num == 0 || Q{} == dimensionless)
    return dimensionless;
  else if constexpr (detail::ratio{Num, Den} == 1)
    return q;
  else if constexpr (detail::DerivedQuantitySpec<Q>)
    return detail::clone_kind_of<Q{}>(
      detail::expr_pow<Num, Den, derived_quantity_spec, struct dimensionless, detail::type_list_of_quantity_spec_less>(
        detail::remove_kind(q)));
  else if constexpr (Den == 1)
    return detail::clone_kind_of<Q{}>(derived_quantity_spec<power<decltype(detail::remove_kind(Q{})), Num>>{});
  else
    return detail::clone_kind_of<Q{}>(derived_quantity_spec<power<decltype(detail::remove_kind(Q{})), Num, Den>>{});
}


/**
 * @brief Computes the square root of a quantity specification
 *
 * @param q Quantity specification being the base of the operation
 *
 * @return QuantitySpec The result of computation
 */
[[nodiscard]] consteval QuantitySpec auto sqrt(QuantitySpec auto q) { return pow<1, 2>(q); }


/**
 * @brief Computes the cubic root of a quantity specification
 *
 * @param q Quantity specification being the base of the operation
 *
 * @return QuantitySpec The result of computation
 */
[[nodiscard]] consteval QuantitySpec auto cbrt(QuantitySpec auto q) { return pow<1, 3>(q); }

MP_UNITS_EXPORT_END

namespace detail {

template<QuantitySpec Q>
[[nodiscard]] consteval int get_complexity(Q);

template<typename... Ts>
[[nodiscard]] consteval int get_complexity(type_list<Ts...>)
{
  return (0 + ... + get_complexity(Ts{}));
}

template<QuantitySpec Q, int... Ints>
[[nodiscard]] consteval int get_complexity(power<Q, Ints...>)
{
  return get_complexity(Q{});
}

template<typename Q>
[[nodiscard]] consteval int get_complexity(kind_of_<Q>)
{
  return get_complexity(Q{});
}

template<QuantitySpec Q>
[[nodiscard]] consteval int get_complexity(Q)
{
  if constexpr (detail::DerivedQuantitySpec<Q>)
    return get_complexity(typename Q::_num_{}) + get_complexity(typename Q::_den_{});
  else if constexpr (requires { Q::_equation_; })
    return 1 + get_complexity(Q::_equation_);
  else
    return 1;
}

// dimension_one is always the last one
// otherwise, sort by typename
template<Dimension D1, Dimension D2>
[[nodiscard]] consteval bool ingredients_dimension_less(D1, D2)
{
  if constexpr (D1{} == D2{} || D1{} == dimension_one)
    return false;
  else if constexpr (D2{} == dimension_one)
    return true;
  else
    return detail::type_name<D1>() < detail::type_name<D2>();
}

template<QuantitySpec Lhs, QuantitySpec Rhs, bool lhs_eq = requires { Lhs::_equation_; },
         bool rhs_eq = requires { Rhs::_equation_; }, ratio lhs_compl = get_complexity(Lhs{}),
         ratio rhs_compl = get_complexity(Rhs{})>
struct ingredients_less :
    std::bool_constant<(lhs_compl > rhs_compl) ||
                       (lhs_compl == rhs_compl && ingredients_dimension_less(Lhs::dimension, Rhs::dimension)) ||
                       (lhs_compl == rhs_compl && Lhs::dimension == Rhs::dimension &&
                        detail::type_name<Lhs>() < detail::type_name<Rhs>())> {};

template<typename T1, typename T2>
using type_list_of_ingredients_less = expr_less<T1, T2, ingredients_less>;

template<QuantitySpec Q>
  requires requires { Q::_equation_; }
[[nodiscard]] consteval bool defines_equation(Q)
{
  if constexpr (requires { Q::_parent_._equation_; })
    return Q::_parent_._equation_ != Q::_equation_;
  else
    return true;
}

enum class specs_convertible_result : std::int8_t { no, cast, explicit_conversion, yes };

template<QuantitySpec Q>
struct explode_to_equation_result {
  Q equation;
  specs_convertible_result result;
};

#if MP_UNITS_COMP_CLANG

template<QuantitySpec Q>
explode_to_equation_result(Q, specs_convertible_result) -> explode_to_equation_result<Q>;

#endif

template<QuantitySpec Q>
  requires requires { Q::_equation_; }
[[nodiscard]] consteval auto explode_to_equation(Q q)
{
  return explode_to_equation_result{
    Q::_equation_, defines_equation(q) ? specs_convertible_result::yes : specs_convertible_result::explicit_conversion};
}

template<QuantitySpec Q, int... Ints>
  requires requires { Q::_equation_; }
[[nodiscard]] consteval auto explode_to_equation(power<Q, Ints...>)
{
  constexpr ratio exp = power<Q, Ints...>::exponent;
  return explode_to_equation_result{
    pow<exp.num, exp.den>(Q::_equation_),
    defines_equation(Q{}) ? specs_convertible_result::yes : specs_convertible_result::explicit_conversion};
}

template<QuantitySpec Q>
struct explode_result {
  Q quantity;
  specs_convertible_result result = specs_convertible_result::yes;

  template<typename T>
  [[nodiscard]] consteval explode_result common_convertibility_with(explode_to_equation_result<T> res) const
  {
    return {quantity, min(result, res.result)};
  }
};

#if MP_UNITS_COMP_CLANG

template<QuantitySpec Q>
explode_result(Q) -> explode_result<Q>;

#endif

template<int Complexity, DerivedQuantitySpec Q>
[[nodiscard]] consteval auto explode(Q q);

template<int Complexity, NamedQuantitySpec Q>
[[nodiscard]] consteval auto explode(Q q);

template<int Complexity, QuantitySpec Q, typename Num, typename... Nums, typename Den, typename... Dens>
[[nodiscard]] consteval auto explode(Q, type_list<Num, Nums...>, type_list<Den, Dens...>)
{
  constexpr auto num = get_complexity(Num{});
  constexpr auto den = get_complexity(Den{});
  constexpr auto max_compl = num > den ? num : den;

  if constexpr (max_compl == Complexity || ((num >= den && !requires { explode_to_equation(Num{}); }) ||
                                            (num < den && !requires { explode_to_equation(Den{}); })))
    return explode_result{(map_power(Num{}) * ... * map_power(Nums{})) / (map_power(Den{}) * ... * map_power(Dens{}))};
  else {
    if constexpr (num >= den) {
      constexpr auto res = explode_to_equation(Num{});
      return explode<Complexity>((res.equation * ... * map_power(Nums{})) /
                                 (map_power(Den{}) * ... * map_power(Dens{})))
        .common_convertibility_with(res);
    } else {
      constexpr auto res = explode_to_equation(Den{});
      return explode<Complexity>((map_power(Num{}) * ... * map_power(Nums{})) /
                                 (res.equation * ... * map_power(Dens{})))
        .common_convertibility_with(res);
    }
  }
}

template<int Complexity, QuantitySpec Q, typename Num, typename... Nums>
[[nodiscard]] consteval auto explode(Q, type_list<Num, Nums...>, type_list<>)
{
  constexpr auto n = get_complexity(Num{});
  if constexpr (n == Complexity || !requires { explode_to_equation(Num{}); })
    return explode_result{(map_power(Num{}) * ... * map_power(Nums{}))};
  else {
    constexpr auto res = explode_to_equation(Num{});
    return explode<Complexity>((res.equation * ... * map_power(Nums{}))).common_convertibility_with(res);
  }
}

template<int Complexity, QuantitySpec Q, typename Den, typename... Dens>
[[nodiscard]] consteval auto explode(Q, type_list<>, type_list<Den, Dens...>)
{
  constexpr auto den = get_complexity(Den{});
  if constexpr (den == Complexity || !requires { explode_to_equation(Den{}); })
    return explode_result{dimensionless / (map_power(Den{}) * ... * map_power(Dens{}))};
  else {
    constexpr auto res = explode_to_equation(Den{});
    return explode<Complexity>(dimensionless / (res.equation * ... * map_power(Dens{})))
      .common_convertibility_with(res);
  }
}

template<int Complexity, QuantitySpec Q>
[[nodiscard]] consteval auto explode(Q, type_list<>, type_list<>)
{
  return explode_result{dimensionless};
}

template<int Complexity, DerivedQuantitySpec Q>
[[nodiscard]] consteval auto explode(Q q)
{
  constexpr auto complexity = get_complexity(Q{});
  if constexpr (complexity > Complexity)
    return explode<Complexity>(q, type_list_sort<typename Q::_num_, type_list_of_ingredients_less>{},
                               type_list_sort<typename Q::_den_, type_list_of_ingredients_less>{});
  else
    return explode_result{q};
}

template<int Complexity, NamedQuantitySpec Q>
[[nodiscard]] consteval auto explode(Q q)
{
  constexpr auto complexity = get_complexity(Q{});
  if constexpr (complexity > Complexity && requires { Q::_equation_; }) {
    constexpr auto res = explode_to_equation(Q{});
    return explode<Complexity>(res.equation).common_convertibility_with(res);
  } else
    return explode_result{q};
}

template<typename NumFrom, typename... NumsFrom, typename DenFrom, typename... DensFrom, typename NumTo,
         typename... NumsTo, typename DenTo, typename... DensTo>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<NumFrom, NumsFrom...> num_from,
                                                                             type_list<DenFrom, DensFrom...> den_from,
                                                                             type_list<NumTo, NumsTo...> num_to,
                                                                             type_list<DenTo, DensTo...> den_to);

template<typename DenFrom, typename... DensFrom, typename NumTo, typename... NumsTo, typename DenTo, typename... DensTo>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<>,
                                                                             type_list<DenFrom, DensFrom...>,
                                                                             type_list<NumTo, NumsTo...>,
                                                                             type_list<DenTo, DensTo...>);

template<typename NumFrom, typename... NumsFrom, typename NumTo, typename... NumsTo, typename DenTo, typename... DensTo>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<NumFrom, NumsFrom...>,
                                                                             type_list<>, type_list<NumTo, NumsTo...>,
                                                                             type_list<DenTo, DensTo...>);

template<typename NumFrom, typename... NumsFrom, typename DenFrom, typename... DensFrom, typename DenTo,
         typename... DensTo>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<NumFrom, NumsFrom...>,
                                                                             type_list<DenFrom, DensFrom...>,
                                                                             type_list<>, type_list<DenTo, DensTo...>);

template<typename NumFrom, typename... NumsFrom, typename DenFrom, typename... DensFrom, typename NumTo,
         typename... NumsTo>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<NumFrom, NumsFrom...>,
                                                                             type_list<DenFrom, DensFrom...>,
                                                                             type_list<NumTo, NumsTo...>, type_list<>);

template<typename NumFrom, typename... NumsFrom, typename NumTo, typename... NumsTo>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<NumFrom, NumsFrom...>,
                                                                             type_list<>, type_list<NumTo, NumsTo...>,
                                                                             type_list<>);

template<typename DenFrom, typename... DensFrom, typename DenTo, typename... DensTo>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<>,
                                                                             type_list<DenFrom, DensFrom...>,
                                                                             type_list<>, type_list<DenTo, DensTo...>);

template<typename... NumsFrom, typename... DensFrom>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<NumsFrom...>,
                                                                             type_list<DensFrom...>, type_list<>,
                                                                             type_list<>);

template<typename... NumsTo, typename... DensTo>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<>, type_list<>,
                                                                             type_list<NumsTo...>,
                                                                             type_list<DensTo...>);

template<typename... NumsFrom>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<NumsFrom...>, type_list<>,
                                                                             type_list<>, type_list<>);
template<typename... DensFrom>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<>, type_list<DensFrom...>,
                                                                             type_list<>, type_list<>);

template<typename... NumsTo>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<>, type_list<>,
                                                                             type_list<NumsTo...>, type_list<>);

template<typename... DensFrom>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<>, type_list<>, type_list<>,
                                                                             type_list<DensFrom...>);

[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<>, type_list<>, type_list<>,
                                                                             type_list<>);

enum class prepend_rest : std::int8_t { no, first, second };

template<QuantitySpec From = struct dimensionless, QuantitySpec To = struct dimensionless, typename Elem = int>
struct extract_results {
  bool same_kind{};
  From from{};
  To to{};
  prepend_rest prepend{};
  Elem elem{};
};

#if MP_UNITS_COMP_CLANG

template<QuantitySpec From = struct dimensionless, QuantitySpec To = struct dimensionless, typename Elem = int>
extract_results(bool, From = {}, To = {}, prepend_rest = {}, Elem = {}) -> extract_results<From, To, Elem>;

#endif

// tries to find the largest common power of a quantity
// in case powers have different factors of the same dimension, returns the remainder
template<typename From, typename To>
[[nodiscard]] consteval auto extract_convertible_quantities(From, To)
{
  constexpr auto qfrom = map_power(From{});
  constexpr auto qto = map_power(To{});
  if constexpr (get_kind_tree_root(qfrom) == get_kind_tree_root(qto)) {
    if constexpr (is_specialization_of_power<From> && is_specialization_of_power<To>)
      return extract_results{true, typename From::factor{}, typename To::factor{}, prepend_rest::no};
    else
      return extract_results{true, qfrom, qto, prepend_rest::no};
  } else {
    auto normalize = []<typename Q>(Q) {
      if constexpr (is_specialization_of_power<Q>)
        return std::tuple{typename Q::factor{}, Q::exponent};
      else
        return std::tuple{Q{}, ratio{1}};
    };
    constexpr auto from_norm = normalize(From{});
    constexpr auto to_norm = normalize(To{});
    constexpr auto from_factor = std::get<0>(from_norm);
    constexpr auto from_exp = std::get<1>(from_norm);
    constexpr auto to_factor = std::get<0>(to_norm);
    constexpr auto to_exp = std::get<1>(to_norm);
    if constexpr (get_kind_tree_root(from_factor) != get_kind_tree_root(to_factor))
      return extract_results{false};
    else if constexpr (from_exp > to_exp)
      return extract_results{true, from_factor, to_factor, prepend_rest::first,
                             power_or_T<decltype(from_factor), from_exp - to_exp>{}};
    else
      return extract_results{true, from_factor, to_factor, prepend_rest::second,
                             power_or_T<decltype(to_factor), to_exp - from_exp>{}};
  }
}

enum class extracted_entities : std::int8_t { numerators, denominators, from, to };

template<extracted_entities Entities, auto Ext, TypeList NumFrom, TypeList DenFrom, TypeList NumTo, TypeList DenTo>
[[nodiscard]] consteval specs_convertible_result prepend_and_process_rest(NumFrom num_from, DenFrom den_from,
                                                                          NumTo num_to, DenTo den_to)
{
  constexpr auto res = [&]() consteval {
    if constexpr (Ext.prepend == prepend_rest::no)
      return are_ingredients_convertible(num_from, den_from, num_to, den_to);
    else {
      using elem = decltype(Ext.elem);
      if constexpr (Entities == extracted_entities::numerators) {
        if constexpr (Ext.prepend == prepend_rest::first)
          return are_ingredients_convertible(type_list_push_front<NumFrom, elem>{}, den_from, num_to, den_to);
        else
          return are_ingredients_convertible(num_from, den_from, type_list_push_front<NumTo, elem>{}, den_to);
      } else if constexpr (Entities == extracted_entities::denominators) {
        if constexpr (Ext.prepend == prepend_rest::first)
          return are_ingredients_convertible(num_from, type_list_push_front<DenFrom, elem>{}, num_to, den_to);
        else
          return are_ingredients_convertible(num_from, den_from, num_to, type_list_push_front<DenTo, elem>{});
      } else if constexpr (Entities == extracted_entities::from) {
        if constexpr (Ext.prepend == prepend_rest::first)
          return are_ingredients_convertible(type_list_push_front<NumFrom, elem>{}, den_from, num_to, den_to);
        else
          return are_ingredients_convertible(num_from, type_list_push_front<DenFrom, elem>{}, num_to, den_to);
      } else if constexpr (Entities == extracted_entities::to) {
        if constexpr (Ext.prepend == prepend_rest::first)
          return are_ingredients_convertible(num_from, den_from, type_list_push_front<NumTo, elem>{}, den_to);
        else
          return are_ingredients_convertible(num_from, den_from, num_to, type_list_push_front<DenTo, elem>{});
      }
    }
  }();

  if constexpr (Entities == extracted_entities::numerators || Entities == extracted_entities::denominators)
    return min(res, convertible_impl(Ext.from, Ext.to));
  else
    return res;
}

template<typename NumFrom, typename... NumsFrom, typename DenFrom, typename... DensFrom, typename NumTo,
         typename... NumsTo, typename DenTo, typename... DensTo>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<NumFrom, NumsFrom...> num_from,
                                                                             type_list<DenFrom, DensFrom...> den_from,
                                                                             type_list<NumTo, NumsTo...> num_to,
                                                                             type_list<DenTo, DensTo...> den_to)
{
  if constexpr (constexpr auto extN = extract_convertible_quantities(NumFrom{}, NumTo{}); extN.same_kind)
    return prepend_and_process_rest<extracted_entities::numerators, extN>(type_list<NumsFrom...>{}, den_from,
                                                                          type_list<NumsTo...>{}, den_to);
  else if constexpr (constexpr auto extD = extract_convertible_quantities(DenFrom{}, DenTo{}); extD.same_kind)
    return prepend_and_process_rest<extracted_entities::denominators, extD>(num_from, type_list<DensFrom...>{}, num_to,
                                                                            type_list<DensTo...>{});
  else if constexpr (constexpr auto extF = extract_convertible_quantities(NumFrom{}, DenFrom{}); extF.same_kind)
    return prepend_and_process_rest<extracted_entities::from, extF>(type_list<NumsFrom...>{}, type_list<DensFrom...>{},
                                                                    num_to, den_to);
  else if constexpr (constexpr auto extT = extract_convertible_quantities(NumTo{}, DenTo{}); extT.same_kind)
    return prepend_and_process_rest<extracted_entities::to, extT>(num_from, den_from, type_list<NumsTo...>{},
                                                                  type_list<DensTo...>{});
  else {
    constexpr auto num_from_compl = get_complexity(NumFrom{});
    constexpr auto den_from_compl = get_complexity(DenFrom{});
    constexpr auto num_to_compl = get_complexity(NumTo{});
    constexpr auto den_to_compl = get_complexity(DenTo{});
    constexpr auto max_compl = max({num_from_compl, num_to_compl, den_from_compl, den_to_compl});
    if constexpr (max_compl > 1) {
      if constexpr (num_from_compl == max_compl) {
        constexpr auto res = explode_to_equation(NumFrom{});
        return convertible_impl(
          (res.equation * ... * map_power(NumsFrom{})) / (map_power(DenFrom{}) * ... * map_power(DensFrom{})),
          (map_power(NumTo{}) * ... * map_power(NumsTo{})) / (map_power(DenTo{}) * ... * map_power(DensTo{})));
      } else if constexpr (den_from_compl == max_compl) {
        constexpr auto res = explode_to_equation(DenFrom{});
        return convertible_impl(
          (map_power(NumFrom{}) * ... * map_power(NumsFrom{})) / (res.equation * ... * map_power(DensFrom{})),
          (map_power(NumTo{}) * ... * map_power(NumsTo{})) / (map_power(DenTo{}) * ... * map_power(DensTo{})));
      } else if constexpr (num_to_compl == max_compl) {
        constexpr auto res = explode_to_equation(NumTo{});
        return min(res.result, convertible_impl((map_power(NumFrom{}) * ... * map_power(NumsFrom{})) /
                                                  (map_power(DenFrom{}) * ... * map_power(DensFrom{})),
                                                (res.equation * ... * map_power(NumsTo{})) /
                                                  (map_power(DenTo{}) * ... * map_power(DensTo{}))));
      } else {
        constexpr auto res = explode_to_equation(DenTo{});
        return min(res.result, convertible_impl((map_power(NumFrom{}) * ... * map_power(NumsFrom{})) /
                                                  (map_power(DenFrom{}) * ... * map_power(DensFrom{})),
                                                (map_power(NumTo{}) * ... * map_power(NumsTo{})) /
                                                  (res.equation * ... * map_power(DensTo{}))));
      }
    }
  }
}

template<typename DenFrom, typename... DensFrom, typename NumTo, typename... NumsTo, typename DenTo, typename... DensTo>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<> num_from,
                                                                             type_list<DenFrom, DensFrom...> den_from,
                                                                             type_list<NumTo, NumsTo...> num_to,
                                                                             type_list<DenTo, DensTo...>)
{
  if constexpr (constexpr auto extD = extract_convertible_quantities(DenFrom{}, DenTo{}); extD.same_kind)
    return prepend_and_process_rest<extracted_entities::denominators, extD>(num_from, type_list<DensFrom...>{}, num_to,
                                                                            type_list<DensTo...>{});
  else if constexpr (constexpr auto extT = extract_convertible_quantities(NumTo{}, DenTo{}); extT.same_kind)
    return prepend_and_process_rest<extracted_entities::to, extT>(num_from, den_from, type_list<NumsTo...>{},
                                                                  type_list<DensTo...>{});
  else {
    constexpr auto den_from_compl = get_complexity(DenFrom{});
    constexpr auto num_to_compl = get_complexity(NumTo{});
    constexpr auto den_to_compl = get_complexity(DenTo{});
    constexpr auto max_compl = max({num_to_compl, den_from_compl, den_to_compl});
    if constexpr (max_compl > 1) {
      if constexpr (den_from_compl == max_compl) {
        constexpr auto res = explode_to_equation(DenFrom{});
        return convertible_impl(
          dimensionless / (res.equation * ... * map_power(DensFrom{})),
          (map_power(NumTo{}) * ... * map_power(NumsTo{})) / (map_power(DenTo{}) * ... * map_power(DensTo{})));
      } else if constexpr (num_to_compl == max_compl) {
        constexpr auto res = explode_to_equation(NumTo{});
        return min(res.result, convertible_impl(dimensionless / (map_power(DenFrom{}) * ... * map_power(DensFrom{})),
                                                (res.equation * ... * map_power(NumsTo{})) /
                                                  (map_power(DenTo{}) * ... * map_power(DensTo{}))));
      } else {
        constexpr auto res = explode_to_equation(DenTo{});
        return min(res.result, convertible_impl(dimensionless / (map_power(DenFrom{}) * ... * map_power(DensFrom{})),
                                                (map_power(NumTo{}) * ... * map_power(NumsTo{})) /
                                                  (res.equation * ... * map_power(DensTo{}))));
      }
    }
  }
}

template<typename NumFrom, typename... NumsFrom, typename NumTo, typename... NumsTo, typename DenTo, typename... DensTo>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<NumFrom, NumsFrom...> num_from,
                                                                             type_list<> den_from,
                                                                             type_list<NumTo, NumsTo...>,
                                                                             type_list<DenTo, DensTo...> den_to)
{
  if constexpr (constexpr auto extN = extract_convertible_quantities(NumFrom{}, NumTo{}); extN.same_kind)
    return prepend_and_process_rest<extracted_entities::numerators, extN>(type_list<NumsFrom...>{}, den_from,
                                                                          type_list<NumsTo...>{}, den_to);
  else if constexpr (constexpr auto extT = extract_convertible_quantities(NumTo{}, DenTo{}); extT.same_kind)
    return prepend_and_process_rest<extracted_entities::to, extT>(num_from, den_from, type_list<NumsTo...>{},
                                                                  type_list<DensTo...>{});
  else {
    constexpr auto num_from_compl = get_complexity(NumFrom{});
    constexpr auto num_to_compl = get_complexity(NumTo{});
    constexpr auto den_to_compl = get_complexity(DenTo{});
    constexpr auto max_compl = max({num_from_compl, num_to_compl, den_to_compl});
    if constexpr (max_compl > 1) {
      if constexpr (num_from_compl == max_compl) {
        constexpr auto res = explode_to_equation(NumFrom{});
        return convertible_impl(
          (res.equation * ... * map_power(NumsFrom{})),
          (map_power(NumTo{}) * ... * map_power(NumsTo{})) / (map_power(DenTo{}) * ... * map_power(DensTo{})));
      } else if constexpr (num_to_compl == max_compl) {
        constexpr auto res = explode_to_equation(NumTo{});
        return min(res.result, convertible_impl((map_power(NumFrom{}) * ... * map_power(NumsFrom{})),
                                                (res.equation * ... * map_power(NumsTo{})) /
                                                  (map_power(DenTo{}) * ... * map_power(DensTo{}))));
      } else {
        constexpr auto res = explode_to_equation(DenTo{});
        return min(res.result, convertible_impl((map_power(NumFrom{}) * ... * map_power(NumsFrom{})),
                                                (map_power(NumTo{}) * ... * map_power(NumsTo{})) /
                                                  (res.equation * ... * map_power(DensTo{}))));
      }
    }
  }
}

template<typename NumFrom, typename... NumsFrom, typename DenFrom, typename... DensFrom, typename DenTo,
         typename... DensTo>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<NumFrom, NumsFrom...> num_from,
                                                                             type_list<DenFrom, DensFrom...>,
                                                                             type_list<> num_to,
                                                                             type_list<DenTo, DensTo...> den_to)
{
  if constexpr (constexpr auto extD = extract_convertible_quantities(DenFrom{}, DenTo{}); extD.same_kind)
    return prepend_and_process_rest<extracted_entities::denominators, extD>(num_from, type_list<DensFrom...>{}, num_to,
                                                                            type_list<DensTo...>{});
  else if constexpr (constexpr auto extF = extract_convertible_quantities(NumFrom{}, DenFrom{}); extF.same_kind)
    return prepend_and_process_rest<extracted_entities::from, extF>(type_list<NumsFrom...>{}, type_list<DensFrom...>{},
                                                                    num_to, den_to);
  else {
    constexpr auto num_from_compl = get_complexity(NumFrom{});
    constexpr auto den_from_compl = get_complexity(DenFrom{});
    constexpr auto den_to_compl = get_complexity(DenTo{});
    constexpr auto max_compl = max({num_from_compl, den_from_compl, den_to_compl});
    if constexpr (max_compl > 1) {
      if constexpr (num_from_compl == max_compl) {
        constexpr auto res = explode_to_equation(NumFrom{});
        return convertible_impl(
          (res.equation * ... * map_power(NumsFrom{})) / (map_power(DenFrom{}) * ... * map_power(DensFrom{})),
          dimensionless / (map_power(DenTo{}) * ... * map_power(DensTo{})));
      } else if constexpr (den_from_compl == max_compl) {
        constexpr auto res = explode_to_equation(DenFrom{});
        return convertible_impl(
          (map_power(NumFrom{}) * ... * map_power(NumsFrom{})) / (res.equation * ... * map_power(DensFrom{})),
          dimensionless / (map_power(DenTo{}) * ... * map_power(DensTo{})));
      } else {
        constexpr auto res = explode_to_equation(DenTo{});
        return min(res.result, convertible_impl((map_power(NumFrom{}) * ... * map_power(NumsFrom{})) /
                                                  (map_power(DenFrom{}) * ... * map_power(DensFrom{})),
                                                dimensionless / (res.equation * ... * map_power(DensTo{}))));
      }
    }
  }
}

template<typename NumFrom, typename... NumsFrom, typename DenFrom, typename... DensFrom, typename NumTo,
         typename... NumsTo>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<NumFrom, NumsFrom...>,
                                                                             type_list<DenFrom, DensFrom...> den_from,
                                                                             type_list<NumTo, NumsTo...> num_to,
                                                                             type_list<> den_to)
{
  if constexpr (constexpr auto extN = extract_convertible_quantities(NumFrom{}, NumTo{}); extN.same_kind)
    return prepend_and_process_rest<extracted_entities::numerators, extN>(type_list<NumsFrom...>{}, den_from,
                                                                          type_list<NumsTo...>{}, den_to);
  else if constexpr (constexpr auto extF = extract_convertible_quantities(NumFrom{}, DenFrom{}); extF.same_kind)
    return prepend_and_process_rest<extracted_entities::from, extF>(type_list<NumsFrom...>{}, type_list<DensFrom...>{},
                                                                    num_to, den_to);
  else {
    constexpr auto num_from_compl = get_complexity(NumFrom{});
    constexpr auto den_from_compl = get_complexity(DenFrom{});
    constexpr auto num_to_compl = get_complexity(NumTo{});
    constexpr auto max_compl = max({num_from_compl, num_to_compl, den_from_compl});
    if constexpr (max_compl > 1) {
      if constexpr (num_from_compl == max_compl) {
        constexpr auto res = explode_to_equation(NumFrom{});
        return convertible_impl(
          (res.equation * ... * map_power(NumsFrom{})) / (map_power(DenFrom{}) * ... * map_power(DensFrom{})),
          (map_power(NumTo{}) * ... * map_power(NumsTo{})));
      } else if constexpr (den_from_compl == max_compl) {
        constexpr auto res = explode_to_equation(DenFrom{});
        return convertible_impl(
          (map_power(NumFrom{}) * ... * map_power(NumsFrom{})) / (res.equation * ... * map_power(DensFrom{})),
          (map_power(NumTo{}) * ... * map_power(NumsTo{})));
      } else {
        constexpr auto res = explode_to_equation(NumTo{});
        return min(res.result, convertible_impl((map_power(NumFrom{}) * ... * map_power(NumsFrom{})) /
                                                  (map_power(DenFrom{}) * ... * map_power(DensFrom{})),
                                                (res.equation * ... * map_power(NumsTo{}))));
      }
    }
  }
}

template<typename NumFrom, typename... NumsFrom, typename NumTo, typename... NumsTo>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<NumFrom, NumsFrom...>,
                                                                             type_list<> den_from,
                                                                             type_list<NumTo, NumsTo...>,
                                                                             type_list<> den_to)
{
  if constexpr (constexpr auto ext = extract_convertible_quantities(NumFrom{}, NumTo{}); ext.same_kind) {
    return prepend_and_process_rest<extracted_entities::numerators, ext>(type_list<NumsFrom...>{}, den_from,
                                                                         type_list<NumsTo...>{}, den_to);
  } else {
    constexpr auto num_from_compl = get_complexity(NumFrom{});
    constexpr auto num_to_compl = get_complexity(NumTo{});
    constexpr auto max_compl = max(num_from_compl, num_to_compl);
    if constexpr (max_compl > 1) {
      if constexpr (num_from_compl == max_compl) {
        constexpr auto res = explode_to_equation(NumFrom{});
        return convertible_impl((res.equation * ... * map_power(NumsFrom{})),
                                (map_power(NumTo{}) * ... * map_power(NumsTo{})));
      } else {
        constexpr auto res = explode_to_equation(NumTo{});
        return min(res.result, convertible_impl((map_power(NumFrom{}) * ... * map_power(NumsFrom{})),
                                                (res.equation * ... * map_power(NumsTo{}))));
      }
    }
  }
}

template<typename DenFrom, typename... DensFrom, typename DenTo, typename... DensTo>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<> num_from,
                                                                             type_list<DenFrom, DensFrom...>,
                                                                             type_list<> num_to,
                                                                             type_list<DenTo, DensTo...>)
{
  if constexpr (constexpr auto ext = extract_convertible_quantities(DenFrom{}, DenTo{}); ext.same_kind)
    return prepend_and_process_rest<extracted_entities::denominators, ext>(num_from, type_list<DensFrom...>{}, num_to,
                                                                           type_list<DensTo...>{});
  else {
    constexpr auto den_from_compl = get_complexity(DenFrom{});
    constexpr auto den_to_compl = get_complexity(DenTo{});
    constexpr auto max_compl = max(den_from_compl, den_to_compl);
    if constexpr (max_compl > 1) {
      if constexpr (den_from_compl == max_compl) {
        constexpr auto res = explode_to_equation(DenFrom{});
        return convertible_impl(dimensionless / (res.equation * ... * map_power(DensFrom{})),
                                dimensionless / (map_power(DenTo{}) * ... * map_power(DensTo{})));
      } else {
        constexpr auto res = explode_to_equation(DenTo{});
        return min(res.result, convertible_impl(dimensionless / (map_power(DenFrom{}) * ... * map_power(DensFrom{})),
                                                dimensionless / (res.equation * ... * map_power(DensTo{}))));
      }
    }
  }
}

template<typename... NumsFrom, typename... DensFrom>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<NumsFrom...>,
                                                                             type_list<DensFrom...>, type_list<>,
                                                                             type_list<>)
{
  return specs_convertible_result::yes;
}

template<typename... NumsTo, typename... DensTo>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<>, type_list<>,
                                                                             type_list<NumsTo...>, type_list<DensTo...>)
{
  return specs_convertible_result::explicit_conversion;
}

template<typename... NumsFrom>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<NumsFrom...>, type_list<>,
                                                                             type_list<>, type_list<>)
{
  return specs_convertible_result::yes;
}

template<typename... DensFrom>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<>, type_list<DensFrom...>,
                                                                             type_list<>, type_list<>)
{
  return specs_convertible_result::yes;
}

template<typename... NumsTo>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<>, type_list<>,
                                                                             type_list<NumsTo...>, type_list<>)
{
  return specs_convertible_result::explicit_conversion;
}

template<typename... DensTo>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<>, type_list<>, type_list<>,
                                                                             type_list<DensTo...>)
{
  return specs_convertible_result::explicit_conversion;
}

[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(type_list<>, type_list<>, type_list<>,
                                                                             type_list<>)
{
  return specs_convertible_result::yes;
}

template<DerivedQuantitySpec From, DerivedQuantitySpec To>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(From, To)
{
  return are_ingredients_convertible(type_list_sort<typename From::_num_, type_list_of_ingredients_less>{},
                                     type_list_sort<typename From::_den_, type_list_of_ingredients_less>{},
                                     type_list_sort<typename To::_num_, type_list_of_ingredients_less>{},
                                     type_list_sort<typename To::_den_, type_list_of_ingredients_less>{});
}

template<DerivedQuantitySpec From, NamedQuantitySpec To>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(From, To)
{
  return are_ingredients_convertible(type_list_sort<typename From::_num_, type_list_of_ingredients_less>{},
                                     type_list_sort<typename From::_den_, type_list_of_ingredients_less>{},
                                     type_list<To>{}, type_list<>{});
}

template<NamedQuantitySpec From, DerivedQuantitySpec To>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(From, To)
{
  return are_ingredients_convertible(type_list<From>{}, type_list<>{},
                                     type_list_sort<typename To::_num_, type_list_of_ingredients_less>{},
                                     type_list_sort<typename To::_den_, type_list_of_ingredients_less>{});
}

template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval specs_convertible_result convertible_kinds(From from_kind, To to_kind)
{
  constexpr auto exploded_kind_result = [](specs_convertible_result res) {
    using enum specs_convertible_result;
    return res == no ? no : yes;
  };
  if constexpr ((NamedQuantitySpec<decltype(From{})> && NamedQuantitySpec<decltype(To{})>) ||
                get_complexity(From{}) == get_complexity(To{}))
    return convertible_impl(from_kind, to_kind);
  else if constexpr (get_complexity(From{}) > get_complexity(To{}))
    return exploded_kind_result(
      convertible_impl(get_kind_tree_root(explode<get_complexity(To{})>(from_kind).quantity), to_kind));
  else
    return exploded_kind_result(
      convertible_impl(from_kind, get_kind_tree_root(explode<get_complexity(From{})>(to_kind).quantity)));
}

template<NamedQuantitySpec From, NamedQuantitySpec To>
[[nodiscard]] consteval specs_convertible_result convertible_named(From from, To to)
{
  using enum specs_convertible_result;

  if constexpr (have_common_base(From{}, To{})) {
    if constexpr (is_child_of(From{}, To{})) return yes;
    if constexpr (is_child_of(To{}, From{})) return explicit_conversion;
    if constexpr (get_kind(From{}) == get_kind(To{})) return cast;
    return no;
  } else if constexpr (get_kind(From{}) != get_kind(To{}))
    return no;
  else if constexpr (get_complexity(From{}) != get_complexity(To{})) {
    if constexpr (get_complexity(From{}) > get_complexity(To{}))
      return convertible_impl(explode<get_complexity(To{})>(from).quantity, to);
    else {
      auto res = explode<get_complexity(From{})>(to);
      return min(res.result, convertible_impl(from, res.quantity));
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
    return convertible_kinds(get_kind_tree_root(from), get_kind_tree_root(to));
  else if constexpr (NestedQuantityKindSpecOf<get_kind_tree_root(To{}), from> && get_kind_tree_root(To{}) == To{})
    return yes;
  else if constexpr (NamedQuantitySpec<From> && NamedQuantitySpec<To>)
    return convertible_named(from, to);
  else if constexpr (DerivedQuantitySpec<From> && DerivedQuantitySpec<To>)
    return are_ingredients_convertible(from, to);
  else if constexpr (DerivedQuantitySpec<From>) {
    auto res = explode<get_complexity(To{})>(from);
    if constexpr (NamedQuantitySpec<decltype(res.quantity)>)
      return convertible_impl(res.quantity, to);
    else if constexpr (requires { to._equation_; }) {
      auto eq = explode_to_equation(to);
      return min(eq.result, convertible_impl(res.quantity, eq.equation));
    } else
      return are_ingredients_convertible(from, to);
  } else if constexpr (DerivedQuantitySpec<To>) {
    auto res = explode<get_complexity(From{})>(to);
    if constexpr (NamedQuantitySpec<decltype(res.quantity)>)
      return min(res.result, convertible_impl(from, res.quantity));
    else if constexpr (requires { from._equation_; })
      return min(res.result, convertible_impl(from._equation_, res.quantity));
    else
      return min(res.result, are_ingredients_convertible(from, to));
  }
  // NOLINTEND(bugprone-branch-clone)
  return no;
}

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval bool implicitly_convertible(From from, To to)
{
  return detail::convertible_impl(from, to) == detail::specs_convertible_result::yes;
}

template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval bool explicitly_convertible(From from, To to)
{
  return detail::convertible_impl(from, to) >= detail::specs_convertible_result::explicit_conversion;
}

template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval bool castable(From from, To to)
{
  return detail::convertible_impl(from, to) >= detail::specs_convertible_result::cast;
}

template<QuantitySpec QS1, QuantitySpec QS2>
[[nodiscard]] consteval bool interconvertible(QS1 qs1, QS2 qs2)
{
  return implicitly_convertible(qs1, qs2) && implicitly_convertible(qs2, qs1);
}

MP_UNITS_EXPORT_END

namespace detail {

template<QuantitySpec Q>
  requires requires(Q q) { get_kind_tree_root(q); }
using to_kind = decltype(get_kind_tree_root(Q{}));

#if MP_UNITS_API_NO_CRTP
template<NamedQuantitySpec auto QS, auto... Args>
[[nodiscard]] consteval bool defined_as_kind(quantity_spec<QS, Args...>)
#else
template<typename Self, NamedQuantitySpec auto QS, auto... Args>
[[nodiscard]] consteval bool defined_as_kind(quantity_spec<Self, QS, Args...>)
#endif
{
  return contains<struct is_kind, Args...>();
}

template<QuantitySpec Q>
[[nodiscard]] consteval QuantitySpec auto get_kind_tree_root(Q q)
{
  auto defined_as_kind = []<typename QQ>(QQ qq) {
    if constexpr (requires { detail::defined_as_kind(qq); })
      return detail::defined_as_kind(QQ{});
    else
      return false;
  };

  // NOLINTBEGIN(bugprone-branch-clone)
  if constexpr (detail::QuantityKindSpec<Q>) {
    return remove_kind(q);
  } else if constexpr (defined_as_kind(Q{})) {
    return q;
  } else if constexpr (requires { Q::_parent_; }) {
    return get_kind_tree_root(Q::_parent_);
  } else if constexpr (detail::DerivedQuantitySpec<Q>) {
    return detail::expr_map<detail::to_kind, derived_quantity_spec, struct dimensionless,
                            detail::type_list_of_quantity_spec_less>(q);
  } else {
    // root quantity
    return q;
  }
  // NOLINTEND(bugprone-branch-clone)
}

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

template<QuantitySpec Q>
[[nodiscard]] consteval detail::QuantityKindSpec auto get_kind(Q)
{
  return kind_of<detail::get_kind_tree_root(Q{})>;
}

[[nodiscard]] consteval QuantitySpec auto common_quantity_spec(QuantitySpec auto q) { return q; }

template<QuantitySpec Q1, QuantitySpec Q2>
  requires detail::QuantitySpecConvertibleTo<detail::get_kind_tree_root(Q1{}), detail::get_kind_tree_root(Q2{})> ||
           detail::QuantitySpecConvertibleTo<detail::get_kind_tree_root(Q2{}), detail::get_kind_tree_root(Q1{})>
[[nodiscard]] consteval QuantitySpec auto common_quantity_spec(Q1 q1, Q2 q2)
{
  using QQ1 = decltype(detail::remove_kind(q1));
  using QQ2 = decltype(detail::remove_kind(q2));

  // NOLINTBEGIN(bugprone-branch-clone)
  if constexpr (is_same_v<Q1, Q2>)
    return q1;
  else if constexpr (detail::NestedQuantityKindSpecOf<Q1{}, Q2{}>)
    return detail::remove_kind(q1);
  else if constexpr (detail::NestedQuantityKindSpecOf<Q2{}, Q1{}>)
    return detail::remove_kind(q2);
  else if constexpr ((detail::QuantityKindSpec<Q1> && !detail::QuantityKindSpec<Q2>) ||
                     (detail::DerivedQuantitySpec<QQ1> && detail::NamedQuantitySpec<QQ2> &&
                      implicitly_convertible(Q1{}, Q2{})))
    return q2;
  else if constexpr ((!detail::QuantityKindSpec<Q1> && detail::QuantityKindSpec<Q2>) ||
                     (detail::NamedQuantitySpec<QQ1> && detail::DerivedQuantitySpec<QQ2> &&
                      implicitly_convertible(Q2{}, Q1{})))
    return q1;
  else if constexpr (detail::have_common_base(Q1{}, Q2{}))
    return detail::get_common_base(q1, q2);
  else if constexpr (implicitly_convertible(Q1{}, Q2{}))
    return q2;
  else if constexpr (implicitly_convertible(Q2{}, Q1{}))
    return q1;
  else if constexpr (implicitly_convertible(detail::get_kind_tree_root(Q1{}), detail::get_kind_tree_root(Q2{})))
    return detail::get_kind_tree_root(q2);
  else
    return detail::get_kind_tree_root(q1);
  // NOLINTEND(bugprone-branch-clone)
}

[[nodiscard]] consteval QuantitySpec auto common_quantity_spec(QuantitySpec auto q1, QuantitySpec auto q2,
                                                               QuantitySpec auto q3, QuantitySpec auto... rest)
  requires requires { common_quantity_spec(common_quantity_spec(q1, q2), q3, rest...); }
{
  return common_quantity_spec(common_quantity_spec(q1, q2), q3, rest...);
}

MP_UNITS_EXPORT_END

}  // namespace mp_units
