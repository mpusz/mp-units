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

#include <units/physical/si/speed.h>
#include <units/physical/international/length.h>
#include <units/math.h>
#include <units/format.h>
#include <units/quantity_point.h>
#include <array>
#include <iostream>
#include <compare>

// horizontal/vertical vector
namespace {

using namespace units;

enum class direction {
  horizontal,
  vertical
};

template<typename Q, direction D>
  requires Quantity<Q> || QuantityPoint<Q>
class vector {
public:
  using value_type = Q;
  using magnitude_type = Q;
  static constexpr direction dir = D;

  vector() = default;
  explicit constexpr vector(const Q& m): magnitude_(m) {}

  template<Quantity QQ>
    requires QuantityPoint<Q> && std::constructible_from<Q, QQ>
  explicit constexpr vector(const QQ& q) : magnitude_(q) {}

  constexpr Q magnitude() const { return magnitude_; }

  [[nodiscard]] constexpr vector operator-() const
    requires requires { -magnitude(); }
  {
    return vector(-magnitude());
  }

  template<typename Q2>
  constexpr vector& operator-=(const vector<Q2, D>& v)
    requires requires(Q q) { q -= v.magnitude(); }
  {
    magnitude_ -= v.magnitude();
    return *this;
  }

  template<typename Q2>
  [[nodiscard]] friend constexpr auto operator+(const vector& lhs, const vector<Q2, D>& rhs)
    requires requires { lhs.magnitude() + rhs.magnitude(); }
  {
    using ret_type = decltype(lhs.magnitude() + rhs.magnitude());
    return vector<ret_type, D>(lhs.magnitude() + rhs.magnitude());
  }

  template<typename Q2>
  [[nodiscard]] friend constexpr auto operator-(const vector& lhs, const vector<Q2, D>& rhs)
    requires requires { lhs.magnitude() - rhs.magnitude(); }
  {
    using ret_type = decltype(lhs.magnitude() - rhs.magnitude());
    return vector<ret_type, D>(lhs.magnitude() - rhs.magnitude());
  }

  template<typename V>
    requires (Scalar<V> || Dimensionless<V>)
  [[nodiscard]] friend constexpr auto operator*(const vector& lhs, const V& value)
    requires requires { lhs.magnitude() * value; }
  {
    return vector<Q, D>(lhs.magnitude() * value);
  }

  template<typename V>
    requires (Scalar<V> || Dimensionless<V>)
  [[nodiscard]] friend constexpr auto operator*(const V& value, const vector& rhs)
    requires requires { value * rhs.magnitude(); }
  {
    return vector<Q, D>(value * rhs.magnitude());
  }

  template<typename Q2, direction D2>
  [[nodiscard]] friend constexpr auto operator/(const vector& lhs, const vector<Q2, D2>& rhs)
    requires requires { lhs.magnitude() / rhs.magnitude(); }
  {
    return lhs.magnitude() / rhs.magnitude();
  }

  template<typename Q2>
  [[nodiscard]] friend constexpr auto operator<=>(const vector& lhs, const vector<Q2, D>& rhs)
    requires requires { lhs.magnitude() <=> rhs.magnitude(); }
  {
    return lhs.magnitude() <=> rhs.magnitude();
  }

  template<typename Q2>
  [[nodiscard]] friend constexpr bool operator==(const vector& lhs, const vector<Q2, D>& rhs)
    requires requires { lhs.magnitude() == rhs.magnitude(); }
  {
    return lhs.magnitude() == rhs.magnitude();
  }

  template<class CharT, class Traits>
    requires Quantity<Q>
  friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const vector& v)
  {
    return os << v.magnitude();
  }

private:
  Q magnitude_{};
};


template<typename T>
inline constexpr bool is_vector = false;
template<typename Q, direction D>
inline constexpr bool is_vector<vector<Q, D>> = true;

} // namespace

template<Quantity Q, direction D>
struct fmt::formatter<vector<Q, D>> : formatter<Q> {
  template <typename FormatContext>
  auto format(const vector<Q, D>& v, FormatContext& ctx)
  {
    return formatter<Q>::format(v.magnitude(), ctx);
  }
};

// custom types
namespace {

using namespace units::physical;

using distance = vector<si::length<si::kilometre>, direction::horizontal>;
using height = vector<si::length<si::metre>, direction::vertical>;
using altitude = vector<quantity_point<si::dim_length, si::metre>, direction::vertical>;

using duration = si::time<si::second>;

using velocity = vector<si::speed<si::kilometre_per_hour>, direction::horizontal>;
using rate_of_climb = vector<si::speed<si::metre_per_second>, direction::vertical>;

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const altitude& a)
{
  return os << a.magnitude().relative() << " AMSL";
}

} // namespace

template<>
struct fmt::formatter<altitude> : formatter<si::length<si::metre>> {
  template <typename FormatContext>
  auto format(altitude a, FormatContext& ctx)
  {
    formatter<si::length<si::metre>>::format(a.magnitude().relative(), ctx);
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

using namespace units::physical::si::literals;
using namespace units::physical::international::literals;

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
    glider{"SZD-30 Pirat", {velocity(83q_km_per_h), rate_of_climb(-0.7389q_m_per_s)}},
    glider{"SZD-51 Junior", {velocity(80q_km_per_h), rate_of_climb(-0.6349q_m_per_s)}},
    glider{"SZD-48 Jantar Std 3", {velocity(110q_km_per_h), rate_of_climb(-0.77355q_m_per_s)}},
    glider{"SZD-56 Diana", {velocity(110q_km_per_h), rate_of_climb(-0.63657q_m_per_s)}}
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
    std::pair("Good", weather{height(1900q_m), rate_of_climb(4.3q_m_per_s)}),
    std::pair("Medium", weather{height(1550q_m), rate_of_climb(2.8q_m_per_s)}),
    std::pair("Bad", weather{height(850q_m), rate_of_climb(1.8q_m_per_s)})
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
  return t.start.alt + alt_diff * (dist / t.dist);
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
  const duration d = at.height_agl / at.performance;
  const flight_point new_point{point.dur + d, point.dist, point.alt + at.height_agl};

  print("Tow", point, new_point);
  return new_point;
}

flight_point circle(const flight_point& point, const glider& g, const weather& w, const task& t, height& height_to_gain)
{
  const height h_agl = agl(point.alt, terrain_level_alt(t, point.dist));
  const height circle_height = std::min(w.cloud_base - h_agl, height_to_gain);
  const rate_of_climb circling_rate = w.thermal_strength + g.polar[0].climb;
  const duration d = circle_height / circling_rate;
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
  return distance((ground_alt + s.min_agl_height - point.alt).magnitude() / ((ground_alt - t.finish.alt) / dist_to_finish - 1 / glide_ratio(g.polar[0])));
}

inline si::length<si::kilometre> length_3d(distance dist, height h)
{
  return sqrt(pow<2>(dist.magnitude()) + pow<2>(h.magnitude()));
}

flight_point glide(const flight_point& point, const glider& g, const task& t, const safety& s)
{
  const auto ground_alt = terrain_level_alt(t, point.dist);
  const auto dist = glide_distance(point, g, t, s, ground_alt);
  const auto alt = ground_alt + s.min_agl_height;
  const auto l3d = length_3d(dist, point.alt - alt);
  const duration d = l3d / g.polar[0].v.magnitude();
  const flight_point new_point{point.dur + d, point.dist + dist, terrain_level_alt(t, point.dist + dist) + s.min_agl_height};

  print("Glide", point, new_point);
  return new_point;
}

flight_point final_glide(const flight_point& point, const glider& g, const task& t)
{
  const auto dist = t.dist - point.dist;
  const auto l3d = length_3d(dist, point.alt - t.finish.alt);
  const duration d = l3d / g.polar[0].v.magnitude();
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
  const altitude final_glide_alt = t.finish.alt + height(t.dist.magnitude() / glide_ratio(g.polar[0]));

  // how much height we still need to gain in the thermalls to reach the destination?
  height height_to_gain = final_glide_alt - point.alt;

  do {
    // glide to the next thermall
    point = glide(point, g, t, s);

    // circle in a thermall to gain height
    point = circle(point, g, w, t, height_to_gain);
  }
  while(height_to_gain > height(0q_m));

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
    waypoint{"EPPR", altitude(16q_ft)},
    waypoint{"EPGI", altitude(115q_ft)},
    distance(81.7q_km)
  };
  print(t);

  const safety s = {height(300q_m)};
  print(s);

  const aircraft_tow tow = {height(400q_m), rate_of_climb(1.6q_m_per_s)};
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
