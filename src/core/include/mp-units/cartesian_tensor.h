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

#include <mp-units/bits/module_macros.h>
#include <mp-units/bits/requires_hosted.h>
#include <mp-units/cartesian_vector.h>  // for matvec/outer_numeric
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
#include <concepts>
#include <cstddef>
#include <type_traits>
#if MP_UNITS_HOSTED
#include <ostream>
#endif
#endif
#endif

namespace mp_units {

// Forward declaration (exported)
MP_UNITS_EXPORT template<detail::Scalar T, std::size_t R, std::size_t C>
class cartesian_tensor;

// ================================ tensor rep ================================

MP_UNITS_EXPORT template<detail::Scalar T, std::size_t R, std::size_t C>
class cartesian_tensor {
  static_assert(R >= 1 && R <= 3 && C >= 1 && C <= 3, "cartesian_tensor supports sizes up to 3x3");

public:
  using value_type = T;
  static constexpr std::size_t rows_v = R;
  static constexpr std::size_t cols_v = C;

  T _data_[R * C]{};

  cartesian_tensor() = default;
  cartesian_tensor(const cartesian_tensor&) = default;
  cartesian_tensor(cartesian_tensor&&) = default;
  cartesian_tensor& operator=(const cartesian_tensor&) = default;
  cartesian_tensor& operator=(cartesian_tensor&&) = default;

  // fill ctor (row-major R*C)
  template<typename... Args>
    requires(sizeof...(Args) == R * C) && (... && std::constructible_from<T, Args>)
  constexpr explicit(!(... && std::convertible_to<Args, T>)) cartesian_tensor(Args&&... args) :
      _data_{static_cast<T>(std::forward<Args>(args))...}
  {
  }

  // element access
  [[nodiscard]] constexpr T& operator()(std::size_t r, std::size_t c) { return _data_[r * C + c]; }
  [[nodiscard]] constexpr const T& operator()(std::size_t r, std::size_t c) const { return _data_[r * C + c]; }

  // elementwise +, -
  template<typename U>
    requires requires(const T& t, const U& u) { t + u; }
  [[nodiscard]] friend constexpr auto operator+(const cartesian_tensor& A, const cartesian_tensor<U, R, C>& B)
  {
    using CT = std::common_type_t<T, U>;
    cartesian_tensor<CT, R, C> Rm{};
    for (std::size_t i = 0; i < R * C; ++i) Rm._data_[i] = static_cast<CT>(A._data_[i]) + static_cast<CT>(B._data_[i]);
    return Rm;
  }

  template<typename U>
    requires requires(const T& t, const U& u) { t - u; }
  [[nodiscard]] friend constexpr auto operator-(const cartesian_tensor& A, const cartesian_tensor<U, R, C>& B)
  {
    using CT = std::common_type_t<T, U>;
    cartesian_tensor<CT, R, C> Rm{};
    for (std::size_t i = 0; i < R * C; ++i) Rm._data_[i] = static_cast<CT>(A._data_[i]) - static_cast<CT>(B._data_[i]);
    return Rm;
  }

  // elementwise % (integral uses %, floating uses fmod)
  template<typename U>
    requires(requires(const T& t, const U& u) { t % u; }) || (std::floating_point<T> && std::floating_point<U>)
  [[nodiscard]] friend constexpr auto operator%(const cartesian_tensor& A, const cartesian_tensor<U, R, C>& B)
  {
    using CT = std::common_type_t<T, U>;
    cartesian_tensor<CT, R, C> Rm{};
    if constexpr (std::floating_point<T> || std::floating_point<U>) {
      using std::fmod;
      for (std::size_t i = 0; i < R * C; ++i)
        Rm._data_[i] =
          static_cast<CT>(fmod(static_cast<long double>(A._data_[i]), static_cast<long double>(B._data_[i])));
    } else {
      for (std::size_t i = 0; i < R * C; ++i) Rm._data_[i] = static_cast<CT>(A._data_[i] % B._data_[i]);
    }
    return Rm;
  }

  // scalar *, /  (constrained to numeric scalars to avoid recursive constraints)
  template<detail::Scalar S>
  [[nodiscard]] friend constexpr auto operator*(const cartesian_tensor& tensor, const S& scalar)
  {
    using CT = std::common_type_t<T, S>;
    cartesian_tensor<CT, R, C> Rm{};
    for (std::size_t i = 0; i < R * C; ++i) Rm._data_[i] = static_cast<CT>(tensor._data_[i]) * static_cast<CT>(scalar);
    return Rm;
  }

  template<detail::Scalar S>
  [[nodiscard]] friend constexpr auto operator*(const S& scalar, const cartesian_tensor& tensor)
  {
    return tensor * scalar;
  }

  template<detail::Scalar S>
  [[nodiscard]] friend constexpr auto operator/(const cartesian_tensor& tensor, const S& scalar)
  {
    using CT = std::common_type_t<T, S>;
    cartesian_tensor<CT, R, C> Rm{};
    for (std::size_t i = 0; i < R * C; ++i) Rm._data_[i] = static_cast<CT>(tensor._data_[i]) / static_cast<CT>(scalar);
    return Rm;
  }

#if MP_UNITS_HOSTED
  friend std::ostream& operator<<(std::ostream& os, const cartesian_tensor& A)
  {
    for (std::size_t r = 0; r < R; ++r) {
      os << (r == 0 ? "[[" : " [");
      for (std::size_t c = 0; c < C; ++c) {
        os << A(r, c);
        if (c + 1 != C) os << ", ";
      }
      os << (r + 1 == R ? "]]" : "]\n");
    }
    return os;
  }
#endif
};

// Register as tensor rep
template<detail::Scalar T, std::size_t R, std::size_t C>
inline constexpr bool is_tensor<cartesian_tensor<T, R, C>> = true;

// ======================== numeric helpers (no units) ========================

// Matrix × Matrix
template<typename T, typename U, std::size_t R, std::size_t K, std::size_t C>
[[nodiscard]] constexpr auto matmul(const cartesian_tensor<T, R, K>& A, const cartesian_tensor<U, K, C>& B)
{
  using CT = std::common_type_t<T, U>;
  cartesian_tensor<CT, R, C> Rm{};
  for (std::size_t r = 0; r < R; ++r)
    for (std::size_t c = 0; c < C; ++c) {
      CT acc{};
      for (std::size_t k = 0; k < K; ++k) acc += static_cast<CT>(A(r, k)) * static_cast<CT>(B(k, c));
      Rm(r, c) = acc;
    }
  return Rm;
}

// Matrix × Vector (3×3)
template<typename T, typename U>
[[nodiscard]] constexpr auto matvec(const cartesian_tensor<T, 3, 3>& M, const cartesian_vector<U>& x)
{
  using CT = std::common_type_t<T, U>;
  cartesian_vector<CT> y{};
  for (std::size_t r = 0; r < 3; ++r) {
    CT acc{};
    for (std::size_t c = 0; c < 3; ++c) acc += static_cast<CT>(M(r, c)) * static_cast<CT>(x[c]);
    y[r] = acc;
  }
  return y;
}

// Double contraction: A : B
template<typename T, typename U, std::size_t R, std::size_t C>
[[nodiscard]] constexpr auto double_contraction(const cartesian_tensor<T, R, C>& A, const cartesian_tensor<U, R, C>& B)
{
  using CT = std::common_type_t<T, U>;
  CT acc{};
  for (std::size_t i = 0; i < R * C; ++i) acc += static_cast<CT>(A._data_[i]) * static_cast<CT>(B._data_[i]);
  return acc;  // numeric scalar
}

// Outer product: vector ⊗ vector -> 3x3 matrix
template<typename T, typename U>
[[nodiscard]] constexpr auto outer_numeric(const cartesian_vector<T>& a, const cartesian_vector<U>& b)
{
  using CT = std::common_type_t<T, U>;
  cartesian_tensor<CT, 3, 3> Rm{};
  for (std::size_t i = 0; i < 3; ++i)
    for (std::size_t j = 0; j < 3; ++j) Rm(i, j) = static_cast<CT>(a[i]) * static_cast<CT>(b[j]);
  return Rm;
}

}  // namespace mp_units

#if MP_UNITS_HOSTED
// fmt/format (or std::format) support
template<typename T, std::size_t R, std::size_t C, typename Char>
struct MP_UNITS_STD_FMT::formatter<mp_units::cartesian_tensor<T, R, C>, Char> :
    formatter<std::basic_string_view<Char>, Char> {
  template<typename Ctx>
  auto format(const mp_units::cartesian_tensor<T, R, C>& A, Ctx& ctx) const
  {
    auto out = ctx.out();
    for (std::size_t r = 0; r < R; ++r) {
      out = format_to(out, "{}", (r == 0 ? "[[" : " ["));
      for (std::size_t c = 0; c < C; ++c) {
        out = format_to(out, "{}", A(r, c));
        if (c + 1 != C) out = format_to(out, "{}", ", ");
      }
      out = format_to(out, "{}", (r + 1 == R ? "]]" : "]\n"));
    }
    return out;
  }
};
#endif
