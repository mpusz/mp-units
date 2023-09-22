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

#include <mp-units/customization_points.h>
#include <concepts>
#include <cstdint>
#include <functional>
#include <type_traits>

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
 * Vectors are physical quantities that possess both magnitude and direction
 * and whose operations obey the axioms of a vector space.
 *
 * Tensors can be used to describe more general physical quantities.
 * For example, the Cauchy stress tensor possess magnitude, direction,
 * and orientation qualities.
 */
enum class quantity_character { scalar, vector, tensor };

namespace detail {

template<typename T>
concept Scalar = is_scalar<T>;

template<typename T>
concept Vector = is_vector<T>;

template<typename T>
concept Tensor = is_tensor<T>;

template<typename T, quantity_character Ch>
concept IsOfCharacter =
  (Ch == quantity_character::scalar && is_scalar<T>) || (Ch == quantity_character::vector && is_vector<T>) ||
  (Ch == quantity_character::tensor && is_tensor<T>);

template<typename T>
[[nodiscard]] consteval quantity_character get_character(T)
{
  if constexpr (Tensor<T>)
    return quantity_character::tensor;
  else if constexpr (Vector<T>)
    return quantity_character::vector;
  else if constexpr (Scalar<T>)
    return quantity_character::scalar;
}

// clang-format off
template<typename T>
concept ScalarRepresentation = std::regular<T> && Scalar<T> &&
  requires(T a, T b, std::intmax_t i, long double f) {
    { a * i } -> Scalar;
    { i * a } -> Scalar;
    { a / i } -> Scalar;
    { a * f } -> Scalar;  // TODO How this affects freestanding?
    { f * a } -> Scalar;
    { a / f } -> Scalar;
    { a + b } -> Scalar;
    { a - b } -> Scalar;
    { a * b } -> Scalar;
    { a / b } -> Scalar;
  };

template<typename T>
concept VectorRepresentation = std::regular<T> && Vector<T> &&
  requires(T a, T b, std::intmax_t i, long double f) {
    { a * i } -> Vector;
    { i * a } -> Vector;
    { a / i } -> Vector;
    { a * f } -> Vector;
    { f * a } -> Vector;
    { a / f } -> Vector;
    { a + b } -> Vector;
    { a - b } -> Vector;
    { dot_product(a, b) } -> Scalar;
    { cross_product(a, b) } -> Vector;
    { tensor_product(a, b) } -> Tensor;
    { norm(a) } -> Scalar;
  };

template<typename T>
concept TensorRepresentation = std::regular<T> && Tensor<T> &&
  requires(T a, T b, std::intmax_t i, long double f) {
    { a * i } -> Tensor;
    { i * a } -> Tensor;
    { a / i } -> Tensor;
    { a * f } -> Tensor;
    { f * a } -> Tensor;
    { a / f } -> Tensor;
    { a + b } -> Tensor;
    { a - b } -> Tensor;
    { tensor_product(a, b) } -> Tensor;
    { inner_product(a, b) } -> Tensor;
    { scalar_product(a, b) } -> Scalar;
  };
// clang-format on

}  // namespace detail

template<typename T>
concept Representation =
  detail::ScalarRepresentation<T> || detail::VectorRepresentation<T> || detail::TensorRepresentation<T>;

template<typename T, quantity_character Ch>
concept RepresentationOf = detail::IsOfCharacter<T, Ch> && Representation<T>;

}  // namespace mp_units
