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
#include "units/physical/si/speed.h"
#include <chrono>
#include <utility>

using namespace units;

namespace {

template<typename T>
struct equality_ops {
  [[nodiscard]] friend constexpr bool operator==(T lhs, T rhs) { return lhs.value_ == rhs.value_; }
  [[nodiscard]] friend constexpr bool operator!=(T lhs, T rhs) { return !(lhs == rhs); }
};

template<typename T>
struct scaling_ops {
  [[nodiscard]] friend constexpr T operator*(T lhs, T rhs) {
    return T(lhs.value_ * rhs.value_);
  }
  [[nodiscard]] friend constexpr T operator/(T lhs, T rhs) {
    return T(lhs.value_ / rhs.value_);
  }
};

template<typename T>
struct scalar_ops : equality_ops<T>, scaling_ops<T> {};

template<typename T>
struct impl_constructible : scalar_ops<impl_constructible<T>> {
  T value_{};
  impl_constructible() = default;
  constexpr impl_constructible(T v) : value_(std::move(v)) {}
  // no conversion to fundamental arithmetic types
};

template<typename T>
using impl = impl_constructible<T>;

template<typename T>
struct expl_constructible : scalar_ops<expl_constructible<T>> {
  T value_{};
  expl_constructible() = default;
  constexpr expl_constructible(T v) : value_(std::move(v)) {}
  // no conversion to fundamental arithmetic types
};

template<typename T>
using expl = expl_constructible<T>;

template<typename T>
struct impl_constructible_impl_convertible : scalar_ops<impl_constructible_impl_convertible<T>> /*, int_scaling_ops<impl_constructible_impl_convertible<T>> */ {
  T value_{};
  impl_constructible_impl_convertible() = default;
  constexpr impl_constructible_impl_convertible(T v) : value_(std::move(v)) {}
  constexpr operator const T&() const& { return value_; }
};

template<typename T>
using impl_impl = impl_constructible_impl_convertible<T>;

static_assert(std::convertible_to<float, impl_impl<float>>);
static_assert(std::convertible_to<impl_impl<float>, float>);
static_assert(units::Value<impl_impl<float>>);

template<typename T>
struct expl_constructible_impl_convertible : scalar_ops<expl_constructible_impl_convertible<T>> {
  T value_{};
  expl_constructible_impl_convertible() = default;
  constexpr explicit expl_constructible_impl_convertible(T v) : value_(std::move(v)) {}
  constexpr operator const T&() const& { return value_; }
};

template<typename T>
using expl_impl = expl_constructible_impl_convertible<T>;

static_assert(!std::convertible_to<float, expl_impl<float>>);
static_assert(std::convertible_to<expl_impl<float>, float>);
static_assert(units::Value<expl_impl<float>>);

template<typename T>
struct impl_constructible_expl_convertible : scalar_ops<impl_constructible_expl_convertible<T>> {
  T value_{};
  impl_constructible_expl_convertible() = default;
  constexpr impl_constructible_expl_convertible(T v) : value_(std::move(v)) {}
  constexpr explicit operator const T&() const& { return value_; }
};

template<typename T>
using impl_expl = impl_constructible_expl_convertible<T>;

static_assert(std::convertible_to<float, impl_expl<float>>);
static_assert(!std::convertible_to<impl_expl<float>, float>);
static_assert(units::Value<impl_expl<float>>);

template<typename T>
struct expl_constructible_expl_convertible : scalar_ops<expl_constructible_expl_convertible<T>> {
  T value_{};
  expl_constructible_expl_convertible() = default;
  constexpr explicit expl_constructible_expl_convertible(T v) : value_(std::move(v)) {}
  constexpr explicit operator const T&() const& { return value_; }
};

template<typename T>
using expl_expl = expl_constructible_expl_convertible<T>;

static_assert(!std::convertible_to<float, expl_expl<float>>);
static_assert(!std::convertible_to<expl_expl<float>, float>);
static_assert(units::Value<expl_expl<float>>);

}  // namespace

namespace units {

template<typename T>
inline constexpr bool treat_as_floating_point<impl<T>> = std::is_floating_point_v<T>;

template<typename T>
inline constexpr bool treat_as_floating_point<expl_constructible<T>> = std::is_floating_point_v<T>;

template<typename T>
inline constexpr bool treat_as_floating_point<impl_impl<T>> = std::is_floating_point_v<T>;

template<typename T>
inline constexpr bool treat_as_floating_point<expl_impl<T>> = std::is_floating_point_v<T>;

template<typename T>
inline constexpr bool treat_as_floating_point<impl_expl<T>> = std::is_floating_point_v<T>;

template<typename T>
inline constexpr bool treat_as_floating_point<expl_expl<T>> = std::is_floating_point_v<T>;

template<typename T>
struct quantity_values<impl<T>> {
  static constexpr impl<T> zero() { return 0; }
  static constexpr impl<T> max() { return std::numeric_limits<T>::max(); }
  static constexpr impl<T> min() { return std::numeric_limits<T>::lowest(); }
};

}  // namespace units

namespace {

using namespace units::physical::si;

// constructors

// Quantity from Value
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

static_assert(length<metre, impl<int>>(length<kilometre, impl<int>>(1)).count() == impl<int>(1000));
static_assert(length<metre, expl<int>>(length<kilometre, expl<int>>(expl<int>(1))).count() == expl<int>(1000));
static_assert(length<metre, impl_impl<int>>(length<kilometre, impl_impl<int>>(1)).count() == impl_impl<int>(1000));
static_assert(length<metre, impl_expl<int>>(length<kilometre, impl_expl<int>>(1)).count() == impl_expl<int>(1000));
static_assert(length<metre, expl_impl<int>>(length<kilometre, expl_impl<int>>(expl_impl<int>(1))).count() == expl_impl<int>(1000));
static_assert(length<metre, expl_expl<int>>(length<kilometre, expl_expl<int>>(expl_expl<int>(1))).count() == expl_expl<int>(1000));

// static_assert(length<kilometre, impl<int>>(length<metre, impl<int>>(2000)).count() == impl<int>(2));  // should not compile (truncating conversion)
static_assert(length<kilometre, impl<int>>(quantity_cast<kilometre>(length<metre, impl<int>>(2000))).count() == impl<int>(2));
// static_assert(length<kilometre, expl<int>>(length<metre, expl<int>>(expl<int>(2000))).count() == expl<int>(2));  // should not compile (truncating conversion)
static_assert(length<kilometre, expl<int>>(quantity_cast<kilometre>(length<metre, expl<int>>(expl<int>(2000)))).count() == expl<int>(2));
// static_assert(length<kilometre, impl_impl<int>>(length<metre, impl_impl<int>>(2000)).count() == impl_impl<int>(2));  // should not compile (truncating conversion)
static_assert(length<kilometre, impl_impl<int>>(quantity_cast<kilometre>(length<metre, impl_impl<int>>(2000))).count() == impl_impl<int>(2));
// static_assert(length<kilometre, impl_expl<int>>(length<metre, impl_expl<int>>(2000)).count() == impl_expl<int>(2));  // should not compile (truncating conversion)
static_assert(length<kilometre, impl_expl<int>>(quantity_cast<kilometre>(length<metre, impl_expl<int>>(2000))).count() == impl_expl<int>(2));
// static_assert(length<kilometre, expl_impl<int>>(length<metre, expl_impl<int>>(expl_impl<int>(2000))).count() == expl_impl<int>(2));  // should not compile (truncating conversion)
static_assert(length<kilometre, expl_impl<int>>(quantity_cast<kilometre>(length<metre, expl_impl<int>>(expl_impl<int>(2000)))).count() == expl_impl<int>(2));
// static_assert(length<kilometre, expl_expl<int>>(length<metre, expl_expl<int>>(expl_expl<int>(2000))).count() == expl_expl<int>(2));  // should not compile (truncating conversion)
static_assert(length<kilometre, expl_expl<int>>(quantity_cast<kilometre>(length<metre, expl_expl<int>>(expl_expl<int>(2000)))).count() == expl_expl<int>(2));

// static_assert(speed<metre_per_second, impl<int>>(speed<kilometre_per_hour, impl<int>>(72)).count() == impl<int>(20));  // should not compile (truncating conversion)
static_assert(speed<metre_per_second, impl<int>>(quantity_cast<metre_per_second>(speed<kilometre_per_hour, impl<int>>(72))).count() == impl<int>(20));
// static_assert(speed<metre_per_second, expl<int>>(speed<kilometre_per_hour, expl<int>>(expl(72))).count() == expl<int>(20));  // should not compile (truncating conversion)
static_assert(speed<metre_per_second, expl<int>>(quantity_cast<metre_per_second>(speed<kilometre_per_hour, expl<int>>(expl<int>(72)))).count() == expl<int>(20));
// static_assert(speed<metre_per_second, impl_impl<int>>(speed<kilometre_per_hour, impl_impl<int>>(72)).count() == impl_impl<int>(20));  // should not compile (truncating conversion)
static_assert(speed<metre_per_second, impl_impl<int>>(quantity_cast<metre_per_second>(speed<kilometre_per_hour, impl_impl<int>>(72))).count() == impl_impl<int>(20));
// static_assert(speed<metre_per_second, impl_expl<int>>(speed<kilometre_per_hour, impl_expl<int>>(72)).count() == impl_expl<int>(20));  // should not compile (truncating conversion)
static_assert(speed<metre_per_second, impl_expl<int>>(quantity_cast<metre_per_second>(speed<kilometre_per_hour, impl_expl<int>>(72))).count() == impl_expl<int>(20));
// static_assert(speed<metre_per_second, expl_impl<int>>(speed<kilometre_per_hour, expl_impl<int>>(expl_impl(72))).count() == expl_impl<int>(20));  // should not compile (truncating conversion)
static_assert(speed<metre_per_second, expl_impl<int>>(quantity_cast<metre_per_second>(speed<kilometre_per_hour, expl_impl<int>>(expl_impl<int>(72)))).count() == expl_impl<int>(20));
// static_assert(speed<metre_per_second, expl_expl<int>>(speed<kilometre_per_hour, expl_expl<int>>(expl_expl(72))).count() == expl_expl<int>(20));  // should not compile (truncating conversion)
static_assert(speed<metre_per_second, expl_expl<int>>(quantity_cast<metre_per_second>(speed<kilometre_per_hour, expl_expl<int>>(expl_expl<int>(72)))).count() == expl_expl<int>(20));

// static_assert(speed<kilometre_per_hour, impl<int>>(speed<metre_per_second, impl<int>>(20)).count() == impl<int>(72));  // should not compile (truncating conversion)
static_assert(speed<kilometre_per_hour, impl<int>>(quantity_cast<kilometre_per_hour>(speed<metre_per_second, impl<int>>(20))).count() == impl<int>(72));
// static_assert(speed<kilometre_per_hour, expl<int>>(speed<metre_per_second, expl<int>>(expl<int>(20))).count() == expl<int>(72));  // should not compile (truncating conversion)
static_assert(speed<kilometre_per_hour, expl<int>>(quantity_cast<kilometre_per_hour>(speed<metre_per_second, expl<int>>(expl<int>(20)))).count() == expl<int>(72));
// static_assert(speed<kilometre_per_hour, impl_impl<int>>(speed<metre_per_second, impl_impl<int>>(20)).count() == impl_impl<int>(72));  // should not compile (truncating conversion)
static_assert(speed<kilometre_per_hour, impl_impl<int>>(quantity_cast<kilometre_per_hour>(speed<metre_per_second, impl_impl<int>>(20))).count() == impl_impl<int>(72));
// static_assert(speed<kilometre_per_hour, impl_expl<int>>(speed<metre_per_second, impl_expl<int>>(20)).count() == impl_expl<int>(72));  // should not compile (truncating conversion)
static_assert(speed<kilometre_per_hour, impl_expl<int>>(quantity_cast<kilometre_per_hour>(speed<metre_per_second, impl_expl<int>>(20))).count() == impl_expl<int>(72));
// static_assert(speed<kilometre_per_hour, expl_impl<int>>(speed<metre_per_second, expl_impl<int>>(expl_impl<int>(20))).count() == expl_impl<int>(72));  // should not compile (truncating conversion)
static_assert(speed<kilometre_per_hour, expl_impl<int>>(quantity_cast<kilometre_per_hour>(speed<metre_per_second, expl_impl<int>>(expl_impl<int>(20)))).count() == expl_impl<int>(72));
// static_assert(speed<kilometre_per_hour, expl_expl<int>>(speed<metre_per_second, expl_expl<int>>(expl_expl<int>(20))).count() == expl_expl<int>(72));  // should not compile (truncating conversion)
static_assert(speed<kilometre_per_hour, expl_expl<int>>(quantity_cast<kilometre_per_hour>(speed<metre_per_second, expl_expl<int>>(expl_expl<int>(20)))).count() == expl_expl<int>(72));

}  // namespace
