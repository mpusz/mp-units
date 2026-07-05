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
#include <mp-units/bits/module_macros.h>
#include <mp-units/ext/type_traits.h>
#include <mp-units/framework/quantity_character.h>
#include <mp-units/framework/quantity_spec_concepts.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#include <cstddef>
#include <limits>
#include <string_view>
#include <type_traits>
#include <utility>
#if MP_UNITS_HOSTED
#include <chrono>
#endif
#endif
#endif

// Public customization points in this file:
//   - treat_as_floating_point<Rep>
//   - representation_values<Rep>
//   - constraint_violation_handler<Rep>
//   - quantity_like_traits<T>
//   - quantity_point_like_traits<T>
// Additional customization points defined in other headers:
//   - implicitly_scalable<FromUnit, FromRep, ToUnit, ToRep>  <mp-units/framework/value_cast.h>
//   - frame_projection<From, To>                             <mp-units/framework/quantity_point_concepts.h>

namespace mp_units {

namespace utility {

/**
 * @brief Sentinel type indicating no default implementation for a variable template
 *
 * Used as the default value of a specializable variable template (a customization point) that must
 * not have a working default, working around the language limitation that variable templates cannot
 * be "deleted" like functions can. `specified` tests whether such a point has been given a real value
 * (i.e. is not `undefined`). These are public authoring vocabulary shared by the library's
 * customization points and by higher-level tools, so they live in `mp_units::utility`.
 */
MP_UNITS_EXPORT struct undefined_t {};
MP_UNITS_EXPORT inline constexpr undefined_t undefined{};

MP_UNITS_EXPORT template<typename T>
concept specified = !std::same_as<std::remove_cvref_t<T>, undefined_t>;

}  // namespace utility

namespace detail {

template<typename>
struct cond_underlying_type {};

template<typename T>
  requires std::is_object_v<T>
struct cond_underlying_type<T> {
  using type = std::remove_cv_t<T>;
};

template<typename T>
concept has_value_type_member = requires { typename T::value_type; };

template<typename T>
concept has_element_type_member = requires { typename T::element_type; };

}  // namespace detail

/**
 * @brief Describes the underlying arithmetic/element type of a representation type.
 *
 * The library uses this trait to discover the "scalar element" inside a representation
 * type (e.g. `int` for `safe_int<int>`, `double` for `cartesian_vector<double>`). It is
 * used to drive classification (integer vs floating-point scaling path, default of
 * `treat_as_floating_point`, etc.) — it is **not** an indirection/dereference trait.
 *
 * The primary template is intentionally empty so that types which do not expose an
 * underlying type are cleanly recognized as "leaf" representations (the recursive
 * unwrapping in @c detail::value_type_t stops at them).
 *
 * Default detection (via partial specializations below) follows the same shape as the
 * standard's `std::indirectly_readable_traits` for its `value_type` / `element_type`
 * cases (see [readable.traits]): nested `::value_type`, else nested `::element_type`;
 * a top-level `const` is passed through; the detected alias has its cv-qualification
 * removed; and if a type provides both `::value_type` and `::element_type` whose
 * underlying types differ after ignoring top-level cv-qualification, the trait is
 * empty (the user must disambiguate explicitly). Pointer and array specializations
 * from the standard are intentionally omitted — those are modeled by the standard's
 * iterator machinery, not by this trait. Additionally, scoped enumeration types are
 * supported via `std::underlying_type_t` — a representation-model extension not
 * present in the standard's iterator-oriented trait (unscoped enums are excluded
 * because they already implicitly convert to their underlying type).
 *
 * @tparam T the representation type
 */
MP_UNITS_EXPORT template<typename T>
struct representation_underlying_type {};

template<typename T>
struct representation_underlying_type<const T> : representation_underlying_type<T> {};

template<typename T>
  requires is_scoped_enum_v<T>
struct representation_underlying_type<T> {
  using type = std::underlying_type_t<T>;
};

template<detail::has_value_type_member T>
struct representation_underlying_type<T> : detail::cond_underlying_type<typename T::value_type> {};

template<detail::has_element_type_member T>
struct representation_underlying_type<T> : detail::cond_underlying_type<typename T::element_type> {};

template<detail::has_value_type_member T>
  requires detail::has_element_type_member<T>
struct representation_underlying_type<T> {};

template<detail::has_value_type_member T>
  requires detail::has_element_type_member<T> &&
           std::same_as<std::remove_cv_t<typename T::value_type>, std::remove_cv_t<typename T::element_type>>
struct representation_underlying_type<T> : detail::cond_underlying_type<typename T::value_type> {};

MP_UNITS_EXPORT template<typename T>
  requires requires { typename representation_underlying_type<T>::type; }
using representation_underlying_type_t = typename representation_underlying_type<T>::type;

/**
 * @brief Maps a representation value type to the concrete type a quantity should store.
 *
 * Expression-template linear algebra libraries (e.g. Eigen, Blaze, Armadillo) return lazy proxy
 * types from their arithmetic operators. Such a proxy keeps references to its operands and must
 * be evaluated to a concrete type before being stored inside a `quantity`; otherwise the quantity
 * would retain dangling references once the operands (often temporaries) go out of scope.
 *
 * The `quantity` deduction guides and the concepts that compute the representation type resulting
 * from an arithmetic operation use this trait so that the stored representation is always a
 * materialized concrete type. The primary template simply decays the type, which is correct for
 * scalars and for vector/matrix types whose operators return a concrete value (e.g. `cartesian_vector`,
 * GLM). Specialize it for an expression-template type to name its evaluated type (for example to
 * `Eigen::Matrix`'s `PlainObject`, or Blaze's `ResultType`) — see the integration guide.
 *
 * @tparam T the representation value type (possibly an expression-template proxy)
 */
MP_UNITS_EXPORT template<typename T>
struct representation_canonical_type {
  using type = std::remove_cvref_t<T>;
};

// A top-level `const` is forwarded to the unqualified type (mirroring `representation_underlying_type`
// above) so that library specializations only need to handle the unqualified type. This matters
// because expression-template libraries return their proxies by `const` value (e.g.
// `decltype(Eigen::Vector3d{} * 2.0)` is a `const Eigen::CwiseBinaryOp<...>`).
template<typename T>
struct representation_canonical_type<const T> : representation_canonical_type<T> {};

MP_UNITS_EXPORT template<typename T>
using representation_canonical_type_t = typename representation_canonical_type<T>::type;

namespace detail {

template<typename T>
struct value_type_impl {
  using type = T;
};

template<typename T>
  requires requires { typename representation_underlying_type_t<T>; }
struct value_type_impl<T> {
  using type = value_type_impl<representation_underlying_type_t<T>>::type;
};

template<typename T>
  requires std::is_object_v<T>
using value_type_t = value_type_impl<T>::type;

}  // namespace detail


/////////////// real / imag / modulus CUSTOMIZATION POINTS ///////////////

// `real`, `imag`, and `modulus` are general numeric customization points: a representation type
// exposes them either as members or as ADL free functions. They underlie the `numeric_field` field
// detection below and the framework's complex-scalar handling.

namespace detail::real_impl {

void real() = delete;  // poison pill

struct real_t {
  // TODO how to constrain the return with RealScalar?
  [[nodiscard]] constexpr auto operator()(const auto& clx) const
    requires requires { clx.real(); } || requires { real(clx); }
  {
    if constexpr (requires { clx.real(); })
      return clx.real();
    else if constexpr (requires { real(clx); })
      return real(clx);
  }
};

}  // namespace detail::real_impl

inline namespace cpo {

MP_UNITS_EXPORT inline constexpr ::mp_units::detail::real_impl::real_t real;

}

namespace detail::imag_impl {

void imag() = delete;  // poison pill

struct imag_t {
  // TODO how to constrain the return with RealScalar?
  [[nodiscard]] constexpr auto operator()(const auto& clx) const
    requires requires { clx.imag(); } || requires { imag(clx); }
  {
    if constexpr (requires { clx.imag(); })
      return clx.imag();
    else if constexpr (requires { imag(clx); })
      return imag(clx);
  }
};

}  // namespace detail::imag_impl

inline namespace cpo {

MP_UNITS_EXPORT inline constexpr ::mp_units::detail::imag_impl::imag_t imag;

}

namespace detail::modulus_impl {

void modulus() = delete;  // poison pill
void abs() = delete;      // poison pill

struct modulus_t {
  // TODO how to constrain the return with RealScalar?
  [[nodiscard]] constexpr auto operator()(const auto& clx) const
    requires requires { clx.modulus(); } || requires { modulus(clx); } || requires { clx.abs(); } ||
             requires { abs(clx); }
  {
    if constexpr (requires { clx.modulus(); })
      return clx.modulus();
    else if constexpr (requires { modulus(clx); })
      return modulus(clx);
    // `std` made a precedence of using `abs` for modulus on `std::complex`
    else if constexpr (requires { clx.abs(); })
      return clx.abs();
    else if constexpr (requires { abs(clx); })
      return abs(clx);
  }
};

}  // namespace detail::modulus_impl

inline namespace cpo {

MP_UNITS_EXPORT inline constexpr ::mp_units::detail::modulus_impl::modulus_t modulus;

}


/////////////// tensor_order ///////////////

namespace detail {

template<typename T>
concept has_vector_indexing = requires(const T& t) { t[std::size_t{}]; };

template<typename T>
concept has_matrix_indexing = requires(const T& t) { t(std::size_t{}, std::size_t{}); }
#if __cpp_multidimensional_subscript && MP_UNITS_COMP_GCC != 12
                              || requires(const T& t) { t[std::size_t{}, std::size_t{}]; }
#endif
;

// A type is structurally ambiguous about its order when it exposes *both* indexing shapes (an N x 1
// matrix modeling a vector, as Eigen does): the same accessors fit a vector and a matrix, and only
// the type's compile-time extents can tell them apart.
template<typename T>
concept has_ambiguous_order = has_vector_indexing<T> && has_matrix_indexing<T>;

}  // namespace detail

// The intrinsic tensor order of a representation: 0 scalar, 1 vector, 2 tensor. The primary template
// is left *undefined* (`utility::undefined_t`); a partial specialization detects the order structurally
// for a type that exposes exactly one indexing shape (single-index `t[i]` -> 1, two-index `t(i, j)` ->
// 2, neither -> 0), and a third-party representation may specialize it (e.g. an Eigen adapter reading
// `RowsAtCompileTime` / `ColsAtCompileTime`). A type that exposes *both* shapes is ambiguous - only
// its extents can decide - so it matches neither and stays `undefined` unless specialized: guessing
// would disagree with an adapter, an ODR hazard across translation units.
MP_UNITS_EXPORT template<typename T>
constexpr utility::undefined_t tensor_order;

template<typename T>
  requires(!detail::has_ambiguous_order<T>)
constexpr std::size_t tensor_order<T> = detail::has_matrix_indexing<T>   ? std::size_t{2}
                                        : detail::has_vector_indexing<T> ? std::size_t{1}
                                                                         : std::size_t{0};


/////////////// numeric_field ///////////////

namespace detail {

template<typename T>
  requires has_vector_indexing<T>
using vector_element_t = std::remove_cvref_t<decltype(std::declval<const T&>()[std::size_t{}])>;

template<typename T>
  requires has_matrix_indexing<T>
[[nodiscard]] consteval auto matrix_element()
{
#if __cpp_multidimensional_subscript && MP_UNITS_COMP_GCC != 12
  if constexpr (requires(const T& t) { t[std::size_t{}, std::size_t{}]; })
    return std::type_identity<std::remove_cvref_t<decltype(std::declval<const T&>()[std::size_t{}, std::size_t{}])>>{};
  else
#endif
    return std::type_identity<std::remove_cvref_t<decltype(std::declval<const T&>()(std::size_t{}, std::size_t{}))>>{};
}

template<typename T>
  requires has_matrix_indexing<T>
using matrix_element_t = typename decltype(matrix_element<T>())::type;

template<typename T>
concept field_reachable = utility::specified<decltype(tensor_order<T>)> &&
                          (tensor_order<T> == 0 || (tensor_order<T> == 1 && has_vector_indexing<T>) ||
                           (tensor_order<T> == 2 && has_matrix_indexing<T>));

template<typename T>
  requires field_reachable<T>
[[nodiscard]] consteval quantity_field detect_numeric_field()
{
  if constexpr (tensor_order<T> == 1)
    return detect_numeric_field<vector_element_t<T>>();
  else if constexpr (tensor_order<T> == 2)
    return detect_numeric_field<matrix_element_t<T>>();
  else if constexpr (requires(const T& v) {
                       ::mp_units::real(v);
                       ::mp_units::imag(v);
                     })
    return quantity_field::complex;
  else
    return quantity_field::real;
}

}  // namespace detail

// The numeric field of a type: real or complex. The primary is left *undefined*; a specialization
// defines it for a type whose order is known, reading the field off a scalar *element* (never off a
// container's surface, since a linear-algebra vector or matrix exposes `real()`/`imag()` even when it
// is real).
MP_UNITS_EXPORT template<typename T>
constexpr utility::undefined_t numeric_field;

template<typename T>
  requires detail::field_reachable<T>
constexpr quantity_field numeric_field<T> = detail::detect_numeric_field<T>();


/////////////// disable_representation ///////////////

namespace detail {

// True when `T` is itself a quantity or a quantity-like external type (e.g. a `std::chrono`
// duration). Forward-declared here and specialized in `quantity_concepts.h`, because the `Quantity`
// and `QuantityLike` concepts are not yet available at this point.
template<typename T>
constexpr bool is_quantity_abstraction = false;

}  // namespace detail

// A specializable opt-out: when `true`, `T` is not accepted as a quantity representation by any
// character. The default rejects a type that is, or whose elements are, a quantity abstraction (so a
// bare quantity and a container of quantities are both excluded), and `bool` is opted out as well. A
// custom type bars itself from ever being a representation with a one-line specialization.
MP_UNITS_EXPORT template<typename T>
constexpr bool disable_representation = detail::is_quantity_abstraction<detail::value_type_t<T>>;

template<>
MP_UNITS_INLINE constexpr bool disable_representation<bool> = true;

MP_UNITS_EXPORT_BEGIN

/**
 * @brief Specifies if a value of a type should be treated as a floating-point value
 *
 * This type trait should be specialized for a custom representation type to specify
 * that values of this type should be treated by the library as a floating-point ones
 * which will enable implicit conversions between quantities.
 *
 * @tparam Rep a representation type for which a type trait is defined
 */
template<typename Rep>
constexpr bool treat_as_floating_point =
#if MP_UNITS_HOSTED
  std::chrono::treat_as_floating_point_v<detail::value_type_t<Rep>>;
#else
  std::is_floating_point_v<detail::value_type_t<Rep>>;
#endif

/**
 * @brief Specifies if a specific conversion between two types is representable without data loss
 *
 * @deprecated Use `mp_units::implicitly_scalable` instead.
 *
 * @tparam From a source representation type
 * @tparam To a destination representation type
 */
template<typename From, typename To>
[[deprecated("2.6.0: Use `mp_units::implicitly_scalable` instead")]]
constexpr bool is_value_preserving = treat_as_floating_point<To> || !treat_as_floating_point<From>;

template<typename Rep>
[[deprecated("2.5.0: `is_scalar` is no longer necessary and can simply be removed")]]
constexpr bool is_scalar = false;

template<typename Rep>
[[deprecated("2.5.0: `is_complex` is no longer necessary and can simply be removed")]]
constexpr bool is_complex = false;

template<typename Rep>
[[deprecated("2.5.0: `is_vector` is no longer necessary and can simply be removed")]]
constexpr bool is_vector = false;

template<typename Rep>
[[deprecated("2.5.0: `is_tensor` is no longer necessary and can simply be removed")]]
constexpr bool is_tensor = false;

/**
 * @brief A type trait that defines zero, one, min, and max for a representation type
 *
 * The zero, one, min, and max member functions in mp_units::quantity forward their work to
 * these methods. This type can be specialized if the representation Rep requires a specific
 * implementation to return these quantity objects.
 *
 * @tparam Rep a representation type for which a type trait is defined
 */
template<typename Rep>
#if MP_UNITS_HOSTED
struct representation_values : std::chrono::duration_values<Rep> {
#else
struct representation_values {
  static constexpr Rep zero() noexcept
    requires std::constructible_from<Rep, int>
  {
    return Rep(0);
  }

  static constexpr Rep min() noexcept
    requires std::numeric_limits<Rep>::is_specialized && requires {
      { std::numeric_limits<Rep>::lowest() } -> std::same_as<Rep>;
    }
  {
    return std::numeric_limits<Rep>::lowest();
  }

  static constexpr Rep max() noexcept
    requires std::numeric_limits<Rep>::is_specialized && requires {
      { std::numeric_limits<Rep>::max() } -> std::same_as<Rep>;
    }
  {
    return std::numeric_limits<Rep>::max();
  }
#endif

  static constexpr Rep one() noexcept
    requires std::constructible_from<Rep, int>
  {
    return Rep(1);
  }
};

template<typename Rep>
using quantity_values [[deprecated("2.5.0: Use `representation_values` instead")]] = representation_values<Rep>;

/**
 * @brief Customization point for handling domain constraint violations.
 *
 * The library calls `constraint_violation_handler<Rep>::on_violation(msg)` when
 * a domain constraint is violated (e.g., out-of-bounds value, non-negativity).
 *
 * No default implementation is provided. When no specialization exists for a given
 * Rep, the library uses MP_UNITS_EXPECTS directly (which may compile to nothing in
 * release builds). Specializing this trait opts the type into guaranteed enforcement.
 *
 * @tparam Rep the representation type
 */
template<typename Rep>
struct constraint_violation_handler;

MP_UNITS_EXPORT_END

namespace detail {

template<typename Rep>
concept HasConstraintViolationHandler =
  requires(std::string_view msg) { constraint_violation_handler<Rep>::on_violation(msg); };

}  // namespace detail

MP_UNITS_EXPORT_BEGIN

/**
 * @brief Provides support for external quantity-like types
 *
 * The type trait should provide the @c reference object, a type alias @c rep,
 * and static member functions @c to_numerical_value(T) that returns the raw value
 * of the quantity and @c from_numerical_value(rep) that returns @c T from @c rep.
 *
 * If the following expression is @c true, the specified conversion will be explicit:
 * - @c explicit_import for the conversion from @c T to a @c quantity type,
 * - @c explicit_export for the conversion from a @c quantity type to @c T.
 *
 * Usage example can be found in @c mp-units/systems/si/chrono.h header file.
 *
 * @tparam T the type to provide support for
 */
template<typename T>
struct quantity_like_traits;

/**
 * @brief Provides support for external quantity point-like types
 *
 * The type trait should provide nested @c reference and @c origin objects,
 * a type alias @c rep, and static member functions @c to_numerical_value(T) that
 * returns the raw value of the the quantity being the offset of the point from the
 * origin and @c from_numerical_value(rep) that returns @c T formed this raw value.
 *
 * If the following expression is @c true, the specified conversion will be explicit:
 * - @c explicit_import for the conversion from @c T to a @c quantity_point type,
 * - @c explicit_export for the conversion from a @c quantity_point type to @c T.
 *
 * Usage example can be found in @c mp-units/systems/si/chrono.h header file.
 *
 * @tparam T the type to provide support for
 */
template<typename T>
struct quantity_point_like_traits;

/**
 * @brief Opts a vector quantity into decomposition into named 1D-vector components
 *
 * Specialize for a vector @c quantity_spec and inherit from @c vector_axes, listing the per-axis
 * quantity specs in coordinate order (see @c mp-units/framework/vector_components.h):
 *
 * @code
 * template<>
 * struct mp_units::vector_components<flight_velocity> :
 *     mp_units::vector_axes<forward_velocity, wind_drift, sink_rate> {};
 * @endcode
 *
 * Left undefined (the quantity is not decomposable) unless specialized. Only a concrete vector
 * quantity spec may be decomposed, so a specialization for a non-vector spec or for a `kind_of<>`
 * quantity kind is ill-formed.
 *
 * @tparam QS the vector quantity spec being decomposed
 */
// TODO Once point and delta quantity specs are introduced, exclude those here as well: an absolute
// point or a delta is not decomposable into plain vector components.
template<QuantitySpec auto QS>
  requires(QS.character.order == quantity_tensor_order::vector) &&
          (!detail::QuantityKindSpec<MP_UNITS_REMOVE_CONST(decltype(QS))>)
struct vector_components;

MP_UNITS_EXPORT_END

}  // namespace mp_units
