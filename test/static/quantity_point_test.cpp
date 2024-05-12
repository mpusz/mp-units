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

#include "test_tools.h"
#include <mp-units/compat_macros.h>
#include <mp-units/ext/type_traits.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#include <mp-units/systems/usc.h>
#include <chrono>
#include <concepts>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

namespace {

using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using namespace mp_units::usc::unit_symbols;
using namespace std::chrono_literals;
using sys_seconds = std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>;

inline constexpr struct zeroth_length : absolute_point_origin<zeroth_length, isq::length> {
} zeroth_length;

inline constexpr struct mean_sea_level : absolute_point_origin<mean_sea_level, isq::height> {
} mean_sea_level;

inline constexpr struct my_mean_sea_level : decltype(mean_sea_level) {
} my_mean_sea_level;

inline constexpr struct same_mean_sea_level : relative_point_origin<mean_sea_level + 0 * isq::height[m]> {
} same_mean_sea_level;

inline constexpr struct ground_level : relative_point_origin<mean_sea_level + 42 * isq::height[m]> {
} ground_level;

inline constexpr struct my_ground_level : decltype(ground_level) {
} my_ground_level;

inline constexpr struct same_ground_level1 : relative_point_origin<mean_sea_level + 42 * isq::height[m]> {
} same_ground_level1;

inline constexpr struct same_ground_level2 : relative_point_origin<my_mean_sea_level + 42 * isq::height[m]> {
} same_ground_level2;

inline constexpr struct tower_peak : relative_point_origin<ground_level + 42 * isq::height[m]> {
} tower_peak;

inline constexpr struct other_ground_level : relative_point_origin<mean_sea_level + 123 * isq::height[m]> {
} other_ground_level;

inline constexpr struct other_absolute_level : absolute_point_origin<other_absolute_level, isq::height> {
} other_absolute_level;

inline constexpr struct zero : absolute_point_origin<zero, dimensionless> {
} zero;

QUANTITY_SPEC(special_height, isq::height);


//////////////////
// point origins
//////////////////

static_assert(si::absolute_zero == si::zeroth_kelvin);
static_assert(si::ice_point == si::zeroth_degree_Celsius);
static_assert(si::absolute_zero != si::ice_point);
static_assert(si::zeroth_kelvin != si::zeroth_degree_Celsius);

static_assert(my_mean_sea_level == mean_sea_level);
static_assert(my_mean_sea_level == same_mean_sea_level);

static_assert(my_ground_level == ground_level);
static_assert(same_ground_level1 == ground_level);
static_assert(same_ground_level2 == my_ground_level);

static_assert(mean_sea_level != zeroth_length);
static_assert(mean_sea_level != other_absolute_level);
static_assert(my_mean_sea_level != other_absolute_level);
static_assert(ground_level != other_ground_level);

template<auto QS>
struct absolute_po_ : absolute_point_origin<absolute_po_<QS>, QS> {};
template<auto QS>
inline constexpr absolute_po_<QS> absolute_po;

template<auto QP>
struct relative_po_ : relative_point_origin<QP> {};
template<auto QP>
inline constexpr relative_po_<QP> relative_po;

static_assert(relative_po<absolute_po<isq::length> + isq::height(42 * m)>.quantity_spec == isq::height);
static_assert(relative_po<absolute_po<kind_of<isq::length>> + isq::height(42 * m)>.quantity_spec == isq::height);
static_assert(relative_po<absolute_po<isq::height> + 42 * m>.quantity_spec == isq::height);

inline constexpr struct my_kelvin : named_unit<"my_K", mag<10> * si::kelvin> {
} my_kelvin;

static_assert(default_point_origin(si::kelvin) == si::absolute_zero);
static_assert(default_point_origin(si::milli<si::kelvin>) == si::absolute_zero);
static_assert(default_point_origin(mag<10> * si::kelvin) == si::absolute_zero);
static_assert(default_point_origin(my_kelvin) == si::absolute_zero);

static_assert(default_point_origin(si::degree_Celsius) == si::ice_point);
static_assert(default_point_origin(mag<10> * si::degree_Celsius) == si::ice_point);

static_assert(default_point_origin(si::metre) == zeroth_point_origin<kind_of<isq::length>>);
static_assert(default_point_origin(si::kelvin / si::second) ==
              zeroth_point_origin<kind_of<isq::thermodynamic_temperature / isq::time>>);
static_assert(default_point_origin(si::degree_Celsius / si::second) ==
              zeroth_point_origin<kind_of<isq::thermodynamic_temperature / isq::time>>);

static_assert(zeroth_point_origin<isq::length / isq::time> == zeroth_point_origin<isq::speed>);
static_assert(zeroth_point_origin<inverse(isq::period_duration)> == zeroth_point_origin<isq::frequency>);
static_assert(zeroth_point_origin<kind_of<isq::length>> == zeroth_point_origin<isq::height>);
static_assert(zeroth_point_origin<kind_of<inverse(isq::time)>> == zeroth_point_origin<isq::frequency>);

static_assert(zeroth_point_origin<isq::length> != zeroth_point_origin<isq::height>);
static_assert(zeroth_point_origin<isq::width> != zeroth_point_origin<isq::height>);
static_assert(zeroth_point_origin<inverse(isq::time)> != zeroth_point_origin<isq::frequency>);

/////////////////////
// class invariants
/////////////////////

static_assert(sizeof(quantity_point<si::metre, mean_sea_level>) == sizeof(double));
static_assert(sizeof(quantity_point<isq::height[m], mean_sea_level>) == sizeof(double));
static_assert(sizeof(quantity_point<si::metre, ground_level, short>) == sizeof(short));
static_assert(sizeof(quantity_point<isq::height[m], ground_level, short>) == sizeof(short));

template<template<auto, auto, typename> typename QP>
concept invalid_types = requires {
  // unit of a different dimension
  requires !requires { typename QP<si::second, mean_sea_level, int>; };
  requires !requires { typename QP<si::second, ground_level, int>; };
  // incompatible quantity_spec in the origin and quantity_point
  requires !requires { typename QP<isq::width[m], mean_sea_level, int>; };
  requires !requires { typename QP<isq::width[m], ground_level, int>; };
  requires !requires { typename QP<isq::length[m], mean_sea_level, int>; };
  requires !requires { typename QP<isq::length[m], ground_level, int>; };
  requires !requires { typename QP<isq::length[m], zeroth_point_origin<isq::height>, int>; };
  requires !requires { typename QP<isq::width[m], zeroth_point_origin<isq::height>, int>; };
  // quantity used as Rep
  requires !requires { typename QP<si::metre, mean_sea_level, quantity<si::metre, int>>; };
  // quantity point used as Rep
  requires !requires { typename QP<si::metre, mean_sea_level, quantity_point<si::metre, mean_sea_level>>; };
  // reordered arguments
  requires !requires { typename QP<mean_sea_level, si::metre, double>; };
  // quantity_spec used as origin
  requires !requires { typename QP<si::metre, isq::length, int>; };
  // quantity_spec used as a reference
  requires !requires { typename QP<isq::length, mean_sea_level, int>; };
  // dimension used as a reference
  requires !requires { typename QP<isq::dim_length, mean_sea_level, int>; };
  // bool used as a representation type
  requires !requires { typename QP<si::metre, mean_sea_level, bool>; };
};
static_assert(invalid_types<quantity_point>);

template<template<auto, auto, typename> typename QP>
concept valid_types = requires {
  typename QP<si::metre, mean_sea_level, int>;
  typename QP<isq::height[m], mean_sea_level, int>;
  typename QP<special_height[m], mean_sea_level, int>;
  typename QP<si::metre, ground_level, int>;
  typename QP<isq::height[m], ground_level, int>;
  typename QP<special_height[m], ground_level, int>;
  typename QP<isq::height[m], zeroth_point_origin<isq::length>, int>;
  typename QP<isq::height[m], zeroth_point_origin<kind_of<isq::length>>, int>;
  typename QP<si::metre, zeroth_point_origin<isq::height>, int>;
};
static_assert(valid_types<quantity_point>);

static_assert(std::is_trivially_default_constructible_v<quantity_point<si::metre, mean_sea_level>>);
static_assert(std::is_trivially_copy_constructible_v<quantity_point<si::metre, mean_sea_level>>);
static_assert(std::is_trivially_move_constructible_v<quantity_point<si::metre, mean_sea_level>>);
static_assert(std::is_trivially_copy_assignable_v<quantity_point<si::metre, mean_sea_level>>);
static_assert(std::is_trivially_move_assignable_v<quantity_point<si::metre, mean_sea_level>>);
static_assert(std::is_trivially_destructible_v<quantity_point<si::metre, mean_sea_level>>);

static_assert(std::is_nothrow_default_constructible_v<quantity_point<si::metre, mean_sea_level>>);
static_assert(std::is_nothrow_copy_constructible_v<quantity_point<si::metre, mean_sea_level>>);
static_assert(std::is_nothrow_move_constructible_v<quantity_point<si::metre, mean_sea_level>>);
static_assert(std::is_nothrow_copy_assignable_v<quantity_point<si::metre, mean_sea_level>>);
static_assert(std::is_nothrow_move_assignable_v<quantity_point<si::metre, mean_sea_level>>);
static_assert(std::is_nothrow_destructible_v<quantity_point<si::metre, mean_sea_level>>);

static_assert(std::is_trivially_copyable_v<quantity_point<si::metre, mean_sea_level>>);
static_assert(std::is_standard_layout_v<quantity_point<si::metre, mean_sea_level>>);

static_assert(std::default_initializable<quantity_point<si::metre, mean_sea_level>>);
static_assert(std::move_constructible<quantity_point<si::metre, mean_sea_level>>);
static_assert(std::copy_constructible<quantity_point<si::metre, mean_sea_level>>);
static_assert(std::equality_comparable<quantity_point<si::metre, mean_sea_level>>);
static_assert(std::totally_ordered<quantity_point<si::metre, mean_sea_level>>);
static_assert(std::regular<quantity_point<si::metre, mean_sea_level>>);

static_assert(std::three_way_comparable<quantity_point<si::metre, mean_sea_level>>);


//////////////////
// member values
//////////////////

static_assert(quantity_point<si::metre>::reference == si::metre);
static_assert(quantity_point<si::metre>::quantity_spec == kind_of<isq::length>);
static_assert(quantity_point<si::metre>::dimension == isq::dim_length);
static_assert(quantity_point<si::metre>::unit == si::metre);
static_assert(is_of_type<quantity_point<si::metre>::point_origin, zeroth_point_origin_<kind_of<isq::length>>>);
static_assert(is_of_type<quantity_point<si::metre>::absolute_point_origin, zeroth_point_origin_<kind_of<isq::length>>>);

static_assert(quantity_point<isq::height[m]>::reference == isq::height[m]);
static_assert(quantity_point<isq::height[m]>::quantity_spec == isq::height);
static_assert(quantity_point<isq::height[m]>::dimension == isq::dim_length);
static_assert(quantity_point<isq::height[m]>::unit == si::metre);
static_assert(is_of_type<quantity_point<isq::height[m]>::point_origin, zeroth_point_origin_<isq::height>>);
static_assert(is_of_type<quantity_point<isq::height[m]>::absolute_point_origin, zeroth_point_origin_<isq::height>>);

static_assert(quantity_point<si::metre, mean_sea_level>::reference == si::metre);
static_assert(quantity_point<si::metre, mean_sea_level>::quantity_spec == kind_of<isq::length>);
static_assert(quantity_point<si::metre, mean_sea_level>::dimension == isq::dim_length);
static_assert(quantity_point<si::metre, mean_sea_level>::unit == si::metre);
static_assert(is_of_type<quantity_point<si::metre, mean_sea_level>::point_origin, struct mean_sea_level>);
static_assert(is_of_type<quantity_point<si::metre, mean_sea_level>::absolute_point_origin, struct mean_sea_level>);

static_assert(quantity_point<isq::height[m], mean_sea_level>::reference == isq::height[m]);
static_assert(quantity_point<isq::height[m], mean_sea_level>::quantity_spec == isq::height);
static_assert(quantity_point<isq::height[m], mean_sea_level>::dimension == isq::dim_length);
static_assert(quantity_point<isq::height[m], mean_sea_level>::unit == si::metre);
static_assert(is_of_type<quantity_point<isq::height[m], mean_sea_level>::point_origin, struct mean_sea_level>);
static_assert(is_of_type<quantity_point<isq::height[m], mean_sea_level>::absolute_point_origin, struct mean_sea_level>);

static_assert(quantity_point<isq::height[m], ground_level>::reference == isq::height[m]);
static_assert(quantity_point<isq::height[m], ground_level>::quantity_spec == isq::height);
static_assert(quantity_point<isq::height[m], ground_level>::dimension == isq::dim_length);
static_assert(quantity_point<isq::height[m], ground_level>::unit == si::metre);
static_assert(is_of_type<quantity_point<isq::height[m], ground_level>::point_origin, struct ground_level>);
static_assert(is_of_type<quantity_point<isq::height[m], ground_level>::absolute_point_origin, struct mean_sea_level>);

static_assert(quantity_point<isq::height[m], tower_peak>::reference == isq::height[m]);
static_assert(quantity_point<isq::height[m], tower_peak>::quantity_spec == isq::height);
static_assert(quantity_point<isq::height[m], tower_peak>::dimension == isq::dim_length);
static_assert(quantity_point<isq::height[m], tower_peak>::unit == si::metre);
static_assert(is_of_type<quantity_point<isq::height[m], tower_peak>::point_origin, struct tower_peak>);
static_assert(is_of_type<quantity_point<isq::height[m], tower_peak>::absolute_point_origin, struct mean_sea_level>);

static_assert(quantity_point<si::kelvin, si::absolute_zero>::reference == si::kelvin);
static_assert(quantity_point<si::kelvin, si::absolute_zero>::quantity_spec == kind_of<isq::thermodynamic_temperature>);
static_assert(quantity_point<si::kelvin, si::absolute_zero>::dimension == isq::dim_thermodynamic_temperature);
static_assert(quantity_point<si::kelvin, si::absolute_zero>::unit == si::kelvin);
static_assert(is_of_type<quantity_point<si::kelvin, si::absolute_zero>::point_origin, struct si::absolute_zero>);
static_assert(
  is_of_type<quantity_point<si::kelvin, si::absolute_zero>::absolute_point_origin, struct si::absolute_zero>);

static_assert(quantity_point<isq::thermodynamic_temperature[si::kelvin], si::absolute_zero>::reference ==
              isq::thermodynamic_temperature[si::kelvin]);
static_assert(quantity_point<isq::thermodynamic_temperature[si::kelvin], si::absolute_zero>::quantity_spec ==
              isq::thermodynamic_temperature);
static_assert(quantity_point<isq::thermodynamic_temperature[si::kelvin], si::absolute_zero>::dimension ==
              isq::dim_thermodynamic_temperature);
static_assert(quantity_point<isq::thermodynamic_temperature[si::kelvin], si::absolute_zero>::unit == si::kelvin);
static_assert(is_of_type<quantity_point<isq::thermodynamic_temperature[si::kelvin], si::absolute_zero>::point_origin,
                         struct si::absolute_zero>);
static_assert(
  is_of_type<quantity_point<isq::thermodynamic_temperature[si::kelvin], si::absolute_zero>::absolute_point_origin,
             struct si::absolute_zero>);

static_assert(quantity_point<isq::Celsius_temperature[si::kelvin], si::absolute_zero>::reference ==
              isq::Celsius_temperature[si::kelvin]);
static_assert(quantity_point<isq::Celsius_temperature[si::kelvin], si::absolute_zero>::quantity_spec ==
              isq::Celsius_temperature);
static_assert(quantity_point<isq::Celsius_temperature[si::kelvin], si::absolute_zero>::dimension ==
              isq::dim_thermodynamic_temperature);
static_assert(quantity_point<isq::Celsius_temperature[si::kelvin], si::absolute_zero>::unit == si::kelvin);
static_assert(is_of_type<quantity_point<isq::Celsius_temperature[si::kelvin], si::absolute_zero>::point_origin,
                         struct si::absolute_zero>);
static_assert(is_of_type<quantity_point<isq::Celsius_temperature[si::kelvin], si::absolute_zero>::absolute_point_origin,
                         struct si::absolute_zero>);

static_assert(quantity_point<si::degree_Celsius, si::ice_point>::reference == si::degree_Celsius);
static_assert(quantity_point<si::degree_Celsius, si::ice_point>::quantity_spec ==
              kind_of<isq::thermodynamic_temperature>);
static_assert(quantity_point<si::degree_Celsius, si::ice_point>::dimension == isq::dim_thermodynamic_temperature);
static_assert(quantity_point<si::degree_Celsius, si::ice_point>::unit == si::degree_Celsius);
static_assert(is_of_type<quantity_point<si::degree_Celsius, si::ice_point>::point_origin, struct si::ice_point>);
static_assert(
  is_of_type<quantity_point<si::degree_Celsius, si::ice_point>::absolute_point_origin, struct si::zeroth_kelvin>);

static_assert(quantity_point<isq::Celsius_temperature[si::degree_Celsius], si::ice_point>::reference ==
              isq::Celsius_temperature[si::degree_Celsius]);
static_assert(quantity_point<isq::Celsius_temperature[si::degree_Celsius], si::ice_point>::quantity_spec ==
              isq::Celsius_temperature);
static_assert(quantity_point<isq::Celsius_temperature[si::degree_Celsius], si::ice_point>::dimension ==
              isq::dim_thermodynamic_temperature);
static_assert(quantity_point<isq::Celsius_temperature[si::degree_Celsius], si::ice_point>::unit == si::degree_Celsius);
static_assert(is_of_type<quantity_point<isq::Celsius_temperature[si::degree_Celsius], si::ice_point>::point_origin,
                         struct si::ice_point>);
static_assert(
  is_of_type<quantity_point<isq::Celsius_temperature[si::degree_Celsius], si::ice_point>::absolute_point_origin,
             struct si::zeroth_kelvin>);


//////////////////
// member types
//////////////////

static_assert(is_same_v<quantity_point<si::metre, mean_sea_level>::rep, double>);
static_assert(is_same_v<quantity_point<si::metre, mean_sea_level>::quantity_type, quantity<si::metre>>);

static_assert(is_same_v<quantity_point<si::metre, mean_sea_level, int>::rep, int>);
static_assert(is_same_v<quantity_point<si::metre, mean_sea_level, int>::quantity_type, quantity<si::metre, int>>);

static_assert(is_same_v<quantity_point<isq::height[m], mean_sea_level>::rep, double>);
static_assert(is_same_v<quantity_point<isq::height[m], mean_sea_level>::quantity_type, quantity<isq::height[m]>>);

static_assert(is_same_v<quantity_point<isq::height[m], mean_sea_level, int>::rep, int>);
static_assert(
  is_same_v<quantity_point<isq::height[m], mean_sea_level, int>::quantity_type, quantity<isq::height[m], int>>);


////////////////////////////
// static member functions
////////////////////////////

static_assert(
  quantity_point<isq::height[m], mean_sea_level>::min().quantity_from(mean_sea_level).numerical_value_in(m) ==
  std::numeric_limits<double>::lowest());
static_assert(
  quantity_point<isq::height[m], mean_sea_level>::max().quantity_from(mean_sea_level).numerical_value_in(m) ==
  std::numeric_limits<double>::max());

static_assert(
  quantity_point<isq::height[m], ground_level, int>::min().quantity_from(ground_level).numerical_value_in(m) ==
  std::numeric_limits<int>::lowest());
static_assert(
  quantity_point<isq::height[m], ground_level, int>::max().quantity_from(ground_level).numerical_value_in(m) ==
  std::numeric_limits<int>::max());


//////////////////////////////
// construction from a value
//////////////////////////////

// there is no construction from a value
static_assert(!std::constructible_from<quantity_point<isq::height[m], mean_sea_level>, double>);
static_assert(!std::convertible_to<double, quantity_point<isq::height[m], mean_sea_level>>);

static_assert(!std::constructible_from<quantity_point<isq::height[m], ground_level, int>, int>);
static_assert(!std::convertible_to<int, quantity_point<isq::height[m], ground_level, int>>);

static_assert(!std::constructible_from<quantity_point<dimensionless[percent], zero>, double>);
static_assert(!std::convertible_to<double, quantity_point<dimensionless[percent], zero>>);

static_assert(!std::constructible_from<quantity_point<dimensionless[percent], zero, int>, int>);
static_assert(!std::convertible_to<int, quantity_point<dimensionless[percent], zero, int>>);
static_assert(!std::constructible_from<quantity_point<dimensionless[one], zero>, double>);
static_assert(!std::convertible_to<double, quantity_point<dimensionless[one], zero>>);
static_assert(!std::constructible_from<quantity_point<dimensionless[one], zero, int>, int>);
static_assert(!std::convertible_to<int, quantity_point<dimensionless[one], zero, int>>);


/////////////////////////////////
// construction from a quantity
/////////////////////////////////

// -----------------------------
// implicit zeroth point origins
// -----------------------------

static_assert(std::constructible_from<quantity_point<si::metre>, quantity<si::metre>>);
static_assert(!std::convertible_to<quantity<si::metre>, quantity_point<si::metre>>);

static_assert(std::constructible_from<quantity_point<isq::height[m]>, quantity<isq::height[m]>>);
static_assert(!std::convertible_to<quantity<isq::height[m]>, quantity_point<isq::height[m]>>);

static_assert(std::constructible_from<quantity_point<isq::height[m]>, quantity<si::metre>>);
static_assert(!std::convertible_to<quantity<si::metre>, quantity_point<isq::height[m]>>);

static_assert(std::constructible_from<quantity_point<si::metre>, quantity<isq::height[m]>>);
static_assert(!std::convertible_to<quantity<isq::height[m]>, quantity_point<si::metre>>);

static_assert(std::constructible_from<quantity_point<m / s>, quantity<isq::speed[m / s]>>);
static_assert(!std::convertible_to<quantity<isq::speed[m / s]>, quantity_point<m / s>>);

static_assert(std::constructible_from<quantity_point<isq::speed[m / s]>, quantity<m / s>>);
static_assert(!std::convertible_to<quantity<m / s>, quantity_point<isq::speed[m / s]>>);

static_assert(std::constructible_from<quantity_point<isq::length[m] / isq::time[s]>, quantity<isq::speed[m / s]>>);
static_assert(!std::convertible_to<quantity<isq::speed[m / s]>, quantity_point<isq::length[m] / isq::time[s]>>);

static_assert(std::constructible_from<quantity_point<isq::speed[m / s]>, quantity<isq::length[m] / isq::time[s]>>);
static_assert(!std::convertible_to<quantity<isq::length[m] / isq::time[s]>, quantity_point<isq::speed[m / s]>>);

static_assert(std::constructible_from<quantity_point<isq::speed[m / s]>, quantity<m / s>>);
static_assert(!std::convertible_to<quantity<m / s>, quantity_point<isq::speed[m / s]>>);

static_assert(std::constructible_from<quantity_point<dimensionless[one]>, quantity<dimensionless[one]>>);
static_assert(!std::convertible_to<quantity<dimensionless[one]>, quantity_point<dimensionless[one]>>);

// different dimensions
static_assert(!std::constructible_from<quantity_point<isq::height[m]>, quantity<si::second>>);
static_assert(!std::convertible_to<quantity<si::second>, quantity_point<isq::height[m]>>);

// convertible but different quantity_specs
static_assert(std::constructible_from<quantity_point<isq::length[m]>, quantity<isq::height[m]>>);
static_assert(!std::convertible_to<quantity<isq::height[m]>, quantity_point<isq::length[m]>>);

static_assert(std::constructible_from<quantity_point<isq::height[m]>, quantity<special_height[m]>>);
static_assert(!std::convertible_to<quantity<special_height[m]>, quantity_point<isq::height[m]>>);

// quantity_specs with common_quantity_spec
static_assert(!std::constructible_from<quantity_point<isq::width[m]>, quantity<isq::height[m]>>);
static_assert(!std::convertible_to<quantity<isq::height[m]>, quantity_point<isq::width[m]>>);

static_assert(!std::constructible_from<quantity_point<isq::height[m]>, quantity<isq::width[m]>>);
static_assert(!std::convertible_to<quantity<isq::width[m]>, quantity_point<isq::height[m]>>);

// non-convertible quantity_specs
static_assert(!std::constructible_from<quantity_point<isq::height[m]>, quantity<isq::length[m]>>);
static_assert(!std::convertible_to<quantity<isq::length[m]>, quantity_point<isq::height[m]>>);

static_assert(!std::constructible_from<quantity_point<special_height[m]>, quantity<isq::height[m]>>);
static_assert(!std::convertible_to<quantity<special_height[m]>, quantity_point<isq::height[m]>>);

// quantity-like
static_assert(!std::constructible_from<quantity_point<si::second>, std::chrono::seconds>);
static_assert(!std::convertible_to<std::chrono::seconds, quantity_point<si::second>>);

static_assert(!std::constructible_from<quantity_point<isq::time[s]>, std::chrono::seconds>);
static_assert(!std::convertible_to<std::chrono::seconds, quantity_point<isq::time[s]>>);

static_assert(!std::constructible_from<quantity_point<isq::period_duration[s]>, std::chrono::seconds>);
static_assert(!std::convertible_to<std::chrono::seconds, quantity_point<isq::period_duration[s]>>);


// ----------------------
// explicit point origins
// ----------------------

static_assert(!std::constructible_from<quantity_point<si::metre, mean_sea_level>, quantity<si::metre>>);
static_assert(!std::convertible_to<quantity<si::metre>, quantity_point<si::metre, mean_sea_level>>);

static_assert(!std::constructible_from<quantity_point<isq::height[m], mean_sea_level>, quantity<isq::height[m]>>);
static_assert(!std::convertible_to<quantity<isq::height[m]>, quantity_point<isq::height[m], mean_sea_level>>);

static_assert(!std::constructible_from<quantity_point<isq::height[m], mean_sea_level>, quantity<si::metre>>);
static_assert(!std::convertible_to<quantity<si::metre>, quantity_point<isq::height[m], mean_sea_level>>);

static_assert(!std::constructible_from<quantity_point<si::metre, mean_sea_level>, quantity<isq::height[m]>>);
static_assert(!std::convertible_to<quantity<isq::height[m]>, quantity_point<si::metre, mean_sea_level>>);

static_assert(!std::constructible_from<quantity_point<isq::height[m], mean_sea_level>, quantity<special_height[m]>>);
static_assert(!std::convertible_to<quantity<special_height[m]>, quantity_point<isq::height[m], mean_sea_level>>);

static_assert(!std::constructible_from<quantity_point<dimensionless[one], zero>, quantity<dimensionless[one]>>);
static_assert(!std::convertible_to<quantity<dimensionless[one]>, quantity_point<dimensionless[one], zero>>);

// quantity_specs with common_quantity_spec
static_assert(!std::constructible_from<quantity_point<isq::width[m], zeroth_length>, quantity<isq::height[m]>>);
static_assert(!std::convertible_to<quantity<isq::height[m]>, quantity_point<isq::width[m], zeroth_length>>);

static_assert(!std::constructible_from<quantity_point<isq::height[m], zeroth_length>, quantity<isq::width[m]>>);
static_assert(!std::convertible_to<quantity<isq::width[m]>, quantity_point<isq::height[m], zeroth_length>>);

// different dimensions
static_assert(!std::constructible_from<quantity_point<isq::height[m], mean_sea_level>, quantity<si::second>>);
static_assert(!std::convertible_to<quantity<si::second>, quantity_point<isq::height[m], mean_sea_level>>);

// non-convertible quantity_specs
static_assert(!std::constructible_from<quantity_point<isq::height[m], mean_sea_level>, quantity<isq::length[m]>>);
static_assert(!std::convertible_to<quantity<isq::length[m]>, quantity_point<isq::height[m], mean_sea_level>>);

static_assert(!std::constructible_from<quantity_point<special_height[m], mean_sea_level>, quantity<isq::height[m]>>);
static_assert(!std::convertible_to<quantity<special_height[m]>, quantity_point<isq::height[m], mean_sea_level>>);

// not-compatible origin
static_assert(!std::constructible_from<quantity_point<si::metre, mean_sea_level>, quantity<isq::length[m]>>);
static_assert(!std::convertible_to<quantity<isq::length[m]>, quantity_point<si::metre, mean_sea_level>>);

// quantity-like
static_assert(!std::constructible_from<quantity_point<si::second, chrono_point_origin<std::chrono::system_clock>>,
                                       std::chrono::seconds>);
static_assert(!std::convertible_to<std::chrono::seconds,
                                   quantity_point<si::second, chrono_point_origin<std::chrono::system_clock>>>);

static_assert(!std::constructible_from<quantity_point<isq::time[s], chrono_point_origin<std::chrono::system_clock>>,
                                       std::chrono::seconds>);
static_assert(!std::convertible_to<std::chrono::seconds,
                                   quantity_point<isq::time[s], chrono_point_origin<std::chrono::system_clock>>>);

static_assert(
  !std::constructible_from<quantity_point<isq::period_duration[s], chrono_point_origin<std::chrono::system_clock>>,
                           std::chrono::seconds>);
static_assert(
  !std::convertible_to<std::chrono::seconds,
                       quantity_point<isq::period_duration[s], chrono_point_origin<std::chrono::system_clock>>>);


///////////////////////////////////////
// construction from a quantity point
///////////////////////////////////////

// implicit origin
static_assert(std::constructible_from<quantity_point<si::metre>, quantity_point<si::metre>>);
static_assert(std::convertible_to<quantity_point<si::metre>, quantity_point<si::metre>>);
static_assert(std::constructible_from<quantity_point<isq::height[m]>, quantity_point<isq::height[m]>>);
static_assert(std::convertible_to<quantity_point<isq::height[m]>, quantity_point<isq::height[m]>>);

static_assert(std::constructible_from<quantity_point<si::metre>, quantity_point<isq::height[m]>>);
static_assert(std::convertible_to<quantity_point<isq::height[m]>, quantity_point<si::metre>>);
static_assert(std::constructible_from<quantity_point<isq::height[m]>, quantity_point<si::metre>>);
static_assert(std::convertible_to<quantity_point<si::metre>, quantity_point<isq::height[m]>>);

static_assert(
  std::constructible_from<quantity_point<isq::length[m] / isq::time[s]>, quantity_point<isq::speed[m / s]>>);
static_assert(std::convertible_to<quantity_point<isq::speed[m / s]>, quantity_point<isq::length[m] / isq::time[s]>>);

static_assert(std::constructible_from<quantity_point<m / s>, quantity_point<isq::speed[m / s]>>);
static_assert(std::convertible_to<quantity_point<isq::speed[m / s]>, quantity_point<m / s>>);

static_assert(std::constructible_from<quantity_point<m / s>, quantity_point<isq::length[m] / isq::time[s]>>);
static_assert(std::convertible_to<quantity_point<isq::length[m] / isq::time[s]>, quantity_point<m / s>>);

// convertible but different quantity_specs
static_assert(!std::constructible_from<quantity_point<isq::length[m]>, quantity_point<isq::height[m]>>);
static_assert(!std::convertible_to<quantity_point<isq::height[m]>, quantity_point<isq::length[m]>>);

static_assert(!std::constructible_from<quantity_point<isq::height[m]>, quantity_point<special_height[m]>>);
static_assert(!std::convertible_to<quantity_point<special_height[m]>, quantity_point<isq::height[m]>>);

// quantity_specs with common_quantity_spec
static_assert(!std::constructible_from<quantity_point<isq::width[m]>, quantity_point<isq::height[m]>>);
static_assert(!std::convertible_to<quantity_point<isq::height[m]>, quantity_point<isq::width[m]>>);

static_assert(!std::constructible_from<quantity_point<isq::height[m]>, quantity_point<isq::width[m]>>);
static_assert(!std::convertible_to<quantity_point<isq::width[m]>, quantity_point<isq::height[m]>>);

// non-convertible quantity_specs
static_assert(!std::constructible_from<quantity_point<isq::height[m]>, quantity_point<isq::length[m]>>);
static_assert(!std::convertible_to<quantity_point<isq::length[m]>, quantity_point<isq::height[m]>>);

static_assert(!std::constructible_from<quantity_point<special_height[m]>, quantity_point<isq::height[m]>>);
static_assert(!std::convertible_to<quantity_point<isq::height[m]>, quantity_point<special_height[m]>>);

// mixed origins
static_assert(!std::constructible_from<quantity_point<si::metre, mean_sea_level>, quantity_point<si::metre>>);
static_assert(!std::convertible_to<quantity_point<si::metre>, quantity_point<si::metre, mean_sea_level>>);
static_assert(!std::constructible_from<quantity_point<isq::height[m], mean_sea_level>, quantity_point<isq::height[m]>>);
static_assert(!std::convertible_to<quantity_point<isq::height[m]>, quantity_point<isq::height[m], mean_sea_level>>);

static_assert(!std::constructible_from<quantity_point<si::metre>, quantity_point<si::metre, mean_sea_level>>);
static_assert(!std::convertible_to<quantity_point<si::metre, mean_sea_level>, quantity_point<si::metre>>);
static_assert(!std::constructible_from<quantity_point<isq::height[m]>, quantity_point<isq::height[m], mean_sea_level>>);
static_assert(!std::convertible_to<quantity_point<isq::height[m], mean_sea_level>, quantity_point<isq::height[m]>>);

// same explicit origins
static_assert(
  std::constructible_from<quantity_point<si::metre, mean_sea_level>, quantity_point<si::metre, mean_sea_level>>);
static_assert(
  std::convertible_to<quantity_point<si::metre, mean_sea_level>, quantity_point<si::metre, mean_sea_level>>);

static_assert(std::constructible_from<quantity_point<isq::height[m], mean_sea_level>,
                                      quantity_point<isq::height[m], mean_sea_level>>);
static_assert(
  std::convertible_to<quantity_point<isq::height[m], mean_sea_level>, quantity_point<isq::height[m], mean_sea_level>>);

static_assert(std::constructible_from<quantity_point<isq::height[km], mean_sea_level>,
                                      quantity_point<isq::height[m], mean_sea_level>>);
static_assert(
  std::convertible_to<quantity_point<isq::height[m], mean_sea_level>, quantity_point<isq::height[km], mean_sea_level>>);

static_assert(std::constructible_from<quantity_point<isq::height[m], mean_sea_level>,
                                      quantity_point<isq::height[km], mean_sea_level>>);
static_assert(
  std::convertible_to<quantity_point<isq::height[km], mean_sea_level>, quantity_point<isq::height[m], mean_sea_level>>);

static_assert(
  std::constructible_from<quantity_point<si::metre, mean_sea_level>, quantity_point<isq::height[m], mean_sea_level>>);
static_assert(
  std::convertible_to<quantity_point<isq::height[m], mean_sea_level>, quantity_point<si::metre, mean_sea_level>>);

static_assert(
  std::constructible_from<quantity_point<isq::height[m], mean_sea_level>, quantity_point<si::metre, mean_sea_level>>);
static_assert(
  std::convertible_to<quantity_point<si::metre, mean_sea_level>, quantity_point<isq::height[m], mean_sea_level>>);

static_assert(std::constructible_from<quantity_point<isq::height[m], mean_sea_level>,
                                      quantity_point<special_height[m], mean_sea_level>>);
static_assert(std::convertible_to<quantity_point<special_height[m], mean_sea_level>,
                                  quantity_point<isq::height[m], mean_sea_level>>);

static_assert(
  std::constructible_from<quantity_point<si::metre, ground_level>, quantity_point<si::metre, ground_level>>);
static_assert(std::convertible_to<quantity_point<si::metre, ground_level>, quantity_point<si::metre, ground_level>>);

static_assert(
  std::constructible_from<quantity_point<isq::height[m], ground_level>, quantity_point<isq::height[m], ground_level>>);
static_assert(
  std::convertible_to<quantity_point<isq::height[m], ground_level>, quantity_point<isq::height[m], ground_level>>);

static_assert(std::constructible_from<quantity_point<isq::height[km], ground_level>,
                                      quantity_point<isq::height[m], mean_sea_level>>);
static_assert(
  std::convertible_to<quantity_point<isq::height[m], ground_level>, quantity_point<isq::height[km], mean_sea_level>>);

static_assert(
  std::constructible_from<quantity_point<isq::height[m], ground_level>, quantity_point<isq::height[km], ground_level>>);
static_assert(
  std::convertible_to<quantity_point<isq::height[km], ground_level>, quantity_point<isq::height[m], ground_level>>);

static_assert(
  std::constructible_from<quantity_point<si::metre, ground_level>, quantity_point<isq::height[m], ground_level>>);
static_assert(
  std::convertible_to<quantity_point<isq::height[m], ground_level>, quantity_point<si::metre, ground_level>>);

static_assert(
  std::constructible_from<quantity_point<isq::height[m], ground_level>, quantity_point<si::metre, ground_level>>);
static_assert(
  std::convertible_to<quantity_point<si::metre, ground_level>, quantity_point<isq::height[m], ground_level>>);

static_assert(std::constructible_from<quantity_point<isq::height[m], ground_level>,
                                      quantity_point<special_height[m], ground_level>>);
static_assert(
  std::convertible_to<quantity_point<special_height[m], ground_level>, quantity_point<isq::height[m], ground_level>>);

static_assert(
  std::constructible_from<quantity_point<si::metre, mean_sea_level>, quantity_point<si::metre, ground_level>>);
static_assert(std::convertible_to<quantity_point<si::metre, ground_level>, quantity_point<si::metre, mean_sea_level>>);

static_assert(
  std::constructible_from<quantity_point<si::metre, ground_level>, quantity_point<si::metre, mean_sea_level>>);
static_assert(std::convertible_to<quantity_point<si::metre, mean_sea_level>, quantity_point<si::metre, ground_level>>);

static_assert(
  std::constructible_from<quantity_point<si::metre, ground_level>, quantity_point<si::metre, other_ground_level>>);
static_assert(
  std::convertible_to<quantity_point<si::metre, other_ground_level>, quantity_point<si::metre, ground_level>>);

static_assert(
  std::constructible_from<quantity_point<dimensionless[one], zero>, quantity_point<dimensionless[one], zero>>);
static_assert(std::convertible_to<quantity_point<dimensionless[one], zero>, quantity_point<dimensionless[one], zero>>);

static_assert(
  std::constructible_from<quantity_point<dimensionless[one], zero>, quantity_point<dimensionless[percent], zero>>);
static_assert(
  std::convertible_to<quantity_point<dimensionless[percent], zero>, quantity_point<dimensionless[one], zero>>);

static_assert(
  std::constructible_from<quantity_point<dimensionless[percent], zero>, quantity_point<dimensionless[one], zero>>);
static_assert(
  std::convertible_to<quantity_point<dimensionless[one], zero>, quantity_point<dimensionless[percent], zero>>);

static_assert(std::constructible_from<quantity_point<isq::length[m], zeroth_length>,
                                      quantity_point<isq::height[m], zeroth_length>>);
static_assert(
  std::convertible_to<quantity_point<isq::height[m], zeroth_length>, quantity_point<isq::length[m], zeroth_length>>);

// quantity_specs with common_quantity_spec
static_assert(!std::constructible_from<quantity_point<isq::width[m], zeroth_length>,
                                       quantity_point<isq::height[m], zeroth_length>>);
static_assert(
  !std::convertible_to<quantity_point<isq::height[m], zeroth_length>, quantity_point<isq::width[m], zeroth_length>>);

static_assert(!std::constructible_from<quantity_point<isq::height[m], zeroth_length>,
                                       quantity_point<isq::width[m], zeroth_length>>);
static_assert(
  !std::convertible_to<quantity_point<isq::width[m], zeroth_length>, quantity_point<isq::height[m], zeroth_length>>);

// different dimensions
static_assert(
  !std::constructible_from<quantity_point<isq::height[m], mean_sea_level>, quantity_point<si::kelvin, si::ice_point>>);
static_assert(
  !std::convertible_to<quantity_point<si::kelvin, si::ice_point>, quantity_point<isq::height[m], mean_sea_level>>);

// non-convertible quantity_specs
static_assert(!std::constructible_from<quantity_point<special_height[m], mean_sea_level>,
                                       quantity_point<isq::height[m], mean_sea_level>>);
static_assert(!std::convertible_to<quantity_point<isq::height[m], mean_sea_level>,
                                   quantity_point<special_height[m], mean_sea_level>>);

// implicit conversion from another quantity point only if non-truncating
// int -> double OK
static_assert(std::constructible_from<quantity_point<isq::height[m], mean_sea_level>,
                                      quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(std::convertible_to<quantity_point<isq::height[m], mean_sea_level, int>,
                                  quantity_point<isq::height[m], mean_sea_level>>);

// truncating double -> int not allowed
static_assert(!std::constructible_from<quantity_point<isq::height[m], mean_sea_level, int>,
                                       quantity_point<isq::height[m], mean_sea_level>>);
static_assert(!std::convertible_to<quantity_point<isq::height[m], mean_sea_level>,
                                   quantity_point<isq::height[m], mean_sea_level, int>>);

// kilometre<int> -> metre<int> OK
static_assert(std::constructible_from<quantity_point<isq::height[m], mean_sea_level, int>,
                                      quantity_point<isq::height[km], mean_sea_level, int>>);
static_assert(std::convertible_to<quantity_point<isq::height[km], mean_sea_level, int>,
                                  quantity_point<isq::height[m], mean_sea_level, int>>);

// truncating metre<int> -> kilometre<int> not allowed
static_assert(!std::constructible_from<quantity_point<isq::height[km], mean_sea_level, int>,
                                       quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(!std::convertible_to<quantity_point<isq::height[m], mean_sea_level, int>,
                                   quantity_point<isq::height[km], mean_sea_level, int>>);

// converting to double always OK
static_assert(std::constructible_from<quantity_point<isq::height[m], mean_sea_level>,
                                      quantity_point<isq::height[km], mean_sea_level, int>>);
static_assert(std::convertible_to<quantity_point<isq::height[km], mean_sea_level, int>,
                                  quantity_point<isq::height[m], mean_sea_level>>);
static_assert(std::constructible_from<quantity_point<isq::height[km], mean_sea_level>,
                                      quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(std::convertible_to<quantity_point<isq::height[m], mean_sea_level, int>,
                                  quantity_point<isq::height[km], mean_sea_level>>);

// same but not a default origin
static_assert(std::constructible_from<quantity_point<isq::height[m], mean_sea_level>,
                                      quantity_point<isq::height[m], mean_sea_level>>);
static_assert(
  std::convertible_to<quantity_point<isq::height[m], mean_sea_level>, quantity_point<isq::height[m], mean_sea_level>>);

static_assert(
  std::constructible_from<quantity_point<si::metre, mean_sea_level>, quantity_point<isq::height[m], mean_sea_level>>);
static_assert(
  std::convertible_to<quantity_point<isq::height[m], mean_sea_level>, quantity_point<si::metre, mean_sea_level>>);

static_assert(
  std::constructible_from<quantity_point<isq::height[m], mean_sea_level>, quantity_point<si::metre, mean_sea_level>>);
static_assert(
  std::convertible_to<quantity_point<si::metre, mean_sea_level>, quantity_point<isq::height[m], mean_sea_level>>);

static_assert(std::constructible_from<quantity_point<isq::height[m], mean_sea_level>,
                                      quantity_point<special_height[m], mean_sea_level>>);
static_assert(std::convertible_to<quantity_point<special_height[m], mean_sea_level>,
                                  quantity_point<isq::height[m], mean_sea_level>>);

static_assert(!std::constructible_from<quantity_point<special_height[m], mean_sea_level>,
                                       quantity_point<isq::height[m], mean_sea_level>>);
static_assert(!std::convertible_to<quantity_point<isq::height[m], mean_sea_level>,
                                   quantity_point<special_height[m], mean_sea_level>>);

// different origins
static_assert(!std::constructible_from<quantity_point<isq::height[m], mean_sea_level>,
                                       quantity_point<isq::height[m], other_absolute_level>>);
static_assert(!std::convertible_to<quantity_point<isq::height[m], other_absolute_level>,
                                   quantity_point<isq::height[m], mean_sea_level>>);
static_assert(!std::constructible_from<quantity_point<isq::height[m], ground_level>,
                                       quantity_point<isq::height[m], other_absolute_level>>);
static_assert(!std::convertible_to<quantity_point<isq::height[m], other_absolute_level>,
                                   quantity_point<isq::height[m], ground_level>>);
static_assert(!std::constructible_from<quantity_point<isq::height[m], other_absolute_level>,
                                       quantity_point<isq::height[m], ground_level>>);
static_assert(!std::convertible_to<quantity_point<isq::height[m], ground_level>,
                                   quantity_point<isq::height[m], other_absolute_level>>);

// quantity-point-like
static_assert(
  std::constructible_from<quantity_point<isq::time[s], chrono_point_origin<std::chrono::system_clock>>, sys_seconds>);
static_assert(
  std::convertible_to<sys_seconds, quantity_point<isq::time[s], chrono_point_origin<std::chrono::system_clock>>>);

// incompatible origin
static_assert(
  !std::constructible_from<quantity_point<isq::time[s], chrono_point_origin<std::chrono::steady_clock>>, sys_seconds>);
static_assert(
  !std::convertible_to<sys_seconds, quantity_point<isq::time[s], chrono_point_origin<std::chrono::steady_clock>>>);


//////////////////////////////////
// obtaining a relative quantity
//////////////////////////////////

static_assert(quantity_point{42 * m}.quantity_from_zero() == 42 * m);
static_assert(quantity_point{isq::height(42 * m)}.quantity_from_zero() == 42 * m);
static_assert(quantity_point{20 * deg_C}.quantity_from_zero() == 20 * deg_C);
static_assert(quantity_point{20. * deg_C}.in(deg_F).quantity_from_zero() == 68 * deg_F);

static_assert((mean_sea_level + 42 * m).quantity_from_zero() == 42 * m);
static_assert((ground_level + 42 * m).quantity_from_zero() == 84 * m);
static_assert((tower_peak + 42 * m).quantity_from_zero() == 126 * m);

static_assert((mean_sea_level + 42 * m).quantity_from(mean_sea_level) == 42 * m);
static_assert((mean_sea_level + isq::height(42 * m)).quantity_from(mean_sea_level) == 42 * m);

static_assert((zero + 1 * one).quantity_from(zero) == 1 * one);
static_assert((zero + dimensionless(1 * one)).quantity_from(zero) == 1 * one);

static_assert((mean_sea_level + 42 * m).quantity_from(mean_sea_level) == 42 * m);
static_assert((ground_level + 42 * m).quantity_from(ground_level) == 42 * m);
static_assert((tower_peak + 42 * m).quantity_from(tower_peak) == 42 * m);

static_assert(quantity_point<isq::height[m], mean_sea_level>(ground_level + 42 * m).quantity_from(mean_sea_level) ==
              84 * m);
static_assert(quantity_point<isq::height[m], mean_sea_level>(tower_peak + 42 * m).quantity_from(mean_sea_level) ==
              126 * m);

static_assert(quantity_point<isq::height[m], ground_level>(mean_sea_level + 84 * m).quantity_from(ground_level) ==
              42 * m);
static_assert(quantity_point<isq::height[m], ground_level>(tower_peak + 42 * m).quantity_from(ground_level) == 84 * m);

static_assert(quantity_point<isq::height[m], tower_peak>(mean_sea_level + 42 * m).quantity_from(tower_peak) == -42 * m);
static_assert(quantity_point<isq::height[m], tower_peak>(ground_level + 84 * m).quantity_from(tower_peak) == 42 * m);

static_assert((mean_sea_level + 42 * m).point_for(mean_sea_level).quantity_from(mean_sea_level) == 42 * m);
static_assert((ground_level + 42 * m).point_for(mean_sea_level).quantity_from(mean_sea_level) == 84 * m);
static_assert((tower_peak + 42 * m).point_for(mean_sea_level).quantity_from(mean_sea_level) == 126 * m);

static_assert((ground_level + 84 * m).point_for(ground_level).quantity_from(ground_level) == 84 * m);
static_assert((mean_sea_level + 84 * m).point_for(ground_level).quantity_from(ground_level) == 42 * m);
static_assert((tower_peak + 42 * m).point_for(ground_level).quantity_from(ground_level) == 84 * m);

static_assert((tower_peak + 42 * m).point_for(tower_peak).quantity_from(tower_peak) == 42 * m);
static_assert((mean_sea_level + 42 * m).point_for(tower_peak).quantity_from(tower_peak) == -42 * m);
static_assert((ground_level + 84 * m).point_for(tower_peak).quantity_from(tower_peak) == 42 * m);

static_assert(is_of_type<(ground_level + isq::height(short{42} * m)).point_for(mean_sea_level),
                         quantity_point<isq::height[m], mean_sea_level, int>>);


///////////////////////////////////
// converting to a different unit
///////////////////////////////////

static_assert((mean_sea_level + 2. * km).in(km).quantity_from(mean_sea_level).numerical_value_in(km) == 2.);
static_assert((mean_sea_level + 2. * km).in(m).quantity_from(mean_sea_level).numerical_value_in(m) == 2000.);
static_assert((mean_sea_level + 2000. * m).in(km).quantity_from(mean_sea_level).numerical_value_in(km) == 2.);
static_assert((ground_level + 2. * km).in(km).quantity_from(ground_level).numerical_value_in(km) == 2.);
static_assert((ground_level + 2. * km).in(m).quantity_from(ground_level).numerical_value_in(m) == 2000.);
static_assert((ground_level + 2000. * m).in(km).quantity_from(ground_level).numerical_value_in(km) == 2.);
static_assert((tower_peak + 2. * km).in(km).quantity_from(tower_peak).numerical_value_in(km) == 2.);
static_assert((tower_peak + 2. * km).in(m).quantity_from(tower_peak).numerical_value_in(m) == 2000.);
static_assert((tower_peak + 2000. * m).in(km).quantity_from(tower_peak).numerical_value_in(km) == 2.);

template<template<auto, auto, typename> typename QP>
concept invalid_unit_conversion = requires {
  requires !requires { QP<isq::height[m], mean_sea_level, int>(2000 * m).in(km); };  // truncating conversion
  requires !requires { QP<isq::height[m], mean_sea_level, int>(2 * m).in(s); };      // invalid unit
};
static_assert(invalid_unit_conversion<quantity_point>);


/////////
// CTAD
/////////

static_assert(std::is_same_v<decltype(quantity_point{123 * m})::rep, int>);
static_assert(std::is_same_v<std::remove_const_t<decltype(quantity_point{123 * m}.point_origin)>,
                             zeroth_point_origin_<kind_of<isq::length>>>);
static_assert(std::is_same_v<std::remove_const_t<decltype(quantity_point{123 * m}.absolute_point_origin)>,
                             zeroth_point_origin_<kind_of<isq::length>>>);
static_assert(quantity_point{123 * m}.unit == si::metre);
static_assert(quantity_point{123 * m}.quantity_spec == kind_of<isq::length>);

static_assert(std::is_same_v<decltype(quantity_point{isq::height(123 * m)})::rep, int>);
static_assert(std::is_same_v<std::remove_const_t<decltype(quantity_point{isq::height(123 * m)}.point_origin)>,
                             zeroth_point_origin_<isq::height>>);
static_assert(std::is_same_v<std::remove_const_t<decltype(quantity_point{isq::height(123 * m)}.absolute_point_origin)>,
                             zeroth_point_origin_<isq::height>>);
static_assert(quantity_point{isq::height(123 * m)}.unit == si::metre);
static_assert(quantity_point{isq::height(123 * m)}.quantity_spec == isq::height);

static_assert(std::is_same_v<decltype(quantity_point{20 * deg_C})::rep, int>);
static_assert(std::is_same_v<std::remove_const_t<decltype(quantity_point{20 * deg_C}.point_origin)>,
                             struct si::zeroth_degree_Celsius>);
static_assert(std::is_same_v<std::remove_const_t<decltype(quantity_point{20 * deg_C}.absolute_point_origin)>,
                             struct si::zeroth_kelvin>);
static_assert(quantity_point{20 * deg_C}.unit == si::degree_Celsius);
static_assert(quantity_point{20 * deg_C}.quantity_spec == kind_of<isq::thermodynamic_temperature>);

using namespace std::chrono_literals;
static_assert(std::is_same_v<decltype(quantity_point{sys_seconds{123s}})::rep, std::chrono::seconds::rep>);
static_assert(std::is_same_v<std::remove_const_t<decltype(quantity_point{sys_seconds{123s}}.point_origin)>,
                             chrono_point_origin_<std::chrono::system_clock>>);
static_assert(std::is_same_v<std::remove_const_t<decltype(quantity_point{sys_seconds{123s}}.absolute_point_origin)>,
                             chrono_point_origin_<std::chrono::system_clock>>);
static_assert(quantity_point{sys_seconds{24h}}.unit == si::second);
static_assert(quantity_point{sys_seconds{24h}}.quantity_spec == kind_of<isq::time>);


////////////
// getters
////////////

constexpr quantity_point mean_sea_level_qp = mean_sea_level + 1 * m;
constexpr quantity_point my_mean_sea_level_qp = my_mean_sea_level + 1 * m;
constexpr quantity_point ground_level_qp = ground_level + 1 * m;
constexpr quantity_point my_ground_level_qp = my_ground_level + 1 * m;
constexpr quantity_point same_ground_level1_qp = same_ground_level1 + 1 * m;
constexpr quantity_point same_ground_level2_qp = same_ground_level2 + 1 * m;

static_assert(mean_sea_level_qp.quantity_ref_from(mean_sea_level) == 1 * m);
static_assert(mean_sea_level_qp.quantity_ref_from(my_mean_sea_level) == 1 * m);
static_assert(my_mean_sea_level_qp.quantity_ref_from(my_mean_sea_level) == 1 * m);
static_assert(my_mean_sea_level_qp.quantity_ref_from(mean_sea_level) == 1 * m);

static_assert(ground_level_qp.quantity_ref_from(ground_level) == 1 * m);
static_assert(ground_level_qp.quantity_ref_from(my_ground_level) == 1 * m);
static_assert(ground_level_qp.quantity_ref_from(same_ground_level1) == 1 * m);
static_assert(ground_level_qp.quantity_ref_from(same_ground_level2) == 1 * m);
static_assert(my_ground_level_qp.quantity_ref_from(my_ground_level) == 1 * m);
static_assert(my_ground_level_qp.quantity_ref_from(ground_level) == 1 * m);
static_assert(my_ground_level_qp.quantity_ref_from(same_ground_level1) == 1 * m);
static_assert(my_ground_level_qp.quantity_ref_from(same_ground_level2) == 1 * m);
static_assert(same_ground_level1_qp.quantity_ref_from(my_ground_level) == 1 * m);
static_assert(same_ground_level1_qp.quantity_ref_from(ground_level) == 1 * m);
static_assert(same_ground_level1_qp.quantity_ref_from(same_ground_level1) == 1 * m);
static_assert(same_ground_level1_qp.quantity_ref_from(same_ground_level2) == 1 * m);
static_assert(same_ground_level2_qp.quantity_ref_from(my_ground_level) == 1 * m);
static_assert(same_ground_level2_qp.quantity_ref_from(ground_level) == 1 * m);
static_assert(same_ground_level2_qp.quantity_ref_from(same_ground_level1) == 1 * m);
static_assert(same_ground_level2_qp.quantity_ref_from(same_ground_level2) == 1 * m);


////////////////////////
// assignment operator
////////////////////////

static_assert(([]() {
                const quantity_point l1{mean_sea_level + 1 * m};
                quantity_point l2{mean_sea_level + 2 * m};
                return l2 = l1;
              }())
                .quantity_from(mean_sea_level) == 1 * m);
static_assert(([]() {
                const quantity_point l1{mean_sea_level + 1 * m};
                quantity_point l2{mean_sea_level + 2 * m};
                return l2 = l1;
              }())
                .quantity_from(mean_sea_level) == 1 * m);
static_assert(([]() {
                quantity_point l1{mean_sea_level + 1 * m}, l2{mean_sea_level + 2 * m};
                return l2 = std::move(l1);  // NOLINT(*-move-const-arg)
              }())
                .quantity_from(mean_sea_level) == 1 * m);


////////////////////
// unary operators
////////////////////

static_assert([](auto v) {
  auto vv = v++;  // NOLINT(bugprone-inc-dec-in-conditions)
  return std::pair(v, vv);
}(mean_sea_level + 123 * m) == std::pair(mean_sea_level + 124 * m, quantity_point(mean_sea_level + 123 * m)));
static_assert([](auto v) {
  auto vv = ++v;  // NOLINT(bugprone-inc-dec-in-conditions)
  return std::pair(v, vv);
}(mean_sea_level + 123 * m) == std::pair(mean_sea_level + 124 * m, mean_sea_level + 124 * m));
static_assert([](auto v) {
  auto vv = v--;  // NOLINT(bugprone-inc-dec-in-conditions)
  return std::pair(v, vv);
}(mean_sea_level + 123 * m) == std::pair(mean_sea_level + 122 * m, mean_sea_level + 123 * m));
static_assert([](auto v) {
  auto vv = --v;  // NOLINT(bugprone-inc-dec-in-conditions)
  return std::pair(v, vv);
}(mean_sea_level + 123 * m) == std::pair(mean_sea_level + 122 * m, mean_sea_level + 122 * m));


////////////////////////
// compound assignment
////////////////////////

// same type
static_assert((mean_sea_level + 1 * m += 1 * m).quantity_from_zero().numerical_value_in(m) == 2);
static_assert((mean_sea_level + 2 * m -= 1 * m).quantity_from_zero().numerical_value_in(m) == 1);

// different types
static_assert((mean_sea_level + 2.5 * m += 3 * m).quantity_from_zero().numerical_value_in(m) == 5.5);
static_assert((mean_sea_level + 123 * m += 1 * km).quantity_from_zero().numerical_value_in(m) == 1123);
static_assert((mean_sea_level + 5.5 * m -= 3 * m).quantity_from_zero().numerical_value_in(m) == 2.5);
static_assert((mean_sea_level + 1123 * m -= 1 * km).quantity_from_zero().numerical_value_in(m) == 123);


template<template<auto, auto, typename> typename QP>
concept invalid_compound_assignments = requires() {
  // truncating not allowed
  requires !requires(QP<isq::height[m], mean_sea_level, int> l) { l += 2.5 * m; };
  requires !requires(QP<isq::height[m], mean_sea_level, int> l) { l -= 2.5 * m; };
  requires !requires(QP<isq::height[km], mean_sea_level, int> l) { l += 2 * isq::height[m]; };
  requires !requires(QP<isq::height[km], mean_sea_level, int> l) { l -= 2 * isq::height[m]; };

  // only quantities can be added or subtracted
  requires !requires(QP<isq::height[m], mean_sea_level, int> l) { l += 2; };
  requires !requires(QP<isq::height[m], mean_sea_level, int> l) { l -= 2; };

  // no unit constants
  requires !requires(QP<isq::height[m], mean_sea_level, int> l) { l += m; };
  requires !requires(QP<isq::height[m], mean_sea_level, int> l) { l -= m; };
};
static_assert(invalid_compound_assignments<quantity_point>);


////////////////////
// binary operators
////////////////////

template<template<auto, auto, typename> typename QP, auto Origin, auto OtherOrigin>
concept invalid_binary_operations = requires {
  // can't add two quantity points
  requires !requires {
    QP<isq::height[m], mean_sea_level, int>(1 * m) + QP<isq::height[m], mean_sea_level, int>(1 * m);
  };
  requires !requires { mean_sea_level + QP<isq::height[m], mean_sea_level, int>(1 * m); };
  requires !requires { QP<isq::height[m], mean_sea_level, int>(1 * m) + mean_sea_level; };
  requires !requires { Origin + Origin; };

  // can't add more generic quantity (violates point_origin quantity_spec)
  requires !requires { QP<si::metre, mean_sea_level, int>(1 * m) + isq::length(1 * m); };
  requires !requires { isq::length(1 * m) + QP<si::metre, mean_sea_level, int>(1 * m); };
  requires !requires { QP<isq::height[m], mean_sea_level, int>(1 * m) + isq::length(1 * m); };
  requires !requires { isq::length(1 * m) + QP<isq::height[m], mean_sea_level, int>(1 * m); };
  requires !requires { QP<si::metre, zeroth_point_origin<isq::height>, int>(1 * m) + isq::length(1 * m); };
  requires !requires { isq::length(1 * m) + QP<si::metre, zeroth_point_origin<isq::height>, int>(1 * m); };
  requires !requires { QP<isq::height[m], zeroth_point_origin<isq::height>, int>(1 * m) + isq::length(1 * m); };
  requires !requires { isq::length(1 * m) + QP<isq::height[m], zeroth_point_origin<isq::height>, int>(1 * m); };
  requires !requires { Origin + isq::length(1 * m); };
  requires !requires { isq::length(1 * m) + Origin; };

  // can't subtract more generic quantity (violates point_origin quantity_spec)
  requires !requires { QP<si::metre, mean_sea_level, int>(1 * m) - isq::length(1 * m); };
  requires !requires { QP<isq::height[m], mean_sea_level, int>(1 * m) - isq::length(1 * m); };
  requires !requires { QP<si::metre, zeroth_point_origin<isq::height>, int>(1 * m) - isq::length(1 * m); };
  requires !requires { QP<isq::height[m], zeroth_point_origin<isq::height>, int>(1 * m) - isq::length(1 * m); };
  requires !requires {
    QP<isq::height[m] / isq::time[s], zeroth_point_origin<isq::height / isq::time>, int>(10 * isq::height[m] /
                                                                                         (2 * isq::time[s])) +
      5 * isq::speed[m / s];
  };
  requires !requires {
    5 * isq::speed[m / s] + QP<isq::height[m] / isq::time[s], zeroth_point_origin<isq::height / isq::time>, int>(
                              10 * isq::height[m] / (2 * isq::time[s]));
  };
  requires !requires {
    QP<isq::height[m] / isq::time[s], zeroth_point_origin<isq::height / isq::time>, int>(10 * isq::height[m] /
                                                                                         (2 * isq::time[s])) -
      5 * isq::speed[m / s];
  };
  requires !requires { Origin - isq::length(1 * m); };

  // quantity point can't be subtracted from a quantity
  requires !requires { 1 * m - QP<si::metre, mean_sea_level, int>(1 * m); };
  requires !requires { 1 * m - Origin; };

  // no crossdimensional addition and subtraction
  requires !requires { QP<si::metre, mean_sea_level, int>(1 * m) + 1 * s; };
  requires !requires { QP<si::metre, mean_sea_level, int>(1 * m) - 1 * s; };
  requires !requires { Origin + 1 * s; };
  requires !requires { Origin - 1 * s; };

  // can't subtract two quantity points of incompatible origins
  requires !requires {
    QP<isq::height[m], mean_sea_level, int>(1 * m) - QP<isq::height[m], other_absolute_level, int>(1 * m);
  };
  requires !requires {
    QP<isq::height[m], other_absolute_level, int>(1 * m) - QP<isq::height[m], mean_sea_level, int>(1 * m);
  };
  requires !requires { mean_sea_level - QP<isq::height[m], other_absolute_level, int>(1 * m); };
  requires !requires { QP<isq::height[m], mean_sea_level, int>(1 * m) - other_absolute_level; };
  requires !requires { QP<isq::height[m], mean_sea_level, int>(1 * m) - quantity_point{1 * m}; };
  requires !requires { quantity_point{1 * m} - QP<isq::height[m], mean_sea_level, int>(1 * m); };
  requires !requires {
    QP<isq::width[m], zeroth_point_origin<isq::width>, int>(1 * m) - quantity_point{isq::height(1 * m)};
  };
  requires !requires {
    quantity_point{isq::height(1 * m)} - QP<isq::width[m], zeroth_point_origin<isq::width>, int>(1 * m);
  };
  requires !requires {
    QP<isq::width[m], zeroth_point_origin<isq::width>, int>(1 * m) - quantity_point{isq::length(1 * m)};
  };
  requires !requires {
    quantity_point{isq::length(1 * m)} - QP<isq::width[m], zeroth_point_origin<isq::width>, int>(1 * m);
  };
  requires !requires {
    quantity_point{10 * isq::height[m] / (2 * isq::time[s])} -
      QP<isq::speed[m / s], zeroth_point_origin<isq::speed>, int>(5 * isq::speed[m / s]);
  };
  requires !requires {
    QP<isq::speed[m / s], zeroth_point_origin<isq::speed>, int>(5 * isq::speed[m / s]) -
      quantity_point{10 * isq::height[m] / (2 * isq::time[s])};
  };

  // cant'subtract two unrelated points
  requires !requires { Origin - OtherOrigin; };

  // cant'subtract the same point as we do not know the unit for the resulting quantity
  requires !requires { Origin - Origin; };

  // unit constants
  requires !requires { QP<si::metre, mean_sea_level, int>(1) + m; };
  requires !requires { QP<si::metre, mean_sea_level, int>(1) - m; };
  requires !requires { Origin + m; };
  requires !requires { Origin - m; };
  requires !requires { m + QP<si::metre, mean_sea_level, int>(1); };
  requires !requires { m - QP<si::metre, mean_sea_level, int>(1); };
  requires !requires { m + Origin; };
  requires !requires { m - Origin; };
};
static_assert(invalid_binary_operations<quantity_point, mean_sea_level, other_absolute_level>);

// same representation type
static_assert(is_of_type<(mean_sea_level + 1 * m) + 1 * m, quantity_point<si::metre, mean_sea_level, int>>);
static_assert(is_of_type<(mean_sea_level + 1 * m) + 1 * km, quantity_point<si::metre, mean_sea_level, int>>);
static_assert(is_of_type<(mean_sea_level + 1 * km) + 1 * m, quantity_point<si::metre, mean_sea_level, int>>);

static_assert(
  is_of_type<(mean_sea_level + 1 * m) + isq::height(1 * m), quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(
  is_of_type<(mean_sea_level + 1 * m) + isq::height(1 * km), quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(
  is_of_type<(mean_sea_level + 1 * km) + isq::height(1 * m), quantity_point<isq::height[m], mean_sea_level, int>>);

static_assert(
  is_of_type<(mean_sea_level + isq::height(1 * m)) + 1 * m, quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(
  is_of_type<(mean_sea_level + isq::height(1 * m)) + 1 * km, quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(
  is_of_type<(mean_sea_level + isq::height(1 * km)) + 1 * m, quantity_point<isq::height[m], mean_sea_level, int>>);

static_assert(is_of_type<(mean_sea_level + isq::height(1 * m)) + isq::height(1 * m),
                         quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(is_of_type<(mean_sea_level + isq::height(1 * m)) + isq::height(1 * km),
                         quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(is_of_type<(mean_sea_level + isq::height(1 * km)) + isq::height(1 * m),
                         quantity_point<isq::height[m], mean_sea_level, int>>);

static_assert(is_of_type<1 * m + (mean_sea_level + 1 * m), quantity_point<si::metre, mean_sea_level, int>>);
static_assert(is_of_type<1 * m + (mean_sea_level + 1 * km), quantity_point<si::metre, mean_sea_level, int>>);
static_assert(is_of_type<1 * km + (mean_sea_level + 1 * m), quantity_point<si::metre, mean_sea_level, int>>);

static_assert(
  is_of_type<1 * m + (mean_sea_level + isq::height(1 * m)), quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(
  is_of_type<1 * m + (mean_sea_level + isq::height(1 * km)), quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(
  is_of_type<1 * km + (mean_sea_level + isq::height(1 * m)), quantity_point<isq::height[m], mean_sea_level, int>>);

static_assert(
  is_of_type<isq::height(1 * m) + (mean_sea_level + 1 * m), quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(
  is_of_type<isq::height(1 * m) + (mean_sea_level + 1 * km), quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(
  is_of_type<isq::height(1 * km) + (mean_sea_level + 1 * m), quantity_point<isq::height[m], mean_sea_level, int>>);

static_assert(is_of_type<isq::height(1 * m) + (mean_sea_level + isq::height(1 * m)),
                         quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(is_of_type<isq::height(1 * m) + (mean_sea_level + isq::height(1 * km)),
                         quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(is_of_type<isq::height(1 * km) + (mean_sea_level + isq::height(1 * m)),
                         quantity_point<isq::height[m], mean_sea_level, int>>);

static_assert(is_of_type<mean_sea_level + 1 * m, quantity_point<si::metre, mean_sea_level, int>>);
static_assert(is_of_type<mean_sea_level + 1 * km, quantity_point<si::kilo<si::metre>, mean_sea_level, int>>);
static_assert(is_of_type<mean_sea_level + isq::height(1 * m), quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(
  is_of_type<mean_sea_level + special_height(1 * m), quantity_point<special_height[si::metre], mean_sea_level, int>>);
static_assert(is_of_type<mean_sea_level + special_height(1 * km),
                         quantity_point<special_height[si::kilo<si::metre>], mean_sea_level, int>>);
static_assert(is_of_type<1 * m + mean_sea_level, quantity_point<si::metre, mean_sea_level, int>>);
static_assert(is_of_type<1 * km + mean_sea_level, quantity_point<si::kilo<si::metre>, mean_sea_level, int>>);
static_assert(is_of_type<isq::height(1 * m) + mean_sea_level, quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(
  is_of_type<special_height(1 * m) + mean_sea_level, quantity_point<special_height[si::metre], mean_sea_level, int>>);
static_assert(is_of_type<special_height(1 * km) + mean_sea_level,
                         quantity_point<special_height[si::kilo<si::metre>], mean_sea_level, int>>);

static_assert(is_of_type<ground_level + 1 * m, quantity_point<si::metre, ground_level, int>>);
static_assert(is_of_type<ground_level + 1 * km, quantity_point<si::kilo<si::metre>, ground_level, int>>);
static_assert(is_of_type<1 * m + ground_level, quantity_point<si::metre, ground_level, int>>);
static_assert(is_of_type<1 * km + ground_level, quantity_point<si::kilo<si::metre>, ground_level, int>>);

static_assert(is_of_type<(mean_sea_level + 1 * m) - 1 * m, quantity_point<si::metre, mean_sea_level, int>>);
static_assert(is_of_type<(mean_sea_level + 1 * km) - 1 * m, quantity_point<si::metre, mean_sea_level, int>>);
static_assert(is_of_type<(mean_sea_level + 1 * m) - 1 * km, quantity_point<si::metre, mean_sea_level, int>>);

static_assert(
  is_of_type<(mean_sea_level + 1 * m) - isq::height(1 * m), quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(
  is_of_type<(mean_sea_level + 1 * m) - isq::height(1 * km), quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(
  is_of_type<(mean_sea_level + 1 * km) - isq::height(1 * m), quantity_point<isq::height[m], mean_sea_level, int>>);

static_assert(
  is_of_type<(mean_sea_level + isq::height(1 * m)) - 1 * m, quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(
  is_of_type<(mean_sea_level + isq::height(1 * m)) - 1 * km, quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(
  is_of_type<(mean_sea_level + isq::height(1 * km)) - 1 * m, quantity_point<isq::height[m], mean_sea_level, int>>);

static_assert(is_of_type<(mean_sea_level + isq::height(1 * m)) - isq::height(1 * m),
                         quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(is_of_type<(mean_sea_level + isq::height(1 * m)) - isq::height(1 * km),
                         quantity_point<isq::height[m], mean_sea_level, int>>);
static_assert(is_of_type<(mean_sea_level + isq::height(1 * km)) - isq::height(1 * m),
                         quantity_point<isq::height[m], mean_sea_level, int>>);

static_assert(is_of_type<mean_sea_level - 1 * m, quantity_point<si::metre, mean_sea_level, int>>);
static_assert(is_of_type<mean_sea_level - 1 * km, quantity_point<si::kilo<si::metre>, mean_sea_level, int>>);
static_assert(
  is_of_type<mean_sea_level - special_height(1 * m), quantity_point<special_height[si::metre], mean_sea_level, int>>);
static_assert(is_of_type<mean_sea_level - special_height(1 * km),
                         quantity_point<special_height[si::kilo<si::metre>], mean_sea_level, int>>);

static_assert(is_of_type<ground_level - 1 * m, quantity_point<si::metre, ground_level, int>>);
static_assert(is_of_type<ground_level - 1 * km, quantity_point<si::kilo<si::metre>, ground_level, int>>);

static_assert(is_of_type<(mean_sea_level + 1 * m) - (mean_sea_level + 1 * m), quantity<si::metre, int>>);
static_assert(is_of_type<(mean_sea_level + 1 * km) - (mean_sea_level + 1 * m), quantity<si::metre, int>>);
static_assert(is_of_type<(mean_sea_level + 1 * m) - (mean_sea_level + 1 * km), quantity<si::metre, int>>);

static_assert(
  is_of_type<(mean_sea_level + 1 * m) - (mean_sea_level + isq::height(1 * m)), quantity<isq::height[m], int>>);
static_assert(
  is_of_type<(mean_sea_level + 1 * m) - (mean_sea_level + isq::height(1 * km)), quantity<isq::height[m], int>>);
static_assert(
  is_of_type<(mean_sea_level + 1 * km) - (mean_sea_level + isq::height(1 * m)), quantity<isq::height[m], int>>);

static_assert(
  is_of_type<(mean_sea_level + isq::height(1 * m)) - (mean_sea_level + 1 * m), quantity<isq::height[m], int>>);
static_assert(
  is_of_type<(mean_sea_level + isq::height(1 * m)) - (mean_sea_level + 1 * km), quantity<isq::height[m], int>>);
static_assert(
  is_of_type<(mean_sea_level + isq::height(1 * km)) - (mean_sea_level + 1 * m), quantity<isq::height[m], int>>);

static_assert(is_of_type<(mean_sea_level + isq::height(1 * m)) - (mean_sea_level + isq::height(1 * m)),
                         quantity<isq::height[m], int>>);
static_assert(is_of_type<(mean_sea_level + isq::height(1 * m)) - (mean_sea_level + isq::height(1 * km)),
                         quantity<isq::height[m], int>>);
static_assert(is_of_type<(mean_sea_level + isq::height(1 * km)) - (mean_sea_level + isq::height(1 * m)),
                         quantity<isq::height[m], int>>);

static_assert(is_of_type<(mean_sea_level + 1 * m) - (mean_sea_level + 1 * m), quantity<si::metre, int>>);
static_assert(is_of_type<(ground_level + 1 * m) - (ground_level + 1 * m), quantity<si::metre, int>>);
static_assert(is_of_type<(tower_peak + 1 * m) - (tower_peak + 1 * m), quantity<si::metre, int>>);
static_assert(is_of_type<(mean_sea_level + 1 * m) - (ground_level + 1 * m), quantity<isq::height[m], int>>);
static_assert(is_of_type<(ground_level + 1 * m) - (mean_sea_level + 1 * m), quantity<isq::height[m], int>>);
static_assert(is_of_type<(tower_peak + 1 * m) - (ground_level + 1 * m), quantity<isq::height[m], int>>);
static_assert(is_of_type<(ground_level + 1 * m) - (tower_peak + 1 * m), quantity<isq::height[m], int>>);
static_assert(is_of_type<(tower_peak + 1 * m) - (mean_sea_level + 1 * m), quantity<isq::height[m], int>>);
static_assert(is_of_type<(mean_sea_level + 1 * m) - (tower_peak + 1 * m), quantity<isq::height[m], int>>);
static_assert(is_of_type<(other_ground_level + 1 * m) - (ground_level + 1 * m), quantity<isq::height[m], int>>);
static_assert(is_of_type<(ground_level + 1 * m) - (other_ground_level + 1 * m), quantity<isq::height[m], int>>);
static_assert(is_of_type<(other_ground_level + 1 * m) - (tower_peak + 1 * m), quantity<isq::height[m], int>>);
static_assert(is_of_type<(tower_peak + 1 * m) - (other_ground_level + 1 * m), quantity<isq::height[m], int>>);

static_assert(is_of_type<mean_sea_level - (mean_sea_level + 1 * m), quantity<si::metre, int>>);
static_assert(is_of_type<mean_sea_level - (mean_sea_level + 1 * km), quantity<si::kilo<si::metre>, int>>);
static_assert(is_of_type<(mean_sea_level + 1 * m) - mean_sea_level, quantity<si::metre, int>>);
static_assert(is_of_type<(mean_sea_level + 1 * km) - mean_sea_level, quantity<si::kilo<si::metre>, int>>);

static_assert(is_of_type<ground_level - (ground_level + 1 * m), quantity<si::metre, int>>);
static_assert(is_of_type<ground_level - (ground_level + 1 * km), quantity<si::kilo<si::metre>, int>>);
static_assert(is_of_type<(ground_level + 1 * m) - ground_level, quantity<si::metre, int>>);
static_assert(is_of_type<(ground_level + 1 * km) - ground_level, quantity<si::kilo<si::metre>, int>>);

static_assert(is_of_type<mean_sea_level - (ground_level + 1 * m), quantity<isq::height[m], int>>);
static_assert(is_of_type<mean_sea_level - (ground_level + 1 * km), quantity<isq::height[m], int>>);
static_assert(is_of_type<(ground_level + 1 * m) - mean_sea_level, quantity<isq::height[m], int>>);
static_assert(is_of_type<(ground_level + 1 * km) - mean_sea_level, quantity<isq::height[m], int>>);

static_assert(is_of_type<ground_level - (mean_sea_level + 1 * m), quantity<isq::height[m], int>>);
static_assert(is_of_type<ground_level - (mean_sea_level + 1 * km), quantity<isq::height[m], int>>);
static_assert(is_of_type<(mean_sea_level + 1 * m) - ground_level, quantity<isq::height[m], int>>);
static_assert(is_of_type<(mean_sea_level + 1 * km) - ground_level, quantity<isq::height[m], int>>);

static_assert(is_of_type<ground_level - ground_level, quantity<isq::height[m], int>>);
static_assert(is_of_type<mean_sea_level - ground_level, quantity<isq::height[m], int>>);
static_assert(is_of_type<ground_level - mean_sea_level, quantity<isq::height[m], int>>);
static_assert(is_of_type<ground_level - tower_peak, quantity<isq::height[m], int>>);
static_assert(is_of_type<tower_peak - ground_level, quantity<isq::height[m], int>>);

static_assert(is_of_type<(1 * m + mean_sea_level) - (1 * m + ground_level), quantity<isq::height[m], int>>);
static_assert(is_of_type<(1 * m + ground_level) - (1 * m + mean_sea_level), quantity<isq::height[m], int>>);
static_assert(is_of_type<(1 * m + tower_peak) - (1 * m + ground_level), quantity<isq::height[m], int>>);
static_assert(is_of_type<(1 * m + ground_level) - (1 * m + tower_peak), quantity<isq::height[m], int>>);
static_assert(is_of_type<(1 * m + tower_peak) - (1 * m + mean_sea_level), quantity<isq::height[m], int>>);
static_assert(is_of_type<(1 * m + mean_sea_level) - (1 * m + tower_peak), quantity<isq::height[m], int>>);
static_assert(is_of_type<(1 * m + other_ground_level) - (1 * m + ground_level), quantity<isq::height[m], int>>);
static_assert(is_of_type<(1 * m + ground_level) - (1 * m + other_ground_level), quantity<isq::height[m], int>>);
static_assert(is_of_type<(1 * m + other_ground_level) - (1 * m + tower_peak), quantity<isq::height[m], int>>);
static_assert(is_of_type<(1 * m + tower_peak) - (1 * m + other_ground_level), quantity<isq::height[m], int>>);


// check for integral types promotion
static_assert(is_same_v<decltype(((mean_sea_level + std::uint8_t{0} * m) + std::uint8_t{0} * m)
                                   .quantity_from(mean_sea_level)
                                   .numerical_value_in(m)),
                        int>);
static_assert(is_same_v<decltype((std::uint8_t{0} * m + (mean_sea_level + std::uint8_t{0} * m))
                                   .quantity_from(mean_sea_level)
                                   .numerical_value_in(m)),
                        int>);
static_assert(is_same_v<decltype(((mean_sea_level + std::uint8_t{0} * m) - std::uint8_t{0} * m)
                                   .quantity_from(mean_sea_level)
                                   .numerical_value_in(m)),
                        int>);
static_assert(is_same_v<decltype(((mean_sea_level + std::uint8_t{0} * m) - (mean_sea_level + std::uint8_t{0} * m))
                                   .numerical_value_in(m)),
                        int>);
static_assert(((mean_sea_level + std::uint8_t{128} * m) + std::uint8_t{128} * m)
                .quantity_from(mean_sea_level)
                .numerical_value_in(m) == std::uint8_t{128} + std::uint8_t{128});
static_assert((std::uint8_t{128} * m + (mean_sea_level + std::uint8_t{128} * m))
                .quantity_from(mean_sea_level)
                .numerical_value_in(m) == std::uint8_t{128} + std::uint8_t{128});
static_assert(
  ((mean_sea_level + std::uint8_t{0} * m) - std::uint8_t{1} * m).quantity_from(mean_sea_level).numerical_value_in(m) ==
  std::uint8_t{0} - std::uint8_t{1});
static_assert(((mean_sea_level + std::uint8_t{0} * m) - (mean_sea_level + std::uint8_t{1} * m)).numerical_value_in(m) ==
              std::uint8_t{0} - std::uint8_t{1});

// different representation types
static_assert(is_of_type<(mean_sea_level + 1. * m) + 1 * m, quantity_point<si::metre, mean_sea_level, double>>);
static_assert(is_of_type<1. * m + (mean_sea_level + 1 * m), quantity_point<si::metre, mean_sea_level, double>>);
static_assert(is_of_type<(mean_sea_level + 1 * m) + 1. * km, quantity_point<si::metre, mean_sea_level, double>>);
static_assert(is_of_type<1 * m + (mean_sea_level + 1. * km), quantity_point<si::metre, mean_sea_level, double>>);
static_assert(is_of_type<(mean_sea_level + 1 * km) + 1. * m, quantity_point<si::metre, mean_sea_level, double>>);
static_assert(is_of_type<1 * km + (mean_sea_level + 1. * m), quantity_point<si::metre, mean_sea_level, double>>);

static_assert(is_of_type<(mean_sea_level + 1 * m) - 1. * m, quantity_point<si::metre, mean_sea_level, double>>);
static_assert(is_of_type<(mean_sea_level + 1. * km) - 1 * m, quantity_point<si::metre, mean_sea_level, double>>);
static_assert(is_of_type<(mean_sea_level + 1. * m) - 1 * km, quantity_point<si::metre, mean_sea_level, double>>);

// different units
static_assert(is_of_type<(mean_sea_level + 1 * m) + 1 * km, quantity_point<si::metre, mean_sea_level, int>>);
static_assert(is_of_type<1 * m + (mean_sea_level + 1 * km), quantity_point<si::metre, mean_sea_level, int>>);
static_assert(is_of_type<(mean_sea_level + 1. * m) + 1 * km, quantity_point<si::metre, mean_sea_level, double>>);
static_assert(is_of_type<1. * m + (mean_sea_level + 1 * km), quantity_point<si::metre, mean_sea_level, double>>);
static_assert(is_of_type<(mean_sea_level + 1 * m) + 1. * km, quantity_point<si::metre, mean_sea_level, double>>);
static_assert(is_of_type<1 * m + (mean_sea_level + 1. * km), quantity_point<si::metre, mean_sea_level, double>>);
static_assert(is_of_type<(mean_sea_level + 1. * m) + 1. * km, quantity_point<si::metre, mean_sea_level, double>>);
static_assert(is_of_type<1. * m + (mean_sea_level + 1. * km), quantity_point<si::metre, mean_sea_level, double>>);

static_assert(is_of_type<(mean_sea_level + 1 * km) + 1 * m, quantity_point<si::metre, mean_sea_level, int>>);
static_assert(is_of_type<1 * km + (mean_sea_level + 1 * m), quantity_point<si::metre, mean_sea_level, int>>);
static_assert(is_of_type<(mean_sea_level + 1. * km) + 1 * m, quantity_point<si::metre, mean_sea_level, double>>);
static_assert(is_of_type<1. * km + (mean_sea_level + 1 * m), quantity_point<si::metre, mean_sea_level, double>>);
static_assert(is_of_type<(mean_sea_level + 1 * km) + 1. * m, quantity_point<si::metre, mean_sea_level, double>>);
static_assert(is_of_type<1 * km + (mean_sea_level + 1. * m), quantity_point<si::metre, mean_sea_level, double>>);
static_assert(is_of_type<(mean_sea_level + 1. * km) + 1. * m, quantity_point<si::metre, mean_sea_level, double>>);
static_assert(is_of_type<1. * km + (mean_sea_level + 1. * m), quantity_point<si::metre, mean_sea_level, double>>);

static_assert(is_of_type<(mean_sea_level + 1 * m) - 1 * km, quantity_point<si::metre, mean_sea_level, int>>);
static_assert(is_of_type<(mean_sea_level + 1. * m) - 1 * km, quantity_point<si::metre, mean_sea_level, double>>);
static_assert(is_of_type<(mean_sea_level + 1 * m) - 1. * km, quantity_point<si::metre, mean_sea_level, double>>);
static_assert(is_of_type<(mean_sea_level + 1. * m) - 1. * km, quantity_point<si::metre, mean_sea_level, double>>);

static_assert(is_of_type<(mean_sea_level + 1 * km) - 1 * m, quantity_point<si::metre, mean_sea_level, int>>);
static_assert(is_of_type<(mean_sea_level + 1. * km) - 1 * m, quantity_point<si::metre, mean_sea_level, double>>);
static_assert(is_of_type<(mean_sea_level + 1 * km) - 1. * m, quantity_point<si::metre, mean_sea_level, double>>);
static_assert(is_of_type<(mean_sea_level + 1. * km) - 1. * m, quantity_point<si::metre, mean_sea_level, double>>);

static_assert(is_of_type<(mean_sea_level + 1 * m) - (mean_sea_level + 1 * km), quantity<si::metre, int>>);
static_assert(is_of_type<(mean_sea_level + 1. * m) - (mean_sea_level + 1 * km), quantity<si::metre, double>>);
static_assert(is_of_type<(mean_sea_level + 1 * m) - (mean_sea_level + 1. * km), quantity<si::metre, double>>);
static_assert(is_of_type<(mean_sea_level + 1. * m) - (mean_sea_level + 1. * km), quantity<si::metre, double>>);

static_assert(is_of_type<(mean_sea_level + 1 * km) - (mean_sea_level + 1 * m), quantity<si::metre, int>>);
static_assert(is_of_type<(mean_sea_level + 1. * km) - (mean_sea_level + 1 * m), quantity<si::metre, double>>);
static_assert(is_of_type<(mean_sea_level + 1 * km) - (mean_sea_level + 1. * m), quantity<si::metre, double>>);
static_assert(is_of_type<(mean_sea_level + 1. * km) - (mean_sea_level + 1. * m), quantity<si::metre, double>>);


static_assert(((mean_sea_level + 1 * m) + 1 * m).quantity_from(mean_sea_level).numerical_value_in(m) == 2);
static_assert((1 * m + (mean_sea_level + 1 * m)).quantity_from(mean_sea_level).numerical_value_in(m) == 2);
static_assert(((mean_sea_level + 1 * m) + 1 * km).quantity_from(mean_sea_level).numerical_value_in(m) == 1001);
static_assert((1 * m + (mean_sea_level + 1 * km)).quantity_from(mean_sea_level).numerical_value_in(m) == 1001);
static_assert(((mean_sea_level + 1 * km) + 1 * m).quantity_from(mean_sea_level).numerical_value_in(m) == 1001);
static_assert((1 * km + (mean_sea_level + 1 * m)).quantity_from(mean_sea_level).numerical_value_in(m) == 1001);
static_assert(((mean_sea_level + 2 * m) - 1 * m).quantity_from(mean_sea_level).numerical_value_in(m) == 1);
static_assert(((mean_sea_level + 1 * km) - 1 * m).quantity_from(mean_sea_level).numerical_value_in(m) == 999);

static_assert(((mean_sea_level + 1.5 * m) + 1 * m).quantity_from(mean_sea_level).numerical_value_in(m) == 2.5);
static_assert((1.5 * m + (mean_sea_level + 1 * m)).quantity_from(mean_sea_level).numerical_value_in(m) == 2.5);
static_assert(((mean_sea_level + 1.5 * m) + 1 * km).quantity_from(mean_sea_level).numerical_value_in(m) == 1001.5);
static_assert((1.5 * m + (mean_sea_level + 1 * km)).quantity_from(mean_sea_level).numerical_value_in(m) == 1001.5);
static_assert(((mean_sea_level + 1.5 * km) + 1 * m).quantity_from(mean_sea_level).numerical_value_in(m) == 1501);
static_assert((1.5 * km + (mean_sea_level + 1 * m)).quantity_from(mean_sea_level).numerical_value_in(m) == 1501);
static_assert(((mean_sea_level + 2.5 * m) - 1 * m).quantity_from(mean_sea_level).numerical_value_in(m) == 1.5);
static_assert(((mean_sea_level + 1.5 * km) - 1 * m).quantity_from(mean_sea_level).numerical_value_in(m) == 1499);

static_assert(((mean_sea_level + 1 * m) + 1.5 * m).quantity_from(mean_sea_level).numerical_value_in(m) == 2.5);
static_assert((1 * m + (mean_sea_level + 1.5 * m)).quantity_from(mean_sea_level).numerical_value_in(m) == 2.5);
static_assert(((mean_sea_level + 1 * m) + 1.5 * km).quantity_from(mean_sea_level).numerical_value_in(m) == 1501);
static_assert((1 * m + (mean_sea_level + 1.5 * km)).quantity_from(mean_sea_level).numerical_value_in(m) == 1501);
static_assert(((mean_sea_level + 1 * km) + 1.5 * m).quantity_from(mean_sea_level).numerical_value_in(m) == 1001.5);
static_assert((1 * km + (mean_sea_level + 1.5 * m)).quantity_from(mean_sea_level).numerical_value_in(m) == 1001.5);
static_assert(((mean_sea_level + 2 * m) - 1.5 * m).quantity_from(mean_sea_level).numerical_value_in(m) == 0.5);
static_assert(((mean_sea_level + 1 * km) - 1.5 * m).quantity_from(mean_sea_level).numerical_value_in(m) == 998.5);

static_assert(((mean_sea_level + 2 * m) - (mean_sea_level + 1 * m)).numerical_value_in(m) == 1);
static_assert(((mean_sea_level + 1 * km) - (mean_sea_level + 1 * m)).numerical_value_in(m) == 999);
static_assert(((mean_sea_level + 2.5 * m) - (mean_sea_level + 1 * m)).numerical_value_in(m) == 1.5);
static_assert(((mean_sea_level + 1.5 * km) - (mean_sea_level + 1 * m)).numerical_value_in(m) == 1499);
static_assert(((mean_sea_level + 2 * m) - (mean_sea_level + 1.5 * m)).numerical_value_in(m) == 0.5);
static_assert(((mean_sea_level + 1 * km) - (mean_sea_level + 1.5 * m)).numerical_value_in(m) == 998.5);

static_assert((mean_sea_level + 42 * m) - (ground_level + 42 * m) == -42 * m);
static_assert((ground_level + 42 * m) - (mean_sea_level + 42 * m) == 42 * m);
static_assert((tower_peak + 42 * m) - (ground_level + 42 * m) == 42 * m);
static_assert((ground_level + 42 * m) - (tower_peak + 42 * m) == -42 * m);
static_assert((tower_peak + 42 * m) - (mean_sea_level + 42 * m) == 84 * m);
static_assert((mean_sea_level + 42 * m) - (tower_peak + 42 * m) == -84 * m);
static_assert((other_ground_level + 42 * m) - (ground_level + 42 * m) == 81 * m);
static_assert((ground_level + 42 * m) - (other_ground_level + 42 * m) == -81 * m);
static_assert((other_ground_level + 42 * m) - (tower_peak + 42 * m) == 39 * m);
static_assert((tower_peak + 42 * m) - (other_ground_level + 42 * m) == -39 * m);

static_assert((mean_sea_level + 42 * m).quantity_from(mean_sea_level) == 42 * m);
static_assert((42 * m + mean_sea_level).quantity_from(mean_sea_level) == 42 * m);
static_assert((mean_sea_level - 42 * m).quantity_from(mean_sea_level) == -42 * m);
static_assert((ground_level + 42 * m).quantity_from(ground_level) == 42 * m);
static_assert((42 * m + ground_level).quantity_from(ground_level) == 42 * m);
static_assert((ground_level - 42 * m).quantity_from(ground_level) == -42 * m);
static_assert((tower_peak + 42 * m).quantity_from(tower_peak) == 42 * m);
static_assert((42 * m + tower_peak).quantity_from(tower_peak) == 42 * m);
static_assert((tower_peak - 42 * m).quantity_from(tower_peak) == -42 * m);

static_assert((mean_sea_level + 42 * m) - ground_level == 0 * m);
static_assert((ground_level + 42 * m) - mean_sea_level == 84 * m);
static_assert((tower_peak + 42 * m) - ground_level == 84 * m);
static_assert((ground_level + 42 * m) - tower_peak == 0 * m);
static_assert((tower_peak + 42 * m) - mean_sea_level == 126 * m);
static_assert((mean_sea_level + 42 * m) - tower_peak == -42 * m);
static_assert((other_ground_level + 42 * m) - ground_level == 123 * m);
static_assert((ground_level + 42 * m) - other_ground_level == -39 * m);
static_assert((other_ground_level + 42 * m) - tower_peak == 81 * m);
static_assert((tower_peak + 42 * m) - other_ground_level == 3 * m);

static_assert((mean_sea_level + 42 * m).quantity_from(ground_level) == 0 * m);
static_assert((ground_level + 42 * m).quantity_from(mean_sea_level) == 84 * m);
static_assert((tower_peak + 42 * m).quantity_from(ground_level) == 84 * m);
static_assert((ground_level + 42 * m).quantity_from(tower_peak) == 0 * m);
static_assert((tower_peak + 42 * m).quantity_from(mean_sea_level) == 126 * m);
static_assert((mean_sea_level + 42 * m).quantity_from(tower_peak) == -42 * m);
static_assert((other_ground_level + 42 * m).quantity_from(ground_level) == 123 * m);
static_assert((ground_level + 42 * m).quantity_from(other_ground_level) == -39 * m);
static_assert((other_ground_level + 42 * m).quantity_from(tower_peak) == 81 * m);
static_assert((tower_peak + 42 * m).quantity_from(other_ground_level) == 3 * m);

static_assert(mean_sea_level - (ground_level + 42 * m) == -84 * m);
static_assert(ground_level - (mean_sea_level + 42 * m) == 0 * m);
static_assert(tower_peak - (ground_level + 42 * m) == 0 * m);
static_assert(ground_level - (tower_peak + 42 * m) == -84 * m);
static_assert(tower_peak - (mean_sea_level + 42 * m) == 42 * m);
static_assert(mean_sea_level - (tower_peak + 42 * m) == -126 * m);
static_assert(other_ground_level - (ground_level + 42 * m) == 39 * m);
static_assert(ground_level - (other_ground_level + 42 * m) == -123 * m);
static_assert(other_ground_level - (tower_peak + 42 * m) == -3 * m);
static_assert(tower_peak - (other_ground_level + 42 * m) == -81 * m);

static_assert(mean_sea_level - ground_level == -42 * m);
static_assert(ground_level - mean_sea_level == 42 * m);
static_assert(tower_peak - ground_level == 42 * m);
static_assert(ground_level - tower_peak == -42 * m);
static_assert(tower_peak - mean_sea_level == 84 * m);
static_assert(mean_sea_level - tower_peak == -84 * m);
static_assert(other_ground_level - ground_level == 81 * m);
static_assert(ground_level - other_ground_level == -81 * m);
static_assert(other_ground_level - tower_peak == 39 * m);
static_assert(tower_peak - other_ground_level == -39 * m);

inline constexpr struct zero_m_per_s : absolute_point_origin<zero_m_per_s, kind_of<isq::speed>> {
} zero_m_per_s;

// commutativity and associativity
static_assert(((zero_m_per_s + 10 * isq::height[m] / (2 * isq::time[s])) + 5 * isq::speed[m / s])
                .quantity_from(zero_m_per_s) == 10 * isq::speed[m / s]);
static_assert((10 * isq::height[m] / (2 * isq::time[s]) + (zero_m_per_s + 5 * isq::speed[m / s]))
                .quantity_from(zero_m_per_s) == 10 * isq::speed[m / s]);
static_assert(((zero_m_per_s + 5 * isq::speed[m / s]) + 10 * isq::height[m] / (2 * isq::time[s]))
                .quantity_from(zero_m_per_s) == 10 * isq::speed[m / s]);
static_assert((5 * isq::speed[m / s] + (zero_m_per_s + 10 * isq::height[m] / (2 * isq::time[s])))
                .quantity_from(zero_m_per_s) == 10 * isq::speed[m / s]);
static_assert(((zero_m_per_s + 10 * isq::height[m] / (2 * isq::time[s])) - 5 * isq::speed[m / s])
                .quantity_from(zero_m_per_s) == 0 * isq::speed[m / s]);
static_assert(((zero_m_per_s + 5 * isq::speed[m / s]) - 10 * isq::height[m] / (2 * isq::time[s]))
                .quantity_from(zero_m_per_s) == 0 * isq::speed[m / s]);
static_assert((zero_m_per_s + 10 * isq::height[m] / (2 * isq::time[s])) - (zero_m_per_s + 5 * isq::speed[m / s]) ==
              0 * isq::speed[m / s]);
static_assert((zero_m_per_s + 5 * isq::speed[m / s]) - (zero_m_per_s + 10 * isq::height[m] / (2 * isq::time[s])) ==
              0 * isq::speed[m / s]);

static_assert(is_of_type<(zero_m_per_s + 10 * isq::height[m] / (2 * isq::time[s])) + 5 * isq::speed[m / s],
                         quantity_point<isq::speed[m / s], zero_m_per_s, int>>);
static_assert(is_of_type<10 * isq::height[m] / (2 * isq::time[s]) + (zero_m_per_s + 5 * isq::speed[m / s]),
                         quantity_point<isq::speed[m / s], zero_m_per_s, int>>);
static_assert(is_of_type<(zero_m_per_s + 5 * isq::speed[m / s]) + 10 * isq::height[m] / (2 * isq::time[s]),
                         quantity_point<isq::speed[m / s], zero_m_per_s, int>>);
static_assert(is_of_type<5 * isq::speed[m / s] + (zero_m_per_s + 10 * isq::height[m] / (2 * isq::time[s])),
                         quantity_point<isq::speed[m / s], zero_m_per_s, int>>);
static_assert(is_of_type<(zero_m_per_s + 10 * isq::height[m] / (2 * isq::time[s])) - 5 * isq::speed[m / s],
                         quantity_point<isq::speed[m / s], zero_m_per_s, int>>);
static_assert(is_of_type<(zero_m_per_s + 5 * isq::speed[m / s]) - 10 * isq::height[m] / (2 * isq::time[s]),
                         quantity_point<isq::speed[m / s], zero_m_per_s, int>>);
static_assert(
  is_of_type<(zero_m_per_s + 10 * isq::height[m] / (2 * isq::time[s])) - (zero_m_per_s + 5 * isq::speed[m / s]),
             quantity<isq::speed[m / s], int>>);
static_assert(
  is_of_type<(zero_m_per_s + 5 * isq::speed[m / s]) - (zero_m_per_s + 10 * isq::height[m] / (2 * isq::time[s])),
             quantity<isq::speed[m / s], int>>);

static_assert(
  is_of_type<(zero_m_per_s + 10 * isq::height[m] / (2 * isq::time[s])) + (10 * isq::height[m] / (2 * isq::time[s])),
             quantity_point<(isq::height / isq::time)[m / s], zero_m_per_s, int>>);

static_assert((quantity_point{5 * isq::speed[m / s]} + 10 * isq::length[m] / (2 * isq::time[s])).quantity_from_zero() ==
              10 * isq::speed[m / s]);
static_assert((10 * isq::length[m] / (2 * isq::time[s]) + quantity_point{5 * isq::speed[m / s]}).quantity_from_zero() ==
              10 * isq::speed[m / s]);
static_assert((quantity_point{5 * isq::speed[m / s]} - 10 * isq::length[m] / (2 * isq::time[s])).quantity_from_zero() ==
              0 * isq::speed[m / s]);

static_assert((quantity_point{10 * isq::length[m] / (2 * isq::time[s])} + 5 * isq::speed[m / s]).quantity_from_zero() ==
              10 * isq::speed[m / s]);
static_assert((5 * isq::speed[m / s] + quantity_point{10 * isq::length[m] / (2 * isq::time[s])}).quantity_from_zero() ==
              10 * isq::speed[m / s]);
static_assert((quantity_point{10 * isq::length[m] / (2 * isq::time[s])} - 5 * isq::speed[m / s]).quantity_from_zero() ==
              0 * isq::speed[m / s]);

static_assert((quantity_point{5 * isq::speed[m / s]} + 10 * isq::height[m] / (2 * isq::time[s])).quantity_from_zero() ==
              10 * isq::speed[m / s]);
static_assert((10 * isq::height[m] / (2 * isq::time[s]) + quantity_point{5 * isq::speed[m / s]}).quantity_from_zero() ==
              10 * isq::speed[m / s]);
static_assert((quantity_point{5 * isq::speed[m / s]} - 10 * isq::height[m] / (2 * isq::time[s])).quantity_from_zero() ==
              0 * isq::speed[m / s]);

static_assert(is_of_type<quantity_point{10 * isq::length[m] / (2 * isq::time[s])} + 5 * isq::speed[m / s],
                         quantity_point<isq::speed[m / s], zeroth_point_origin<isq::speed>, int>>);
static_assert(is_of_type<10 * isq::height[m] / (2 * isq::time[s]) + quantity_point{5 * isq::speed[m / s]},
                         quantity_point<isq::speed[m / s], zeroth_point_origin<isq::speed>, int>>);
static_assert(is_of_type<quantity_point{5 * isq::speed[m / s]} + 10 * isq::height[m] / (2 * isq::time[s]),
                         quantity_point<isq::speed[m / s], zeroth_point_origin<isq::speed>, int>>);
static_assert(is_of_type<5 * isq::speed[m / s] + quantity_point{10 * isq::length[m] / (2 * isq::time[s])},
                         quantity_point<isq::speed[m / s], zeroth_point_origin<isq::speed>, int>>);
static_assert(is_of_type<quantity_point{10 * isq::length[m] / (2 * isq::time[s])} - 5 * isq::speed[m / s],
                         quantity_point<isq::speed[m / s], zeroth_point_origin<isq::speed>, int>>);
static_assert(is_of_type<quantity_point{5 * isq::speed[m / s]} - 10 * isq::height[m] / (2 * isq::time[s]),
                         quantity_point<isq::speed[m / s], zeroth_point_origin<isq::speed>, int>>);
static_assert(
  is_of_type<quantity_point{10 * isq::length[m] / (2 * isq::time[s])} - quantity_point{5 * isq::speed[m / s]},
             quantity<isq::speed[m / s], int>>);
static_assert(
  is_of_type<quantity_point{5 * isq::speed[m / s]} - quantity_point{10 * isq::length[m] / (2 * isq::time[s])},
             quantity<isq::speed[m / s], int>>);

static_assert(
  is_of_type<quantity_point{10 * isq::height[m] / (2 * isq::time[s])} + (10 * isq::height[m] / (2 * isq::time[s])),
             quantity_point<(isq::height / isq::time)[m / s], zeroth_point_origin<isq::height / isq::time>, int>>);

inline constexpr struct zero_Hz : absolute_point_origin<zero_Hz, kind_of<isq::frequency>> {
} zero_Hz;

static_assert(((zero_Hz + 10 / (2 * isq::period_duration[s])) + 5 * isq::frequency[Hz]).quantity_from(zero_Hz) ==
              10 * isq::frequency[Hz]);
static_assert((10 / (2 * isq::period_duration[s]) + (zero_Hz + 5 * isq::frequency[Hz])).quantity_from(zero_Hz) ==
              10 * isq::frequency[Hz]);
static_assert(((zero_Hz + 5 * isq::frequency[Hz]) + 10 / (2 * isq::period_duration[s])).quantity_from(zero_Hz) ==
              10 * isq::frequency[Hz]);
static_assert((5 * isq::frequency[Hz] + (zero_Hz + 10 / (2 * isq::period_duration[s]))).quantity_from(zero_Hz) ==
              10 * isq::frequency[Hz]);
static_assert(((zero_Hz + 10 / (2 * isq::period_duration[s])) - 5 * isq::frequency[Hz]).quantity_from(zero_Hz) ==
              0 * isq::frequency[Hz]);
static_assert(((zero_Hz + 5 * isq::frequency[Hz]) - 10 / (2 * isq::period_duration[s])).quantity_from(zero_Hz) ==
              0 * isq::frequency[Hz]);
static_assert((zero_Hz + 10 / (2 * isq::period_duration[s])) - (zero_Hz + 5 * isq::frequency[Hz]) ==
              0 * isq::frequency[Hz]);
static_assert((zero_Hz + 5 * isq::frequency[Hz]) - (zero_Hz + 10 / (2 * isq::period_duration[s])) ==
              0 * isq::frequency[Hz]);

static_assert(is_of_type<(zero_Hz + 10 / (2 * isq::period_duration[s])) + 5 * isq::frequency[Hz],
                         quantity_point<isq::frequency[Hz], zero_Hz, int>>);
static_assert(is_of_type<10 / (2 * isq::period_duration[s]) + (zero_Hz + 5 * isq::frequency[Hz]),
                         quantity_point<isq::frequency[Hz], zero_Hz, int>>);
static_assert(is_of_type<(zero_Hz + 5 * isq::frequency[Hz]) + 10 / (2 * isq::period_duration[s]),
                         quantity_point<isq::frequency[Hz], zero_Hz, int>>);
static_assert(is_of_type<5 * isq::frequency[Hz] + (zero_Hz + 10 / (2 * isq::period_duration[s])),
                         quantity_point<isq::frequency[Hz], zero_Hz, int>>);
static_assert(is_of_type<(zero_Hz + 10 / (2 * isq::period_duration[s])) - 5 * isq::frequency[Hz],
                         quantity_point<isq::frequency[Hz], zero_Hz, int>>);
static_assert(is_of_type<(zero_Hz + 5 * isq::frequency[Hz]) - 10 / (2 * isq::period_duration[s]),
                         quantity_point<isq::frequency[Hz], zero_Hz, int>>);
static_assert(is_of_type<(zero_Hz + 10 / (2 * isq::period_duration[s])) - (zero_Hz + 5 * isq::frequency[Hz]),
                         quantity<isq::frequency[Hz], int>>);
static_assert(is_of_type<(zero_Hz + 5 * isq::frequency[Hz]) - (zero_Hz + 10 / (2 * isq::period_duration[s])),
                         quantity<isq::frequency[Hz], int>>);

static_assert((quantity_point{10 / (2 * isq::period_duration[s])} + 5 * isq::frequency[Hz]).quantity_from_zero() ==
              10 * isq::frequency[Hz]);
static_assert((10 / (2 * isq::period_duration[s]) + quantity_point{zero_Hz + 5 * isq::frequency[Hz]})
                .quantity_from_zero() == 10 * isq::frequency[Hz]);
static_assert((quantity_point{5 * isq::frequency[Hz]} + 10 / (2 * isq::period_duration[s])).quantity_from_zero() ==
              10 * isq::frequency[Hz]);
static_assert((5 * isq::frequency[Hz] + quantity_point{10 / (2 * isq::period_duration[s])}).quantity_from_zero() ==
              10 * isq::frequency[Hz]);
static_assert((quantity_point{10 / (2 * isq::period_duration[s])} - 5 * isq::frequency[Hz]).quantity_from_zero() ==
              0 * isq::frequency[Hz]);
static_assert((quantity_point{5 * isq::frequency[Hz]} - 10 / (2 * isq::period_duration[s])).quantity_from_zero() ==
              0 * isq::frequency[Hz]);
static_assert(quantity_point{10 / (2 * isq::period_duration[s])} - quantity_point{5 * isq::frequency[Hz]} ==
              0 * isq::frequency[Hz]);
static_assert(quantity_point{5 * isq::frequency[Hz]} - quantity_point{10 / (2 * isq::period_duration[s])} ==
              0 * isq::frequency[Hz]);

static_assert(is_of_type<quantity_point{10 / (2 * isq::period_duration[s])} + 5 * isq::frequency[Hz],
                         quantity_point<isq::frequency[Hz], zeroth_point_origin<isq::frequency>, int>>);
static_assert(is_of_type<10 / (2 * isq::period_duration[s]) + quantity_point{5 * isq::frequency[Hz]},
                         quantity_point<isq::frequency[Hz], zeroth_point_origin<isq::frequency>, int>>);
static_assert(is_of_type<quantity_point{5 * isq::frequency[Hz]} + 10 / (2 * isq::period_duration[s]),
                         quantity_point<isq::frequency[Hz], zeroth_point_origin<isq::frequency>, int>>);
static_assert(is_of_type<5 * isq::frequency[Hz] + quantity_point{10 / (2 * isq::period_duration[s])},
                         quantity_point<isq::frequency[Hz], zeroth_point_origin<isq::frequency>, int>>);
static_assert(is_of_type<quantity_point{10 / (2 * isq::period_duration[s])} - 5 * isq::frequency[Hz],
                         quantity_point<isq::frequency[Hz], zeroth_point_origin<isq::frequency>, int>>);
static_assert(is_of_type<quantity_point{5 * isq::frequency[Hz]} - 10 / (2 * isq::period_duration[s]),
                         quantity_point<isq::frequency[Hz], zeroth_point_origin<isq::frequency>, int>>);
static_assert(is_of_type<quantity_point{10 / (2 * isq::period_duration[s])} - quantity_point{5 * isq::frequency[Hz]},
                         quantity<isq::frequency[Hz], int>>);
static_assert(is_of_type<quantity_point{5 * isq::frequency[Hz]} - quantity_point{10 / (2 * isq::period_duration[s])},
                         quantity<isq::frequency[Hz], int>>);

// Different named dimensions
template<typename... Ts>
consteval bool invalid_addition(Ts... ts)
{
  return !requires { (... + ts); };
}

template<typename... Ts>
consteval bool invalid_subtraction(Ts... ts)
{
  return !requires { (... - ts); };
}

inline constexpr struct zero_Bq : absolute_point_origin<zero_Bq, kind_of<isq::activity>> {
} zero_Bq;

static_assert(invalid_addition(zero_Bq + 5 * isq::activity[Bq], 5 * isq::frequency[Hz]));
static_assert(invalid_addition(5 * isq::activity[Bq], zero_Hz + 5 * isq::frequency[Hz]));
static_assert(invalid_subtraction(zero_Bq + 5 * isq::activity[Bq], 5 * isq::frequency[Hz]));
static_assert(invalid_subtraction(zero_Bq + 5 * isq::activity[Bq], zero_Hz + 5 * isq::frequency[Hz]));

static_assert(invalid_addition(zero_Bq + 5 * isq::activity[Bq], 10 / (2 * isq::time[s]), 5 * isq::frequency[Hz]));
static_assert(invalid_addition(5 * isq::activity[Bq], zero_Hz + 10 / (2 * isq::time[s]), 5 * isq::frequency[Hz]));
static_assert(invalid_addition(5 * isq::activity[Bq], 10 / (2 * isq::time[s]), zero_Hz + 5 * isq::frequency[Hz]));
static_assert(invalid_subtraction(zero_Bq + 5 * isq::activity[Bq], 10 / (2 * isq::time[s]), 5 * isq::frequency[Hz]));

static_assert(invalid_addition(quantity_point{5 * isq::activity[Bq]}, 5 * isq::frequency[Hz]));
static_assert(invalid_addition(5 * isq::activity[Bq], quantity_point{5 * isq::frequency[Hz]}));
static_assert(invalid_subtraction(quantity_point{5 * isq::activity[Bq]}, 5 * isq::frequency[Hz]));
static_assert(invalid_subtraction(quantity_point{5 * isq::activity[Bq]}, quantity_point{5 * isq::frequency[Hz]}));

static_assert(invalid_addition(quantity_point{5 * isq::activity[Bq]}, 10 / (2 * isq::time[s]), 5 * isq::frequency[Hz]));
static_assert(invalid_addition(5 * isq::activity[Bq], quantity_point{10 / (2 * isq::time[s])}, 5 * isq::frequency[Hz]));
static_assert(invalid_addition(5 * isq::activity[Bq], 10 / (2 * isq::time[s]), quantity_point{5 * isq::frequency[Hz]}));
static_assert(invalid_subtraction(quantity_point{5 * isq::activity[Bq]}, 10 / (2 * isq::time[s]),
                                  5 * isq::frequency[Hz]));

// value_cast

static_assert(value_cast<m>(quantity_point{2 * km}).quantity_from_zero().numerical_value_in(m) == 2000);
static_assert(value_cast<km>(quantity_point{2000 * m}).quantity_from_zero().numerical_value_in(km) == 2);
static_assert(value_cast<int>(quantity_point{1.23 * m}).quantity_from_zero().numerical_value_in(m) == 1);
static_assert(
  value_cast<km / h>(quantity_point{2000.0 * m / (3600.0 * s)}).quantity_from_zero().numerical_value_in(km / h) == 2);
// lvalue references in value_cast
namespace lvalue_tests {
constexpr quantity_point lvalue_qp{2 * km};
static_assert(value_cast<m>(lvalue_qp).quantity_from_zero().numerical_value_in(m) == 2000);
static_assert(value_cast<float>(lvalue_qp).quantity_from_zero().numerical_value_in(km) == 2.f);
static_assert(value_cast<m, float>(lvalue_qp).quantity_from_zero().numerical_value_in(m) == 2000.f);
}  // namespace lvalue_tests

static_assert(value_cast<quantity<km, int>>(quantity_point{2000 * m}).quantity_from_zero().numerical_value_in(km) == 2);
static_assert(value_cast<quantity_point<km>>(quantity_point{2000 * m}).quantity_from_zero().numerical_value_in(km) ==
              2);

template<typename ToQ, typename FromQ>
constexpr bool value_cast_is_forbidden()
{
  // it appears we cannot have the requires clause right inside static_assert
  return !requires(FromQ q) { value_cast<ToQ>(q); };
}
static_assert(value_cast_is_forbidden<quantity_point<m>, quantity_point<isq::width[m]>>(),
              "value_cast shall not cast between different quantity types");
static_assert(value_cast_is_forbidden<quantity_point<isq::width[m]>, quantity_point<m>>(),
              "value_cast shall not cast between different quantity types");
static_assert(value_cast<quantity_point<isq::height[m], mean_sea_level>>(quantity_point{2 * isq::height[km],
                                                                                        ground_level})
                .quantity_from_origin_is_an_implementation_detail_.numerical_value_in(m) == 2042);
static_assert(value_cast<quantity_point<isq::height[cm], mean_sea_level, int>>(
                quantity_point{std::int8_t{100} * isq::height[mm], ground_level})
                .quantity_from_origin_is_an_implementation_detail_.numerical_value_in(cm) == 4210);
static_assert(value_cast<quantity_point<isq::height[mm], ground_level, std::int8_t>>(
                quantity_point{4210 * isq::height[cm], mean_sea_level})
                .quantity_from_origin_is_an_implementation_detail_.numerical_value_in(mm) == 100);


}  // namespace
