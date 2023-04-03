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

#include <mp_units/bits/algorithm.h>
#include <mp_units/bits/expression_template.h>
#include <mp_units/bits/external/type_name.h>
#include <mp_units/bits/external/type_traits.h>
#include <mp_units/bits/get_common_base.h>
#include <mp_units/bits/quantity_concepts.h>
#include <mp_units/bits/quantity_spec_concepts.h>
#include <mp_units/bits/reference_concepts.h>
#include <mp_units/bits/representation_concepts.h>
#include <mp_units/dimension.h>

namespace mp_units {

namespace detail {

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
  quantity_character num = common_quantity_character(quantity_character::scalar, expr_type<Qs1>::character...);
  quantity_character den = common_quantity_character(quantity_character::scalar, expr_type<Qs2>::character...);
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

#ifndef __cpp_explicit_this_parameter
template<typename Self>
#endif
struct quantity_spec_interface {
#ifdef __cpp_explicit_this_parameter
  template<typename Self, AssociatedUnit U>
  [[nodiscard]] consteval std::same_as<reference<Self{}, U{}>> auto operator[](this const Self, U u)
#else
  template<AssociatedUnit U>
  // TODO can we somehow return an explicit reference type here?
  [[nodiscard]] consteval std::same_as<reference<Self{}, U{}>> auto operator[](U u) const
#endif
    requires(Self::dimension == detail::get_associated_quantity(u).dimension)
  {
    return reference<Self{}, u>{};
  }

#ifdef __cpp_explicit_this_parameter
  template<typename Q>
    requires Quantity<std::remove_cvref_t<Q>> &&
             (explicitly_convertible_to(std::remove_cvref_t<Q>::quantity_spec, Self{}))
  [[nodiscard]] constexpr std::same_as<
    quantity<reference<Self{}, std::remove_cvref_t<Q>::unit>{}, typename Q::rep>> auto
  operator()(this const Self, Q&& q) const
#else
  template<typename Q>
    requires Quantity<std::remove_cvref_t<Q>> &&
             (explicitly_convertible_to(std::remove_cvref_t<Q>::quantity_spec, Self{}))
  // TODO can we somehow return an explicit quantity type here?
  [[nodiscard]] constexpr std::same_as<
    quantity<reference<Self{}, std::remove_cvref_t<Q>::unit>{}, typename std::remove_cvref_t<Q>::rep>> auto
  operator()(Q&& q) const
#endif
  {
    return std::forward<Q>(q).number() * reference<Self{}, std::remove_cvref_t<Q>::unit>{};
  }
};

}  // namespace detail

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
#ifdef __cpp_explicit_this_parameter
template<auto...>
#else
template<typename, auto...>
#endif
struct quantity_spec;

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
 * @tparam Args optionally a value of a `quantity_character` in case the base quantity should not be scalar
 */
#ifdef __cpp_explicit_this_parameter
template<BaseDimension auto Dim, one_of<quantity_character> auto... Args>
struct quantity_spec<Dim, Args...> : detail::quantity_spec_interface {
#else
template<typename Self, BaseDimension auto Dim, one_of<quantity_character> auto... Args>
struct quantity_spec<Self, Dim, Args...> : detail::quantity_spec_interface<Self> {
#endif
  static constexpr BaseDimension auto dimension = Dim;
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
 * inline constexpr struct area : quantity_spec<pow<2>(length)> {} area;
 * inline constexpr struct volume : quantity_spec<pow<3>(length)> {} volume;
 * inline constexpr struct velocity : quantity_spec<position_vector / duration> {} velocity;  // vector
 * inline constexpr struct speed : quantity_spec<distance / duration> {} speed;
 * inline constexpr struct force : quantity_spec<mass * acceleration, quantity_character::vector> {} force;
 * inline constexpr struct power : quantity_spec<force * velocity, quantity_character::scalar> {} power;
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
#ifdef __cpp_explicit_this_parameter
template<detail::IntermediateDerivedQuantitySpec auto Eq, one_of<quantity_character> auto... Args>
struct quantity_spec<Eq, Args...> : detail::quantity_spec_interface {
#else
template<typename Self, detail::IntermediateDerivedQuantitySpec auto Eq, auto... Args>
struct quantity_spec<Self, Eq, Args...> : detail::quantity_spec_interface<Self> {
#endif
  static constexpr auto _equation_ = Eq;
  static constexpr Dimension auto dimension = Eq.dimension;
  static constexpr quantity_character character = detail::quantity_character_init<Args...>(Eq.character);
};

/**
 * @brief Specialization defining a leaf quantity in the hierarchy
 *
 * Quantities of the same kind form a hierarchy. This specialization adds new leaf to such a tree which
 * can later be used as parent by other quantities.
 *
 * The character of those quantities by default is derived from the parent quantity.
 *
 * User should derive a strong type from this class template rather than use it directly in the source code.
 * For example:
 *
 * @code{.cpp}
 * inline constexpr struct width : quantity_spec<length> {} width;
 * inline constexpr struct height : quantity_spec<length> {} height;
 * inline constexpr struct diameter : quantity_spec<width> {} diameter;
 * inline constexpr struct position_vector : quantity_spec<length, quantity_character::vector> {} position_vector;
 * inline constexpr struct speed : quantity_spec<length / time> {} speed;
 * @endcode
 *
 * @note A common convention in this library is to assign the same name for a type and an object of this type.
 *       Besides defining them user never works with the types in the source code. All operations
 *       are done on the objects. Contrarily, the types are the only one visible in the compilation
 *       errors. Having them of the same names improves user experience and somehow blurs those separate domains.
 *
 * @tparam Q quantity specification of a parent quantity
 * @tparam Args optionally a value of a `quantity_character` in case the base quantity should not be scalar
 */
#ifdef __cpp_explicit_this_parameter
template<detail::NamedQuantitySpec auto Q, one_of<quantity_character> auto... Args>
struct quantity_spec<Q, Args...> : std::remove_const_t<decltype(Q)> {
#else
template<typename Self, detail::NamedQuantitySpec auto QS, auto... Args>
struct quantity_spec<Self, QS, Args...> : std::remove_const_t<decltype(QS)> {
#endif
  static constexpr auto _parent_ = QS;
  static constexpr quantity_character character = detail::quantity_character_init<Args...>(QS.character);

#ifndef __cpp_explicit_this_parameter
  template<AssociatedUnit U>
  [[nodiscard]] consteval std::same_as<reference<Self{}, U{}>> auto operator[](U u) const
    requires(Self::dimension == detail::get_associated_quantity(u).dimension)
  {
    return reference<Self{}, u>{};
  }

  template<typename Q>
    requires Quantity<std::remove_cvref_t<Q>> &&
             (explicitly_convertible_to(std::remove_cvref_t<Q>::quantity_spec, Self{}))
  [[nodiscard]] constexpr std::same_as<
    quantity<reference<Self{}, std::remove_cvref_t<Q>::unit>{}, typename std::remove_cvref_t<Q>::rep>> auto
  operator()(Q&& q) const
  {
    return std::forward<Q>(q).number() * reference<Self{}, std::remove_cvref_t<Q>::unit>{};
  }
#endif
};

/**
 * @brief Specialization defining a leaf derived quantity in the hierarchy and refining paren't equation
 *
 * Quantities of the same kind form a hierarchy. This specialization adds new leaf to such a tree which
 * can later be used as parent by other quantities. Additionally, this defintion adds additional
 * constraints on the derived quantity's equation.
 *
 * The character of those quantities by default is derived from the parent quantity.
 *
 * User should derive a strong type from this class template rather than use it directly in the source code.
 * For example:
 *
 * @code{.cpp}
 * inline constexpr struct velocity : quantity_spec<speed, position_vector / duration> {} velocity;
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
 * @tparam Args optionally a value of a `quantity_character` in case the base quantity should not be scalar
 */
#ifdef __cpp_explicit_this_parameter
template<detail::NamedQuantitySpec auto QS, detail::IntermediateDerivedQuantitySpec auto Eq,
         one_of<quantity_character> auto... Args>
  requires requires { QS._equation_; } && (implicitly_convertible_to(Eq, QS._equation_))
struct quantity_spec<QS, Eq, Args...> : quantity_spec<QS, Args...> {
#else
template<typename Self, detail::NamedQuantitySpec auto QS, detail::IntermediateDerivedQuantitySpec auto Eq,
         auto... Args>
  requires requires { QS._equation_; } && (implicitly_convertible_to(Eq, QS._equation_))
struct quantity_spec<Self, QS, Eq, Args...> : quantity_spec<Self, QS, Args...> {
#endif
  static constexpr auto _equation_ = Eq;
  static constexpr quantity_character character = detail::quantity_character_init<Args...>(Eq.character);
};

#ifdef __cpp_explicit_this_parameter

#define QUANTITY_SPEC(name, ...)                                            \
  inline constexpr struct name : ::mp_units::quantity_spec<##__VA_ARGS__> { \
  } name

#else

#define QUANTITY_SPEC(name, ...)                                                  \
  inline constexpr struct name : ::mp_units::quantity_spec<name, ##__VA_ARGS__> { \
  } name

#endif

/**
 * @brief Quantity kind specifier
 *
 * Specifies that the provided `Q` should be treated as a quantity kind.
 */
template<detail::QuantitySpecWithNoSpecifiers auto Q>
  requires(get_kind(Q) == Q)
#ifdef __cpp_explicit_this_parameter
struct kind_of_ : Q {
};
#else
struct kind_of_ : quantity_spec<kind_of_<Q>, Q> {
};
#endif

template<detail::QuantitySpecWithNoSpecifiers auto Q>
  requires(get_kind(Q) == Q)
inline constexpr kind_of_<Q> kind_of;

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
 * is different than `1` the dimension type is enclosed in `power<Dim, Num, Den>` class template. Otherwise, it is
 * just put directly in the list without any wrapper. In case all of the exponents are negative than the
 * `dimensionless`/`dimension_one` is put in the front to increase the readability.
 *
 * The character of those quantities is derived from ingredients or overriden with a template parameter.
 *
 * For example:
 *
 * @code{.cpp}
 * auto frequency = 1 / period_duration;
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
template<IntermediateDerivedQuantitySpecExpr... Expr>
struct derived_quantity_spec :
    detail::quantity_spec_interface<derived_quantity_spec<Expr...>>,
    detail::expr_fractions<detail::is_dimensionless, Expr...> {
  using _base_ = detail::expr_fractions<detail::is_dimensionless, Expr...>;

  static constexpr Dimension auto dimension =
    detail::expr_map<detail::to_dimension, derived_dimension, struct dimension_one,
                     detail::type_list_of_base_dimension_less>(_base_{});
  static constexpr quantity_character character =
    detail::derived_quantity_character(typename _base_::_num_{}, typename _base_::_num_{});
};

/**
 * @brief Quantity of dimension one
 *
 * Quantity of dimension one also commonly named as "dimensionless" is a quantity with a dimension
 * for which all the exponents of the factors corresponding to the base dimensions are zero.
 */
QUANTITY_SPEC(dimensionless, derived_quantity_spec<>{});

namespace detail {

template<>
struct is_dimensionless<struct dimensionless> : std::true_type {};

template<QuantitySpec auto... From, QuantitySpec Q>
[[nodiscard]] consteval QuantitySpec auto clone_kind_of(Q q)
{
  if constexpr ((... && QuantityKindSpec<std::remove_const_t<decltype(From)>>))
    return kind_of<Q{}>;
  else
    return q;
}

}  // namespace detail

// Operators

[[nodiscard]] consteval QuantitySpec auto operator*(QuantitySpec auto lhs, QuantitySpec auto rhs)
{
  return clone_kind_of<lhs, rhs>(
    detail::expr_multiply<derived_quantity_spec, struct dimensionless, detail::type_list_of_quantity_spec_less>(
      remove_kind(lhs), remove_kind(rhs)));
}

template<QuantitySpec Lhs, QuantitySpec Rhs>
[[nodiscard]] consteval QuantitySpec auto operator/(Lhs lhs, Rhs rhs)
{
  return clone_kind_of<lhs, rhs>(
    detail::expr_divide<derived_quantity_spec, struct dimensionless, detail::type_list_of_quantity_spec_less>(
      remove_kind(lhs), remove_kind(rhs)));
}

[[nodiscard]] consteval QuantitySpec auto operator/(int value, QuantitySpec auto q)
{
  gsl_Expects(value == 1);
  return clone_kind_of<q>(detail::expr_invert<derived_quantity_spec, struct dimensionless>(q));
}

[[nodiscard]] consteval QuantitySpec auto operator/(QuantitySpec auto, int) = delete;

template<QuantitySpec Lhs, QuantitySpec Rhs>
[[nodiscard]] consteval bool operator==(Lhs, Rhs)
{
  return is_same_v<Lhs, Rhs>;
}

template<QuantityKindSpec Lhs, QuantityKindSpec Rhs>
[[nodiscard]] consteval bool operator==(Lhs, Rhs)
{
  return is_same_v<Lhs, Rhs>;
}

template<QuantitySpec Lhs, QuantityKindSpec Rhs>
[[nodiscard]] consteval bool operator==(Lhs, Rhs rhs)
{
  return is_same_v<Lhs, std::remove_const_t<decltype(remove_kind(rhs))>>;
}


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
  if constexpr (q == dimensionless)
    return q;
  else if constexpr (detail::IntermediateDerivedQuantitySpec<Q>)
    return detail::clone_kind_of<Q{}>(
      detail::expr_pow<Num, Den, derived_quantity_spec, struct dimensionless, detail::type_list_of_quantity_spec_less>(
        remove_kind(q)));
  else if constexpr (Den == 1)
    return detail::clone_kind_of<Q{}>(
      derived_quantity_spec<power<std::remove_const_t<decltype(remove_kind(Q{}))>, Num>>{});
  else
    return detail::clone_kind_of<Q{}>(
      derived_quantity_spec<power<std::remove_const_t<decltype(remove_kind(Q{}))>, Num, Den>>{});
}

namespace detail {

enum class convertible_to_result { no, cast, explicit_conversion, yes };

template<QuantitySpec Q>
[[nodiscard]] consteval ratio get_complexity(Q);

template<typename... Ts>
[[nodiscard]] consteval ratio get_complexity(type_list<Ts...>)
{
  return (0 + ... + get_complexity(Ts{}));
}

template<QuantitySpec Q, int... Ints>
[[nodiscard]] consteval ratio get_complexity(power<Q, Ints...>)
{
  return get_complexity(Q{}) * power<Q, Ints...>::exponent;
}

template<QuantitySpec Q>
[[nodiscard]] consteval ratio get_complexity(Q)
{
  if constexpr (detail::IntermediateDerivedQuantitySpec<Q>)
    return get_complexity(typename Q::_num_{}) + get_complexity(typename Q::_den_{});
  else if constexpr (requires { Q::_equation_; })
    return 1 + get_complexity(Q::_equation_);
  else
    return 1;
}

template<QuantitySpec Lhs, QuantitySpec Rhs, bool lhs_eq = requires { Lhs::_equation_; },
         bool rhs_eq = requires { Rhs::_equation_; }, ratio lhs_compl = get_complexity(Lhs{}),
         ratio rhs_compl = get_complexity(Rhs{})>
struct ingredients_less :
    std::bool_constant<(lhs_eq > rhs_eq) || (lhs_eq == rhs_eq && lhs_compl > rhs_compl) ||
                       (lhs_eq == rhs_eq && lhs_compl == rhs_compl &&
                        type_name<std::remove_const_t<decltype(Lhs::dimension)>>() <
                          type_name<std::remove_const_t<decltype(Lhs::dimension)>>()) ||
                       (lhs_eq == rhs_eq && lhs_compl == rhs_compl &&
                        type_name<std::remove_const_t<decltype(Lhs::dimension)>>() ==
                          type_name<std::remove_const_t<decltype(Lhs::dimension)>>() &&
                        type_name<Lhs>() < type_name<Rhs>())> {};

template<typename T1, typename T2>
using type_list_of_ingredients_less = expr_less<T1, T2, ingredients_less>;

template<typename... Q1, typename... Q2>
[[nodiscard]] consteval convertible_to_result are_ingredients_convertible_to(derived_quantity_spec<Q1...>,
                                                                             derived_quantity_spec<Q2...>)
{
  // order ingredients by their complexity (number of base quantities involved in the definition)
  // if first ingredients are of different complexity extract the most complex one
  // repeat above until first ingredients will have the same complexity and dimension
  // check interconvertibility of quantities with the same dimension and continue to the nex one if successful
  // repeat until the end of the list or not interconvertible quantities are found
  return convertible_to_result::yes;
}

template<typename... Q1>
[[nodiscard]] consteval convertible_to_result are_ingredients_convertible_to(derived_quantity_spec<Q1...>,
                                                                             QuantitySpec auto)
{
  // order ingredients by their complexity (number of base quantities involved in the definition)
  // if first ingredients are of different complexity extract the most complex one
  // repeat above until first ingredients will have the same complexity and dimension
  // check interconvertibility of quantities with the same dimension and continue to the nex one if successful
  // repeat until the end of the list or not interconvertible quantities are found
  return convertible_to_result::yes;
}

template<typename... Q2>
[[nodiscard]] consteval convertible_to_result are_ingredients_convertible_to(QuantitySpec auto,
                                                                             derived_quantity_spec<Q2...>)
{
  // order ingredients by their complexity (number of base quantities involved in the definition)
  // if first ingredients are of different complexity extract the most complex one
  // repeat above until first ingredients will have the same complexity and dimension
  // check interconvertibility of quantities with the same dimension and continue to the nex one if successful
  // repeat until the end of the list or not interconvertible quantities are found
  return convertible_to_result::yes;
}

template<QuantitySpec Q>
[[nodiscard]] consteval auto get_equation(Q)
{
  return Q::_equation_;
}

template<QuantitySpec Q, int... Ints>
[[nodiscard]] consteval auto get_equation(power<Q, Ints...>)
{
  return pow<power<Q, Ints...>::exponent>(Q::_equation_);
}

template<ratio Complexity, IntermediateDerivedQuantitySpec Q>
[[nodiscard]] consteval auto explode(Q q);

template<ratio Complexity, NamedQuantitySpec Q>
[[nodiscard]] consteval auto explode(Q q);

template<ratio Complexity, QuantitySpec Q, typename Num, typename... Nums, typename Den, typename... Dens>
[[nodiscard]] consteval auto explode(Q q, type_list<Num, Nums...>, type_list<Den, Dens...>)
{
  constexpr auto n = get_complexity(Num{});
  constexpr auto d = get_complexity(Den{});
  constexpr auto max = n > d ? n : d;

  if constexpr (max <= Complexity)
    return q;
  else {
    if constexpr (n >= d)
      return explode<Complexity>((get_equation(Num{}) * ... * map_power(Nums{})) /
                                 (map_power(Den{}) * ... * map_power(Dens{})));
    else
      return explode<Complexity>((map_power(Num{}) * ... * map_power(Nums{})) /
                                 (get_equation(Den{}) * ... * map_power(Dens{})));
  }
}

template<ratio Complexity, QuantitySpec Q, typename Num, typename... Nums>
[[nodiscard]] consteval auto explode(Q q, type_list<Num, Nums...>, type_list<>)
{
  constexpr auto n = get_complexity(Num{});
  if constexpr (n <= Complexity)
    return q;
  else
    return explode<Complexity>((get_equation(Num{}) * ... * map_power(Nums{})));
}

template<ratio Complexity, QuantitySpec Q, typename Den, typename... Dens>
[[nodiscard]] consteval auto explode(Q q, type_list<>, type_list<Den, Dens...>)
{
  constexpr auto d = get_complexity(Den{});
  if constexpr (d <= Complexity)
    return q;
  else
    return explode<Complexity>(dimensionless / (get_equation(Den{}) * ... * map_power(Dens{})));
}

template<ratio Complexity, QuantitySpec Q>
[[nodiscard]] consteval auto explode(Q, type_list<>, type_list<>)
{
  return dimensionless;
}

template<ratio Complexity, NamedQuantitySpec Q>
[[nodiscard]] consteval auto explode(Q q)
{
  if constexpr (requires { Q::_equation_; })
    return explode<Complexity>(
      q, type_list_sort<typename decltype(Q::_equation_)::_num_, type_list_of_ingredients_less>{},
      type_list_sort<typename decltype(Q::_equation_)::_den_, type_list_of_ingredients_less>{});
  else
    return q;
}

template<ratio Complexity, IntermediateDerivedQuantitySpec Q>
[[nodiscard]] consteval auto explode(Q q)
{
  return explode<Complexity>(q, type_list_sort<typename Q::_num_, type_list_of_ingredients_less>{},
                             type_list_sort<typename Q::_den_, type_list_of_ingredients_less>{});
}

template<QuantitySpec Q1, QuantitySpec Q2>
[[nodiscard]] consteval convertible_to_result convertible_to_impl(Q1 q1, Q2 q2)
{
  using enum convertible_to_result;

  if constexpr (Q1::dimension != Q2::dimension)
    return no;
  else if constexpr (q1 == q2)
    return yes;
  else if constexpr (QuantityKindSpec<Q1> || QuantityKindSpec<Q2>) {
    if constexpr (IntermediateDerivedQuantitySpec<Q1> &&
                  NamedQuantitySpec<std::remove_const_t<decltype(remove_kind(q2))>>)
      return convertible_to_impl(get_kind(q1), remove_kind(q2));
    else
      return convertible_to_impl(get_kind(q1), get_kind(q2)) != no ? yes : no;
  } else if constexpr (IntermediateDerivedQuantitySpec<Q1> && IntermediateDerivedQuantitySpec<Q2>)
    return are_ingredients_convertible_to(q1, q2);
  else if constexpr (NamedQuantitySpec<Q1> && NamedQuantitySpec<Q2>) {
    if constexpr (have_common_base(q1, q2)) {
      if (std::derived_from<Q1, Q2>)
        return yes;
      else
        return std::derived_from<Q2, Q1> ? explicit_conversion : cast;
    }
  } else if constexpr (IntermediateDerivedQuantitySpec<Q1>) {
    auto q1_exploded = explode<get_complexity(q2)>(q1);
    if constexpr (NamedQuantitySpec<std::remove_const_t<decltype(q1_exploded)>>)
      return convertible_to_impl(q1_exploded, q2);
    else if constexpr (requires { q2._equation_; })
      return convertible_to_impl(q1_exploded, q2._equation_);
  } else if constexpr (IntermediateDerivedQuantitySpec<Q2>) {
    auto q2_exploded = explode<get_complexity(q1)>(q2);
    if constexpr (NamedQuantitySpec<std::remove_const_t<decltype(q2_exploded)>>)
      return convertible_to_impl(q2_exploded, q1);
    else if constexpr (requires { q1._equation_; })
      return std::min(explicit_conversion, convertible_to_impl(q1._equation_, q2_exploded));
  }
  return no;
}

}  // namespace detail

template<QuantitySpec Q1, QuantitySpec Q2>
[[nodiscard]] consteval bool implicitly_convertible_to(Q1 q1, Q2 q2)
{
  return detail::convertible_to_impl(q1, q2) == detail::convertible_to_result::yes;
}

template<QuantitySpec Q1, QuantitySpec Q2>
[[nodiscard]] consteval bool explicitly_convertible_to(Q1 q1, Q2 q2)
{
  return detail::convertible_to_impl(q1, q2) >= detail::convertible_to_result::explicit_conversion;
}

template<QuantitySpec Q1, QuantitySpec Q2>
[[nodiscard]] consteval bool castable_to(Q1 q1, Q2 q2)
{
  return detail::convertible_to_impl(q1, q2) >= detail::convertible_to_result::cast;
}

namespace detail {

template<QuantitySpec Q>
  requires requires(Q q) { get_kind(q); }
using to_kind = std::remove_const_t<decltype(get_kind(Q{}))>;

}

template<QuantitySpec Q>
[[nodiscard]] consteval auto remove_kind(Q q)
{
  if constexpr (QuantityKindSpec<Q>) {
    if constexpr (requires { Q::_parent_; })
      return Q::_parent_;
    else
      return Q::_equation_;
  } else
    return q;
}

template<QuantitySpec Q>
[[nodiscard]] consteval QuantitySpec auto get_kind(Q q)
{
  if constexpr (QuantityKindSpec<Q>) {
    return remove_kind(q);
  } else if constexpr (requires { Q::_parent_; }) {
    return get_kind(Q::_parent_);
  } else if constexpr (detail::IntermediateDerivedQuantitySpec<Q>) {
    return detail::expr_map<detail::to_kind, derived_quantity_spec, struct dimensionless,
                            detail::type_list_of_quantity_spec_less>(q);
  } else {
    // root quantity
    return q;
  }
}

[[nodiscard]] consteval auto common_quantity_spec(QuantitySpec auto q) { return q; }

template<QuantitySpec Q1, QuantitySpec Q2>
[[nodiscard]] consteval QuantitySpec auto common_quantity_spec(Q1 q1, Q2 q2)
  requires(implicitly_convertible_to(get_kind(q1), get_kind(q2)) ||
           implicitly_convertible_to(get_kind(q2), get_kind(q1)))
{
  if constexpr (q1 == q2)
    return q1;
  else if constexpr (detail::have_common_base(q1, q2))
    return detail::get_common_base(q1, q2);
  else if constexpr (implicitly_convertible_to(get_kind(q1), get_kind(q2)))
    return get_kind(q2);
  else
    return get_kind(q1);
}

[[nodiscard]] consteval QuantitySpec auto common_quantity_spec(QuantitySpec auto q1, QuantitySpec auto q2,
                                                               QuantitySpec auto q3, QuantitySpec auto... rest)
  requires requires { common_quantity_spec(common_quantity_spec(q1, q2), q3, rest...); }
{
  return common_quantity_spec(common_quantity_spec(q1, q2), q3, rest...);
}

}  // namespace mp_units
