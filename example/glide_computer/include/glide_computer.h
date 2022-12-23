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

#include "geographic.h"
#include <units/chrono.h>
#include <units/format.h>
#include <units/isq/space_and_time.h>
#include <units/math.h>  // IWYU pragma: keep
#include <units/quantity_point.h>
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

namespace glide_computer {

// https://en.wikipedia.org/wiki/Flight_planning#Units_of_measurement
inline constexpr struct mean_sea_level : units::absolute_point_origin<units::isq::height> {
} mean_sea_level;
QUANTITY_SPEC(rate_of_climb_speed, units::isq::height / units::isq::time);

// length
using distance = units::quantity<units::isq::distance[units::si::kilo<units::si::metre>]>;
using height = units::quantity<units::isq::height[units::si::metre]>;
using altitude = units::quantity_point<units::isq::altitude[units::si::metre], mean_sea_level>;

// time
using duration = units::quantity<units::isq::duration[units::si::second]>;
using timestamp =
  units::quantity_point<units::isq::time[units::si::second], units::chrono_point_origin<std::chrono::system_clock>{}>;

// speed
using velocity = units::quantity<units::isq::speed[units::si::kilo<units::si::metre> / units::si::hour]>;
using rate_of_climb = units::quantity<rate_of_climb_speed[units::si::metre / units::si::second]>;

// text output
template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const altitude& a)
{
  return os << a.absolute() << " AMSL";
}

}  // namespace glide_computer

template<>
struct STD_FMT::formatter<glide_computer::altitude> :
    formatter<units::quantity<units::isq::altitude[units::si::metre]>> {
  template<typename FormatContext>
  auto format(const glide_computer::altitude& a, FormatContext& ctx)
  {
    formatter<units::quantity<units::isq::altitude[units::si::metre]>>::format(a.absolute(), ctx);
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

constexpr units::weak_quantity_of<units::dimensionless> auto glide_ratio(const glider::polar_point& polar)
{
  return polar.v / -polar.climb;
}

struct weather {
  height cloud_base;
  rate_of_climb thermal_strength;
};

struct waypoint {
  std::string name;
  geographic::position<long double> pos;
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
    constexpr distance get_length() const { return length_; }
  };
  using legs = std::vector<leg>;

  template<std::ranges::input_range R>
    requires std::same_as<std::ranges::range_value_t<R>, waypoint>
  explicit task(const R& r) : waypoints_(std::ranges::begin(r), std::ranges::end(r))
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

inline units::quantity<units::isq::length[units::si::kilo<units::si::metre>]> length_3d(distance dist, height h)
{
  // TODO Should we be able to calculate this on quantity of different kinds? What to return?
  return hypot(quantity_cast<units::isq::length>(dist), quantity_cast<units::isq::length>(h));
}

distance glide_distance(const flight_point& pos, const glider& g, const task& t, const safety& s, altitude ground_alt);

void estimate(timestamp start_ts, const glider& g, const weather& w, const task& t, const safety& s,
              const aircraft_tow& at);

}  // namespace glide_computer
