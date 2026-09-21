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

// The `mp_units::utility` namespace is the public *extension/authoring* API tier: stable, supported,
// and exported, but intentionally NOT part of the minimal standardization-targeted `mp_units::` API.
// It is what a customer needs to write their own representation types / tools the way the library
// does. `detail` stays private (never exported); the `utility` concepts below are independent public
// definitions that merely delegate to the private ones, so the standardization surface remains
// exactly `mp_units::` minus `mp_units::utility`.

#include <mp-units/bits/module_macros.h>
#include <mp-units/framework/representation_concepts.h>

namespace mp_units::utility {

// The value-classification concepts a customer needs to build, test, or constrain their own
// abstractions on top of the library. Each is the public face of a private `detail` concept (the
// `detail` ones stay invisible). Deliberately a small, stable set: the field (real/complex), the
// scalar kinds, and the tensor order. The algebra primitives, scaling engine, and representation
// concepts that drive them remain implementation detail.

/// @brief A real-field representation type.
MP_UNITS_EXPORT template<typename T>
concept Real = ::mp_units::detail::Real<T>;

/// @brief A complex-field representation type.
MP_UNITS_EXPORT template<typename T>
concept Complex = ::mp_units::detail::Complex<T>;

/// @brief A real scalar (tensor order 0): real field, totally ordered, self-scalable.
MP_UNITS_EXPORT template<typename T>
concept RealScalar = ::mp_units::detail::RealScalar<T>;

/// @brief A complex scalar: complex field with `real()`/`imag()`/`modulus()`.
MP_UNITS_EXPORT template<typename T>
concept ComplexScalar = ::mp_units::detail::ComplexScalar<T>;

/// @brief A scalar (real or complex, tensor order 0) - the element type a vector/tensor is built from.
MP_UNITS_EXPORT template<typename T>
concept Scalar = ::mp_units::detail::Scalar<T>;

/// @brief A vector (a scalar, or a magnitude-bearing tensor-order-1 type).
MP_UNITS_EXPORT template<typename T>
concept Vector = ::mp_units::detail::Vector<T>;

/// @brief A second-order tensor (or a lower-order type filling its slot).
MP_UNITS_EXPORT template<typename T>
concept Tensor = ::mp_units::detail::Tensor<T>;

}  // namespace mp_units::utility
