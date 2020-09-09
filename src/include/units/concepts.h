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

#include <units/bits/external/downcasting.h>
#include <units/bits/external/fixed_string.h>
#include <units/bits/external/hacks.h>
#include <units/ratio.h>
#include <units/bits/external/type_traits.h>
#include <functional>
#include <cstdint>
#include <utility>

namespace units {

// PrefixFamily
struct prefix_family;

/**
 * @brief A concept matching a prefix family
 *
 * Satisfied by all types derived from `prefix_family`
 */
template<typename T>
concept PrefixFamily = std::derived_from<T, prefix_family>;

// Prefix
namespace detail {

template<PrefixFamily PF, ratio R>
struct prefix_base;

struct is_derived_from_prefix_base_impl {
  template<typename PF, ratio R>
  static constexpr std::true_type check_base(const prefix_base<PF, R>&);
  static constexpr std::false_type check_base(...);
};

template<typename T>
inline constexpr bool is_derived_from_prefix_base = decltype(is_derived_from_prefix_base_impl::check_base(std::declval<T>()))::value;

}  // namespace detail

/**
 * @brief A concept matching a symbol prefix
 *
 * Satisfied by all specializations of `prefix`.
 */
template<typename T>
concept Prefix = detail::is_derived_from_prefix_base<T>;

/**
 * @brief A concept matching unit's ratio
 *
 * Satisfied by all ratio values for which `R.num > 0` and `R.den > 0`.
 */
template<ratio R>
concept UnitRatio = R.num > 0 && R.den > 0;

// Unit
template<ratio R, typename U>
  requires UnitRatio<R>
struct scaled_unit;

// TODO: Remove when P1985 accepted
namespace detail {

template<ratio R, typename U>
void to_base_scaled_unit(const volatile scaled_unit<R, U>*);

}  // namespace detail

/**
 * @brief A concept matching all unit types in the library
 *
 * Satisfied by all unit types derived from an specialization of :class:`scaled_unit`.
 */
template<typename T>
concept Unit = requires { detail::to_base_scaled_unit(std::declval<const volatile T*>()); };

// BaseDimension
template<basic_fixed_string Symbol, Unit U>
  requires U::is_named
struct base_dimension;

namespace detail {

template<basic_fixed_string Symbol, typename U>
void to_base_base_dimension(const volatile base_dimension<Symbol, U>*);

}  // namespace detail

/**
 * @brief A concept matching all base dimensions in the library.
 *
 * Satisfied by all dimension types derived from an specialization of `base_dimension`.
 */
template<typename T>
concept BaseDimension = requires { detail::to_base_base_dimension(std::declval<const volatile T*>()); };

// Exponent
namespace detail {

template<typename T>
inline constexpr bool is_exponent = false;

}  // namespace detail

/**
 * @brief A concept matching dimension's exponents.
 *
 * Satisfied by all specializations of :class:`exponent`.
 */
template<typename T>
concept Exponent = detail::is_exponent<T>;

// DerivedDimension
namespace detail {

template<Exponent... Es>
  requires (BaseDimension<typename Es::dimension> && ...)
struct derived_dimension_base;

} // namespace detail

/**
 * @brief A concept matching all derived dimensions in the library.
 *
 * Satisfied by all dimension types derived from an specialization of `detail::derived_dimension_base`.
 */
template<typename T>
concept DerivedDimension = is_derived_from_specialization_of<T, detail::derived_dimension_base>;

// Dimension
/**
 * @brief A concept matching all dimensions in the library.
 *
 * Satisfied by all dimension types for which either `BaseDimension<T>` or `DerivedDimension<T>` is `true`.
 */
template<typename T>
concept Dimension = BaseDimension<T> || DerivedDimension<T>;

// UnitOf
namespace detail {

template<Dimension D>
auto default_unit()
{
  if constexpr (BaseDimension<D>)
    return typename D::base_unit{};
  else
    return typename D::coherent_unit{};
}

} // namespace detail

/**
 * @brief Returns a 'default' unit of the dimension
 *
 * Depending on the dimension type it returns a base unit (for base dimensions)
 * or a coherent unit (in case of derived dimensions).
 *
 * @tparam D Dimension type to get the unit from.
 */
template<Dimension D>
using dimension_unit = decltype(detail::default_unit<D>());
// TODO: replace with the below code when VS catches up
// decltype([]{
//     if constexpr (BaseDimension<D>)
//         return TYPENAME D::base_unit{};
//     else
//         return TYPENAME D::coherent_unit{};
// }());

/**
 * @brief A concept matching only units of a specified dimension.
 *
 * Satisfied by all unit types that satisfy `Unit<U>`, `Dimension<D>`, and for which
 * `U::reference` and @c dimension_unit<D>::reference denote the same unit type.
 *
 * @tparam U Type to verify.
 * @tparam D Dimension type to use for verification.
 */
template<typename U, typename D>
concept UnitOf =
  Unit<U> &&
  Dimension<D> &&
  std::same_as<typename U::reference, typename dimension_unit<D>::reference>;

// Quantity, QuantityPoint
namespace detail {

template<typename T>
inline constexpr bool is_quantity = false;

template<typename T>
inline constexpr bool is_quantity_point = false;

}  // namespace detail

/**
 * @brief A concept matching all quantities in the library.
 *
 * Satisfied by all specializations of :class:`quantity`.
 */
template<typename T>
concept Quantity = detail::is_quantity<T>;

/**
 * @brief A concept matching all quantity points in the library.
 *
 * Satisfied by all specializations of :class:`quantity_point`.
 */
template<typename T>
concept QuantityPoint = detail::is_quantity_point<T>;


// WrappedQuantity
namespace detail {

template<typename T>
inline constexpr bool is_wrapped_quantity = false;

template<typename T>
  requires requires { typename T::value_type; }
inline constexpr bool is_wrapped_quantity<T> = Quantity<typename T::value_type> || is_wrapped_quantity<typename T::value_type>;

}  // namespace detail

/**
 * @brief A concept matching types that wrap quantity objects.
 *
 * Satisfied by all wrapper types that satisfy `Quantity<typename T::value_type>`
 * recursively (i.e. `std::optional<si::length<si::metre>>`).
 */
template<typename T>
concept WrappedQuantity = detail::is_wrapped_quantity<T>;

// ScalableNumber

namespace detail {

template<typename T>
concept constructible_from_integral =
  // construction from an integral type
  std::constructible_from<T, std::int64_t> &&
  // unit scaling
  std::regular_invocable<std::multiplies<>, T, T> &&
  std::regular_invocable<std::divides<>, T, T>;

template<typename T>
concept not_constructible_from_integral =
  // not construction from an integral type
  (!std::constructible_from<T, std::int64_t>) &&

  // scaling by the value from ratio
  std::regular_invocable<std::multiplies<>, T, std::int64_t> &&
  std::regular_invocable<std::multiplies<>, std::int64_t, T>; // &&
  // std::regular_invocable<std::divides<>, T, std::int64_t>;  // TODO Uncomment when a bug in LA is fixed

}  // namespace detail

/**
 * @brief A concept matching non-Quantity types.
 *
 * Satisfied by types that satisfy `(!Quantity<T>) && (!WrappedQuantity<T>) && std::regular<T>`.
 */
template<typename T>
concept ScalableNumber =
  (!Quantity<T>) &&
  (!WrappedQuantity<T>) &&
  std::regular<T> &&
  (detail::constructible_from_integral<T> || detail::not_constructible_from_integral<T>);

}  // namespace units
