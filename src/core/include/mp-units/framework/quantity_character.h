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
#include <mp-units/bits/hacks.h>
#include <mp-units/bits/module_macros.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cstdint>
#endif  // MP_UNITS_IMPORT_STD
#endif  // MP_UNITS_IN_MODULE_INTERFACE

namespace mp_units {

// A quantity's character splits into two orthogonal axes: its *tensor order* (scalar, vector,
// tensor) and its *numeric field* (real, complex). The enumerators are declared in increasing
// rank so that the underlying comparison orders them (scalar < vector < tensor, real < complex).
MP_UNITS_EXPORT enum class quantity_tensor_order : std::int8_t { scalar, vector, tensor };
MP_UNITS_EXPORT enum class quantity_field : std::int8_t { real, complex };

// The pre-2.6.0 flat character enumeration, kept as a user-facing compatibility spelling
// (`quantity_character::real_scalar`, `::vector`, ...). The library itself uses the two-axis
// form; this enum is provided only so existing user code keeps compiling. It is deprecated to
// steer new code toward the `quantity_tensor_order` and `quantity_field` axes.
#define MP_UNITS_QCL_DEPRECATED \
  [[deprecated("2.6.0: use the 'quantity_tensor_order' and 'quantity_field' axes instead")]]
MP_UNITS_EXPORT enum class MP_UNITS_QCL_DEPRECATED quantity_character_legacy : std::int8_t {
  real_scalar MP_UNITS_QCL_DEPRECATED,
  complex_scalar MP_UNITS_QCL_DEPRECATED,
  vector MP_UNITS_QCL_DEPRECATED,
  tensor MP_UNITS_QCL_DEPRECATED
};
#undef MP_UNITS_QCL_DEPRECATED

/**
 * @brief Quantity character
 *
 * A quantity character classifies a quantity along two orthogonal axes:
 *
 * - its *tensor order* (`quantity_tensor_order`): scalar (order 0), vector (order 1), or
 *   tensor (order 2), and
 * - its *numeric field* (`quantity_field`): real or complex.
 *
 * Scalars, vectors, and tensors are mathematical objects independent of the choice of a
 * coordinate system, whereas their components depend on it. A scalar has magnitude but no
 * direction; a vector possesses both magnitude and direction; a tensor (for example the Cauchy
 * stress tensor) additionally carries orientation. The numeric field separately distinguishes real
 * quantities from complex ones (such as a voltage phasor or a complex permittivity), which affects
 * the operations a quantity admits.
 *
 * The pre-2.6.0 flat spelling (`quantity_character::real_scalar`, `::complex_scalar`, `::vector`,
 * `::tensor`) remains available for backward compatibility and maps onto the two axes.
 */
MP_UNITS_EXPORT struct quantity_character {
  MP_UNITS_DIAGNOSTIC_PUSH
  MP_UNITS_DIAGNOSTIC_IGNORE_DEPRECATED
  using enum quantity_character_legacy;
  MP_UNITS_DIAGNOSTIC_POP

  quantity_tensor_order order = quantity_tensor_order::scalar;
  quantity_field field = quantity_field::real;

  consteval quantity_character() = default;
  consteval quantity_character(quantity_tensor_order tensor_order, quantity_field numeric_field) :
      order(tensor_order), field(numeric_field)
  {
  }
  consteval quantity_character(quantity_tensor_order tensor_order) : order(tensor_order) {}
  consteval quantity_character(quantity_field numeric_field) : field(numeric_field) {}
  MP_UNITS_DIAGNOSTIC_PUSH
  MP_UNITS_DIAGNOSTIC_IGNORE_DEPRECATED
  consteval quantity_character(quantity_character_legacy legacy)
  {
    switch (legacy) {
      case real_scalar:
        order = quantity_tensor_order::scalar;
        field = quantity_field::real;
        break;
      case complex_scalar:
        order = quantity_tensor_order::scalar;
        field = quantity_field::complex;
        break;
      case vector:
        order = quantity_tensor_order::vector;
        field = quantity_field::real;
        break;
      case tensor:
        order = quantity_tensor_order::tensor;
        field = quantity_field::real;
        break;
    }
  }
  MP_UNITS_DIAGNOSTIC_POP

  // Lexicographic on (order, field). Matches the pre-2.6.0 enum ordering
  // (real_scalar < complex_scalar < vector < tensor), so `max`-based character combination is
  // unchanged. `operator==` also enables use as a non-type template argument.
  [[nodiscard]] friend constexpr auto operator<=>(quantity_character, quantity_character) = default;
};

}  // namespace mp_units
