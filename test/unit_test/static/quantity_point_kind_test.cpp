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
#include "units/chrono.h"
#include "units/physical/si/cgs/derived/speed.h"
#include "units/physical/si/derived/area.h"
#include "units/physical/si/derived/frequency.h"
#include "units/physical/si/derived/speed.h"
#include "units/physical/si/fps/derived/speed.h"
#include "units/quantity_point.h"
#include "units/quantity_point_kind.h"
#include <cassert>
#include <concepts>
#include <functional>

namespace {

using namespace units;
namespace si = physical::si;
using namespace si;
using namespace unit_constants;
using sys_clock_origin = chrono_clock_point_origin<std::chrono::system_clock>;
using sys_seconds = std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>;

constexpr auto cgs_cm = cgs::unit_constants::cm;

using namespace std::chrono_literals;

struct width_kind : kind<width_kind, dim_length> {};
struct height_kind : kind<height_kind, dim_length> {};
struct abscissa_kind : point_kind<abscissa_kind, width_kind> {};
struct ordinate_kind : point_kind<ordinate_kind, height_kind> {};

struct distance_kind : kind<distance_kind, dim_length> {};
struct cgs_width_kind : kind<cgs_width_kind, cgs::dim_length> {};
struct cgs_height_kind : kind<cgs_height_kind, cgs::dim_length> {};
struct rate_of_climb_kind : derived_kind<rate_of_climb_kind, height_kind, dim_speed> {};
struct altitude_kind : point_kind<altitude_kind, cgs_height_kind> {};

struct apple : kind<apple, dim_one> {};
struct orange : kind<orange, dim_one> {};
struct nth_apple_kind : point_kind<nth_apple_kind, apple> {};
struct nth_orange_kind : point_kind<nth_orange_kind, orange> {};

struct time_kind : kind<time_kind, dim_time> {};
struct time_point_kind : point_kind<time_point_kind, time_kind> {};

template <Unit U, QuantityValue Rep = double> using width = quantity_kind<width_kind, U, Rep>;
template <Unit U, QuantityValue Rep = double> using height = quantity_kind<height_kind, U, Rep>;
template <Unit U, QuantityValue Rep = double> using abscissa = quantity_point_kind<abscissa_kind, U, Rep>;
template <Unit U, QuantityValue Rep = double> using ordinate = quantity_point_kind<ordinate_kind, U, Rep>;

template <Unit U, QuantityValue Rep = double> using distance = quantity_kind<distance_kind, U, Rep>;
template <Unit U, QuantityValue Rep = double> using cgs_width = quantity_kind<cgs_width_kind, U, Rep>;
template <Unit U, QuantityValue Rep = double> using cgs_height = quantity_kind<cgs_height_kind, U, Rep>;
template <Unit U, QuantityValue Rep = double> using rate_of_climb = quantity_kind<rate_of_climb_kind, U, Rep>;
template <Unit U, QuantityValue Rep = double> using altitude = quantity_point_kind<altitude_kind, U, Rep>;

template <Unit U = one, QuantityValue Rep = double> using apples = quantity_kind<apple, U, Rep>;
template <Unit U = one, QuantityValue Rep = double> using oranges = quantity_kind<orange, U, Rep>;
template <Unit U = one, QuantityValue Rep = double> using nth_apple = quantity_point_kind<nth_apple_kind, U, Rep>;
template <Unit U = one, QuantityValue Rep = double> using nth_orange = quantity_point_kind<nth_orange_kind, U, Rep>;

/////////////
// concepts
/////////////

static_assert(QuantityPointKind<abscissa<metre>>);
static_assert(QuantityPointKind<nth_apple<one>>);
static_assert(!QuantityPointKind<double>);
static_assert(!QuantityPointKind<length<metre>>);
static_assert(!QuantityPointKind<quantity_point<dim_length, metre>>);
static_assert(!QuantityPointKind<width<metre>>);

static_assert(QuantityPointKindOf<abscissa<metre>, abscissa_kind>);
static_assert(!QuantityPointKindOf<abscissa<metre>, ordinate_kind>);
static_assert(!QuantityPointKindOf<abscissa<metre>, metre>);
static_assert(!QuantityPointKindOf<length<metre>, abscissa_kind>);
static_assert(!QuantityPointKindOf<length<metre>, metre>);
static_assert(!QuantityPointKindOf<width<metre>, abscissa_kind>);
static_assert(!QuantityPointKindOf<width<metre>, width_kind>);
static_assert(!QuantityPointKindOf<width<metre>, metre>);
static_assert(!QuantityPointKindOf<quantity_point<dim_length, metre>, width_kind>);
static_assert(!QuantityPointKindOf<quantity_point<dim_length, metre>, dim_length>);
static_assert(!QuantityPointKindOf<quantity_point<dim_length, metre>, metre>);


///////////////
// invariants
///////////////

static_assert(sizeof(abscissa<metre, double>) == sizeof(double));
static_assert(sizeof(ordinate<metre, short>) == sizeof(short));

template<typename Width, typename Abscissa>
concept invalid_types = requires {
  requires !requires { typename quantity_point_kind<Width, metre, int>; };      // width_kind is not a point kind
  requires !requires { typename quantity_point_kind<Abscissa, second, int>; };  // unit of a different dimension
  requires !requires { typename quantity_point_kind<Abscissa, metre, length<metre>>; };  // quantity used as Rep
  requires !requires { typename quantity_point_kind<Abscissa, metre, quantity_point<dim_length, metre>>; };  // quantity point used as Rep
  requires !requires { typename quantity_point_kind<Abscissa, metre, width<metre>>; };  // quantity kind used as Rep
  requires !requires { typename quantity_point_kind<Abscissa, metre, abscissa<metre>>; };  // quantity point kind used as Rep
  requires !requires { typename quantity_point_kind<metre, Abscissa, double, default_point_origin<metre>>; };  // reordered arguments
  requires !requires { typename quantity_point_kind<metre, double, Abscissa, default_point_origin<metre>>; };  // reordered arguments
};
static_assert(invalid_types<width_kind, abscissa_kind>);

static_assert(std::is_trivially_default_constructible_v<abscissa<metre>>);
static_assert(std::is_trivially_copy_constructible_v<abscissa<metre>>);
static_assert(std::is_trivially_move_constructible_v<abscissa<metre>>);
static_assert(std::is_trivially_copy_assignable_v<abscissa<metre>>);
static_assert(std::is_trivially_move_assignable_v<abscissa<metre>>);
static_assert(std::is_trivially_destructible_v<abscissa<metre>>);

static_assert(std::is_nothrow_default_constructible_v<abscissa<metre>>);
static_assert(std::is_nothrow_copy_constructible_v<abscissa<metre>>);
static_assert(std::is_nothrow_move_constructible_v<abscissa<metre>>);
static_assert(std::is_nothrow_copy_assignable_v<abscissa<metre>>);
static_assert(std::is_nothrow_move_assignable_v<abscissa<metre>>);
static_assert(std::is_nothrow_destructible_v<abscissa<metre>>);

static_assert(std::is_trivially_copyable_v<abscissa<metre>>);
static_assert(std::is_standard_layout_v<abscissa<metre>>);

static_assert(std::default_initializable<abscissa<metre>>);
static_assert(std::move_constructible<abscissa<metre>>);
static_assert(std::copy_constructible<abscissa<metre>>);
static_assert(std::equality_comparable<abscissa<metre>>);
static_assert(std::totally_ordered<abscissa<metre>>);
static_assert(std::regular<abscissa<metre>>);

static_assert(std::three_way_comparable<abscissa<metre>>);

static_assert(!std::is_aggregate_v<abscissa<metre>>);


///////////////////
// member aliases
///////////////////

static_assert(is_same_v<abscissa<metre>::point_kind_type, abscissa_kind>);
static_assert(is_same_v<abscissa<metre>::kind_type, width_kind>);
static_assert(is_same_v<abscissa<metre>::quantity_kind_type, width<metre>>);
static_assert(is_same_v<abscissa<metre>::quantity_type, length<metre>>);
static_assert(is_same_v<abscissa<metre>::dimension, dim_length>);
static_assert(is_same_v<abscissa<metre>::unit, metre>);
static_assert(is_same_v<abscissa<metre>::rep, double>);


//////////////////////
// relative observer
//////////////////////

static_assert(same(abscissa<metre>{}.relative(), width<metre>{}));


////////////////////////////
// static member functions
////////////////////////////

static_assert(abscissa<metre, unsigned>::min().relative().common() == 0 * m);
static_assert(abscissa<metre, unsigned>::max().relative().common() == std::numeric_limits<unsigned>::max() * m);
static_assert(abscissa<metre, double>::min().relative().common().count() == std::numeric_limits<double>::lowest());
static_assert(abscissa<metre, double>::max().relative().common().count() == std::numeric_limits<double>::max());


////////////////////////
// default constructor
////////////////////////

// default initialization
#if !defined(COMP_MSVC)
static_assert([] {
  const auto read_uninitialized_quantity = [] {
    abscissa<metre> w;
    ++w;
  };
  return !require_constant_invocation<read_uninitialized_quantity>;
}());
#endif

// value initialization
static_assert(abscissa<metre>{}.relative().common() == 0 * m);


/////////
// CTAD
/////////

static_assert(same(quantity_point_kind(width<metre, int>(0 * m)), abscissa<metre, int>{}));
static_assert(same(quantity_point_kind(abscissa<metre, int>(0 * m)), abscissa<metre, int>{}));


////////////////////////////
// construction from a rep
////////////////////////////

static_assert(construct_from_only<nth_apple<one, double>>(1.0).relative().common() == 1);
static_assert(construct_from_only<nth_apple<one, double>>(1.0f).relative().common() == 1);
static_assert(construct_from_only<nth_apple<one, double>>(1).relative().common() == 1);
static_assert(construct_from_only<nth_apple<one, double>>(short{1}).relative().common() == 1);
static_assert(construct_from_only<nth_apple<one, short>>(1).relative().common() == 1);
static_assert(construct_from_only<nth_apple<one, int>>(1).relative().common() == 1);
static_assert(construct_from_only<nth_apple<one, double>>(one_rep{}).relative().common() == 1);
static_assert(construct_from_only<nth_apple<one, int>>(one_rep{}).relative().common() == 1);
static_assert(construct_from_only<nth_apple<one, short>>(one_rep{}).relative().common() == 1);
static_assert(construct_from_only<nth_apple<percent, int>>(1ULL).relative().common().count() == 1);
static_assert(construct_from_only<nth_apple<percent, double>>(1).relative().common().count() == 1);
static_assert(!constructible_or_convertible_from<nth_apple<percent, int>>(1.0));
static_assert(!constructible_or_convertible_from<nth_apple<one, int>>(1.0));
static_assert(!constructible_or_convertible_from<abscissa<metre, double>>(1.0));
static_assert(!constructible_or_convertible_from<abscissa<metre, double>>(1.0f));
static_assert(!constructible_or_convertible_from<abscissa<metre, double>>(1));
static_assert(!constructible_or_convertible_from<abscissa<metre, double>>(short{1}));
static_assert(!constructible_or_convertible_from<abscissa<metre, short>>(1));


/////////////////////////////////
// construction from a quantity
/////////////////////////////////

// clang-format off
static_assert(construct_from_only<abscissa<metre, short>>(1 * m).relative().common() == 1 * m);
static_assert(construct_from_only<abscissa<metre, int>>(1 * m).relative().common() == 1 * m);
static_assert(construct_from_only<abscissa<metre, int>>(1 * km).relative().common() == 1 * km);
static_assert(construct_from_only<abscissa<metre, int>>(1ULL * m).relative().common() == 1 * m);
// static_assert(construct_from_only<abscissa<metre, int>>(1 * cgs_cm).relative().common() == 1 * cm);  // TODO: Fix #210
static_assert(construct_from_only<abscissa<metre, double>>(1 * m).relative().common() == 1 * m);
static_assert(construct_from_only<abscissa<metre, double>>(1.0 * km).relative().common() == 1 * km);
static_assert(construct_from_only<abscissa<metre, double>>(1 * cgs_cm).relative().common() == 1 * cm);
static_assert(construct_from_only<abscissa<metre, double>>(1.0 * cgs_cm).relative().common() == 1 * cm);
static_assert(!constructible_or_convertible_from<abscissa<metre, int>>(1 * mm));
static_assert(!constructible_or_convertible_from<abscissa<metre, int>>(1.0 * m));
static_assert(!constructible_or_convertible_from<abscissa<metre, int>>(1.0 * km));
static_assert(!constructible_or_convertible_from<abscissa<metre, int>>(1 * cgs_cm));
static_assert(!constructible_or_convertible_from<abscissa<metre, int>>(quantity(1)));
static_assert(!constructible_or_convertible_from<abscissa<metre, int>>(1 * s));
static_assert(!constructible_or_convertible_from<abscissa<metre, int>>(1s));

static_assert(construct_from_only<nth_apple<one, int>>(quantity(1)).relative().common() == 1);
static_assert(construct_from_only<nth_apple<one, double>>(dimensionless<percent>(1)).relative().common() == 0.01);
static_assert(construct_from_only<nth_apple<one, double>>(dimensionless<percent>(1)).relative().common() == 0.01);
static_assert(construct_from_only<nth_apple<percent, double>>(dimensionless<percent>(1)).relative().common().count() == 1);
static_assert(construct_from_only<nth_apple<percent, double>>(quantity(1)).relative().common().count() == 100);
static_assert(!constructible_or_convertible_from<nth_apple<percent, int>>(quantity(1.0)));
static_assert(!constructible_or_convertible_from<nth_apple<percent, int>>(dimensionless<percent>(1)));
static_assert(!constructible_or_convertible_from<nth_apple<one, int>>(quantity(1.0)));
static_assert(!constructible_or_convertible_from<nth_apple<one, int>>(dimensionless<percent>(1)));
static_assert(!constructible_or_convertible_from<nth_apple<one, int>>(1 * m));
static_assert(!constructible_or_convertible_from<nth_apple<one, int>>(1 * s));
static_assert(!constructible_or_convertible_from<nth_apple<one, int>>(1s));
// clang-format on


///////////////////////////////////////
// construction from a quantity point
///////////////////////////////////////

static_assert(construct_from_only<abscissa<metre, short>>(1 * m).relative().common() == 1 * m);
static_assert(construct_from_only<abscissa<metre, int>>(quantity_point(short{1} * m)).relative().common() == 1 * m);
static_assert(construct_from_only<abscissa<metre, int>>(quantity_point(1 * m)).relative().common() == 1 * m);
static_assert(construct_from_only<abscissa<metre, int>>(quantity_point(1 * km)).relative().common() == 1 * km);
static_assert(construct_from_only<abscissa<metre, double>>(quantity_point(1 * m)).relative().common() == 1 * m);
static_assert(construct_from_only<abscissa<metre, double>>(quantity_point(1 * km)).relative().common() == 1 * km);
static_assert(construct_from_only<abscissa<metre, double>>(quantity_point(1.0 * m)).relative().common() == 1 * m);
static_assert(construct_from_only<abscissa<metre, double>>(quantity_point(1.0 * mm)).relative().common() == 1 * mm);
static_assert(!constructible_or_convertible_from<abscissa<metre, int>>(quantity_point(1 * mm)));
static_assert(!constructible_or_convertible_from<abscissa<metre, int>>(quantity_point(1.0 * m)));
static_assert(!constructible_or_convertible_from<abscissa<metre, int>>(quantity_point(1.0 * km)));
static_assert(!constructible_or_convertible_from<abscissa<metre, double>>(quantity_point(1.0 * m * m)));
static_assert(!constructible_or_convertible_from<abscissa<metre, double>>(quantity_point(1.0 * s)));

// clang-format off
static_assert(construct_from_only<nth_apple<one, short>>(quantity_point(1)).relative().common() == 1);
static_assert(construct_from_only<nth_apple<one, int>>(quantity_point(1)).relative().common() == 1);
static_assert(construct_from_only<nth_apple<one, double>>(quantity_point(1)).relative().common() == 1);
static_assert(construct_from_only<nth_apple<one, double>>(quantity_point(dimensionless<percent, int>(1))).relative().common() == 0.01);
static_assert(construct_from_only<nth_apple<one, double>>(quantity_point(1.0)).relative().common() == 1);
static_assert(construct_from_only<nth_apple<one, double>>(quantity_point(dimensionless<percent, double>(1.0))).relative().common() == 0.01);
static_assert(construct_from_only<nth_apple<percent, int>>(quantity_point(1)).relative().common().count() == 100);
static_assert(construct_from_only<nth_apple<percent, double>>(quantity_point(dimensionless<percent>(1))).relative().common().count() == 1);
static_assert(!constructible_or_convertible_from<nth_apple<percent, int>>(quantity_point(1.0)));
static_assert(!constructible_or_convertible_from<nth_apple<percent, int>>(quantity_point(dimensionless<percent>(1))));
static_assert(!constructible_or_convertible_from<nth_apple<one, int>>(quantity_point(1.0)));
static_assert(!constructible_or_convertible_from<nth_apple<one, int>>(quantity_point(dimensionless<percent, int>(1))));
static_assert(!constructible_or_convertible_from<nth_apple<one, int>>(quantity_point(dimensionless<percent, double>(1))));
static_assert(!constructible_or_convertible_from<nth_apple<one, double>>(quantity_point(1.0 * s)));

static_assert(construct_from_only<quantity_point_kind<time_point_kind, second, int, sys_clock_origin>>(sys_seconds{42s}).relative().common() == 42 * s);
// clang-format on


//////////////////////////////////////
// construction from a quantity kind
//////////////////////////////////////

// clang-format off
static_assert(construct_from_only<abscissa<metre, int>>(width<metre, int>(1 * m)).relative().common() == 1 * m);
static_assert(construct_from_only<abscissa<metre, int>>(width<kilometre, unsigned long long>(1ULL * km)).relative().common() == 1 * km);
static_assert(construct_from_only<abscissa<centimetre, int>>(width<cgs::centimetre, int>(1 * cgs_cm)).relative().common() == 1 * cm);
static_assert(construct_from_only<abscissa<fps::foot, double>>(width<cgs::centimetre, int>(1 * cgs_cm)).relative().common() == 1 * cm);
static_assert(construct_from_only<abscissa<metre, double>>(width<metre, int>(1 * m)).relative().common() == 1 * m);
static_assert(construct_from_only<abscissa<metre, double>>(width<millimetre, double>(1.0 * mm)).relative().common() == 1 * mm);
static_assert(construct_from_only<abscissa<metre, double>>(width<kilometre, unsigned long long>(1ULL * km)).relative().common() == 1 * km);
static_assert(!constructible_or_convertible_from<abscissa<metre, int>>(width<metre, double>(1.0 * m)));
static_assert(!constructible_or_convertible_from<abscissa<metre, int>>(width<millimetre, int>(1 * mm)));
static_assert(!constructible_or_convertible_from<abscissa<metre, int>>(height<metre, int>(1 * m)));
static_assert(!constructible_or_convertible_from<abscissa<metre, int>>(abscissa_kind{}, width<metre, double>(1.0 * m)));
static_assert(!constructible_or_convertible_from<abscissa<metre, int>>(abscissa_kind{}, width<millimetre, int>(1 * mm)));
static_assert(!constructible_or_convertible_from<abscissa<metre, int>>(abscissa_kind{}, height<metre, int>(1 * m)));

static_assert(construct_from_only<nth_apple<one, int>>(apples<one, int>(1)).relative().common() == 1);
static_assert(construct_from_only<nth_apple<one, double>>(apples<percent, double>(dimensionless<percent>(1))).relative().common() == 0.01);
static_assert(construct_from_only<nth_apple<percent, int>>(apples<one, int>(1)).relative().common().count() == 100);
static_assert(construct_from_only<nth_apple<percent, double>>(apples<percent, double>(dimensionless<percent>(1))).relative().common().count() == 1);
static_assert(!constructible_or_convertible_from<nth_apple<percent, int>>(apples<one, double>(1.0)));
static_assert(!constructible_or_convertible_from<nth_apple<percent, int>>(apples<percent, double>(dimensionless<percent>(1))));
static_assert(!constructible_or_convertible_from<nth_apple<one, int>>(apples<one, double>(1.0)));
static_assert(!constructible_or_convertible_from<nth_apple<one, int>>(apples<percent, double>(dimensionless<percent>(1))));
static_assert(!constructible_or_convertible_from<nth_apple<one, int>>(oranges<one, int>(1)));
// clang-format on


//////////////////////////////////////////////////
// construction from another quantity point kind
//////////////////////////////////////////////////

// clang-format off
static_assert(construct_and_convert_from<abscissa<metre, int>>(abscissa<metre, int>(1 * m)).relative().common() == 1 * m);
static_assert(construct_and_convert_from<abscissa<metre, int>>(abscissa<kilometre, unsigned long long>(1ULL * km)).relative().common() == 1 * km);
static_assert(construct_and_convert_from<abscissa<metre, double>>(abscissa<metre, unsigned long long>(1ULL * m)).relative().common() == 1 * m);
static_assert(construct_and_convert_from<abscissa<metre, double>>(abscissa<cgs::centimetre, int>(1 * cgs_cm)).relative().common() == 1 * cm);
static_assert(construct_and_convert_from<abscissa<fps::foot, double>>(abscissa<cgs::centimetre, int>(1 * cgs_cm)).relative().common() == 1 * cm);
static_assert(!constructible_or_convertible_from<abscissa<metre, int>>(abscissa<metre, double>(1.0 * m)));
static_assert(!constructible_or_convertible_from<abscissa<kilometre, int>>(abscissa<metre, int>(1 * m)));
static_assert(!constructible_or_convertible_from<abscissa<metre, int>>(ordinate<metre, int>(1 * m)));
static_assert(!constructible_or_convertible_from<abscissa<metre, int>>(quantity_point_kind<time_point_kind, second, int>(1 * s)));

static_assert(construct_and_convert_from<nth_apple<one, int>>(nth_apple<one, int>(1)).relative().common() == 1);
static_assert(construct_and_convert_from<nth_apple<one, double>>(nth_apple<percent, double>(dimensionless<percent>(1))).relative().common() == 0.01);
static_assert(construct_and_convert_from<nth_apple<percent, int>>(nth_apple<one, int>(1)).relative().common().count() == 100);
static_assert(construct_and_convert_from<nth_apple<percent, double>>(nth_apple<percent, double>(dimensionless<percent>(1))).relative().common().count() == 1);
static_assert(!constructible_or_convertible_from<nth_apple<percent, int>>(nth_apple<one, double>(1.0)));
static_assert(!constructible_or_convertible_from<nth_apple<percent, int>>(nth_apple<percent, double>(dimensionless<percent>(1))));
static_assert(!constructible_or_convertible_from<nth_apple<one, int>>(nth_apple<one, double>(1.0)));
static_assert(!constructible_or_convertible_from<nth_apple<one, int>>(nth_apple<percent, int>(dimensionless<percent, int>(1))));
static_assert(!constructible_or_convertible_from<nth_apple<one, int>>(nth_orange<one, int>(1)));
static_assert(!constructible_or_convertible_from<nth_apple<one, int>>(abscissa<metre, int>(1 * m)));
// clang-format on


//////////////////////
// other conversions
//////////////////////

static_assert(!std::is_convertible_v<abscissa<metre, int>, int>);
static_assert(!std::is_convertible_v<abscissa<metre, int>, dimensionless<one, int>>);
static_assert(!std::is_convertible_v<abscissa<metre, int>, length<metre, int>>);
static_assert(!std::is_convertible_v<abscissa<metre, int>, width<metre, int>>);
static_assert(!std::is_convertible_v<abscissa<metre, int>, height<metre, int>>);
static_assert(!std::is_convertible_v<abscissa<metre, int>, quantity_point<dim_length, metre, int>>);


////////////////////////
// assignment operator
////////////////////////

// clang-format off
static_assert((abscissa<metre, int>(2 * m) = abscissa<metre, int>(1 * m)).relative().common() == 1 * m);
static_assert((abscissa<metre, int>(2 * m) = abscissa<kilometre, int>(1 * km)).relative().common() == 1 * km);
static_assert(!std::is_assignable_v<abscissa<metre, int>, abscissa<metre, double>>);
static_assert(!std::is_assignable_v<abscissa<metre, int>, abscissa<millimetre, int>>);
// clang-format on


/////////////////////
// member operators
/////////////////////

#if !defined(COMP_MSVC) || defined(NDEBUG)
static_assert([]() {
  const width<metre, int> w(1 * m);
  quantity_point_kind x(w);
  assert(&++x == &x && x.relative().common() == 2 * m);
  assert(&--x == &x && x.relative().common() == 1 * m);
  assert((x++).relative().common() == 1 * m && x.relative().common() == 2 * m);
  assert((x--).relative().common() == 2 * m && x.relative().common() == 1 * m);
  assert(&(x += w) == &x && x.relative().common() == 2 * m);
  assert(&(x -= w) == &x && x.relative().common() == 1 * m);
  return true;
}());
#endif

template<typename PK, typename U, typename Qx>
concept invalid_compound_assignments_ = requires(quantity_point_kind<PK, U, int> x, Qx q) {
  requires !requires { x += q; };
  requires !requires { x -= q; };
};
template<typename Abscissa>
concept invalid_compound_assignments = requires(quantity_point_kind<Abscissa, metre, int> x) {
  requires !requires { x += 1; };
  requires !requires { x -= 1; };
  requires invalid_compound_assignments_<Abscissa, metre, length<metre, int>>;
  requires invalid_compound_assignments_<Abscissa, metre, height<metre, int>>;
  requires invalid_compound_assignments_<Abscissa, metre, rate_of_climb<metre_per_second, int>>;
  requires invalid_compound_assignments_<Abscissa, metre, quantity_point<dim_length, metre, int>>;
  requires invalid_compound_assignments_<Abscissa, metre, std::chrono::seconds>;
};
static_assert(invalid_compound_assignments<abscissa_kind>);
static_assert(invalid_compound_assignments_<time_point_kind, second, std::chrono::seconds>);
#if  __cpp_lib_chrono >= 201907L
static_assert(invalid_compound_assignments_<time_point_kind, second, std::chrono::sys_seconds>);
#endif


/////////////////////////
// non-member operators
/////////////////////////

// clang-format off
static_assert(same(abscissa<metre, int>(2 * m) + width<metre, int>(3 * m), abscissa<metre, int>(5 * m)));
static_assert(same(abscissa<metre, int>(2 * m) + width<metre, double>(3. * m), abscissa<metre, double>(5. * m)));
static_assert(same(abscissa<metre, double>(2. * m) + width<metre, int>(3 * m), abscissa<metre, double>(5. * m)));
static_assert(same(abscissa<kilometre, int>(2 * km) + width<metre, double>(3e3 * m), abscissa<metre, double>(5e3 * m)));
static_assert(same(abscissa<metre, double>(2e3 * m) + width<kilometre, int>(3 * km), abscissa<metre, double>(5e3 * m)));
static_assert(same(width<metre, int>(2 * m) + abscissa<metre, int>(3 * m), abscissa<metre, int>(5 * m)));
static_assert(same(width<metre, int>(2 * m) + abscissa<metre, double>(3. * m), abscissa<metre, double>(5. * m)));
static_assert(same(width<metre, double>(2. * m) + abscissa<metre, int>(3 * m), abscissa<metre, double>(5. * m)));
static_assert(same(width<kilometre, int>(2 * km) + abscissa<metre, double>(3e3 * m), abscissa<metre, double>(5e3 * m)));
static_assert(same(width<metre, double>(2e3 * m) + abscissa<kilometre, int>(3 * km), abscissa<metre, double>(5e3 * m)));
static_assert(!std::is_invocable_v<std::plus<>, abscissa<metre>, double>);
static_assert(!std::is_invocable_v<std::plus<>, abscissa<metre>, length<metre>>);
static_assert(!std::is_invocable_v<std::plus<>, abscissa<metre>, quantity_point<dim_length, metre>>);
static_assert(!std::is_invocable_v<std::plus<>, abscissa<metre>, height<metre>>);
static_assert(!std::is_invocable_v<std::plus<>, abscissa<metre>, abscissa<metre>>);
static_assert(!std::is_invocable_v<std::plus<>, abscissa<metre>, abscissa<kilometre>>);
static_assert(!std::is_invocable_v<std::plus<>, abscissa<kilometre>, abscissa<metre>>);
static_assert(!std::is_invocable_v<std::plus<>, height<metre>, abscissa<metre>>);
static_assert(!std::is_invocable_v<std::plus<>, quantity_point<dim_length, metre>, abscissa<metre>>);
static_assert(!std::is_invocable_v<std::plus<>, length<metre>, abscissa<metre>>);
static_assert(!std::is_invocable_v<std::plus<>, double, abscissa<metre>>);

static_assert(same(abscissa<metre, int>(2 * m) - width<metre, int>(3 * m), abscissa<metre, int>(-1 * m)));
static_assert(same(abscissa<metre, int>(2 * m) - width<metre, double>(3. * m), abscissa<metre, double>(-1. * m)));
static_assert(same(abscissa<metre, double>(2. * m) - width<metre, int>(3 * m), abscissa<metre, double>(-1. * m)));
static_assert(same(abscissa<kilometre, int>(2 * km) - width<metre, double>(3e3 * m), abscissa<metre, double>(-1e3 * m)));
static_assert(same(abscissa<metre, double>(2e3 * m) - width<kilometre, int>(3 * km), abscissa<metre, double>(-1e3 * m)));
static_assert(same(abscissa<metre, int>(2 * m) - abscissa<metre, int>(3 * m), width<metre, int>(-1 * m)));
static_assert(same(abscissa<metre, int>(2 * m) - abscissa<metre, double>(3. * m), width<metre, double>(-1. * m)));
static_assert(same(abscissa<metre, double>(2. * m) - abscissa<metre, int>(3 * m), width<metre, double>(-1. * m)));
static_assert(same(abscissa<kilometre, int>(2 * km) - abscissa<metre, double>(3e3 * m), width<metre, double>(-1e3 * m)));
static_assert(same(abscissa<metre, double>(2e3 * m) - abscissa<kilometre, int>(3 * km), width<metre, double>(-1e3 * m)));
static_assert(!std::is_invocable_v<std::minus<>, abscissa<metre>, double>);
static_assert(!std::is_invocable_v<std::minus<>, abscissa<metre>, length<metre>>);
static_assert(!std::is_invocable_v<std::minus<>, abscissa<metre>, quantity_point<dim_length, metre>>);
static_assert(!std::is_invocable_v<std::minus<>, abscissa<metre>, height<metre>>);
static_assert(!std::is_invocable_v<std::minus<>, abscissa<metre>, ordinate<metre>>);
static_assert(!std::is_invocable_v<std::minus<>, ordinate<metre>, abscissa<metre>>);
static_assert(!std::is_invocable_v<std::minus<>, height<metre>, abscissa<metre>>);
static_assert(!std::is_invocable_v<std::minus<>, quantity_point<dim_length, metre>, abscissa<metre>>);
static_assert(!std::is_invocable_v<std::minus<>, length<metre>, abscissa<metre>>);
static_assert(!std::is_invocable_v<std::minus<>, double, abscissa<metre>>);
// clang-format on


/////////////////////////
// comparison operators
/////////////////////////

// clang-format off
static_assert(abscissa<metre, int>(1 * m) == abscissa<metre, int>(1 * m));
static_assert(abscissa<metre, int>(1 * m) == abscissa<metre, double>(1.0 * m));
static_assert(abscissa<metre, int>(1 * m) == abscissa<millimetre, int>(1000 * mm));
static_assert(abscissa<metre, int>(1 * m) == abscissa<millimetre, double>(1e3 * mm));
static_assert(abscissa<metre, int>(2 * m) != abscissa<metre, int>(1 * m));
static_assert(abscissa<metre, int>(2 * m) != abscissa<cgs::centimetre, double>(1.0 * cgs_cm));
static_assert(std::equality_comparable_with<abscissa<metre, int>, abscissa<metre, double>>);
static_assert(std::equality_comparable_with<abscissa<nanometre, int>, abscissa<kilometre, int>>);
static_assert(std::equality_comparable_with<abscissa<cgs::centimetre, int>, abscissa<millimetre, double>>);
static_assert(std::equality_comparable_with<abscissa<metre>, abscissa<cgs::centimetre>>);
// clang-format on
template<typename Abscissa>
concept invalid_equality = requires(quantity_point_kind<Abscissa, metre, int> x) {
  requires !requires { x == 1; };
  requires !requires { x != 1.0; };
  requires !requires { x == 1 * m; };
  requires !requires { x != 1.0 * cgs_cm; };
  requires !requires { x == 1 * km; };
  requires !requires { x != quantity(1); };
  requires !requires { x == dimensionless<percent>(1.0); };
  requires !requires { x != width<metre, int>(1 * m); };
  requires !requires { x == width<kilometre, double>(1.0 * km); };
  requires !requires { x != height<metre, int>(1 * m); };
  requires !requires { x == height<kilometre, double>(1.0 * km); };
  requires !requires { x == rate_of_climb<kilometre_per_hour, double>(1.0 * km / h); };
  requires !requires { x != quantity_point(1 * m); };
  requires !requires { x == quantity_point(1.0 * mm); };
  requires !requires { x != quantity_point(quantity(1)); };
  requires !requires { x == quantity_point(dimensionless<percent>(1.0)); };
  requires !requires { x != quantity_point_kind(cgs_width<metre, int>(1 * m)); };
  requires !requires { x == ordinate<metre, int>(1 * m); };
};
static_assert(invalid_equality<abscissa_kind>);

// clang-format off
static_assert(abscissa<metre, int>(1 * m) < abscissa<metre, int>(2 * m));
static_assert(abscissa<metre, int>(1 * m) <= abscissa<metre, double>(2.0 * m));
static_assert(abscissa<metre, int>(1 * m) <= abscissa<kilometre, int>(1 * km));
static_assert(abscissa<metre, int>(1 * m) >= abscissa<millimetre, double>(1e3 * mm));
static_assert(abscissa<metre, int>(2 * m) >= abscissa<millimetre, int>(1 * mm));
static_assert(abscissa<metre, int>(2 * m) > abscissa<cgs::centimetre, int>(1 * cgs_cm));
static_assert(std::three_way_comparable_with<abscissa<metre, int>, abscissa<metre, double>>);
static_assert(std::three_way_comparable_with<abscissa<nanometre, int>, abscissa<kilometre, int>>);
static_assert(std::three_way_comparable_with<abscissa<cgs::centimetre, int>, abscissa<millimetre, double>>);
static_assert(std::three_way_comparable_with<abscissa<metre>, abscissa<cgs::centimetre>>);
// clang-format on
template<typename Abscissa>
concept invalid_relational = requires(quantity_point_kind<Abscissa, metre, int> x) {
  requires !requires { x < 1; };
  requires !requires { x <= 1.0; };
  requires !requires { x >= 1 * m; };
  requires !requires { x > 1.0 * cgs_cm; };
  requires !requires { x <=> 1 * km; };
  requires !requires { x < quantity(1); };
  requires !requires { x <= dimensionless<percent>(1.0); };
  requires !requires { x >= width<metre, int>(1 * m); };
  requires !requires { x > width<kilometre, double>(1.0 * km); };
  requires !requires { x <=> height<metre, int>(1 * m); };
  requires !requires { x < height<kilometre, double>(1.0 * km); };
  requires !requires { x <= rate_of_climb<kilometre_per_hour, double>(1.0 * km / h); };
  requires !requires { x >= quantity_point(1 * m); };
  requires !requires { x > quantity_point(1.0 * mm); };
  requires !requires { x <=> quantity_point(quantity(1)); };
  requires !requires { x < quantity_point(dimensionless<percent>(1.0)); };
  requires !requires { x <= quantity_point_kind(cgs_width<metre, int>(1 * m)); };
  requires !requires { x >= ordinate<metre, int>(1 * m); };
};
static_assert(invalid_relational<abscissa_kind>);


/////////////////////////////
// quantity_point_kind_cast
/////////////////////////////

// clang-format off
static_assert(same(quantity_point_kind_cast<abscissa<metre, int>>(abscissa<metre, int>(1 * m)), abscissa<metre, int>(1 * m)));
static_assert(same(quantity_point_kind_cast<abscissa<metre, double>>(abscissa<metre, int>(1 * m)), abscissa<metre, double>(1.0 * m)));
static_assert(same(quantity_point_kind_cast<abscissa<kilometre, int>>(abscissa<metre, int>(999 * m)), abscissa<kilometre, int>(0 * km)));
static_assert(same(quantity_point_kind_cast<abscissa<kilometre, int>>(abscissa<metre, int>(1000 * m)), abscissa<kilometre, int>(1 * km)));
static_assert(same(quantity_point_kind_cast<abscissa<kilometre, double>>(abscissa<metre, int>(999 * m)), abscissa<kilometre, double>(0.999 * km)));
static_assert(same(quantity_point_kind_cast<width<metre, int>>(abscissa<metre, int>(1 * m)), abscissa<metre, int>(1 * m)));
static_assert(same(quantity_point_kind_cast<width<metre, double>>(abscissa<metre, int>(1 * m)), abscissa<metre, double>(1.0 * m)));
static_assert(same(quantity_point_kind_cast<width<kilometre, int>>(abscissa<metre, int>(999 * m)), abscissa<kilometre, int>(0 * km)));
static_assert(same(quantity_point_kind_cast<width<kilometre, int>>(abscissa<metre, int>(1000 * m)), abscissa<kilometre, int>(1 * km)));
static_assert(same(quantity_point_kind_cast<width<kilometre, double>>(abscissa<metre, int>(999 * m)), abscissa<kilometre, double>(0.999 * km)));
static_assert(same(quantity_point_kind_cast<double>(abscissa<metre, int>(1 * m)), abscissa<metre, double>(1.0 * m)));
static_assert(same(quantity_point_kind_cast<metre>(abscissa<metre, int>(1 * m)), abscissa<metre, int>(1 * m)));
static_assert(same(quantity_point_kind_cast<kilometre>(abscissa<metre, int>(999 * m)), abscissa<kilometre, int>(0 * km)));
static_assert(same(quantity_point_kind_cast<kilometre>(abscissa<metre, int>(1000 * m)), abscissa<kilometre, int>(1 * km)));
static_assert(same(quantity_point_kind_cast<ordinate<metre, int>>(abscissa<metre, int>(1 * m)), ordinate<metre, int>(1 * m)));
static_assert(same(quantity_point_kind_cast<ordinate<metre, double>>(abscissa<metre, int>(1 * m)), ordinate<metre, double>(1.0 * m)));
static_assert(same(quantity_point_kind_cast<ordinate<kilometre, int>>(abscissa<metre, int>(999 * m)), ordinate<kilometre, int>(0 * km)));
static_assert(same(quantity_point_kind_cast<ordinate<kilometre, int>>(abscissa<metre, int>(1000 * m)), ordinate<kilometre, int>(1 * km)));
static_assert(same(quantity_point_kind_cast<ordinate<kilometre, double>>(abscissa<metre, int>(999 * m)), ordinate<kilometre, double>(0.999 * km)));
static_assert(same(quantity_point_kind_cast<height<metre, int>>(abscissa<metre, int>(1 * m)), ordinate<metre, int>(1 * m)));
static_assert(same(quantity_point_kind_cast<height<metre, double>>(abscissa<metre, int>(1 * m)), ordinate<metre, double>(1.0 * m)));
static_assert(same(quantity_point_kind_cast<height<kilometre, int>>(abscissa<metre, int>(999 * m)), ordinate<kilometre, int>(0 * km)));
static_assert(same(quantity_point_kind_cast<height<kilometre, int>>(abscissa<metre, int>(1000 * m)), ordinate<kilometre, int>(1 * km)));
static_assert(same(quantity_point_kind_cast<height<kilometre, double>>(abscissa<metre, int>(999 * m)), ordinate<kilometre, double>(0.999 * km)));
static_assert(same(quantity_point_kind_cast<height_kind>(abscissa<metre, int>(1 * m)), ordinate<metre, int>(1 * m)));
static_assert(same(quantity_point_kind_cast<ordinate_kind, metre>(abscissa<metre, int>(1 * m)), ordinate<metre, int>(1 * m)));
static_assert(same(quantity_point_kind_cast<ordinate_kind, kilometre>(abscissa<metre, int>(999 * m)), ordinate<kilometre, int>(0 * km)));
static_assert(same(quantity_point_kind_cast<ordinate_kind, kilometre>(abscissa<metre, int>(1000 * m)), ordinate<kilometre, int>(1 * km)));
static_assert(same(quantity_point_kind_cast<cgs_width<cgs::centimetre, int>>(abscissa<centimetre, int>(1 * cm)), quantity_point_kind(cgs_width<cgs::centimetre, int>(1 * cgs_cm))));
static_assert(same(quantity_point_kind_cast<cgs_width_kind>(abscissa<centimetre, int>(1 * cm)), quantity_point_kind(cgs_width<cgs::centimetre, int>(1 * cgs_cm))));
static_assert(same(quantity_point_kind_cast<altitude_kind>(abscissa<centimetre, int>(1 * cm)), altitude<cgs::centimetre, int>(1 * cgs_cm)));
static_assert(same(quantity_point_kind_cast<altitude_kind, cgs::centimetre>(abscissa<centimetre, int>(1 * cm)), altitude<cgs::centimetre, int>(1 * cgs_cm)));
static_assert(same(quantity_point_kind_cast<cgs_width_kind>(abscissa<metre, int>(1 * m)), quantity_point_kind(cgs_width<metre, int>(1 * m))));
static_assert(same(quantity_point_kind_cast<altitude_kind>(abscissa<metre, int>(1 * m)), altitude<metre, int>(1 * m)));
static_assert(same(quantity_point_kind_cast<altitude_kind, metre>(abscissa<metre, int>(1 * m)), altitude<metre, int>(1 * m)));
static_assert(same(quantity_point_kind_cast<cgs::dim_length>(abscissa<centimetre, int>(1 * cm)), abscissa<cgs::centimetre, int>(1 * cgs_cm)));
static_assert(same(quantity_point_kind_cast<length<kilometre, int>>(abscissa<metre, int>(1 * m)), abscissa<kilometre, int>(0 * km)));
static_assert(same(quantity_point_kind_cast<length<centimetre, int>>(abscissa<metre, int>(1 * m)), abscissa<centimetre, int>(100 * cm)));
static_assert(same(quantity_point_kind_cast<length<centimetre, int>>(abscissa<metre, double>(0.01 * m)), abscissa<centimetre, int>(1 * cm)));
static_assert(same(quantity_point_kind_cast<length<centimetre, int>>(abscissa<cgs::centimetre, int>(1 * cgs_cm)), abscissa<cgs::centimetre, int>(1 * cgs_cm)));
// clang-format on
template<typename Int>
concept invalid_cast = requires(Int i) {
  requires !requires { quantity_point_kind_cast<abscissa<metre, one_rep>>(abscissa<metre, Int>(i * m)); };
  requires !requires { quantity_point_kind_cast<apples<one, Int>>(abscissa<metre, Int>(i * m)); };
  requires !requires { quantity_point_kind_cast<rate_of_climb<metre_per_second, Int>>(abscissa<metre, Int>(i * m)); };
  requires !requires { quantity_point_kind_cast<apple>(abscissa<metre, Int>(i * m)); };
  requires !requires { quantity_point_kind_cast<rate_of_climb_kind>(abscissa<metre, Int>(i * m)); };
  requires !requires { quantity_point_kind_cast<apple, one>(abscissa<metre, Int>(i * m)); };
  requires !requires { quantity_point_kind_cast<width_kind, metre>(abscissa<metre, Int>(i * m)); };
  requires !requires { quantity_point_kind_cast<width_kind, kilometre>(abscissa<metre, Int>(i * m)); };
  requires !requires { quantity_point_kind_cast<height_kind, metre>(abscissa<metre, Int>(i * m)); };
  requires !requires { quantity_point_kind_cast<height_kind, kilometre>(abscissa<metre, Int>(i * m)); };
  requires !requires { quantity_point_kind_cast<cgs_width_kind, cgs::centimetre>(abscissa<metre, Int>(i * m)); };
  requires !requires { quantity_point_kind_cast<rate_of_climb_kind, metre_per_second>(abscissa<metre, Int>(i * m)); };
  requires !requires { quantity_point_kind_cast<dimensionless<one>>(abscissa<metre, Int>(i * m)); };
  requires !requires { quantity_point_kind_cast<square_metre>(abscissa<metre, Int>(i * m)); };
  requires !requires { quantity_point_kind_cast<second>(abscissa<metre, Int>(i * m)); };
  requires !requires { quantity_point_kind_cast<one_rep>(abscissa<metre, Int>(i * m)); };
  requires !requires { quantity_point_kind_cast<quantity_point<dim_length, metre, Int>>(abscissa<metre, Int>(i * m)); };
  requires !requires { quantity_point_kind_cast<quantity_point<dim_one, one, Int>>(abscissa<metre, Int>(i * m)); };
};
static_assert(invalid_cast<int>);


/////////////////////////
// extensible interface
/////////////////////////

namespace mylib {

struct width_kind : kind<width_kind, units::physical::si::dim_length> {};
struct height_kind : kind<height_kind, units::physical::si::dim_length> {};
struct abscissa_kind : point_kind<abscissa_kind, width_kind> {};
struct ordinate_kind : point_kind<ordinate_kind, height_kind> {};

struct point {};

template <units::QuantityPointKindOf<abscissa_kind> Abscissa, units::QuantityPointKindOf<ordinate_kind> Ordinate>
point operator+(Abscissa, Ordinate);

}  // namespace mylib

namespace yourapp {

static_assert(is_same_v<mylib::point,
  decltype(quantity_point_kind(quantity_kind<mylib::width_kind, metre, int>(1 * m)) +
           quantity_point_kind(quantity_kind<mylib::height_kind, metre, int>(1 * m)))>);

}  // namespace yourapp

}  // namespace
