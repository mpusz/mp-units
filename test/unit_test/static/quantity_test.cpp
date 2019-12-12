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
// constexpr quantity<si::dim_length, scaled_unit<metre, ratio<-1, 1>>, int> error(0);  // should not compile (negative unit ratio)

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
static_assert(length<metre, int>(quantity_cast<length<metre, my_int>>(3.14m)).count() == 3);
// static_assert(length<metre, int>(length<metre, my_double>(1000.0)).count() == 1000);   // should not compile (truncating conversion)
// static_assert(length<metre, my_int>(1000.0m).count() == my_int{1000});   // should not compile (truncating conversion)
static_assert(length<metre, double>(1000.0m).count() == 1000.0);
static_assert(length<metre, double>(length<metre, my_double>(1000.0)).count() == 1000.0);
static_assert(length<metre, my_double>(1000.0m).count() == my_double{1000.0});
static_assert(length<metre, double>(km).count() == 1000.0);
static_assert(length<metre, my_double>(km).count() == my_double{1000.0});
static_assert(length<metre, int>(1km).count() == 1000);
// static_assert(length<metre, int>(1s).count() == 1);   // should not compile (different dimensions)
//static_assert(length<kilometre, int>(1010m).count() == 1);   // should not compile (truncating conversion)
static_assert(length<kilometre, int>(quantity_cast<length<kilometre, my_int>>(1010m)).count() == 1);

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

static_assert((1m += 1m).count() == 2);
static_assert((2m -= 1m).count() == 1);
static_assert((1m *= 2).count() == 2);
static_assert((2m /= 2).count() == 1);
static_assert((7m %= 2).count() == 1);
static_assert((7m %= 2m).count() == 1);
//  static_assert((7.m %= 2.).count() == 1);  // should not compile (operation not allowed for floating-point types)
//  static_assert((7.m %= 2).count() == 1);  // should not compile (operation not allowed for floating-point types)
//  static_assert((7m %= 2.).count() == 1);  // should not compile (operation not allowed for floating-point types)
static_assert((7m %= 2m).count() == 1);
//  static_assert((7.m %= 2.m).count() == 1);  // should not compile (operation not allowed for floating-point types)
//  static_assert((7.m %= 2m).count() == 1);  // should not compile (operation not allowed for floating-point types)
//  static_assert((7m %= 2.m).count() == 1);  // should not compile (operation not allowed for floating-point types)

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
    std::is_same_v<decltype(velocity<metre_per_second, int>() * si::time<hour, int>()), length<scaled_unit<metre, ratio<3600>>, int>>);
// TODO uncomment below when fixed in gcc
// static_assert(std::is_same_v<decltype(length<metre>() * si::time<minute>()),
//                              quantity<derived_dimension<exp<dim_length, 1>, exp<dim_time, 1>>, scaled_unit<unknown_unit, ratio<60>>>>);
static_assert(std::is_same_v<decltype(1 / si::time<second, int>()), frequency<hertz, int>>);
static_assert(std::is_same_v<decltype(1 / si::time<minute, int>()), frequency<scaled_unit<hertz, ratio<1, 60>>, int>>);
static_assert(std::is_same_v<decltype(1 / frequency<hertz, int>()), si::time<second, int>>);
// TODO uncomment below when fixed in gcc
// static_assert(std::is_same_v<decltype(1 / length<kilometre>()),
//                              quantity<derived_dimension<exp<dim_length, -1>>, scaled_unit<unknown_unit, ratio<1, 1000>>>>);
static_assert(std::is_same_v<decltype(length<metre, int>() / 1.0), length<metre, double>>);
static_assert(std::is_same_v<decltype(length<metre, int>() / length<metre, double>()), double>);
static_assert(std::is_same_v<decltype(length<kilometre, int>() / length<metre, double>()), double>);
static_assert(
    std::is_same_v<decltype(length<metre, int>() / si::time<second, int>()), velocity<metre_per_second, int>>);
static_assert(
    std::is_same_v<decltype(length<metre>() / si::time<minute>()), velocity<scaled_unit<metre_per_second, ratio<1, 60>>>>);
// TODO uncomment below when fixed in gcc
// static_assert(std::is_same_v<decltype(si::time<minute>() / length<metre>()),
//                              quantity<derived_dimension<exp<dim_length, -1>, exp<dim_time, 1>>, scaled_unit<unknown_unit, ratio<60>>>>);
static_assert(std::is_same_v<decltype(length<metre, int>() % short(1)), length<metre, int>>);
static_assert(std::is_same_v<decltype(length<metre, int>() % length<metre, short>(1)), length<metre, int>>);

static_assert((1m + km).count() == 1001);
static_assert((1m + 1km).count() == 1001);
static_assert((km - 1m).count() == 999);
static_assert((1km - 1m).count() == 999);
static_assert((2m * 2).count() == 4);
static_assert((3 * 3m).count() == 9);
static_assert((4m / 2).count() == 2);
static_assert(4m / 2m == 2);
static_assert(4km / 2000m == 2);
static_assert((7m % 2).count() == 1);
static_assert((7m % 2m).count() == 1);
static_assert((7km % 2000m).count() == 1000);

static_assert((10sq_km * 10sq_km) / 50sq_km == 2sq_km);

// comparators

static_assert(2m + 1m == 3m);
static_assert(!(2m + 2m == 3m));
static_assert(2m + 2m != 3m);
static_assert(!(2m + 2m != 4m));
static_assert(2m > 1m);
static_assert(!(1m > 1m));
static_assert(1m < 2m);
static_assert(!(2m < 2m));
static_assert(2m >= 1m);
static_assert(2m >= 2m);
static_assert(!(2m >= 3m));
static_assert(1m <= 2m);
static_assert(2m <= 2m);
static_assert(!(3m <= 2m));

static_assert(3m == 3.0m);
static_assert(3m != 3.14m);
static_assert(2m > 1.0m);
static_assert(1.0m < 2m);
static_assert(2.0m >= 1m);
static_assert(1m <= 2.0m);

static_assert(1000m == 1km);
static_assert(1001m != 1km);
static_assert(1001m > 1km);
static_assert(999m < 1km);
static_assert(1000m >= 1km);
static_assert(1000m <= 1km);

// is_quantity

static_assert(Quantity<length<millimetre, int>>);

// common_quantity

static_assert(std::is_same_v<common_quantity<length<metre, int>, length<kilometre, int>>, length<metre, int>>);
static_assert(
    std::is_same_v<common_quantity<length<kilometre, long long>, length<metre, int>>, length<metre, long long>>);
static_assert(std::is_same_v<common_quantity<length<kilometre, long long>, length<millimetre, double>>,
                             length<millimetre, double>>);

// quantity_cast

static_assert(std::is_same_v<decltype(quantity_cast<scaled_unit<metre, ratio<1>>>(2km))::unit, metre>);

static_assert(quantity_cast<length<metre, int>>(2km).count() == 2000);
static_assert(quantity_cast<length<kilometre, int>>(2000m).count() == 2);
static_assert(quantity_cast<length<metre, int>>(1.23m).count() == 1);
static_assert(quantity_cast<metre>(2km).count() == 2000);
static_assert(quantity_cast<kilometre>(2000m).count() == 2);
static_assert(quantity_cast<int>(1.23m).count() == 1);

// time

// static_assert(1s == 1m);  // should not compile (different dimensions)
static_assert(1h == 3600s);

// length

static_assert(1km == 1000m);
static_assert(1km + 1m == 1001m);
static_assert(10km / 5km == 2);
static_assert(10km / 2 == 5km);

// velocity

static_assert(10m / 5s == 2mps);
static_assert(10 / 5s * 1m == 2mps);
static_assert(1km / 1s == 1000mps);
static_assert(2kmph * 2h == 4km);
static_assert(2km / 2kmph == 1h);

static_assert(std::is_same_v<decltype(pow<2>(2m)), decltype(4sq_m)>);

}  // namespace
