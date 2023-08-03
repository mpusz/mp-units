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
#include <mp-units/chrono.h>
#include <mp-units/quantity_point.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si/si.h>
#include <limits>
#include <type_traits>
#include <utility>

namespace {

using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using namespace std::chrono_literals;
using sys_seconds = std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>;

inline constexpr struct mean_sea_level : absolute_point_origin<isq::height> {
} mean_sea_level;

inline constexpr struct ground_level : relative_point_origin<quantity_point{42 * isq::height[m], mean_sea_level}> {
} ground_level;

inline constexpr struct tower_peak : relative_point_origin<quantity_point{42 * isq::height[m], ground_level}> {
} tower_peak;

inline constexpr struct other_ground_level :
    relative_point_origin<quantity_point{123 * isq::height[m], mean_sea_level}> {
} other_ground_level;

QUANTITY_SPEC(special_height, isq::height);
QUANTITY_SPEC(activity, 1 / isq::time);

/////////////////////
// class invariants
/////////////////////

static_assert(sizeof(quantity_point<si::metre>) == sizeof(double));
static_assert(sizeof(quantity_point<isq::length[m]>) == sizeof(double));
static_assert(sizeof(quantity_point<si::metre, absolute_point_origin<isq::length>{}, short>) == sizeof(short));
static_assert(sizeof(quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, short>) == sizeof(short));

template<template<auto, auto, typename> typename QP>
concept invalid_types = requires {
  // unit of a different dimension
  requires !requires { typename QP<si::second, absolute_point_origin<isq::length>{}, int>; };
  requires !requires { typename QP<si::second, absolute_point_origin<kind_of<isq::length>>{}, int>; };
  // not compatible quantity_spec
  requires !requires { typename QP<isq::width[si::metre], absolute_point_origin<isq::height>{}, int>; };
  requires !requires { typename QP<isq::width[m], absolute_point_origin<isq::thickness>{}, int>; };
  // quantity used as Rep
  requires !requires { typename QP<si::metre, absolute_point_origin<isq::length>{}, quantity<si::metre, int>>; };
  // quantity point used as Rep
  requires !requires { typename QP<si::metre, absolute_point_origin<isq::length>{}, quantity_point<si::metre>>; };
  // reordered arguments
  requires !requires { typename QP<absolute_point_origin<isq::length>{}, si::metre, double>; };
  // quantity_spec used as origin
  requires !requires { typename QP<si::metre, isq::length, int>; };
  // quantity_spec used as a reference
  requires !requires { typename QP<isq::length, absolute_point_origin<isq::length>{}, int>; };
  // dimension used as a reference
  requires !requires { typename QP<isq::dim_length, absolute_point_origin<isq::length>{}, int>; };
  // bool used as a representation type
  requires !requires { typename QP<si::metre, absolute_point_origin<isq::length>{}, bool>; };
  // incompatible quantity_spec in the origin and quantity_point
  requires !requires { typename QP<isq::length[m], absolute_point_origin<isq::height>{}, int>; };
  requires !requires { typename QP<isq::length[m], mean_sea_level, int>; };
  requires !requires { typename QP<isq::length[m], ground_level, int>; };
};
static_assert(invalid_types<quantity_point>);

template<template<auto, auto> typename QP>
concept valid_types = requires {
  typename QP<si::metre, absolute_point_origin<kind_of<isq::length>>{}>;
  typename QP<si::metre, absolute_point_origin<isq::length>{}>;
  typename QP<si::metre, absolute_point_origin<isq::height>{}>;
  typename QP<isq::length[m], absolute_point_origin<isq::length>{}>;
  typename QP<isq::height[m], absolute_point_origin<isq::length>{}>;
  typename QP<isq::length[m], absolute_point_origin<kind_of<isq::length>>{}>;
  typename QP<isq::height[m], absolute_point_origin<kind_of<isq::length>>{}>;
  typename QP<isq::thickness[m], absolute_point_origin<isq::width>{}>;
  typename QP<si::metre, mean_sea_level>;
  typename QP<isq::height[m], mean_sea_level>;
  typename QP<special_height[m], mean_sea_level>;
  typename QP<si::metre, ground_level>;
  typename QP<isq::height[m], ground_level>;
  typename QP<special_height[m], ground_level>;
};
static_assert(valid_types<quantity_point>);

static_assert(std::is_trivially_default_constructible_v<quantity_point<si::metre>>);
static_assert(std::is_trivially_copy_constructible_v<quantity_point<si::metre>>);
static_assert(std::is_trivially_move_constructible_v<quantity_point<si::metre>>);
static_assert(std::is_trivially_copy_assignable_v<quantity_point<si::metre>>);
static_assert(std::is_trivially_move_assignable_v<quantity_point<si::metre>>);
static_assert(std::is_trivially_destructible_v<quantity_point<si::metre>>);

static_assert(std::is_nothrow_default_constructible_v<quantity_point<si::metre>>);
static_assert(std::is_nothrow_copy_constructible_v<quantity_point<si::metre>>);
static_assert(std::is_nothrow_move_constructible_v<quantity_point<si::metre>>);
static_assert(std::is_nothrow_copy_assignable_v<quantity_point<si::metre>>);
static_assert(std::is_nothrow_move_assignable_v<quantity_point<si::metre>>);
static_assert(std::is_nothrow_destructible_v<quantity_point<si::metre>>);

static_assert(std::is_trivially_copyable_v<quantity_point<si::metre>>);
static_assert(std::is_standard_layout_v<quantity_point<si::metre>>);

static_assert(std::default_initializable<quantity_point<si::metre>>);
static_assert(std::move_constructible<quantity_point<si::metre>>);
static_assert(std::copy_constructible<quantity_point<si::metre>>);
static_assert(std::equality_comparable<quantity_point<si::metre>>);
static_assert(std::totally_ordered<quantity_point<si::metre>>);
static_assert(std::regular<quantity_point<si::metre>>);

static_assert(std::three_way_comparable<quantity_point<si::metre>>);


//////////////////
// member values
//////////////////

static_assert(quantity_point<si::metre>::reference == si::metre);
static_assert(quantity_point<si::metre>::quantity_spec == kind_of<isq::length>);
static_assert(quantity_point<si::metre>::dimension == isq::dim_length);
static_assert(quantity_point<si::metre>::unit == si::metre);
static_assert(is_of_type<quantity_point<si::metre>::point_origin, absolute_point_origin<kind_of<isq::length>>>);

static_assert(quantity_point<isq::length[m]>::reference == isq::length[m]);
static_assert(quantity_point<isq::length[m]>::quantity_spec == isq::length);
static_assert(quantity_point<isq::length[m]>::dimension == isq::dim_length);
static_assert(quantity_point<isq::length[m]>::unit == si::metre);
static_assert(is_of_type<quantity_point<isq::length[m]>::point_origin, absolute_point_origin<isq::length>>);
static_assert(is_of_type<quantity_point<isq::length[m]>::absolute_point_origin, absolute_point_origin<isq::length>>);

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


//////////////////
// member types
//////////////////

static_assert(is_same_v<quantity_point<si::metre>::rep, double>);
static_assert(is_same_v<quantity_point<si::metre>::quantity_type, quantity<si::metre>>);

static_assert(is_same_v<quantity_point<si::metre, absolute_point_origin<isq::length>{}, int>::rep, int>);
static_assert(is_same_v<quantity_point<si::metre, absolute_point_origin<isq::length>{}, int>::quantity_type,
                        quantity<si::metre, int>>);

static_assert(is_same_v<quantity_point<isq::length[m]>::rep, double>);
static_assert(is_same_v<quantity_point<isq::length[m]>::quantity_type, quantity<isq::length[m]>>);

static_assert(is_same_v<quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>::rep, int>);
static_assert(is_same_v<quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>::quantity_type,
                        quantity<isq::length[m], int>>);


////////////////////////////
// static member functions
////////////////////////////

static_assert(quantity_point<isq::length[m]>::zero().relative().number() == 0);
static_assert(quantity_point<isq::length[m]>::min().relative().number() == std::numeric_limits<double>::lowest());
static_assert(quantity_point<isq::length[m]>::max().relative().number() == std::numeric_limits<double>::max());
static_assert(quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>::zero().relative().number() ==
              0);
static_assert(quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>::min().relative().number() ==
              std::numeric_limits<int>::lowest());
static_assert(quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>::max().relative().number() ==
              std::numeric_limits<int>::max());


//////////////////////////////
// construction from a value
//////////////////////////////

// construction from a value is private
static_assert(!std::constructible_from<quantity_point<isq::length[m]>, double>);
static_assert(!std::convertible_to<double, quantity_point<isq::length[m]>>);

static_assert(!std::constructible_from<quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>, int>);
static_assert(!std::convertible_to<int, quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);

static_assert(!std::constructible_from<quantity_point<dimensionless[percent]>, double>);
static_assert(!std::convertible_to<double, quantity_point<dimensionless[percent]>>);

static_assert(
  !std::constructible_from<quantity_point<dimensionless[percent], absolute_point_origin<dimensionless>{}, int>, int>);
static_assert(
  !std::convertible_to<int, quantity_point<dimensionless[percent], absolute_point_origin<dimensionless>{}, int>>);
static_assert(!std::constructible_from<quantity_point<dimensionless[one]>, double>);
static_assert(!std::convertible_to<double, quantity_point<dimensionless[one]>>);
static_assert(
  !std::constructible_from<quantity_point<dimensionless[one], absolute_point_origin<dimensionless>{}, int>, int>);
static_assert(
  !std::convertible_to<int, quantity_point<dimensionless[one], absolute_point_origin<dimensionless>{}, int>>);


/////////////////////////////////
// construction from a quantity
/////////////////////////////////

static_assert(std::constructible_from<quantity_point<si::second>, quantity<si::second>>);
static_assert(!std::convertible_to<quantity<si::second>, quantity_point<si::second>>);

static_assert(std::constructible_from<quantity_point<isq::time[s]>, quantity<isq::time[s]>>);
static_assert(!std::convertible_to<quantity<isq::time[s]>, quantity_point<isq::time[s]>>);

static_assert(std::constructible_from<quantity_point<si::second>, quantity<isq::time[s]>>);
static_assert(!std::convertible_to<quantity<isq::time[s]>, quantity_point<si::second>>);

static_assert(std::constructible_from<quantity_point<isq::time[s]>, quantity<si::second>>);
static_assert(!std::convertible_to<quantity<si::second>, quantity_point<isq::time[s]>>);

static_assert(std::constructible_from<quantity_point<isq::length[m]>, quantity<isq::distance[m]>>);
static_assert(!std::convertible_to<quantity<isq::distance[m]>, quantity_point<isq::length[m]>>);

static_assert(std::constructible_from<quantity_point<dimensionless[one]>, quantity<dimensionless[one]>>);
static_assert(!std::convertible_to<quantity<dimensionless[one]>, quantity_point<dimensionless[one]>>);

// different dimensions
static_assert(!std::constructible_from<quantity_point<isq::distance[m]>, quantity<si::second>>);
static_assert(!std::convertible_to<quantity<si::second>, quantity_point<isq::distance[m]>>);

// non-convertible quantity_specs
static_assert(!std::constructible_from<quantity_point<isq::distance[m]>, quantity<isq::length[m]>>);
static_assert(!std::convertible_to<quantity<isq::length[m]>, quantity_point<isq::distance[m]>>);

// quantity-like
static_assert(std::constructible_from<quantity_point<isq::time[s]>, std::chrono::seconds>);
static_assert(!std::convertible_to<std::chrono::seconds, quantity_point<isq::time[s]>>);


///////////////////////////////////////
// construction from a quantity point
///////////////////////////////////////

// same origins
static_assert(std::constructible_from<quantity_point<si::metre>, quantity_point<si::metre>>);
static_assert(std::convertible_to<quantity_point<si::metre>, quantity_point<si::metre>>);

static_assert(std::constructible_from<quantity_point<isq::length[m]>, quantity_point<isq::length[m]>>);
static_assert(std::convertible_to<quantity_point<isq::length[m]>, quantity_point<isq::length[m]>>);

static_assert(std::constructible_from<quantity_point<si::metre>, quantity_point<isq::length[m]>>);
static_assert(std::convertible_to<quantity_point<isq::length[m]>, quantity_point<si::metre>>);

static_assert(std::constructible_from<quantity_point<isq::length[m]>, quantity_point<si::metre>>);
static_assert(std::convertible_to<quantity_point<si::metre>, quantity_point<isq::length[m]>>);

static_assert(std::constructible_from<quantity_point<isq::length[m]>, quantity_point<isq::distance[m]>>);
static_assert(std::convertible_to<quantity_point<isq::distance[m]>, quantity_point<isq::length[m]>>);

static_assert(std::constructible_from<quantity_point<dimensionless[one]>, quantity_point<dimensionless[one]>>);
static_assert(std::convertible_to<quantity_point<dimensionless[one]>, quantity_point<dimensionless[one]>>);

// different dimensions
static_assert(!std::constructible_from<quantity_point<isq::distance[m]>, quantity_point<si::second>>);
static_assert(!std::convertible_to<quantity_point<si::second>, quantity_point<isq::distance[m]>>);

// non-convertible quantity_specs
static_assert(!std::constructible_from<quantity_point<isq::distance[m]>, quantity_point<isq::length[m]>>);
static_assert(!std::convertible_to<quantity_point<isq::length[m]>, quantity_point<isq::distance[m]>>);

// implicit conversion from another quantity only if non-truncating
// int -> double OK
static_assert(std::constructible_from<quantity_point<isq::length[m]>,
                                      quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);
static_assert(std::convertible_to<quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>,
                                  quantity_point<isq::length[m]>>);

// truncating double -> int not allowed
static_assert(!std::constructible_from<quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>,
                                       quantity_point<isq::length[m]>>);
static_assert(!std::convertible_to<quantity_point<isq::length[m]>,
                                   quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);

// kilometre<int> -> metre<int> OK
static_assert(std::constructible_from<quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>,
                                      quantity_point<isq::length[km], absolute_point_origin<isq::length>{}, int>>);
static_assert(std::convertible_to<quantity_point<isq::length[km], absolute_point_origin<isq::length>{}, int>,
                                  quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);

// truncating metre<int> -> kilometre<int> not allowed
static_assert(!std::constructible_from<quantity_point<isq::length[km], absolute_point_origin<isq::length>{}, int>,
                                       quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);
static_assert(!std::convertible_to<quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>,
                                   quantity_point<isq::length[km], absolute_point_origin<isq::length>{}, int>>);

// converting to double always OK
static_assert(std::constructible_from<quantity_point<isq::length[m]>,
                                      quantity_point<isq::length[km], absolute_point_origin<isq::length>{}, int>>);
static_assert(std::convertible_to<quantity_point<isq::length[km], absolute_point_origin<isq::length>{}, int>,
                                  quantity_point<isq::length[m]>>);
static_assert(std::constructible_from<quantity_point<isq::length[km]>,
                                      quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);
static_assert(std::convertible_to<quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>,
                                  quantity_point<isq::length[km]>>);

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
static_assert(!std::constructible_from<quantity_point<isq::height[m]>, quantity_point<isq::height[m], mean_sea_level>>);
static_assert(!std::convertible_to<quantity_point<isq::height[m], mean_sea_level>, quantity_point<isq::height[m]>>);
static_assert(!std::constructible_from<quantity_point<isq::height[m], mean_sea_level>, quantity_point<isq::height[m]>>);
static_assert(!std::convertible_to<quantity_point<isq::height[m]>, quantity_point<isq::height[m], mean_sea_level>>);

// quantity-point-like
static_assert(
  std::constructible_from<quantity_point<isq::time[s], chrono_point_origin<std::chrono::system_clock>>, sys_seconds>);
static_assert(
  !std::convertible_to<sys_seconds, quantity_point<isq::time[s], chrono_point_origin<std::chrono::system_clock>>>);

// incompatible origin
static_assert(!std::constructible_from<quantity_point<isq::time[s]>, sys_seconds>);
static_assert(!std::convertible_to<sys_seconds, quantity_point<isq::time[s]>>);


//////////////////////////////////
// obtaining a relative quantity
//////////////////////////////////

static_assert(quantity_point(42 * m).relative() == 42 * m);
static_assert(quantity_point(isq::height(42 * m)).relative() == 42 * m);

static_assert(quantity_point(1 * one).relative() == 1 * one);
static_assert(quantity_point(dimensionless(1 * one)).relative() == 1 * one);

static_assert(quantity_point<isq::height[m], mean_sea_level>(42 * m).relative() == 42 * m);
static_assert(quantity_point<isq::height[m], ground_level>(42 * m).relative() == 42 * m);
static_assert(quantity_point<isq::height[m], tower_peak>(42 * m).relative() == 42 * m);

static_assert(quantity_point<isq::height[m], mean_sea_level>(quantity_point<isq::height[m], ground_level>(42 * m))
                .relative() == 84 * m);
static_assert(quantity_point<isq::height[m], mean_sea_level>(quantity_point<isq::height[m], tower_peak>(42 * m))
                .relative() == 126 * m);

static_assert(quantity_point<isq::height[m], ground_level>(quantity_point<isq::height[m], mean_sea_level>(84 * m))
                .relative() == 42 * m);
static_assert(quantity_point<isq::height[m], ground_level>(quantity_point<isq::height[m], tower_peak>(42 * m))
                .relative() == 84 * m);

static_assert(quantity_point<isq::height[m], tower_peak>(quantity_point<isq::height[m], mean_sea_level>(42 * m))
                .relative() == -42 * m);
static_assert(quantity_point<isq::height[m], tower_peak>(quantity_point<isq::height[m], ground_level>(84 * m))
                .relative() == 42 * m);

static_assert(quantity_point<isq::height[m], mean_sea_level>(42 * m).point_from(mean_sea_level).relative() == 42 * m);
static_assert(quantity_point<isq::height[m], ground_level>(42 * m).point_from(mean_sea_level).relative() == 84 * m);
static_assert(quantity_point<isq::height[m], tower_peak>(42 * m).point_from(mean_sea_level).relative() == 126 * m);

static_assert(quantity_point<isq::height[m], ground_level>(84 * m).point_from(ground_level).relative() == 84 * m);
static_assert(quantity_point<isq::height[m], mean_sea_level>(84 * m).point_from(ground_level).relative() == 42 * m);
static_assert(quantity_point<isq::height[m], tower_peak>(42 * m).point_from(ground_level).relative() == 84 * m);

static_assert(quantity_point<isq::height[m], tower_peak>(42 * m).point_from(tower_peak).relative() == 42 * m);
static_assert(quantity_point<isq::height[m], mean_sea_level>(42 * m).point_from(tower_peak).relative() == -42 * m);
static_assert(quantity_point<isq::height[m], ground_level>(84 * m).point_from(tower_peak).relative() == 42 * m);


///////////////////////////////////
// obtaining an absolute quantity
///////////////////////////////////

static_assert(quantity_point(42 * m).absolute() == 42 * m);
static_assert(quantity_point(isq::height(42 * m)).absolute() == 42 * m);

static_assert(quantity_point(1 * one).absolute() == 1 * one);
static_assert(quantity_point(dimensionless(1 * one)).absolute() == 1 * one);

static_assert(quantity_point<isq::height[m], mean_sea_level>(42 * m).absolute() == 42 * m);
static_assert(quantity_point<isq::height[m], ground_level>(42 * m).absolute() == 84 * m);

static_assert(quantity_point<isq::height[m], mean_sea_level>(quantity_point<isq::height[m], ground_level>(42 * m))
                .absolute() == 84 * m);
static_assert(quantity_point<isq::height[m], mean_sea_level>(quantity_point<isq::height[m], tower_peak>(42 * m))
                .absolute() == 126 * m);

static_assert(quantity_point<isq::height[m], ground_level>(quantity_point<isq::height[m], mean_sea_level>(84 * m))
                .absolute() == 84 * m);
static_assert(quantity_point<isq::height[m], ground_level>(quantity_point<isq::height[m], tower_peak>(42 * m))
                .absolute() == 126 * m);

static_assert(quantity_point<isq::height[m], tower_peak>(quantity_point<isq::height[m], mean_sea_level>(42 * m))
                .absolute() == 42 * m);
static_assert(quantity_point<isq::height[m], tower_peak>(quantity_point<isq::height[m], ground_level>(84 * m))
                .absolute() == 126 * m);

static_assert(quantity_point<isq::height[m], mean_sea_level>(42 * m).point_from(mean_sea_level).absolute() == 42 * m);
static_assert(quantity_point<isq::height[m], ground_level>(42 * m).point_from(mean_sea_level).absolute() == 84 * m);
static_assert(quantity_point<isq::height[m], tower_peak>(42 * m).point_from(mean_sea_level).absolute() == 126 * m);

static_assert(quantity_point<isq::height[m], ground_level>(42 * m).point_from(ground_level).absolute() == 84 * m);
static_assert(quantity_point<isq::height[m], mean_sea_level>(84 * m).point_from(ground_level).absolute() == 84 * m);
static_assert(quantity_point<isq::height[m], tower_peak>(42 * m).point_from(ground_level).absolute() == 126 * m);

static_assert(quantity_point<isq::height[m], tower_peak>(42 * m).point_from(tower_peak).absolute() == 126 * m);
static_assert(quantity_point<isq::height[m], mean_sea_level>(42 * m).point_from(tower_peak).absolute() == 42 * m);
static_assert(quantity_point<isq::height[m], ground_level>(84 * m).point_from(tower_peak).absolute() == 126 * m);


///////////////////////////////////
// converting to a different unit
///////////////////////////////////

static_assert(quantity_point<isq::length[km]>(2. * km)[km].relative().number() == 2.);
static_assert(quantity_point<isq::length[km]>(2. * km)[m].relative().number() == 2000.);
static_assert(quantity_point<isq::length[m]>(2000. * m)[km].relative().number() == 2.);
static_assert(
  quantity_point<isq::length[km], absolute_point_origin<isq::length>{}, int>(2 * km)[km].relative().number() == 2);
static_assert(
  quantity_point<isq::length[km], absolute_point_origin<isq::length>{}, int>(2 * km)[m].relative().number() == 2000);

#if MP_UNITS_COMP_GCC != 10 || MP_UNITS_COMP_GCC_MINOR > 2
template<template<auto, auto, typename> typename QP>
concept invalid_unit_conversion = requires {
  requires !requires {
    QP<isq::length[m], absolute_point_origin<isq::length>{}, int>(2000 * m)[km];
  };  // truncating conversion
  requires !requires { QP<isq::length[m], absolute_point_origin<isq::length>{}, int>(2 * m)[s]; };  // invalid unit
};
static_assert(invalid_unit_conversion<quantity_point>);
#endif


/////////
// CTAD
/////////

static_assert(std::is_same_v<std::remove_const_t<decltype(quantity_point{123 * m}.point_origin)>,
                             absolute_point_origin<kind_of<isq::length>>>);
static_assert(std::is_same_v<std::remove_const_t<decltype(quantity_point{123 * m, mean_sea_level}.point_origin)>,
                             std::remove_const_t<decltype(mean_sea_level)>>);
static_assert(std::is_same_v<std::remove_const_t<decltype(quantity_point{123 * m, ground_level}.point_origin)>,
                             std::remove_const_t<decltype(ground_level)>>);
static_assert(std::is_same_v<decltype(quantity_point{123 * m})::rep, int>);
static_assert(std::is_same_v<decltype(quantity_point{123. * m})::rep, double>);
static_assert(quantity_point{123. * m}.unit == si::metre);
static_assert(quantity_point{123. * m}.quantity_spec == kind_of<isq::length>);
static_assert(quantity_point{123. * h}.unit == si::hour);
static_assert(quantity_point{123. * h}.quantity_spec == kind_of<isq::time>);

using namespace std::chrono_literals;
static_assert(std::is_same_v<decltype(quantity_point{sys_seconds{123s}})::rep, std::chrono::seconds::rep>);
static_assert(std::is_same_v<std::remove_const_t<decltype(quantity_point{sys_seconds{123s}}.point_origin)>,
                             chrono_point_origin_<std::chrono::system_clock>>);
static_assert(quantity_point{sys_seconds{24h}}.unit == si::second);
static_assert(quantity_point{sys_seconds{24h}}.quantity_spec == kind_of<isq::time>);


////////////////////////
// assignment operator
////////////////////////

static_assert(([]() {
                quantity_point l1{1 * m}, l2{2 * m};
                return l2 = l1;
              }())
                .relative() == 1 * m);
static_assert(([]() {
                const quantity_point l1{1 * m};
                quantity_point l2{2 * m};
                return l2 = l1;
              }())
                .relative() == 1 * m);
static_assert(([]() {
                quantity_point l1{1 * m}, l2{2 * m};
                return l2 = std::move(l1);
              }())
                .relative() == 1 * m);


////////////////////
// unary operators
////////////////////

static_assert([](auto v) {
  auto vv = v++;
  return std::pair(v, vv);
}(quantity_point(123 * m)) == std::pair(quantity_point(124 * m), quantity_point(123 * m)));
static_assert([](auto v) {
  auto vv = ++v;
  return std::pair(v, vv);
}(quantity_point(123 * m)) == std::pair(quantity_point(124 * m), quantity_point(124 * m)));
static_assert([](auto v) {
  auto vv = v--;
  return std::pair(v, vv);
}(quantity_point(123 * m)) == std::pair(quantity_point(122 * m), quantity_point(123 * m)));
static_assert([](auto v) {
  auto vv = --v;
  return std::pair(v, vv);
}(quantity_point(123 * m)) == std::pair(quantity_point(122 * m), quantity_point(122 * m)));


////////////////////////
// compound assignment
////////////////////////

// same type
static_assert((quantity_point(1 * m) += 1 * m).relative().number() == 2);
static_assert((quantity_point(2 * m) -= 1 * m).relative().number() == 1);

// different types
static_assert((quantity_point(2.5 * m) += 3 * m).relative().number() == 5.5);
static_assert((quantity_point(123 * m) += 1 * km).relative().number() == 1123);
static_assert((quantity_point(5.5 * m) -= 3 * m).relative().number() == 2.5);
static_assert((quantity_point(1123 * m) -= 1 * km).relative().number() == 123);


template<template<auto, auto, typename> typename QP>
concept invalid_compound_assignments = requires() {
  // truncating not allowed
  requires !requires(QP<isq::length[m], absolute_point_origin<isq::length>{}, int> l) { l += 2.5 * m; };
  requires !requires(QP<isq::length[m], absolute_point_origin<isq::length>{}, int> l) { l -= 2.5 * m; };
  requires !requires(QP<isq::length[km], absolute_point_origin<isq::length>{}, int> l) { l += 2 * isq::length[m]; };
  requires !requires(QP<isq::length[km], absolute_point_origin<isq::length>{}, int> l) { l -= 2 * isq::length[m]; };

  // only quantities can be added or subtracted
  requires !requires(QP<isq::length[m], absolute_point_origin<isq::length>{}, int> l) { l += 2; };
  requires !requires(QP<isq::length[m], absolute_point_origin<isq::length>{}, int> l) { l -= 2; };

  // no unit constants
  requires !requires(QP<isq::length[m], absolute_point_origin<isq::length>{}, int> l) { l += m; };
  requires !requires(QP<isq::length[m], absolute_point_origin<isq::length>{}, int> l) { l -= m; };
};
static_assert(invalid_compound_assignments<quantity_point>);


////////////////////
// binary operators
////////////////////

template<template<auto> typename QP>
concept invalid_binary_operations = requires {
  // can't add two quantity points
  requires !requires { QP<isq::length[m]>(1 * m) + QP<isq::length[m]>(1 * m); };

  // can't add more generic quantity (violates point_origin quantity_spec)
  requires !requires { QP<isq::height[m]>(1 * m) + isq::length(1 * m); };
  requires !requires { isq::length(1 * m) + QP<isq::height[m]>(1 * m); };

  // can't subtract more generic quantity (violates point_origin quantity_spec)
  requires !requires { QP<isq::height[m]>(1 * m) - isq::length(1 * m); };

  // quantity point can't be subtracted from a quantity
  requires !requires { 1 * m - QP<si::metre>(1 * m); };

  // no crossdimensional addition and subtraction
  requires !requires { QP<si::metre>(1 * m) + 1 * s; };
  requires !requires { QP<si::metre>(1 * m) - 1 * s; };

  // unit constants
  requires !requires { QP<si::metre>(1) + m; };
  requires !requires { QP<si::metre>(1) - m; };
  requires !requires { m + QP<si::metre>(1); };
  requires !requires { m - QP<si::metre>(1); };
};
static_assert(invalid_binary_operations<quantity_point>);

template<template<auto, auto> typename QP>
concept invalid_binary_operations_with_origins = requires {
  // can't subtract two quantity points of incompatible origins
  requires !requires {
    QP<isq::length[m], absolute_point_origin<isq::length>{}>(1 * m) - QP<isq::length[m], mean_sea_level>(1 * m);
  };
  requires !requires {
    QP<isq::length[m], mean_sea_level>(1 * m) - QP<isq::length[m], absolute_point_origin<isq::length>{}>(1 * m);
  };
  requires !requires {
    QP<isq::length[m], absolute_point_origin<isq::length>{}>(1 * m) - QP<isq::length[m], ground_level>(1 * m);
  };
  requires !requires {
    QP<isq::length[m], ground_level>(1 * m) - QP<isq::length[m], absolute_point_origin<isq::length>{}>(1 * m);
  };
};
static_assert(invalid_binary_operations_with_origins<quantity_point>);

// same representation type
static_assert(is_of_type<quantity_point(1 * m) + 1 * m,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, int>>);
static_assert(is_of_type<quantity_point(1 * m) + 1 * km,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, int>>);
static_assert(is_of_type<quantity_point(1 * km) + 1 * m,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, int>>);

static_assert(is_of_type<quantity_point(1 * m) + isq::length(1 * m),
                         quantity_point<isq::length[m], absolute_point_origin<kind_of<isq::length>>{}, int>>);
static_assert(is_of_type<quantity_point(1 * m) + isq::length(1 * km),
                         quantity_point<isq::length[m], absolute_point_origin<kind_of<isq::length>>{}, int>>);
static_assert(is_of_type<quantity_point(1 * km) + isq::length(1 * m),
                         quantity_point<isq::length[m], absolute_point_origin<kind_of<isq::length>>{}, int>>);

static_assert(is_of_type<quantity_point(isq::length(1 * m)) + 1 * m,
                         quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);
static_assert(is_of_type<quantity_point(isq::length(1 * m)) + 1 * km,
                         quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);
static_assert(is_of_type<quantity_point(isq::length(1 * km)) + 1 * m,
                         quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);

static_assert(is_of_type<quantity_point(isq::length(1 * m)) + isq::height(1 * m),
                         quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);
static_assert(is_of_type<quantity_point(isq::length(1 * m)) + isq::height(1 * km),
                         quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);
static_assert(is_of_type<quantity_point(isq::length(1 * km)) + isq::height(1 * m),
                         quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);

static_assert(is_of_type<1 * m + quantity_point(1 * m),
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, int>>);
static_assert(is_of_type<1 * m + quantity_point(1 * km),
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, int>>);
static_assert(is_of_type<1 * km + quantity_point(1 * m),
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, int>>);

static_assert(is_of_type<1 * m + quantity_point(isq::length(1 * m)),
                         quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);
static_assert(is_of_type<1 * m + quantity_point(isq::length(1 * km)),
                         quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);
static_assert(is_of_type<1 * km + quantity_point(isq::length(1 * m)),
                         quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);

static_assert(is_of_type<isq::length(1 * m) + quantity_point(1 * m),
                         quantity_point<isq::length[m], absolute_point_origin<kind_of<isq::length>>{}, int>>);
static_assert(is_of_type<isq::length(1 * m) + quantity_point(1 * km),
                         quantity_point<isq::length[m], absolute_point_origin<kind_of<isq::length>>{}, int>>);
static_assert(is_of_type<isq::length(1 * km) + quantity_point(1 * m),
                         quantity_point<isq::length[m], absolute_point_origin<kind_of<isq::length>>{}, int>>);

static_assert(is_of_type<isq::height(1 * m) + quantity_point(isq::length(1 * m)),
                         quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);
static_assert(is_of_type<isq::height(1 * m) + quantity_point(isq::length(1 * km)),
                         quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);
static_assert(is_of_type<isq::height(1 * km) + quantity_point(isq::length(1 * m)),
                         quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);

static_assert(is_of_type<quantity_point(1 * m) - 1 * m,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, int>>);
static_assert(is_of_type<quantity_point(1 * km) - 1 * m,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, int>>);
static_assert(is_of_type<quantity_point(1 * m) - 1 * km,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, int>>);

static_assert(is_of_type<quantity_point(1 * m) - isq::length(1 * m),
                         quantity_point<isq::length[m], absolute_point_origin<kind_of<isq::length>>{}, int>>);
static_assert(is_of_type<quantity_point(1 * m) - isq::length(1 * km),
                         quantity_point<isq::length[m], absolute_point_origin<kind_of<isq::length>>{}, int>>);
static_assert(is_of_type<quantity_point(1 * km) - isq::length(1 * m),
                         quantity_point<isq::length[m], absolute_point_origin<kind_of<isq::length>>{}, int>>);

static_assert(is_of_type<quantity_point(isq::length(1 * m)) - 1 * m,
                         quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);
static_assert(is_of_type<quantity_point(isq::length(1 * m)) - 1 * km,
                         quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);
static_assert(is_of_type<quantity_point(isq::length(1 * km)) - 1 * m,
                         quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);

static_assert(is_of_type<quantity_point(isq::length(1 * m)) - isq::height(1 * m),
                         quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);
static_assert(is_of_type<quantity_point(isq::length(1 * m)) - isq::height(1 * km),
                         quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);
static_assert(is_of_type<quantity_point(isq::length(1 * km)) - isq::height(1 * m),
                         quantity_point<isq::length[m], absolute_point_origin<isq::length>{}, int>>);

static_assert(is_of_type<quantity_point(1 * m) - quantity_point(1 * m), quantity<si::metre, int>>);
static_assert(is_of_type<quantity_point(1 * km) - quantity_point(1 * m), quantity<si::metre, int>>);
static_assert(is_of_type<quantity_point(1 * m) - quantity_point(1 * km), quantity<si::metre, int>>);

static_assert(is_of_type<quantity_point(1 * m) - quantity_point(isq::length(1 * m)), quantity<isq::length[m], int>>);
static_assert(is_of_type<quantity_point(1 * m) - quantity_point(isq::length(1 * km)), quantity<isq::length[m], int>>);
static_assert(is_of_type<quantity_point(1 * km) - quantity_point(isq::length(1 * m)), quantity<isq::length[m], int>>);

static_assert(is_of_type<quantity_point(isq::length(1 * m)) - quantity_point(1 * m), quantity<isq::length[m], int>>);
static_assert(is_of_type<quantity_point(isq::length(1 * m)) - quantity_point(1 * km), quantity<isq::length[m], int>>);
static_assert(is_of_type<quantity_point(isq::length(1 * km)) - quantity_point(1 * m), quantity<isq::length[m], int>>);

static_assert(
  is_of_type<quantity_point(isq::length(1 * m)) - quantity_point(isq::height(1 * m)), quantity<isq::length[m], int>>);
static_assert(
  is_of_type<quantity_point(isq::length(1 * m)) - quantity_point(isq::height(1 * km)), quantity<isq::length[m], int>>);
static_assert(
  is_of_type<quantity_point(isq::length(1 * km)) - quantity_point(isq::height(1 * m)), quantity<isq::length[m], int>>);

static_assert(is_of_type<quantity_point(1 * m, mean_sea_level) - quantity_point(1 * m, ground_level),
                         quantity<isq::height[si::metre], int>>);
static_assert(is_of_type<quantity_point(1 * m, ground_level) - quantity_point(1 * m, mean_sea_level),
                         quantity<isq::height[si::metre], int>>);
static_assert(is_of_type<quantity_point(1 * m, tower_peak) - quantity_point(1 * m, ground_level),
                         quantity<isq::height[si::metre], int>>);
static_assert(is_of_type<quantity_point(1 * m, ground_level) - quantity_point(1 * m, tower_peak),
                         quantity<isq::height[si::metre], int>>);
static_assert(is_of_type<quantity_point(1 * m, tower_peak) - quantity_point(1 * m, mean_sea_level),
                         quantity<isq::height[si::metre], int>>);
static_assert(is_of_type<quantity_point(1 * m, mean_sea_level) - quantity_point(1 * m, tower_peak),
                         quantity<isq::height[si::metre], int>>);
static_assert(is_of_type<quantity_point(1 * m, other_ground_level) - quantity_point(1 * m, ground_level),
                         quantity<isq::height[si::metre], int>>);
static_assert(is_of_type<quantity_point(1 * m, ground_level) - quantity_point(1 * m, other_ground_level),
                         quantity<isq::height[si::metre], int>>);
static_assert(is_of_type<quantity_point(1 * m, other_ground_level) - quantity_point(1 * m, tower_peak),
                         quantity<isq::height[si::metre], int>>);
static_assert(is_of_type<quantity_point(1 * m, tower_peak) - quantity_point(1 * m, other_ground_level),
                         quantity<isq::height[si::metre], int>>);


// check for integral types promotion
static_assert(
  is_same_v<decltype((quantity_point{std::uint8_t(0) * m} + std::uint8_t(0) * m).relative().number()), int&&>);
static_assert(
  is_same_v<decltype((std::uint8_t(0) * m + quantity_point{std::uint8_t(0) * m}).relative().number()), int&&>);
static_assert(
  is_same_v<decltype((quantity_point{std::uint8_t(0) * m} - std::uint8_t(0) * m).relative().number()), int&&>);
static_assert(
  is_same_v<decltype((quantity_point{std::uint8_t(0) * m} - quantity_point{std::uint8_t(0) * m}).number()), int&&>);
static_assert((quantity_point{std::uint8_t(128) * m} + std::uint8_t(128) * m).relative().number() ==
              std::uint8_t(128) + std::uint8_t(128));
static_assert((std::uint8_t(128) * m + quantity_point{std::uint8_t(128) * m}).relative().number() ==
              std::uint8_t(128) + std::uint8_t(128));
static_assert((quantity_point{std::uint8_t(0) * m} - std::uint8_t(1) * m).relative().number() ==
              std::uint8_t(0) - std::uint8_t(1));
static_assert((quantity_point{std::uint8_t(0) * m} - quantity_point{std::uint8_t(1) * m}).number() ==
              std::uint8_t(0) - std::uint8_t(1));

// different representation types
static_assert(is_of_type<quantity_point{1. * m} + 1 * m,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);
static_assert(is_of_type<1. * m + quantity_point{1 * m},
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);
static_assert(is_of_type<quantity_point{1 * m} + 1. * km,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);
static_assert(is_of_type<1 * m + quantity_point{1. * km},
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);
static_assert(is_of_type<quantity_point{1 * km} + 1. * m,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);
static_assert(is_of_type<1 * km + quantity_point{1. * m},
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);

static_assert(is_of_type<quantity_point{1 * m} - 1. * m,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);
static_assert(is_of_type<quantity_point{1. * km} - 1 * m,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);
static_assert(is_of_type<quantity_point{1. * m} - 1 * km,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);

// different units
static_assert(is_of_type<quantity_point{1 * m} + 1 * km,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, int>>);
static_assert(is_of_type<1 * m + quantity_point{1 * km},
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, int>>);
static_assert(is_of_type<quantity_point{1. * m} + 1 * km,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);
static_assert(is_of_type<1. * m + quantity_point{1 * km},
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);
static_assert(is_of_type<quantity_point{1 * m} + 1. * km,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);
static_assert(is_of_type<1 * m + quantity_point{1. * km},
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);
static_assert(is_of_type<quantity_point{1. * m} + 1. * km,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);
static_assert(is_of_type<1. * m + quantity_point{1. * km},
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);

static_assert(is_of_type<quantity_point{1 * km} + 1 * m,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, int>>);
static_assert(is_of_type<1 * km + quantity_point{1 * m},
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, int>>);
static_assert(is_of_type<quantity_point{1. * km} + 1 * m,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);
static_assert(is_of_type<1. * km + quantity_point{1 * m},
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);
static_assert(is_of_type<quantity_point{1 * km} + 1. * m,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);
static_assert(is_of_type<1 * km + quantity_point{1. * m},
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);
static_assert(is_of_type<quantity_point{1. * km} + 1. * m,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);
static_assert(is_of_type<1. * km + quantity_point{1. * m},
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);

static_assert(is_of_type<quantity_point{1 * m} - 1 * km,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, int>>);
static_assert(is_of_type<quantity_point{1. * m} - 1 * km,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);
static_assert(is_of_type<quantity_point{1 * m} - 1. * km,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);
static_assert(is_of_type<quantity_point{1. * m} - 1. * km,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);

static_assert(is_of_type<quantity_point{1 * km} - 1 * m,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, int>>);
static_assert(is_of_type<quantity_point{1. * km} - 1 * m,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);
static_assert(is_of_type<quantity_point{1 * km} - 1. * m,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);
static_assert(is_of_type<quantity_point{1. * km} - 1. * m,
                         quantity_point<si::metre, absolute_point_origin<kind_of<isq::length>>{}, double>>);

static_assert(is_of_type<quantity_point{1 * m} - quantity_point{1 * km}, quantity<si::metre, int>>);
static_assert(is_of_type<quantity_point{1. * m} - quantity_point{1 * km}, quantity<si::metre, double>>);
static_assert(is_of_type<quantity_point{1 * m} - quantity_point{1. * km}, quantity<si::metre, double>>);
static_assert(is_of_type<quantity_point{1. * m} - quantity_point{1. * km}, quantity<si::metre, double>>);

static_assert(is_of_type<quantity_point{1 * km} - quantity_point{1 * m}, quantity<si::metre, int>>);
static_assert(is_of_type<quantity_point{1. * km} - quantity_point{1 * m}, quantity<si::metre, double>>);
static_assert(is_of_type<quantity_point{1 * km} - quantity_point{1. * m}, quantity<si::metre, double>>);
static_assert(is_of_type<quantity_point{1. * km} - quantity_point{1. * m}, quantity<si::metre, double>>);


static_assert((quantity_point{1 * m} + 1 * m).relative().number() == 2);
static_assert((1 * m + quantity_point{1 * m}).relative().number() == 2);
static_assert((quantity_point{1 * m} + 1 * km).relative().number() == 1001);
static_assert((1 * m + quantity_point{1 * km}).relative().number() == 1001);
static_assert((quantity_point{1 * km} + 1 * m).relative().number() == 1001);
static_assert((1 * km + quantity_point{1 * m}).relative().number() == 1001);
static_assert((quantity_point{2 * m} - 1 * m).relative().number() == 1);
static_assert((quantity_point{1 * km} - 1 * m).relative().number() == 999);

static_assert((quantity_point{1.5 * m} + 1 * m).relative().number() == 2.5);
static_assert((1.5 * m + quantity_point{1 * m}).relative().number() == 2.5);
static_assert((quantity_point{1.5 * m} + 1 * km).relative().number() == 1001.5);
static_assert((1.5 * m + quantity_point{1 * km}).relative().number() == 1001.5);
static_assert((quantity_point{1.5 * km} + 1 * m).relative().number() == 1501);
static_assert((1.5 * km + quantity_point{1 * m}).relative().number() == 1501);
static_assert((quantity_point{2.5 * m} - 1 * m).relative().number() == 1.5);
static_assert((quantity_point{1.5 * km} - 1 * m).relative().number() == 1499);

static_assert((quantity_point{1 * m} + 1.5 * m).relative().number() == 2.5);
static_assert((1 * m + quantity_point{1.5 * m}).relative().number() == 2.5);
static_assert((quantity_point{1 * m} + 1.5 * km).relative().number() == 1501);
static_assert((1 * m + quantity_point{1.5 * km}).relative().number() == 1501);
static_assert((quantity_point{1 * km} + 1.5 * m).relative().number() == 1001.5);
static_assert((1 * km + quantity_point{1.5 * m}).relative().number() == 1001.5);
static_assert((quantity_point{2 * m} - 1.5 * m).relative().number() == 0.5);
static_assert((quantity_point{1 * km} - 1.5 * m).relative().number() == 998.5);

static_assert((quantity_point{2 * m} - quantity_point{1 * m}).number() == 1);
static_assert((quantity_point{1 * km} - quantity_point{1 * m}).number() == 999);
static_assert((quantity_point{2.5 * m} - quantity_point{1 * m}).number() == 1.5);
static_assert((quantity_point{1.5 * km} - quantity_point{1 * m}).number() == 1499);
static_assert((quantity_point{2 * m} - quantity_point{1.5 * m}).number() == 0.5);
static_assert((quantity_point{1 * km} - quantity_point{1.5 * m}).number() == 998.5);

static_assert(quantity_point(42 * m, mean_sea_level) - quantity_point(42 * m, ground_level) == -42 * m);
static_assert(quantity_point(42 * m, ground_level) - quantity_point(42 * m, mean_sea_level) == 42 * m);
static_assert(quantity_point(42 * m, tower_peak) - quantity_point(42 * m, ground_level) == 42 * m);
static_assert(quantity_point(42 * m, ground_level) - quantity_point(42 * m, tower_peak) == -42 * m);
static_assert(quantity_point(42 * m, tower_peak) - quantity_point(42 * m, mean_sea_level) == 84 * m);
static_assert(quantity_point(42 * m, mean_sea_level) - quantity_point(42 * m, tower_peak) == -84 * m);
static_assert(quantity_point(42 * m, other_ground_level) - quantity_point(42 * m, ground_level) == 81 * m);
static_assert(quantity_point(42 * m, ground_level) - quantity_point(42 * m, other_ground_level) == -81 * m);
static_assert(quantity_point(42 * m, other_ground_level) - quantity_point(42 * m, tower_peak) == 39 * m);
static_assert(quantity_point(42 * m, tower_peak) - quantity_point(42 * m, other_ground_level) == -39 * m);


// commutativity and associativity
static_assert((quantity_point{10 * isq::length[si::metre] / (2 * isq::time[s])} + 5 * isq::speed[m / s]).relative() ==
              10 * isq::speed[m / s]);
static_assert((10 * isq::length[si::metre] / (2 * isq::time[s]) + quantity_point{5 * isq::speed[m / s]}).relative() ==
              10 * isq::speed[m / s]);
static_assert((quantity_point{5 * isq::speed[m / s]} + 10 * isq::length[m] / (2 * isq::time[s])).relative() ==
              10 * isq::speed[m / s]);
static_assert((5 * isq::speed[m / s] + quantity_point{10 * isq::length[m] / (2 * isq::time[s])}).relative() ==
              10 * isq::speed[m / s]);
static_assert((quantity_point{10 * isq::length[m] / (2 * isq::time[s])} - 5 * isq::speed[m / s]).relative() ==
              0 * isq::speed[m / s]);
static_assert((quantity_point{5 * isq::speed[m / s]} - 10 * isq::length[m] / (2 * isq::time[s])).relative() ==
              0 * isq::speed[m / s]);
static_assert(quantity_point{10 * isq::length[m] / (2 * isq::time[s])} - quantity_point{5 * isq::speed[m / s]} ==
              0 * isq::speed[m / s]);
static_assert(quantity_point{5 * isq::speed[m / s]} - quantity_point{10 * isq::length[m] / (2 * isq::time[s])} ==
              0 * isq::speed[m / s]);

// NOTE: quantity_spec of the origin is not "upgraded" to a better type
static_assert(is_of_type<quantity_point{10 * isq::length[m] / (2 * isq::time[s])} + 5 * isq::speed[m / s],
                         quantity_point<isq::speed[m / s], absolute_point_origin<isq::length / isq::time>{}, int>>);
static_assert(is_of_type<10 * isq::length[m] / (2 * isq::time[s]) + quantity_point{5 * isq::speed[m / s]},
                         quantity_point<isq::speed[m / s], absolute_point_origin<isq::speed>{}, int>>);
static_assert(is_of_type<quantity_point{5 * isq::speed[m / s]} + 10 * isq::length[m] / (2 * isq::time[s]),
                         quantity_point<isq::speed[m / s], absolute_point_origin<isq::speed>{}, int>>);
static_assert(is_of_type<5 * isq::speed[m / s] + quantity_point{10 * isq::length[m] / (2 * isq::time[s])},
                         quantity_point<isq::speed[m / s], absolute_point_origin<isq::length / isq::time>{}, int>>);
static_assert(is_of_type<quantity_point{10 * isq::length[m] / (2 * isq::time[s])} - 5 * isq::speed[m / s],
                         quantity_point<isq::speed[m / s], absolute_point_origin<isq::length / isq::time>{}, int>>);
static_assert(is_of_type<quantity_point{5 * isq::speed[m / s]} - 10 * isq::length[m] / (2 * isq::time[s]),
                         quantity_point<isq::speed[m / s], absolute_point_origin<isq::speed>{}, int>>);
static_assert(
  is_of_type<quantity_point{10 * isq::length[m] / (2 * isq::time[s])} - quantity_point{5 * isq::speed[m / s]},
             quantity<isq::speed[m / s], int>>);
static_assert(
  is_of_type<quantity_point{5 * isq::speed[m / s]} - quantity_point{10 * isq::length[m] / (2 * isq::time[s])},
             quantity<isq::speed[m / s], int>>);

// NOTE: 1 / isq::time[s] works for quantities but not for quantity_point (origin can't be weakened)
static_assert((quantity_point{10 / (2 * isq::period_duration[s])} + 5 * isq::frequency[Hz]).relative() ==
              10 * isq::frequency[Hz]);
static_assert((10 / (2 * isq::period_duration[s]) + quantity_point{5 * isq::frequency[Hz]}).relative() ==
              10 * isq::frequency[Hz]);
static_assert((quantity_point{5 * isq::frequency[Hz]} + 10 / (2 * isq::period_duration[s])).relative() ==
              10 * isq::frequency[Hz]);
static_assert((5 * isq::frequency[Hz] + quantity_point{10 / (2 * isq::period_duration[s])}).relative() ==
              10 * isq::frequency[Hz]);
static_assert((quantity_point{10 / (2 * isq::period_duration[s])} - 5 * isq::frequency[Hz]).relative() ==
              0 * isq::frequency[Hz]);
static_assert((quantity_point{5 * isq::frequency[Hz]} - 10 / (2 * isq::period_duration[s])).relative() ==
              0 * isq::frequency[Hz]);
static_assert(quantity_point{10 / (2 * isq::period_duration[s])} - quantity_point{5 * isq::frequency[Hz]} ==
              0 * isq::frequency[Hz]);
static_assert(quantity_point{5 * isq::frequency[Hz]} - quantity_point{10 / (2 * isq::period_duration[s])} ==
              0 * isq::frequency[Hz]);

// NOTE: quantity_spec of the origin is not "upgraded" to a better type
static_assert(is_of_type<quantity_point{10 / (2 * isq::period_duration[s])} + 5 * isq::frequency[Hz],
                         quantity_point<isq::frequency[Hz], absolute_point_origin<1 / isq::period_duration>{}, int>>);
static_assert(is_of_type<10 / (2 * isq::period_duration[s]) + quantity_point{5 * isq::frequency[Hz]},
                         quantity_point<isq::frequency[Hz], absolute_point_origin<isq::frequency>{}, int>>);
static_assert(is_of_type<quantity_point{5 * isq::frequency[Hz]} + 10 / (2 * isq::period_duration[s]),
                         quantity_point<isq::frequency[Hz], absolute_point_origin<isq::frequency>{}, int>>);
static_assert(is_of_type<5 * isq::frequency[Hz] + quantity_point{10 / (2 * isq::period_duration[s])},
                         quantity_point<isq::frequency[Hz], absolute_point_origin<1 / isq::period_duration>{}, int>>);
static_assert(is_of_type<quantity_point{10 / (2 * isq::period_duration[s])} - 5 * isq::frequency[Hz],
                         quantity_point<isq::frequency[Hz], absolute_point_origin<1 / isq::period_duration>{}, int>>);
static_assert(is_of_type<quantity_point{5 * isq::frequency[Hz]} - 10 / (2 * isq::period_duration[s]),
                         quantity_point<isq::frequency[Hz], absolute_point_origin<isq::frequency>{}, int>>);
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

static_assert(invalid_addition(quantity_point{5 * activity[Bq]}, 5 * isq::frequency[Hz]));
static_assert(invalid_addition(5 * activity[Bq], quantity_point{5 * isq::frequency[Hz]}));
static_assert(invalid_subtraction(quantity_point{5 * activity[Bq]}, 5 * isq::frequency[Hz]));
static_assert(invalid_subtraction(quantity_point{5 * activity[Bq]}, quantity_point{5 * isq::frequency[Hz]}));

static_assert(invalid_addition(quantity_point{5 * activity[Bq]}, 10 / (2 * isq::time[s]), 5 * isq::frequency[Hz]));
static_assert(invalid_addition(5 * activity[Bq], quantity_point{10 / (2 * isq::time[s])}, 5 * isq::frequency[Hz]));
static_assert(invalid_addition(5 * activity[Bq], 10 / (2 * isq::time[s]), quantity_point{5 * isq::frequency[Hz]}));
static_assert(invalid_subtraction(quantity_point{5 * activity[Bq]}, 10 / (2 * isq::time[s]), 5 * isq::frequency[Hz]));

}  // namespace
