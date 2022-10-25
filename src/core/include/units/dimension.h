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
#include <units/bits/external/type_traits.h>
#include <units/unit.h>

namespace units {

namespace detail {

template<typename T>
inline constexpr bool is_derived_dimension = false;

}

/**
 * @brief A concept matching all derived dimensions in the library.
 *
 * Satisfied by all dimension types either being a specialization of `derived_dimension`
 * or derived from it.
 */
template<typename T>
concept DerivedDimension = detail::is_derived_dimension<T>;

/**
 * @brief A concept matching all dimensions in the library.
 *
 * Satisfied by all dimension types for which either `BaseDimension<T>` or `DerivedDimension<T>` is `true`.
 */
template<typename T>
concept Dimension = BaseDimension<T> || DerivedDimension<T>;


namespace detail {

[[nodiscard]] consteval Dimension auto get_dimension_for(Unit auto);

}

template<Dimension D, Unit U>
struct reference;

/**
 * @brief A dimension of a base quantity
 *
 * Base quantity is a quantity in a conventionally chosen subset of a given system of quantities, where no quantity
 * in the subset can be expressed in terms of the other quantities within that subset. They are referred to as
 * being mutually independent since a base quantity cannot be expressed as a product of powers of the other base
 * quantities.
 *
 * Symbol template parameters is an unique identifier of the base dimension. The same identifiers can be multiplied
 * and divided which will result with an adjustment of its factor in an exponent of a derived_dimension
 * (in case of zero the dimension will be simplified and removed from further analysis of current expresion).
 *
 * User should derive a strong type from this class template rather than use it directly in the source code.
 * For example:
 *
 * @code{.cpp}
 * inline constexpr struct length : base_dimension<"L"> {} length;
 * inline constexpr struct time : base_dimension<"T"> {} time;
 * inline constexpr struct mass : base_dimension<"M"> {} mass;
 * @endcode
 *
 * @note A common convention in this library is to assign the same name for a type and an object of this type.
 *       Besides defining them user never works with the dimension types in the source code. All operations
 *       are done on the objects. Contrarily, the dimension types are the only one visible in the compilation
 *       errors. Having them of the same names improves user experience and somehow blurs those separate domains.
 *
 * @tparam Symbol an unique identifier of the base dimension used to provide dimensional analysis support
 */
#ifdef __cpp_explicit_this_parameter__
template<basic_fixed_string Symbol>
#else
template<typename Self, basic_fixed_string Symbol>
#endif
struct base_dimension {
  static constexpr auto symbol = Symbol;  ///< Unique base dimension identifier

#ifdef __cpp_explicit_this_parameter__
  template<Unit U, typename Self>
    requires(convertible(Self{}, detail::get_dimension_for(U{})))
  [[nodiscard]] constexpr reference<Self, U> operator[](this const Self, U)
#else
  template<Unit U>
    requires(convertible(Self{}, detail::get_dimension_for(U{})))
  [[nodiscard]] constexpr reference<Self, U> operator[](U) const
#endif
  {
    return {};
  }
};

namespace detail {

template<BaseDimension Lhs, BaseDimension Rhs>
struct base_dimension_less : std::bool_constant<(Lhs::symbol < Rhs::symbol)> {};

template<typename T1, typename T2>
using type_list_of_base_dimension_less = expr_less<T1, T2, base_dimension_less>;

template<typename T>
inline constexpr bool is_dimensionless = false;

template<typename T>
inline constexpr bool is_power_of_dim =
  requires {
    requires is_specialization_of_power<T> &&
               (BaseDimension<typename T::factor> || is_dimensionless<typename T::factor>);
  };

template<typename T>
inline constexpr bool is_per_of_dims = false;

template<typename... Ts>
inline constexpr bool is_per_of_dims<per<Ts...>> =
  (... && (BaseDimension<Ts> || is_dimensionless<Ts> || is_power_of_dim<Ts>));

}  // namespace detail

template<typename T>
concept DerivedDimensionSpec =
  BaseDimension<T> || detail::is_dimensionless<T> || detail::is_power_of_dim<T> || detail::is_per_of_dims<T>;

template<typename...>
struct derived_dimension;

namespace detail {

template<typename... Ds>
struct derived_dimension_impl : detail::expr_fractions<derived_dimension<>, Ds...> {
  using _type_ = derived_dimension<Ds...>;  // exposition only
};

}  // namespace detail


/**
 * @brief A dimension of a derived quantity
 *
 * Derived dimension is an expression of the dependence of a quantity on the base quantities of a system of quantities
 * as a product of powers of factors corresponding to the base quantities, omitting any numerical factors.
 *
 * Instead of using a raw list of exponents this library decided to use expression template syntax to make types
 * more digestable for the user. The positive exponents are ordered first and all negative exponents are put as a list
 * into the `per<...>` class template. If a power of exponent is different than `1` the dimension type is enclosed in
 * `power<Dim, Num, Den>` class template. Otherwise, it is just put directly in the list without any wrapper. There
 * is also one special case. In case all of the exponents are negative than the `dimensionless` being a dimension of
 * a dimensionless quantity is put in the front to increase the readability.
 *
 * For example:
 *
 * @code{.cpp}
 * DERIVED_DIMENSION(frequency, 1 / time);
 * DERIVED_DIMENSION(speed, length / time);
 * DERIVED_DIMENSION(acceleration, speed / time);
 * DERIVED_DIMENSION(force, mass * acceleration);
 * DERIVED_DIMENSION(energy, force * length);
 * DERIVED_DIMENSION(moment_of_force, length * force);
 * DERIVED_DIMENSION(torque, moment_of_force);
 * @endcode
 *
 * - `frequency` will be derived from type `derived_dimension<dimensionless, per<time>>`
 * - `speed` will be derived from type `derived_dimension<length, per<time>>`
 * - `acceleration` will be derived from type `derived_dimension<length, per<power<time, 2>>>`
 * - `force` will be derived from type `derived_dimension<length, mass, per<power<time, 2>>>`
 * - `energy` will be derived from type `derived_dimension<power<length, 2>, mass, per<power<time, 2>>>`
 *
 * @note A common convention in this library is to assign the same name for a type and an object of this type.
 *       Besides defining them user never works with the dimension types in the source code. All operations
 *       are done on the objects. Contrarily, the dimension types are the only one visible in the compilation
 *       errors. Having them of the same names improves user experience and somehow blurs those separate domains.
 *
 * Two dimensions are deemed equal when they are of the same type. With that strong type `speed` and
 * `derived_dimension<length, per<time>>` are considered not equal. They are convertible though.
 * User can implicitly convert up and down the inheritance hierarchy between those two.
 * `torque` and `moment_of_force` are convertible as well. However, `energy` and `torque`
 * are not convertible as they do not inherit from each other. They are from two separate branches of
 * dimensionally equivalent quantities.
 *
 * @tparam Ds a parameter pack consisting tokens allowed in the dimension specification
 *         (base dimensions, `dimensionless`, `power<Dim, Num, Den>`, `per<...>`)
 *
 * @note User should not instantiate this type! It is not exported from the C++ module. The library will
 *       instantiate this type automatically based on the dimensional arithmetic equation provided by the user.
 */
#ifdef __cpp_explicit_this_parameter__

template<DerivedDimensionSpec... Ds>
struct derived_dimension : detail::derived_dimension_impl<Ds...> {
  template<Unit U, typename Self>
    requires(convertible(Self{}, detail::get_dimension_for(U{})))
  [[nodiscard]] constexpr reference<Self, U> operator[](this const Self, U)
  {
    return {};
  }
};

#else

template<typename...>
struct derived_dimension;

template<DerivedDimensionSpec... Ds>
struct derived_dimension<Ds...> : detail::derived_dimension_impl<Ds...> {
  template<Unit U>
    requires(convertible(derived_dimension{}, detail::get_dimension_for(U{})))
  [[nodiscard]] constexpr reference<derived_dimension, U> operator[](U) const
  {
    return {};
  }
};

template<typename Self, DerivedDimension D>
struct derived_dimension<Self, D> : D {
  template<Unit U>
    requires(convertible(Self{}, detail::get_dimension_for(U{})))
  [[nodiscard]] constexpr reference<Self, U> operator[](U) const
  {
    return {};
  }
};

#endif

namespace detail {

template<typename... Ds>
void to_base_specialization_of_derived_dimension(const volatile derived_dimension<Ds...>*);

template<typename T>
inline constexpr bool is_derived_from_specialization_of_derived_dimension =
  requires(T * t) { to_base_specialization_of_derived_dimension(t); };

template<typename T>
  requires is_derived_from_specialization_of_derived_dimension<T>
inline constexpr bool is_derived_dimension<T> = true;

}  // namespace detail

/**
 * @brief Dimension one
 *
 * Dimension for which all the exponents of the factors corresponding to the base
 * dimensions are zero. Also commonly named as "dimensionless".
 */
inline constexpr struct dimensionless : derived_dimension<> {
} dimensionless;

namespace detail {

template<>
inline constexpr bool is_dimensionless<struct dimensionless> = true;

template<Dimension T>
struct dim_type_impl {
  using type = T;
};

template<DerivedDimension T>
struct dim_type_impl<T> {
  using type = T::_type_;
};

template<Dimension T>
using dim_type = dim_type_impl<T>::type;

}  // namespace detail


// Operators

template<Dimension Lhs, Dimension Rhs>
[[nodiscard]] consteval Dimension auto operator*(Lhs, Rhs)
{
  return detail::expr_multiply<derived_dimension, struct dimensionless, detail::type_list_of_base_dimension_less>(
    detail::dim_type<Lhs>{}, detail::dim_type<Rhs>{});
}

template<Dimension Lhs, Dimension Rhs>
[[nodiscard]] consteval Dimension auto operator/(Lhs, Rhs)
{
  return detail::expr_divide<derived_dimension, struct dimensionless, detail::type_list_of_base_dimension_less>(
    detail::dim_type<Lhs>{}, detail::dim_type<Rhs>{});
}

template<Dimension D>
[[nodiscard]] consteval Dimension auto operator/(int value, D)
{
  gsl_Expects(value == 1);
  return detail::expr_invert<derived_dimension, struct dimensionless>(detail::dim_type<D>{});
}

template<Dimension D>
[[nodiscard]] consteval Dimension auto operator/(D, int) = delete;

template<Dimension Lhs, Dimension Rhs>
[[nodiscard]] consteval bool operator==(Lhs, Rhs)
{
  return is_same_v<Lhs, Rhs>;
}

template<Dimension D1, Dimension D2>
[[nodiscard]] consteval bool convertible(D1, D2)
{
  return std::derived_from<D1, D2> || std::derived_from<D2, D1>;
}

/**
 * @brief Computes the value of a dimension raised to the `Num/Den` power
 *
 * @tparam Num Exponent numerator
 * @tparam Den Exponent denominator
 * @param d Dimension being the base of the operation
 *
 * @return Dimension The result of computation
 */
template<std::intmax_t Num, std::intmax_t Den = 1, Dimension D>
  requires detail::non_zero<Den>
[[nodiscard]] consteval Dimension auto pow(D d)
{
  if constexpr (BaseDimension<D>) {
    if constexpr (Den == 1)
      return derived_dimension<power<D, Num>>{};
    else
      return derived_dimension<power<D, Num, Den>>{};
  } else
    return detail::expr_pow<Num, Den, derived_dimension, struct dimensionless,
                            detail::type_list_of_base_dimension_less>(d);
}

namespace detail {

template<Unit U>
[[nodiscard]] consteval Dimension auto get_dimension_for_impl(U)
  requires requires { U::base_dimension; }
{
  return U::base_dimension;
}

template<Unit U>
  requires requires { U::base_dimension; }
using to_base_dimension = std::remove_const_t<decltype(U::base_dimension)>;

template<typename... Us>
[[nodiscard]] consteval Dimension auto get_dimension_for_impl(const derived_unit<Us...>& u)
{
  return detail::expr_map<to_base_dimension, derived_dimension, struct dimensionless,
                          detail::type_list_of_base_dimension_less>(u);
}

[[nodiscard]] consteval Dimension auto get_dimension_for(Unit auto u)
{
  return get_dimension_for_impl(get_canonical_unit(u).reference_unit);
}

}  // namespace detail


// TODO consider adding the support for text output of the dimensional equation

}  // namespace units

namespace std {

/**
 * @brief Partial specialization of `std::common_type` for dimensions
 *
 * Defined only for convertible types and returns the most derived/specific dimension type of
 * the two provided.
 */
template<units::Dimension D1, units::Dimension D2>
  requires(units::convertible(D1{}, D2{}))
struct common_type<D1, D2> {
  using type = ::units::conditional<std::derived_from<std::remove_const_t<D1>, std::remove_const_t<D2>>,
                                    std::remove_const_t<D1>, std::remove_const_t<D2>>;
};

}  // namespace std


#ifdef __cpp_explicit_this_parameter__

#define BASE_DIMENSION(name, symbol)                      \
  inline constexpr struct name : base_dimension<symbol> { \
  } name

#define DERIVED_DIMENSION(name, base)   \
  inline constexpr struct name : base { \
  } name

#else

#define BASE_DIMENSION(name, symbol)                            \
  inline constexpr struct name : base_dimension<name, symbol> { \
  } name

#define DERIVED_DIMENSION(name, base)                            \
  inline constexpr struct name : derived_dimension<name, base> { \
  } name

#endif
