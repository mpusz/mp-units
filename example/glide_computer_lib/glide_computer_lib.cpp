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

#include "glide_computer_lib.h"
#include <mp-units/format.h>
#include <iostream>
#include <numeric>
#include <string_view>

namespace glide_computer {

using namespace mp_units;

task::legs task::make_legs(const waypoints& wpts)
{
  task::legs res;
  res.reserve(wpts.size() - 1);
  auto to_leg = [](const waypoint& w1, const waypoint& w2) { return task::leg(w1, w2); };
  std::ranges::transform(wpts.cbegin(), prev(wpts.cend()), next(wpts.cbegin()), wpts.cend(), std::back_inserter(res),
                         to_leg);
  return res;
}

std::vector<distance> task::make_leg_total_distances(const legs& legs)
{
  std::vector<distance> res;
  res.reserve(legs.size());
  auto to_length = [](const leg& l) { return l.get_distance(); };
  std::transform_inclusive_scan(legs.cbegin(), legs.cend(), std::back_inserter(res), std::plus(), to_length,
                                distance::zero());
  return res;
}

geographic::msl_altitude terrain_level_alt(const task& t, const flight_point& pos)
{
  const task::leg& l = t.get_legs()[pos.leg_idx];
  const height alt_diff = l.end().alt - l.begin().alt;
  return l.begin().alt + alt_diff * ((pos.dist - t.get_leg_dist_offset(pos.leg_idx)) / l.get_distance());
}

// Returns `x` of the intersection of a glide line and a terrain line.
// y = -x / glide_ratio + pos.alt;
// y = (finish_alt - ground_alt) / dist_to_finish * x + ground_alt + min_agl_height;
distance glide_distance(const flight_point& pos, const glider& g, const task& t, const safety& s,
                        geographic::msl_altitude ground_alt)
{
  const auto dist_to_finish = t.get_distance() - pos.dist;
  return quantity_cast<isq::distance>(ground_alt + s.min_agl_height - pos.alt) /
         ((ground_alt - t.get_finish().alt) / dist_to_finish - 1 / glide_ratio(g.polar[0]));
}

}  // namespace glide_computer

namespace {

using namespace glide_computer;

void print(std::string_view phase_name, timestamp start_ts, const glide_computer::flight_point& point,
           const glide_computer::flight_point& new_point)
{
  std::cout << MP_UNITS_STD_FMT::format(
    "| {:<12} | {:>9%.1Q %q} (Total: {:>9%.1Q %q}) | {:>8%.1Q %q} (Total: {:>8%.1Q %q}) | {:>7%.0Q %q} ({:>6%.0Q %q}) "
    "|\n",
    phase_name, value_cast<si::minute>(new_point.ts - point.ts), value_cast<si::minute>(new_point.ts - start_ts),
    new_point.dist - point.dist, new_point.dist, new_point.alt - point.alt, new_point.alt);
}

flight_point takeoff(timestamp start_ts, const task& t) { return {start_ts, t.get_start().alt}; }

flight_point tow(timestamp start_ts, const flight_point& pos, const aircraft_tow& at)
{
  const duration d = (at.height_agl / at.performance);
  const flight_point new_pos{pos.ts + d, pos.alt + at.height_agl, pos.leg_idx, pos.dist};

  print("Tow", start_ts, pos, new_pos);
  return new_pos;
}

flight_point circle(timestamp start_ts, const flight_point& pos, const glider& g, const weather& w, const task& t,
                    height& height_to_gain)
{
  const height h_agl = agl(pos.alt, terrain_level_alt(t, pos));
  const height circling_height = std::min(w.cloud_base - h_agl, height_to_gain);
  const rate_of_climb circling_rate = w.thermal_strength + g.polar[0].climb;
  const duration d = (circling_height / circling_rate);
  const flight_point new_pos{pos.ts + d, pos.alt + circling_height, pos.leg_idx, pos.dist};

  height_to_gain -= circling_height;

  print("Circle", start_ts, pos, new_pos);
  return new_pos;
}

flight_point glide(timestamp start_ts, const flight_point& pos, const glider& g, const task& t, const safety& s)
{
  const auto ground_alt = terrain_level_alt(t, pos);
  const auto dist = glide_distance(pos, g, t, s, ground_alt);
  const auto new_distance = pos.dist + dist;
  const auto alt = ground_alt + s.min_agl_height;
  const auto l3d = length_3d(dist, pos.alt - alt);
  const duration d = l3d / g.polar[0].v;
  const flight_point new_pos{pos.ts + d, terrain_level_alt(t, pos) + s.min_agl_height, t.get_leg_index(new_distance),
                             new_distance};

  print("Glide", start_ts, pos, new_pos);
  return new_pos;
}

flight_point final_glide(timestamp start_ts, const flight_point& pos, const glider& g, const task& t)
{
  const auto dist = t.get_distance() - pos.dist;
  const auto l3d = length_3d(dist, pos.alt - t.get_finish().alt);
  const duration d = l3d / g.polar[0].v;
  const flight_point new_pos{pos.ts + d, t.get_finish().alt, t.get_legs().size() - 1, pos.dist + dist};

  print("Final Glide", start_ts, pos, new_pos);
  return new_pos;
}

}  // namespace

namespace glide_computer {

void estimate(timestamp start_ts, const glider& g, const weather& w, const task& t, const safety& s,
              const aircraft_tow& at)
{
  std::cout << MP_UNITS_STD_FMT::format("| {:<12} | {:^28} | {:^26} | {:^21} |\n", "Flight phase", "Duration",
                                        "Distance", "Height");
  std::cout << MP_UNITS_STD_FMT::format("|{0:-^14}|{0:-^30}|{0:-^28}|{0:-^23}|\n", "");

  // ready to takeoff
  flight_point pos = takeoff(start_ts, t);

  // estimate aircraft towing
  pos = tow(start_ts, pos, at);

  // estimate the msl_altitude needed to reach the finish line from this place
  const geographic::msl_altitude final_glide_alt =
    t.get_finish().alt + quantity_cast<isq::height>(t.get_distance() / glide_ratio(g.polar[0]));

  // how much height we still need to gain in the thermalls to reach the destination?
  height height_to_gain = final_glide_alt - pos.alt;

  do {
    // glide to the next thermall
    pos = glide(start_ts, pos, g, t, s);

    // circle in a thermall to gain height
    pos = circle(start_ts, pos, g, w, t, height_to_gain);
  } while (height_to_gain > height{});

  // final glide
  pos = final_glide(start_ts, pos, g, t);
}

}  // namespace glide_computer
