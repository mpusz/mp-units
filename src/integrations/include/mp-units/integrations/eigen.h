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

// Opt-in integration that lets Eigen vectors and matrices be used directly as mp-units
// `quantity` representation types. Include this header (instead of `<Eigen/...>` alone) and the
// library's customization points are wired up automatically.
//
// What Eigen already provides out of the box:
//   - `value_type`           -> `representation_underlying_type` is detected automatically
//   - `norm()` member        -> the `magnitude()` CPO uses it for the Euclidean magnitude
// What this header adds:
//   - `representation_canonical_type` for every Eigen expression, mapping it to the evaluated
//     `PlainObject` so a `quantity` never stores a lazy expression template (which would hold
//     dangling references to its operands).
//
// The whole header is inert unless Eigen is actually available, so it is always safe to include.
//
// The header is dual-mode: included textually for header-mode consumers, and pulled into the
// `mp_units.integrations.eigen` module interface unit otherwise. In module mode, `<Eigen/Core>`
// lives in the module's global module fragment and the customization points come from
// `import mp_units.core`, so neither is included from the module purview here.

#if __has_include(<Eigen/Core>)

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <Eigen/Core>
#include <mp-units/framework/customization_points.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#include <type_traits>
#endif
#endif

namespace mp_units {

// Eigen arithmetic operators return lazy expression templates; store their evaluated concrete
// type (`PlainObject`) in a quantity instead. Concrete matrices/vectors map to themselves.
//
// The `typename T::PlainObject` requirement is checked first and short-circuits the rest of the
// constraint: `representation_canonical_type` is instantiated for every representation type
// (including `int`, `double`, ...), and `Eigen::EigenBase<T>` is ill-formed for a non-Eigen `T`,
// so it must not be instantiated unless `T` already looks like an Eigen type.
template<typename T>
  requires requires { typename T::PlainObject; } && std::derived_from<T, Eigen::EigenBase<T>>
struct representation_canonical_type<T> {
  using type = std::remove_cvref_t<typename T::PlainObject>;
};

}  // namespace mp_units

#endif  // __has_include(<Eigen/Core>)
