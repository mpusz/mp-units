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

#include "units/quantity_point.h"
#include "test_tools.h"
#include "units/math.h"
#include "units/physical/si/si.h"
#include "units/physical/si/us/us.h"
#include <chrono>
#include <utility>

namespace {

using namespace units;
using namespace units::physical::si;

// class invariants

template<typename DimLength>
concept invalid_types = requires
{
  !requires { typename quantity_point<DimLength, second, int>; };  // unit of a different dimension
  !requires { typename quantity_point<DimLength, metre, quantity<DimLength, metre, int>>; };  // quantity used as Rep
  // quantity point used as Rep
  !requires { typename quantity_point<DimLength, metre, quantity_point<DimLength, metre, int>>; };
  !requires { typename quantity<metre, DimLength, double>; };  // reordered arguments
};

static_assert(invalid_types<dim_length>);

// member types

static_assert(is_same_v<quantity_point<dim_length, metre, int>::rep, int>);
static_assert(is_same_v<quantity_point<dim_length, metre, double>::rep, double>);
static_assert(is_same_v<quantity_point<dim_length, metre, int>::unit, metre>);
static_assert(is_same_v<quantity_point<dim_length, kilometre, int>::unit, kilometre>);

// constructors

static_assert(quantity_point<dim_length, metre, int>().relative() == 0_q_m);
constexpr quantity_point<dim_length, metre, int> km{1000_q_m};
static_assert(km.relative() == 1000_q_m);
static_assert(quantity_point<dim_length, metre, int>(km).relative() == km.relative());

static_assert(quantity_point<dim_length, metre, int>(1_q_m).relative() == 1_q_m);
static_assert(!std::is_constructible_v<quantity_point<dim_length, metre, int>, double>); // truncating conversion
static_assert(quantity_point<dim_length, metre, double>(1.0_q_m).relative() == 1.0_q_m);
static_assert(quantity_point<dim_length, metre, double>(1_q_m).relative() == 1_q_m);
static_assert(quantity_point<dim_length, metre, long double>(3.14_q_m).relative() == 3.14_q_m);

static_assert(quantity_point<dim_length, metre, int>(km).relative() == 1000_q_m);
static_assert(!std::is_constructible_v<quantity_point<dim_length, metre, int>,
                                       quantity_point<dim_length, metre, double>>);  // truncating conversion
static_assert(quantity_point<dim_length, metre, double>(quantity_point(1000.0_q_m)).relative() == 1000.0_q_m);
static_assert(quantity_point<dim_length, metre, double>(km).relative() == 1000.0_q_m);
static_assert(quantity_point<dim_length, metre, int>(quantity_point(1_q_km)).relative() == 1000_q_m);
static_assert(!std::is_constructible_v<quantity_point<dim_length, metre, int>,
                                       quantity_point<dim_time, second, int>>);  // different dimensions
static_assert(!std::is_constructible_v<quantity_point<dim_length, kilometre, int>,
                                       quantity_point<dim_length, metre, int>>);  // truncating conversion

// assignment operator

static_assert([]() { quantity_point<dim_length, metre, int> l1(1_q_m), l2{}; return l2 = l1; }().relative() == 1_q_m);

// static member functions

static_assert(quantity_point<dim_length, metre, int>::min().relative().count() == std::numeric_limits<int>::lowest());
static_assert(quantity_point<dim_length, metre, int>::max().relative().count() == std::numeric_limits<int>::max());
static_assert(quantity_point<dim_length, metre, double>::min().relative().count() ==
              std::numeric_limits<double>::lowest());
static_assert(quantity_point<dim_length, metre, double>::max().relative().count() ==
              std::numeric_limits<double>::max());

// unary member operators

static_assert([](auto v) {
  auto vv = v++;
  return std::pair(v, vv);
}(km) == std::pair(quantity_point<dim_length, metre, int>(1001_q_m), quantity_point<dim_length, metre, int>(1000_q_m)));
static_assert([](auto v) {
  auto vv = ++v;
  return std::pair(v, vv);
}(km) == std::pair(quantity_point<dim_length, metre, int>(1001_q_m), quantity_point<dim_length, metre, int>(1001_q_m)));
static_assert([](auto v) {
  auto vv = v--;
  return std::pair(v, vv);
}(km) == std::pair(quantity_point<dim_length, metre, int>(999_q_m), quantity_point<dim_length, metre, int>(1000_q_m)));
static_assert([](auto v) {
  auto vv = --v;
  return std::pair(v, vv);
}(km) == std::pair(quantity_point<dim_length, metre, int>(999_q_m), quantity_point<dim_length, metre, int>(999_q_m)));

// compound assignment

static_assert((quantity_point(1_q_m) += 1_q_m).relative().count() == 2);
static_assert((quantity_point(2_q_m) -= 1_q_m).relative().count() == 1);

// non-member arithmetic operators

static_assert(compare<decltype(quantity_point<dim_length, metre, int>() + length<metre, double>()),
                             quantity_point<dim_length, metre, double>>);
static_assert(compare<decltype(length<metre, int>() + quantity_point<dim_length, metre, double>()),
                             quantity_point<dim_length, metre, double>>);
static_assert(compare<decltype(quantity_point<dim_length, kilometre, int>() + length<metre, double>()),
                             quantity_point<dim_length, metre, double>>);
static_assert(compare<decltype(length<kilometre, int>() + quantity_point<dim_length, metre, double>()),
                             quantity_point<dim_length, metre, double>>);
static_assert(compare<decltype(quantity_point<dim_length, metre, double>() - length<metre, int>()),
                             quantity_point<dim_length, metre, double>>);
static_assert(compare<decltype(quantity_point<dim_length, kilometre, double>() - length<metre, int>()),
                             quantity_point<dim_length, metre, double>>);
static_assert(
    compare<decltype(quantity_point<dim_length, metre, double>() - quantity_point<dim_length, metre, int>()),
                   length<metre, double>>);
static_assert(
    compare<decltype(quantity_point<dim_length, kilometre, double>() - quantity_point<dim_length, metre, int>()),
                   length<metre, double>>);

static_assert((1_q_m + km).relative().count() == 1001);
static_assert((quantity_point(1_q_m) + 1_q_km).relative().count() == 1001);
static_assert((km - 1_q_m).relative().count() == 999);
static_assert((quantity_point(1_q_km) - quantity_point(1_q_m)).count() == 999);

// comparators

static_assert(quantity_point(2_q_m) + 1_q_m == quantity_point(3_q_m));
static_assert(!(2_q_m + quantity_point(2_q_m) == quantity_point(3_q_m)));
static_assert(quantity_point(2_q_m) + 2_q_m != quantity_point(3_q_m));
static_assert(!(2_q_m + quantity_point(2_q_m) != quantity_point(4_q_m)));
static_assert(quantity_point(2_q_m) > quantity_point(1_q_m));
static_assert(!(quantity_point(1_q_m) > quantity_point(1_q_m)));
static_assert(quantity_point(1_q_m) < quantity_point(2_q_m));
static_assert(!(quantity_point(2_q_m) < quantity_point(2_q_m)));
static_assert(quantity_point(2_q_m) >= quantity_point(1_q_m));
static_assert(quantity_point(2_q_m) >= quantity_point(2_q_m));
static_assert(!(quantity_point(2_q_m) >= quantity_point(3_q_m)));
static_assert(quantity_point(1_q_m) <= quantity_point(2_q_m));
static_assert(quantity_point(2_q_m) <= quantity_point(2_q_m));
static_assert(!(quantity_point(3_q_m) <= quantity_point(2_q_m)));

static_assert(quantity_point(3_q_m) == quantity_point(3.0_q_m));
static_assert(quantity_point(3_q_m) != quantity_point(3.14_q_m));
static_assert(quantity_point(2_q_m) > quantity_point(1.0_q_m));
static_assert(quantity_point(1.0_q_m) < quantity_point(2_q_m));
static_assert(quantity_point(2.0_q_m) >= quantity_point(1_q_m));
static_assert(quantity_point(1_q_m) <= quantity_point(2.0_q_m));

static_assert(quantity_point(1000_q_m) == quantity_point(1_q_km));
static_assert(quantity_point(1001_q_m) != quantity_point(1_q_km));
static_assert(quantity_point(1001_q_m) > quantity_point(1_q_km));
static_assert(quantity_point(999_q_m) < quantity_point(1_q_km));
static_assert(quantity_point(1000_q_m) >= quantity_point(1_q_km));
static_assert(quantity_point(1000_q_m) <= quantity_point(1_q_km));

// alias units

static_assert(quantity_point(2_q_l) + 2_q_ml == quantity_point(2002_q_ml));
static_assert(2_q_l + quantity_point(2_q_ml) == quantity_point(2002_q_cm3));
static_assert(quantity_point(2_q_l) + 2_q_cm3 == quantity_point(2002_q_ml));
static_assert(2_q_dm3 + quantity_point(2_q_cm3) == quantity_point(2002_q_ml));

// is_quantity_point

static_assert(QuantityPoint<quantity_point<dim_length, millimetre, int>>);

// common_quantity_point

static_assert(compare<
              common_quantity_point<quantity_point<dim_length, metre, int>, quantity_point<dim_length, kilometre, int>>,
              quantity_point<dim_length, metre, int>>);
static_assert(compare<common_quantity_point<quantity_point<dim_length, kilometre, long long>,
                                                   quantity_point<dim_length, metre, int>>,
                             quantity_point<dim_length, metre, long long>>);
static_assert(compare<common_quantity_point<quantity_point<dim_length, kilometre, long long>,
                                                   quantity_point<dim_length, millimetre, double>>,
                             quantity_point<dim_length, millimetre, double>>);

// common_type

using namespace units::physical::si::us::literals;

static_assert(std::equality_comparable<decltype(quantity_point(1_q_m))>);
static_assert(std::equality_comparable_with<decltype(quantity_point(1_q_m)), decltype(quantity_point(1_q_km))>);
static_assert(quantity_point(0_q_m) == quantity_point(0_q_ft_us));
static_assert(std::equality_comparable_with<decltype(quantity_point(1_q_m)), decltype(quantity_point(1_q_ft_us))>);

// quantity_cast

static_assert(compare<decltype(quantity_point_cast<scaled_unit<ratio(1), metre>>(quantity_point(2_q_km)))::unit, metre>);

static_assert(quantity_point_cast<quantity_point<dim_length, metre, int>>(quantity_point(2_q_km)).relative().count() ==
              2000);
static_assert(
    quantity_point_cast<quantity_point<dim_length, kilometre, int>>(quantity_point(2000_q_m)).relative().count() == 2);
static_assert(quantity_point_cast<quantity_point<dim_length, metre, int>>(quantity_point(1.23_q_m)).relative().count() ==
              1);
static_assert(quantity_point_cast<length<metre, int>>(quantity_point(2_q_km)).relative().count() == 2000);
static_assert(quantity_point_cast<length<kilometre, int>>(quantity_point(2000_q_m)).relative().count() == 2);
static_assert(quantity_point_cast<length<metre, int>>(quantity_point(1.23_q_m)).relative().count() == 1);
static_assert(quantity_point_cast<metre>(quantity_point(2_q_km)).relative().count() == 2000);
static_assert(quantity_point_cast<kilometre>(quantity_point(2000_q_m)).relative().count() == 2);
static_assert(quantity_point_cast<int>(quantity_point(1.23_q_m)).relative().count() == 1);
static_assert(quantity_point_cast<dim_speed, kilometre_per_hour>(quantity_point(2000.0_q_m / 3600.0_q_s)).relative().count() == 2);

// time

static_assert(quantity_point{1_q_h} == quantity_point{3600_q_s});

template<typename Metre>
concept no_crossdimensional_equality = !requires
{
    quantity_point(1_q_s) == quantity_point(length<Metre, int>(1));
};

static_assert(no_crossdimensional_equality<metre>);

// length

static_assert(quantity_point(1_q_km) != quantity_point(1_q_m));
static_assert(quantity_point(1_q_km) == quantity_point(1000_q_m));
static_assert(quantity_point(1_q_km) + 1_q_m == quantity_point(1001_q_m));
static_assert(1_q_km + quantity_point(1_q_m) == quantity_point(1001_q_m));

template<class T>
concept dimensional_analysis = requires(T t)
{
  pow<2>(t);
};

static_assert(!dimensional_analysis<quantity_point<dim_length, metre, int>>);

}  // namespace
