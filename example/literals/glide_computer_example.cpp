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

#include "glide_computer.h"
#include <units/bits/fmt_hacks.h>
#include <units/chrono.h>
#include <units/generic/dimensionless.h>
#include <units/isq/si/international/length.h>

#include <array>
#include <exception>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

namespace {

using namespace glide_computer;

using namespace units::isq;

auto get_gliders()
{
  using namespace si::literals;
UNITS_DIAGNOSTIC_PUSH
UNITS_DIAGNOSTIC_IGNORE_MISSING_BRACES
  static const std::array gliders = {
      glider{"SZD-30 Pirat", {velocity(83_q_km_per_h), rate_of_climb(-0.7389_q_m_per_s)}},
      glider{"SZD-51 Junior", {velocity(80_q_km_per_h), rate_of_climb(-0.6349_q_m_per_s)}},
      glider{"SZD-48 Jantar Std 3", {velocity(110_q_km_per_h), rate_of_climb(-0.77355_q_m_per_s)}},
      glider{"SZD-56 Diana", {velocity(110_q_km_per_h), rate_of_climb(-0.63657_q_m_per_s)}}};
UNITS_DIAGNOSTIC_POP
  return gliders;
}

auto get_weather_conditions()
{
  using namespace si::literals;
  static const std::array weather_conditions = {
      std::pair("Good", weather{height(1900_q_m), rate_of_climb(4.3_q_m_per_s)}),
      std::pair("Medium", weather{height(1550_q_m), rate_of_climb(2.8_q_m_per_s)}),
      std::pair("Bad", weather{height(850_q_m), rate_of_climb(1.8_q_m_per_s)})};
  return weather_conditions;
}

auto get_waypoints()
{
  using namespace geographic::literals;
  using namespace units::isq::si::international::literals;
  static const std::array waypoints = {
      waypoint{"EPPR", {54.24772_N, 18.6745_E}, altitude(16_q_ft)},    // N54°14'51.8" E18°40'28.2"
      waypoint{"EPGI", {53.52442_N, 18.84947_E}, altitude(115_q_ft)}   // N53°31'27.9" E18°50'58.1"
  };
  return waypoints;
}

template<std::ranges::input_range R>
  requires std::same_as<std::ranges::range_value_t<R>, glider>
void print(const R& gliders)
{
  std::cout << "Gliders:\n";
  std::cout << "========\n";
  for (const auto& g : gliders) {
    std::cout << "- Name: " << g.name << "\n";
    std::cout << "- Polar:\n";
    for (const auto& p : g.polar)
      std::cout << STD_FMT::format("  * {:%.4Q %q} @ {:%.1Q %q} -> {:%.1Q %q}\n", p.climb, p.v, units::quantity_cast<units::one>(glide_ratio(g.polar[0])));
    std::cout << "\n";
  }
}

template<std::ranges::input_range R>
  requires std::same_as<std::ranges::range_value_t<R>, std::pair<const char*, weather>>
void print(const R& conditions)
{
  std::cout << "Weather:\n";
  std::cout << "========\n";
  for (const auto& c : conditions) {
    std::cout << "- " << c.first << "\n";
    const auto& w = c.second;
    std::cout << "  * Cloud base:        " << STD_FMT::format("{:%.0Q %q}", w.cloud_base) << " AGL\n";
    std::cout << "  * Thermals strength: " << STD_FMT::format("{:%.1Q %q}", w.thermal_strength) << "\n";
    std::cout << "\n";
  }
}

template<std::ranges::input_range R>
  requires std::same_as<std::ranges::range_value_t<R>, waypoint>
void print(const R& waypoints)
{
  std::cout << "Waypoints:\n";
  std::cout << "==========\n";
  for (const auto& w : waypoints)
    std::cout << STD_FMT::format("- {}: {} {}, {:%.1Q %q}\n", w.name, w.pos.lat, w.pos.lon, w.alt);
  std::cout << "\n";
}

void print(const task& t)
{
  std::cout << "Task:\n";
  std::cout << "=====\n";

  std::cout << "- Start: " << t.get_start().name << "\n";
  std::cout << "- Finish: " << t.get_finish().name << "\n";
  std::cout << "- Length:  " << STD_FMT::format("{:%.1Q %q}", t.get_length()) << "\n";

  std::cout << "- Legs: " << "\n";
  for (const auto& l : t.get_legs())
    std::cout << STD_FMT::format("  * {} -> {} ({:%.1Q %q})\n", l.begin().name, l.end().name, l.get_length());
  std::cout << "\n";
}

void print(const safety& s)
{
  std::cout << "Safety:\n";
  std::cout << "=======\n";
  std::cout << "- Min AGL separation: " << STD_FMT::format("{:%.0Q %q}", s.min_agl_height) << "\n";
  std::cout << "\n";
}

void print(const aircraft_tow& tow)
{
  std::cout << "Tow:\n";
  std::cout << "====\n";
  std::cout << "- Type:        aircraft\n";
  std::cout << "- Height:      " << STD_FMT::format("{:%.0Q %q}", tow.height_agl) << "\n";
  std::cout << "- Performance: " << STD_FMT::format("{:%.1Q %q}", tow.performance) << "\n";
  std::cout << "\n";
}

void example()
{
  using namespace si::literals;

  const safety sfty = {height(300_q_m)};
  const auto gliders = get_gliders();
  const auto waypoints = get_waypoints();
  const auto weather_conditions = get_weather_conditions();
  const task t = {waypoints[0], waypoints[1], waypoints[0]};
  const aircraft_tow tow = {height(400_q_m), rate_of_climb(1.6_q_m_per_s)};
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
      std::cout << STD_FMT::format("{0:=^{1}}\n\n", "", txt.size());

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
