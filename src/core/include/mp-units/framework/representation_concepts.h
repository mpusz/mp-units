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
#include <mp-units/bits/fixed_point.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/quantity_character.h>
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/unit_magnitude.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#if MP_UNITS_HOSTED
#include <complex>
#endif
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <type_traits>
#endif  // MP_UNITS_IMPORT_STD
#endif  // MP_UNITS_IN_MODULE_INTERFACE

namespace mp_units {

/////////////// ALGEBRA PRIMITIVES ///////////////

namespace detail {

// Extends std::integral to cover int128_t and uint128_t.
// On GCC in strict mode (-std=c++20), std::integral<__int128> = false because the
// standard traits (std::is_integral, std::is_arithmetic) are not specialized for
// __int128 outside of GNU extensions (-std=gnu++20).  When __SIZEOF_INT128__ is
// absent, int128_t / uint128_t are double_width_int<> emulations which also do not
// satisfy std::integral.  This concept patches both gaps so the scaling engine can
// treat all supported 128-bit integer types uniformly.
template<typename T>
concept integral =
  std::integral<T> || std::same_as<std::remove_cv_t<T>, int128_t> || std::same_as<std::remove_cv_t<T>, uint128_t>;

// The wider integer type used for magnitude constants to prevent overflow when
// scaling by a rational magnitude (v * num / den).  For elements up to 32 bits
// we widen to (u)int64_t; for 64-bit elements we widen to (u)int128_t; otherwise the
// element type itself is wide enough. Signedness is preserved so that safe_int<unsigned>
// can multiply by a wider factor without crossing the mixed-signedness boundary.
template<typename element_t>
using wider_int_for =
  std::conditional_t<std::is_signed_v<element_t>,
                     std::conditional_t<(sizeof(element_t) <= sizeof(std::int32_t)), std::int64_t,
                                        conditional<(sizeof(element_t) < sizeof(int128_t)), int128_t, element_t>>,
                     std::conditional_t<(sizeof(element_t) <= sizeof(std::uint32_t)), std::uint64_t,
                                        conditional<(sizeof(element_t) < sizeof(uint128_t)), uint128_t, element_t>>>;

// `ScalableWith` and `Addable` describe the representation type's *public algebra*, so each
// operation's result is required to be `common_with<T>`. `common_with` is the right relation here
// precisely because it is *symmetric*: it asserts the result and `T` share a common type without
// privileging a lossy direction.
//
//  - `constructible_from<T, result>` (or `convertible_to<result, T>`) would instead bless
//    truncation: for a widening operation (`int16_t + int16_t -> int`, or a fixed-point type that
//    widens) it "validates" by checking the wide result narrows back into the narrow `T`.
//  - the non-narrowing directional check `convertible_to<T, result>` ("T widens into the result")
//    rejects expression-template results - e.g. `Eigen::Vector3d` is not convertible to its own
//    `CwiseBinaryOp` sum.
//
// `common_with` accepts both promotion (common type = the wider type) and expression templates
// (common type = the evaluated vector) while rejecting `void` / unrelated junk, and without
// asserting any narrowing. It is deliberately a bit stronger than the bare mechanics of a single
// operation (much as `std::totally_ordered` requires `equality_comparable`, and as mp-units requires
// every representation to be `WeaklyRegular`): a promoting type satisfies it by providing the
// matching `std::common_type` specialization - the type author's part of the contract.
//
// This is a representation-validity gate only; the quantity operators do NOT consume it. `operator+`
// materializes results via `common_quantity_for` (`get_common_reference` +
// `representation_canonical_type_t<invoke_result_t<...>>`), and the comparison operators enforce
// their own `std::common_type<Rep1, Rep2>` requirement (`CommonlyComparableQuantities`).
//
// It is also distinct from the *internal* scaling concepts (`UsesFloatingPointScaling` /
// `UsesIntegerScaling`): those use `constructible_from` because the integer path *deliberately*
// widens to a transient and `static_cast`s back to `To` - there the narrowing is the intended
// operation, not a silent truncation. See the note on those concepts.
template<typename T, typename S>
concept ScalableWith = requires(const T v, const S s) {
  { v * s / s } -> std::common_with<T>;
  { s * v / s } -> std::common_with<T>;
  { v / s * s } -> std::common_with<T>;
};

template<typename T>
concept Addable = requires(const T a, const T b) {
  { -a } -> std::common_with<T>;
  { a + b } -> std::common_with<T>;
  { a - b } -> std::common_with<T>;
};

template<typename T>
concept WeaklyRegular = std::copyable<T> && std::equality_comparable<T>;

template<typename T>
concept RegularAddable = Addable<T>
#if MP_UNITS_COMP_GCC != 12 && !defined(MP_UNITS_XCODE15_HACKS)
                         && WeaklyRegular<T>
#endif
  ;

template<typename T>
concept BaseScalar = RegularAddable<T> && ScalableWith<T, T>;

}  // namespace detail


/////////////// SCALAR ///////////////

namespace detail {

// The field axis as character concepts, derived from the `numeric_field` trait so they honor adapter
// declarations (a real Eigen vector exposes `real()`/`imag()` yet its adapter declares its field
// real). They are order-agnostic: `Real` / `Complex` classify scalars, vectors, and tensors alike.
template<typename T>
concept Real = (numeric_field<T> == quantity_field::real);

template<typename T>
concept Complex = (numeric_field<T> == quantity_field::complex);

// A real scalar is a real, totally ordered scalar: its field is real (no `real()`/`imag()` API), and
// it is totally ordered, since the framework and its users rely on `<`/`==` to compare, clamp, and
// sort scalar quantities. Both clauses exclude `std::complex`.
template<typename T>
concept RealScalar = Real<T> && BaseScalar<T> && std::totally_ordered<T>;

// A complex scalar is `Complex` (so `real`/`imag` work) and, on top of the shared scalar algebra,
// supports `modulus` and reconstruction from its parts. Construction from `(real, imag)` is what
// lets users (and the framework's complex paths) build and round-trip a value efficiently.
template<typename T>
concept ComplexScalar = Complex<T> && BaseScalar<T> && requires(const T v, const T& ref) {
  requires std::constructible_from<T, decltype(::mp_units::real(ref)), decltype(::mp_units::imag(ref))>;
  ::mp_units::modulus(v);
  requires ScalableWith<T, decltype(::mp_units::modulus(v))>;
};

// A scalar is an order-0 representation of either field. The `tensor_order == 0` gate keeps a type
// that has scalar algebra but also exposes indexing out of the scalar slot, so the order axis
// partitions cleanly (`Vector` / `Tensor` below state their own order).
template<typename T>
concept Scalar = (RealScalar<T> || ComplexScalar<T>) && tensor_order<T> == 0;

}  // namespace detail


/////////////// MAGNITUDE ///////////////

namespace detail::magnitude_impl {

void magnitude() = delete;  // poison pill
void norm() = delete;       // poison pill
void abs() = delete;        // poison pill

struct magnitude_t {
  template<typename T>
  [[nodiscard]] constexpr Scalar auto operator()(const T& vec) const
    requires requires { vec.magnitude(); } || requires { magnitude(vec); } ||
             (!Scalar<T> && (requires { vec.norm(); } || requires { norm(vec); })) ||
             (RealScalar<T> && (std::is_arithmetic_v<T> || requires { vec.abs(); } || requires { abs(vec); })) ||
             ComplexScalar<T>
  {
    if constexpr (requires { vec.magnitude(); })
      return vec.magnitude();
    else if constexpr (requires { magnitude(vec); })
      return magnitude(vec);
    // fallback: linear algebra libraries use norm() for the L2 magnitude;
    // guarded by !Scalar to exclude both real and complex types (std::norm(complex) returns |z|² not |z|)
    else if constexpr (!Scalar<T> && requires { vec.norm(); })
      return vec.norm();
    else if constexpr (!Scalar<T> && requires { norm(vec); })
      return norm(vec);
    // allow real types to represent one dimensional vector quantities
    else if constexpr (std::is_arithmetic_v<T>)
#if MP_UNITS_HOSTED || __cpp_lib_freestanding_cstdlib >= 202306L
      return std::abs(vec);
#else
      return vec >= 0 ? vec : -vec;
#endif
    // allow complex scalars to represent one dimensional complex vector quantities: the L2 magnitude
    // of a 1D complex vector is the modulus |z| of its single component
    else if constexpr (ComplexScalar<T>)
      return ::mp_units::modulus(vec);
    else if constexpr (requires { vec.abs(); })
      return vec.abs();
    else if constexpr (requires { abs(vec); })
      return abs(vec);
  }
};

}  // namespace detail::magnitude_impl

inline namespace cpo {

MP_UNITS_EXPORT inline constexpr ::mp_units::detail::magnitude_impl::magnitude_t magnitude;

}


/////////////// REPRESENTATION ///////////////

namespace detail {

// treat_as_floating_point (not std::floating_point) is intentional: it is the library's
// extensibility point for user-defined floating-point-like types (e.g. a fixed-size float
// wrapper).  scale_fp() computes the magnitude constant via get_value<long double> and
// then silent_cast<element_t>()s it, so element_t must be constructible from long double.
// The concept also covers container types whose element type is floating-point (e.g.
// cartesian_vector<double>): treat_as_floating_point<value_type_t<T>> is checked so that
// the FP scaling path is taken even when T itself is not floating-point.
//
// The scaled result need not be `T` itself, nor even weakly-regular: expression-template
// linear algebra libraries (e.g. Eigen, Blaze) return lazy proxy types from `operator*` /
// `operator/` rather than a materialized vector/matrix.  `scale()` always materializes the
// result via `static_cast<To>` (direct-initialization), so it is sufficient that `T` be
// constructible from the scaled result.
//
// `constructible_from` (rather than the `common_with` used by `Addable` / `ScalableWith`) is
// deliberate: it is the exact precondition of `static_cast<To>` and nothing more. In particular it
// admits explicit-only conversions - e.g. the rational integer path widens `safe_int<int>` to
// `safe_int<int64_t>` and narrows back, which is `constructible_from` but neither `convertible_to`
// nor required to be `common_with`. `Addable` uses `common_with` because its results instead feed
// the `common_type`-based quantity machinery, where a common-reference relationship is what matters.
template<typename T>
concept UsesFloatingPointScaling =
  (treat_as_floating_point<T> || treat_as_floating_point<value_type_t<T>>) &&
  std::constructible_from<value_type_t<T>, long double> && requires(T value, value_type_t<T> f) {
    requires std::constructible_from<T, decltype(value * f)>;
    requires std::constructible_from<T, decltype(value / f)>;
  };

// detail::integral (not std::integral) is required so that int128_t / uint128_t are
// accepted as element types.  On GCC in strict mode (-std=c++20), std::integral<__int128>
// = false because the standard traits are not specialized for __int128 outside of GNU
// extensions; when __SIZEOF_INT128__ is absent, int128_t / uint128_t are double_width_int<>
// emulations that also do not satisfy std::integral.  detail::integral patches both gaps.
// The scaling engine internals (get_value<element_t>, wider_int_for<element_t>,
// fixed_point<element_t>) are all specialized for int128_t / uint128_t, so the full
// scaling pipeline works correctly once the concept admits those types.
//
// The concept covers both plain arithmetic types (int, long, int128_t) and
// wrappers/containers whose element type is a supported integer (safe_int<int>,
// cartesian_vector<int>): the scaling engine uses the type's own operator* / operator/
// so that wrappers can check for overflow, containers can scale element-wise, etc.
//
// The rational-magnitude path in detail::scale_int multiplies by a factor of type
// wider_int_for<element_t> (e.g. int64_t for signed int16_t, uint64_t for uint16_t) to
// avoid overflowing the intermediate. The concept therefore requires `value * WF` and
// `value / WF` for the wider factor.
//
// As in UsesFloatingPointScaling, the result is materialized via `static_cast<To>`, so we require
// `T` to be constructible from it rather than (implicitly) convertible.  This matters because the
// integer path deliberately produces a *wider* result (e.g. safe_int<int> * int64_t yields
// safe_int<int64_t>) that narrows back to `T` only explicitly.
template<typename T>
concept UsesIntegerScaling = integral<value_type_t<T>> && requires(T value, wider_int_for<value_type_t<T>> wf) {
  requires std::constructible_from<T, decltype(value * wf)>;
  requires std::constructible_from<T, decltype(value / wf)>;
};

// A type that provides its own magnitude-aware operator*(T, UnitMagnitude) customization
// point.  scale() will prefer this path when available, and the return type may differ
// from the input (e.g. a wrapper with scaled bounds).
template<typename T>
concept UsesUnitMagnitudeAwareScaling = requires(const T& v) { v * mag<1>; };

/**
 * @brief UnitMagnitudeScalable
 *
 * A type is `UnitMagnitudeScalable` if the library's `scale()` can apply a unit
 * magnitude ratio to it.  The three sub-concepts map to the scaling paths:
 *
 *  - `UsesUnitMagnitudeAwareScaling<T>` — type provides `operator*(T, UnitMagnitude)` (preferred)
 *  - `UsesFloatingPointScaling<T>`  — floating-point type or container thereof
 *  - `UsesIntegerScaling<T>`        — integer type, wrapper, or container thereof
 */
template<typename T>
concept UnitMagnitudeScalable =
  WeaklyRegular<T> && (UsesUnitMagnitudeAwareScaling<T> || UsesFloatingPointScaling<T> || UsesIntegerScaling<T>);

// A type that exposes an L2 magnitude (the `magnitude` CPO) and is scalable by it. This is the
// container algebra a vector or tensor needs on top of being regularly addable; for an order-0
// scalar the analogous contracts live in `RealScalar` / `ComplexScalar`.
template<typename T>
concept HasMagnitude = requires(const T& v) {
  ::mp_units::magnitude(v);
  requires ScalableWith<T, decltype(::mp_units::magnitude(v))>;
};

// The order axis as rank-ordered character concepts: a `Scalar` is also a degenerate `Vector` and
// `Tensor`, and a `Vector` a degenerate `Tensor` (ISO 80000-2:2019, 18). `Scalar` carries the
// per-field element algebra; an order >= 1 representation adds regular addition and an L2 magnitude
// (its element contracts are then enforced by the container type itself, e.g.
// `cartesian_vector<detail::Scalar T>`). These deliberately omit the representation-validity checks
// (`disable_representation`, `UnitMagnitudeScalable`) so that in V3 they also classify a quantity by
// its character.
template<typename T>
concept Vector = Scalar<T> || (tensor_order<T> == 1 && RegularAddable<T> && HasMagnitude<T>);

template<typename T>
concept Tensor = Vector<T> || (tensor_order<T> == 2 && RegularAddable<T> && HasMagnitude<T>);

// A representation is a character that is also valid `quantity` storage: it is not opted out of
// being a representation (`disable_representation`) and the library can scale it by a unit magnitude.
// The order axis is rank-ordered through the character concepts above, so a lower-order
// representation fills a higher-order slot.
//
// `RepresentationBaseline` is only a floor, not a verdict: it captures the character-independent
// part shared by every representation, and a type satisfying it is still not a representation until
// it also matches a character below. It leads each conjunction deliberately. The character concepts
// carry no validity guard (so that in V3 they can also classify a *quantity* by its character), so
// for a `quantity` argument `Scalar`/`Vector`/`Tensor` would instantiate the quantity's own
// `operator+` / `operator==` whose constraints recurse back into `RepresentationOf` - a satisfaction
// cycle. `disable_representation` is `true` for a quantity, so the leading guard rejects it before any
// character (and thus any operator) is evaluated.
template<typename T>
concept RepresentationBaseline = !disable_representation<T> && UnitMagnitudeScalable<T>;

template<typename T>
concept ScalarRepresentation = RepresentationBaseline<T> && Scalar<T>;

template<typename T>
concept VectorRepresentation = RepresentationBaseline<T> && Vector<T>;

template<typename T>
concept TensorRepresentation = RepresentationBaseline<T> && Tensor<T>;

template<typename T>
concept SomeRepresentation = TensorRepresentation<T>;

// The two matching axes. Order is rank-ordered: `T` fills a slot of equal-or-higher order. Field is
// disjoint and matched exactly: a real quantity needs a real representation and a complex one a
// complex representation.
template<typename T, quantity_tensor_order Order>
concept RepresentationOfOrder = (Order == quantity_tensor_order::scalar && ScalarRepresentation<T>) ||
                                (Order == quantity_tensor_order::vector && VectorRepresentation<T>) ||
                                (Order == quantity_tensor_order::tensor && TensorRepresentation<T>);

template<typename T, quantity_field Field>
concept RepresentationOfField = SomeRepresentation<T> && (numeric_field<T> == Field);

[[nodiscard]] consteval quantity_tensor_order order_of(quantity_character ch) { return ch.order; }
[[nodiscard]] consteval quantity_field field_of(quantity_character ch) { return ch.field; }

// Match a representation `T` against a character `V`: a bare axis constrains only its own
// axis (the other is free), a full `quantity_character` (or the legacy flat enum) constrains both.
MP_UNITS_DIAGNOSTIC_PUSH
MP_UNITS_DIAGNOSTIC_IGNORE_DEPRECATED
template<typename T, auto V>
concept RepresentationOfCharacter =
  (std::same_as<MP_UNITS_REMOVE_CONST(decltype(V)), quantity_tensor_order> && RepresentationOfOrder<T, V>) ||
  (std::same_as<MP_UNITS_REMOVE_CONST(decltype(V)), quantity_field> && RepresentationOfField<T, V>) ||
  ((std::same_as<MP_UNITS_REMOVE_CONST(decltype(V)), quantity_character> ||
    std::same_as<MP_UNITS_REMOVE_CONST(decltype(V)), quantity_character_legacy>) &&
   RepresentationOfOrder<T, order_of(V)> && RepresentationOfField<T, field_of(V)>);
MP_UNITS_DIAGNOSTIC_POP

}  // namespace detail

#ifdef MP_UNITS_XCODE15_HACKS
MP_UNITS_EXPORT template<typename T, auto V>
concept RepresentationOf =
  detail::SomeRepresentation<T> && ((QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(V))> &&
                                     (detail::QuantityKindSpec<MP_UNITS_REMOVE_CONST(decltype(V))> ||
                                      (detail::RepresentationOfOrder<T, detail::order_of(get_character(V))> &&
                                       detail::RepresentationOfField<T, detail::field_of(get_character(V))>))) ||
                                    detail::RepresentationOfCharacter<T, V>);

#else

MP_UNITS_EXPORT template<typename T, auto V>
concept RepresentationOf =
  (QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(V))> &&
   ((detail::QuantityKindSpec<MP_UNITS_REMOVE_CONST(decltype(V))> && detail::SomeRepresentation<T>) ||
    (detail::RepresentationOfOrder<T, detail::order_of(get_character(V))> &&
     detail::RepresentationOfField<T, detail::field_of(get_character(V))>))) ||
  detail::RepresentationOfCharacter<T, V>;
#endif

}  // namespace mp_units
