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
#include "units/generic/angle.h"
#include "units/kind.h"
#include "units/physical/si/base/length.h"
#include "units/physical/si/derived/area.h"
#include "units/physical/si/derived/speed.h"

using namespace units;
using namespace physical::si;

namespace {

template<Downcastable T>
using downcast_result = std::conditional_t<UNITS_DOWNCAST_MODE, T, units::downcast_base_t<T>>;


// no library-defined base kind


// spherical coordinates
struct radius : kind<radius, dim_length> {};  // program-defined base kind
struct colatitude : kind<colatitude, units::dim_angle<>> {};
struct azimuth : kind<azimuth, units::dim_angle<>> {};

static_assert(Kind<radius>);
static_assert(Kind<colatitude>);
static_assert(Kind<azimuth>);
static_assert(Kind<azimuth::kind>);
static_assert(Kind<azimuth::_kind_base>);

static_assert(!PointKind<radius>);
static_assert(!PointKind<colatitude>);
static_assert(!PointKind<azimuth>);
static_assert(!PointKind<azimuth::kind>);
static_assert(!PointKind<azimuth::_kind_base>);

static_assert(is_same_v<radius::base_kind, radius>);
static_assert(is_same_v<radius::dimension, dim_length>);
static_assert(is_same_v<downcast_result<radius>, downcast_kind<radius, dim_length>>);

static_assert(equivalent<radius, radius>);
static_assert(equivalent<radius, radius::kind>);
static_assert(equivalent<radius, radius::_kind_base>);
static_assert(equivalent<radius::kind, radius::_kind_base>);
static_assert(equivalent<radius::_kind_base, radius::_kind_base>);
static_assert(!equivalent<radius, colatitude>);
static_assert(!equivalent<radius, azimuth>);
static_assert(!equivalent<azimuth, colatitude>);
static_assert(!equivalent<azimuth, colatitude::_kind_base>);
static_assert(!equivalent<azimuth::_kind_base, colatitude::_kind_base>);
static_assert(!equivalent<colatitude, downcast_kind<radius, dim_length>>);
static_assert(!equivalent<azimuth, downcast_kind<radius, dim_length>>);

using radial_area = downcast_kind<radius, dim_area>;  // library-defined derived kind
using radial_point = downcast_point_kind<radius>;     // library-defined base point kind

static_assert(Kind<radial_area>);
static_assert(!PointKind<radial_area>);

static_assert(is_same_v<radial_area::base_kind, radius>);
static_assert(is_same_v<radial_area::dimension, dim_area>);
static_assert(is_same_v<radial_area, detail::_kind_base<radius, dim_area>>);
static_assert(is_same_v<radial_area, downcast_kind<radius, dim_area>>);
static_assert(is_same_v<radial_area, downcast_kind<radial_area, dim_area>>);

static_assert(equivalent<radial_area, radial_area>);
static_assert(!equivalent<radial_area, radius>);
static_assert(!equivalent<radial_area, radius::_kind_base>);

static_assert(!Kind<radial_point>);
static_assert(PointKind<radial_point>);

static_assert(is_same_v<radial_point::base_kind, radius>);
static_assert(is_same_v<radial_point::dimension, dim_length>);
static_assert(is_same_v<radial_point, detail::_point_kind_base<radius>>);
static_assert(is_same_v<radial_point, downcast_point_kind<radius>>);

static_assert(equivalent<radial_point, radial_point>);
static_assert(!equivalent<radial_point, radius>);
static_assert(!equivalent<radial_point, radius::_kind_base>);

static_assert(equivalent<radius, downcast_kind<radial_area, dim_length>>);
static_assert(!equivalent<radius, radial_area>);
static_assert(!equivalent<radius, radial_point>);


struct width : kind<width, units::physical::si::dim_length> {};
using horizontal_speed = downcast_kind<width, dim_speed>;

struct abscissa : point_kind<abscissa, width> {};                                  // program-defined base point kind
using horizontal_velocity = downcast_point_kind<downcast_kind<width, dim_speed>>;  // library-defined derived point kind

static_assert(!Kind<abscissa>);
static_assert(!Kind<abscissa::point_kind>);
static_assert(!Kind<abscissa::_point_kind_base>);
static_assert(PointKind<abscissa>);
static_assert(PointKind<abscissa::point_kind>);
static_assert(PointKind<abscissa::_point_kind_base>);

static_assert(is_same_v<abscissa::base_kind, width>);
static_assert(is_same_v<abscissa::dimension, dim_length>);
static_assert(is_same_v<downcast_result<abscissa>, downcast_point_kind<width>>);

static_assert(equivalent<abscissa, abscissa>);
static_assert(equivalent<abscissa, abscissa::point_kind>);
static_assert(equivalent<abscissa, abscissa::_point_kind_base>);
static_assert(!equivalent<abscissa, width>);
static_assert(!equivalent<abscissa, width::_kind_base>);

static_assert(!Kind<horizontal_velocity>);
static_assert(PointKind<horizontal_velocity>);

static_assert(is_same_v<horizontal_velocity::base_kind, horizontal_speed>);
static_assert(is_same_v<horizontal_velocity::dimension, dim_speed>);
static_assert(is_same_v<horizontal_velocity, detail::_point_kind_base<horizontal_speed>>);
static_assert(is_same_v<horizontal_velocity, downcast_point_kind<horizontal_speed>>);

static_assert(equivalent<horizontal_velocity, horizontal_velocity>);
static_assert(!equivalent<horizontal_velocity, horizontal_speed>);
static_assert(!equivalent<horizontal_velocity, width>);
static_assert(!equivalent<horizontal_velocity, width::_kind_base>);

static_assert(!equivalent<abscissa, horizontal_velocity>);
static_assert(!equivalent<abscissa::_point_kind_base, horizontal_velocity>);


struct height : kind<height, dim_length> {};

struct rate_of_climb : derived_kind<rate_of_climb, height, dim_speed> {};    // program-defined derived kind
struct velocity_of_climb : point_kind<velocity_of_climb, rate_of_climb> {};  // program-defined derived point kind

static_assert(Kind<rate_of_climb>);
static_assert(Kind<rate_of_climb::derived_kind>);
static_assert(Kind<rate_of_climb::_kind_base>);
static_assert(!PointKind<rate_of_climb>);
static_assert(!PointKind<rate_of_climb::derived_kind>);
static_assert(!PointKind<rate_of_climb::_kind_base>);

static_assert(is_same_v<rate_of_climb::base_kind, height>);
static_assert(is_same_v<rate_of_climb::dimension, dim_speed>);
static_assert(is_same_v<downcast_result<rate_of_climb>, downcast_kind<rate_of_climb, dim_speed>>);
static_assert(is_same_v<downcast_result<rate_of_climb>, downcast_kind<height, dim_speed>>);

static_assert(equivalent<rate_of_climb, rate_of_climb>);
static_assert(equivalent<rate_of_climb, rate_of_climb::derived_kind>);
static_assert(equivalent<rate_of_climb, rate_of_climb::_kind_base>);
static_assert(equivalent<rate_of_climb::derived_kind, rate_of_climb::_kind_base>);
static_assert(!equivalent<rate_of_climb, height>);
static_assert(!equivalent<rate_of_climb, height::_kind_base>);

static_assert(!Kind<velocity_of_climb>);
static_assert(!Kind<velocity_of_climb::point_kind>);
static_assert(!Kind<velocity_of_climb::_point_kind_base>);
static_assert(PointKind<velocity_of_climb>);
static_assert(PointKind<velocity_of_climb::point_kind>);
static_assert(PointKind<velocity_of_climb::_point_kind_base>);

static_assert(is_same_v<velocity_of_climb::base_kind, rate_of_climb>);
static_assert(is_same_v<velocity_of_climb::dimension, dim_speed>);
static_assert(is_same_v<downcast_result<velocity_of_climb>, downcast_point_kind<rate_of_climb>>);

static_assert(equivalent<velocity_of_climb, velocity_of_climb>);
static_assert(equivalent<velocity_of_climb, velocity_of_climb::point_kind>);
static_assert(equivalent<velocity_of_climb, velocity_of_climb::_point_kind_base>);
static_assert(equivalent<velocity_of_climb::point_kind, velocity_of_climb::_point_kind_base>);
static_assert(equivalent<velocity_of_climb::_point_kind_base, velocity_of_climb::_point_kind_base>);

static_assert(!equivalent<height, rate_of_climb>);
static_assert(!equivalent<height, rate_of_climb::_kind_base>);
static_assert(!equivalent<height::_kind_base, rate_of_climb::_kind_base>);

static_assert(!equivalent<height, velocity_of_climb>);
static_assert(!equivalent<height, velocity_of_climb::_point_kind_base>);
static_assert(!equivalent<height::_kind_base, velocity_of_climb::_point_kind_base>);

static_assert(!equivalent<rate_of_climb, velocity_of_climb>);
static_assert(!equivalent<rate_of_climb::_kind_base, velocity_of_climb::_point_kind_base>);

static_assert(is_same_v<downcast_result<height>, downcast_kind<rate_of_climb, dim_length>>);

}  // namespace
