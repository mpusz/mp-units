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

#include <units/bits/external/hacks.h>

// IWYU pragma: begin_exports
#include <units/bits/external/fixed_string.h>
#include <units/bits/external/type_traits.h>
#include <units/customization_points.h>
#include <units/magnitude.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <cstdint>
#include <functional>
#include <utility>

namespace units {

// Prefix
namespace detail {

template<Magnitude auto M>
struct prefix_base;

template<Magnitude auto M>
void to_prefix_base(const volatile prefix_base<M>*);

}  // namespace detail

/**
 * @brief A concept matching a symbol prefix
 *
 * Satisfied by all specializations of `prefix`.
 */
template<typename T>
concept Prefix = requires(T* t) { detail::to_prefix_base(t); };

// Unit
template<Magnitude auto M, typename U>
struct scaled_unit;

// TODO: Remove when P1985 accepted
namespace detail {

template<Magnitude auto M, typename U>
void to_base_scaled_unit(const volatile scaled_unit<M, U>*);

}  // namespace detail

/**
 * @brief A concept matching all unit types in the library
 *
 * Satisfied by all unit types derived from an specialization of :class:`scaled_unit`.
 */
template<typename T>
concept Unit = requires(T* t) { detail::to_base_scaled_unit(t); };

namespace detail {

template<typename>
inline constexpr bool is_named = false;

}

template<typename T>
concept NamedUnit = Unit<T> && detail::is_named<T>;

template<Unit U, basic_symbol_text Symbol>
struct alias_unit;

// TODO: Remove when P1985 accepted
namespace detail {

template<Unit U, basic_symbol_text Symbol>
void to_base_alias_unit(const volatile alias_unit<U, Symbol>*);

}  // namespace detail

template<typename T>
concept AliasUnit = requires(T* t) { detail::to_base_alias_unit(t); };

// BaseDimension
template<basic_fixed_string Symbol, NamedUnit U>
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
concept BaseDimension = requires(T* t) { detail::to_base_base_dimension(t); };

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
  requires(BaseDimension<typename Es::dimension> && ...)
struct derived_dimension_base;

}  // namespace detail

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

}  // namespace detail

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
concept UnitOf = Unit<U> && Dimension<D> && std::same_as<typename U::reference, typename dimension_unit<D>::reference>;

// PointOrigin

template<Dimension D>
struct point_origin;

/**
 * @brief A concept matching a point origin
 *
 * Satisfied by types derived from an specialization of @c point_origin.
 */
template<typename T>
concept PointOrigin = is_derived_from_specialization_of<T, point_origin> && requires {
  typename T::dimension;
  requires Dimension<typename T::dimension>;
  typename T::point_origin;
  requires std::same_as<typename T::point_origin, point_origin<typename T::dimension>>;
  requires !std::same_as<T, point_origin<typename T::dimension>>;
};

// RebindablePointOriginFor

namespace detail {

template<typename O, typename D>
struct rebind_point_origin_dimension_impl {
  using type = typename O::template rebind<D>;
};

}  // namespace detail

template<PointOrigin O, Dimension D>
using rebind_point_origin_dimension = typename conditional<is_same_v<typename O::dimension, D>, std::type_identity<O>,
                                                           detail::rebind_point_origin_dimension_impl<O, D>>::type;

/**
 * @brief A concept predicating the possibility of changing an origin's dimension
 *
 * Satisfied by point origins whose dimension can be made to be `D`.
 */
template<typename T, typename D>
concept RebindablePointOriginFor =
  requires { typename rebind_point_origin_dimension<T, D>; } && PointOrigin<rebind_point_origin_dimension<T, D>> &&
  std::same_as<D, typename rebind_point_origin_dimension<T, D>::dimension>;

// Kind
namespace detail {

template<typename, Dimension>
struct _kind_base;

}  // namespace detail

template<typename T, template<typename...> typename Base>
concept kind_impl_ = is_derived_from_specialization_of<T, Base> && requires {
  typename T::base_kind;
  typename T::dimension;
  requires Dimension<typename T::dimension>;
};

/**
 * @brief A concept matching all kind types
 *
 * Satisfied by all kind types derived from an specialization of @c kind.
 */
template<typename T>
concept Kind = kind_impl_<T, detail::_kind_base> && kind_impl_<typename T::base_kind, detail::_kind_base> &&
               std::same_as<typename T::base_kind, typename T::base_kind::base_kind>;

// PointKind
namespace detail {

template<Kind, PointOrigin>
struct _point_kind_base;

}  // namespace detail

/**
 * @brief A concept matching all point kind types
 *
 * Satisfied by all point kind types derived from an specialization of @c point_kind.
 */
template<typename T>
concept PointKind =
  kind_impl_<T, detail::_point_kind_base> && requires { typename T::origin; } && PointOrigin<typename T::origin> &&
  std::same_as<typename T::dimension, typename T::base_kind::dimension> &&
  std::same_as<typename T::dimension, typename T::origin::dimension>;

// Reference
namespace detail {

template<typename T>
inline constexpr bool is_reference = false;

}  // namespace detail

/**
 * @brief A concept matching all references in the library.
 *
 * Satisfied by all specializations of @c reference.
 */
template<typename T>
concept Reference = detail::is_reference<T>;

// Quantity, QuantityPoint, QuantityKind, QuantityPointKind
namespace detail {

template<typename T>
inline constexpr bool is_quantity = false;

template<typename T>
inline constexpr bool is_quantity_point = false;

template<typename T>
inline constexpr bool is_quantity_kind = false;

template<typename T>
inline constexpr bool is_quantity_point_kind = false;

template<typename T>
inline constexpr bool is_quantity_like = false;

template<typename T>
inline constexpr bool is_quantity_point_like = false;

}  // namespace detail

/**
 * @brief A concept matching all quantities in the library.
 *
 * Satisfied by all specializations of @c quantity.
 */
template<typename T>
concept Quantity = detail::is_quantity<T>;

/**
 * @brief A concept matching all quantity points in the library.
 *
 * Satisfied by all specializations of @c quantity_point.
 */
template<typename T>
concept QuantityPoint = detail::is_quantity_point<T>;

/**
 * @brief A concept matching all quantity kinds in the library.
 *
 * Satisfied by all specializations of @c quantity_kind.
 */
template<typename T>
concept QuantityKind = detail::is_quantity_kind<T>;

/**
 * @brief A concept matching all quantity point kinds in the library.
 *
 * Satisfied by all specializations of @c quantity_point_kind.
 */
template<typename T>
concept QuantityPointKind = detail::is_quantity_point_kind<T>;

// QuantityLike, QuantityPointLike

/**
 * @brief A concept matching all quantity-like types (other than specialization of @c quantity)
 *
 * Satisfied by all types for which a correct specialization of `quantity_like_traits`
 * type trait is provided.
 */
template<typename T>
concept QuantityLike = detail::is_quantity_like<T>;

/**
 * @brief A concept matching all quantity point-like types (other than specialization of @c quantity_point)
 *
 * Satisfied by all types for which a correct specialization of `quantity_point_like_traits`
 * type trait is provided.
 */
template<typename T>
concept QuantityPointLike = detail::is_quantity_point_like<T>;

// Representation

template<typename T, typename U>
concept common_type_with_ =  // exposition only
  (std::same_as<std::common_type_t<T, U>, std::common_type_t<U, T>>)&&(
    std::constructible_from<std::common_type_t<T, U>, T>)&&(std::constructible_from<std::common_type_t<T, U>, U>);

template<typename T, typename U = T>
concept scalable_number_ =  // exposition only
  (std::regular_invocable<std::multiplies<>, T, U>)&&(std::regular_invocable<std::divides<>, T, U>);

template<typename T>
concept castable_number_ =  // exposition only
  common_type_with_<T, std::intmax_t> && scalable_number_<std::common_type_t<T, std::intmax_t>>;

template<typename T>
concept scalable_ =  // exposition only
  castable_number_<T> ||
  (requires { typename T::value_type; } && castable_number_<typename T::value_type> &&
   scalable_number_<T, std::common_type_t<typename T::value_type, std::intmax_t>>) ||
  (requires { typename T::element_type; } && castable_number_<typename T::element_type> &&
   scalable_number_<T, std::common_type_t<typename T::element_type, std::intmax_t>>);

template<typename T, typename U>
concept scalable_with_ =  // exposition only
  common_type_with_<T, U> && scalable_<std::common_type_t<T, U>>;

// WrappedQuantity
namespace detail {

template<typename T>
inline constexpr bool is_wrapped_quantity = false;

template<typename T>
  requires requires { typename T::value_type; }
inline constexpr bool is_wrapped_quantity<T> =
  Quantity<typename T::value_type> || QuantityLike<typename T::value_type> ||
  is_wrapped_quantity<typename T::value_type>;

template<typename T>
  requires requires { typename T::element_type; }
inline constexpr bool is_wrapped_quantity<T> =
  Quantity<typename T::element_type> || QuantityLike<typename T::element_type> ||
  is_wrapped_quantity<typename T::element_type>;

template<typename T>
  requires requires { typename T::quantity_type; }
inline constexpr bool is_wrapped_quantity<T> = Quantity<typename T::quantity_type>;

}  // namespace detail

/**
 * @brief A concept matching types that wrap quantity objects.
 *
 * Satisfied by all wrapper types that satisfy `Quantity<typename T::value_type>` or
 * `Quantity<typename T::element_type>` recursively
 * (i.e. `std::optional<si::length<si::metre>>`).
 */
template<typename T>
concept wrapped_quantity_ =  // exposition only
  detail::is_wrapped_quantity<T>;

/**
 * @brief A concept matching non-Quantity types.
 *
 * Satisfied by types that satisfy `(!Quantity<T>) && (!WrappedQuantity<T>) && std::regular<T>`.
 */
template<typename T>
concept Representation = (!Quantity<T>)&&(!QuantityLike<T>)&&(!wrapped_quantity_<T>)&&std::regular<T> && scalable_<T>;

namespace detail {

template<typename T>
  requires requires(T q) {
    typename quantity_like_traits<T>::dimension;
    typename quantity_like_traits<T>::unit;
    typename quantity_like_traits<T>::rep;
    requires Dimension<typename quantity_like_traits<T>::dimension>;
    requires Unit<typename quantity_like_traits<T>::unit>;
    requires Representation<typename quantity_like_traits<T>::rep>;
    {
      quantity_like_traits<T>::number(q)
    } -> std::convertible_to<typename quantity_like_traits<T>::rep>;
  }
inline constexpr bool is_quantity_like<T> = true;

template<typename T>
  requires requires(T q) {
    typename quantity_point_like_traits<T>::dimension;
    typename quantity_point_like_traits<T>::unit;
    typename quantity_point_like_traits<T>::rep;
    requires Dimension<typename quantity_point_like_traits<T>::dimension>;
    requires Unit<typename quantity_point_like_traits<T>::unit>;
    requires Representation<typename quantity_point_like_traits<T>::rep>;
    {
      quantity_point_like_traits<T>::relative(q)
    } -> QuantityLike;
  }
inline constexpr bool is_quantity_point_like<T> = true;

}  // namespace detail

}  // namespace units
