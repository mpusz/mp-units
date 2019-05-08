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

#include "units/velocity.h"
#include "units/frequency.h"
#include <utility>
#include <chrono>

using namespace units;

namespace {

  template<typename T>
  class my_value {
    T value_{};
  public:
    my_value() = default;
    constexpr my_value(T v) : value_{std::move(v)} {}
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
  struct common_type<my_value<T>, U> : common_type<T, U> {
  };

  template<typename T, typename U>
  struct common_type<T, my_value<U>> : common_type<T, U> {
  };

}  // namespace std

namespace {

  using namespace units;

  // class invariants

  // constexpr quantity<dimension_length, second, int> q;  // should a static_assert
  // constexpr quantity<dimension_length, meter, length<meter, int>> error(0_m);  // should trigger a static_assert
  // constexpr quantity<int, int, double> error(0);  // should trigger a static_assert
  // constexpr quantity<dimension_length, unit<dimension_length, std::ratio<-1, 1>>, int> error(0);  // should trigger a static_assert

  // member types

  static_assert(std::is_same_v<length<meter, int>::rep, int>);
  static_assert(std::is_same_v<length<meter, double>::rep, double>);
  static_assert(std::is_same_v<length<meter, int>::unit, meter>);
  static_assert(std::is_same_v<length<kilometer, int>::unit, kilometer>);

  // constructors

  static_assert(length<meter, int>().count() == 0);
  constexpr length<meter, int> km{1000};
  static_assert(km.count() == 1000);
  static_assert(length<meter, int>(km).count() == km.count());

  static_assert(length<meter, int>(1).count() == 1);
  static_assert(length<meter, int>(my_value(1)).count() == 1);
  static_assert(length<meter, my_value<int>>(1).count() == 1);
  //  static_assert(length<meter, int>(1.0).count() == 1);   // should not compile
  //  static_assert(length<meter, int>(my_value(1.0)).count() == 1); // should not compile
  //  static_assert(length<meter, my_value>(1.0).count() == 1);   // should not compile
  static_assert(length<meter, double>(1.0).count() == 1.0);
  static_assert(length<meter, double>(my_value(1.0)).count() == 1.0);
  static_assert(length<meter, double>(1).count() == 1.0);
  static_assert(length<meter, double>(my_value(1)).count() == 1.0);
  static_assert(length<meter, double>(3.14).count() == 3.14);
  static_assert(length<meter, my_value<double>>(1.0).count() == 1.0);
  static_assert(length<meter, my_value<double>>(1).count() == 1.0);
  static_assert(length<meter, my_value<double>>(3.14).count() == 3.14);

  static_assert(length<meter, int>(km).count() == 1000);
  //  static_assert(length<meter, int>(length<meter, double>(3.14)).count() == 3);   // should not compile
  static_assert(length<meter, int>(quantity_cast<length<meter, int>>(3.14_m)).count() == 3);
  //  static_assert(length<meter, int>(length<meter, my_value<double>>(1000.0)).count() == 1000);   // should not compile
  //  static_assert(length<meter, my_value>(1000.0_m).count() == 1000);   // should not compile
  static_assert(length<meter, double>(1000.0_m).count() == 1000.0);
  static_assert(length<meter, double>(length<meter, my_value<double>>(1000.0)).count() == 1000.0);
  static_assert(length<meter, my_value<double>>(1000.0_m).count() == 1000.0);
  static_assert(length<meter, double>(km).count() == 1000.0);
  static_assert(length<meter, my_value<double>>(km).count() == 1000.0);
  static_assert(length<meter, int>(1_km).count() == 1000);
  //  static_assert(length<meter, int>(1_s).count() == 1);   // should not compile
  //  static_assert(length<kilometer, int>(1010_m).count() == 1);   // should not compile
  static_assert(length<kilometer, int>(quantity_cast<length<kilometer, int>>(1010_m)).count() == 1);

  // assignment operator

  static_assert([]() {
    length<meter, int> l1(1), l2(2);
    return l2 = l1;
  }()
                    .count() == 1);

  // static member functions

  static_assert(length<meter, int>::zero().count() == 0);
  static_assert(length<meter, int>::min().count() == std::numeric_limits<int>::lowest());
  static_assert(length<meter, int>::max().count() == std::numeric_limits<int>::max());
  static_assert(length<meter, double>::zero().count() == 0.0);
  static_assert(length<meter, double>::min().count() == std::numeric_limits<double>::lowest());
  static_assert(length<meter, double>::max().count() == std::numeric_limits<double>::max());
  static_assert(length<meter, my_value<int>>::zero().count() == 0);
  static_assert(length<meter, my_value<int>>::min().count() == std::numeric_limits<int>::lowest());
  static_assert(length<meter, my_value<int>>::max().count() == std::numeric_limits<int>::max());
  static_assert(length<meter, my_value<double>>::zero().count() == 0.0);
  static_assert(length<meter, my_value<double>>::min().count() == std::numeric_limits<double>::lowest());
  static_assert(length<meter, my_value<double>>::max().count() == std::numeric_limits<double>::max());

  // unary member operators

  static_assert((+km).count() == 1000);
  static_assert((-km).count() == -1000);
  static_assert((+(-km)).count() == -1000);
  static_assert((-(-km)).count() == 1000);

  // binary member operators

  static_assert([](auto v) {
    auto vv = v++;
    return std::make_pair(v, vv);
  }(km) == std::make_pair(length<meter, int>(1001), length<meter, int>(1000)));
  static_assert([](auto v) {
    auto vv = ++v;
    return std::make_pair(v, vv);
  }(km) == std::make_pair(length<meter, int>(1001), length<meter, int>(1001)));
  static_assert([](auto v) {
    auto vv = v--;
    return std::make_pair(v, vv);
  }(km) == std::make_pair(length<meter, int>(999), length<meter, int>(1000)));
  static_assert([](auto v) {
    auto vv = --v;
    return std::make_pair(v, vv);
  }(km) == std::make_pair(length<meter, int>(999), length<meter, int>(999)));

  // compound assignment

  static_assert((1_m += 1_m).count() == 2);
  static_assert((2_m -= 1_m).count() == 1);
  static_assert((1_m *= 2).count() == 2);
  static_assert((2_m /= 2).count() == 1);
  static_assert((7_m %= 2).count() == 1);
  static_assert((7_m %= 2_m).count() == 1);

  // non-member arithmetic operators

  static_assert(std::is_same_v<decltype(length<meter, int>() + length<meter, double>()), quantity<dimension_length, meter, double>>);
  static_assert(std::is_same_v<decltype(length<meter, int>() + length<meter, double>()), quantity<dimension_length, meter, double>>);
  static_assert(std::is_same_v<decltype(length<kilometer, int>() + length<meter, double>()), quantity<dimension_length, meter, double>>);
  static_assert(std::is_same_v<decltype(length<meter, double>() - length<meter, int>()), quantity<dimension_length, meter, double>>);
  static_assert(std::is_same_v<decltype(length<kilometer, double>() - length<meter, int>()), quantity<dimension_length, meter, double>>);
  static_assert(std::is_same_v<decltype(length<meter, int>() * 1.0), quantity<dimension_length, meter, double>>);
  static_assert(std::is_same_v<decltype(1.0 * length<meter, int>()), quantity<dimension_length, meter, double>>);
  static_assert(std::is_same_v<decltype(velocity<meter_per_second, int>() * units::time<second, int>()), quantity<dimension_length, units::meter, int>>);
  static_assert(std::is_same_v<decltype(1 / units::time<second, int>()), quantity<dimension_frequency, hertz, int>>);
  static_assert(std::is_same_v<decltype(length<meter, int>() / 1.0), quantity<dimension_length, meter, double>>);
  static_assert(std::is_same_v<decltype(length<meter, int>() / length<meter, double>()), double>);
  static_assert(std::is_same_v<decltype(length<kilometer, int>() / length<meter, double>()), double>);
  static_assert(std::is_same_v<decltype(length<meter, int>() / units::time<second, int>()), quantity<dimension_velocity, meter_per_second, int>>);
  static_assert(std::is_same_v<decltype(length<meter, int>() % short(1)), quantity<dimension_length, meter, int>>);
  static_assert(std::is_same_v<decltype(length<meter, int>() % length<meter, short>(1)), quantity<dimension_length, meter, int>>);

  static_assert((1_m + km).count() == 1001);
  static_assert((1_m + 1_km).count() == 1001);
  static_assert((km - 1_m).count() == 999);
  static_assert((1_km - 1_m).count() == 999);
  static_assert((2_m * 2).count() == 4);
  static_assert((3 * 3_m).count() == 9);
  static_assert((4_m / 2).count() == 2);
  static_assert(4_m / 2_m == 2);
  static_assert(4_km / 2000_m == 2);
  static_assert((7_m % 2).count() == 1);
  static_assert((7_m % 2_m).count() == 1);
  static_assert((7_km % 2000_m).count() == 1000);

  // comparators

  static_assert(2_m + 1_m == 3_m);
  static_assert(!(2_m + 2_m == 3_m));
  static_assert(2_m + 2_m != 3_m);
  static_assert(!(2_m + 2_m != 4_m));
  static_assert(2_m > 1_m);
  static_assert(!(1_m > 1_m));
  static_assert(1_m < 2_m);
  static_assert(!(2_m < 2_m));
  static_assert(2_m >= 1_m);
  static_assert(2_m >= 2_m);
  static_assert(!(2_m >= 3_m));
  static_assert(1_m <= 2_m);
  static_assert(2_m <= 2_m);
  static_assert(!(3_m <= 2_m));

  static_assert(3_m == 3.0_m);
  static_assert(3_m != 3.14_m);
  static_assert(2_m > 1.0_m);
  static_assert(1.0_m < 2_m);
  static_assert(2.0_m >= 1_m);
  static_assert(1_m <= 2.0_m);

  static_assert(1000_m == 1_km);
  static_assert(1001_m != 1_km);
  static_assert(1001_m > 1_km);
  static_assert(999_m < 1_km);
  static_assert(1000_m >= 1_km);
  static_assert(1000_m <= 1_km);

  // is_quantity

  static_assert(Quantity<length<millimeter, int>>);

  // common_quantity

  static_assert(std::is_same_v<common_quantity_t<length<meter, int>, length<kilometer, int>>, length<meter, int>>);
  static_assert(std::is_same_v<common_quantity_t<length<kilometer, long long>, length<meter, int>>, length<meter, long long>>);
  static_assert(std::is_same_v<common_quantity_t<length<kilometer, long long>, length<millimeter, double>>, length<millimeter, double>>);

  // quantity_cast

  // static_assert(quantity_cast<int>(2_km).count() == 2000);  // should not compile
  static_assert(quantity_cast<length<meter, int>>(2_km).count() == 2000);
  static_assert(quantity_cast<length<kilometer, int>>(2000_m).count() == 2);

  // time

  //  static_assert(1_s == 1_m);  // should not compile
  static_assert(1_h == 3600_s);

  // length

  static_assert(1_km == 1000_m);
  static_assert(1_km + 1_m == 1001_m);
  static_assert(10_km / 5_km == 2);
  static_assert(10_km / 2 == 5_km);

  // velocity

  static_assert(10_m / 5_s == 2_mps);
  static_assert(10 / 5_s * 1_m == 2_mps);
  static_assert(1_km / 1_s == 1000_mps);
  static_assert(2_kmph * 2_h == 4_km);
  static_assert(2_km / 2_kmph == 1_h);

}  // namespace
