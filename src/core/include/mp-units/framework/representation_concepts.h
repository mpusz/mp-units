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
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/quantity_spec_concepts.h>

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

template<typename T>
concept WeaklyRegular = std::copyable<T> && std::equality_comparable<T>;

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

}  // namespace detail

/**
 * @brief Quantity character
 *
 * Scalars, vectors and tensors are mathematical objects that can be used to
 * denote certain physical quantities and their values. They are as such
 * independent of the particular choice of a coordinate system, whereas
 * each scalar component of a vector or a tensor and each component vector and
 * component tensor depend on that choice.
 *
 * A scalar is a physical quantity that has magnitude but no direction. It might
 * be a real or complex number which affects which operations are allowed on a quantity.
 *
 * Vectors are physical quantities that possess both magnitude and direction
 * and whose operations obey the axioms of a vector space.
 *
 * Tensors can be used to describe more general physical quantities.
 * For example, the Cauchy stress tensor possess magnitude, direction,
 * and orientation qualities.
 */
MP_UNITS_EXPORT enum class quantity_character : std::int8_t { real_scalar, complex_scalar, vector, tensor };

/////////////// COMPLEX SCALAR ///////////////

namespace detail::real_impl {

void real() = delete;  // poison pill

struct real_t {
  // TODO how to constrain the return with RealScalar?
  [[nodiscard]] constexpr auto operator()(const WeaklyRegular auto& clx) const
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
  [[nodiscard]] constexpr auto operator()(const WeaklyRegular auto& clx) const
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
  [[nodiscard]] constexpr auto operator()(const WeaklyRegular auto& clx) const
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
concept NumberLike = Addable<T>
#if MP_UNITS_COMP_GCC != 12 && !defined(MP_UNITS_XCODE15_HACKS)
                     && WeaklyRegular<T>
#endif
  ;

template<typename T>
concept BaseScalar = NumberLike<T> && ScalableWith<T, T>;

template<typename T>
concept HasComplexOperations = requires(const T v, const T& ref) {
  requires std::constructible_from<T, decltype(::mp_units::real(ref)), decltype(::mp_units::imag(ref))>;
  ::mp_units::real(v);
  ::mp_units::imag(v);
  ::mp_units::modulus(v);
  requires ScalableWith<T, decltype(::mp_units::modulus(v))>;
};

template<typename T>
concept ComplexScalar =
  // TODO should the below be provided?
  // (!disable_complex<T>) &&
  HasComplexOperations<T> && BaseScalar<T>;

}  // namespace detail


/////////////// REAL SCALAR ///////////////

MP_UNITS_EXPORT template<typename T>
constexpr bool disable_real = false;

template<>
MP_UNITS_INLINE constexpr bool disable_real<bool> = true;

namespace detail {

template<typename T>
concept RealScalar = (!disable_real<T>) && (!HasComplexOperations<T>) && BaseScalar<T> && std::totally_ordered<T>;

template<typename T>
concept Scalar = RealScalar<T> || ComplexScalar<T>;

}  // namespace detail


/////////////// VECTOR ///////////////

namespace detail::magnitude_impl {

void magnitude() = delete;  // poison pill
void abs() = delete;        // poison pill

struct magnitude_t {
  template<WeaklyRegular T>
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
constexpr bool is_quantity = false;

// `std::complex<double>` * `long double` does not work, this is why we use the `value_type` if present and correct
// TODO Remove when P3788 gets accepted
template<typename T>
using scaling_factor_type_t = conditional<std::is_arithmetic_v<value_type_t<T>>, value_type_t<T>,
                                          conditional<treat_as_floating_point<T>, long double, std::intmax_t>>;

// TODO replace the below and above with the logic from #615 when available
template<typename T>
concept ScalableByFactor = requires(const T v, const scaling_factor_type_t<T> f) {
  { v * f } -> std::common_with<T>;
  { f * v } -> std::common_with<T>;
  { v / f } -> std::common_with<T>;
};

// TODO how can we use `(!Quantity<T>)` below?
template<typename T>
concept NotQuantity = (!is_quantity<T>);

template<typename T>
concept RealScalarRepresentation = NotQuantity<T> && RealScalar<T> && ScalableByFactor<T>;

template<typename T>
concept ComplexScalarRepresentation = NotQuantity<T> && ComplexScalar<T> && ScalableByFactor<T>;

template<typename T>
concept ScalarRepresentation = RealScalarRepresentation<T> || ComplexScalarRepresentation<T>;

template<typename T>
concept VectorRepresentation = NotQuantity<T> && Vector<T> && ScalableByFactor<T>;

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
  ((QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(V))> &&
    ((detail::QuantityKindSpec<MP_UNITS_REMOVE_CONST(decltype(V))> && detail::SomeRepresentation<T>) ||
     detail::IsOfCharacter<T, V.character>)) ||
   (std::same_as<quantity_character, decltype(V)> && detail::IsOfCharacter<T, V>));
#endif

}  // namespace mp_units
