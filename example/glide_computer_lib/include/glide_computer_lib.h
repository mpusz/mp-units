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
#include <mp-units/chrono.h>
#include <mp-units/math.h>  // IWYU pragma: keep
#include <mp-units/quantity_point.h>
#include <mp-units/systems/isq/space_and_time.h>
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
// - no ground obstacles (e.g. mountains) to pass
// - flight path exactly on a shortest possible line to destination

namespace glide_computer {

// https://en.wikipedia.org/wiki/Flight_planning#Units_of_measurement
QUANTITY_SPEC(rate_of_climb_speed, mp_units::isq::speed, mp_units::isq::height / mp_units::isq::time);

// length
using distance = mp_units::quantity<mp_units::isq::distance[mp_units::si::kilo<mp_units::si::metre>]>;
using height = mp_units::quantity<mp_units::isq::height[mp_units::si::metre]>;

// time
using duration = mp_units::quantity<mp_units::isq::duration[mp_units::si::second]>;
using timestamp = mp_units::quantity_point<mp_units::isq::time[mp_units::si::second],
                                           mp_units::chrono_point_origin<std::chrono::system_clock>>;

// speed
using velocity = mp_units::quantity<mp_units::isq::speed[mp_units::si::kilo<mp_units::si::metre> / mp_units::si::hour]>;
using rate_of_climb = mp_units::quantity<rate_of_climb_speed[mp_units::si::metre / mp_units::si::second]>;

// definition of glide computer databases and utilities
struct glider {
  struct polar_point {
    velocity v;
    rate_of_climb climb;
  };

  std::string name;
  std::array<polar_point, 1> polar;
};

constexpr mp_units::QuantityOf<mp_units::dimensionless> auto glide_ratio(const glider::polar_point& polar)
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
  geographic::msl_altitude alt;
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
    constexpr distance get_distance() const { return length_; }
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

  distance get_distance() const { return length_; }

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
  geographic::msl_altitude alt;
  std::size_t leg_idx = 0;
  distance dist{};
};

geographic::msl_altitude terrain_level_alt(const task& t, const flight_point& pos);

constexpr height agl(geographic::msl_altitude glider_alt, geographic::msl_altitude terrain_level)
{
  return glider_alt - terrain_level;
}

inline mp_units::quantity<mp_units::isq::length[mp_units::si::kilo<mp_units::si::metre>]> length_3d(distance dist,
                                                                                                    height h)
{
  return hypot(dist, h);
}

distance glide_distance(const flight_point& pos, const glider& g, const task& t, const safety& s,
                        geographic::msl_altitude ground_alt);

void estimate(timestamp start_ts, const glider& g, const weather& w, const task& t, const safety& s,
              const aircraft_tow& at);

}  // namespace glide_computer
