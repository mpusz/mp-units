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
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#include <type_traits>
#endif

// All origins and test assertions live in the anonymous namespace.
// frame_projection specializations must be in namespace mp_units (or at global scope
// with qualified name), so they are placed after the anonymous-namespace forward
// declarations of the origins they reference.

namespace {

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

////////////////////////////////////////////////////////////////////////////////////
// Origin types for axis-inversion test (altitude ↔ depth)
//   sea_level   : absolute origin for isq::height (positive = up)
//   ocean_surface: independent absolute origin for isq::height (positive = down)
////////////////////////////////////////////////////////////////////////////////////

inline constexpr struct sea_level_t final : absolute_point_origin<isq::altitude> {
} sea_level;

inline constexpr struct ocean_surface_t final : absolute_point_origin<isq::altitude> {
} ocean_surface;

// A relative origin 10 m below ocean_surface (represents a fixed shallow-water depth).
inline constexpr struct shallow_water_t final : relative_point_origin<ocean_surface + 10. * isq::height[m]> {
} shallow_water;

////////////////////////////////////////////////////////////////////////////////////
// Origin types for extra-argument forwarding test
//   base_origin : absolute origin for isq::height
//   shifted_origin: independent absolute origin for isq::height
// The projection adds a caller-supplied offset.
////////////////////////////////////////////////////////////////////////////////////

inline constexpr struct base_origin_t final : absolute_point_origin<isq::altitude> {
} base_origin;

inline constexpr struct shifted_origin_t final : absolute_point_origin<isq::altitude> {
} shifted_origin;

}  // namespace

// Explicit specializations of mp_units::frame_projection must live in namespace mp_units
// or at global scope. The origin variables above are defined in the file-scope anonymous
// namespace and are therefore accessible here by unqualified lookup.

template<>
inline constexpr auto mp_units::frame_projection<sea_level, ocean_surface> =
  // Axis inversion: altitude (positive up, measured from sea_level)
  //             → depth (positive down, measured from ocean_surface)
  [](mp_units::QuantityPointOf<mp_units::isq::height> auto qp) constexpr {
    return ocean_surface - qp.quantity_from(sea_level);
  };

template<>
inline constexpr auto mp_units::frame_projection<ocean_surface, sea_level> =
  // Axis inversion: depth (positive down) → altitude (positive up)
  [](mp_units::QuantityPointOf<mp_units::isq::height> auto qp) constexpr {
    return sea_level - qp.quantity_from(ocean_surface);
  };

template<>
inline constexpr auto mp_units::frame_projection<shifted_origin, base_origin> =
  // Translation by a caller-supplied offset (extra-arg forwarding).
  [](mp_units::QuantityPointOf<mp_units::isq::height> auto qp,
     mp_units::quantity<mp_units::isq::height[mp_units::si::metre], double> offset) constexpr {
    return base_origin + (qp.quantity_from(shifted_origin) + offset);
  };

namespace {

////////////////////////////////////////////////////////////////////////////////////
// HasFrameProjection concept checks
////////////////////////////////////////////////////////////////////////////////////

static_assert(detail::HasFrameProjection<sea_level, ocean_surface>);
static_assert(detail::HasFrameProjection<ocean_surface, sea_level>);
static_assert(detail::HasFrameProjection<shifted_origin, base_origin>);

// No projection defined for these pairs — must evaluate to false.
static_assert(!detail::HasFrameProjection<sea_level, sea_level>);
static_assert(!detail::HasFrameProjection<sea_level, base_origin>);
static_assert(!detail::HasFrameProjection<ocean_surface, base_origin>);

////////////////////////////////////////////////////////////////////////////////////
// Axis-inversion: sea_level → ocean_surface
////////////////////////////////////////////////////////////////////////////////////

// 100 m above sea level → depth 0 (should be at ocean_surface origin, no offset)
static_assert((sea_level + 0. * m).point_for(ocean_surface).quantity_from(ocean_surface) == 0. * m);

// 100 m below sea level (altitude = -100 m) → depth 100 m (positive downward)
static_assert((sea_level + (-100. * m)).point_for(ocean_surface).quantity_from(ocean_surface) == 100. * m);

// 50 m above sea level → depth -50 m (above ocean surface / sea level)
static_assert((sea_level + 50. * m).point_for(ocean_surface).quantity_from(ocean_surface) == -50. * m);

////////////////////////////////////////////////////////////////////////////////////
// Axis-inversion: ocean_surface → sea_level
////////////////////////////////////////////////////////////////////////////////////

static_assert((ocean_surface + 0. * m).point_for(sea_level).quantity_from(sea_level) == 0. * m);
static_assert((ocean_surface + 100. * m).point_for(sea_level).quantity_from(sea_level) == -100. * m);
static_assert((ocean_surface + (-50.) * m).point_for(sea_level).quantity_from(sea_level) == 50. * m);

////////////////////////////////////////////////////////////////////////////////////
// Round-trip: sea_level → ocean_surface → sea_level
////////////////////////////////////////////////////////////////////////////////////

static_assert((sea_level + (-100. * m)).point_for(ocean_surface).point_for(sea_level).quantity_from(sea_level) ==
              -100. * m);
static_assert((sea_level + 42. * m).point_for(ocean_surface).point_for(sea_level).quantity_from(sea_level) == 42. * m);

////////////////////////////////////////////////////////////////////////////////////
// Walk-down after projection: sea_level → shallow_water (relative child of ocean_surface)
////////////////////////////////////////////////////////////////////////////////////

// Point 100 m below sea level → depth 100 m from ocean_surface.
// shallow_water is 10 m below ocean_surface.
// So relative to shallow_water: 100 − 10 = 90 m.
static_assert((sea_level + (-100. * m)).point_for(shallow_water).quantity_from(shallow_water) == 90. * m);

// Point at sea_level (altitude = 0) → depth 0 from ocean_surface → relative to shallow_water: 0 − 10 = −10 m.
static_assert((sea_level + 0. * m).point_for(shallow_water).quantity_from(shallow_water) == -10. * m);

////////////////////////////////////////////////////////////////////////////////////
// Extra-argument forwarding: shifted_origin → base_origin with an offset arg
////////////////////////////////////////////////////////////////////////////////////

static_assert((shifted_origin + 10. * m).point_for(base_origin, 5. * m).quantity_from(base_origin) == 15. * m);
static_assert((shifted_origin + 0. * m).point_for(base_origin, -3. * m).quantity_from(base_origin) == -3. * m);

////////////////////////////////////////////////////////////////////////////////////
// Callability constraint: point_for disabled when args don't match the functor
//
// GCC propagates "no matching function" as a hard error inside a bare
// static_assert(!requires { expr; }), so we use a template<auto&> concept
// wrapper to make the expression dependent — the same pattern used in
// dimension_test.cpp.
////////////////////////////////////////////////////////////////////////////////////

inline constexpr auto qp_at_sea_level = sea_level + 0. * m;
inline constexpr auto qp_at_shifted_origin = shifted_origin + 0. * m;

template<auto& qp>
concept no_point_for_base_no_args = requires { requires !requires { qp.point_for(base_origin); }; };

template<auto& qp>
concept no_point_for_base_int_arg = requires { requires !requires { qp.point_for(base_origin, 42); }; };

// No projection at all — HasFrameProjection is false
static_assert(no_point_for_base_no_args<qp_at_sea_level>);

// Projection exists but wrong extra-arg type (int instead of quantity)
static_assert(no_point_for_base_int_arg<qp_at_shifted_origin>);

// Projection exists but missing required arg
static_assert(no_point_for_base_no_args<qp_at_shifted_origin>);

////////////////////////////////////////////////////////////////////////////////////
// Type checks for point_for return type
// Note: 100. * m yields quantity<si::metre, double> so the reference is si::metre.
////////////////////////////////////////////////////////////////////////////////////

static_assert(
  is_of_type<(sea_level + (-100. * m)).point_for(ocean_surface), quantity_point<si::metre, ocean_surface, double>>);

static_assert(
  is_of_type<(ocean_surface + 100. * m).point_for(sea_level), quantity_point<si::metre, sea_level, double>>);

}  // namespace
