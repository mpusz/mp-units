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

#include "units/math.h"
#include "units/physical/si/area.h"
#include "units/physical/si/frequency.h"
#include "units/physical/si/velocity.h"
#include <chrono>
#include <utility>

using namespace units;

namespace {

template<typename T>
struct arithmetic_ops {
  // constexpr T& operator+=(T other) { value_ += other.value_; return *this; }
  // constexpr T& operator-=(T other) { value_ -= other.value_; return *this; }
  // constexpr T& operator*=(T other) { value_ *= other.value_; return *this; }
  // constexpr T& operator/=(T other) { value_ /= other.value_; return *this; }

  // [[nodiscard]] constexpr T operator-() const { return T(-value_); }

  [[nodiscard]] friend constexpr T operator+(T lhs, T rhs) {
    return T(lhs.value_ + rhs.value_);
  }
  [[nodiscard]] friend constexpr T operator-(T lhs, T rhs) {
    return T(lhs.value_ - rhs.value_);
  }
  [[nodiscard]] friend constexpr T operator*(T lhs, T rhs) {
    return T(lhs.value_ * rhs.value_);
  }
  [[nodiscard]] friend constexpr T operator/(T lhs, T rhs) {
    return T(lhs.value_ / rhs.value_);
  }

  [[nodiscard]] friend constexpr bool operator==(T lhs, T rhs) { return lhs.value_ == rhs.value_; }
  [[nodiscard]] friend constexpr bool operator!=(T lhs, T rhs) { return !(lhs == rhs); }
  [[nodiscard]] friend constexpr bool operator<(T lhs, T rhs) { return lhs.value_ < rhs.value_; }
  [[nodiscard]] friend constexpr bool operator>(T lhs, T rhs) { return rhs < lhs; }
  [[nodiscard]] friend constexpr bool operator<=(T lhs, T rhs) { return !(rhs < lhs); }
  [[nodiscard]] friend constexpr bool operator>=(T lhs, T rhs) { return !(lhs < rhs); }
};

template<typename T>
struct impl_constructible_impl_convertible : arithmetic_ops<impl_constructible_impl_convertible<T>> {
  T value_{};
  impl_constructible_impl_convertible() = default;
  constexpr impl_constructible_impl_convertible(T v) : value_(std::move(v)) {}
  constexpr operator const T&() const& { return value_; }
};

template<typename T>
using impl_impl = impl_constructible_impl_convertible<T>;

static_assert(std::convertible_to<float, impl_impl<float>>);
static_assert(std::convertible_to<impl_impl<float>, float>);
static_assert(units::Scalar<impl_impl<float>>);

template<typename T>
struct expl_constructible_impl_convertible : arithmetic_ops<expl_constructible_impl_convertible<T>> {
  T value_{};
  expl_constructible_impl_convertible() = default;
  constexpr explicit expl_constructible_impl_convertible(T v) : value_(std::move(v)) {}
  constexpr operator const T&() const& { return value_; }
};

template<typename T>
using expl_impl = expl_constructible_impl_convertible<T>;

static_assert(!std::convertible_to<float, expl_impl<float>>);
static_assert(std::convertible_to<expl_impl<float>, float>);
static_assert(units::Scalar<expl_impl<float>>);

template<typename T>
struct impl_constructible_expl_convertible : arithmetic_ops<impl_constructible_expl_convertible<T>> {
  T value_{};
  impl_constructible_expl_convertible() = default;
  constexpr impl_constructible_expl_convertible(T v) : value_(std::move(v)) {}
  constexpr explicit operator const T&() const& { return value_; }
};

template<typename T>
using impl_expl = impl_constructible_expl_convertible<T>;

static_assert(std::convertible_to<float, impl_expl<float>>);
static_assert(!std::convertible_to<impl_expl<float>, float>);
static_assert(units::Scalar<impl_expl<float>>);

template<typename T>
struct expl_constructible_expl_convertible : arithmetic_ops<expl_constructible_expl_convertible<T>> {
  T value_{};
  expl_constructible_expl_convertible() = default;
  constexpr explicit expl_constructible_expl_convertible(T v) : value_(std::move(v)) {}
  constexpr explicit operator const T&() const& { return value_; }
};

template<typename T>
using expl_expl = expl_constructible_expl_convertible<T>;

static_assert(!std::convertible_to<float, expl_expl<float>>);
static_assert(!std::convertible_to<expl_expl<float>, float>);
static_assert(units::Scalar<expl_expl<float>>);

}  // namespace

namespace units {

template<typename T>
inline constexpr bool treat_as_floating_point<impl_impl<T>> = std::is_floating_point_v<T>;

template<typename T>
inline constexpr bool treat_as_floating_point<expl_impl<T>> = std::is_floating_point_v<T>;

template<typename T>
inline constexpr bool treat_as_floating_point<impl_expl<T>> = std::is_floating_point_v<T>;

template<typename T>
inline constexpr bool treat_as_floating_point<expl_expl<T>> = std::is_floating_point_v<T>;

template<typename T>
struct quantity_values<impl_impl<T>> {
  static constexpr impl_impl<T> zero() { return impl_impl<T>(0); }
  static constexpr impl_impl<T> max() { return std::numeric_limits<T>::max(); }
  static constexpr impl_impl<T> min() { return std::numeric_limits<T>::lowest(); }
};

}  // namespace units

namespace std {

// template<typename T, typename U>
// struct common_type<my_value<T>, my_value<U>> : std::type_identity<my_value<common_type_t<T, U>>> {};

// template<typename T, typename U>
// struct common_type<my_value<T>, U> : common_type<T, U> {};

// template<typename T, typename U>
// struct common_type<T, my_value<U>> : common_type<T, U> {};

}  // namespace std

namespace {

using namespace units::si;

// constructors

// Quantity from Scalar
// int <- int
static_assert(length<metre, int>(expl_impl<int>(1)).count() == 1);
// static_assert(length<metre, int>(impl_expl<int>(1)).count() == 1);  // should not compile (not convertible)
static_assert(length<metre, int>(int(impl_expl<int>(1))).count() == 1);
// static_assert(length<metre, expl_impl<int>>(1).count() == expl_impl<int>{1});  // should not compile (not convertible)
static_assert(length<metre, expl_impl<int>>(expl_impl<int>(1)).count() == expl_impl<int>{1});
static_assert(length<metre, impl_expl<int>>(1).count() == impl_expl<int>{1});

// double <- double
static_assert(length<metre, double>(expl_impl<double>(1.0)).count() == 1.0);
// static_assert(length<metre, double>(impl_expl<double>(1.0)).count() == 1.0);  // should not compile (not convertible)
static_assert(length<metre, double>(double(impl_expl<double>(1.0))).count() == 1.0);
// static_assert(length<metre, expl_impl<double>>(1.0).count() == expl_impl<double>{1.0});  // should not compile (not convertible)
static_assert(length<metre, expl_impl<double>>(expl_impl<double>(1.0)).count() == expl_impl<double>{1.0});
static_assert(length<metre, impl_expl<double>>(1.0).count() == impl_expl<double>{1.0});

// double <- int
static_assert(length<metre, double>(expl_impl<int>(1)).count() == 1.0);
// static_assert(length<metre, double>(impl_expl<int>(1)).count() == 1.0);  // should not compile (not convertible)
static_assert(length<metre, double>(int(impl_expl<int>(1))).count() == 1.0);
// static_assert(length<metre, expl_impl<double>>(1).count() == expl_impl<double>{1});  // should not compile (not convertible)
static_assert(length<metre, expl_impl<double>>(expl_impl<double>(1)).count() == expl_impl<double>{1});
static_assert(length<metre, impl_expl<double>>(1).count() == impl_expl<double>{1.0});

// int <- double
// static_assert(length<metre, int>(expl_impl<double>(1.0)).count() == 1); // should not compile (truncating conversion)
// static_assert(length<metre, impl_expl<int>>(1.0).count() == impl_expl<int>{1});   // should not compile (truncating conversion)

// Quantity from other Quantity with different Rep
// int <- int
static_assert(length<metre, int>(length<metre, expl_impl<int>>(expl_impl<int>(1))).count() == 1);
// static_assert(length<metre, int>(length<metre, impl_expl<int>>(1)).count() == 1);  // should not compile (not convertible)
static_assert(length<metre, int>(quantity_cast<int>(length<metre, impl_expl<int>>(1))).count() == 1);
// static_assert(length<metre, expl_impl<int>>(length<metre, int>(1)).count() == expl_impl<int>{1});  // should not compile (not convertible)
static_assert(length<metre, expl_impl<int>>(quantity_cast<expl_impl<int>>(length<metre, int>(1))).count() == expl_impl<int>{1});
static_assert(length<metre, impl_expl<int>>(length<metre, int>(1)).count() == impl_expl<int>{1});

// double <- double
static_assert(length<metre, double>(length<metre, expl_impl<double>>(expl_impl<double>(1.0))).count() == 1.0);
// static_assert(length<metre, double>(length<metre, impl_expl<double>>(1.0)).count() == 1.0);  // should not compile (not convertible)
static_assert(length<metre, double>(quantity_cast<double>(length<metre, impl_expl<double>>(1.0))).count() == 1.0);
// static_assert(length<metre, expl_impl<double>>(length<metre>(1.0).count() == expl_impl<double>{1.0});  // should not compile (not convertible)
static_assert(length<metre, expl_impl<double>>(quantity_cast<expl_impl<double>>(length<metre>(1.0))).count() == expl_impl<double>{1.0});
static_assert(length<metre, impl_expl<double>>(length<metre>(1.0)).count() == impl_expl<double>{1.0});

// double <- int
static_assert(length<metre, double>(length<metre, expl_impl<int>>(expl_impl<int>(1))).count() == 1.0);
// static_assert(length<metre, double>(length<metre, impl_expl<int>>(1)).count() == 1.0);  // should not compile (not convertible)
static_assert(length<metre, double>(quantity_cast<int>(length<metre, impl_expl<int>>(1))).count() == 1.0);
// static_assert(length<metre, expl_impl<double>>(length<metre, int>(1)).count() == expl_impl<double>{1});  // should not compile (not convertible)
static_assert(length<metre, expl_impl<double>>(quantity_cast<expl_impl<double>>(length<metre, int>(1))).count() == expl_impl<double>{1});
static_assert(length<metre, impl_expl<double>>(length<metre, int>(1)).count() == impl_expl<double>{1.0});

// int <- double
// static_assert(length<metre, int>(length<metre, expl_impl<double>>(1.0)).count() == 1); // should not compile (truncating conversion)
// static_assert(length<metre, impl_expl<int>>(length<metre, double>(1.0)).count() == impl_expl<int>{1});   // should not compile (truncating conversion)

// unit conversions

static_assert(length<metre, impl_expl<int>>(length<kilometre, impl_expl<int>>(1)).count() == impl_expl<int>(1000));
// static_assert(length<kilometre, impl_expl<int>>(length<metre, impl_expl<int>>(2000)).count() == impl_expl<int>(2));  // should not compile (truncating conversion)
static_assert(length<kilometre, impl_expl<int>>(quantity_cast<kilometre>(length<metre, impl_expl<int>>(2000))).count() == impl_expl<int>(2));

static_assert(length<metre, impl_impl<int>>::zero().count() == impl_impl<int>{0});
static_assert(length<metre, impl_impl<int>>::min().count() == impl_impl<int>{std::numeric_limits<int>::lowest()});
static_assert(length<metre, impl_impl<int>>::max().count() == impl_impl<int>{std::numeric_limits<int>::max()});
static_assert(length<metre, impl_impl<double>>::zero().count() == impl_impl<double>{0.0});
static_assert(length<metre, impl_impl<double>>::min().count() == impl_impl<double>{std::numeric_limits<double>::lowest()});
static_assert(length<metre, impl_impl<double>>::max().count() == impl_impl<double>{std::numeric_limits<double>::max()});

}  // namespace
