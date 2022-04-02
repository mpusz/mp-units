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

#pragma once

// IWYU pragma: begin_exports
#include "geographic.h"
#include <units/isq/si/length.h>
#include <units/isq/si/speed.h>
#include <units/isq/si/time.h>
#include <units/quantity_point_kind.h>
// IWYU pragma: end_exports

#include <units/chrono.h>
#include <units/format.h>
#include <units/math.h>  // IWYU pragma: keep
#include <algorithm>
#include <array>
#include <initializer_list>
#include <iterator>
#include <ostream>
#include <ranges>
#include <string>  // IWYU pragma: keep
#include <vector>

// An example of a really simplified tactical glide computer
// Simplifications:
// - glider 100% clean and with full factory performance (brand new painting)
// - no influence of the ballast (pilot weight, water, etc) to glider performance
// - only one point on a glider polar curve
// - no influence of bank angle (during circling) on a glider performance
// - no wind
// - constant thermals strength
// - thermals exactly where and when we need them ;-)
// - no airspaces
// - ground level changes linearly between waypoints
// - no ground obstacles (i.e. mountains) to pass
// - flight path exactly on a shortest possible line to destination

template<units::QuantityKind QK>
struct STD_FMT::formatter<QK> : formatter<typename QK::quantity_type> {
  template<typename FormatContext>
  auto format(const QK& v, FormatContext& ctx)
  {
    return formatter<typename QK::quantity_type>::format(v.common(), ctx);
  }
};

namespace glide_computer {

template<units::QuantityKind QK1, units::QuantityKind QK2>
constexpr units::Dimensionless auto operator/(const QK1& lhs, const QK2& rhs)
  requires(!units::QuantityKindRelatedTo<QK1, QK2>) && requires { lhs.common() / rhs.common(); }
{
  return lhs.common() / rhs.common();
}

// kinds
using horizontal_kind = geographic::horizontal_kind;
struct vertical_kind : units::kind<vertical_kind, units::isq::si::dim_length> {};
struct vertical_point_kind : units::point_kind<vertical_point_kind, vertical_kind> {};
struct velocity_kind : units::derived_kind<velocity_kind, units::isq::si::dim_speed, horizontal_kind> {};
struct rate_of_climb_kind : units::derived_kind<rate_of_climb_kind, units::isq::si::dim_speed, vertical_kind> {};

// https://en.wikipedia.org/wiki/Flight_planning#Units_of_measurement
// length
using distance = units::quantity_kind<horizontal_kind, units::isq::si::kilometre>;
using height = units::quantity_kind<vertical_kind, units::isq::si::metre>;
using altitude = units::quantity_point_kind<vertical_point_kind, units::isq::si::metre>;

// time
using duration = units::isq::si::time<units::isq::si::second>;
using timestamp = units::quantity_point<units::clock_origin<std::chrono::system_clock>, units::isq::si::second>;

// speed
using velocity = units::quantity_kind<velocity_kind, units::isq::si::kilometre_per_hour>;
using rate_of_climb = units::quantity_kind<rate_of_climb_kind, units::isq::si::metre_per_second>;

// text output
template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const altitude& a)
{
  return os << a.relative().common() << " AMSL";
}

}  // namespace glide_computer

template<>
struct STD_FMT::formatter<glide_computer::altitude> : formatter<units::isq::si::length<units::isq::si::metre>> {
  template<typename FormatContext>
  auto format(glide_computer::altitude a, FormatContext& ctx)
  {
    formatter<units::isq::si::length<units::isq::si::metre>>::format(a.relative().common(), ctx);
    return STD_FMT::format_to(ctx.out(), " AMSL");
  }
};

// definition of glide computer databases and utilities
namespace glide_computer {

struct glider {
  struct polar_point {
    velocity v;
    rate_of_climb climb;
  };

  std::string name;
  std::array<polar_point, 1> polar;
};

constexpr units::Dimensionless auto glide_ratio(const glider::polar_point& polar) { return polar.v / -polar.climb; }

struct weather {
  height cloud_base;
  rate_of_climb thermal_strength;
};

struct waypoint {
  std::string name;
  geographic::position pos;
  altitude alt;
};

class task {
public:
  using waypoints = std::vector<waypoint>;

  class leg {
    const waypoint* begin_;
    const waypoint* end_;
    distance length_ = geographic::spherical_distance(begin().pos, end().pos);
  public:
    leg(const waypoint& b, const waypoint& e) noexcept : begin_(&b), end_(&e) {}
    constexpr const waypoint& begin() const { return *begin_; };
    constexpr const waypoint& end() const { return *end_; }
    constexpr const distance get_length() const { return length_; }
  };
  using legs = std::vector<leg>;

  template<std::ranges::input_range R>
    requires std::same_as<std::ranges::range_value_t<R>,
                          waypoint> explicit task(const R& r) :
      waypoints_(std::ranges::begin(r), std::ranges::end(r))
  {
  }

  task(std::initializer_list<waypoint> wpts) : waypoints_(wpts) {}

  const waypoints& get_waypoints() const { return waypoints_; }
  const legs& get_legs() const { return legs_; }

  const waypoint& get_start() const { return waypoints_.front(); }
  const waypoint& get_finish() const { return waypoints_.back(); }

  distance get_length() const { return length_; }

  distance get_leg_dist_offset(std::size_t leg_index) const
  {
    return leg_index == 0 ? distance{} : leg_total_distances_[leg_index - 1];
  }
  std::size_t get_leg_index(distance dist) const
  {
    return static_cast<std::size_t>(
      std::ranges::distance(leg_total_distances_.cbegin(), std::ranges::lower_bound(leg_total_distances_, dist)));
  }

private:
  waypoints waypoints_;
  legs legs_ = make_legs(waypoints_);
  std::vector<distance> leg_total_distances_ = make_leg_total_distances(legs_);
  distance length_ = leg_total_distances_.back();

  static legs make_legs(const task::waypoints& wpts);
  static std::vector<distance> make_leg_total_distances(const legs& legs);
};

struct safety {
  height min_agl_height;
};

struct aircraft_tow {
  height height_agl;
  rate_of_climb performance;
};

struct flight_point {
  timestamp ts;
  altitude alt;
  std::size_t leg_idx = 0;
  distance dist{};
};

altitude terrain_level_alt(const task& t, const flight_point& pos);

constexpr height agl(altitude glider_alt, altitude terrain_level) { return glider_alt - terrain_level; }

inline units::isq::si::length<units::isq::si::kilometre> length_3d(distance dist, height h)
{
  // TODO support for hypot(q, q)
  return sqrt(pow<2>(dist.common()) + pow<2>(h.common()));
}

distance glide_distance(const flight_point& pos, const glider& g, const task& t, const safety& s, altitude ground_alt);

void estimate(timestamp start_ts, const glider& g, const weather& w, const task& t, const safety& s,
              const aircraft_tow& at);

}  // namespace glide_computer
