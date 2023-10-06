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
#include <mp-units/bits/fmt_hacks.h>
#include <mp-units/chrono.h>
#include <mp-units/math.h>
#include <mp-units/systems/international/international.h>
#include <mp-units/systems/si/unit_symbols.h>
#include <array>
#include <exception>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

namespace {

using namespace geographic;
using namespace glide_computer;
using namespace mp_units;

auto get_gliders()
{
  using namespace mp_units::si::unit_symbols;
  MP_UNITS_DIAGNOSTIC_PUSH
  MP_UNITS_DIAGNOSTIC_IGNORE_MISSING_BRACES
  static const std::array gliders = {glider{"SZD-30 Pirat", {83 * km / h, -0.7389 * m / s}},
                                     glider{"SZD-51 Junior", {80 * km / h, -0.6349 * m / s}},
                                     glider{"SZD-48 Jantar Std 3", {110 * km / h, -0.77355 * m / s}},
                                     glider{"SZD-56 Diana", {110 * km / h, -0.63657 * m / s}}};
  MP_UNITS_DIAGNOSTIC_POP
  return gliders;
}

auto get_weather_conditions()
{
  using namespace mp_units::si::unit_symbols;
  static const std::array weather_conditions = {std::pair{"Good", weather{1900 * m, 4.3 * m / s}},
                                                std::pair{"Medium", weather{1550 * m, 2.8 * m / s}},
                                                std::pair{"Bad", weather{850 * m, 1.8 * m / s}}};
  return weather_conditions;
}

auto get_waypoints()
{
  using namespace geographic::literals;
  using namespace mp_units::international::unit_symbols;
  static const std::array waypoints = {
    waypoint{"EPPR", {54.24772_N, 18.6745_E}, mean_sea_level + 16. * ft},   // N54°14'51.8" E18°40'28.2"
    waypoint{"EPGI", {53.52442_N, 18.84947_E}, mean_sea_level + 115. * ft}  // N53°31'27.9" E18°50'58.1"
  };
  return waypoints;
}

template<std::ranges::input_range R>
  requires(std::same_as<std::ranges::range_value_t<R>, glider>)
void print(const R& gliders)
{
  std::cout << "Gliders:\n";
  std::cout << "========\n";
  for (const auto& g : gliders) {
    std::cout << "- Name: " << g.name << "\n";
    std::cout << "- Polar:\n";
    for (const auto& p : g.polar) {
      const auto ratio = glide_ratio(g.polar[0]).force_in(one);
      std::cout << MP_UNITS_STD_FMT::format("  * {:%.4Q %q} @ {:%.1Q %q} -> {:%.1Q %q} ({:%.1Q %q})\n", p.climb, p.v,
                                            ratio,
                                            // TODO is it possible to make ADL work below (we need another set of trig
                                            // functions for strong angle in a different namespace)
                                            isq::asin(1 / ratio).force_in(si::degree));
    }
    std::cout << "\n";
  }
}

template<std::ranges::input_range R>
  requires(std::same_as<std::ranges::range_value_t<R>, std::pair<const char*, weather>>)
void print(const R& conditions)
{
  std::cout << "Weather:\n";
  std::cout << "========\n";
  for (const auto& c : conditions) {
    std::cout << "- " << c.first << "\n";
    const auto& w = c.second;
    std::cout << "  * Cloud base:        " << MP_UNITS_STD_FMT::format("{:%.0Q %q}", w.cloud_base) << " AGL\n";
    std::cout << "  * Thermals strength: " << MP_UNITS_STD_FMT::format("{:%.1Q %q}", w.thermal_strength) << "\n";
    std::cout << "\n";
  }
}

template<std::ranges::input_range R>
  requires(std::same_as<std::ranges::range_value_t<R>, waypoint>)
void print(const R& waypoints)
{
  std::cout << "Waypoints:\n";
  std::cout << "==========\n";
  for (const auto& w : waypoints)
    std::cout << MP_UNITS_STD_FMT::format("- {}: {} {}, {:%.1Q %q}\n", w.name, w.pos.lat, w.pos.lon, w.alt);
  std::cout << "\n";
}

void print(const task& t)
{
  std::cout << "Task:\n";
  std::cout << "=====\n";

  std::cout << "- Start: " << t.get_start().name << "\n";
  std::cout << "- Finish: " << t.get_finish().name << "\n";
  std::cout << "- Length:  " << MP_UNITS_STD_FMT::format("{:%.1Q %q}", t.get_distance()) << "\n";

  std::cout << "- Legs: "
            << "\n";
  for (const auto& l : t.get_legs())
    std::cout << MP_UNITS_STD_FMT::format("  * {} -> {} ({:%.1Q %q})\n", l.begin().name, l.end().name,
                                          l.get_distance());
  std::cout << "\n";
}

void print(const safety& s)
{
  std::cout << "Safety:\n";
  std::cout << "=======\n";
  std::cout << "- Min AGL separation: " << MP_UNITS_STD_FMT::format("{:%.0Q %q}", s.min_agl_height) << "\n";
  std::cout << "\n";
}

void print(const aircraft_tow& tow)
{
  std::cout << "Tow:\n";
  std::cout << "====\n";
  std::cout << "- Type:        aircraft\n";
  std::cout << "- Height:      " << MP_UNITS_STD_FMT::format("{:%.0Q %q}", tow.height_agl) << "\n";
  std::cout << "- Performance: " << MP_UNITS_STD_FMT::format("{:%.1Q %q}", tow.performance) << "\n";
  std::cout << "\n";
}

void example()
{
  using namespace mp_units::si::unit_symbols;

  const safety sfty = {300 * m};
  const auto gliders = get_gliders();
  const auto waypoints = get_waypoints();
  const auto weather_conditions = get_weather_conditions();
  const task t = {waypoints[0], waypoints[1], waypoints[0]};
  const aircraft_tow tow = {400 * m, 1.6 * m / s};
  // TODO use C++20 date library when available
  // set `start_time` to 11:00 am today
  const timestamp start_time(std::chrono::system_clock::now());

  print(sfty);
  print(gliders);
  print(waypoints);
  print(weather_conditions);
  print(t);
  print(tow);

  for (const auto& g : gliders) {
    for (const auto& c : weather_conditions) {
      std::string txt = "Scenario: Glider = " + g.name + ", Weather = " + c.first;
      std::cout << txt << "\n";
      std::cout << MP_UNITS_STD_FMT::format("{0:=^{1}}\n\n", "", txt.size());

      estimate(start_time, g, c.second, t, sfty, tow);

      std::cout << "\n\n";
    }
  }
}

}  // namespace

int main()
{
  try {
    example();
  } catch (const std::exception& ex) {
    std::cerr << "Unhandled std exception caught: " << ex.what() << '\n';
  } catch (...) {
    std::cerr << "Unhandled unknown exception caught\n";
  }
}
