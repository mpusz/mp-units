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
#include <mp-units/bits/module_macros.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/representation_concepts.h>
#include <type_traits>

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
#include <utility>
#if MP_UNITS_HOSTED
#include <ostream>
#endif
#endif
#endif

namespace mp_units {

MP_UNITS_EXPORT template<detail::Scalar T>
class cartesian_vector;

MP_UNITS_EXPORT template<detail::Scalar T = double>
class cartesian_vector {
public:
  // NOTE: This type is intentionally an aggregate (like std::array).
  // All special member functions are implicitly defined.
  T _coordinates_[3];
  using value_type = T;

  [[nodiscard]] constexpr T magnitude() const
    requires treat_as_floating_point<T>
  {
    using namespace std;
    if constexpr (detail::ComplexScalar<T>)
      return hypot(mp_units::modulus(_coordinates_[0]), mp_units::modulus(_coordinates_[1]),
                   mp_units::modulus(_coordinates_[2]));
    else
      return hypot(_coordinates_[0], _coordinates_[1], _coordinates_[2]);
  }

  [[nodiscard]] constexpr cartesian_vector unit() const
    requires treat_as_floating_point<T>
  {
    return *this / magnitude();
  }

  [[nodiscard]] constexpr T& operator[](std::size_t i) { return _coordinates_[i]; }
  [[nodiscard]] constexpr const T& operator[](std::size_t i) const { return _coordinates_[i]; }

  [[nodiscard]] constexpr cartesian_vector operator+() const { return *this; }
  [[nodiscard]] constexpr cartesian_vector operator-() const
  {
    return cartesian_vector{-_coordinates_[0], -_coordinates_[1], -_coordinates_[2]};
  }

  template<typename U>
    requires requires(T& t, const U& u) {
      { t += u } -> std::same_as<T&>;
    }
  constexpr cartesian_vector& operator+=(const cartesian_vector<U>& other)
  {
    _coordinates_[0] += other[0];
    _coordinates_[1] += other[1];
    _coordinates_[2] += other[2];
    return *this;
  }

  template<typename U>
    requires requires(T& t, const U& u) {
      { t -= u } -> std::same_as<T&>;
    }
  constexpr cartesian_vector& operator-=(const cartesian_vector<U>& other)
  {
    _coordinates_[0] -= other[0];
    _coordinates_[1] -= other[1];
    _coordinates_[2] -= other[2];
    return *this;
  }

  template<typename S>
    requires requires(T& t, const S& s) { t *= s; }
  constexpr cartesian_vector& operator*=(const S& scalar)
  {
    _coordinates_[0] *= scalar;
    _coordinates_[1] *= scalar;
    _coordinates_[2] *= scalar;
    return *this;
  }

  template<typename S>
    requires requires(T& t, const S& s) { t /= s; }
  constexpr cartesian_vector& operator/=(const S& scalar)
  {
    _coordinates_[0] /= scalar;
    _coordinates_[1] /= scalar;
    _coordinates_[2] /= scalar;
    return *this;
  }

  [[nodiscard]] friend constexpr T magnitude(const cartesian_vector& vec)
    requires treat_as_floating_point<T>
  {
    return vec.magnitude();
  }

  [[nodiscard]] friend constexpr cartesian_vector unit_vector(const cartesian_vector& vec)
    requires treat_as_floating_point<T>
  {
    return vec.unit();
  }

#if MP_UNITS_HOSTED
  friend std::ostream& operator<<(std::ostream& os, const cartesian_vector& vec)
  {
    return os << '[' << vec[0] << ", " << vec[1] << ", " << vec[2] << ']';
  }
#endif
};

template<typename Arg, typename... Args>
  requires(sizeof...(Args) <= 2) && requires { typename std::common_type_t<Arg, Args...>; }
cartesian_vector(Arg, Args...) -> cartesian_vector<std::common_type_t<Arg, Args...>>;

MP_UNITS_EXPORT template<typename T, typename U>
  requires requires(const T& t, const U& u) { t + u; }
[[nodiscard]] constexpr auto operator+(const cartesian_vector<T>& lhs, const cartesian_vector<U>& rhs)
{
  return ::mp_units::cartesian_vector{lhs._coordinates_[0] + rhs._coordinates_[0],
                                      lhs._coordinates_[1] + rhs._coordinates_[1],
                                      lhs._coordinates_[2] + rhs._coordinates_[2]};
}

MP_UNITS_EXPORT template<typename T, typename U>
  requires requires(const T& t, const U& u) { t - u; }
[[nodiscard]] constexpr auto operator-(const cartesian_vector<T>& lhs, const cartesian_vector<U>& rhs)
{
  return ::mp_units::cartesian_vector{lhs._coordinates_[0] - rhs._coordinates_[0],
                                      lhs._coordinates_[1] - rhs._coordinates_[1],
                                      lhs._coordinates_[2] - rhs._coordinates_[2]};
}

MP_UNITS_EXPORT template<typename T, typename U>
  requires(!treat_as_floating_point<T> && !treat_as_floating_point<U> && requires(const T& t, const U& u) { t % u; })
[[nodiscard]] constexpr auto operator%(const cartesian_vector<T>& lhs, const cartesian_vector<U>& rhs)
{
  using CT = std::common_type_t<T, U>;
  return ::mp_units::cartesian_vector<CT>{static_cast<CT>(lhs._coordinates_[0] % rhs._coordinates_[0]),
                                          static_cast<CT>(lhs._coordinates_[1] % rhs._coordinates_[1]),
                                          static_cast<CT>(lhs._coordinates_[2] % rhs._coordinates_[2])};
}

MP_UNITS_EXPORT template<typename T, typename S>
  requires requires(const T& t, const S& s) { t * s; }
[[nodiscard]] constexpr auto operator*(const cartesian_vector<T>& vector, const S& scalar)
{
  return ::mp_units::cartesian_vector{vector._coordinates_[0] * scalar, vector._coordinates_[1] * scalar,
                                      vector._coordinates_[2] * scalar};
}

MP_UNITS_EXPORT template<typename S, typename U>
  requires requires(const S& s, const U& u) { s * u; }
[[nodiscard]] constexpr auto operator*(const S& scalar, const cartesian_vector<U>& vector)
{
  return vector * scalar;
}

MP_UNITS_EXPORT template<typename T, typename S>
  requires requires(const T& t, const S& s) { t / s; }
[[nodiscard]] constexpr auto operator/(const cartesian_vector<T>& vector, const S& scalar)
{
  return ::mp_units::cartesian_vector{vector._coordinates_[0] / scalar, vector._coordinates_[1] / scalar,
                                      vector._coordinates_[2] / scalar};
}

MP_UNITS_EXPORT template<typename T, std::equality_comparable_with<T> U>
[[nodiscard]] constexpr bool operator==(const cartesian_vector<T>& lhs, const cartesian_vector<U>& rhs)
{
  return lhs._coordinates_[0] == rhs._coordinates_[0] && lhs._coordinates_[1] == rhs._coordinates_[1] &&
         lhs._coordinates_[2] == rhs._coordinates_[2];
}

MP_UNITS_EXPORT template<typename T, typename U>
  requires requires(const T& t, const U& u, decltype(t * u) v) {
    t * u;
    v + v;
  }
[[nodiscard]] constexpr auto scalar_product(const cartesian_vector<T>& lhs, const cartesian_vector<U>& rhs)
{
  return lhs._coordinates_[0] * rhs._coordinates_[0] + lhs._coordinates_[1] * rhs._coordinates_[1] +
         lhs._coordinates_[2] * rhs._coordinates_[2];
}

MP_UNITS_EXPORT template<typename T, typename U>
  requires requires(const T& t, const U& u, decltype(t * u) v) {
    t * u;
    v - v;
  }
[[nodiscard]] constexpr auto vector_product(const cartesian_vector<T>& lhs, const cartesian_vector<U>& rhs)
{
  return ::mp_units::cartesian_vector{
    lhs._coordinates_[1] * rhs._coordinates_[2] - lhs._coordinates_[2] * rhs._coordinates_[1],
    lhs._coordinates_[2] * rhs._coordinates_[0] - lhs._coordinates_[0] * rhs._coordinates_[2],
    lhs._coordinates_[0] * rhs._coordinates_[1] - lhs._coordinates_[1] * rhs._coordinates_[0]};
}

template<class T>
inline constexpr bool treat_as_floating_point<cartesian_vector<T>> = treat_as_floating_point<T>;

template<class S, class T>
inline constexpr bool is_value_preserving<S, cartesian_vector<T>> = is_value_preserving<S, T>;
}  // namespace mp_units

namespace std {
template<class T, class U>
struct common_type<mp_units::cartesian_vector<T>, mp_units::cartesian_vector<U>> {
  using type = mp_units::cartesian_vector<common_type_t<T, U>>;
};

template<class T, class U>
struct common_type<mp_units::cartesian_vector<T>, U> {
  using type = mp_units::cartesian_vector<common_type_t<T, U>>;
};

template<class T, class U>
struct common_type<U, mp_units::cartesian_vector<T>> {
  using type = mp_units::cartesian_vector<common_type_t<U, T>>;
};
}  // namespace std

#if MP_UNITS_HOSTED
// TODO use parse and use formatter for the underlying type
template<typename T, typename Char>
struct MP_UNITS_STD_FMT::formatter<mp_units::cartesian_vector<T>, Char> :
    formatter<std::basic_string_view<Char>, Char> {
  template<typename FormatContext>
  auto format(const mp_units::cartesian_vector<T>& vec, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "[{}, {}, {}]", vec[0], vec[1], vec[2]);
  }
};
#endif