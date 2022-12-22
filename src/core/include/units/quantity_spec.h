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

#include <units/bits/algorithm.h>
#include <units/bits/expression_template.h>
#include <units/bits/external/type_name.h>
#include <units/bits/external/type_traits.h>
#include <units/dimension.h>
#include <units/unit.h>
#include <tuple>

namespace units {

/**
 * @brief Quantity character
 *
 * Scalars, vectors and tensors are mathematical objects that can be used to
 * denote certain physical quantities and their values. They are as such
 * independent of the particular choice of a coordinate system, whereas
 * each scalar component of a vector or a tensor and each component vector and
 * component tensor depend on that choice.
 *
 * A scalar is a physical quantity that has magnitude but no direction.
 *
 * Vectors are physical quantities that possess both magnitude and direction
 * and whose operations obey the axioms of a vector space.
 *
 * Tensors can be used to describe more general physical quantities.
 * For example, the Cauchy stress tensor possess magnitude, direction,
 * and orientation qualities.
 */
enum class quantity_character { scalar, vector, tensor };

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
  return detail::max({args...});
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
  if constexpr (one_of<quantity_character, std::remove_const_t<decltype(Args)>...>)
    return std::get<quantity_character>(std::make_tuple(Args...));
  else
    return ch;
}

template<typename T>
inline constexpr bool is_specialization_of_derived_quantity_spec = false;

template<typename T>
inline constexpr bool is_dimensionless = false;

template<typename T>
inline constexpr bool is_power_of_quantity_spec = requires {
  requires is_specialization_of_power<T> &&
             (NamedQuantitySpec<typename T::factor> || is_dimensionless<typename T::factor>);
};

template<typename T>
inline constexpr bool is_per_of_quantity_specs = false;

template<typename... Ts>
inline constexpr bool is_per_of_quantity_specs<per<Ts...>> =
  (... && (NamedQuantitySpec<Ts> || is_dimensionless<Ts> || is_power_of_quantity_spec<Ts>));

}  // namespace detail

/**
 * @brief Concept matching quantity specification types
 *
 * Satisfied by all `derived_quantity_spec` specializations.
 */
template<typename T>
concept DerivedQuantitySpec = detail::is_specialization_of_derived_quantity_spec<T>;

template<typename T>
concept QuantitySpec = NamedQuantitySpec<T> || DerivedQuantitySpec<T>;

template<typename T>
concept DerivedQuantitySpecExpr = NamedQuantitySpec<T> || detail::is_dimensionless<T> ||
                                  detail::is_power_of_quantity_spec<T> || detail::is_per_of_quantity_specs<T>;

namespace detail {

template<NamedQuantitySpec Lhs, NamedQuantitySpec Rhs>
struct quantity_spec_less : std::bool_constant<type_name<Lhs>() < type_name<Rhs>()> {};

template<typename T1, typename T2>
using type_list_of_quantity_spec_less = expr_less<T1, T2, quantity_spec_less>;

template<NamedQuantitySpec Q>
  requires requires { Q::dimension; }
using to_dimension = std::remove_const_t<decltype(Q::dimension)>;

template<Unit U>
  requires requires { U::base_quantity.dimension; }
using to_base_dimension = std::remove_const_t<decltype(U::base_quantity.dimension)>;

template<Unit U>
[[nodiscard]] consteval Dimension auto get_dimension_for_impl(U)
  requires requires { U::base_quantity.dimension; }
{
  return U::base_quantity.dimension;
}

template<typename... Us>
[[nodiscard]] consteval Dimension auto get_dimension_for_impl(const derived_unit<Us...>& u)
  requires detail::expr_projectable<derived_unit<Us...>, to_base_dimension>
{
  return detail::expr_map<to_base_dimension, derived_dimension, struct dimension_one,
                          detail::type_list_of_base_dimension_less>(u);
}

template<typename U>
concept associated_unit = Unit<U> && requires(U u) { get_dimension_for_impl(get_canonical_unit(u).reference_unit); };

[[nodiscard]] consteval Dimension auto get_dimension_for(associated_unit auto u)
{
  return get_dimension_for_impl(get_canonical_unit(u).reference_unit);
}

}  // namespace detail

template<QuantitySpec auto Q, Unit auto U>
struct reference;

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
 * @note A common convention in this library is to assign the same name for a type and an object of this type.
 *       Besides defining them user never works with the dimension types in the source code. All operations
 *       are done on the objects. Contrarily, the dimension types are the only one visible in the compilation
 *       errors. Having them of the same names improves user experience and somehow blurs those separate domains.
 *
 * Derived quantity specification will have a character being the strongest ones from its ingredients.
 *
 * Binding a proper unit to a quantity specification via an indexing operator (`operator[]`) results
 * in a quantity reference.
 *
 * @tparam Qs a parameter pack consisting tokens allowed in the quantity specification
 *         (named quantity specification, `dimensionless`, `power<Q, Num, Den>`, `per<...>`)
 *
 * @note User should not instantiate this type! It is not exported from the C++ module. The library will
 *       instantiate this type automatically based on the dimensional arithmetic equation provided by the user.
 */
template<DerivedQuantitySpecExpr... Qs>
struct derived_quantity_spec : detail::expr_fractions<derived_quantity_spec<>, Qs...> {
  using base = detail::expr_fractions<derived_quantity_spec<>, Qs...>;

  static constexpr Dimension auto dimension =
    detail::expr_map<detail::to_dimension, derived_dimension, struct dimension_one,
                     detail::type_list_of_base_dimension_less>(base{});
  static constexpr quantity_character character =
    detail::derived_quantity_character(typename base::_num_{}, typename base::_num_{});

#ifdef __cpp_explicit_this_parameter
  template<typename Self, Unit U>
  [[nodiscard]] consteval Reference auto operator[](this const Self, U u)
    requires(dimension == detail::get_dimension_for(u))
  {
    return reference<Self{}, u>{};
  }
#else
  // TODO can we somehow return an explicit reference type here?
  template<Unit U>
  [[nodiscard]] consteval Reference auto operator[](U u) const
    requires(dimension == detail::get_dimension_for(u))
  {
    return reference<derived_quantity_spec{}, u>{};
  }
#endif
};

namespace detail {

template<typename... Args>
inline constexpr bool is_specialization_of_derived_quantity_spec<derived_quantity_spec<Args...>> = true;

}

/**
 * @brief Quantity Specification
 *
 * This type specifies all the properties of a quantity and allow modeling most of the quantities in the ISO 80000.
 * It serves to define base and derived quantities as well as quantity kinds. Each quantity specification
 * provides an information on how this quantity relates to other quantities, specifies its dimension and character.
 *
 * Quantity character can be derived from other quantities or explicitly overriden through a template parameter.
 *
 * Binding a proper unit to a quantity specification via an indexing operator (`operator[]`) results
 * in a quantity reference.
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
 *       Besides defining them user never works with the dimension types in the source code. All operations
 *       are done on the objects. Contrarily, the dimension types are the only one visible in the compilation
 *       errors. Having them of the same names improves user experience and somehow blurs those separate domains.
 *
 * @tparam BaseDimension base dimension for which a base quantity is being defined
 * @tparam Args optionally a value of a `quantity_character` in case the base quantity should not be scalar
 */
#ifdef __cpp_explicit_this_parameter
template<BaseDimension auto Dim, one_of<quantity_character> auto... Args>
struct quantity_spec<Dim, Args...> {
#else
template<typename Self, BaseDimension auto Dim, one_of<quantity_character> auto... Args>
struct quantity_spec<Self, Dim, Args...> {
#endif
  static constexpr BaseDimension auto dimension = Dim;
  static constexpr quantity_character character = detail::quantity_character_init<Args...>(quantity_character::scalar);

#ifdef __cpp_explicit_this_parameter
  template<typename Self, Unit U>
  [[nodiscard]] consteval Reference auto operator[](this const Self, U u)
    requires(dimension == detail::get_dimension_for(u))
#else
  template<Unit U>
  // TODO can we somehow return an explicit reference type here?
  [[nodiscard]] consteval Reference auto operator[](U u) const
    requires(dimension == detail::get_dimension_for(u))
#endif
  {
    return reference<Self{}, u>{};
  }
};

/**
 * @brief Specialization defining a named derived quantity or a quantity kind
 *
 * The division of the concept "quantity" into several kinds is to some extent arbitrary.
 * For example the quantities diameter, circumference, and wavelength are generally considered
 * to be quantities of the same kind, namely, of the kind of quantity called length.
 *
 * Quantities of the same kind within a given system of quantities have the same quantity dimension.
 * However, quantities of the same dimension are not necessarily of the same kind.
 *
 * The character of those quantities is derived from ingredients or overriden with a template parameter.
 *
 * User should derive a strong type from this class template rather than use it directly in the source code.
 * For example:
 *
 * @code{.cpp}
 * // quantity kinds
 * inline constexpr struct width : quantity_spec<length> {} width;
 * inline constexpr struct height : quantity_spec<length> {} height;
 * inline constexpr struct thickness : quantity_spec<width> {} thickness;
 * inline constexpr struct diameter : quantity_spec<width> {} diameter;
 * inline constexpr struct position_vector : quantity_spec<length, quantity_character::vector> {} position_vector;
 *
 * // derived quantities
 * inline constexpr struct area : quantity_spec<pow<2>(length)> {} area;
 * inline constexpr struct volume : quantity_spec<pow<3>(length)> {} volume;
 * inline constexpr struct velocity : quantity_spec<position_vector / duration> {} velocity;  // vector
 * inline constexpr struct speed : quantity_spec<distance / duration> {} speed;
 * inline constexpr struct force : quantity_spec<mass * acceleration, quantity_character::vector> {} force;
 * inline constexpr struct power : quantity_spec<force * velocity, quantity_character::scalar> {} power;
 * @endcode
 *
 * Two quantity specifications are deemed equal when they are of the same type. With that, both strong
 * types `speed` and `velocity` are considered not equal to `derived_dimension<length, per<time>>` or
 * to each other. However, they are both convertible to `derived_dimension<length, per<time>>`.
 * User can implicitly convert up and down the inheritance hierarchy between them. However, `speed` and
 * `velocity` are not convertible as they do not inherit from each other. They are from two separate branches
 * of dimensionally equivalent quantities.
 *
 * @tparam Q quantity specification of a parent quantity
 * @tparam Args optionally a value of a `quantity_character` in case the base quantity should not be scalar
 */
#ifdef __cpp_explicit_this_parameter
template<QuantitySpec auto Q, one_of<quantity_character> auto... Args>
struct quantity_spec<Q, Args...> : std::remove_const_t<decltype(Q)> {
#else
template<typename Self, QuantitySpec auto Q, one_of<quantity_character> auto... Args>
struct quantity_spec<Self, Q, Args...> : std::remove_const_t<decltype(Q)> {
#endif
  static constexpr auto kind_of = Q;
  static constexpr quantity_character character = detail::quantity_character_init<Args...>(Q.character);

#ifndef __cpp_explicit_this_parameter
  template<Unit U>
  // TODO can we somehow return an explicit reference type here?
  [[nodiscard]] consteval Reference auto operator[](U u) const
    requires(this->dimension == detail::get_dimension_for(u))
  {
    return reference<Self{}, u>{};
  }
#endif
};

#ifdef __cpp_explicit_this_parameter

#define QUANTITY_SPEC(name, ...)                                \
  inline constexpr struct name : quantity_spec<##__VA_ARGS__> { \
  } name

#else

#define QUANTITY_SPEC(name, ...)                                      \
  inline constexpr struct name : quantity_spec<name, ##__VA_ARGS__> { \
  } name

#endif

// TODO Should a quantity_cast be allowed to cast between speed and velocity?


/**
 * @brief Quantity of dimension one
 *
 * Quantity of dimension one also commonly named as "dimensionless" is a quantity with a dimension
 * for which all the exponents of the factors corresponding to the base dimensions are zero.
 */
QUANTITY_SPEC(dimensionless, derived_quantity_spec<>{});

namespace detail {

template<>
inline constexpr bool is_dimensionless<struct dimensionless> = true;

}  // namespace detail


// Operators

template<QuantitySpec Lhs, QuantitySpec Rhs>
[[nodiscard]] consteval QuantitySpec auto operator*(Lhs lhs, Rhs rhs)
{
  return detail::expr_multiply<derived_quantity_spec, struct dimensionless, detail::type_list_of_quantity_spec_less>(
    lhs, rhs);
}

template<QuantitySpec Lhs, QuantitySpec Rhs>
[[nodiscard]] consteval QuantitySpec auto operator/(Lhs lhs, Rhs rhs)
{
  return detail::expr_divide<derived_quantity_spec, struct dimensionless, detail::type_list_of_quantity_spec_less>(lhs,
                                                                                                                   rhs);
}

template<QuantitySpec Q>
[[nodiscard]] consteval QuantitySpec auto operator/(int value, Q q)
{
  gsl_Expects(value == 1);
  return detail::expr_invert<derived_quantity_spec, struct dimensionless>(q);
}

template<QuantitySpec Q>
[[nodiscard]] consteval QuantitySpec auto operator/(Q, int) = delete;

template<QuantitySpec Lhs, QuantitySpec Rhs>
[[nodiscard]] consteval bool operator==(Lhs, Rhs)
{
  return is_same_v<Lhs, Rhs>;
}

template<QuantitySpec Q1, QuantitySpec Q2>
[[nodiscard]] consteval bool interconvertible(Q1, Q2)
{
  if constexpr (NamedQuantitySpec<Q1> && NamedQuantitySpec<Q2>)
    // check if quantity kinds are convertible (across one hierarchy)
    return std::derived_from<Q1, Q2> || std::derived_from<Q2, Q1>;
  else {
    // check weather the quantity spec's dimension is the same
    // TODO Can we improve that to account for quantity kinds (i.e. `altitude` / `time` -> `sink_rate`)
    return Q1::dimension == Q2::dimension;
  }
}

[[nodiscard]] consteval auto common_quantity_spec(QuantitySpec auto q) { return q; }

template<QuantitySpec Q1, QuantitySpec Q2>
[[nodiscard]] consteval auto common_quantity_spec(Q1 q1, Q2 q2)
  requires(interconvertible(q1, q2))
{
  if constexpr (std::derived_from<Q1, Q2>)
    return q1;
  else if constexpr (std::derived_from<Q2, Q1>)
    return q2;
  else if constexpr (NamedQuantitySpec<Q1>)
    return q1;
  else
    return q2;
}

[[nodiscard]] consteval auto common_quantity_spec(QuantitySpec auto q1, QuantitySpec auto q2, QuantitySpec auto q3,
                                                  QuantitySpec auto... rest)
  requires requires { common_quantity_spec(common_quantity_spec(q1, q2), q3, rest...); }
{
  return common_quantity_spec(common_quantity_spec(q1, q2), q3, rest...);
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
  if constexpr (BaseQuantitySpec<Q>) {
    if constexpr (Den == 1)
      return derived_quantity_spec<power<Q, Num>>{};
    else
      return derived_quantity_spec<power<Q, Num, Den>>{};
  } else
    return detail::expr_pow<Num, Den, derived_quantity_spec, struct dimensionless,
                            detail::type_list_of_quantity_spec_less>(q);
}

}  // namespace units
