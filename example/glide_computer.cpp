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

#include <units/format.h>
#include <units/math.h>
#include <units/physical/si/international/base/length.h>
#include <units/physical/si/derived/speed.h>
#include <units/quantity_point_kind.h>
#include <array>
#include <compare>
#include <iostream>

template<units::QuantityKind QK>
struct fmt::formatter<QK> : formatter<typename QK::quantity_type> {
  template <typename FormatContext>
  auto format(const QK& v, FormatContext& ctx)
  {
    return formatter<typename QK::quantity_type>::format(v.common(), ctx);
  }
};

// custom types
namespace {

using namespace units::physical;
using namespace units;

struct horizontal_vector : kind<horizontal_vector, si::dim_length> {};
struct vertical_vector : kind<vertical_vector, si::dim_length> {};
struct vertical_point : point_kind<vertical_point, vertical_vector> {};
struct velocity_vector : derived_kind<velocity_vector, horizontal_vector, si::dim_speed> {};
struct rate_of_climb_vector : derived_kind<rate_of_climb_vector, vertical_vector, si::dim_speed> {};

using distance = quantity_kind<horizontal_vector, si::kilometre>;
using height = quantity_kind<vertical_vector, si::metre>;
using altitude = quantity_point_kind<vertical_point, si::metre>;

using duration = si::time<si::second>;

using velocity = quantity_kind<velocity_vector, si::kilometre_per_hour>;
using rate_of_climb = quantity_kind<rate_of_climb_vector, si::metre_per_second>;

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const altitude& a)
{
  return os << a.relative().common() << " AMSL";
}

} // namespace

template<>
struct fmt::formatter<altitude> : formatter<si::length<si::metre>> {
  template <typename FormatContext>
  auto format(altitude a, FormatContext& ctx)
  {
    formatter<si::length<si::metre>>::format(a.relative().common(), ctx);
    return format_to(ctx.out(), " AMSL");
  }
};

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
// - Earth is flat
// - ground level changes linearly between airports
// - no ground obstacles (i.e. mountains) to pass
// - flight path exactly on a shortest possible line to destination

// gliders database
namespace {

using namespace si::literals;
using namespace si::international::literals;
using namespace si::unit_constants;

template<QuantityKind QK1, QuantityKind QK2>
constexpr Dimensionless auto operator/(const QK1& lhs, const QK2& rhs)
  requires (!units::QuantityKindRelatedTo<QK1, QK2>) && requires { lhs.common() / rhs.common(); } {
  return lhs.common() / rhs.common();
}

struct glider {
  struct polar_point {
    velocity v;
    rate_of_climb climb;
  };

  std::string name;
  std::array<polar_point, 1> polar;
};

auto get_gliders()
{
  const std::array gliders = {
    glider{"SZD-30 Pirat", {velocity(83 * km / h), rate_of_climb(-0.7389 * m / s)}},
    glider{"SZD-51 Junior", {velocity(80 * km / h), rate_of_climb(-0.6349 * m / s)}},
    glider{"SZD-48 Jantar Std 3", {velocity(110 * km / h), rate_of_climb(-0.77355 * m / s)}},
    glider{"SZD-56 Diana", {velocity(110 * km / h), rate_of_climb(-0.63657 * m / s)}}
  };
  return gliders;
}

constexpr Dimensionless auto glide_ratio(const glider::polar_point& polar)
{
  return polar.v / -polar.climb;
}

template<std::ranges::forward_range R>
  requires std::same_as<std::ranges::range_value_t<R>, glider>
void print(const R& gliders)
{
  std::cout << "Gliders:\n";
  std::cout << "========\n";
  for(const auto& g : gliders) {
    std::cout << "- Name: " << g.name << "\n";
    std::cout << "- Polar:\n";
    for(const auto& p : g.polar)
      fmt::print("  * {:%.4Q %q} @ {:%.1Q %q} -> {:.1f}\n", p.climb, p.v, glide_ratio(g.polar[0]));
    std::cout << "\n";
  }
}

} // namespace

// weather conditions
namespace {

struct weather {
  height cloud_base;
  rate_of_climb thermal_strength;
};

auto get_weather_conditions()
{
  const std::array weather_conditions = {
    std::pair("Good", weather{height(1900 * m), rate_of_climb(4.3 * m / s)}),
    std::pair("Medium", weather{height(1550 * m), rate_of_climb(2.8 * m / s)}),
    std::pair("Bad", weather{height(850 * m), rate_of_climb(1.8 * m / s)})
  };
  return weather_conditions;
}

template<std::ranges::forward_range R>
  requires std::same_as<std::ranges::range_value_t<R>, std::pair<const char*, weather>>
void print(const R& conditions)
{
  std::cout << "Weather:\n";
  std::cout << "========\n";
  for(const auto& c : conditions) {
    std::cout << "- Kind:              " << c.first << "\n";
    const auto& w = c.second;
    std::cout << "- Cloud base:        " << fmt::format("{:%.0Q %q}", w.cloud_base) << " AGL\n";
    std::cout << "- Thermals strength: " << fmt::format("{:%.1Q %q}", w.thermal_strength) << "\n";
    std::cout << "\n";
  }
}

} // namespace

// task
namespace {

struct waypoint {
  std::string name;
  altitude alt;
};

struct task {
  waypoint start;
  waypoint finish;
  distance dist;
};

void print(const task& t)
{
  std::cout << "Task:\n";
  std::cout << "=====\n";
  std::cout << "- Start:    " << t.start.name << " (" << fmt::format("{:%.1Q %q}", t.start.alt) << ")\n";
  std::cout << "- Finish:   " << t.finish.name << " (" << fmt::format("{:%.1Q %q}", t.finish.alt) << ")\n";
  std::cout << "- Distance: " << fmt::format("{:%.1Q %q}", t.dist) << "\n";
  std::cout << "\n";
}

} // namespace

// safety params
namespace {

struct safety {
  height min_agl_height;
};

void print(const safety& s)
{
  std::cout << "Safety:\n";
  std::cout << "=======\n";
  std::cout << "- Min AGL separation: " << fmt::format("{:%.0Q %q}", s.min_agl_height) << "\n";
  std::cout << "\n";
}

} // namespace

// tow parameters
namespace {

struct aircraft_tow {
  height height_agl;
  rate_of_climb performance;
};

void print(const aircraft_tow& tow)
{
  std::cout << "Tow:\n";
  std::cout << "====\n";
  std::cout << " - Type:        aircraft\n" ;
  std::cout << " - Height:      " << fmt::format("{:%.0Q %q}", tow.height_agl) <<"\n";
  std::cout << " - Performance: " << fmt::format("{:%.1Q %q}", tow.performance) <<"\n";
  std::cout << "\n";
}

} // namespace

// tactical flight computer basics
namespace {

struct flight_point {
  duration dur;
  distance dist;
  altitude alt;
};

constexpr altitude terrain_level_alt(const task& t, distance dist)
{
  const height alt_diff = t.finish.alt - t.start.alt;
  return t.start.alt + alt_diff * (dist / t.dist).common();
}

constexpr height agl(altitude glider_alt, altitude terrain_level)
{
  return glider_alt - terrain_level;
}

flight_point takeoff(const task& t)
{
  const flight_point new_point{{}, {}, t.start.alt};
  return new_point;
}

void print(std::string_view phase_name, const flight_point& point, const flight_point& new_point)
{
  fmt::print("| {:<12} | {:>9%.1Q %q} (Total: {:>9%.1Q %q}) | {:>8%.1Q %q} (Total: {:>8%.1Q %q}) | {:>7%.0Q %q} ({:>6%.0Q %q}) |\n",
    phase_name,
    quantity_cast<si::minute>(new_point.dur - point.dur), quantity_cast<si::minute>(new_point.dur),
    new_point.dist - point.dist, new_point.dist,
    new_point.alt - point.alt, new_point.alt);
}

flight_point tow(const flight_point& point, const aircraft_tow& at)
{
  const duration d = (at.height_agl / at.performance).common();
  const flight_point new_point{point.dur + d, point.dist, point.alt + at.height_agl};

  print("Tow", point, new_point);
  return new_point;
}

flight_point circle(const flight_point& point, const glider& g, const weather& w, const task& t, height& height_to_gain)
{
  const height h_agl = agl(point.alt, terrain_level_alt(t, point.dist));
  const height circle_height = std::min(w.cloud_base - h_agl, height_to_gain);
  const rate_of_climb circling_rate = w.thermal_strength + g.polar[0].climb;
  const duration d = (circle_height / circling_rate).common();
  const flight_point new_point{point.dur + d, point.dist, point.alt + circle_height};

  height_to_gain -= circle_height;

  print("Circle", point, new_point);
  return new_point;
}

// Returns `x` of the intersection of a glide line and a terrain line.
// y = -x / glide_ratio + point.alt;
// y = (finish_alt - ground_alt) / dist_to_finish * x + ground_alt + min_agl_height;
constexpr distance glide_distance(const flight_point& point, const glider& g, const task& t, const safety& s, altitude ground_alt)
{
  const auto dist_to_finish = t.dist - point.dist;
  return distance((ground_alt + s.min_agl_height - point.alt).common() / ((ground_alt - t.finish.alt) / dist_to_finish - 1 / glide_ratio(g.polar[0])));
}

inline si::length<si::kilometre> length_3d(distance dist, height h)
{
  return sqrt(pow<2>(dist.common()) + pow<2>(h.common()));
}

flight_point glide(const flight_point& point, const glider& g, const task& t, const safety& s)
{
  const auto ground_alt = terrain_level_alt(t, point.dist);
  const auto dist = glide_distance(point, g, t, s, ground_alt);
  const auto alt = ground_alt + s.min_agl_height;
  const auto l3d = length_3d(dist, point.alt - alt);
  const duration d = l3d / g.polar[0].v.common();
  const flight_point new_point{point.dur + d, point.dist + dist, terrain_level_alt(t, point.dist + dist) + s.min_agl_height};

  print("Glide", point, new_point);
  return new_point;
}

flight_point final_glide(const flight_point& point, const glider& g, const task& t)
{
  const auto dist = t.dist - point.dist;
  const auto l3d = length_3d(dist, point.alt - t.finish.alt);
  const duration d = l3d / g.polar[0].v.common();
  const flight_point new_point{point.dur + d, point.dist + dist, t.finish.alt};

  print("Final Glide", point, new_point);
  return new_point;
}

void estimate(const glider& g, const weather& w, const task& t, const safety& s, const aircraft_tow& at)
{
  // ready to takeoff
  flight_point point = takeoff(t);

  // estimate aircraft towing
  point = tow(point, at);

  // estimate the altitude needed to reach the finish line from this place
  const altitude final_glide_alt = t.finish.alt + height(t.dist.common() / glide_ratio(g.polar[0]));

  // how much height we still need to gain in the thermalls to reach the destination?
  height height_to_gain = final_glide_alt - point.alt;

  do {
    // glide to the next thermall
    point = glide(point, g, t, s);

    // circle in a thermall to gain height
    point = circle(point, g, w, t, height_to_gain);
  }
  while(height_to_gain > height(0 * m));

  // final glide
  point = final_glide(point, g, t);
}

} // namespace

// example
namespace {

void example()
{
  const auto gliders = get_gliders();
  print(gliders);

  const auto weather_conditions = get_weather_conditions();
  print(weather_conditions);

  const task t = {
    waypoint{"EPPR", altitude(16_q_ft)},
    waypoint{"EPGI", altitude(115_q_ft)},
    distance(81.7_q_km)
  };
  print(t);

  const safety s = {height(300 * m)};
  print(s);

  const aircraft_tow tow = {height(400 * m), rate_of_climb(1.6 * m / ::s)};
  print(tow);

  for(const auto& g : gliders) {
    for(const auto& c : weather_conditions) {
      std::string txt = "Scenario: Glider = " + g.name + ", Weather = " + c.first;
      std::cout << txt << "\n";
      fmt::print("{0:=^{1}}\n\n", "", txt.size());
      fmt::print("| {:<12} | {:^28} | {:^26} | {:^21} |\n", "Flight phase", "Duration", "Distance", "Height");
      fmt::print("|{0:-^14}|{0:-^30}|{0:-^28}|{0:-^23}|\n", "");

      estimate(g, c.second, t, s, tow);

      std::cout << "\n\n";
    }
  }
}

} // namespace

int main()
{
  try {
    example();
  }
  catch (const std::exception& ex) {
    std::cerr << "Unhandled std exception caught: " << ex.what() << '\n';
  }
  catch (...) {
    std::cerr << "Unhandled unknown exception caught\n";
  }
}
