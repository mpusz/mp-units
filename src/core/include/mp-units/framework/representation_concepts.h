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
#include <functional>
#include <type_traits>
#endif
#endif

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

namespace detail {

template<typename T>
concept WeaklyRegular = std::copyable<T> && std::equality_comparable<T>;

template<typename T>
concept Scalar = is_scalar<T>;

template<typename T>
concept Complex = is_complex<T>;

template<typename T>
concept Vector = is_vector<T>;

template<typename T>
concept Tensor = is_tensor<T>;

template<typename T, quantity_character Ch>
constexpr bool is_of_character_impl = false;
template<typename T>
constexpr bool is_of_character_impl<T, quantity_character::scalar> = is_scalar<T>;
template<typename T>
constexpr bool is_of_character_impl<T, quantity_character::complex> = is_complex<T>;
template<typename T>
constexpr bool is_of_character_impl<T, quantity_character::vector> = is_vector<T>;
template<typename T>
constexpr bool is_of_character_impl<T, quantity_character::tensor> = is_tensor<T>;

template<typename T, quantity_character Ch>
concept IsOfCharacter = is_of_character_impl<T, Ch>;

template<typename T>
using scaling_factor_type_t = conditional<treat_as_floating_point<T>, long double, std::intmax_t>;

template<typename T>
concept ScalarRepresentation = Scalar<T> && WeaklyRegular<T> && requires(T a, T b, scaling_factor_type_t<T> f) {
  // scaling
  { a* f } -> Scalar;
  { f* a } -> Scalar;
  { a / f } -> Scalar;

  // scalar operations
  { -a } -> Scalar;
  { a + b } -> Scalar;
  { a - b } -> Scalar;
  { a* b } -> Scalar;
  { a / b } -> Scalar;
};

template<typename T>
concept ComplexRepresentation = Complex<T> && WeaklyRegular<T> && requires(T a, T b, scaling_factor_type_t<T> f) {
  // scaling
  // TODO The below conversion to `T` is an exception compared to other representation types
  // `std::complex<T>` * `U` do not work, but `std::complex<T>` is convertible from `U`
  // Maybe expose this as a customization point?
  { a* T(f) } -> Complex;
  { T(f) * a } -> Complex;
  { a / T(f) } -> Complex;

  // complex operations
  { -a } -> Complex;
  { a + b } -> Complex;
  { a - b } -> Complex;
  { a* b } -> Complex;
  { a / b } -> Complex;
  { real(a) } -> Scalar;
  { imag(a) } -> Scalar;
  { abs(a) } -> Scalar;
  { arg(a) } -> Scalar;
  { conj(a) } -> Complex;
};

// TODO how to check for a complex(Scalar, Scalar) -> Complex?

template<typename T>
concept VectorRepresentation = Vector<T> && WeaklyRegular<T> && requires(T a, T b, scaling_factor_type_t<T> f) {
  // scaling
  { a* f } -> Vector;
  { f* a } -> Vector;
  { a / f } -> Vector;

  // vector operations
  { -a } -> Vector;
  { a + b } -> Vector;
  { a - b } -> Vector;
  // TBD
  // { norm(a) } -> Scalar;
  // { zero_vector<T>() } -> Vector;
  // { unit_vector(a) } -> Vector;
  // { scalar_product(a, b) } -> Scalar;
  // { vector_product(a, b) } -> Vector;
  // { tensor_product(a, b) } -> Tensor2;
  // divergence(a)
  // rotation(a)
};

template<typename T>
concept TensorRepresentation = Tensor<T> && WeaklyRegular<T>;  // && requires(T a, T b) {
                                                               // TBD
                                                               // tensor operations
                                                               // { tensor_product(a, b) } -> Tensor4;
                                                               // { inner_product(a, b) } -> Tensor2;
                                                               // { scalar_product(a, b) } -> Scalar;
//};

}  // namespace detail

MP_UNITS_EXPORT template<typename T>
concept Representation = detail::ScalarRepresentation<T> || detail::ComplexRepresentation<T> ||
                         detail::VectorRepresentation<T> || detail::TensorRepresentation<T>;

MP_UNITS_EXPORT template<typename T, auto V>
concept RepresentationOf =
  Representation<T> &&
  ((QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(V))> &&
    (detail::QuantityKindSpec<MP_UNITS_REMOVE_CONST(decltype(V))> || detail::IsOfCharacter<T, V.character>)) ||
   (std::same_as<quantity_character, decltype(V)> && detail::IsOfCharacter<T, V>));

}  // namespace mp_units
