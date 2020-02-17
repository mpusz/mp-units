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
class my_value {
  T value_{};

public:
  my_value() = default;
  constexpr my_value(T v) : value_(std::move(v)) {}

  // constexpr my_value& operator+=(my_value other) { value_ += other.value_; return *this; }
  // constexpr my_value& operator-=(my_value other) { value_ -= other.value_; return *this; }
  // constexpr my_value& operator*=(my_value other) { value_ *= other.value_; return *this; }
  // constexpr my_value& operator/=(my_value other) { value_ /= other.value_; return *this; }

  [[nodiscard]] constexpr my_value operator-() const { return my_value(-value_); }

  [[nodiscard]] friend constexpr my_value operator+(my_value lhs, my_value rhs) {
    return my_value(lhs.value_ + rhs.value_);
  }
  [[nodiscard]] friend constexpr my_value operator-(my_value lhs, my_value rhs) {
    return my_value(lhs.value_ - rhs.value_);
  }
  [[nodiscard]] friend constexpr my_value operator*(my_value lhs, my_value rhs) {
    return my_value(lhs.value_ * rhs.value_);
  }
  [[nodiscard]] friend constexpr my_value operator/(my_value lhs, my_value rhs) {
    return my_value(lhs.value_ / rhs.value_);
  }

  [[nodiscard]] friend constexpr bool operator==(my_value lhs, my_value rhs) { return lhs.value_ == rhs.value_; }
  [[nodiscard]] friend constexpr bool operator!=(my_value lhs, my_value rhs) { return !(lhs == rhs); }
  [[nodiscard]] friend constexpr bool operator<(my_value lhs, my_value rhs) { return lhs.value_ < rhs.value_; }
  [[nodiscard]] friend constexpr bool operator>(my_value lhs, my_value rhs) { return rhs < lhs; }
  [[nodiscard]] friend constexpr bool operator<=(my_value lhs, my_value rhs) { return !(rhs < lhs); }
  [[nodiscard]] friend constexpr bool operator>=(my_value lhs, my_value rhs) { return !(lhs < rhs); }

  constexpr operator const T&() const& { return value_; }
};

}  // namespace

namespace units {

template<typename T>
inline constexpr bool treat_as_floating_point<my_value<T>> = std::is_floating_point_v<T>;

template<typename T>
struct quantity_values<my_value<T>> {
  static constexpr my_value<T> zero() { return my_value<T>(0); }
  static constexpr my_value<T> max() { return std::numeric_limits<T>::max(); }
  static constexpr my_value<T> min() { return std::numeric_limits<T>::lowest(); }
};

}  // namespace units

namespace std {

template<typename T, typename U>
struct common_type<my_value<T>, my_value<U>> : std::type_identity<my_value<common_type_t<T, U>>> {};

template<typename T, typename U>
struct common_type<my_value<T>, U> : common_type<T, U> {};

template<typename T, typename U>
struct common_type<T, my_value<U>> : common_type<T, U> {};

}  // namespace std

namespace {

static_assert(units::Scalar<my_value<float>>);
static_assert(std::convertible_to<my_value<float>, float>);
static_assert(std::convertible_to<float, my_value<float>>);

using namespace units;
using namespace units::si;

// class invariants

// constexpr quantity<si::dim_length, second, int> error(0);  // should not compile (unit of a different dimension)
// constexpr quantity<si::dim_length, metre, quantity<si::dim_length, metre, int>> error(0);  // should not compile (quantity used as Rep)
// constexpr quantity<metre, si::dim_length, double> error(0);  // should not compile (reordered arguments)
// constexpr quantity<si::dim_length, scaled_unit<ratio<-1, 1>, metre>, int> error(0);  // should not compile (negative unit ratio)

// member types

static_assert(std::is_same_v<length<metre, int>::rep, int>);
static_assert(std::is_same_v<length<metre, double>::rep, double>);
static_assert(std::is_same_v<length<metre, int>::unit, metre>);
static_assert(std::is_same_v<length<kilometre, int>::unit, kilometre>);

// constructors

using my_int = my_value<int>;
using my_double = my_value<double>;

static_assert(length<metre, int>().count() == 0);
constexpr length<metre, int> km{1000};
static_assert(km.count() == 1000);
static_assert(length<metre, int>(km).count() == km.count());

static_assert(length<metre, int>(1).count() == 1);
static_assert(length<metre, int>(my_value(1)).count() == 1);
static_assert(length<metre, my_int>(1).count() == my_int{1});
// static_assert(length<metre, int>(1.0).count() == 1);   // should not compile (truncating conversion)
// static_assert(length<metre, int>(my_value(1.0)).count() == 1); // should not compile (truncating conversion)
// static_assert(length<metre, my_int>(1.0).count() == my_int{1});   // should not compile (truncating conversion)
static_assert(length<metre, double>(1.0).count() == 1.0);
static_assert(length<metre, double>(my_value(1.0)).count() == 1.0);
static_assert(length<metre, double>(1).count() == 1.0);
static_assert(length<metre, double>(my_value(1)).count() == 1.0);
static_assert(length<metre, double>(3.14).count() == 3.14);
static_assert(length<metre, my_double>(1.0).count() == my_double{1.0});
static_assert(length<metre, my_double>(1).count() == my_double{1.0});
static_assert(length<metre, my_double>(3.14).count() == my_double{3.14});

static_assert(length<metre, int>(km).count() == 1000);
// static_assert(length<metre, int>(length<metre, double>(3.14)).count() == 3);   // should not compile (truncating conversion)
static_assert(length<metre, int>(quantity_cast<length<metre, my_int>>(3.14q_m)).count() == 3);
// static_assert(length<metre, int>(length<metre, my_double>(1000.0)).count() == 1000);   // should not compile (truncating conversion)
// static_assert(length<metre, my_int>(1000.0q_m).count() == my_int{1000});   // should not compile (truncating conversion)
static_assert(length<metre, double>(1000.0q_m).count() == 1000.0);
static_assert(length<metre, double>(length<metre, my_double>(1000.0)).count() == 1000.0);
static_assert(length<metre, my_double>(1000.0q_m).count() == my_double{1000.0});
static_assert(length<metre, double>(km).count() == 1000.0);
static_assert(length<metre, my_double>(km).count() == my_double{1000.0});
static_assert(length<metre, int>(1q_km).count() == 1000);
// static_assert(length<metre, int>(1q_s).count() == 1);   // should not compile (different dimensions)
//static_assert(length<kilometre, int>(1010q_m).count() == 1);   // should not compile (truncating conversion)
static_assert(length<kilometre, int>(quantity_cast<length<kilometre, my_int>>(1010q_m)).count() == 1);
static_assert(length<metre, int>(quantity_cast<length<kilometre, my_int>>(1010q_m)).count() == 1000);

// assignment operator

static_assert([]() { length<metre, int> l1(1), l2(2); return l2 = l1; }().count() == 1);

// static member functions

static_assert(length<metre, int>::zero().count() == 0);
static_assert(length<metre, int>::min().count() == std::numeric_limits<int>::lowest());
static_assert(length<metre, int>::max().count() == std::numeric_limits<int>::max());
static_assert(length<metre, double>::zero().count() == 0.0);
static_assert(length<metre, double>::min().count() == std::numeric_limits<double>::lowest());
static_assert(length<metre, double>::max().count() == std::numeric_limits<double>::max());
static_assert(length<metre, my_int>::zero().count() == my_int{0});
static_assert(length<metre, my_int>::min().count() == my_int{std::numeric_limits<int>::lowest()});
static_assert(length<metre, my_int>::max().count() == my_int{std::numeric_limits<int>::max()});
static_assert(length<metre, my_double>::zero().count() == my_double{0.0});
static_assert(length<metre, my_double>::min().count() == my_double{std::numeric_limits<double>::lowest()});
static_assert(length<metre, my_double>::max().count() == my_double{std::numeric_limits<double>::max()});

// unary member operators

static_assert((+km).count() == 1000);
static_assert((-km).count() == -1000);
static_assert((+(-km)).count() == -1000);
static_assert((-(-km)).count() == 1000);

// binary member operators

static_assert([](auto v) {
  auto vv = v++;
  return std::make_pair(v, vv);
}(km) == std::make_pair(length<metre, int>(1001), length<metre, int>(1000)));
static_assert([](auto v) {
  auto vv = ++v;
  return std::make_pair(v, vv);
}(km) == std::make_pair(length<metre, int>(1001), length<metre, int>(1001)));
static_assert([](auto v) {
  auto vv = v--;
  return std::make_pair(v, vv);
}(km) == std::make_pair(length<metre, int>(999), length<metre, int>(1000)));
static_assert([](auto v) {
  auto vv = --v;
  return std::make_pair(v, vv);
}(km) == std::make_pair(length<metre, int>(999), length<metre, int>(999)));

// compound assignment

static_assert((1q_m += 1q_m).count() == 2);
static_assert((2q_m -= 1q_m).count() == 1);
static_assert((1q_m *= 2).count() == 2);
static_assert((2q_m /= 2).count() == 1);
static_assert((7q_m %= 2).count() == 1);
static_assert((7q_m %= 2q_m).count() == 1);
//  static_assert((7.m %= 2.).count() == 1);  // should not compile (operation not allowed for floating-point types)
//  static_assert((7.m %= 2).count() == 1);  // should not compile (operation not allowed for floating-point types)
//  static_assert((7q_m %= 2.).count() == 1);  // should not compile (operation not allowed for floating-point types)
static_assert((7q_m %= 2q_m).count() == 1);
//  static_assert((7.m %= 2.m).count() == 1);  // should not compile (operation not allowed for floating-point types)
//  static_assert((7.m %= 2q_m).count() == 1);  // should not compile (operation not allowed for floating-point types)
//  static_assert((7q_m %= 2.m).count() == 1);  // should not compile (operation not allowed for floating-point types)

// non-member arithmetic operators

static_assert(std::is_same_v<decltype(length<metre, int>() + length<metre, double>()), length<metre, double>>);
static_assert(std::is_same_v<decltype(length<metre, int>() + length<metre, double>()), length<metre, double>>);
static_assert(
    std::is_same_v<decltype(length<kilometre, int>() + length<metre, double>()), length<metre, double>>);
static_assert(std::is_same_v<decltype(length<metre, double>() - length<metre, int>()), length<metre, double>>);
static_assert(
    std::is_same_v<decltype(length<kilometre, double>() - length<metre, int>()), length<metre, double>>);
static_assert(std::is_same_v<decltype(length<metre, int>() * 1.0), length<metre, double>>);
static_assert(std::is_same_v<decltype(1.0 * length<metre, int>()), length<metre, double>>);
static_assert(
    std::is_same_v<decltype(velocity<metre_per_second, int>() * si::time<second, int>()), length<metre, int>>);
static_assert(
    std::is_same_v<decltype(velocity<metre_per_second, int>() * si::time<hour, int>()), length<scaled_unit<ratio<36, 1, 2>, metre>, int>>);
static_assert(std::is_same_v<decltype(length<metre>() * si::time<minute>()),
              quantity<unknown_dimension<units::exp<dim_length, 1>, units::exp<dim_time, 1>>, scaled_unit<ratio<6, 1, 1>, unknown_unit>>>);
static_assert(std::is_same_v<decltype(1 / si::time<second, int>()), frequency<hertz, int>>);
static_assert(std::is_same_v<decltype(1 / si::time<minute, int>()), frequency<scaled_unit<ratio<1, 6, -1>, hertz>, int>>);
static_assert(std::is_same_v<decltype(1 / frequency<hertz, int>()), si::time<second, int>>);
static_assert(std::is_same_v<decltype(1 / length<kilometre>()),
              quantity<unknown_dimension<units::exp<dim_length, -1>>, scaled_unit<ratio<1, 1, -3>, unknown_unit>>>);
static_assert(std::is_same_v<decltype(length<metre, int>() / 1.0), length<metre, double>>);
static_assert(std::is_same_v<decltype(length<metre, int>() / length<metre, double>()), double>);
static_assert(std::is_same_v<decltype(length<kilometre, int>() / length<metre, double>()), double>);
static_assert(
    std::is_same_v<decltype(length<metre, int>() / si::time<second, int>()), velocity<metre_per_second, int>>);
static_assert(
    std::is_same_v<decltype(length<metre>() / si::time<minute>()), velocity<scaled_unit<ratio<1, 6, -1>, metre_per_second>>>);
static_assert(std::is_same_v<decltype(si::time<minute>() / length<metre>()),
              quantity<unknown_dimension<units::exp<dim_length, -1>, units::exp<dim_time, 1>>, scaled_unit<ratio<6 ,1 , 1>, unknown_unit>>>);
static_assert(std::is_same_v<decltype(length<metre, int>() % short(1)), length<metre, int>>);
static_assert(std::is_same_v<decltype(length<metre, int>() % length<metre, short>(1)), length<metre, int>>);

static_assert((1q_m + km).count() == 1001);
static_assert((1q_m + 1q_km).count() == 1001);
static_assert((km - 1q_m).count() == 999);
static_assert((1q_km - 1q_m).count() == 999);
static_assert((2q_m * 2).count() == 4);
static_assert((3 * 3q_m).count() == 9);
static_assert((4q_m / 2).count() == 2);
static_assert(4q_m / 2q_m == 2);
static_assert(4q_km / 2000q_m == 2);
static_assert((7q_m % 2).count() == 1);
static_assert((7q_m % 2q_m).count() == 1);
static_assert((7q_km % 2000q_m).count() == 1000);

static_assert((10q_km2 * 10q_km2) / 50q_km2 == 2q_km2);

// comparators

static_assert(2q_m + 1q_m == 3q_m);
static_assert(!(2q_m + 2q_m == 3q_m));
static_assert(2q_m + 2q_m != 3q_m);
static_assert(!(2q_m + 2q_m != 4q_m));
static_assert(2q_m > 1q_m);
static_assert(!(1q_m > 1q_m));
static_assert(1q_m < 2q_m);
static_assert(!(2q_m < 2q_m));
static_assert(2q_m >= 1q_m);
static_assert(2q_m >= 2q_m);
static_assert(!(2q_m >= 3q_m));
static_assert(1q_m <= 2q_m);
static_assert(2q_m <= 2q_m);
static_assert(!(3q_m <= 2q_m));

static_assert(3q_m == 3.0q_m);
static_assert(3q_m != 3.14q_m);
static_assert(2q_m > 1.0q_m);
static_assert(1.0q_m < 2q_m);
static_assert(2.0q_m >= 1q_m);
static_assert(1q_m <= 2.0q_m);

static_assert(1000q_m == 1q_km);
static_assert(1001q_m != 1q_km);
static_assert(1001q_m > 1q_km);
static_assert(999q_m < 1q_km);
static_assert(1000q_m >= 1q_km);
static_assert(1000q_m <= 1q_km);

// is_quantity

static_assert(Quantity<length<millimetre, int>>);

// common_quantity

static_assert(std::is_same_v<common_quantity<length<metre, int>, length<kilometre, int>>, length<metre, int>>);
static_assert(
    std::is_same_v<common_quantity<length<kilometre, long long>, length<metre, int>>, length<metre, long long>>);
static_assert(std::is_same_v<common_quantity<length<kilometre, long long>, length<millimetre, double>>,
                             length<millimetre, double>>);

// quantity_cast

static_assert(std::is_same_v<decltype(quantity_cast<scaled_unit<ratio<1>, metre>>(2q_km))::unit, metre>);

static_assert(quantity_cast<length<metre, int>>(2q_km).count() == 2000);
static_assert(quantity_cast<length<kilometre, int>>(2000q_m).count() == 2);
static_assert(quantity_cast<length<metre, int>>(1.23q_m).count() == 1);
static_assert(quantity_cast<metre>(2q_km).count() == 2000);
static_assert(quantity_cast<kilometre>(2000q_m).count() == 2);
static_assert(quantity_cast<int>(1.23q_m).count() == 1);

// time

// static_assert(1q_s == 1q_m);  // should not compile (different dimensions)
static_assert(1q_h == 3600q_s);

// length

static_assert(1q_km == 1000q_m);
static_assert(1q_km + 1q_m == 1001q_m);
static_assert(10q_km / 5q_km == 2);
static_assert(10q_km / 2 == 5q_km);

// velocity

static_assert(10q_m / 5q_s == 2q_mps);
static_assert(10 / 5q_s * 1q_m == 2q_mps);
static_assert(1q_km / 1q_s == 1000q_mps);
static_assert(2q_kmph * 2q_h == 4q_km);
static_assert(2q_km / 2q_kmph == 1q_h);

static_assert(std::is_same_v<decltype(pow<2>(2q_m)), decltype(4q_m2)>);

}  // namespace
