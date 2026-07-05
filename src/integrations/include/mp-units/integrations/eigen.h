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
#include <cstddef>
#include <type_traits>
#include <utility>
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

// Eigen reports its shape at compile time, so the tensor order is read from `RowsAtCompileTime` /
// `ColsAtCompileTime` rather than from structural detection: an Eigen column vector is an N×1 matrix
// whose two-index `operator()` would otherwise make it look like an order-2 tensor. A row or column
// (one dimension fixed to 1) is order 1; anything else is an order-2 matrix.
template<typename T>
  requires requires { typename T::PlainObject; } && std::derived_from<T, Eigen::EigenBase<T>>
constexpr std::size_t tensor_order<T> = (T::RowsAtCompileTime == 1 || T::ColsAtCompileTime == 1) ? 1 : 2;

namespace detail {

// A fixed-size Eigen vector (a row or column whose length is known at compile time). This is the
// shape the tuple protocol below is defined for: `std::tuple_size` needs a compile-time size, so
// dynamic-size vectors (`SizeAtCompileTime == Eigen::Dynamic`) are deliberately excluded, and a
// two-dimensional matrix is not a vector. The `typename T::PlainObject` probe is checked first and
// short-circuits the rest, because the concept is evaluated for arbitrary representation types
// (`int`, `double`, ...) for which `Eigen::EigenBase<T>` would be ill-formed.
template<typename T>
concept eigen_fixed_vector = requires { typename T::PlainObject; } && std::derived_from<T, Eigen::EigenBase<T>> &&
                             (T::SizeAtCompileTime != Eigen::Dynamic) && (T::SizeAtCompileTime >= 1) &&
                             (T::RowsAtCompileTime == 1 || T::ColsAtCompileTime == 1);

}  // namespace detail

}  // namespace mp_units

// Tuple protocol for fixed-size Eigen vectors: makes them structured-bindings friendly
// (`auto [x, y, z] = vec;`) and, lets `mp_units::utility`'s `polar_vector`/`spherical_vector`
// read a vector representation's dimension via `std::tuple_size` and its components via `get`.
namespace Eigen {

template<std::size_t I, typename T>
  requires mp_units::detail::eigen_fixed_vector<std::remove_cvref_t<T>>
[[nodiscard]] constexpr decltype(auto) get(T&& v)
{
  return std::forward<T>(v)[static_cast<Eigen::Index>(I)];
}

}  // namespace Eigen

template<mp_units::detail::eigen_fixed_vector T>
struct std::tuple_size<T> : std::integral_constant<std::size_t, static_cast<std::size_t>(T::SizeAtCompileTime)> {};

template<std::size_t I, mp_units::detail::eigen_fixed_vector T>
struct std::tuple_element<I, T> {
  using type = std::remove_cv_t<typename T::Scalar>;
};

#endif  // __has_include(<Eigen/Core>)
