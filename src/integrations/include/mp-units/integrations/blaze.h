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

// Opt-in integration that lets Blaze vectors and matrices be used directly as mp-units
// `quantity` representation types.
//
// What Blaze already provides out of the box:
//   - free `norm()` -> the `magnitude()` CPO uses it (via ADL) for the Euclidean magnitude
// What this header adds:
//   - `representation_underlying_type` from Blaze's `ElementType` (Blaze does not expose the
//     `value_type`/`element_type` names the library detects automatically)
//   - `representation_canonical_type` mapping every Blaze expression to its evaluated `ResultType`
//     so a `quantity` never stores a lazy expression template
//   - a `blaze::IsScalar` opt-out for the library's compile-time unit-magnitude tag: Blaze treats
//     every unknown type as a scalar, so without this its greedy `vector * scalar` operator would
//     match the `value * unit_magnitude` probe and hard-error deep inside `MultTrait`
//
// The whole header is inert unless Blaze is actually available, so it is always safe to include.
//
// The header is dual-mode: included textually for header-mode consumers, and pulled into the
// `mp_units.integrations.blaze` module interface unit otherwise. It only declares trait
// specializations (which are reachable to importers without `export`); in module mode the Blaze
// headers live in the module's global module fragment and the mp-units customization points come
// from `import mp_units.core`, so none of those are included from the module purview here.

#if __has_include(<blaze/math/StaticVector.h>)

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <blaze/math/DynamicMatrix.h>
#include <blaze/math/DynamicVector.h>
#include <blaze/math/StaticMatrix.h>
#include <blaze/math/StaticVector.h>
#include <blaze/math/typetraits/IsMatrix.h>
#include <blaze/math/typetraits/IsScalar.h>
#include <blaze/math/typetraits/IsVector.h>
#include <blaze/util/IntegralConstant.h>  // blaze::FalseType
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/unit_magnitude_concepts.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <type_traits>
#endif
#endif

namespace mp_units {

namespace detail {

// A Blaze dense/sparse vector or matrix - the shared guard for the trait specializations below.
// `blaze::IsVector_v` / `IsMatrix_v` are defined (and `false`) for every type, so this is
// SFINAE-safe to evaluate for arbitrary representation types such as `int`.
template<typename T>
concept blaze_tensor = blaze::IsVector_v<T> || blaze::IsMatrix_v<T>;

}  // namespace detail

// Blaze does not expose the `value_type`/`element_type` names the library detects automatically,
// so map its `ElementType` explicitly.
template<typename T>
  requires detail::blaze_tensor<T> && requires { typename T::ElementType; }
struct representation_underlying_type<T> {
  using type = std::remove_cv_t<typename T::ElementType>;
};

// Blaze arithmetic operators return lazy expression templates; store their evaluated `ResultType`.
template<typename T>
  requires detail::blaze_tensor<T> && requires { typename T::ResultType; }
struct representation_canonical_type<T> {
  using type = std::remove_cvref_t<typename T::ResultType>;
};

// Blaze provides free `real()`/`imag()` for its vectors and matrices even when the element type is
// real, so the API-based default would misread a real Blaze tensor as complex. Declare the field
// from Blaze's `ElementType` instead - mirroring the Eigen adapter.
template<typename T>
  requires detail::blaze_tensor<T> && requires { typename T::ElementType; }
constexpr quantity_field numeric_field<T> = numeric_field<typename T::ElementType>;

}  // namespace mp_units

namespace blaze {

// The library probes `value * unit_magnitude{}` to detect a magnitude-aware scaling operator.
// Tell Blaze that the unit-magnitude tag is not one of its scalars so its `DenseVector * scalar`
// operator is not even considered for that probe.
template<mp_units::UnitMagnitude T>
struct IsScalar<T> : public FalseType {};

}  // namespace blaze

#endif  // __has_include(<blaze/math/StaticVector.h>)
