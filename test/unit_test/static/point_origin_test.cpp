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

#include <units/bits/equivalent.h>
#include <units/chrono.h>
#include <units/kind.h>
#include <units/quantity_point.h>
#include <units/isq/si/length.h>
#include <units/isq/si/cgs/length.h>

using namespace units;
namespace si = isq::si;

namespace {

struct width : kind<width, si::dim_length> {};
struct abscissa : point_kind<abscissa, width> {};

struct ones_viewpoint1 : point_origin<si::dim_length> {
  template<typename D>
  using rebind = ones_viewpoint1;
};

struct ones_viewpoint2 : point_origin<si::cgs::dim_length> {
  template<typename D>
  using rebind = ones_viewpoint1;
};

static_assert(PointOrigin<unspecified_origin<si::dim_length>>);
static_assert(!PointOrigin<point_origin<si::dim_length>>);
static_assert(!PointOrigin<width>);
static_assert(!PointOrigin<abscissa>);

static_assert(RebindablePointOriginFor<unspecified_origin<si::dim_length>, si::dim_length>);
static_assert(is_same_v<rebind_point_origin_dimension<unspecified_origin<si::dim_length>, si::dim_length>,
                        unspecified_origin<si::dim_length>>);
static_assert(!RebindablePointOriginFor<unspecified_origin<si::dim_length>, si::dim_time>);
static_assert(RebindablePointOriginFor<ones_viewpoint1, si::dim_length>);
static_assert(is_same_v<rebind_point_origin_dimension<ones_viewpoint1, si::dim_length>, ones_viewpoint1>);
static_assert(RebindablePointOriginFor<ones_viewpoint2, si::dim_length>);
static_assert(is_same_v<rebind_point_origin_dimension<ones_viewpoint2, si::dim_length>, ones_viewpoint1>);
static_assert(RebindablePointOriginFor<ones_viewpoint2, si::cgs::dim_length>);
static_assert(is_same_v<rebind_point_origin_dimension<ones_viewpoint2, si::cgs::dim_length>, ones_viewpoint2>);
static_assert(!RebindablePointOriginFor<ones_viewpoint1, si::cgs::dim_length>);

static_assert(equivalent<unspecified_origin<si::dim_time>, unspecified_origin<si::dim_time>>);
static_assert(equivalent<unspecified_origin<si::dim_length>, unspecified_origin<si::cgs::dim_length>>);
static_assert(!equivalent<unspecified_origin<si::dim_time>, clock_origin<std::chrono::system_clock>>);
static_assert(!equivalent<clock_origin<std::chrono::steady_clock>, clock_origin<std::chrono::system_clock>>);
static_assert(!equivalent<unspecified_origin<si::dim_time>, unspecified_origin<si::dim_length>>);
static_assert(!equivalent<ones_viewpoint1, ones_viewpoint2>);

}  // namespace
