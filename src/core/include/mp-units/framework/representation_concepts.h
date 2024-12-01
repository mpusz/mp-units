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
 * A complex is a physical quantity that is represented with a complex number.
 *
 * Vectors are physical quantities that possess both magnitude and direction
 * and whose operations obey the axioms of a vector space.
 *
 * Tensors can be used to describe more general physical quantities.
 * For example, the Cauchy stress tensor possess magnitude, direction,
 * and orientation qualities.
 */
MP_UNITS_EXPORT enum class quantity_character : std::int8_t { scalar, complex, vector, tensor };

MP_UNITS_EXPORT template<typename T>
constexpr bool disable_scalar = false;

template<>
MP_UNITS_INLINE constexpr bool disable_scalar<bool> = true;

namespace detail {

template<typename T>
concept WeaklyRegular = std::copyable<T> && std::equality_comparable<T>;

template<typename T, typename S>
concept ScalableWith = WeaklyRegular<T> && requires(T v, S s) {
  { v* s / s } -> std::common_with<T>;
  { s* v / s } -> std::common_with<T>;
  { v / s* s } -> std::common_with<T>;
};

template<typename T>
concept Scalar = (!disable_scalar<T>) && WeaklyRegular<T> && requires(T a, T b) {
  { -a } -> std::common_with<T>;
  { a + b } -> std::common_with<T>;
  { a - b } -> std::common_with<T>;
} && ScalableWith<T, T>;

namespace real_impl {

void real() = delete;  // poison pill

struct real_t {
  [[nodiscard]] constexpr Scalar auto operator()(const WeaklyRegular auto& clx) const
    requires requires { clx.real(); } || requires { real(clx); }
  {
    if constexpr (requires { clx.real(); })
      return clx.real();
    else if constexpr (requires { real(clx); })
      return real(clx);
  }
};

}  // namespace real_impl

}  // namespace detail

inline namespace cpo {

MP_UNITS_EXPORT inline constexpr ::mp_units::detail::real_impl::real_t real;

}

namespace detail::imag_impl {

void imag() = delete;  // poison pill

struct imag_t {
  [[nodiscard]] constexpr Scalar auto operator()(const WeaklyRegular auto& clx) const
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
  [[nodiscard]] constexpr Scalar auto operator()(const WeaklyRegular auto& clx) const
    requires requires { clx.modulus(); } || requires { modulus(clx); } || requires { clx.abs(); } ||
             requires { abs(clx); }
  {
    if constexpr (requires { clx.modulus(); })
      return clx.modulus();
    else if constexpr (requires { modulus(clx); })
      return modulus(clx);
    // `std` made a precedence of using `abs` for modulo on `std::complex`
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

MP_UNITS_EXPORT template<typename T>
constexpr bool disable_complex = false;

namespace detail {

template<typename T>
concept Complex = (!disable_complex<T>) && WeaklyRegular<T> && requires(T a, T b) {
  // complex operations
  { -a } -> std::common_with<T>;
  { a + b } -> std::common_with<T>;
  { a - b } -> std::common_with<T>;
  { a* b } -> std::common_with<T>;
  { a / b } -> std::common_with<T>;
  ::mp_units::real(a);
  ::mp_units::imag(a);
  ::mp_units::modulus(a);
  requires ScalableWith<T, decltype(::mp_units::modulus(a))>;
  requires std::constructible_from<T, decltype(::mp_units::real(a)), decltype(::mp_units::imag(a))>;
};

namespace magnitude_impl {

void magnitude() = delete;  // poison pill
void abs() = delete;        // poison pill

struct magnitude_t {
  template<WeaklyRegular T>
  [[nodiscard]] constexpr Scalar auto operator()(const T& vec) const
    requires requires { vec.magnitude(); } || requires { magnitude(vec); } ||
             (Scalar<T> &&
              (requires { vec.abs(); } || requires { abs(vec); } || (std::is_arithmetic_v<T> && (!is_same_v<T, bool>))))
  {
    if constexpr (requires { vec.magnitude(); })
      return vec.magnitude();
    else if constexpr (requires { magnitude(vec); })
      return magnitude(vec);
    // allow scalar types to represent one dimensional vector quantities
    if constexpr (Scalar<T>) {
      if constexpr (requires { vec.abs(); })
        return vec.abs();
      else if constexpr (requires { abs(vec); })
        return abs(vec);
      else if constexpr (std::is_arithmetic_v<T> && (!is_same_v<T, bool>))
#if MP_UNITS_HOSTED || __cpp_lib_freestanding_cstdlib >= 202306L
        return std::abs(vec);
#else
        return vec >= 0 ? vec : -vec;
#endif
    }
  }
};

}  // namespace magnitude_impl

}  // namespace detail

inline namespace cpo {

MP_UNITS_EXPORT inline constexpr ::mp_units::detail::magnitude_impl::magnitude_t magnitude;

}

MP_UNITS_EXPORT template<typename T>
constexpr bool disable_vector = false;

namespace detail {

template<typename T>
concept Vector = (!disable_vector<T>) && WeaklyRegular<T> && requires(T a, T b) {
  // vector operations
  { -a } -> std::common_with<T>;
  { a + b } -> std::common_with<T>;
  { a - b } -> std::common_with<T>;
  ::mp_units::magnitude(a);
  requires ScalableWith<T, decltype(::mp_units::magnitude(a))>;
  // TODO should we also check for the below (e.g., when `size() > 1` or `2`)
  // { zero_vector<T>() } -> Vector;
  // { unit_vector(a) } -> Vector;
  // { scalar_product(a, b) } -> Scalar;
  // { vector_product(a, b) } -> Vector;
  // { tensor_product(a, b) } -> Tensor2;
};

// TODO provide when some actual operations will be required
// template<typename T>
// concept Tensor = is_tensor<T> && WeaklyRegular<T>;  // && requires(T a, T b) {
//                                                     // tensor operations
//                                                     // { tensor_product(a, b) } -> Tensor4;
//                                                     // { inner_product(a, b) } -> Tensor2;
//                                                     // { scalar_product(a, b) } -> Scalar;
// //};

template<typename T>
constexpr bool is_quantity = false;

template<typename T>
using scaling_factor_type_t = conditional<treat_as_floating_point<T>, long double, std::intmax_t>;

// TODO how can we use `(!Quantity<T>)` below?
template<typename T>
concept ScalarRepresentation = (!is_quantity<T>) && Scalar<T> && requires(T v, scaling_factor_type_t<T> f) {
  // scaling
  { v* f } -> std::common_with<T>;
  { f* v } -> std::common_with<T>;
  { v / f } -> std::common_with<T>;
};

template<typename T>
concept ComplexRepresentation = (!is_quantity<T>) && Complex<T> && requires(T v, scaling_factor_type_t<T> f) {
  // scaling
  // TODO The below conversion to `T` is an exception compared to other representation types
  // `std::complex<T>` * `U` do not work, but `std::complex<T>` is convertible from `U`
  // Maybe expose this as a customization point?
  { v* T(f) } -> std::common_with<T>;
  { T(f) * v } -> std::common_with<T>;
  { v / T(f) } -> std::common_with<T>;
};

template<typename T>
concept VectorRepresentation = (!is_quantity<T>) && Vector<T> && requires(T v, scaling_factor_type_t<T> f) {
  // scaling
  { v* f } -> std::common_with<T>;
  { f* v } -> std::common_with<T>;
  { v / f } -> std::common_with<T>;
};

// template<typename T>
// concept TensorRepresentation = (!is_quantity<T>) && Tensor<T>;

template<typename T, quantity_character Ch>
concept IsOfCharacter =
  (Ch == quantity_character::scalar && Scalar<T>) || (Ch == quantity_character::complex && Complex<T>) ||
  (Ch == quantity_character::vector && Vector<T>);  // || (Ch == quantity_character::tensor && Tensor<T>);

}  // namespace detail

MP_UNITS_EXPORT template<typename T>
concept Representation = detail::ScalarRepresentation<T> || detail::ComplexRepresentation<T> ||
                         detail::VectorRepresentation<T>;  // || detail::TensorRepresentation<T>;

MP_UNITS_EXPORT template<typename T, auto V>
concept RepresentationOf =
  Representation<T> &&
  ((QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(V))> &&
    (detail::QuantityKindSpec<MP_UNITS_REMOVE_CONST(decltype(V))> || detail::IsOfCharacter<T, V.character>)) ||
   (std::same_as<quantity_character, decltype(V)> && detail::IsOfCharacter<T, V>));

}  // namespace mp_units
