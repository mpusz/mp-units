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
#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <type_traits>
#endif  // MP_UNITS_IMPORT_STD
#endif  // MP_UNITS_IN_MODULE_INTERFACE

namespace mp_units {

namespace detail {

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
concept NumberLike = Addable<T>
#if MP_UNITS_COMP_GCC != 12 && !defined(MP_UNITS_XCODE15_HACKS)
                     && WeaklyRegular<T>
#endif
  ;

template<typename T>
concept BaseScalar = NumberLike<T> && ScalableWith<T, T>;

}  // namespace detail


/////////////// REAL SCALAR ///////////////

MP_UNITS_EXPORT template<typename T>
constexpr bool disable_real = false;

template<>
MP_UNITS_INLINE constexpr bool disable_real<bool> = true;

namespace detail {

template<typename T>
concept RealScalar = (!disable_real<T>) && BaseScalar<T> && std::totally_ordered<T>;

}


/////////////// COMPLEX SCALAR ///////////////

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

namespace detail {

template<typename T>
concept ComplexScalar = requires(const T v, const T& ref) {
  requires std::constructible_from<T, decltype(::mp_units::real(ref)), decltype(::mp_units::imag(ref))>;
  ::mp_units::real(v);
  ::mp_units::imag(v);
  ::mp_units::modulus(v);
  requires ScalableWith<T, decltype(::mp_units::modulus(v))>;
} && BaseScalar<T>;

template<typename T>
concept Scalar = RealScalar<T> || ComplexScalar<T>;

}  // namespace detail


/////////////// VECTOR ///////////////

namespace detail::magnitude_impl {

void magnitude() = delete;  // poison pill
void abs() = delete;        // poison pill

struct magnitude_t {
  template<typename T>
  [[nodiscard]] constexpr Scalar auto operator()(const T& vec) const
    requires requires { vec.magnitude(); } || requires { magnitude(vec); } ||
             (RealScalar<T> && (std::is_arithmetic_v<T> || requires { vec.abs(); } || requires { abs(vec); }))
  {
    if constexpr (requires { vec.magnitude(); })
      return vec.magnitude();
    else if constexpr (requires { magnitude(vec); })
      return magnitude(vec);
    // allow real types to represent one dimensional vector quantities
    if constexpr (std::is_arithmetic_v<T>)
#if MP_UNITS_HOSTED || __cpp_lib_freestanding_cstdlib >= 202306L
      return std::abs(vec);
#else
      return vec >= 0 ? vec : -vec;
#endif
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

namespace detail {

template<typename T>
concept Vector = requires(const T v) {
  ::mp_units::magnitude(v);
  requires ScalableWith<T, decltype(::mp_units::magnitude(v))>;
  // TODO should we also check for the below (e.g., when `size() > 1` or `2`)
  // ::mp_units::zero_vector<T>();
  // ::mp_units::scalar_product(a, b);
  // ::mp_units::vector_product(a, b);
  // ::mp_units::tensor_product(a, b);
} && NumberLike<T>;

}  // namespace detail

/////////////// TENSOR ///////////////

// MP_UNITS_EXPORT template<typename T>
// constexpr bool disable_tensor = false;

// TODO provide when some actual operations will be required
// template<typename T>
// concept Tensor = (!disable_tensor<T>) && WeaklyRegular<T> && requires(const T a, const T b) {
//   ::mp_units::tensor_product(a, b);
//   ::mp_units::inner_product(a, b);
//   ::mp_units::scalar_product(a, b);
// };


namespace detail {

template<typename T>
constexpr bool is_quantity_like = false;

// TODO how can we use `(!Quantity<T>)` below?
template<typename T>
concept NotQuantity = (!is_quantity_like<T>);

// treat_as_floating_point (not std::floating_point) is intentional: it is the library's
// extensibility point for user-defined floating-point-like types (e.g. a fixed-size float
// wrapper).  scale_fp() computes the magnitude constant via get_value<long double> and
// then silent_cast<element_t>()s it, so element_t must be constructible from long double.
// The concept also covers container types whose element type is floating-point (e.g.
// cartesian_vector<double>): treat_as_floating_point<value_type_t<T>> is checked so that
// the FP scaling path is taken even when T itself is not floating-point.
template<typename T>
concept UsesFloatingPointScaling =
  (treat_as_floating_point<T> || treat_as_floating_point<value_type_t<T>>) &&
  std::constructible_from<value_type_t<T>, long double> && requires(T value, value_type_t<T> f) {
    { value * f } -> WeaklyRegular;
    { value / f } -> WeaklyRegular;
  };

// std::integral (not just treat_as_integral) is required because the scaling engine
// relies on get_value<element_t>, wider_int_for<element_t>, and fixed_point<element_t>,
// which are only defined for fundamental integer types.  The concept covers both plain
// arithmetic types (int, long) and wrappers/containers whose element type is a standard
// integer (safe_int<int>, cartesian_vector<int>): the scaling engine uses the type's
// own operator* / operator/ so that wrappers can check for overflow, containers can
// scale element-wise, etc.
//
// The rational-magnitude path in detail::scale_int multiplies by a factor of type
// wider_int_for<element_t> (e.g. int64_t for signed int16_t, uint64_t for uint16_t) to
// avoid overflowing the intermediate. The concept therefore requires `value * WF` and
// `value / WF` for the wider factor.
template<typename T>
concept UsesIntegerScaling = std::integral<value_type_t<T>> && requires(T value, wider_int_for<value_type_t<T>> wf) {
  { value * wf };
  { value / wf };
};

// A type that provides its own magnitude-aware operator*(T, UnitMagnitude) customization
// point.  scale() will prefer this path when available, and the return type may differ
// from the input (e.g. a wrapper with scaled bounds).
template<typename T>
concept UsesMagnitudeAwareScaling = requires(const T& v) { v * mag<1>; };

/**
 * @brief MagnitudeScalable
 *
 * A type is `MagnitudeScalable` if the library's `scale()` can apply a unit
 * magnitude ratio to it.  The three sub-concepts map to the scaling paths:
 *
 *  - `UsesMagnitudeAwareScaling<T>` — type provides `operator*(T, UnitMagnitude)` (preferred)
 *  - `UsesFloatingPointScaling<T>`  — floating-point type or container thereof
 *  - `UsesIntegerScaling<T>`        — integer type, wrapper, or container thereof
 */
template<typename T>
concept MagnitudeScalable =
  WeaklyRegular<T> && (UsesMagnitudeAwareScaling<T> || UsesFloatingPointScaling<T> || UsesIntegerScaling<T>);

template<typename T>
concept RealScalarRepresentation = NotQuantity<value_type_t<T>> && RealScalar<T> && MagnitudeScalable<T>;

template<typename T>
concept ComplexScalarRepresentation = NotQuantity<value_type_t<T>> && ComplexScalar<T> && MagnitudeScalable<T>;

template<typename T>
concept ScalarRepresentation = RealScalarRepresentation<T> || ComplexScalarRepresentation<T>;

template<typename T>
concept VectorRepresentation = NotQuantity<value_type_t<T>> && Vector<T> && MagnitudeScalable<T>;

// template<typename T>
// concept TensorRepresentation = NotQuantity<T> && Tensor<T>;

template<typename T, quantity_character Ch>
concept IsOfCharacter = (Ch == quantity_character::real_scalar && RealScalarRepresentation<T>) ||
                        (Ch == quantity_character::complex_scalar && ComplexScalarRepresentation<T>) ||
                        (Ch == quantity_character::vector && VectorRepresentation<T>);
// || (Ch == quantity_character::tensor && TensorRepresentation<T>);

template<typename T>
concept SomeRepresentation =
  detail::ScalarRepresentation<T> || detail::VectorRepresentation<T>;  // || detail::TensorRepresentation<T>;

}  // namespace detail

#ifdef MP_UNITS_XCODE15_HACKS
MP_UNITS_EXPORT template<typename T, auto V>
concept RepresentationOf =
  detail::SomeRepresentation<T> &&
  ((QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(V))> &&
    (detail::QuantityKindSpec<MP_UNITS_REMOVE_CONST(decltype(V))> || detail::IsOfCharacter<T, V.character>)) ||
   (std::same_as<quantity_character, decltype(V)> && detail::IsOfCharacter<T, V>));

#else

MP_UNITS_EXPORT template<typename T, auto V>
concept RepresentationOf =
  (QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(V))> &&
   ((detail::QuantityKindSpec<MP_UNITS_REMOVE_CONST(decltype(V))> && detail::SomeRepresentation<T>) ||
    detail::IsOfCharacter<T, V.character>)) ||
  (std::same_as<quantity_character, MP_UNITS_REMOVE_CONST(decltype(V))> && detail::IsOfCharacter<T, V>);
#endif

}  // namespace mp_units
