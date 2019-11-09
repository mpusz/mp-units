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

#include "units/dimensions/velocity.h"
#include "units/dimensions/frequency.h"
#include "units/dimensions/area.h"
#include "units/math.h"
#include <utility>
#include <chrono>

using namespace units;

namespace {

  template<typename T>
  class my_value {
    T value_{};
  public:
    my_value() = default;
    constexpr my_value(T v) : value_(std::move(v)) {}
    constexpr my_value& operator+=(const my_value& other) { value_ += other.value_; return *this; }
    constexpr my_value& operator-=(const my_value& other) { value_ -= other.value_; return *this; }
    constexpr my_value& operator*=(const my_value& other) { value_ *= other.value_; return *this; }
    constexpr my_value& operator/=(const my_value& other) { value_ /= other.value_; return *this; }
    constexpr operator const T&() const & { return value_; }
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
  struct common_type<my_value<T>, my_value<U>> : std::type_identity<my_value<common_type_t<T, U>>> {
  };

  template<typename T, typename U>
  struct common_type<my_value<T>, U> : common_type<T, U> {
  };

  template<typename T, typename U>
  struct common_type<T, my_value<U>> : common_type<T, U> {
  };

}  // namespace std

namespace {

  using namespace units;

  // class invariants

  // constexpr quantity<length, second, int> q;  // should a static_assert
  // constexpr quantity<length, metre, quantity<metre, int>> error(0m);  // should trigger a static_assert
  // constexpr quantity<int, int, double> error(0);  // should trigger a static_assert
  // constexpr quantity<length, unit<length, std::ratio<-1, 1>>, int> error(0);  // should trigger a static_assert

  // member types

  static_assert(std::is_same_v<quantity<metre, int>::rep, int>);
  static_assert(std::is_same_v<quantity<metre, double>::rep, double>);
  static_assert(std::is_same_v<quantity<metre, int>::unit, metre>);
  static_assert(std::is_same_v<quantity<kilometre, int>::unit, kilometre>);

  // constructors

  static_assert(quantity<metre, int>().count() == 0);
  constexpr quantity<metre, int> km{1000};
  static_assert(km.count() == 1000);
  static_assert(quantity<metre, int>(km).count() == km.count());

  static_assert(quantity<metre, int>(1).count() == 1);
  static_assert(quantity<metre, int>(my_value(1)).count() == 1);
  static_assert(quantity<metre, my_value<int>>(1).count() == 1);
  //  static_assert(quantity<metre, int>(1.0).count() == 1);   // should not compile
  //  static_assert(quantity<metre, int>(my_value(1.0)).count() == 1); // should not compile
  //  static_assert(quantity<metre, my_value>(1.0).count() == 1);   // should not compile
  static_assert(quantity<metre, double>(1.0).count() == 1.0);
  static_assert(quantity<metre, double>(my_value(1.0)).count() == 1.0);
  static_assert(quantity<metre, double>(1).count() == 1.0);
  static_assert(quantity<metre, double>(my_value(1)).count() == 1.0);
  static_assert(quantity<metre, double>(3.14).count() == 3.14);
  static_assert(quantity<metre, my_value<double>>(1.0).count() == 1.0);
  static_assert(quantity<metre, my_value<double>>(1).count() == 1.0);
  static_assert(quantity<metre, my_value<double>>(3.14).count() == 3.14);

  static_assert(quantity<metre, int>(km).count() == 1000);
  //  static_assert(quantity<metre, int>(quantity<metre, double>(3.14)).count() == 3);   // should not compile
  static_assert(quantity<metre, int>(quantity_cast<quantity<metre, my_value<int>>>(3.14m)).count() == 3);
  //  static_assert(quantity<metre, int>(quantity<metre, my_value<double>>(1000.0)).count() == 1000);   // should not compile
  //  static_assert(quantity<metre, my_value>(1000.0m).count() == 1000);   // should not compile
  static_assert(quantity<metre, double>(1000.0m).count() == 1000.0);
  static_assert(quantity<metre, double>(quantity<metre, my_value<double>>(1000.0)).count() == 1000.0);
  static_assert(quantity<metre, my_value<double>>(1000.0m).count() == 1000.0);
  static_assert(quantity<metre, double>(km).count() == 1000.0);
  static_assert(quantity<metre, my_value<double>>(km).count() == 1000.0);
  static_assert(quantity<metre, int>(1km).count() == 1000);
  //  static_assert(quantity<metre, int>(1_s).count() == 1);   // should not compile
  //  static_assert(quantity<kilometre, int>(1010m).count() == 1);   // should not compile
  static_assert(quantity<kilometre, int>(quantity_cast<quantity<kilometre, my_value<int>>>(1010m)).count() == 1);

  // assignment operator

  static_assert([]() {
    quantity<metre, int> l1(1), l2(2);
    return l2 = l1;
  }().count() == 1);

  // static member functions

  static_assert(quantity<metre, int>::zero().count() == 0);
  static_assert(quantity<metre, int>::min().count() == std::numeric_limits<int>::lowest());
  static_assert(quantity<metre, int>::max().count() == std::numeric_limits<int>::max());
  static_assert(quantity<metre, double>::zero().count() == 0.0);
  static_assert(quantity<metre, double>::min().count() == std::numeric_limits<double>::lowest());
  static_assert(quantity<metre, double>::max().count() == std::numeric_limits<double>::max());
  static_assert(quantity<metre, my_value<int>>::zero().count() == 0);
  static_assert(quantity<metre, my_value<int>>::min().count() == std::numeric_limits<int>::lowest());
  static_assert(quantity<metre, my_value<int>>::max().count() == std::numeric_limits<int>::max());
  static_assert(quantity<metre, my_value<double>>::zero().count() == 0.0);
  static_assert(quantity<metre, my_value<double>>::min().count() == std::numeric_limits<double>::lowest());
  static_assert(quantity<metre, my_value<double>>::max().count() == std::numeric_limits<double>::max());

  // unary member operators

  static_assert((+km).count() == 1000);
  static_assert((-km).count() == -1000);
  static_assert((+(-km)).count() == -1000);
  static_assert((-(-km)).count() == 1000);

  // binary member operators

  static_assert([](auto v) {
    auto vv = v++;
    return std::make_pair(v, vv);
  }(km) == std::make_pair(quantity<metre, int>(1001), quantity<metre, int>(1000)));
  static_assert([](auto v) {
    auto vv = ++v;
    return std::make_pair(v, vv);
  }(km) == std::make_pair(quantity<metre, int>(1001), quantity<metre, int>(1001)));
  static_assert([](auto v) {
    auto vv = v--;
    return std::make_pair(v, vv);
  }(km) == std::make_pair(quantity<metre, int>(999), quantity<metre, int>(1000)));
  static_assert([](auto v) {
    auto vv = --v;
    return std::make_pair(v, vv);
  }(km) == std::make_pair(quantity<metre, int>(999), quantity<metre, int>(999)));

  // compound assignment

  static_assert((1m += 1m).count() == 2);
  static_assert((2m -= 1m).count() == 1);
  static_assert((1m *= 2).count() == 2);
  static_assert((2m /= 2).count() == 1);
  static_assert((7m %= 2).count() == 1);
  static_assert((7m %= 2m).count() == 1);
//  static_assert((7.m %= 2.).count() == 1);  // should not compile
//  static_assert((7.m %= 2).count() == 1);  // should not compile
//  static_assert((7m %= 2.).count() == 1);  // should not compile
  static_assert((7m %= 2m).count() == 1);
//  static_assert((7.m %= 2.m).count() == 1);  // should not compile
//  static_assert((7.m %= 2m).count() == 1);  // should not compile
//  static_assert((7m %= 2.m).count() == 1);  // should not compile

  // non-member arithmetic operators

  static_assert(std::is_same_v<decltype(quantity<metre, int>() + quantity<metre, double>()), quantity<metre, double>>);
  static_assert(std::is_same_v<decltype(quantity<metre, int>() + quantity<metre, double>()), quantity<metre, double>>);
  static_assert(std::is_same_v<decltype(quantity<kilometre, int>() + quantity<metre, double>()), quantity<metre, double>>);
  static_assert(std::is_same_v<decltype(quantity<metre, double>() - quantity<metre, int>()), quantity<metre, double>>);
  static_assert(std::is_same_v<decltype(quantity<kilometre, double>() - quantity<metre, int>()), quantity<metre, double>>);
  static_assert(std::is_same_v<decltype(quantity<metre, int>() * 1.0), quantity<metre, double>>);
  static_assert(std::is_same_v<decltype(1.0 * quantity<metre, int>()), quantity<metre, double>>);
  static_assert(std::is_same_v<decltype(quantity<metre_per_second, int>() * quantity<second, int>()), quantity<metre, int>>);
  static_assert(std::is_same_v<decltype(1 / quantity<second, int>()), quantity<hertz, int>>);
  static_assert(std::is_same_v<decltype(quantity<metre, int>() / 1.0), quantity<metre, double>>);
  static_assert(std::is_same_v<decltype(quantity<metre, int>() / quantity<metre, double>()), double>);
  static_assert(std::is_same_v<decltype(quantity<kilometre, int>() / quantity<metre, double>()), double>);
  static_assert(std::is_same_v<decltype(quantity<metre, int>() / quantity<second, int>()), quantity<metre_per_second, int>>);
  static_assert(std::is_same_v<decltype(quantity<metre, int>() % short(1)), quantity<metre, int>>);
  static_assert(std::is_same_v<decltype(quantity<metre, int>() % quantity<metre, short>(1)), quantity<metre, int>>);

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

  static_assert(Quantity<quantity<millimetre, int>>);

  // common_quantity

  static_assert(std::is_same_v<common_quantity<quantity<metre, int>, quantity<kilometre, int>>, quantity<metre, int>>);
  static_assert(std::is_same_v<common_quantity<quantity<kilometre, long long>, quantity<metre, int>>, quantity<metre, long long>>);
  static_assert(std::is_same_v<common_quantity<quantity<kilometre, long long>, quantity<millimetre, double>>, quantity<millimetre, double>>);

  // quantity_cast

  static_assert(std::is_same_v<decltype(quantity_cast<unit<length, ratio<1>>>(2km))::unit, metre>);
  static_assert(std::is_same_v<decltype(quantity_cast<unit<dimension<units::exp<base_dim_length, 1>>, ratio<1>>>(2km))::unit, metre>);

  // static_assert(quantity_cast<int>(2km).count() == 2000);  // should not compile
  static_assert(quantity_cast<quantity<metre, int>>(2km).count() == 2000);
  static_assert(quantity_cast<quantity<kilometre, int>>(2000m).count() == 2);

  // time

  //  static_assert(1s == 1m);  // should not compile
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
