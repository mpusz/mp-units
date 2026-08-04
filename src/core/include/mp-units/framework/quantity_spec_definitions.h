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

// IWYU pragma: private, include <mp-units/framework/quantity_spec.h>
#include <mp-units/bits/hacks.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/bits/quantity_spec_hierarchy.h>
#include <mp-units/ext/algorithm.h>
#include <mp-units/ext/type_name.h>
#include <mp-units/ext/type_traits.h>
#include <mp-units/framework/dimension.h>
#include <mp-units/framework/quantity_concepts.h>
#include <mp-units/framework/quantity_point_concepts.h>
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/reference_concepts.h>
#include <mp-units/framework/representation_concepts.h>
#include <mp-units/framework/symbolic_expression.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#include <cstdint>
#include <type_traits>
#include <utility>
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
  QuantitySpec<T> && is_derived_from_specialization_of_quantity_spec<T> && !QuantityKindSpec<T>;

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


template<QuantitySpec QS, UnitOf<QS{}> U>
[[nodiscard]] consteval Reference auto make_reference(QS, U u)
{
  if constexpr (requires { requires(get_quantity_spec(U{}) == QS{}); })
    return u;
  else
    return reference<QS, U>{};
}

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
    detail::common_quantity_character(quantity_character{}, get_character(expr_type<Qs1>{})...);
  constexpr quantity_character den =
    detail::common_quantity_character(quantity_character{}, get_character(expr_type<Qs2>{})...);
  return detail::max(num, den);
}

/**
 * @brief Initializes quantity character
 *
 * If a quantity character value is present in template parameters, this value will be used.
 * Otherwise, an inherited/derived value provided through the function argument is returned.
 */
template<auto... Args>
[[nodiscard]] consteval quantity_character quantity_character_init(quantity_character inherited)
{
  quantity_character ch = inherited;
  // public compatibility spelling: `quantity_character::vector` etc. set both axes at once
  MP_UNITS_DIAGNOSTIC_PUSH
  MP_UNITS_DIAGNOSTIC_IGNORE_DEPRECATED
  if constexpr (mp_units::contains<quantity_character_legacy, Args...>())
    ch = mp_units::get<quantity_character_legacy, Args...>();
  MP_UNITS_DIAGNOSTIC_POP
  // two-axis spelling: each axis may be overridden independently, inheriting the other
  if constexpr (mp_units::contains<quantity_tensor_order, Args...>())
    ch.order = mp_units::get<quantity_tensor_order, Args...>();
  if constexpr (mp_units::contains<quantity_field, Args...>()) ch.field = mp_units::get<quantity_field, Args...>();
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
consteval bool quantity_spec_less_value()
{
  constexpr auto lhs_root_name = detail::type_name<LhsRoot>();
  constexpr auto rhs_root_name = detail::type_name<RhsRoot>();
  if (lhs_root_name != rhs_root_name) return lhs_root_name < rhs_root_name;
  constexpr auto lhs_kind_name = detail::type_name<LhsKindRoot>();
  constexpr auto rhs_kind_name = detail::type_name<RhsKindRoot>();
  if (lhs_kind_name != rhs_kind_name) return lhs_kind_name < rhs_kind_name;
  return detail::type_name<Lhs>() < detail::type_name<Rhs>();
}

template<NamedQuantitySpec Lhs, NamedQuantitySpec Rhs, NamedQuantitySpec LhsRoot, NamedQuantitySpec RhsRoot,
         NamedQuantitySpec LhsKindRoot, NamedQuantitySpec RhsKindRoot>
struct quantity_spec_less_impl :
    std::bool_constant<quantity_spec_less_value<Lhs, Rhs, LhsRoot, RhsRoot, LhsKindRoot, RhsKindRoot>()> {};

template<NamedQuantitySpec Lhs, NamedQuantitySpec Rhs>
struct quantity_spec_less :
    quantity_spec_less_impl<Lhs, Rhs, decltype(detail::get_hierarchy_root(Lhs{})),
                            decltype(detail::get_hierarchy_root(Rhs{})), decltype(detail::get_kind_tree_root(Lhs{})),
                            decltype(detail::get_kind_tree_root(Rhs{}))> {};

template<typename T1, typename T2>
using type_list_of_quantity_spec_less = expr_less<T1, T2, quantity_spec_less>;

struct quantity_spec_interface_base {
  template<QuantitySpec QS>
  [[nodiscard]] friend consteval Dimension auto get_dimension(QS qs)
  {
    return qs._dimension_;
  }

  template<QuantitySpec QS>
  [[nodiscard]] friend consteval quantity_character get_character(QS qs)
  {
    return qs._character_;
  }

  template<QuantitySpec Lhs, QuantitySpec Rhs>
  [[nodiscard]] friend consteval QuantitySpec auto operator*(Lhs lhs, Rhs rhs)
  {
    return detail::clone_kind_of<Lhs{}, Rhs{}>(
      detail::expr_multiply<derived_quantity_spec, dimensionless, type_list_of_quantity_spec_less>(
        detail::remove_kind(lhs), detail::remove_kind(rhs)));
  }

  template<QuantitySpec Lhs, QuantitySpec Rhs>
  [[nodiscard]] friend consteval QuantitySpec auto operator/(Lhs lhs, Rhs rhs)
  {
    return detail::clone_kind_of<Lhs{}, Rhs{}>(
      detail::expr_divide<derived_quantity_spec, dimensionless, type_list_of_quantity_spec_less>(
        detail::remove_kind(lhs), detail::remove_kind(rhs)));
  }

  template<QuantitySpec Lhs, QuantitySpec Rhs>
  [[nodiscard]] friend consteval bool operator==(Lhs, Rhs)
  {
    return std::is_same_v<Lhs, Rhs>;
  }

  // Clang <= 18 does not support default template arguments in friend function templates.
  // The two overloads below are a workaround. The intended library API (and the one that
  // should be proposed for ISO standardization) is a single function template:
  //
  //   template<std::intmax_t Num, std::intmax_t Den = 1, QuantitySpec Q>
  //     requires(Den != 0)
  //   [[nodiscard]] friend consteval QuantitySpec auto pow(Q q)
  //   {
  //     return detail::clone_kind_of<Q{}>(
  //       detail::expr_pow<Num, Den, derived_quantity_spec, struct dimensionless,
  //                        detail::type_list_of_quantity_spec_less>(detail::remove_kind(q)));
  //   }
  template<std::intmax_t Num, std::intmax_t Den, QuantitySpec Q>
    requires(Den != 0)
  [[nodiscard]] friend consteval QuantitySpec auto pow(Q q)
  {
    return detail::clone_kind_of<Q{}>(
      detail::expr_pow<Num, Den, derived_quantity_spec, struct dimensionless, detail::type_list_of_quantity_spec_less>(
        detail::remove_kind(q)));
  }

  template<std::intmax_t Num, QuantitySpec Q>
  [[nodiscard]] friend consteval QuantitySpec auto pow(Q q)
  {
    return detail::clone_kind_of<Q{}>(
      detail::expr_pow<Num, 1, derived_quantity_spec, struct dimensionless, detail::type_list_of_quantity_spec_less>(
        detail::remove_kind(q)));
  }

  [[nodiscard]] friend consteval QuantitySpec auto sqrt(QuantitySpec auto q) { return pow<1, 2>(q); }
  [[nodiscard]] friend consteval QuantitySpec auto cbrt(QuantitySpec auto q) { return pow<1, 3>(q); }
};

[[nodiscard]] consteval bool explicitly_convertible_to_dimensionless(QuantitySpec auto qs);
template<QuantitySpec auto QS, typename FwdValue>
[[nodiscard]] constexpr Quantity auto make_quantity_of_one(FwdValue&& val);

#if !MP_UNITS_API_NO_CRTP
template<typename Self>
#endif
struct quantity_spec_interface : quantity_spec_interface_base {
#if MP_UNITS_API_NO_CRTP
  template<typename Self, UnitOf<Self{}> U>
  [[nodiscard]] consteval Reference auto operator[](this Self, U)
  {
    return detail::make_reference(Self{}, U{});
  }

  template<typename Self, typename FwdQ, Quantity Q = std::remove_cvref_t<FwdQ>>
    requires(mp_units::explicitly_convertible(Q::quantity_spec, Self{})) &&
            requires { typename quantity<make_reference(Self{}, Q::unit), typename Q::rep>; }
  [[nodiscard]] constexpr Quantity auto operator()(this Self, FwdQ && q)
  {
    return quantity{std::forward<FwdQ>(q).numerical_value_is_an_implementation_detail_,
                    detail::make_reference(Self{}, Q::unit)};
  }

  template<typename Self, typename FwdQP, QuantityPoint QP = std::remove_cvref_t<FwdQP>>
    requires(mp_units::explicitly_convertible(QP::quantity_spec, Self{})) &&
            requires { typename quantity_point<make_reference(Self{}, QP::unit), QP::point_origin, typename QP::rep>; }
  [[nodiscard]] constexpr QuantityPoint auto operator()(this Self self, FwdQP && qp)
  {
    return quantity_point{self(std::forward<FwdQP>(qp).quantity_from_origin_is_an_implementation_detail_),
                          qp.point_origin};
  }

  template<typename Self, typename FwdValue, RepresentationOf<Self{}> Value = std::remove_cvref_t<FwdValue>>
    requires(detail::explicitly_convertible_to_dimensionless(Self{}))
  [[nodiscard]] constexpr Quantity auto operator()(this Self, FwdValue && val)
  {
    return make_quantity_of_one<Self{}>(std::forward<FwdValue>(val));
  }
#else
  template<typename Self_ = Self, UnitOf<Self_{}> U>
  [[nodiscard]] MP_UNITS_CONSTEVAL Reference auto operator[](U) const
  {
    return detail::make_reference(Self{}, U{});
  }

  template<typename FwdQ, Quantity Q = std::remove_cvref_t<FwdQ>, typename Self_ = Self>
    requires(mp_units::explicitly_convertible(Q::quantity_spec, Self_{})) &&
            requires { typename quantity<make_reference(Self{}, Q::unit), typename Q::rep>; }
  [[nodiscard]] constexpr Quantity auto operator()(FwdQ && q) const
  {
    return quantity{std::forward<FwdQ>(q).numerical_value_is_an_implementation_detail_,
                    detail::make_reference(Self{}, Q::unit)};
  }

  template<typename FwdQP, QuantityPoint QP = std::remove_cvref_t<FwdQP>, typename Self_ = Self>
    requires(mp_units::explicitly_convertible(QP::quantity_spec, Self_{})) &&
            requires { typename quantity_point<make_reference(Self{}, QP::unit), QP::point_origin, typename QP::rep>; }
  [[nodiscard]] constexpr QuantityPoint auto operator()(FwdQP && qp) const
  {
    return quantity_point{Self{}(std::forward<FwdQP>(qp).quantity_from_origin_is_an_implementation_detail_),
                          qp.point_origin};
  }

  template<typename FwdValue, typename Self_ = Self, RepresentationOf<Self_{}> Value = std::remove_cvref_t<FwdValue>>
    requires(detail::explicitly_convertible_to_dimensionless(Self_{}))
  [[nodiscard]] constexpr Quantity auto operator()(FwdValue && val) const
  {
    return make_quantity_of_one<Self{}>(std::forward<FwdValue>(val));
  }
#endif
};

template<typename T>
concept QSProperty = !QuantitySpec<T>;

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

inline constexpr struct is_kind {
} is_kind;

inline constexpr struct non_negative {
} non_negative;

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
 * Base quantities have real scalar character by default.
 *
 * User should derive a strong type from this class template rather than use it directly in the source code.
 * For example:
 *
 * @code{.cpp}
 * inline constexpr struct dim_length : base_dimension<"L"> {} dim_length;
 * inline constexpr struct dim_mass : base_dimension<"M"> {} dim_mass;
 * inline constexpr struct dim_time : base_dimension<"T"> {} dim_time;
 *
 * inline constexpr struct length : quantity_spec<dim_length> {} length;
 * inline constexpr struct mass : quantity_spec<dim_mass> {} mass;
 * inline constexpr struct time : quantity_spec<dim_time> {} time;
 * @endcode
 *
 * @note A common convention in this library is to assign the same name for a type and an object of this type.
 *       Besides defining them user never works with the types in the source code. All operations
 *       are done on the objects. Contrarily, the types are the only one visible in the compilation
 *       errors. Having them of the same names improves user experience and somehow blurs those separate domains.
 *
 * @tparam BaseDimension base dimension for which a base quantity is being defined
 * @tparam Args optionally a value of a `quantity_character` in case the base quantity should not be real scalar
 */
#if MP_UNITS_API_NO_CRTP
template<detail::BaseDimension auto Dim, detail::QSProperty auto... Args>
struct quantity_spec<Dim, Args...> : detail::quantity_spec_interface {
#else
template<typename Self, detail::BaseDimension auto Dim, detail::QSProperty auto... Args>
struct quantity_spec<Self, Dim, Args...> : detail::quantity_spec_interface<Self> {
#endif
  using _base_type_ = quantity_spec;
  static constexpr detail::BaseDimension auto _dimension_ = Dim;
  static constexpr quantity_character _character_ = detail::quantity_character_init<Args...>(quantity_character{});
  static_assert(!mp_units::contains<struct non_negative, Args...>() || _character_ == quantity_character{},
                "non_negative can only be applied to real scalar quantities");
  static constexpr bool _is_non_negative_ = mp_units::contains<struct non_negative, Args...>();

  [[deprecated("2.6.0: use `get_dimension(qs)` instead")]] static constexpr detail::BaseDimension auto dimension =
    _dimension_;
  [[deprecated("2.6.0: use `get_character(qs)` instead")]] static constexpr quantity_character character = _character_;
};

/**
 * @brief Specialization defining a named quantity being the result of a quantity calculus
 *
 * Derived quantity is a quantity, in a system of quantities, defined in terms of other quantities
 * of that system.
 *
 * This quantity serves as a root/kind for a new hierarchy of quantities of the same kind.
 *
 * Such quantities obtain the character from the derived quantity equation.
 *
 * User should derive a strong type from this class template rather than use it directly in the source code.
 * For example:
 *
 * @code{.cpp}
 * inline constexpr struct area : quantity_spec<pow<2>(length)> {} area;
 * inline constexpr struct volume : quantity_spec<pow<3>(length)> {} volume;
 * inline constexpr struct velocity : quantity_spec<displacement / duration> {} velocity;  // vector
 * inline constexpr struct force : quantity_spec<mass * acceleration> {} force;  // vector
 * @endcode
 *
 * @note A common convention in this library is to assign the same name for a type and an object of this type.
 *       Besides defining them user never works with the types in the source code. All operations
 *       are done on the objects. Contrarily, the types are the only one visible in the compilation
 *       errors. Having them of the same names improves user experience and somehow blurs those separate domains.
 *
 * @tparam Eq quantity equation specification of a derived quantity
 * @tparam Args optionally a value of a `quantity_character` in case the base quantity should not be real scalar
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
  static constexpr Dimension auto _dimension_ = get_dimension(Eq);

  static constexpr quantity_character _character_ = detail::quantity_character_init<Args...>(get_character(Eq));
  static_assert(!mp_units::contains<struct non_negative, Args...>() || _character_ == quantity_character{},
                "non_negative can only be applied to real scalar quantities");
  static constexpr bool _is_non_negative_ = mp_units::contains<struct non_negative, Args...>();

  [[deprecated("2.6.0: use `get_dimension(qs)` instead")]] static constexpr Dimension auto dimension = _dimension_;
  [[deprecated("2.6.0: use `get_character(qs)` instead")]] static constexpr quantity_character character = _character_;
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
 * The character of those quantities by default is derived from the parent quantity but can be overriden
 * by explicitly passing a property.
 *
 * User should derive a strong type from this class template rather than use it directly in the source code.
 * For example:
 *
 * @code{.cpp}
 * inline constexpr struct width : quantity_spec<length> {} width;
 * inline constexpr struct height : quantity_spec<length> {} height;
 * inline constexpr struct diameter : quantity_spec<width> {} diameter;
 * inline constexpr struct displacement : quantity_spec<length, quantity_tensor_order::vector> {} displacement;
 * inline constexpr struct voltage_phasor : quantity_spec<voltage, quantity_field::complex> {} voltage_phasor;
 * @endcode
 *
 * @note A common convention in this library is to assign the same name for a type and an object of this type.
 *       Besides defining them user never works with the types in the source code. All operations
 *       are done on the objects. Contrarily, the types are the only one visible in the compilation
 *       errors. Having them of the same names improves user experience and somehow blurs those separate domains.
 *
 * @tparam Q quantity specification of a parent quantity
 * @tparam Args optionally a value of a `quantity_character` in case the base quantity should not be real scalar
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
  static constexpr Dimension auto _dimension_ = get_dimension(_parent_);
  static constexpr quantity_character _character_ = detail::quantity_character_init<Args...>(get_character(QS));
  static_assert(!mp_units::contains<struct non_negative, Args...>() || _character_ == quantity_character{},
                "non_negative can only be applied to real scalar quantities");
  static constexpr bool _is_non_negative_ =
    mp_units::contains<struct non_negative, Args...>() || (_character_ == quantity_character{} && QS._is_non_negative_);

  [[deprecated("2.6.0: use `get_dimension(qs)` instead")]] static constexpr Dimension auto dimension = _dimension_;
  [[deprecated("2.6.0: use `get_character(qs)` instead")]] static constexpr quantity_character character = _character_;
};

// clang-format off
/**
 * @brief Specialization defining a leaf derived quantity in the hierarchy and refining paren't equation
 *
 * Quantities of the same kind form a hierarchy. This specialization adds new leaf to such a tree which
 * can later be used as a parent by other quantities. Additionally, this definition adds additional
 * constraints on the derived quantity's equation.
 *
 * Such quantities obtain the character from the derived quantity equation.
 *
 * User should derive a strong type from this class template rather than use it directly in the source code.
 * For example:
 *
 * @code{.cpp}
 * inline constexpr struct angular_measure : quantity_spec<dimensionless, arc_length / radius, is_kind> {} angular_measure;
 * inline constexpr struct velocity : quantity_spec<speed, displacement / duration> {} velocity;
 * inline constexpr struct weight : quantity_spec<force, mass * acceleration_of_free_fall> {} weight;
 * inline constexpr struct kinetic_energy : quantity_spec<mechanical_energy, mass * pow<2>(speed)> {} kinetic_energy;
 * @endcode
 *
 * @note A common convention in this library is to assign the same name for a type and an object of this type.
 *       Besides defining them user never works with the types in the source code. All operations
 *       are done on the objects. Contrarily, the types are the only one visible in the compilation
 *       errors. Having them of the same names improves user experience and somehow blurs those separate domains.
 *
 * @tparam Q quantity specification of a parent quantity
 * @tparam Args optionally a value of `quantity_character` in case the base quantity should not
 *              be real scalar or `is_kind` in case the quantity starts a new hierarchy tree of a kind
 */
// clang-format on
#if MP_UNITS_API_NO_CRTP
template<detail::NamedQuantitySpec auto QS, detail::DerivedQuantitySpec auto Eq, detail::QSProperty auto... Args>
  requires(mp_units::explicitly_convertible(Eq, QS))
struct quantity_spec<QS, Eq, Args...> : detail::quantity_spec_interface {
#else
template<typename Self, detail::NamedQuantitySpec auto QS, detail::DerivedQuantitySpec auto Eq,
         detail::QSProperty auto... Args>
  requires(mp_units::explicitly_convertible(Eq, QS))
struct quantity_spec<Self, QS, Eq, Args...> : detail::quantity_spec_interface<Self> {
#endif
  using _base_type_ = quantity_spec;
  static constexpr auto _parent_ = QS;
  static constexpr auto _equation_ = Eq;
  static constexpr Dimension auto _dimension_ = get_dimension(_parent_);

  static constexpr quantity_character _character_ = detail::quantity_character_init<Args...>(get_character(Eq));
  static_assert(!mp_units::contains<struct non_negative, Args...>() || _character_ == quantity_character{},
                "non_negative can only be applied to real scalar quantities");
  static constexpr bool _is_non_negative_ =
    mp_units::contains<struct non_negative, Args...>() || (_character_ == quantity_character{} && QS._is_non_negative_);

  [[deprecated("2.6.0: use `get_dimension(qs)` instead")]] static constexpr Dimension auto dimension = _dimension_;
  [[deprecated("2.6.0: use `get_character(qs)` instead")]] static constexpr quantity_character character = _character_;
};

namespace detail {

template<NamedQuantitySpec Q>
  requires requires { get_dimension(Q{}); }
using to_dimension = MP_UNITS_NONCONST_TYPE(get_dimension(Q{}));

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

  static constexpr Dimension auto _dimension_ =
    detail::expr_map<to_dimension, derived_dimension, struct dimension_one>(_base_{});
  static constexpr quantity_character _character_ =
    detail::derived_quantity_character(typename _base_::_num_{}, typename _base_::_den_{});
  static constexpr bool _is_non_negative_ = false;

  [[deprecated("2.6.0: use `get_dimension(qs)` instead")]] static constexpr Dimension auto dimension = _dimension_;
  [[deprecated("2.6.0: use `get_character(qs)` instead")]] static constexpr quantity_character character = _character_;
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
// TODO Restore `final` in V3
struct derived_quantity_spec : detail::derived_quantity_spec_impl<Expr...> {};

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
// TODO Restore `final` in V3
struct kind_of_<Q> : Q::_base_type_ {
#else
struct kind_of_<Q> : quantity_spec<kind_of_<Q>, Q{}>::_base_type_ {
#endif
  using _base_type_ = kind_of_;
  static constexpr auto _quantity_spec_ = Q{};
  // A kind encompasses the *entire* quantity tree — including vector quantities and signed
  // coordinates (e.g. displacement, altitude, depth) — so it can never be universally
  // non-negative, even when the tree root carries a non_negative tag.
  static constexpr bool _is_non_negative_ = false;
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

MP_UNITS_EXPORT [[nodiscard]] consteval QuantitySpec auto inverse(QuantitySpec auto q) { return dimensionless / q; }

}  // namespace mp_units
