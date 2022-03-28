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
#include <units/bits/external/type_traits.h>
#include <units/chrono.h>
#include <units/generic/dimensionless.h>
#include <units/isq/si/area.h>
#include <units/isq/si/cgs/length.h>
#include <units/isq/si/fps/length.h>
#include <units/isq/si/frequency.h>
#include <units/isq/si/speed.h>
#include <units/quantity_kind.h>
#include <units/quantity_point.h>
#include <units/unit.h>
#include <cassert>
#include <functional>
#include <limits>
#include <type_traits>

namespace {

using namespace units;
namespace si = isq::si;
using namespace si;
using namespace references;

constexpr auto cgs_cm = cgs::references::cm;

using namespace std::chrono_literals;

struct radius_kind : kind<radius_kind, dim_length> {};
struct width_kind : kind<width_kind, dim_length> {};
struct height_kind : kind<height_kind, dim_length> {};

struct horizontal_area_kind : derived_kind<horizontal_area_kind, dim_area, width_kind> {};
struct rate_of_climb_kind : derived_kind<rate_of_climb_kind, dim_speed, height_kind> {};

struct apple : kind<apple, dim_one> {};
struct orange : kind<orange, dim_one> {};

struct time_kind : kind<time_kind, dim_time> {};

struct cgs_width_kind : kind<cgs_width_kind, cgs::dim_length> {};

template<Unit U, Representation Rep = double>
using radius = quantity_kind<radius_kind, U, Rep>;
template<Unit U, Representation Rep = double>
using width = quantity_kind<width_kind, U, Rep>;
template<Unit U, Representation Rep = double>
using height = quantity_kind<height_kind, U, Rep>;

template<Unit U, Representation Rep = double>
using horizontal_area = quantity_kind<horizontal_area_kind, U, Rep>;
template<Unit U, Representation Rep = double>
using rate_of_climb = quantity_kind<rate_of_climb_kind, U, Rep>;

template<Unit U = one, Representation Rep = double>
using apples = quantity_kind<apple, U, Rep>;
template<Unit U = one, Representation Rep = double>
using oranges = quantity_kind<orange, U, Rep>;

template<Unit U, Representation Rep = double>
using cgs_width = quantity_kind<cgs_width_kind, U, Rep>;

/////////////
// concepts
/////////////

static_assert(QuantityKind<width<metre>>);
static_assert(QuantityKind<rate_of_climb<metre_per_second>>);
static_assert(!QuantityKind<double>);
static_assert(!QuantityKind<length<metre>>);
static_assert(!QuantityKind<quantity_point<dynamic_origin<dim_length>, metre>>);

static_assert(QuantityKindOf<width<metre>, width_kind>);
static_assert(!QuantityKindOf<width<metre>, height_kind>);
static_assert(!QuantityKindOf<width<metre>, metre>);
static_assert(!QuantityKindOf<length<metre>, width_kind>);
static_assert(!QuantityKindOf<length<metre>, metre>);
static_assert(!QuantityKindOf<quantity_point<dynamic_origin<dim_length>, metre>, width_kind>);
static_assert(!QuantityKindOf<quantity_point<dynamic_origin<dim_length>, metre>, dim_length>);
static_assert(!QuantityKindOf<quantity_point<dynamic_origin<dim_length>, metre>, metre>);


///////////////
// invariants
///////////////

static_assert(sizeof(width<metre, double>) == sizeof(double));
static_assert(sizeof(height<metre, short>) == sizeof(short));

template<typename Width>
concept invalid_types =
  requires {
    requires !requires { typename quantity_kind<Width, second, int>; };           // unit of a different dimension
    requires !requires { typename quantity_kind<Width, metre, length<metre>>; };  // quantity used as Rep
    requires !requires {  // quantity point used as Rep
                typename quantity_kind<Width, metre, quantity_point<dynamic_origin<dim_length>, metre>>;
              };
    requires !requires { typename quantity_kind<Width, metre, width<metre>>; };  // quantity kind used as Rep
    requires !requires { typename quantity_kind<metre, Width, double>; };        // reordered arguments
    requires !requires { typename quantity_kind<metre, double, Width>; };        // reordered arguments
  };
static_assert(invalid_types<width_kind>);

static_assert(std::is_trivially_default_constructible_v<width<metre>>);
static_assert(std::is_trivially_copy_constructible_v<width<metre>>);
static_assert(std::is_trivially_move_constructible_v<width<metre>>);
static_assert(std::is_trivially_copy_assignable_v<width<metre>>);
static_assert(std::is_trivially_move_assignable_v<width<metre>>);
static_assert(std::is_trivially_destructible_v<width<metre>>);

static_assert(std::is_nothrow_default_constructible_v<width<metre>>);
static_assert(std::is_nothrow_copy_constructible_v<width<metre>>);
static_assert(std::is_nothrow_move_constructible_v<width<metre>>);
static_assert(std::is_nothrow_copy_assignable_v<width<metre>>);
static_assert(std::is_nothrow_move_assignable_v<width<metre>>);
static_assert(std::is_nothrow_destructible_v<width<metre>>);

static_assert(std::is_trivially_copyable_v<width<metre>>);
static_assert(std::is_standard_layout_v<width<metre>>);

static_assert(std::default_initializable<width<metre>>);
static_assert(std::move_constructible<width<metre>>);
static_assert(std::copy_constructible<width<metre>>);
static_assert(std::equality_comparable<width<metre>>);
static_assert(std::totally_ordered<width<metre>>);
static_assert(std::regular<width<metre>>);

static_assert(std::three_way_comparable<width<metre>>);

static_assert(!std::is_aggregate_v<width<metre>>);


///////////////////
// member aliases
///////////////////

static_assert(is_same_v<width<metre>::kind_type, width_kind>);
static_assert(is_same_v<width<metre>::quantity_type, length<metre>>);
static_assert(is_same_v<width<metre>::dimension, dim_length>);
static_assert(is_same_v<width<metre>::unit, metre>);
static_assert(is_same_v<width<metre>::rep, double>);


////////////////////
// common observer
////////////////////

static_assert(same(radius<metre>{}.common(), length<metre>{}));
static_assert(radius<metre>{}.common() ==  // [VIM3] 1.2 kind of quantity
              height<metre>{}.common());   // aspect common to mutually comparable quantities
                                           // hence `.common()`
static_assert(!std::equality_comparable_with<apples<>, oranges<>>);


////////////////////////////
// static member functions
////////////////////////////

static_assert(width<metre, double>::zero().common() == 0 * m);
static_assert(width<metre, double>::one().common() == 1 * m);
static_assert(width<metre, unsigned>::min().common() == 0 * m);
static_assert(width<metre, unsigned>::max().common() == std::numeric_limits<unsigned>::max() * m);
static_assert(width<metre, double>::min().common().number() == std::numeric_limits<double>::lowest());
static_assert(width<metre, double>::max().common().number() == std::numeric_limits<double>::max());


////////////////////////
// default constructor
////////////////////////

// default initialization
#if !defined(UNITS_COMP_MSVC)
static_assert([] {
  const auto read_uninitialized_quantity = [] {
    width<metre> w;
    ++w;
  };
  return !require_constant_invocation<read_uninitialized_quantity>;
}());
#endif

// value initialization
static_assert(width<metre>{}.common() == 0 * m);


/////////
// CTAD
/////////

static_assert(same(quantity_kind(rate_of_climb<kilometre_per_hour, double>(0.01 * (km / h))),
                   rate_of_climb<kilometre_per_hour, double>(0.01 * (km / h))));


////////////////////////////
// construction from a rep
////////////////////////////

static_assert(construct_from_only<apples<one, int>>(1).common() == 1);
static_assert(construct_from_only<apples<one, double>>(1.0).common() == 1);
static_assert(construct_from_only<apples<percent, int>>(1ULL).common().number() == 1);
static_assert(construct_from_only<apples<percent, double>>(1.0L).common().number() == 1);
static_assert(!constructible_or_convertible_from<apples<one, int>>(1.0));
static_assert(!constructible_or_convertible_from<apples<percent, int>>(1.0));
static_assert(!constructible_or_convertible_from<width<metre, double>>(1.0));
static_assert(!constructible_or_convertible_from<width<metre, double>>(1.0f));
static_assert(!constructible_or_convertible_from<width<metre, float>>(1.0));
static_assert(!constructible_or_convertible_from<width<metre, double>>(1));


/////////////////////////////////
// construction from a quantity
/////////////////////////////////

static_assert(construct_from_only<width<metre, int>>(1 * m).common() == 1 * m);
static_assert(construct_from_only<width<metre, int>>(1 * km).common() == 1 * km);
// static_assert(construct_from_only<width<metre, int>>(1 * cgs_cm).common() == 1 * cm);  // TODO: Fix #210
static_assert(construct_from_only<width<metre, double>>(1 * cgs_cm).common() == 1 * cm);
static_assert(construct_from_only<width<metre, double>>(1 * mm).common() == 1 * mm);
static_assert(construct_from_only<width<metre, double>>(1 * m).common() == 1 * m);
static_assert(construct_from_only<width<metre, double>>(1 * km).common() == 1 * km);
static_assert(construct_from_only<width<metre, double>>(1.0 * mm).common() == 1 * mm);
static_assert(construct_from_only<width<metre, double>>(1.0 * m).common() == 1 * m);
static_assert(construct_from_only<width<metre, double>>(1.0 * km).common() == 1 * km);
static_assert(construct_from_only<width<metre, float>>(1.0 * mm).common() == 1 * mm);
static_assert(construct_from_only<width<metre, float>>(1.0 * m).common() == 1 * m);
static_assert(construct_from_only<width<metre, float>>(1.0 * km).common() == 1 * km);

static_assert(!constructible_or_convertible_from<width<metre, int>>(1 * mm));
static_assert(!constructible_or_convertible_from<width<metre, int>>(1.0 * mm));
static_assert(!constructible_or_convertible_from<width<metre, int>>(1.0 * m));
static_assert(!constructible_or_convertible_from<width<metre, int>>(1.0 * km));
static_assert(!constructible_or_convertible_from<width<metre, int>>(1 * s));
static_assert(!constructible_or_convertible_from<width<metre, int>>(1 * (m * m)));
static_assert(!constructible_or_convertible_from<width<metre, int>>(1 * (m / s)));


static_assert(construct_from_only<width<metre, double>>(1.0f * m).common() == 1 * m);
static_assert(construct_from_only<width<metre, double>>(short{1} * m).common() == 1 * m);
static_assert(construct_from_only<width<metre, short>>(1 * m).common() == 1 * m);


static_assert(construct_from_only<apples<one, int>>(quantity(1)).common() == 1);
static_assert(construct_from_only<apples<one, double>>(dimensionless<percent>(1)).common() == 0.01);
static_assert(construct_from_only<apples<percent, double>>(quantity(1.0)).common().number() == 100);
static_assert(construct_from_only<apples<percent, double>>(dimensionless<percent>(1)).common().number() == 1);
static_assert(construct_from_only<apples<one, double>>(quantity(1.0)).common() == 1);
static_assert(construct_from_only<apples<one, double>>(quantity(1.0f)).common() == 1);
static_assert(construct_from_only<apples<one, float>>(quantity(1.0)).common() == 1);
static_assert(construct_from_only<apples<one, double>>(quantity(1)).common() == 1);
static_assert(construct_from_only<apples<one, double>>(quantity(short{1})).common() == 1);
static_assert(construct_from_only<apples<one, short>>(quantity(1)).common() == 1);
static_assert(construct_from_only<apples<percent, double>>(quantity(1.0)).common().number() == 1e2);
static_assert(construct_from_only<apples<percent, double>>(quantity(1.0f)).common().number() == 1e2);
static_assert(construct_from_only<apples<percent, float>>(quantity(1.0)).common().number() == 1e2f);
static_assert(construct_from_only<apples<percent, double>>(quantity(1)).common().number() == 1e2);
static_assert(construct_from_only<apples<percent, double>>(quantity(short{1})).common().number() == 1e2);
static_assert(construct_from_only<apples<percent, short>>(quantity(1)).common().number() == 1e2);

static_assert(!constructible_or_convertible_from<dimensionless<one, double>>(apples<one, double>{}));
static_assert(!constructible_or_convertible_from<dimensionless<one, double>>(apples<one, float>{}));
static_assert(!constructible_or_convertible_from<dimensionless<one, float>>(apples<one, double>{}));
static_assert(!constructible_or_convertible_from<dimensionless<one, double>>(apples<one, int>{}));
static_assert(!constructible_or_convertible_from<dimensionless<one, double>>(apples<one, short>{}));
static_assert(!constructible_or_convertible_from<dimensionless<one, short>>(apples<one, int>{}));
static_assert(!constructible_or_convertible_from<dimensionless<percent, double>>(apples<percent, double>{}));
static_assert(!constructible_or_convertible_from<dimensionless<percent, double>>(apples<percent, float>{}));
static_assert(!constructible_or_convertible_from<dimensionless<percent, float>>(apples<percent, double>{}));
static_assert(!constructible_or_convertible_from<dimensionless<percent, double>>(apples<percent, int>{}));
static_assert(!constructible_or_convertible_from<dimensionless<percent, double>>(apples<percent, short>{}));
static_assert(!constructible_or_convertible_from<dimensionless<percent, short>>(apples<percent, int>{}));
static_assert(!constructible_or_convertible_from<dimensionless<one, double>>(apples<percent, double>{}));
static_assert(!constructible_or_convertible_from<dimensionless<one, double>>(apples<percent, float>{}));
static_assert(!constructible_or_convertible_from<dimensionless<one, float>>(apples<percent, double>{}));
static_assert(!constructible_or_convertible_from<dimensionless<one, double>>(apples<percent, int>{}));
static_assert(!constructible_or_convertible_from<dimensionless<one, double>>(apples<percent, short>{}));
static_assert(!constructible_or_convertible_from<dimensionless<one, short>>(apples<percent, int>{}));
static_assert(!constructible_or_convertible_from<dimensionless<percent, double>>(apples<one, double>{}));
static_assert(!constructible_or_convertible_from<dimensionless<percent, double>>(apples<one, float>{}));
static_assert(!constructible_or_convertible_from<dimensionless<percent, float>>(apples<one, double>{}));
static_assert(!constructible_or_convertible_from<dimensionless<percent, double>>(apples<one, int>{}));
static_assert(!constructible_or_convertible_from<dimensionless<percent, double>>(apples<one, short>{}));
static_assert(!constructible_or_convertible_from<dimensionless<percent, short>>(apples<one, int>{}));


static_assert(construct_from_only<quantity_kind<time_kind, second, int>>(42s).common() == 42 * s);


static_assert(!constructible_or_convertible_from<width<metre, int>>(1 * s));
static_assert(!constructible_or_convertible_from<width<metre, int>>(1 * (m * m)));
static_assert(!constructible_or_convertible_from<width<metre, int>>(1 * (m / s)));


static_assert(construct_from_only<width<centimetre, double>>(1.0 * cgs_cm).common() == 1 * cm);
static_assert(construct_from_only<width<cgs::centimetre, double>>(1.0 * cm).common() == 1 * cm);


////////////////////////////////////////////
// construction from another quantity kind
////////////////////////////////////////////

// clang-format off
static_assert(construct_and_convert_from<width<metre, int>>(width<metre, int>(1 * m)).common() == 1 * m);
static_assert(construct_and_convert_from<width<centimetre, int>>(width<cgs::centimetre, int>(1 * cgs_cm)).common() == 1 * cm);
static_assert(construct_and_convert_from<width<fps::foot, double>>(width<cgs::centimetre, int>(1 * cgs_cm)).common() == 1 * cm);
// clang-format on

static_assert(construct_and_convert_from<width<metre, double>>(width<metre, int>(1 * m)).common() == 1 * m);
static_assert(!constructible_or_convertible_from<width<metre, int>>(width<metre, double>(1.0 * m)));

static_assert(construct_and_convert_from<width<metre, int>>(width<kilometre, int>(1 * km)).common() == 1 * km);
static_assert(!constructible_or_convertible_from<width<kilometre, int>>(width<metre, int>(1 * m)));

static_assert(construct_and_convert_from<width<metre, double>>(width<kilometre, int>(1 * km)).common() == 1 * km);
static_assert(construct_and_convert_from<width<kilometre, double>>(width<metre, int>(1 * m)).common() == 1 * m);

static_assert(!constructible_or_convertible_from<width<metre, int>>(height<metre, int>(1 * m)));
static_assert(!constructible_or_convertible_from<apples<one, int>>(width<metre, int>(1 * m) / (1 * m)));
static_assert(!constructible_or_convertible_from<apples<one, int>>(oranges<one, int>(1)));


//////////////////////////////////
// construction from other types
//////////////////////////////////

// clang-format off
static_assert(!constructible_or_convertible_from<width<metre, int>>(quantity_point(1 * m)));
static_assert(!constructible_or_convertible_from<width<metre, int>>(quantity_point(1 * km)));
static_assert(!constructible_or_convertible_from<width<metre, double>>(quantity_point(1 * m)));
static_assert(!constructible_or_convertible_from<width<metre, double>>(quantity_point(1 * km)));
static_assert(!constructible_or_convertible_from<width<metre, double>>(quantity_point(1.0 * m)));
static_assert(!constructible_or_convertible_from<width<metre, double>>(quantity_point(1.0 * km)));
static_assert(!constructible_or_convertible_from<width<metre, double>>(quantity_point(1.0 * (m * m))));
static_assert(!constructible_or_convertible_from<width<metre, double>>(quantity_point(1.0 * s)));
static_assert(!constructible_or_convertible_from<width<metre, double>>(quantity_point(1.0 * s)));
static_assert(!constructible_or_convertible_from<width<metre, double>>(1s));
static_assert(!constructible_or_convertible_from<width<metre, double>>(1.0s));
static_assert(!constructible_or_convertible_from<apples<one, int>>(quantity_point(1)));
static_assert(!constructible_or_convertible_from<apples<one, int>>(quantity_point(dimensionless<percent, int>(1))));
static_assert(!constructible_or_convertible_from<apples<one, double>>(quantity_point(1)));
static_assert(!constructible_or_convertible_from<apples<one, double>>(quantity_point(dimensionless<percent, int>(1))));
static_assert(!constructible_or_convertible_from<apples<one, double>>(quantity_point(1.0)));
static_assert(!constructible_or_convertible_from<apples<one, double>>(quantity_point(dimensionless<percent, double>(1.0))));
static_assert(!constructible_or_convertible_from<apples<one, double>>(quantity_point(1.0 * m)));
static_assert(!constructible_or_convertible_from<apples<one, double>>(1s));
static_assert(!constructible_or_convertible_from<apples<one, double>>(1.0s));
// clang-format on


////////////////////////
// assignment operator
////////////////////////

static_assert((width<metre, int>(2 * m) = width<metre, int>(1 * m)).common() == 1 * m);
static_assert((width<metre, int>(2 * m) = width<metre, int>(1 * km)).common() == 1 * km);
static_assert(!std::is_assignable_v<width<metre, int>, width<metre, double>>);
static_assert(!std::is_assignable_v<width<metre, int>, width<millimetre, int>>);


/////////////////////
// member operators
/////////////////////

#if !defined(UNITS_COMP_MSVC) || defined(NDEBUG)
static_assert([]() {
  width<metre, int> w(1 * m);
  assert(+w.common() == 1 * m);
  assert(-w.common() == -1 * m);
  assert(&++w == &w && w.common() == 2 * m);
  assert(&--w == &w && w.common() == 1 * m);
  assert((w++).common() == 1 * m && w.common() == 2 * m);
  assert((w--).common() == 2 * m && w.common() == 1 * m);
  assert(&(w += w) == &w && w.common() == 2 * m);
  assert(&(w -= w) == &w && w.common() == 0 * m);
  w = width<metre, int>(3 * m);
  assert(&(w *= 3) == &w && w.common() == 9 * m);
  assert(&(w *= quantity(1)) == &w && w.common() == 9 * m);
  assert(&(w *= (w / w)) == &w && w.common() == 9 * m);
  assert(&(w /= 2) == &w && w.common() == 4 * m);
  assert(&(w /= quantity(1)) == &w && w.common() == 4 * m);
  assert(&(w /= (w / w)) == &w && w.common() == 4 * m);
  assert(&(w %= 3) == &w && w.common() == 1 * m);
  assert(&(w %= quantity(3)) == &w && w.common() == 1 * m);
  assert(&(w %= 3 * (w / w)) == &w && w.common() == 1 * m);
  assert(&(w %= w) == &w && w.common() == 0 * m);
  w = width<metre, int>(3 * m);
  assert(&(w *= 3.9) == &w && w.common() == 11 * m);
  assert(&(w *= quantity(1.0)) == &w && w.common() == 11 * m);
  assert(&(w *= 1.0 * (w / w)) == &w && w.common() == 11 * m);
  assert(&(w /= 3.9) == &w && w.common() == 2 * m);
  assert(&(w /= quantity(1.0)) == &w && w.common() == 2 * m);
  assert(&(w /= 1.0 * (w / w)) == &w && w.common() == 2 * m);
  return true;
}());
#endif

static_assert((std::uint8_t(255) * m %= 256) == (width<metre, std::uint8_t>(255 * m) %= 256).common());
static_assert((std::uint8_t(255) * m %= quantity(256)) ==
              (width<metre, std::uint8_t>(255 * m) %= quantity(256)).common());
// static_assert((std::uint8_t(255) * m %= 256 * m) ==
//               (width<metre, std::uint8_t>(255 * m) %=
//                 quantity_kind<downcast_kind<width_kind, dim_one>, one, std::uint8_t>(256)).common());  // UB
// static_assert((std::uint8_t(255) * m %= 256 * m) !=
//               (width<metre, std::uint8_t>(255 * m) %= width<metre, std::uint8_t>(256 * m)).common());  // UB
static_assert((std::uint8_t(255) * m %= 257) == (width<metre, std::uint8_t>(255 * m) %= 257).common());
static_assert((std::uint8_t(255) * m %= quantity(257)) ==
              (width<metre, std::uint8_t>(255 * m) %= quantity(257)).common());
static_assert((std::uint8_t(255) * m %= 257 * m) ==
              (width<metre, std::uint8_t>(255 * m) %=
               quantity_kind<downcast_kind<width_kind, dim_one>, one, std::uint8_t>(257))
                .common());
static_assert((std::uint8_t(255) * m %= 257 * m) ==
              (width<metre, std::uint8_t>(255 * m) %= width<metre, std::uint8_t>(257 * m)).common());

static_assert(same((-width<metre, short>(short{1} * m)).common(), int{-1} * m));

template<typename K, typename U, typename Qx>
concept invalid_compound_assignments_ = requires(quantity_kind<K, U, int> w, Qx q) {
                                          requires !requires { w += q; };
                                          requires !requires { w -= q; };
                                          requires !requires { w *= q; };
                                          requires !requires { w /= q; };
                                          requires !requires { w %= q; };
                                        };
template<typename Width>
concept invalid_compound_assignments =
  requires(quantity_kind<Width, metre, int> w, height<metre, int> h) {
    requires !requires { w += 1; };
    requires !requires { w -= 1; };
    requires !requires { w *= 1 * (km / m); };
    requires !requires { w /= 1 * (km / m); };
    requires !requires { w %= 1 * (km / m); };
    requires !requires { w += m; };
    requires !requires { w -= m; };
    requires !requires { w *= m; };
    requires !requires { w /= m; };
    requires !requires { w %= m; };
    requires !requires { w *= quantity_kind<downcast_kind<Width, dim_one>, scaled_unit<ratio{1, 1, 3}, one>, int>{1}; };
    requires !requires { w /= quantity_kind<downcast_kind<Width, dim_one>, scaled_unit<ratio{1, 1, 3}, one>, int>{1}; };
    requires !requires { w %= quantity_kind<downcast_kind<Width, dim_one>, scaled_unit<ratio{1, 1, 3}, one>, int>{1}; };
    requires !requires { w %= 1.0; };
    requires !requires { w %= quantity(1.0); };
    requires !requires { w %= 1.0 * (w / w); };
    requires !requires { w %= 1.0 * w; };
    requires !requires { w %= h / h; };
    requires invalid_compound_assignments_<Width, metre, length<metre, int>>;
    requires invalid_compound_assignments_<Width, metre, height<metre, int>>;
    requires invalid_compound_assignments_<Width, metre, horizontal_area<square_metre, int>>;
    requires invalid_compound_assignments_<Width, metre, quantity_point<dynamic_origin<dim_length>, metre, int>>;
    requires invalid_compound_assignments_<Width, metre, std::chrono::seconds>;
  };
static_assert(invalid_compound_assignments<width_kind>);
static_assert(invalid_compound_assignments_<time_kind, second, std::chrono::seconds>);


/////////////////////////
// non-member operators
/////////////////////////

static_assert(same(width<metre, int>(2 * m) + width<metre, int>(3 * m), width<metre, int>(5 * m)));
static_assert(same(width<metre, int>(2 * m) + width<metre, double>(3. * m), width<metre, double>(5. * m)));
static_assert(same(width<metre, double>(2. * m) + width<metre, int>(3 * m), width<metre, double>(5. * m)));
static_assert(comp(width<kilometre, int>(2 * km) + width<metre, double>(3e3 * m), width<metre, double>(5e3 * m)));
static_assert(same(width<metre, int>(2 * m) - width<metre, int>(3 * m), width<metre, int>(-1 * m)));
static_assert(same(width<metre, int>(2 * m) - width<metre, double>(3. * m), width<metre, double>(-1. * m)));
static_assert(same(width<metre, double>(2. * m) - width<metre, int>(3 * m), width<metre, double>(-1. * m)));
static_assert(comp(width<metre, double>(2e3 * m) - width<kilometre, int>(3 * km), width<metre, double>(-1e3 * m)));

static_assert(
  is_same_v<decltype((width<metre, std::uint8_t>(0 * m) + width<metre, std::uint8_t>(0 * m)).common().number()),
            int&&>);
static_assert(
  is_same_v<decltype((width<metre, std::uint8_t>(0 * m) - width<metre, std::uint8_t>(0 * m)).common().number()),
            int&&>);
static_assert((width<metre, std::uint8_t>(128 * m) + width<metre, std::uint8_t>(128 * m)).common().number() ==
              std::uint8_t(128) + std::uint8_t(128));
static_assert((width<metre, std::uint8_t>(0 * m) - width<metre, std::uint8_t>(1 * m)).common().number() ==
              std::uint8_t(0) - std::uint8_t(1));

static_assert(!std::is_invocable_v<std::plus<>, width<metre>, double>);
static_assert(!std::is_invocable_v<std::plus<>, width<metre>, length<metre>>);
static_assert(!std::is_invocable_v<std::plus<>, width<metre>, quantity_point<dynamic_origin<dim_length>, metre>>);
static_assert(!std::is_invocable_v<std::plus<>, width<metre>, height<metre>>);
static_assert(!std::is_invocable_v<std::plus<>, width<metre>, reference<dim_length, metre>>);
static_assert(!std::is_invocable_v<std::minus<>, width<metre>, double>);
static_assert(!std::is_invocable_v<std::minus<>, width<metre>, length<metre>>);
static_assert(!std::is_invocable_v<std::minus<>, width<metre>, quantity_point<dynamic_origin<dim_length>, metre>>);
static_assert(!std::is_invocable_v<std::minus<>, width<metre>, height<metre>>);
static_assert(!std::is_invocable_v<std::minus<>, width<metre>, reference<dim_length, metre>>);

// clang-format off
static_assert(!std::is_invocable_v<std::plus<>, quantity_kind<downcast_kind<width_kind, dim_one>, one>, quantity_kind<downcast_kind<height_kind, dim_one>, one>>);
static_assert(!std::is_invocable_v<std::plus<>, quantity_kind<downcast_kind<width_kind, dim_one>, one>, quantity_kind<              height_kind,         metre>>);
static_assert(!std::is_invocable_v<std::plus<>, quantity_kind<              width_kind,         metre>, quantity_kind<downcast_kind<height_kind, dim_one>, one>>);
static_assert(!std::is_invocable_v<std::plus<>, quantity_kind<downcast_kind<width_kind, dim_time>, day>, quantity_kind<              height_kind,         metre>>);
static_assert(!std::is_invocable_v<std::plus<>, quantity_kind<              width_kind,         metre>, quantity_kind<downcast_kind<height_kind, dim_time>, day>>);
static_assert(!std::is_invocable_v<std::plus<>, quantity_kind<downcast_kind<width_kind, dim_time>, day>, quantity_kind<downcast_kind<height_kind, dim_time>, day>>);
static_assert(!std::is_invocable_v<std::minus<>, quantity_kind<downcast_kind<width_kind, dim_one>, one>, quantity_kind<downcast_kind<height_kind, dim_one>, one>>);
static_assert(!std::is_invocable_v<std::minus<>, quantity_kind<downcast_kind<width_kind, dim_one>, one>, quantity_kind<              height_kind,         metre>>);
static_assert(!std::is_invocable_v<std::minus<>, quantity_kind<              width_kind,         metre>, quantity_kind<downcast_kind<height_kind, dim_one>, one>>);
static_assert(!std::is_invocable_v<std::minus<>, quantity_kind<downcast_kind<width_kind, dim_time>, day>, quantity_kind<              height_kind,         metre>>);
static_assert(!std::is_invocable_v<std::minus<>, quantity_kind<              width_kind,         metre>, quantity_kind<downcast_kind<height_kind, dim_time>, day>>);
static_assert(!std::is_invocable_v<std::minus<>, quantity_kind<downcast_kind<width_kind, dim_time>, day>, quantity_kind<downcast_kind<height_kind, dim_time>, day>>);
// clang-format on

static_assert(same(width<metre, int>(2 * m) * 3, width<metre, int>(6 * m)));
static_assert(same(width<metre, int>(2 * m) * 3., width<metre, double>(6. * m)));
static_assert(same(width<metre, double>(2. * m) * 3, width<metre, double>(6. * m)));
static_assert(same(2 * width<metre, int>(3 * m), width<metre, int>(6 * m)));
static_assert(same(2 * width<metre, double>(3. * m), width<metre, double>(6. * m)));
static_assert(same(2. * width<metre, int>(3 * m), width<metre, double>(6. * m)));

static_assert(comp(width<metre, int>(2 * m) * quantity(3), width<metre, int>(6 * m)));
static_assert(comp(width<metre, int>(2 * m) * quantity(3.), width<metre, double>(6. * m)));
static_assert(comp(width<metre, double>(2. * m) * quantity(3), width<metre, double>(6. * m)));
static_assert(comp(quantity(2) * width<metre, int>(3 * m), width<metre, int>(6 * m)));
static_assert(comp(quantity(2) * width<metre, double>(3. * m), width<metre, double>(6. * m)));
static_assert(comp(quantity(2.) * width<metre, int>(3 * m), width<metre, double>(6. * m)));

static_assert(comp(width<metre, int>(2 * m) * quantity_kind<downcast_kind<width_kind, dim_one>, one, int>(3),
                   width<metre, int>(6 * m)));
static_assert(comp(width<metre, int>(2 * m) * quantity_kind<downcast_kind<width_kind, dim_one>, one, double>(3.),
                   width<metre, double>(6. * m)));
static_assert(comp(width<metre, double>(2. * m) * quantity_kind<downcast_kind<width_kind, dim_one>, one, int>(3),
                   width<metre, double>(6. * m)));
static_assert(comp(quantity_kind<downcast_kind<width_kind, dim_one>, one, int>(2) * width<metre, int>(3 * m),
                   width<metre, int>(6 * m)));
static_assert(comp(quantity_kind<downcast_kind<width_kind, dim_one>, one, int>(2) * width<metre, double>(3. * m),
                   width<metre, double>(6. * m)));
static_assert(comp(quantity_kind<downcast_kind<width_kind, dim_one>, one, double>(2.) * width<metre, int>(3 * m),
                   width<metre, double>(6. * m)));

static_assert(comp(height<metre, int>(2 * m) * (3 * Hz), rate_of_climb<metre_per_second, int>(6 * (m / s))));
static_assert(comp(height<metre, int>(2 * m) * (3. * Hz), rate_of_climb<metre_per_second, double>(6. * (m / s))));
static_assert(comp(height<metre, double>(2. * m) * (3 * Hz), rate_of_climb<metre_per_second, double>(6. * (m / s))));
static_assert(comp((2 * Hz) * height<metre, int>(3 * m), rate_of_climb<metre_per_second, int>(6 * (m / s))));
static_assert(comp((2 * Hz) * height<metre, double>(3. * m), rate_of_climb<metre_per_second, double>(6. * (m / s))));
static_assert(comp((2. * Hz) * height<metre, int>(3 * m), rate_of_climb<metre_per_second, double>(6. * (m / s))));

static_assert(comp(quantity_kind<time_kind, second, int>(2 * s) * (3 * Hz),
                   quantity_kind<downcast_kind<time_kind, dim_one>, one, int>(6)));
static_assert(comp((3 * Hz) * quantity_kind<time_kind, second, int>(2 * s),
                   quantity_kind<downcast_kind<time_kind, dim_one>, one, int>(6)));

static_assert(comp(apples<one, int>(2) * quantity(2), apples<one, int>(4)));
static_assert(comp(quantity(2) * apples<one, int>(2), apples<one, int>(4)));

// clang-format off
static_assert(comp(width<metre, int>(4 * m) * (1 * m), horizontal_area<square_metre, int>(4 * (m * m))));
static_assert(comp(width<metre, int>(2 * m) * width<metre, int>(2 * m), horizontal_area<square_metre, int>(4 * (m * m))));
static_assert(comp(width<metre, int>(2 * m) * width<metre, double>(2 * m), horizontal_area<square_metre, double>(4 * (m * m))));
static_assert(comp(width<metre, double>(2 * m) * width<metre, int>(2 * m), horizontal_area<square_metre, double>(4 * (m * m))));
// clang-format on

static_assert(comp(apples<one, int>(2) * apples<one, int>(2), apples<one, int>(4)));
static_assert(comp(apples<one, int>(2) * (2 / apples<one, int>(1)), apples<one, int>(4)));

static_assert(comp(width<kilometre>(4 * m) * (1 * mm), horizontal_area<square_metre>(4 * (m * mm))));
static_assert(comp(width<kilometre>(2 * m) * width<millimetre>(2 * m), horizontal_area<square_metre>(4 * (m * m))));
static_assert(comp(width<metre>(2 * m) * (1 / width<metre>(2 * m)),
                   quantity_kind<downcast_kind<width_kind, dim_one>, one>(1)));

static_assert(same(width<metre, int>(2 * m) / 3, width<metre, int>(0 * m)));
static_assert(same(width<metre, int>(2 * m) / 3., width<metre, double>(2 / 3. * m)));
static_assert(same(width<metre, double>(2. * m) / 3, width<metre, double>(2. / 3 * m)));

static_assert(comp(width<metre, int>(2 * m) / quantity(3), width<metre, int>(0 * m)));
static_assert(comp(width<metre, int>(2 * m) / quantity(3.), width<metre, double>(2 / 3. * m)));
static_assert(comp(width<metre, double>(2. * m) / quantity(3), width<metre, double>(2. / 3 * m)));

static_assert(comp(width<metre, int>(2 * m) / quantity_kind<downcast_kind<width_kind, dim_one>, one, int>(3),
                   width<metre, int>(0 * m)));
static_assert(comp(width<metre, int>(2 * m) / quantity_kind<downcast_kind<width_kind, dim_one>, one, double>(3.),
                   width<metre, double>(2 / 3. * m)));
static_assert(comp(width<metre, double>(2. * m) / quantity_kind<downcast_kind<width_kind, dim_one>, one, double>(3),
                   width<metre, double>(2. / 3 * m)));

static_assert(comp(2 / quantity_kind<time_kind, second, int>(3 * s),
                   quantity_kind<downcast_kind<time_kind, dim_frequency>, hertz, int>(2 / 3 / (1 * s))));
static_assert(comp(2 / quantity_kind<time_kind, second, double>(3. * s),
                   quantity_kind<downcast_kind<time_kind, dim_frequency>, hertz, double>(2 / 3. / (1 * s))));
static_assert(comp(2. / quantity_kind<time_kind, second, int>(3 * s),
                   quantity_kind<downcast_kind<time_kind, dim_frequency>, hertz, double>(2 / 3. / (1 * s))));

static_assert(comp(quantity(2) / quantity_kind<time_kind, second, int>(3 * s),
                   quantity_kind<downcast_kind<time_kind, dim_frequency>, hertz, int>(2 / 3 / (1 * s))));
static_assert(comp(quantity(2) / quantity_kind<time_kind, second, double>(3. * s),
                   quantity_kind<downcast_kind<time_kind, dim_frequency>, hertz, double>(2 / 3. / (1 * s))));
static_assert(comp(quantity(2.) / quantity_kind<time_kind, second, int>(3 * s),
                   quantity_kind<downcast_kind<time_kind, dim_frequency>, hertz, double>(2 / 3. / (1 * s))));

static_assert(comp(quantity_kind<downcast_kind<time_kind, dim_one>, one, int>(2) /
                     quantity_kind<time_kind, second, int>(3 * s),
                   quantity_kind<downcast_kind<time_kind, dim_frequency>, hertz, int>(2 / 3 / (1 * s))));
static_assert(comp(quantity_kind<downcast_kind<time_kind, dim_one>, one, int>(2) /
                     quantity_kind<time_kind, second, double>(3. * s),
                   quantity_kind<downcast_kind<time_kind, dim_frequency>, hertz, double>(2 / 3. / (1 * s))));
static_assert(comp(quantity_kind<downcast_kind<time_kind, dim_one>, one, double>(2.) /
                     quantity_kind<time_kind, second, int>(3 * s),
                   quantity_kind<downcast_kind<time_kind, dim_frequency>, hertz, double>(2 / 3. / (1 * s))));

static_assert(comp(height<metre, int>(2 * m) / (3 * s), rate_of_climb<metre_per_second, int>(0 * (m / s))));
static_assert(comp(height<metre, int>(2 * m) / (3. * s), rate_of_climb<metre_per_second, double>(2 / 3. * (m / s))));
static_assert(comp(height<metre, double>(2. * m) / (3 * s), rate_of_climb<metre_per_second, double>(2. / 3 * (m / s))));

static_assert(comp(width<metre, int>(2 * m) * dimensionless<percent, int>(3), width<centimetre, int>(6 * cm)));
static_assert(comp(dimensionless<percent, int>(2) * width<metre, int>(3 * m), width<centimetre, int>(6 * cm)));
static_assert(comp(width<metre, int>(2 * m) / dimensionless<percent, double>(3),
                   width<hectometre, double>(2. / 3 * hm)));
static_assert(same(width<metre, int>(2 * m) % dimensionless<percent, int>(3), width<metre, int>(2 * m)));

static_assert(comp(height<metre, int>(2 * m) / (3 * m),
                   quantity_kind<downcast_kind<height_kind, dim_one>, one, int>(0)));
static_assert(comp(height<metre, int>(2 * m) / (3. * m),
                   quantity_kind<downcast_kind<height_kind, dim_one>, one, double>(2 / 3.)));
static_assert(comp(height<metre, double>(2. * m) / (3 * m),
                   quantity_kind<downcast_kind<height_kind, dim_one>, one, double>(2. / 3)));

static_assert(comp((2 * m) / height<metre, int>(3 * m),
                   quantity_kind<downcast_kind<height_kind, dim_one>, one, int>(0)));
static_assert(comp((2 * m) / height<metre, double>(3. * m),
                   quantity_kind<downcast_kind<height_kind, dim_one>, one, double>(2 / 3.)));
static_assert(comp((2. * m) / height<metre, int>(3 * m),
                   quantity_kind<downcast_kind<height_kind, dim_one>, one, double>(2. / 3)));

static_assert(comp(width<metre, int>(8 * m) / width<metre, int>(2 * m),
                   quantity_kind<downcast_kind<width_kind, dim_one>, one, int>(4)));
static_assert(comp(width<metre, int>(8 * m) / width<metre, double>(2 * m),
                   quantity_kind<downcast_kind<width_kind, dim_one>, one, double>(4.0)));
static_assert(comp(width<metre, double>(8 * m) / width<metre, int>(2 * m),
                   quantity_kind<downcast_kind<width_kind, dim_one>, one, double>(4.0)));

static_assert(comp(apples<one, int>(8) / apples<one, int>(2), apples<one, int>(4)));
static_assert(comp(apples<one, int>(8) / (2 / apples<one, int>(1)), apples<one, int>(4)));

static_assert(comp(horizontal_area<square_metre>(8 * (m * m)) / width<metre>(2 * m), width<metre>(4 * m)));
static_assert(comp(horizontal_area<square_metre>(4 * (m * m)) / (1 * m), width<metre>(4 * m)));

static_assert(same(width<metre, int>(2 * m) % 3, width<metre, int>(2 * m)));
static_assert(same(width<metre, int>(3 * m) % width<metre, int>(2 * m), width<metre, int>(1 * m)));

static_assert(
  is_same_v<decltype((width<metre, std::uint8_t>(0 * m) % width<metre, std::uint8_t>(0 * m)).common().number()),
            decltype(std::uint8_t(0) % std::uint8_t(0))&&>);

static_assert(!std::is_invocable_v<std::multiplies<>, reference<dim_length, metre>, width<metre>>);
static_assert(!std::is_invocable_v<std::multiplies<>, width<metre>, height<metre>>);
static_assert(
  !std::is_invocable_v<std::multiplies<>, height<metre>, quantity_point<dynamic_origin<dim_length>, metre>>);
static_assert(
  !std::is_invocable_v<std::multiplies<>, quantity_point<dynamic_origin<dim_length>, metre>, height<metre>>);

static_assert(!std::is_invocable_v<std::divides<>, reference<dim_length, metre>, width<metre>>);
static_assert(!std::is_invocable_v<std::divides<>, width<metre>, height<metre>>);
static_assert(!std::is_invocable_v<std::divides<>, height<metre>, quantity_point<dynamic_origin<dim_length>, metre>>);
static_assert(!std::is_invocable_v<std::divides<>, quantity_point<dynamic_origin<dim_length>, metre>, height<metre>>);

static_assert(!std::is_invocable_v<std::modulus<>, width<metre, int>, reference<dim_length, metre>>);
static_assert(!std::is_invocable_v<std::modulus<>, width<metre, int>, length<metre, int>>);
static_assert(
  !std::is_invocable_v<std::modulus<>, width<metre, int>, quantity_point<dynamic_origin<dim_length>, metre, int>>);
static_assert(!std::is_invocable_v<std::modulus<>, width<metre, int>, double>);
static_assert(!std::is_invocable_v<std::modulus<>, width<metre, int>, width<metre, double>>);

// clang-format off
static_assert(!std::is_invocable_v<std::multiplies<>, quantity_kind<downcast_kind<width_kind, dim_one>, one>, quantity_kind<downcast_kind<height_kind, dim_one>, one>>);
static_assert(!std::is_invocable_v<std::multiplies<>, quantity_kind<downcast_kind<width_kind, dim_one>, one>, quantity_kind<              height_kind,         metre>>);
static_assert(!std::is_invocable_v<std::multiplies<>, quantity_kind<              width_kind,         metre>, quantity_kind<downcast_kind<height_kind, dim_one>, one>>);
static_assert(!std::is_invocable_v<std::multiplies<>, quantity_kind<downcast_kind<width_kind, dim_time>, day>, quantity_kind<              height_kind,         metre>>);
static_assert(!std::is_invocable_v<std::multiplies<>, quantity_kind<              width_kind,         metre>, quantity_kind<downcast_kind<height_kind, dim_time>, day>>);
static_assert(!std::is_invocable_v<std::multiplies<>, quantity_kind<downcast_kind<width_kind, dim_time>, day>, quantity_kind<downcast_kind<height_kind, dim_time>, day>>);
static_assert(!std::is_invocable_v<std::divides<>, quantity_kind<downcast_kind<width_kind, dim_one>, one>, quantity_kind<downcast_kind<height_kind, dim_one>, one>>);
static_assert(!std::is_invocable_v<std::divides<>, quantity_kind<downcast_kind<width_kind, dim_one>, one>, quantity_kind<              height_kind,         metre>>);
static_assert(!std::is_invocable_v<std::divides<>, quantity_kind<              width_kind,         metre>, quantity_kind<downcast_kind<height_kind, dim_one>, one>>);
static_assert(!std::is_invocable_v<std::divides<>, quantity_kind<downcast_kind<width_kind, dim_time>, day>, quantity_kind<              height_kind,         metre>>);
static_assert(!std::is_invocable_v<std::divides<>, quantity_kind<              width_kind,         metre>, quantity_kind<downcast_kind<height_kind, dim_time>, day>>);
static_assert(!std::is_invocable_v<std::divides<>, quantity_kind<downcast_kind<width_kind, dim_time>, day>, quantity_kind<downcast_kind<height_kind, dim_time>, day>>);
static_assert(!std::is_invocable_v<std::modulus<>, quantity_kind<downcast_kind<width_kind, dim_one>, one>, quantity_kind<downcast_kind<height_kind, dim_one>, one>>);
static_assert(!std::is_invocable_v<std::modulus<>, quantity_kind<downcast_kind<width_kind, dim_one>, one>, quantity_kind<              height_kind,         metre>>);
static_assert(!std::is_invocable_v<std::modulus<>, quantity_kind<              width_kind,         metre>, quantity_kind<downcast_kind<height_kind, dim_one>, one>>);
static_assert(!std::is_invocable_v<std::modulus<>, quantity_kind<downcast_kind<width_kind, dim_time>, day>, quantity_kind<              height_kind,         metre>>);
static_assert(!std::is_invocable_v<std::modulus<>, quantity_kind<              width_kind,         metre>, quantity_kind<downcast_kind<height_kind, dim_time>, day>>);
static_assert(!std::is_invocable_v<std::modulus<>, quantity_kind<downcast_kind<width_kind, dim_time>, day>, quantity_kind<downcast_kind<height_kind, dim_time>, day>>);
// clang-format on


/////////////////////////
// comparison operators
/////////////////////////

static_assert(width<metre, int>(1 * m) == width<metre, int>(1 * m));
static_assert(width<metre, int>(1 * m) == width<metre, double>(1.0 * m));
static_assert(width<metre, int>(1 * m) == width<millimetre, int>(1000 * mm));
static_assert(width<metre, int>(1 * m) == width<millimetre, double>(1e3 * mm));
static_assert(width<metre, int>(2 * m) != width<metre, int>(1 * m));
static_assert(width<metre, int>(2 * m) != width<cgs::centimetre, double>(1.0 * cgs_cm));
static_assert(std::equality_comparable_with<width<metre, int>, width<metre, double>>);
static_assert(std::equality_comparable_with<width<nanometre, int>, width<kilometre, int>>);
static_assert(std::equality_comparable_with<width<cgs::centimetre, int>, width<millimetre, double>>);
static_assert(std::equality_comparable_with<width<metre>, width<cgs::centimetre>>);
template<typename Width>
concept invalid_equality = requires(quantity_kind<Width, metre, int> w) {
                             requires !requires { w == 1; };
                             requires !requires { w != 1.0; };
                             requires !requires { w == 1 * m; };
                             requires !requires { w != 1.0 * cgs_cm; };
                             requires !requires { w == 1 * km; };
                             requires !requires { w != m; };
                             requires !requires { w == km; };
                             requires !requires { w != quantity(1); };
                             requires !requires { w == dimensionless<percent>(1.0); };
                             requires !requires { w != height<metre, int>(1 * m); };
                             requires !requires { w == height<kilometre, double>(1.0 * km); };
                             requires !requires { w != horizontal_area<square_metre, int>(1 * (m * m)); };
                             requires !requires { w == rate_of_climb<kilometre_per_hour, double>(1.0 * (km / h)); };
                             requires !requires { w != quantity_point(1 * m); };
                             requires !requires { w == quantity_point(1.0 * mm); };
                             requires !requires { w != quantity_point(quantity(1)); };
                             requires !requires { w == quantity_point(dimensionless<percent>(1.0)); };
                           };
static_assert(invalid_equality<width_kind>);

static_assert(width<metre, int>(1 * m) < width<metre, int>(2 * m));
static_assert(width<metre, int>(1 * m) <= width<metre, double>(2.0 * m));
static_assert(width<metre, int>(1 * m) <= width<kilometre, int>(1 * km));
static_assert(width<metre, int>(1 * m) >= width<millimetre, double>(1e3 * mm));
static_assert(width<metre, int>(2 * m) >= width<millimetre, int>(1 * mm));
static_assert(width<metre, int>(2 * m) > width<cgs::centimetre, int>(1 * cgs_cm));
static_assert(std::three_way_comparable_with<width<metre, int>, width<metre, double>>);
static_assert(std::three_way_comparable_with<width<nanometre, int>, width<kilometre, int>>);
static_assert(std::three_way_comparable_with<width<cgs::centimetre, int>, width<millimetre, double>>);
static_assert(std::three_way_comparable_with<width<metre>, width<cgs::centimetre>>);
template<typename Width>
concept invalid_relational = requires(quantity_kind<Width, metre, int> w) {
                               requires !requires { w < 1; };
                               requires !requires { w <= 1.0; };
                               requires !requires { w >= 1 * m; };
                               requires !requires { w > 1.0 * cgs_cm; };
                               requires !requires { w <=> 1 * km; };
                               requires !requires { w < quantity(1); };
                               requires !requires { w <= dimensionless<percent>(1.0); };
                               requires !requires { w >= height<metre, int>(1 * m); };
                               requires !requires { w > height<kilometre, double>(1.0 * km); };
                               requires !requires { w <=> horizontal_area<square_metre, int>(1 * (m * m)); };
                               requires !requires { w < rate_of_climb<kilometre_per_hour, double>(1.0 * (km / h)); };
                               requires !requires { w <= quantity_point(1 * m); };
                               requires !requires { w >= quantity_point(1.0 * mm); };
                               requires !requires { w > quantity_point(quantity(1)); };
                               requires !requires { w <=> quantity_point(dimensionless<percent>(1.0)); };
                             };
static_assert(invalid_relational<width_kind>);


///////////////////////
// quantity_kind_cast
///////////////////////

// clang-format off
static_assert(same(quantity_kind_cast<width<metre, int>>(width<metre, int>(1 * m)), width<metre, int>(1 * m)));
static_assert(same(quantity_kind_cast<width<metre, double>>(width<metre, int>(1 * m)), width<metre, double>(1.0 * m)));
static_assert(same(quantity_kind_cast<width<kilometre, int>>(width<metre, int>(999 * m)), width<kilometre, int>(0 * km)));
static_assert(same(quantity_kind_cast<width<kilometre, int>>(width<metre, int>(1000 * m)), width<kilometre, int>(1 * km)));
static_assert(same(quantity_kind_cast<width<kilometre, double>>(width<metre, int>(999 * m)), width<kilometre, double>(0.999 * km)));
static_assert(same(quantity_kind_cast<double>(width<metre, int>(1 * m)), width<metre, double>(1.0 * m)));
static_assert(same(quantity_kind_cast<metre>(width<metre, int>(1 * m)), width<metre, int>(1 * m)));
static_assert(same(quantity_kind_cast<kilometre>(width<metre, int>(999 * m)), width<kilometre, int>(0 * km)));
static_assert(same(quantity_kind_cast<kilometre>(width<metre, int>(1000 * m)), width<kilometre, int>(1 * km)));
static_assert(same(quantity_kind_cast<height<metre, int>>(width<metre, int>(1 * m)), height<metre, int>(1 * m)));
static_assert(same(quantity_kind_cast<height<metre, double>>(width<metre, int>(1 * m)), height<metre, double>(1.0 * m)));
static_assert(same(quantity_kind_cast<height<kilometre, int>>(width<metre, int>(999 * m)), height<kilometre, int>(0 * km)));
static_assert(same(quantity_kind_cast<height<kilometre, int>>(width<metre, int>(1000 * m)), height<kilometre, int>(1 * km)));
static_assert(same(quantity_kind_cast<height<kilometre, double>>(width<metre, int>(999 * m)), height<kilometre, double>(0.999 * km)));
static_assert(same(quantity_kind_cast<height_kind>(width<metre, int>(1 * m)), height<metre, int>(1 * m)));
static_assert(same(quantity_kind_cast<height_kind, metre>(width<metre, int>(1 * m)), height<metre, int>(1 * m)));
static_assert(same(quantity_kind_cast<height_kind, kilometre>(width<metre, int>(999 * m)), height<kilometre, int>(0 * km)));
static_assert(same(quantity_kind_cast<height_kind, kilometre>(width<metre, int>(1000 * m)), height<kilometre, int>(1 * km)));
static_assert(same(quantity_kind_cast<cgs_width<cgs::centimetre, int>>(width<centimetre, int>(1 * cm)), cgs_width<cgs::centimetre, int>(1 * cgs_cm)));
static_assert(same(quantity_kind_cast<cgs_width_kind>(width<centimetre, int>(1 * cm)), cgs_width<cgs::centimetre, int>(1 * cgs_cm)));
static_assert(same(quantity_kind_cast<cgs_width_kind, cgs::centimetre>(width<centimetre, int>(1 * cm)), cgs_width<cgs::centimetre, int>(1 * cgs_cm)));
static_assert(same(quantity_kind_cast<cgs_width_kind>(width<metre, int>(1 * m)), cgs_width<metre, int>(1 * m)));
static_assert(same(quantity_kind_cast<cgs_width_kind, metre>(width<metre, int>(1 * m)), cgs_width<metre, int>(1 * m)));
static_assert(comp(quantity_kind_cast<cgs::dim_length>(width<centimetre, int>(1 * cm)), width<cgs::centimetre, int>(1 * cgs_cm)));
static_assert(same(quantity_kind_cast<length<kilometre, int>>(width<metre, int>(1 * m)), width<kilometre, int>(0 * km)));
static_assert(same(quantity_kind_cast<length<centimetre, int>>(width<metre, int>(1 * m)), width<centimetre, int>(100 * cm)));
static_assert(same(quantity_kind_cast<length<centimetre, int>>(width<metre, double>(0.01 * m)), width<centimetre, int>(1 * cm)));
static_assert(same(quantity_kind_cast<length<centimetre, int>>(width<cgs::centimetre, int>(1 * cgs_cm)), width<cgs::centimetre, int>(1 * cgs_cm)));
// clang-format on
template<typename Width>
concept invalid_cast =
  requires {
    requires !requires { quantity_kind_cast<apples<one, int>>(quantity_kind<Width, metre, int>(1 * m)); };
    requires !requires {
                quantity_kind_cast<horizontal_area<square_metre, int>>(quantity_kind<Width, metre, int>(1 * m));
              };
    requires !requires {
                quantity_kind_cast<rate_of_climb<metre_per_second, int>>(quantity_kind<Width, metre, int>(1 * m));
              };
    requires !requires { quantity_kind_cast<apple>(quantity_kind<Width, metre, int>(1 * m)); };
    requires !requires { quantity_kind_cast<horizontal_area_kind>(quantity_kind<Width, metre, int>(1 * m)); };
    requires !requires { quantity_kind_cast<rate_of_climb_kind>(quantity_kind<Width, metre, int>(1 * m)); };
    requires !requires { quantity_kind_cast<apple, one>(quantity_kind<Width, metre, int>(1 * m)); };
    requires !requires {
                quantity_kind_cast<horizontal_area_kind, square_metre>(quantity_kind<Width, metre, int>(1 * m));
              };
    requires !requires {
                quantity_kind_cast<rate_of_climb_kind, metre_per_second>(quantity_kind<Width, metre, int>(1 * m));
              };
    requires !requires { quantity_kind_cast<dimensionless<one>>(quantity_kind<Width, metre, int>(1 * m)); };
    requires !requires { quantity_kind_cast<square_metre>(quantity_kind<Width, metre, int>(1 * m)); };
    requires !requires { quantity_kind_cast<second>(quantity_kind<Width, metre, int>(1 * m)); };
    requires !requires {
                quantity_kind_cast<quantity_point<dynamic_origin<dim_length>, metre, int>>(
                  quantity_kind<Width, metre, int>(1 * m));
              };
    requires !requires {
                quantity_kind_cast<quantity_point<dynamic_origin<dim_one>, one, int>>(
                  quantity_kind<Width, metre, int>(1 * m));
              };
  };
static_assert(invalid_cast<width_kind>);


/////////////////////////
// extensible interface
/////////////////////////

namespace mylib {

struct radius_kind : kind<radius_kind, si::dim_length> {};

struct cylinder_size {};

template<units::QuantityKindOf<radius_kind> Radius, units::QuantityKindOf<height_kind> Height>
cylinder_size operator+(Radius, Height);

}  // namespace mylib

namespace yourapp {

static_assert(is_same_v<mylib::cylinder_size, decltype(quantity_kind<mylib::radius_kind, metre, double>(1. * m) +
                                                       height<metre, double>(1. * m))>);

}  // namespace yourapp

}  // namespace
