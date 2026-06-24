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
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/representation_concepts.h>

#if MP_UNITS_HOSTED
#include <mp-units/bits/fmt.h>
#endif

#ifndef MP_UNITS_IN_MODULE_INTERFACE
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

namespace mp_units {

MP_UNITS_EXPORT template<detail::Scalar T>
class cartesian_tensor;

namespace detail {

// `cartesian_tensor` models a second-order Cartesian tensor in ordinary 3D space (a fixed 3x3
// array of components), as described in ISO 80000-2:2019, 18 (items 2-18.19 to 2-18.25). As with
// `cartesian_vector`, all operations live in this hidden-friend interface so that both operands of
// a binary operation are deduced symmetrically (neither side undergoes an implicit conversion).
struct cartesian_tensor_iface {
  template<typename T, typename U>
    requires requires(const T& t, const U& u) { t + u; }
  [[nodiscard]] friend constexpr auto operator+(const cartesian_tensor<T>& lhs, const cartesian_tensor<U>& rhs)
  {
    ::mp_units::cartesian_tensor<decltype(lhs._data_[0] + rhs._data_[0])> res;
    for (std::size_t i = 0; i < 9; ++i) res._data_[i] = lhs._data_[i] + rhs._data_[i];
    return res;
  }

  template<typename T, typename U>
    requires requires(const T& t, const U& u) { t - u; }
  [[nodiscard]] friend constexpr auto operator-(const cartesian_tensor<T>& lhs, const cartesian_tensor<U>& rhs)
  {
    ::mp_units::cartesian_tensor<decltype(lhs._data_[0] - rhs._data_[0])> res;
    for (std::size_t i = 0; i < 9; ++i) res._data_[i] = lhs._data_[i] - rhs._data_[i];
    return res;
  }

  template<typename T, typename U>
    requires requires(const T& t, const U& u) { t * u; }
  [[nodiscard]] friend constexpr auto operator*(const cartesian_tensor<T>& lhs, const U& rhs)
  {
    ::mp_units::cartesian_tensor<decltype(lhs._data_[0] * rhs)> res;
    for (std::size_t i = 0; i < 9; ++i) res._data_[i] = lhs._data_[i] * rhs;
    return res;
  }

  template<typename T, typename U>
    requires requires(const T& t, const U& u) { t * u; }
  [[nodiscard]] friend constexpr auto operator*(const T& lhs, const cartesian_tensor<U>& rhs)
  {
    return rhs * lhs;
  }

  template<typename T, typename U>
    requires requires(const T& t, const U& u) { t / u; }
  [[nodiscard]] friend constexpr auto operator/(const cartesian_tensor<T>& lhs, const U& rhs)
  {
    ::mp_units::cartesian_tensor<decltype(lhs._data_[0] / rhs)> res;
    for (std::size_t i = 0; i < 9; ++i) res._data_[i] = lhs._data_[i] / rhs;
    return res;
  }

  template<typename T, std::equality_comparable_with<T> U>
  [[nodiscard]] friend constexpr bool operator==(const cartesian_tensor<T>& lhs, const cartesian_tensor<U>& rhs)
  {
    for (std::size_t i = 0; i < 9; ++i)
      if (!(lhs._data_[i] == rhs._data_[i])) return false;
    return true;
  }

  // inner product of two second-order tensors (ISO 80000-2:2019, 2-18.23): (T . S)_ik = sum_j T_ij S_jk
  template<typename T, typename U>
    requires requires(const T& t, const U& u, decltype(t * u) v) {
      t * u;
      v + v;
    }
  [[nodiscard]] friend constexpr auto inner_product(const cartesian_tensor<T>& lhs, const cartesian_tensor<U>& rhs)
  {
    ::mp_units::cartesian_tensor<decltype(lhs._data_[0] * rhs._data_[0] + lhs._data_[0] * rhs._data_[0])> res;
    for (std::size_t i = 0; i < 3; ++i)
      for (std::size_t k = 0; k < 3; ++k) {
        auto acc = lhs._data_[i * 3] * rhs._data_[k];
        for (std::size_t j = 1; j < 3; ++j) acc = acc + lhs._data_[i * 3 + j] * rhs._data_[j * 3 + k];
        res._data_[i * 3 + k] = acc;
      }
    return res;
  }

  // inner product of a second-order tensor and a vector (ISO 80000-2:2019, 2-18.24): (T . a)_i = sum_j T_ij a_j
  template<typename T, typename U>
    requires requires(const T& t, const U& u, decltype(t * u) v) {
      t * u;
      v + v;
    }
  [[nodiscard]] friend constexpr auto inner_product(const cartesian_tensor<T>& lhs, const cartesian_vector<U>& rhs)
  {
    return ::mp_units::cartesian_vector{lhs._data_[0] * rhs[0] + lhs._data_[1] * rhs[1] + lhs._data_[2] * rhs[2],
                                        lhs._data_[3] * rhs[0] + lhs._data_[4] * rhs[1] + lhs._data_[5] * rhs[2],
                                        lhs._data_[6] * rhs[0] + lhs._data_[7] * rhs[1] + lhs._data_[8] * rhs[2]};
  }

  // scalar (double-dot) product of two second-order tensors (ISO 80000-2:2019, 2-18.25):
  // T : S = sum_i sum_j T_ij S_ij  -> scalar
  template<typename T, typename U>
    requires requires(const T& t, const U& u, decltype(t * u) v) {
      t * u;
      v + v;
    }
  [[nodiscard]] friend constexpr auto scalar_product(const cartesian_tensor<T>& lhs, const cartesian_tensor<U>& rhs)
  {
    auto acc = lhs._data_[0] * rhs._data_[0];
    for (std::size_t i = 1; i < 9; ++i) acc = acc + lhs._data_[i] * rhs._data_[i];
    return acc;
  }
};

}  // namespace detail

MP_UNITS_EXPORT template<detail::Scalar T = double>
class cartesian_tensor : public detail::cartesian_tensor_iface {
public:
  // public members required to satisfy structural type requirements :-(
  T _data_[9];
  using value_type = T;

  template<typename... Args>
    requires(sizeof...(Args) <= 9) && (... && std::constructible_from<T, Args>)
  constexpr explicit(!(... && std::convertible_to<Args, T>)) cartesian_tensor(Args&&... args) :
      _data_{static_cast<T>(std::forward<Args>(args))...}
  {
  }

  template<typename U>
    requires std::constructible_from<T, U>
  constexpr explicit(!std::convertible_to<U, T>) cartesian_tensor(const cartesian_tensor<U>& other)
  {
    for (std::size_t i = 0; i < 9; ++i) _data_[i] = static_cast<T>(other._data_[i]);
  }

  template<std::convertible_to<T> U>
  constexpr cartesian_tensor& operator=(const cartesian_tensor<U>& other)
  {
    for (std::size_t i = 0; i < 9; ++i) _data_[i] = other._data_[i];
    return *this;
  }

  // element access via (row, column), row-major storage
  [[nodiscard]] constexpr T& operator()(std::size_t row, std::size_t col) { return _data_[row * 3 + col]; }
  [[nodiscard]] constexpr const T& operator()(std::size_t row, std::size_t col) const { return _data_[row * 3 + col]; }

  // Frobenius norm: sqrt(T : T). Not a distinct ISO 80000-2 item (2-18.4 magnitude is vector-only) but
  // the natural tensor norm; required for the type to model the `Tensor` representation concept.
  [[nodiscard]] constexpr auto magnitude() const
    requires requires(T t) { requires requires { sqrt(t * t); } || requires { std::sqrt(t * t); }; }
  {
    using std::sqrt;
    auto sum = _data_[0] * _data_[0];
    for (std::size_t i = 1; i < 9; ++i) sum += _data_[i] * _data_[i];
    return sqrt(sum);
  }

  [[nodiscard]] constexpr auto norm() const
    requires requires(const cartesian_tensor& t) { t.magnitude(); }
  {
    return magnitude();
  }

  [[nodiscard]] constexpr cartesian_tensor operator+() const { return *this; }
  [[nodiscard]] constexpr cartesian_tensor operator-() const
  {
    cartesian_tensor res;
    for (std::size_t i = 0; i < 9; ++i) res._data_[i] = -_data_[i];
    return res;
  }

  template<typename U>
    requires requires(T& t, const U& u) {
      { t += u } -> std::same_as<T&>;
    }
  constexpr cartesian_tensor& operator+=(const cartesian_tensor<U>& other)
  {
    for (std::size_t i = 0; i < 9; ++i) _data_[i] += other._data_[i];
    return *this;
  }

  template<typename U>
    requires requires(T& t, const U& u) {
      { t -= u } -> std::same_as<T&>;
    }
  constexpr cartesian_tensor& operator-=(const cartesian_tensor<U>& other)
  {
    for (std::size_t i = 0; i < 9; ++i) _data_[i] -= other._data_[i];
    return *this;
  }

  template<typename U>
    requires requires(T& t, const U& u) {
      { t *= u } -> std::same_as<T&>;
    }
  constexpr cartesian_tensor& operator*=(const U& value)
  {
    for (std::size_t i = 0; i < 9; ++i) _data_[i] *= value;
    return *this;
  }

  template<typename U>
    requires requires(T& t, const U& u) {
      { t /= u } -> std::same_as<T&>;
    }
  constexpr cartesian_tensor& operator/=(const U& value)
  {
    for (std::size_t i = 0; i < 9; ++i) _data_[i] /= value;
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
    for (std::size_t row = 0; row < 3; ++row) {
      os << (row == 0 ? "[" : ", [") << t._data_[row * 3] << ", " << t._data_[row * 3 + 1] << ", "
         << t._data_[row * 3 + 2] << ']';
    }
    return os << ']';
  }
#endif
};

template<typename Arg, typename... Args>
  requires(sizeof...(Args) <= 8) && requires { typename std::common_type_t<Arg, Args...>; }
cartesian_tensor(Arg, Args...) -> cartesian_tensor<std::common_type_t<Arg, Args...>>;

// tensor (dyadic) product of two vectors (ISO 80000-2:2019, 2-18.21): (a (x) b)_ij = a_i b_j
// Defined as a free function because both operands are vectors (it is found by ADL on
// `cartesian_vector`); the result is a second-order tensor.
MP_UNITS_EXPORT template<typename T, typename U>
  requires requires(const T& t, const U& u) { t * u; }
[[nodiscard]] constexpr auto tensor_product(const cartesian_vector<T>& lhs, const cartesian_vector<U>& rhs)
{
  return ::mp_units::cartesian_tensor{lhs[0] * rhs[0], lhs[0] * rhs[1], lhs[0] * rhs[2],
                                      lhs[1] * rhs[0], lhs[1] * rhs[1], lhs[1] * rhs[2],
                                      lhs[2] * rhs[0], lhs[2] * rhs[1], lhs[2] * rhs[2]};
}

// a second-order tensor is not a vector; opt out of the (lower-rank) vector representation concept
template<typename T>
MP_UNITS_INLINE constexpr bool disable_vector<cartesian_tensor<T>> = true;

}  // namespace mp_units

template<typename T, typename U>
  requires requires { typename std::common_type_t<T, U>; }
struct std::common_type<mp_units::cartesian_tensor<T>, mp_units::cartesian_tensor<U>> {
  using type = mp_units::cartesian_tensor<std::common_type_t<T, U>>;
};

#if MP_UNITS_HOSTED
// TODO use parse and use formatter for the underlying type
template<typename T, typename Char>
struct MP_UNITS_STD_FMT::formatter<mp_units::cartesian_tensor<T>, Char> :
    formatter<std::basic_string_view<Char>, Char> {
  template<typename FormatContext>
  auto format(const mp_units::cartesian_tensor<T>& tensor, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "[[{}, {}, {}], [{}, {}, {}], [{}, {}, {}]]", tensor(0, 0), tensor(0, 1), tensor(0, 2),
                     tensor(1, 0), tensor(1, 1), tensor(1, 2), tensor(2, 0), tensor(2, 1), tensor(2, 2));
  }
};
#endif
