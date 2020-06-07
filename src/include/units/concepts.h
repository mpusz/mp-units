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
#include <units/bits/external/type_traits.h>

namespace units {

// in_prefix_family
struct prefix_family;

/**
 * @brief A concept matching a prefix family
 * 
 * Satisfied by all types derived from `prefix_family`
 */
template<typename T>
concept in_prefix_family = std::derived_from<T, prefix_family>;

// in_prefix
// TODO gcc:92150
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=92150
// namespace detail {

//   template<typename T>
//   inline constexpr bool is_prefix = false;

//   template<typename in_prefix_family, in_ratio R, basic_fixed_string Symbol>
//   inline constexpr bool is_prefix<prefix<in_prefix_family, R, Symbol>> = true;

// }  // namespace detail

/**
 * @brief A concept matching a symbol prefix
 * 
 * Satisfied by all instantiations of `prefix`.
 */
template<typename T>
//  concept in_prefix = detail::is_prefix<T>;
concept in_prefix = true;


namespace detail {

template<typename T>
inline constexpr bool is_ratio = false;

} // namespace detail

/**
 * @brief A concept matching a ratio
 * 
 * Satisfied by all instantiations of `ratio`.
 */
template<typename T>
concept in_ratio = detail::is_ratio<T>;

/**
 * @brief A concept matching unit's ratio
 * 
 * Satisfied by all types that satisfy `in_ratio<R>` and for which `R::num > 0` and `R::den > 0`
 */
template<typename R>
concept in_unit_ratio = in_ratio<R> && R::num > 0 && R::den > 0; // double negatives not allowed

// in_unit
template<in_unit_ratio R, typename U>
struct scaled_unit;

/**
 * @brief A concept matching all unit types in the library
 * 
 * Satisfied by all unit types derived from the instantiation of :class:`scaled_unit`.
 */
template<typename T>
concept in_unit = is_derived_from_instantiation<T, scaled_unit>;

template<basic_fixed_string Symbol, in_unit U>
  requires U::is_named
struct base_dimension;

namespace detail {

#if __GNUC__ == 9 && __GNUC_MINOR__ < 2

template<typename T>
inline constexpr bool is_base_dimension = true;

#else

template<typename T>
inline constexpr bool is_base_dimension = false;

template<basic_fixed_string Name, typename... Params>
inline constexpr bool is_base_dimension<base_dimension<Name, Params...>> = true;

#endif

}  // namespace detail

/**
 * @brief A concept matching all base dimensions in the library.
 * 
 * Satisfied by all dimension types derived from the instantiation of `base_dimension`.
 */
template<typename T>
concept in_base_dimension = detail::is_base_dimension<typename T::base_type_workaround>;

// in_exponent
namespace detail {

template<typename T>
inline constexpr bool is_exp = false;

}  // namespace detail

/**
 * @brief A concept matching dimension's exponents.
 * 
 * Satisfied by all instantiations of :class:`exp`.
 */
template<typename T>
concept in_exponent = detail::is_exp<T>;

// in_derived_dimension
namespace detail {

template<in_exponent E, in_exponent... ERest>
  requires (in_base_dimension<typename E::dimension> && ... && in_base_dimension<typename ERest::dimension>)
struct derived_dimension_base;

} // namespace detail

/**
 * @brief A concept matching all derived dimensions in the library.
 * 
 * Satisfied by all dimension types derived from the instantiation of `detail::derived_dimension_base`.
 */
template<typename T>
concept in_derived_dimension = is_instantiation<downcast_base_t<T>, detail::derived_dimension_base>;

// in_dimension
/**
 * @brief A concept matching all dimensions in the library. 
 * 
 * Satisfied by all dimension types for which either `in_base_dimension<T>` or `in_derived_dimension<T>` is `true`.
 */
template<typename T>
concept in_dimension = in_base_dimension<T> || in_derived_dimension<T>;

// UnitOf
namespace detail {

template<in_dimension D>
struct dimension_unit_impl;

template<in_base_dimension D>
struct dimension_unit_impl<D> {
  using type = D::base_unit;
};

template<in_derived_dimension D>
struct dimension_unit_impl<D> {
  using type = D::coherent_unit;
};

} // namespace detail

/**
 * @brief Returns a 'default' unit of the dimension
 * 
 * Depending on the dimension type it returns a base unit (for base dimensions)
 * or a coherent unit (in case of derived dimensions).
 * 
 * @tparam D in_dimension type to get the unit from.
 */
template<in_dimension D>
using dimension_unit = detail::dimension_unit_impl<D>::type;

/**
 * @brief A concept matching only units of a specified dimension.
 * 
 * Satisfied by all unit types that satisfy `in_unit<U>`, `in_dimension<D>`, and for which
 * `U::reference` and @c dimension_unit<D>::reference denote the same unit type.
 * 
 * @tparam U Type to verify.
 * @tparam D in_dimension type to use for verification.
 */
template<typename U, typename D>
concept UnitOf =
  in_unit<U> &&
  in_dimension<D> &&
  std::same_as<typename U::reference, typename dimension_unit<D>::reference>;

// in_quantity
namespace detail {

template<typename T>
inline constexpr bool is_quantity = false;

}  // namespace detail

/**
 * @brief A concept matching all quantities in the library.
 * 
 * Satisfied by all instantiations of :class:`quantity`.
 */
template<typename T>
concept in_quantity = detail::is_quantity<T>;


// in_wrapped_quantity
namespace detail {

template<typename T>
inline constexpr bool is_wrapped_quantity = false;

template<typename T>
  requires requires { typename T::value_type; }
inline constexpr bool is_wrapped_quantity<T> = in_quantity<typename T::value_type> || is_wrapped_quantity<typename T::value_type>;

}  // namespace detail

/**
 * @brief A concept matching types that wrap quantity objects.
 * 
 * Satisfied by all wrapper types that satisfy `in_quantity<typename T::value_type>`
 * recursively (i.e. `std::optional<si::length<si::metre>>`).
 */
template<typename T>
concept in_wrapped_quantity = detail::is_wrapped_quantity<T>;

// in_numeric_value

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
 * @brief A concept matching non-in_quantity types.
 * 
 * Satisfied by types that satisfy `(!in_quantity<T>) && (!in_wrapped_quantity<T>) && std::regular<T>`.
 */
template<typename T>
concept in_numeric_value =
  (!in_quantity<T>) &&
  (!in_wrapped_quantity<T>) &&
  std::regular<T> &&
  (detail::constructible_from_integral<T> || detail::not_constructible_from_integral<T>);

}  // namespace units
