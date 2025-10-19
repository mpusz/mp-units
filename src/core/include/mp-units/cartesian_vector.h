// SPDX-License-Identifier: MIT
#pragma once

#include <mp-units/bits/requires_hosted.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/representation_concepts.h>

#if MP_UNITS_HOSTED
  #include <mp-units/bits/fmt.h>
#endif

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#  ifdef MP_UNITS_IMPORT_STD
     import std;
#  else
#    include <cstddef>
#    include <type_traits>
#    include <concepts>
#    include <cmath>
#    if MP_UNITS_HOSTED
#      include <ostream>
#    endif
#  endif
#endif

namespace mp_units {

MP_UNITS_EXPORT template<detail::Scalar T = double>
class cartesian_vector;

namespace detail {

struct cartesian_vector_iface {
  // A + B
  template<typename T, typename U>
    requires requires(const T& t, const U& u) { t + u; }
  [[nodiscard]] friend constexpr auto operator+(const cartesian_vector<T>& lhs,
                                                const cartesian_vector<U>& rhs)
  {
    return ::mp_units::cartesian_vector{
      lhs._coordinates_[0] + rhs._coordinates_[0],
      lhs._coordinates_[1] + rhs._coordinates_[1],
      lhs._coordinates_[2] + rhs._coordinates_[2]};
  }

  // A - B
  template<typename T, typename U>
    requires requires(const T& t, const U& u) { t - u; }
  [[nodiscard]] friend constexpr auto operator-(const cartesian_vector<T>& lhs,
                                                const cartesian_vector<U>& rhs)
  {
    return ::mp_units::cartesian_vector{
      lhs._coordinates_[0] - rhs._coordinates_[0],
      lhs._coordinates_[1] - rhs._coordinates_[1],
      lhs._coordinates_[2] - rhs._coordinates_[2]};
  }

  // A % B  (integral: %, floating: fmod into CT)
  template<typename T, typename U>
    requires (requires(const T& t, const U& u) { t % u; }) ||
             (std::floating_point<T> && std::floating_point<U>)
  [[nodiscard]] friend constexpr auto operator%(const cartesian_vector<T>& lhs,
                                                const cartesian_vector<U>& rhs)
  {
    using CT = std::common_type_t<T, U>;
    if constexpr (std::floating_point<T> && std::floating_point<U>) {
      using std::fmod;
      return ::mp_units::cartesian_vector<CT>{
        static_cast<CT>(fmod(static_cast<long double>(lhs._coordinates_[0]),
                             static_cast<long double>(rhs._coordinates_[0]))),
        static_cast<CT>(fmod(static_cast<long double>(lhs._coordinates_[1]),
                             static_cast<long double>(rhs._coordinates_[1]))),
        static_cast<CT>(fmod(static_cast<long double>(lhs._coordinates_[2]),
                             static_cast<long double>(rhs._coordinates_[2])))};
    } else {
      return ::mp_units::cartesian_vector<CT>{
        static_cast<CT>(lhs._coordinates_[0] % rhs._coordinates_[0]),
        static_cast<CT>(lhs._coordinates_[1] % rhs._coordinates_[1]),
        static_cast<CT>(lhs._coordinates_[2] % rhs._coordinates_[2])};
    }
  }

  // (vector * scalar)
  template<typename T, typename S>
    requires requires(const T& t, const S& s) { t * s; }
  [[nodiscard]] friend constexpr auto operator*(const cartesian_vector<T>& vector, const S& scalar)
  {
    return ::mp_units::cartesian_vector{
      vector._coordinates_[0] * scalar,
      vector._coordinates_[1] * scalar,
      vector._coordinates_[2] * scalar};
  }

  // (scalar * vector)
  template<typename S, typename U>
    requires requires(const S& s, const U& u) { s * u; }
  [[nodiscard]] friend constexpr auto operator*(const S& s, const cartesian_vector<U>& v)
  {
    return v * s;
  }

  // (vector / scalar)
  template<typename T, typename S>
    requires requires(const T& t, const S& s) { t / s; }
  [[nodiscard]] friend constexpr auto operator/(const cartesian_vector<T>& v, const S& s)
  {
    return ::mp_units::cartesian_vector{
      v._coordinates_[0] / s,
      v._coordinates_[1] / s,
      v._coordinates_[2] / s};
  }

  // equality
  template<typename T, std::equality_comparable_with<T> U>
  [[nodiscard]] friend constexpr bool operator==(const cartesian_vector<T>& lhs,
                                                 const cartesian_vector<U>& rhs)
  {
    return lhs._coordinates_[0] == rhs._coordinates_[0] &&
           lhs._coordinates_[1] == rhs._coordinates_[1] &&
           lhs._coordinates_[2] == rhs._coordinates_[2];
  }

  // dot (numeric)
  template<typename T, typename U>
    requires requires(const T& t, const U& u, decltype(t * u) v) { t * u; v + v; }
  [[nodiscard]] friend constexpr auto scalar_product(const cartesian_vector<T>& lhs,
                                                     const cartesian_vector<U>& rhs)
  {
    return lhs._coordinates_[0] * rhs._coordinates_[0] +
           lhs._coordinates_[1] * rhs._coordinates_[1] +
           lhs._coordinates_[2] * rhs._coordinates_[2];
  }

  // cross (numeric) — 3D
  template<typename T, typename U>
    requires requires(const T& t, const U& u, decltype(t * u) v) { t * u; v - v; }
  [[nodiscard]] friend constexpr auto vector_product(const cartesian_vector<T>& lhs,
                                                     const cartesian_vector<U>& rhs)
  {
    return ::mp_units::cartesian_vector{
      lhs._coordinates_[1] * rhs._coordinates_[2] - lhs._coordinates_[2] * rhs._coordinates_[1],
      lhs._coordinates_[2] * rhs._coordinates_[0] - lhs._coordinates_[0] * rhs._coordinates_[2],
      lhs._coordinates_[0] * rhs._coordinates_[1] - lhs._coordinates_[1] * rhs._coordinates_[0]};
  }
};

} // namespace detail

MP_UNITS_EXPORT template<detail::Scalar T>
class cartesian_vector : public detail::cartesian_vector_iface {
public:
  using value_type = T;
  T _coordinates_[3]{};

  cartesian_vector() = default;
  cartesian_vector(const cartesian_vector&) = default;
  cartesian_vector(cartesian_vector&&) = default;
  cartesian_vector& operator=(const cartesian_vector&) = default;
  cartesian_vector& operator=(cartesian_vector&&) = default;

  template<typename... Args>
    requires (sizeof...(Args) == 3) && (... && std::constructible_from<T, Args>)
  constexpr explicit(!(... && std::convertible_to<Args, T>))
  cartesian_vector(Args&&... args)
    : _coordinates_{ static_cast<T>(std::forward<Args>(args))... } {}

  template<typename U>
    requires std::constructible_from<T, const U&>
  constexpr explicit(!std::convertible_to<U, T>)
  cartesian_vector(const cartesian_vector<U>& other)
    : _coordinates_{ static_cast<T>(other[0]),
                     static_cast<T>(other[1]),
                     static_cast<T>(other[2]) } {}

  template<typename U>
    requires std::constructible_from<T, U&&>
  constexpr explicit(!std::convertible_to<U, T>)
  cartesian_vector(cartesian_vector<U>&& other)
    : _coordinates_{ static_cast<T>(std::move(other[0])),
                     static_cast<T>(std::move(other[1])),
                     static_cast<T>(std::move(other[2])) } {}

  template<std::convertible_to<T> U>
  constexpr cartesian_vector& operator=(const cartesian_vector<U>& other) {
    _coordinates_[0] = other[0];
    _coordinates_[1] = other[1];
    _coordinates_[2] = other[2];
    return *this;
  }

  template<std::convertible_to<T> U>
  constexpr cartesian_vector& operator=(cartesian_vector<U>&& other) {
    _coordinates_[0] = std::move(other[0]);
    _coordinates_[1] = std::move(other[1]);
    _coordinates_[2] = std::move(other[2]);
    return *this;
  }

  // magnitude / unit (floating/complex-like)
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
  { return *this / magnitude(); }

  [[nodiscard]] constexpr       T& operator[](std::size_t i)       { return _coordinates_[i]; }
  [[nodiscard]] constexpr const T& operator[](std::size_t i) const { return _coordinates_[i]; }

  [[nodiscard]] constexpr cartesian_vector operator+() const { return *this; }
  [[nodiscard]] constexpr cartesian_vector operator-() const {
    return cartesian_vector{-_coordinates_[0], -_coordinates_[1], -_coordinates_[2]};
  }

  template<typename U> requires requires(T& t, const U& u) { t += u; }
  constexpr cartesian_vector& operator+=(const cartesian_vector<U>& other) {
    _coordinates_[0] += other[0];
    _coordinates_[1] += other[1];
    _coordinates_[2] += other[2];
    return *this;
  }

  template<typename U> requires requires(T& t, const U& u) { t -= u; }
  constexpr cartesian_vector& operator-=(const cartesian_vector<U>& other) {
    _coordinates_[0] -= other[0];
    _coordinates_[1] -= other[1];
    _coordinates_[2] -= other[2];
    return *this;
  }

  template<typename S> requires requires(T& t, const S& s) { t *= s; }
  constexpr cartesian_vector& operator*=(const S& scalar)
  {
    _coordinates_[0] *= scalar;
    _coordinates_[1] *= scalar;
    _coordinates_[2] *= scalar;
    return *this;
  }

  template<typename S> requires requires(T& t, const S& s) { t /= s; }
  constexpr cartesian_vector& operator/=(const S& scalar) {
    _coordinates_[0] /= scalar;
    _coordinates_[1] /= scalar;
    _coordinates_[2] /= scalar;
    return *this;
  }

  [[nodiscard]] friend constexpr T magnitude(const cartesian_vector& vec)
    requires treat_as_floating_point<T>
  { return vec.magnitude(); }

  [[nodiscard]] friend constexpr cartesian_vector unit_vector(const cartesian_vector& vec)
    requires treat_as_floating_point<T>
  { return vec.unit(); }

#if MP_UNITS_HOSTED
  friend std::ostream& operator<<(std::ostream& os, const cartesian_vector& vec)
  {
    return os << '[' << vec[0] << ", " << vec[1] << ", " << vec[2] << ']';
  }
#endif
};

template<typename Arg, typename... Args>
  requires (sizeof...(Args) == 2) && requires { typename std::common_type_t<Arg, Args...>; }
cartesian_vector(Arg, Args...) -> cartesian_vector<std::common_type_t<Arg, Args...>>;

template<typename T, typename U>
[[nodiscard]] constexpr auto dot(const cartesian_vector<T>& lhs, const cartesian_vector<U>& rhs)
{ return scalar_product(lhs, rhs); }

template<typename T, typename U>
[[nodiscard]] constexpr auto cross(const cartesian_vector<T>& lhs, const cartesian_vector<U>& rhs)
{ return vector_product(lhs, rhs); }

template<typename T>
  requires treat_as_floating_point<T>
[[nodiscard]] constexpr auto norm(const cartesian_vector<T>& vec)
{ return magnitude(vec); }

template<detail::Scalar T>
inline constexpr bool is_vector<cartesian_vector<T>> = true;

} // namespace mp_units

#if MP_UNITS_HOSTED
template<typename T, typename Char>
struct MP_UNITS_STD_FMT::formatter<mp_units::cartesian_vector<T>, Char>
    : formatter<std::basic_string_view<Char>, Char> {
  template<typename Ctx>
  auto format(const mp_units::cartesian_vector<T>& v, Ctx& ctx) const {
    return format_to(ctx.out(), "[{}, {}, {}]", v[0], v[1], v[2]);
  }
};
#endif
