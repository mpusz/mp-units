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
 * Vectors are physical quantities that possess both magnitude and direction
 * and whose operations obey the axioms of a vector space.
 *
 * Tensors can be used to describe more general physical quantities.
 * For example, the Cauchy stress tensor possess magnitude, direction,
 * and orientation qualities.
 */
MP_UNITS_EXPORT enum class quantity_character : std::int8_t { scalar, vector, tensor };

namespace detail {

template<typename T, typename U>
concept CommonTypeWith =
  std::same_as<std::common_type_t<T, U>, std::common_type_t<U, T>> &&
  std::constructible_from<std::common_type_t<T, U>, T> && std::constructible_from<std::common_type_t<T, U>, U>;

template<typename T, typename U = T>
concept ScalableNumber =
  std::regular_invocable<std::multiplies<>, T, U> && std::regular_invocable<std::divides<>, T, U>;

template<typename T>
concept CastableNumber = CommonTypeWith<T, std::intmax_t> && ScalableNumber<std::common_type_t<T, std::intmax_t>>;

// TODO Fix it according to sudo_cast implementation
template<typename T>
concept Scalable =
  CastableNumber<T> || (requires { typename wrapped_type_t<T>; } && CastableNumber<wrapped_type_t<T>> &&
                        ScalableNumber<T, std::common_type_t<wrapped_type_t<T>, std::intmax_t>>);

template<typename T>
concept WeaklyRegular = std::copyable<T> && std::equality_comparable<T>;

}  // namespace detail

MP_UNITS_EXPORT template<typename T>
concept Representation =
  (is_scalar<T> || is_vector<T> || is_tensor<T>) && detail::WeaklyRegular<T> && detail::Scalable<T>;

MP_UNITS_EXPORT template<typename T, quantity_character Ch>
concept RepresentationOf = Representation<T> && ((Ch == quantity_character::scalar && is_scalar<T>) ||
                                                 (Ch == quantity_character::vector && is_vector<T>) ||
                                                 (Ch == quantity_character::tensor && is_tensor<T>));

}  // namespace mp_units
