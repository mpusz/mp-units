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
#include <mp-units/quantity.h>
#include <mp-units/math.h>

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

// ============================================================================
// Cartesian 3D vector & up-to-3x3 tensor reps + quantity-level LA operations
// ============================================================================

namespace mp_units {

MP_UNITS_EXPORT template<detail::Scalar T>
class cartesian_vector;

MP_UNITS_EXPORT template<detail::Scalar T = double, std::size_t R = 3, std::size_t C = 3>
class cartesian_tensor;

// --------------------------- cartesian_vector -------------------------------

namespace detail {

struct cartesian_vector_iface { // -> how it behaves (math)
  // elementwise A + B
  template<typename T, typename U>
    requires requires(const T& t, const U& u) { t + u; } //only compile this if the underlying number types can be added
  [[nodiscard]] friend constexpr auto operator+(const cartesian_vector<T>& lhs,
                                                const cartesian_vector<U>& rhs)
  {
    return ::mp_units::cartesian_vector{lhs._coordinates_[0] + rhs._coordinates_[0],
                                        lhs._coordinates_[1] + rhs._coordinates_[1],
                                        lhs._coordinates_[2] + rhs._coordinates_[2]};
  }

  // elementwise A - B
  template<typename T, typename U>
    requires requires(const T& t, const U& u) { t - u; }
  [[nodiscard]] friend constexpr auto operator-(const cartesian_vector<T>& lhs,
                                                const cartesian_vector<U>& rhs)
  {
    return ::mp_units::cartesian_vector{lhs._coordinates_[0] - rhs._coordinates_[0],
                                        lhs._coordinates_[1] - rhs._coordinates_[1],
                                        lhs._coordinates_[2] - rhs._coordinates_[2]};
  }

  // elementwise A % B  (integral: %, floating: fmod)
  template<typename T, typename U>
    requires requires(const T& t, const U& u) { t % u; } || (std::floating_point<T> && std::floating_point<U>)
  [[nodiscard]] friend constexpr auto operator%(const cartesian_vector<T>& lhs,
                                                const cartesian_vector<U>& rhs)
  {
    if constexpr (std::floating_point<T> && std::floating_point<U>) {
      using std::fmod;
      return ::mp_units::cartesian_vector{
        fmod(static_cast<long double>(lhs._coordinates_[0]), static_cast<long double>(rhs._coordinates_[0])),
        fmod(static_cast<long double>(lhs._coordinates_[1]), static_cast<long double>(rhs._coordinates_[1])),
        fmod(static_cast<long double>(lhs._coordinates_[2]), static_cast<long double>(rhs._coordinates_[2]))};
    } else {
      return ::mp_units::cartesian_vector{
        lhs._coordinates_[0] % rhs._coordinates_[0],
        lhs._coordinates_[1] % rhs._coordinates_[1],
        lhs._coordinates_[2] % rhs._coordinates_[2]};
    }
  }

  // scalar multiply (vector * scalar)
  template<typename T, typename S>
    requires requires(const T& t, const S& s) { t * s; }
  [[nodiscard]] friend constexpr auto operator*(const cartesian_vector<T>& vector, const S& scalar)
  {
    return ::mp_units::cartesian_vector{vector._coordinates_[0] * scalar,
                                        vector._coordinates_[1] * scalar,
                                        vector._coordinates_[2] * scalar};
  }

  // scalar multiply (scalar * vector)
  template<typename S, typename T>
    requires requires(const S& s, const T& t) { s * t; }
  [[nodiscard]] friend constexpr auto operator*(const S& scalar, const cartesian_vector<U>& vector)
  {
    return vector * scalar;
  }

  // scalar divide (vector / scalar)
  template<typename T, typename S>
    requires requires(const T& t, const S& s) { t / s; }
  [[nodiscard]] friend constexpr auto operator/(const cartesian_vector<T>& vector, const S& scalar)
  {
    return ::mp_units::cartesian_vector{vector._coordinates_[0] / scalar,
                                        vector._coordinates_[1] / scalar,
                                        vector._coordinates_[2] / scalar};
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

  // Dot product (scalar product of vectors)
  template<typename T, typename U>
    requires requires(const T& t, const U& u, decltype(t * u) v) { t * u; v + v; }
  [[nodiscard]] friend constexpr auto scalar_product(const cartesian_vector<T>& lhs,
                                                     const cartesian_vector<U>& rhs)
  {
    return lhs._coordinates_[0] * rhs._coordinates_[0] +
           lhs._coordinates_[1] * rhs._coordinates_[1] +
           lhs._coordinates_[2] * rhs._coordinates_[2];
  }

  // Cross product (vector product) — 3D only
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

MP_UNITS_EXPORT template<detail::Scalar T = double>
class cartesian_vector : public detail::cartesian_vector_iface { // -> what it is (the data)
public:
  T _coordinates_[3]{};
  using value_type = T; //number type inside the vector

  cartesian_vector() = default;
  cartesian_vector(const cartesian_vector&) = default;
  cartesian_vector(cartesian_vector&&) = default;
  cartesian_vector& operator=(const cartesian_vector&) = default;
  cartesian_vector& operator=(cartesian_vector&&) = default;

  template<typename... Args>
    requires (sizeof...(Args) == 3) && (... && std::constructible_from<T, Args>)
  constexpr explicit(!(... && std::convertible_to<Args, T>))
  cartesian_vector(Args&&... args)
      : _coordinates_{static_cast<T>(std::forward<Args>(args))...} {}

  template<typename U>
    requires std::constructible_from<T, const U&>
  constexpr explicit(!std::convertible_to<U, T>)
  cartesian_vector(const cartesian_vector<U>& other)
      : _coordinates_{static_cast<T>(other[0]),
                      static_cast<T>(other[1]),
                      static_cast<T>(other[2])} {}

  template<typename U>
    requires std::constructible_from<T, U&&>
  constexpr explicit(!std::convertible_to<U, T>)
  cartesian_vector(cartesian_vector<U>&& other)
      : _coordinates_{static_cast<T>(std::move(other[0])),
                      static_cast<T>(std::move(other[1])),
                      static_cast<T>(std::move(other[2]))} {}

  template<std::convertible_to<T> U>
  constexpr cartesian_vector& operator=(const cartesian_vector<U>& other)
  {
    _coordinates_[0] = other[0];
    _coordinates_[1] = other[1];
    _coordinates_[2] = other[2];
    return *this;
  }

  template<std::convertible_to<T> U>
  constexpr cartesian_vector& operator=(cartesian_vector<U>&& other)
  {
    _coordinates_[0] = std::move(other[0]);
    _coordinates_[1] = std::move(other[1]);
    _coordinates_[2] = std::move(other[2]);
    return *this;
  }

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

  [[nodiscard]] constexpr       T& operator[](std::size_t i)       { return _coordinates_[i]; }
  [[nodiscard]] constexpr const T& operator[](std::size_t i) const { return _coordinates_[i]; }

  [[nodiscard]] constexpr cartesian_vector operator+() const { return *this; }
  [[nodiscard]] constexpr cartesian_vector operator-() const
  {
    return {-_coordinates_[0], -_coordinates_[1], -_coordinates_[2]};
  }

  template<typename U> requires requires(T& t, const U& u) { t += u; }
  constexpr cartesian_vector& operator+=(const cartesian_vector<U>& other)
  {
    _coordinates_[0] += other[0];
    _coordinates_[1] += other[1];
    _coordinates_[2] += other[2];
    return *this;
  }

  template<typename U> requires requires(T& t, const U& u) { t -= u; }
  constexpr cartesian_vector& operator-=(const cartesian_vector<U>& v)
  {
    _coordinates_[0] -= other[0];
    _coordinates_[1] -= other[1];
    _coordinates_[2] -= other[2];
    return *this;
  }

  template<typename U> requires requires(T& t, const U& u) { t *= u; }
  constexpr cartesian_vector& operator*=(const U& value)
  {
    _coordinates_[0] *= value;
    _coordinates_[1] *= value;
    _coordinates_[2] *= value;
    return *this;
  }

  template<typename U> requires requires(T& t, const U& u) { t /= u; }
  constexpr cartesian_vector& operator/=(const S& s)
  {
    _coordinates_[0] /= s; _coordinates_[1] /= s; _coordinates_[2] /= s;
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

// CTAD guide: deduce value_type from ctor args
template<typename Arg, typename... Args>
  requires (sizeof...(Args) == 2) && requires { typename std::common_type_t<Arg, Args...>; }
cartesian_vector(Arg, Args...) -> cartesian_vector<std::common_type_t<Arg, Args...>>;

// Friendly aliases for numeric-level ops
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

// Register as a vector representation type
template<detail::Scalar T>
inline constexpr bool is_vector<cartesian_vector<T>> = true;

// ---------------------------- cartesian_tensor ------------------------------

namespace detail {
template<typename T> struct type_identity { using type = T; };
template<typename T> using type_identity_t = typename type_identity<T>::type;
} // namespace detail

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

  template<typename... Args>
    requires (sizeof...(Args) == R * C) && (... && std::constructible_from<T, Args>)
  constexpr explicit(!(... && std::convertible_to<Args, T>))
  cartesian_tensor(Args&&... args) : _data_{static_cast<T>(std::forward<Args>(args))...} {}

  [[nodiscard]] constexpr       T& operator()(std::size_t r, std::size_t c)       { return _data_[r * C + c]; }
  [[nodiscard]] constexpr const T& operator()(std::size_t r, std::size_t c) const { return _data_[r * C + c]; }

  template<typename U>
    requires requires(const T& t, const U& u) { t + u; }
  [[nodiscard]] friend constexpr auto operator+(const cartesian_tensor& lhs, const cartesian_tensor<U, R, C>& rhs)
  {
    using CT = std::common_type_t<T, U>;
    cartesian_tensor<CT, R, C> Rm{};
    for (std::size_t i = 0; i < R * C; ++i) Rm._data_[i] = static_cast<CT>(lhs._data_[i]) + static_cast<CT>(rhs._data_[i]);
    return Rm;
  }

  template<typename U>
    requires requires(const T& t, const U& u) { t - u; }
  [[nodiscard]] friend constexpr auto operator-(const cartesian_tensor& lhs, const cartesian_tensor<U, R, C>& rhs)
  {
    using CT = std::common_type_t<T, U>;
    cartesian_tensor<CT, R, C> Rm{};
    for (std::size_t i = 0; i < R * C; ++i) Rm._data_[i] = static_cast<CT>(lhs._data_[i]) - static_cast<CT>(rhs._data_[i]);
    return Rm;
  }

  template<typename U>
    requires (requires(const T& t, const U& u) { t % u; } || (std::floating_point<T> && std::floating_point<U>))
  [[nodiscard]] friend constexpr auto operator%(const cartesian_tensor& lhs, const cartesian_tensor<U, R, C>& rhs)
  {
    using CT = std::conditional_t<(std::floating_point<T> || std::floating_point<U>), long double, std::common_type_t<T,U>>;
    cartesian_tensor<CT, R, C> Rm{};
    if constexpr (std::floating_point<T> || std::floating_point<U>) {
      using std::fmod;
      for (std::size_t i = 0; i < R * C; ++i)
        Rm._data_[i] = static_cast<CT>(fmod(static_cast<long double>(lhs._data_[i]),
                                            static_cast<long double>(rhs._data_[i])));
    } else {
      for (std::size_t i = 0; i < R * C; ++i)
        Rm._data_[i] = static_cast<CT>(lhs._data_[i] % rhs._data_[i]);
    }
    return Rm;
  }

  template<typename S>
    requires requires(const T& t, const S& s) { t * s; }
  [[nodiscard]] friend constexpr auto operator*(const cartesian_tensor& tensor, const S& scalar)
  {
    using CT = std::common_type_t<T, S>;
    cartesian_tensor<CT, R, C> Rm{};
    for (std::size_t i = 0; i < R * C; ++i) Rm._data_[i] = static_cast<CT>(tensor._data_[i]) * static_cast<CT>(scalar);
    return Rm;
  }

  template<typename S>
    requires requires(const S& s, const T& t) { s * t; }
  [[nodiscard]] friend constexpr auto operator*(const S& scalar, const cartesian_tensor& tensor) { return tensor * scalar; }

  template<typename S>
    requires requires(const T& t, const S& s) { t / s; }
  [[nodiscard]] friend constexpr auto operator/(const cartesian_tensor& tensor, const S& scalar)
  {
    using CT = std::common_type_t<T, S>;
    cartesian_tensor<CT, R, C> Rm{};
    for (std::size_t i = 0; i < R * C; ++i) Rm._data_[i] = static_cast<CT>(tensor._data_[i]) / static_cast<CT>(scalar);
    return Rm;
  }

#if MP_UNITS_HOSTED
  friend std::ostream& operator<<(std::ostream& os, const cartesian_tensor& tensor)
  {
    for (std::size_t r = 0; r < R; ++r) {
      os << (r == 0 ? "[[" : " [");
      for (std::size_t c = 0; c < C; ++c) {
        os << tensor(r, c);
        if (c + 1 != C) os << ", ";
      }
      os << (r + 1 == R ? "]]" : "]\n");
    }
    return os;
  }
#endif
};

// Register as a tensor representation type
template<detail::Scalar T, std::size_t R, std::size_t C>
inline constexpr bool is_tensor<cartesian_tensor<T, R, C>> = true;

// ---------------------- Numeric-level helpers for tensors -------------------

template<typename T, typename U, std::size_t R, std::size_t K, std::size_t C>
[[nodiscard]] constexpr auto matmul(const cartesian_tensor<T, R, K>& tensor,
                                    const cartesian_tensor<U, K, C>& other)
{
  using CT = std::common_type_t<T, U>;
  cartesian_tensor<CT, R, C> Rm{};
  for (std::size_t r = 0; r < R; ++r)
    for (std::size_t c = 0; c < C; ++c) {
      CT acc{};
      for (std::size_t k = 0; k < K; ++k) acc += static_cast<CT>(tensor(r, k)) * static_cast<CT>(other(k, c));
      Rm(r, c) = acc;
    }
  return Rm;
}

template<typename T, typename U>
[[nodiscard]] constexpr auto matvec(const cartesian_tensor<T, 3, 3>& tensor,
                                    const cartesian_vector<U>& vector)
{
  using CT = std::common_type_t<T, U>;
  cartesian_vector<CT> Rv{};
  for (std::size_t r = 0; r < 3; ++r) {
    CT acc{};
    for (std::size_t c = 0; c < 3; ++c) acc += static_cast<CT>(tensor(r, c)) * static_cast<CT>(vector._coordinates_[c]);
    Rv[r] = acc;
  }
  return Rv;
}

template<typename T, typename U, std::size_t R, std::size_t C>
[[nodiscard]] constexpr auto double_contraction(const cartesian_tensor<T, R, C>& tensor,
                                                const cartesian_tensor<U, R, C>& other)
{
  using CT = std::common_type_t<T, U>;
  CT acc{};
  for (std::size_t i = 0; i < R * C; ++i) acc += static_cast<CT>(tensor._data_[i]) * static_cast<CT>(other._data_[i]);
  return acc; 
}

template<typename T, typename U> 
[[nodiscard]] constexpr auto outer_numeric(const cartesian_vector<T>& vector,
                                           const cartesian_vector<U>& other)
{
  using CT = std::common_type_t<T, U>;
  cartesian_tensor<CT, 3, 3> Rm{};
  for (std::size_t i = 0; i < 3; ++i)
    for (std::size_t j = 0; j < 3; ++j)
      Rm(i, j) = static_cast<CT>(vector._coordinates_[i]) * static_cast<CT>(other._coordinates_[j]);
  return Rm;
}

// ---------------------- Quantity-level LA operations ------------------------

namespace detail {
  template<class Q>
  [[nodiscard]] consteval auto ref_of()
  {
    return std::remove_cvref_t<Q>::reference;
  }
} // namespace detail

// ---- Vectors ----

// dot: a ⋅ b -> scalar quantity (units multiply)
template<class QA, class QB>
  requires (Quantity<QA> && Quantity<QB> &&
            is_vector<typename std::remove_cvref_t<QA>::rep> &&
            is_vector<typename std::remove_cvref_t<QB>::rep>)
[[nodiscard]] constexpr auto dot(const QA& a, const QB& b)
{
  using A = std::remove_cvref_t<QA>;
  using B = std::remove_cvref_t<QB>;
  constexpr auto RA = detail::ref_of<A>();
  constexpr auto RB = detail::ref_of<B>();
  const auto av = a.numerical_value_in(RA);
  const auto bv = b.numerical_value_in(RB);
  return quantity{ scalar_product(av, bv), RA * RB };
}

// cross: a × b -> vector quantity (units multiply)
template<class QV>
  requires (Quantity<QV> && is_vector<typename std::remove_cvref_t<QV>::rep>)
[[nodiscard]] constexpr auto cross(const QV& a, const QV& b)
{
  using VQ = std::remove_cvref_t<QV>;
  constexpr auto R = detail::ref_of<VQ>();
  const auto av = a.numerical_value_in(R);
  const auto bv = b.numerical_value_in(R);
  return quantity{ vector_product(av, bv), R * R };
}

// |a|: magnitude -> scalar quantity (sqrt(a⋅a))
template<class QV>
  requires (Quantity<QV> && is_vector<typename std::remove_cvref_t<QV>::rep>)
[[nodiscard]] constexpr auto magnitude(const QV& a)
{
  auto aa = dot(a, a);          // scalar quantity with R*R
  using mp_units::sqrt;
  return sqrt(aa);              // scalar quantity with R
}

// ---- Tensor / Vector & Tensor / Tensor ----

// tensor product: a ⊗ b (vector ⊗ vector -> tensor)
template<class QA, class QB>
  requires (Quantity<QA> && Quantity<QB> &&
            is_vector<typename std::remove_cvref_t<QA>::rep> &&
            is_vector<typename std::remove_cvref_t<QB>::rep>)
[[nodiscard]] constexpr auto outer(const QA& a, const QB& b)
{
  using A = std::remove_cvref_t<QA>;
  using B = std::remove_cvref_t<QB>;
  constexpr auto RA = detail::ref_of<A>();
  constexpr auto RB = detail::ref_of<B>();
  const auto av = a.numerical_value_in(RA);
  const auto bv = b.numerical_value_in(RB);
  return quantity{ outer_numeric(av, bv), RA * RB };
}

// inner: A ⋅ x (tensor ⋅ vector) -> vector
template<class QT, class QV>
  requires (Quantity<QT> && Quantity<QV> &&
            is_tensor<typename std::remove_cvref_t<QT>::rep> &&
            is_vector<typename std::remove_cvref_t<QV>::rep>)
[[nodiscard]] constexpr auto inner(const QT& A, const QV& x)
{
  using TQ = std::remove_cvref_t<QT>;
  using VQ = std::remove_cvref_t<QV>;
  constexpr auto RT = detail::ref_of<TQ>();
  constexpr auto RX = detail::ref_of<VQ>();
  const auto Av = A.numerical_value_in(RT);
  const auto xv = x.numerical_value_in(RX);
  return quantity{ matvec(Av, xv), RT * RX };
}

// inner: A ⋅ B (tensor ⋅ tensor) -> tensor
template<class QA, class QB>
  requires (Quantity<QA> && Quantity<QB> &&
            is_tensor<typename std::remove_cvref_t<QA>::rep> &&
            is_tensor<typename std::remove_cvref_t<QB>::rep>)
[[nodiscard]] constexpr auto inner(const QA& A, const QB& B)
{
  using TA = std::remove_cvref_t<QA>;
  using TB = std::remove_cvref_t<QB>;
  constexpr auto RA = detail::ref_of<TA>();
  constexpr auto RB = detail::ref_of<TB>();
  const auto Av = A.numerical_value_in(RA);
  const auto Bv = B.numerical_value_in(RB);
  return quantity{ matmul(Av, Bv), RA * RB };
}

// scalar product: A : B (double contraction) -> scalar
template<class QA, class QB>
  requires (Quantity<QA> && Quantity<QB> &&
            is_tensor<typename std::remove_cvref_t<QA>::rep> &&
            is_tensor<typename std::remove_cvref_t<QB>::rep>)
[[nodiscard]] constexpr auto scalar_product(const QA& A, const QB& B)
{
  using TA = std::remove_cvref_t<QA>;
  using TB = std::remove_cvref_t<QB>;
  constexpr auto RA = detail::ref_of<TA>();
  constexpr auto RB = detail::ref_of<TB>();
  const auto Av = A.numerical_value_in(RA);
  const auto Bv = B.numerical_value_in(RB);
  return quantity{ double_contraction(Av, Bv), RA * RB };
}

} // namespace mp_units

#if MP_UNITS_HOSTED
template<typename T, typename Char>
struct MP_UNITS_STD_FMT::formatter<mp_units::cartesian_vector<T>, Char>
    : formatter<std::basic_string_view<Char>, Char> {
  template<typename Ctx>
  auto format(const mp_units::cartesian_vector<T>& v, Ctx& ctx) const {
    return format_to(ctx.out(), "[{}, {}, {}]", v._coordinates_[0], v._coordinates_[1], v._coordinates_[2]);
  }
};

template<typename T, std::size_t R, std::size_t C, typename Char>
struct MP_UNITS_STD_FMT::formatter<mp_units::cartesian_tensor<T, R, C>, Char>
    : formatter<std::basic_string_view<Char>, Char> {
  template<typename Ctx>
  auto format(const mp_units::cartesian_tensor<T, R, C>& A, Ctx& ctx) const {
    auto out = ctx.out();
    for (std::size_t r = 0; r < R; ++r) {
      out = format_to(out, r == 0 ? "[[" : " [");
      for (std::size_t c = 0; c < C; ++c) {
        out = format_to(out, "{}", A(r, c));
        if (c + 1 != C) out = format_to(out, ", ");
      }
      out = format_to(out, r + 1 == R ? "]]" : "]\n");
    }
    return out;
  }
};
#endif
