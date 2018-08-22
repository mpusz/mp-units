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

#include "units/si/velocity.h"
#include <gtest/gtest.h>
#include <utility>

using namespace units;
using namespace units::literals;



// is_quantity

static_assert(units::detail::is_quantity<millimeters<int>>::value);


// common_type

static_assert(std::is_same_v<std::common_type_t<meters<int>, kilometers<int>>, meters<int>>);


// constructors

constexpr quantity<dimension_length, int> kilometer{1000};
static_assert(kilometer.count() == 1000);
static_assert(length<int>(kilometer).count() == kilometer.count());
static_assert(meters<int>(kilometer).count() == kilometer.count());
static_assert(millimeters<int>(kilometer).count() == kilometer.count() * 1000);
static_assert(quantity_cast<kilometers<int>>(kilometer).count() == kilometer.count() / 1000);

//static_assert(meters<int>(1.0) == 1_m);   // should not compile
static_assert(meters<int>(1) == 1_m);
static_assert(meters<float>(1.0) == 1_m);
static_assert(meters<float>(1) == 1_m);

//static_assert(kilometers<int>(1000_m) == 1000_m);   // should not compile
static_assert(kilometers<float>(1000_m) == 1000_m);
//static_assert(meters<int>(meters<float>(1)) == 1_m);  // should not compile
static_assert(meters<int>(quantity_cast<meters<int>>(meters<float>(1))) == 1_m);


// zero(), min(), max()

static_assert(length<int>::zero().count() == 0);
static_assert(length<int>::min().count() == std::numeric_limits<int>::min());
static_assert(length<int>::max().count() == std::numeric_limits<int>::max());


// unary member operators

static_assert((+1_m).count() == 1);
static_assert((-1_m).count() == -1);


// binary member operators

template<typename Dimension, typename Rep, class Ratio = std::ratio<1>>
constexpr auto post_inc(quantity<Dimension, Rep, Ratio> v)
{
  auto vv = v++;
  return std::make_pair(v, vv);
}

template<typename Dimension, typename Rep, class Ratio = std::ratio<1>>
constexpr auto pre_inc(quantity<Dimension, Rep, Ratio> v)
{
  auto vv = ++v;
  return std::make_pair(v, vv);
}

template<typename Dimension, typename Rep, class Ratio = std::ratio<1>>
constexpr auto post_dec(quantity<Dimension, Rep, Ratio> v)
{
  auto vv = v--;
  return std::make_pair(v, vv);
}

template<typename Dimension, typename Rep, class Ratio = std::ratio<1>>
constexpr auto pre_dec(quantity<Dimension, Rep, Ratio> v)
{
  auto vv = --v;
  return std::make_pair(v, vv);
}

constexpr auto r1 = post_inc(1_m);
static_assert(r1.first.count() == 2);
static_assert(r1.second.count() == 1);

constexpr auto r2 = pre_inc(1_m);
static_assert(r2.first.count() == 2);
static_assert(r2.second.count() == 2);

constexpr auto r3 = post_dec(1_m);
static_assert(r3.first.count() == 0);
static_assert(r3.second.count() == 1);

constexpr auto r4 = pre_dec(1_m);
static_assert(r4.first.count() == 0);
static_assert(r4.second.count() == 0);


// compound assignment

static_assert((1_km += 1_km).count() == 2);
static_assert((2_km -= 1_km).count() == 1);
static_assert((1_km *= 2).count() == 2);
static_assert((2_km /= 2).count() == 1);
static_assert((7_km %= 2).count() == 1);
static_assert((7_km %= 2_km).count() == 1);


// non-member arithmetic operators

static_assert((1_km + 1_m).count() == 1001);
static_assert((1_km - 1_m).count() == 999);
static_assert((2_km * 2).count() == 4);
static_assert((3 * 3_km).count() == 9);
static_assert((2_kmph * 2_h).count() == 4);
//static_assert(kilometers<float>(2_kmph * 15_min).count() == 500_m); // should not compile
static_assert(kilometers<float>(2_kmph * 120.0_min).count() == 4);
static_assert(kilometers<float>(2.0_kmph * 120_min).count() == 4);
static_assert((4_km / 2).count() == 2);
static_assert(4_km / 2_km == 2);
static_assert((1_km / 1_s).count() == 1);
//static_assert((1_km / 1_h).count() == 1);  // should not compile
static_assert((1.0_km / 1_h).count() == 1);
static_assert((7_km % 2).count() == 1);
static_assert((7_km % 2_km).count() == 1);


// comparators

static_assert(2_km + 1_km == 3_km);
static_assert(!(2_km + 2_km == 3_km));
static_assert(2_km + 2_km != 3_km);
static_assert(!(2_km + 2_km != 4_km));
static_assert(2_km > 1_km);
static_assert(!(1_km > 1_km));
static_assert(1_km < 2_km);
static_assert(!(2_km < 2_km));
static_assert(2_km >= 1_km);
static_assert(2_km >= 2_km);
static_assert(!(2_km >= 3_km));
static_assert(1_km <= 2_km);
static_assert(2_km <= 2_km);
static_assert(!(3_km <= 2_km));



// quantity_cast

static_assert(quantity_cast<kilometers<int>>(2000_m) == 2_km);
// static_assert(quantity_cast<seconds<int>>(2_m) == 2_s);  // should not compile
