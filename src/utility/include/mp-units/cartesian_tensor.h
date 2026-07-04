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

#include <mp-units/bits/requires_hosted.h>
//
#include <mp-units/bits/module_macros.h>
#include <mp-units/cartesian_vector.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/representation_concepts.h>
#include <mp-units/utility/representation.h>
#if MP_UNITS_HOSTED
#include <mp-units/bits/fmt.h>
#endif
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cmath>
#include <cstddef>
#include <type_traits>
#include <utility>
#if MP_UNITS_HOSTED
#include <ostream>
#endif
#endif
#endif

namespace mp_units::utility {

MP_UNITS_EXPORT template<Scalar T, std::size_t N>
  requires(N == 2 || N == 3)
class cartesian_tensor;

namespace detail {

// Build a `cartesian_tensor` by evaluating `f(idx)` for each flat (row-major) index in [0, N*N). The
// element type and the dimension N are deduced from the produced components, so - like
// `cartesian_vector_from` - the result is built in place with no default-constructed intermediate
// (`T` need not be default-constructible).
template<typename F, std::size_t... Is>
[[nodiscard]] constexpr auto cartesian_tensor_from(std::index_sequence<Is...>, F&& f)
{
  return cartesian_tensor{f(Is)...};
}

// `cartesian_tensor` models a second-order Cartesian tensor in ordinary N-dimensional space (a fixed
// N×N array of components), as described in ISO 80000-2:2019, 18 (items 2-18.19 to 2-18.25). As with
// `cartesian_vector`, all operations live in this hidden-friend interface so that both operands of
// a binary operation are deduced symmetrically (neither side undergoes an implicit conversion), and
// each binary operation deduces a single dimension N for both operands - so mixing a 2x2 and a 3x3
// tensor is ill-formed.
struct cartesian_tensor_iface {
  template<typename T, std::size_t N, typename U>
    requires requires(const T& t, const U& u) { t + u; }
  [[nodiscard]] friend constexpr auto operator+(const cartesian_tensor<T, N>& lhs, const cartesian_tensor<U, N>& rhs)
  {
    return ::mp_units::utility::detail::cartesian_tensor_from(
      std::make_index_sequence<N * N>{}, [&](std::size_t i) { return lhs._data_[i] + rhs._data_[i]; });
  }

  template<typename T, std::size_t N, typename U>
    requires requires(const T& t, const U& u) { t - u; }
  [[nodiscard]] friend constexpr auto operator-(const cartesian_tensor<T, N>& lhs, const cartesian_tensor<U, N>& rhs)
  {
    return ::mp_units::utility::detail::cartesian_tensor_from(
      std::make_index_sequence<N * N>{}, [&](std::size_t i) { return lhs._data_[i] - rhs._data_[i]; });
  }

  template<typename T, std::size_t N, typename U>
    requires requires(const T& t, const U& u) { t * u; }
  [[nodiscard]] friend constexpr auto operator*(const cartesian_tensor<T, N>& lhs, const U& rhs)
  {
    return ::mp_units::utility::detail::cartesian_tensor_from(std::make_index_sequence<N * N>{},
                                                              [&](std::size_t i) { return lhs._data_[i] * rhs; });
  }

  template<typename T, std::size_t N, typename U>
    requires requires(const T& t, const U& u) { t * u; }
  [[nodiscard]] friend constexpr auto operator*(const T& lhs, const cartesian_tensor<U, N>& rhs)
  {
    return rhs * lhs;
  }

  template<typename T, std::size_t N, typename U>
    requires requires(const T& t, const U& u) { t / u; }
  [[nodiscard]] friend constexpr auto operator/(const cartesian_tensor<T, N>& lhs, const U& rhs)
  {
    return ::mp_units::utility::detail::cartesian_tensor_from(std::make_index_sequence<N * N>{},
                                                              [&](std::size_t i) { return lhs._data_[i] / rhs; });
  }

  template<typename T, std::size_t N, std::equality_comparable_with<T> U>
  [[nodiscard]] friend constexpr bool operator==(const cartesian_tensor<T, N>& lhs, const cartesian_tensor<U, N>& rhs)
  {
    for (std::size_t i = 0; i < N * N; ++i)
      if (!(lhs._data_[i] == rhs._data_[i])) return false;
    return true;
  }

  // inner product of two second-order tensors (ISO 80000-2:2019, 2-18.23): (T ⋅ S)_ik = sum_j T_ij S_jk
  template<typename T, std::size_t N, typename U>
    requires requires(const T& t, const U& u, decltype(t * u) v) {
      t * u;
      v + v;
    }
  [[nodiscard]] friend constexpr auto inner_product(const cartesian_tensor<T, N>& lhs,
                                                    const cartesian_tensor<U, N>& rhs)
  {
    return ::mp_units::utility::detail::cartesian_tensor_from(std::make_index_sequence<N * N>{}, [&](std::size_t idx) {
      const std::size_t i = idx / N, k = idx % N;
      auto acc = lhs._data_[i * N] * rhs._data_[k];
      for (std::size_t j = 1; j < N; ++j) acc = acc + lhs._data_[i * N + j] * rhs._data_[j * N + k];
      return acc;
    });
  }

  // inner product of a second-order tensor and a vector (ISO 80000-2:2019, 2-18.24): (T ⋅ a)_i = sum_j T_ij a_j
  template<typename T, std::size_t N, typename U>
    requires requires(const T& t, const U& u, decltype(t * u) v) {
      t * u;
      v + v;
    }
  [[nodiscard]] friend constexpr auto inner_product(const cartesian_tensor<T, N>& lhs,
                                                    const cartesian_vector<U, N>& rhs)
  {
    return ::mp_units::utility::detail::cartesian_vector_from(std::make_index_sequence<N>{}, [&](std::size_t i) {
      auto acc = lhs._data_[i * N] * rhs[0];
      for (std::size_t j = 1; j < N; ++j) acc = acc + lhs._data_[i * N + j] * rhs[j];
      return acc;
    });
  }

  // scalar (double-dot) product of two second-order tensors (ISO 80000-2:2019, 2-18.25):
  // T : S = sum_i sum_j T_ij S_ij  -> scalar
  template<typename T, std::size_t N, typename U>
    requires requires(const T& t, const U& u, decltype(t * u) v) {
      t * u;
      v + v;
    }
  [[nodiscard]] friend constexpr auto scalar_product(const cartesian_tensor<T, N>& lhs,
                                                     const cartesian_tensor<U, N>& rhs)
  {
    // Hermitian for complex elements (conjugate the first argument); identity for real elements.
    auto acc = ::mp_units::utility::detail::conjugate(lhs._data_[0]) * rhs._data_[0];
    for (std::size_t i = 1; i < N * N; ++i)
      acc = acc + ::mp_units::utility::detail::conjugate(lhs._data_[i]) * rhs._data_[i];
    return acc;
  }

  // Explicit conversions between dimensions (no implicit cross-dimension conversion). `embed` places
  // the 2×2 tensor in the top-left block of a 3×3, zero-filling the new row and column; `project`
  // keeps that top-left 2×2 block. The zero is the additive identity from a component (`x - x`), so
  // no value-initialization of `T` is required.
  template<typename T>
  [[nodiscard]] friend constexpr ::mp_units::utility::cartesian_tensor<T, 3> embed(const cartesian_tensor<T, 2>& t)
  {
    const auto z = t(0, 0) - t(0, 0);
    return ::mp_units::utility::cartesian_tensor<T, 3>{t(0, 0), t(0, 1), z, t(1, 0), t(1, 1), z, z, z, z};
  }

  template<typename T>
  [[nodiscard]] friend constexpr ::mp_units::utility::cartesian_tensor<T, 2> project(const cartesian_tensor<T, 3>& t)
  {
    return ::mp_units::utility::cartesian_tensor<T, 2>{t(0, 0), t(0, 1), t(1, 0), t(1, 1)};
  }
};

}  // namespace detail

MP_UNITS_EXPORT template<Scalar T = double, std::size_t N = 3>
  requires(N == 2 || N == 3)
class cartesian_tensor : public detail::cartesian_tensor_iface {
public:
  // public members required to satisfy structural type requirements :-(
  T _data_[N * N];
  using value_type = T;

  // The dimension N - the per-axis count (`std::extent`-style: count in one direction, not the N×N
  // element count). The tensor is `extent()` × `extent()`: `t(row, col)` with `row, col < extent()`.
  // Queryable at compile time and, like `basic_fixed_string::size`, usable both as `t.extent` and
  // `t.extent()`.
  static constexpr std::integral_constant<std::size_t, N> extent{};

  template<typename... Args>
    requires(sizeof...(Args) <= N * N) && (... && std::constructible_from<T, Args>)
  constexpr explicit(!(... && detail::ImplicitlyConvertibleScalar<Args, T>)) cartesian_tensor(Args&&... args) :
      _data_{static_cast<T>(std::forward<Args>(args))...}
  {
  }

private:
  // See `cartesian_vector`: delegating through an index sequence builds `_data_` in the member
  // initializer list (real initialization, not default-construct-then-assign). Indexing the public
  // `_data_` member directly forwards the value category, so one helper would serve a move ctor too.
  template<typename V, std::size_t... Is>
  constexpr cartesian_tensor(std::index_sequence<Is...>, V&& other) :
      _data_{static_cast<T>(std::forward<V>(other)._data_[Is])...}
  {
  }

public:
  template<typename U>
    requires std::constructible_from<T, U>
  constexpr explicit(!detail::ImplicitlyConvertibleScalar<U, T>) cartesian_tensor(const cartesian_tensor<U, N>& other) :
      cartesian_tensor(std::make_index_sequence<N * N>{}, other)
  {
  }

  template<typename U>
    requires detail::ImplicitlyConvertibleScalar<U, T>
  constexpr cartesian_tensor& operator=(const cartesian_tensor<U, N>& other)
  {
    for (std::size_t i = 0; i < N * N; ++i) _data_[i] = other._data_[i];
    return *this;
  }

  // element access via (row, column), row-major storage
  [[nodiscard]] constexpr T& operator()(std::size_t row, std::size_t col) { return _data_[row * N + col]; }
  [[nodiscard]] constexpr const T& operator()(std::size_t row, std::size_t col) const { return _data_[row * N + col]; }

#if __cpp_multidimensional_subscript && MP_UNITS_COMP_GCC != 12
  // C++23 multidimensional subscript, equivalent to `operator()` above (GCC 12 defines the feature
  // macro but does not implement it, so it is excluded)
  [[nodiscard]] constexpr T& operator[](std::size_t row, std::size_t col) { return _data_[row * N + col]; }
  [[nodiscard]] constexpr const T& operator[](std::size_t row, std::size_t col) const { return _data_[row * N + col]; }
#endif

  // Element-wise real and imaginary parts, present only for complex elements. Their existence is
  // what marks this type as a complex (rather than real) representation through the `real`/`imag`
  // customization points.
  [[nodiscard]] constexpr auto real() const
    requires ComplexScalar<T>
  {
    return ::mp_units::utility::detail::cartesian_tensor_from(
      std::make_index_sequence<N * N>{}, [&](std::size_t i) { return ::mp_units::real(_data_[i]); });
  }

  [[nodiscard]] constexpr auto imag() const
    requires ComplexScalar<T>
  {
    return ::mp_units::utility::detail::cartesian_tensor_from(
      std::make_index_sequence<N * N>{}, [&](std::size_t i) { return ::mp_units::imag(_data_[i]); });
  }

  // Frobenius norm: sqrt(T : T). Not a distinct ISO 80000-2 item (2-18.4 magnitude is vector-only) but
  // the natural tensor norm; required for the type to model the `Tensor` representation concept.
  [[nodiscard]] constexpr auto magnitude() const
    requires requires(T t) {
      requires(requires { sqrt(t * t); } || requires { std::sqrt(t * t); }) || requires { ::mp_units::modulus(t); };
    }
  {
    using std::sqrt;
    if constexpr (ComplexScalar<T>) {
      // Frobenius norm sqrt(sum |T_ij|²) for complex elements
      auto sum = ::mp_units::modulus(_data_[0]) * ::mp_units::modulus(_data_[0]);
      for (std::size_t i = 1; i < N * N; ++i) sum += ::mp_units::modulus(_data_[i]) * ::mp_units::modulus(_data_[i]);
      return sqrt(sum);
    } else {
      auto sum = _data_[0] * _data_[0];
      for (std::size_t i = 1; i < N * N; ++i) sum += _data_[i] * _data_[i];
      return sqrt(sum);
    }
  }

  [[nodiscard]] constexpr auto norm() const
    requires requires(const cartesian_tensor& t) { t.magnitude(); }
  {
    return magnitude();
  }

  [[nodiscard]] constexpr cartesian_tensor operator+() const { return *this; }
  [[nodiscard]] constexpr cartesian_tensor operator-() const
  {
    return ::mp_units::utility::detail::cartesian_tensor_from(std::make_index_sequence<N * N>{},
                                                              [&](std::size_t i) { return -_data_[i]; });
  }

  template<typename U>
    requires requires(T& t, const U& u) {
      { t += u } -> std::same_as<T&>;
    }
  constexpr cartesian_tensor& operator+=(const cartesian_tensor<U, N>& other)
  {
    for (std::size_t i = 0; i < N * N; ++i) _data_[i] += other._data_[i];
    return *this;
  }

  template<typename U>
    requires requires(T& t, const U& u) {
      { t -= u } -> std::same_as<T&>;
    }
  constexpr cartesian_tensor& operator-=(const cartesian_tensor<U, N>& other)
  {
    for (std::size_t i = 0; i < N * N; ++i) _data_[i] -= other._data_[i];
    return *this;
  }

  template<typename U>
    requires requires(T& t, const U& u) {
      { t *= u } -> std::same_as<T&>;
    }
  constexpr cartesian_tensor& operator*=(const U& value)
  {
    for (std::size_t i = 0; i < N * N; ++i) _data_[i] *= value;
    return *this;
  }

  template<typename U>
    requires requires(T& t, const U& u) {
      { t /= u } -> std::same_as<T&>;
    }
  constexpr cartesian_tensor& operator/=(const U& value)
  {
    for (std::size_t i = 0; i < N * N; ++i) _data_[i] /= value;
    return *this;
  }

  [[nodiscard]] friend constexpr auto norm(const cartesian_tensor& t)
    requires requires { t.norm(); }
  {
    return t.norm();
  }

  [[nodiscard]] friend constexpr auto magnitude(const cartesian_tensor& t)
    requires requires { t.norm(); }
  {
    return t.norm();
  }

#if MP_UNITS_HOSTED
  friend constexpr std::ostream& operator<<(std::ostream& os, const cartesian_tensor& t)
  {
    os << '[';
    for (std::size_t row = 0; row < N; ++row) {
      os << (row == 0 ? "[" : ", [");
      for (std::size_t col = 0; col < N; ++col) os << (col == 0 ? "" : ", ") << t._data_[row * N + col];
      os << ']';
    }
    return os << ']';
  }
#endif
};

template<typename Arg, typename... Args>
  requires(1 + sizeof...(Args) == 4 || 1 + sizeof...(Args) == 9) &&
          requires { typename std::common_type_t<Arg, Args...>; }
cartesian_tensor(Arg, Args...)
  -> cartesian_tensor<std::common_type_t<Arg, Args...>, (1 + sizeof...(Args) == 4 ? 2 : 3)>;

// tensor (dyadic) product of two vectors (ISO 80000-2:2019, 2-18.21): (a (x) b)_ij = a_i b_j
// Defined as a free function because both operands are vectors (it is found by ADL on
// `cartesian_vector`); the result is a second-order tensor of the same dimension.
MP_UNITS_EXPORT template<typename T, std::size_t N, typename U>
  requires requires(const T& t, const U& u) { t * u; }
[[nodiscard]] constexpr auto tensor_product(const cartesian_vector<T, N>& lhs, const cartesian_vector<U, N>& rhs)
{
  return ::mp_units::utility::detail::cartesian_tensor_from(
    std::make_index_sequence<N * N>{}, [&](std::size_t idx) { return lhs[idx / N] * rhs[idx % N]; });
}

}  // namespace mp_units::utility

template<typename T, std::size_t N, typename U>
  requires requires { typename std::common_type_t<T, U>; }
struct std::common_type<mp_units::utility::cartesian_tensor<T, N>, mp_units::utility::cartesian_tensor<U, N>> {
  using type = mp_units::utility::cartesian_tensor<std::common_type_t<T, U>, N>;
};

#if MP_UNITS_HOSTED
// TODO use parse and use formatter for the underlying type
template<typename T, std::size_t N, typename Char>
struct MP_UNITS_STD_FMT::formatter<mp_units::utility::cartesian_tensor<T, N>, Char> :
    formatter<std::basic_string_view<Char>, Char> {
  template<typename FormatContext>
  auto format(const mp_units::utility::cartesian_tensor<T, N>& tensor, FormatContext& ctx) const
  {
    auto out = format_to(ctx.out(), "[");
    for (std::size_t row = 0; row < N; ++row) {
      out = format_to(out, "{}[", row == 0 ? "" : ", ");
      for (std::size_t col = 0; col < N; ++col) out = format_to(out, "{}{}", col == 0 ? "" : ", ", tensor(row, col));
      out = format_to(out, "]");
    }
    return format_to(out, "]");
  }
};
#endif
