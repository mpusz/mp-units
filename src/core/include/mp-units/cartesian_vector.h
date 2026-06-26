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
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/representation_concepts.h>
#include <mp-units/framework/value_cast.h>  // implicitly_scalable (+ `one` via unit.h)

#if MP_UNITS_HOSTED
#include <mp-units/bits/fmt.h>
#endif

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cmath>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>
#if MP_UNITS_HOSTED
#include <ostream>
#endif
#endif
#endif

namespace mp_units {

MP_UNITS_EXPORT template<detail::Scalar T, std::size_t N>
  requires(N == 2 || N == 3)
class cartesian_vector;

namespace detail {

// Complex conjugate of a scalar element, derived from the `real`/`imag` CPOs. For a real scalar
// this is the identity, so the generic vector/tensor code can use one Hermitian formula for both
// real and complex representations without a partial specialization.
template<typename T>
[[nodiscard]] constexpr T conjugate(const T& v)
{
  if constexpr (ComplexScalar<T>)
    return T(::mp_units::real(v), -::mp_units::imag(v));
  else
    return v;
}

// A scalar element conversion `From` -> `To` is implicit only when it is also non-truncating. That
// decision is deferred to `implicitly_scalable` - the very customization point a quantity's rep
// conversion uses - so a user who specializes it gets consistent behavior here rather than a
// hardcoded copy of its default. Identical (`one`) units degenerate it to the rep-only decision (a
// bare vector has no unit scaling). `std::convertible_to` is retained because an element type need
// not be fundamental and may define its own implicit-conversion rules.
template<typename From, typename To>
concept ImplicitlyConvertibleScalar =
  std::convertible_to<From, To> && implicitly_scalable<one, std::remove_cvref_t<From>, one, To>;

// Build a `cartesian_vector` by evaluating `f(i)` for each i in [0, N); the element type and the
// dimension N are deduced from the produced components via the deduction guide.
template<typename F, std::size_t... Is>
[[nodiscard]] constexpr auto cartesian_vector_from(std::index_sequence<Is...>, F&& f)
{
  return cartesian_vector{f(Is)...};
}

struct cartesian_vector_iface {
  template<typename T, std::size_t N, typename U>
    requires requires(const T& t, const U& u) { t + u; }
  [[nodiscard]] friend constexpr auto operator+(const cartesian_vector<T, N>& lhs, const cartesian_vector<U, N>& rhs)
  {
    return ::mp_units::detail::cartesian_vector_from(std::make_index_sequence<N>{},
                                                     [&](std::size_t i) { return lhs[i] + rhs[i]; });
  }

  template<typename T, std::size_t N, typename U>
    requires requires(const T& t, const U& u) { t - u; }
  [[nodiscard]] friend constexpr auto operator-(const cartesian_vector<T, N>& lhs, const cartesian_vector<U, N>& rhs)
  {
    return ::mp_units::detail::cartesian_vector_from(std::make_index_sequence<N>{},
                                                     [&](std::size_t i) { return lhs[i] - rhs[i]; });
  }

  template<typename T, std::size_t N, typename U>
    requires requires(const T& t, const U& u) { t * u; }
  [[nodiscard]] friend constexpr auto operator*(const cartesian_vector<T, N>& lhs, const U& rhs)
  {
    return ::mp_units::detail::cartesian_vector_from(std::make_index_sequence<N>{},
                                                     [&](std::size_t i) { return lhs[i] * rhs; });
  }

  template<typename T, std::size_t N, typename U>
    requires requires(const T& t, const U& u) { t * u; }
  [[nodiscard]] friend constexpr auto operator*(const T& lhs, const cartesian_vector<U, N>& rhs)
  {
    return rhs * lhs;
  }

  template<typename T, std::size_t N, typename U>
    requires requires(const T& t, const U& u) { t / u; }
  [[nodiscard]] friend constexpr auto operator/(const cartesian_vector<T, N>& lhs, const U& rhs)
  {
    return ::mp_units::detail::cartesian_vector_from(std::make_index_sequence<N>{},
                                                     [&](std::size_t i) { return lhs[i] / rhs; });
  }

  template<typename T, std::size_t N, std::equality_comparable_with<T> U>
  [[nodiscard]] friend constexpr bool operator==(const cartesian_vector<T, N>& lhs, const cartesian_vector<U, N>& rhs)
  {
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      return (... && (lhs[Is] == rhs[Is]));
    }(std::make_index_sequence<N>{});
  }

  template<typename T, std::size_t N, typename U>
    requires requires(const T& t, const U& u, decltype(t * u) v) {
      t * u;
      v + v;
    }
  [[nodiscard]] friend constexpr auto scalar_product(const cartesian_vector<T, N>& lhs,
                                                     const cartesian_vector<U, N>& rhs)
  {
    // Hermitian (sesquilinear) for complex elements, conjugating the first argument (physics
    // convention). `conjugate` is the identity for real elements, so this is the ordinary dot
    // product there and reduces to a real result.
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      return (... + (::mp_units::detail::conjugate(lhs[Is]) * rhs[Is]));
    }(std::make_index_sequence<N>{});
  }

  // The cross product yields an (axial) vector only in three dimensions (ISO 80000-2:2019, 2-18.11).
  template<typename T, std::size_t N, typename U>
    requires(N == 3) && requires(const T& t, const U& u, decltype(t * u) v) {
      t * u;
      v - v;
    }
  [[nodiscard]] friend constexpr auto vector_product(const cartesian_vector<T, N>& lhs,
                                                     const cartesian_vector<U, N>& rhs)
  {
    return ::mp_units::cartesian_vector{lhs[1] * rhs[2] - lhs[2] * rhs[1], lhs[2] * rhs[0] - lhs[0] * rhs[2],
                                        lhs[0] * rhs[1] - lhs[1] * rhs[0]};
  }

  // In two dimensions the cross product degenerates to the perp-dot product: the (pseudo)scalar
  // z-component of the cross of the vectors embedded in 3D space (the signed parallelogram area).
  template<typename T, std::size_t N, typename U>
    requires(N == 2) && requires(const T& t, const U& u, decltype(t * u) v) {
      t * u;
      v - v;
    }
  [[nodiscard]] friend constexpr auto vector_product(const cartesian_vector<T, N>& lhs,
                                                     const cartesian_vector<U, N>& rhs)
  {
    return lhs[0] * rhs[1] - lhs[1] * rhs[0];
  }
};

}  // namespace detail

MP_UNITS_EXPORT template<detail::Scalar T = double, std::size_t N = 3>
  requires(N == 2 || N == 3)
class cartesian_vector : public detail::cartesian_vector_iface {
public:
  // public members required to satisfy structural type requirements :-(
  T _coordinates_[N];
  using value_type = T;

  // The dimension N - the per-axis count (`std::extent`-style: count in one direction), which for a
  // vector also equals its number of coordinates. Queryable at compile time and, like
  // `basic_fixed_string::size`, usable both as `v.extent` and `v.extent()`.
  static constexpr std::integral_constant<std::size_t, N> extent{};

  template<typename... Args>
    requires(sizeof...(Args) <= N) && (... && std::constructible_from<T, Args>)
  constexpr explicit(!(... && detail::ImplicitlyConvertibleScalar<Args, T>)) cartesian_vector(Args&&... args) :
      _coordinates_{static_cast<T>(std::forward<Args>(args))...}
  {
  }

private:
  // Element-wise conversion from another vector of the same dimension. Delegating through an index
  // sequence lets us build `_coordinates_` in the member initializer list (real initialization, not
  // default-construct-then-assign), so `T` need not be default-constructible. Indexing the public
  // `_coordinates_` member directly (rather than `operator[]`, which always yields an lvalue)
  // forwards the value category, so one helper serves both the copy and the move constructor.
  template<typename V, std::size_t... Is>
  constexpr cartesian_vector(std::index_sequence<Is...>, V&& other) :
      _coordinates_{static_cast<T>(std::forward<V>(other)._coordinates_[Is])...}
  {
  }

public:
  template<typename U>
    requires std::constructible_from<T, U>
  constexpr explicit(!detail::ImplicitlyConvertibleScalar<U, T>) cartesian_vector(const cartesian_vector<U, N>& other) :
      cartesian_vector(std::make_index_sequence<N>{}, other)
  {
  }

  template<typename U>
    requires std::constructible_from<T, U>
  constexpr explicit(!detail::ImplicitlyConvertibleScalar<U, T>) cartesian_vector(cartesian_vector<U, N>&& other) :
      cartesian_vector(std::make_index_sequence<N>{}, std::move(other))
  {
  }

  template<typename U>
    requires detail::ImplicitlyConvertibleScalar<U, T>
  constexpr cartesian_vector& operator=(const cartesian_vector<U, N>& other)
  {
    for (std::size_t i = 0; i < N; ++i) _coordinates_[i] = other[i];
    return *this;
  }

  template<typename U>
    requires detail::ImplicitlyConvertibleScalar<U, T>
  constexpr cartesian_vector& operator=(cartesian_vector<U, N>&& other)
  {
    for (std::size_t i = 0; i < N; ++i) _coordinates_[i] = std::move(other[i]);
    return *this;
  }

  [[nodiscard]] constexpr auto magnitude() const
    requires requires(T t) {
      requires(
                requires { hypot(t, t, t); } || requires { std::hypot(t, t, t); }) ||
                requires { ::mp_units::modulus(t); };
    }
  {
    if constexpr (detail::ComplexScalar<T>) {
      // Hermitian norm sqrt(sum |zᵢ|²); the real branch keeps hypot for its overflow behavior
      using std::sqrt;
      return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return sqrt((... + (::mp_units::modulus(_coordinates_[Is]) * ::mp_units::modulus(_coordinates_[Is]))));
      }(std::make_index_sequence<N>{});
    } else {
      using std::hypot;
      if constexpr (N == 2)
        return hypot(_coordinates_[0], _coordinates_[1]);
      else
        return hypot(_coordinates_[0], _coordinates_[1], _coordinates_[2]);
    }
  }

  [[nodiscard]] constexpr auto norm() const
    requires requires(const cartesian_vector& v) { v.magnitude(); }
  {
    return magnitude();
  }

  [[nodiscard]] constexpr cartesian_vector unit() const
    requires treat_as_floating_point<T>
  {
    return *this / norm();
  }

  [[nodiscard]] constexpr T& operator[](std::size_t i) { return _coordinates_[i]; }
  [[nodiscard]] constexpr const T& operator[](std::size_t i) const { return _coordinates_[i]; }

  // Component-wise real and imaginary parts, present only for complex elements. Their existence is
  // what marks this type as a complex (rather than real) representation through the `real`/`imag`
  // customization points.
  [[nodiscard]] constexpr auto real() const
    requires detail::ComplexScalar<T>
  {
    return detail::cartesian_vector_from(std::make_index_sequence<N>{},
                                         [&](std::size_t i) { return ::mp_units::real(_coordinates_[i]); });
  }

  [[nodiscard]] constexpr auto imag() const
    requires detail::ComplexScalar<T>
  {
    return detail::cartesian_vector_from(std::make_index_sequence<N>{},
                                         [&](std::size_t i) { return ::mp_units::imag(_coordinates_[i]); });
  }

  // tuple-like element access (enables structured bindings and vector quantity decomposition)
  template<std::size_t Idx>
  [[nodiscard]] friend constexpr T& get(cartesian_vector& v) noexcept
  {
    return v._coordinates_[Idx];
  }

  template<std::size_t Idx>
  [[nodiscard]] friend constexpr const T& get(const cartesian_vector& v) noexcept
  {
    return v._coordinates_[Idx];
  }

  template<std::size_t Idx>
  [[nodiscard]] friend constexpr T&& get(cartesian_vector&& v) noexcept
  {
    return std::move(v._coordinates_[Idx]);
  }

  template<std::size_t Idx>
  [[nodiscard]] friend constexpr const T&& get(const cartesian_vector&& v) noexcept
  {
    return std::move(v._coordinates_[Idx]);
  }

  [[nodiscard]] constexpr cartesian_vector operator+() const { return *this; }
  [[nodiscard]] constexpr cartesian_vector operator-() const
  {
    return detail::cartesian_vector_from(std::make_index_sequence<N>{},
                                         [&](std::size_t i) { return -_coordinates_[i]; });
  }

  template<typename U>
    requires requires(T& t, const U& u) {
      { t += u } -> std::same_as<T&>;
    }
  constexpr cartesian_vector& operator+=(const cartesian_vector<U, N>& other)
  {
    for (std::size_t i = 0; i < N; ++i) _coordinates_[i] += other[i];
    return *this;
  }

  template<typename U>
    requires requires(T& t, const U& u) {
      { t -= u } -> std::same_as<T&>;
    }
  constexpr cartesian_vector& operator-=(const cartesian_vector<U, N>& other)
  {
    for (std::size_t i = 0; i < N; ++i) _coordinates_[i] -= other[i];
    return *this;
  }

  template<typename U>
    requires requires(T& t, const U& u) {
      { t *= u } -> std::same_as<T&>;
    }
  constexpr cartesian_vector& operator*=(const U& value)
  {
    for (std::size_t i = 0; i < N; ++i) _coordinates_[i] *= value;
    return *this;
  }

  template<typename U>
    requires requires(T& t, const U& u) {
      { t /= u } -> std::same_as<T&>;
    }
  constexpr cartesian_vector& operator/=(const U& value)
  {
    for (std::size_t i = 0; i < N; ++i) _coordinates_[i] /= value;
    return *this;
  }

  [[nodiscard]] friend constexpr auto norm(const cartesian_vector& vec)
    requires requires { vec.norm(); }
  {
    return vec.norm();
  }

  [[nodiscard]] friend constexpr auto magnitude(const cartesian_vector& vec)
    requires requires { vec.norm(); }
  {
    return vec.norm();
  }

  [[nodiscard]] friend constexpr cartesian_vector unit_vector(const cartesian_vector& vec)
    requires treat_as_floating_point<T>
  {
    return vec.unit();
  }

#if MP_UNITS_HOSTED
  friend constexpr std::ostream& operator<<(std::ostream& os, const cartesian_vector& vec)
  {
    os << '[';
    for (std::size_t i = 0; i < N; ++i) os << (i == 0 ? "" : ", ") << vec[i];
    return os << ']';
  }
#endif
};

template<typename Arg, typename... Args>
  requires requires { typename std::common_type_t<Arg, Args...>; }
cartesian_vector(Arg, Args...) -> cartesian_vector<std::common_type_t<Arg, Args...>, 1 + sizeof...(Args)>;

// Explicit conversions between dimensions (there is no implicit cross-dimension conversion). `embed`
// is the canonical inclusion of the plane into space, zero-filling the new coordinate; `project` is
// the canonical projection onto the plane, dropping the last coordinate. The zero is the additive
// identity derived from a component (`x - x`), so no value-initialization of `T` is required.
MP_UNITS_EXPORT template<typename T>
[[nodiscard]] constexpr cartesian_vector<T, 3> embed(const cartesian_vector<T, 2>& v)
{
  return cartesian_vector<T, 3>{v[0], v[1], v[0] - v[0]};
}

MP_UNITS_EXPORT template<typename T>
[[nodiscard]] constexpr cartesian_vector<T, 2> project(const cartesian_vector<T, 3>& v)
{
  return cartesian_vector<T, 2>{v[0], v[1]};
}

}  // namespace mp_units

template<typename T, std::size_t N, typename U>
  requires requires { typename std::common_type_t<T, U>; }
struct std::common_type<mp_units::cartesian_vector<T, N>, mp_units::cartesian_vector<U, N>> {
  using type = mp_units::cartesian_vector<std::common_type_t<T, U>, N>;
};

template<typename T, std::size_t N>
struct std::tuple_size<mp_units::cartesian_vector<T, N>> : std::integral_constant<std::size_t, N> {};

template<std::size_t Idx, typename T, std::size_t N>
struct std::tuple_element<Idx, mp_units::cartesian_vector<T, N>> {
  using type = T;
};

#if MP_UNITS_HOSTED
// TODO use parse and use formatter for the underlying type
template<typename T, std::size_t N, typename Char>
struct MP_UNITS_STD_FMT::formatter<mp_units::cartesian_vector<T, N>, Char> :
    formatter<std::basic_string_view<Char>, Char> {
  template<typename FormatContext>
  auto format(const mp_units::cartesian_vector<T, N>& vec, FormatContext& ctx) const
  {
    auto out = format_to(ctx.out(), "[");
    for (std::size_t i = 0; i < N; ++i) out = format_to(out, "{}{}", i == 0 ? "" : ", ", vec[i]);
    return format_to(out, "]");
  }
};
#endif
