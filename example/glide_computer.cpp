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
  using magnitude_type = Q;
  static constexpr direction dir = D;

  vector() = default;
  explicit constexpr vector(Q m): magnitude_(std::move(m)) {}

  // template<Quantity QQ>
  //   requires QuantityPoint<Q>
  // explicit constexpr vector(QQ q)
  //   : magnitude_(std::move(q)) {}

  constexpr const Q& magnitude() const & { return magnitude_; }
  constexpr Q magnitude() const && { return std::move(magnitude_); }

  template<typename QQ = Q>
  [[nodiscard]] constexpr vector operator-() const
    requires requires(QQ q) { -q; }
    // requires requires { -magnitude(); } // TODO gated by gcc-9 (fixed in gcc-10)
  {
    return vector(-magnitude());
  }

#if __GNUC__ >= 10

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

#else

  template<typename Q2>
  [[nodiscard]] friend constexpr auto operator==(const vector& lhs, const vector<Q2, D>& rhs)
    requires requires { lhs.magnitude() == rhs.magnitude(); }
  {
    return lhs.magnitude() == rhs.magnitude();
  }

  template<typename Q2>
  [[nodiscard]] friend constexpr auto operator!=(const vector& lhs, const vector<Q2, D>& rhs)
    requires requires { lhs.magnitude() != rhs.magnitude(); }
  {
    return !(lhs == rhs);
  }

  template<typename Q2>
  [[nodiscard]] friend constexpr auto operator<(const vector& lhs, const vector<Q2, D>& rhs)
    requires requires { lhs.magnitude() < rhs.magnitude(); }
  {
    return lhs.magnitude() < rhs.magnitude();
  }

  template<typename Q2>
  [[nodiscard]] friend constexpr auto operator>(const vector& lhs, const vector<Q2, D>& rhs)
    requires requires { lhs.magnitude() > rhs.magnitude(); }
  {
    return rhs < lhs;
  }

  template<typename Q2>
  [[nodiscard]] friend constexpr auto operator<=(const vector& lhs, const vector<Q2, D>& rhs)
    requires requires { lhs.magnitude() <= rhs.magnitude(); }
  {
    return !(rhs < lhs);
  }

  template<typename Q2>
  [[nodiscard]] friend constexpr auto operator>=(const vector& lhs, const vector<Q2, D>& rhs)
    requires requires { lhs.magnitude() >= rhs.magnitude(); }
  {
    return !(lhs < rhs);
  }

#endif

  // template<class CharT, class Traits>
    // requires Quantity<Q> // TODO gated by gcc-9 (fixed in gcc-10)
  template<class CharT, class Traits, typename QQ = Q>
    requires Quantity<QQ>
  friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const vector& v)
  {
    return os << v.magnitude();
  }

private:
  Q magnitude_{};
};

template<typename Q1, typename Q2, direction D>
[[nodiscard]] constexpr auto operator+(const vector<Q1, D>& lhs, const vector<Q2, D>& rhs)
  requires requires { lhs.magnitude() + rhs.magnitude(); }
{
  using ret_type = decltype(lhs.magnitude() + rhs.magnitude());
  return vector<ret_type, D>(lhs.magnitude() + rhs.magnitude());
}

template<typename Q1, typename Q2, direction D>
[[nodiscard]] constexpr auto operator-(const vector<Q1, D>& lhs, const vector<Q2, D>& rhs)
  requires requires { lhs.magnitude() - rhs.magnitude(); }
{
  using ret_type = decltype(lhs.magnitude() - rhs.magnitude());
  return vector<ret_type, D>(lhs.magnitude() - rhs.magnitude());
}

// template<typename Q1, typename Q2, direction D>
// constexpr AUTO operator*(const vector<Q1, D>& lhs, const vector<Q2, D>& rhs)
//   requires requires { lhs.magnitude() * rhs.magnitude(); }
// {
//   using ret_type = decltype(lhs.magnitude() * rhs.magnitude());
//   return vector<ret_type, D>(lhs.magnitude() * rhs.magnitude());
// }

template<typename Q1, typename Q2, direction D1, direction D2>
[[nodiscard]] constexpr double operator/(const vector<Q1, D1>& lhs, const vector<Q2, D2>& rhs)
  requires equivalent_dim<typename units::get_dimension<Q1>::type, typename units::get_dimension<Q2>::type> &&
           requires { lhs.magnitude() / rhs.magnitude(); }
{
  return lhs.magnitude() / rhs.magnitude();
}

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

// concepts do not scale :-(
// template<typename T>
// concept QPLength = QuantityPoint<T> && Length<typename T::quantity_type>;

using distance = vector<si::length<si::kilometre>, direction::horizontal>;
using height = vector<si::length<si::metre>, direction::vertical>;
using altitude = vector<quantity_point<si::dim_length, si::metre>, direction::vertical>;

using velocity = vector<si::speed<si::kilometre_per_hour>, direction::horizontal>;
using rate_of_climb = vector<si::speed<si::metre_per_second>, direction::vertical>;

using time_point = quantity_point<si::dim_time, si::second>;
using duration = si::time<si::second>;

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

constexpr double glide_ratio(const glider::polar_point& polar)
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

struct position {
  time_point timestamp;
  distance dist;
  altitude alt;
};

constexpr altitude terrain_level_alt(const task& t, distance pos)
{
  const height alt_diff = t.finish.alt - t.start.alt;
  return t.start.alt + height(alt_diff.magnitude() * (pos / t.dist));
}

constexpr height agl(altitude glider_alt, altitude terrain_level)
{
  return glider_alt - terrain_level;
}

position takeoff(const task& t)
{
  const position new_pos{{}, {}, t.start.alt};
  return new_pos;
}

void print(std::string_view phase_name, const position& pos, const position& new_pos)
{
  fmt::print("| {:<12} | {:>9%.1Q %q} (Total: {:>9%.1Q %q}) | {:>8%.1Q %q} (Total: {:>8%.1Q %q}) | {:>7%.0Q %q} ({:>6%.0Q %q}) |\n",
    phase_name,
    quantity_cast<si::minute>(new_pos.timestamp - pos.timestamp), quantity_cast<si::minute>(new_pos.timestamp.relative()),
    new_pos.dist - pos.dist, new_pos.dist,
    new_pos.alt - pos.alt, new_pos.alt);
}

position tow(const position& pos, const aircraft_tow& at)
{
  const duration d = at.height_agl.magnitude() / at.performance.magnitude();
  const position new_pos{pos.timestamp + d, pos.dist, pos.alt + at.height_agl};

  print("Tow", pos, new_pos);
  return new_pos;
}

position circle(const position& pos, const glider& g, const weather& w, const task& t, height& height_to_gain)
{
  const height h_agl = agl(pos.alt, terrain_level_alt(t, pos.dist));
  const height circle_height = std::min(w.cloud_base - h_agl, height_to_gain);
  const rate_of_climb circling_rate = w.thermal_strength + g.polar[0].climb;
  const duration d = circle_height.magnitude() / circling_rate.magnitude();
  const position new_pos{pos.timestamp + d, pos.dist, pos.alt + circle_height};

  height_to_gain = height_to_gain - circle_height; // TODO -=

  print("Circle", pos, new_pos);
  return new_pos;
}

// Returns `x` of the intersection of a glide line and a terrain line.
// y = -x / glide_ratio + pos.alt;
// y = (finish_alt - ground_alt) / dist_to_finish * x + ground_alt + min_agl_height;
constexpr distance glide_distance(const position& pos, const glider& g, const task& t, const safety& s, altitude ground_alt)
{
  const auto dist_to_finish = t.dist - pos.dist;
  return distance((ground_alt + s.min_agl_height - pos.alt).magnitude() / ((ground_alt - t.finish.alt) / dist_to_finish - 1 / glide_ratio(g.polar[0])));
}

position glide(const position& pos, const glider& g, const task& t, const safety& s)
{
  const auto ground_alt = terrain_level_alt(t, pos.dist);
  const auto dist = glide_distance(pos, g, t, s, ground_alt);
  const auto alt = ground_alt + s.min_agl_height;
  const auto dist3d = sqrt(pow<2>(dist.magnitude()) + pow<2>((pos.alt - alt).magnitude()));
  const duration d = dist3d / g.polar[0].v.magnitude();
  const position new_pos{pos.timestamp + d, pos.dist + dist, terrain_level_alt(t, pos.dist + dist) + s.min_agl_height};

  print("Glide", pos, new_pos);
  return new_pos;
}

position final_glide(const position& pos, const glider& g, const task& t)
{
  const auto dist = t.dist - pos.dist;
  const auto dist3d = sqrt(pow<2>(dist.magnitude()) + pow<2>((pos.alt - t.finish.alt).magnitude()));
  const duration d = dist3d / g.polar[0].v.magnitude();
  const position new_pos{pos.timestamp + d, pos.dist + dist, t.finish.alt};

  print("Final Glide", pos, new_pos);
  return new_pos;
}

void estimate(const glider& g, const weather& w, const task& t, const safety& s, const aircraft_tow& at)
{
  // ready to takeoff
  position pos = takeoff(t);

  // estimate aircraft towing
  pos = tow(pos, at);

  // estimate the altitude needed to reach the finish line from this place
  const altitude final_glide_alt = t.finish.alt + height(t.dist.magnitude() / glide_ratio(g.polar[0]));

  // how much height we still need to gain in the thermalls to reach the destination?
  height height_to_gain = final_glide_alt - pos.alt;

  do {
    // glide to the next thermall
    pos = glide(pos, g, t, s);

    // circle in a thermall to gain height
    pos = circle(pos, g, w, t, height_to_gain);
  }
  while(height_to_gain > height(0q_m));

  // final glide
  pos = final_glide(pos, g, t);
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
    waypoint{"EPPR", altitude(quantity_point(16q_ft))},
    waypoint{"EPGI", altitude(quantity_point(115q_ft))},
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
