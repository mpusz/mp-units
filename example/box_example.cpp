#include <units/physical/si/si.h>
#include <units/format.h>
#include <cassert>

namespace {

using namespace units::physical;
using namespace units::physical::si::literals;

using m = si::metre;
using kg = si::kilogram;
using N = si::newton;
using m3 = si::cubic_metre;
using kgpm3 = si::kilogram_per_metre_cub;

inline constexpr auto g = si::si2019::standard_gravity<>;

}  // namespace

struct Box {
  static constexpr auto air_density = 1.225_q_kg_per_m3;

  si::length<m> length;
  si::length<m> width;
  si::length<m> height;

  struct contents {
    si::density<kgpm3> density = air_density;
  } contents;

  constexpr Box(const si::length<m>& l, const si::length<m>& w, const si::length<m>& h) : length{l}, width{w}, height{h} {}

  constexpr si::force<N> filled_weight() const
  {
    const si::volume<m3> volume = length * width * height;
    const si::mass<kg> mass = contents.density * volume;
    return mass * g;
  }

  constexpr si::length<m> fill_level(const si::mass<kg>& measured_mass) const
  {
    return height * (measured_mass * g) / filled_weight();
  }

  constexpr si::volume<m3> spare_capacity(const si::mass<kg>& measured_mass) const
  {
    return (height - fill_level(measured_mass)) * width * length;
  }

  constexpr void set_contents_density(const si::density<kgpm3>& density_in)
  {
    assert(density_in > air_density);
    contents.density = density_in;
  }
};

#include <iostream>

int main()
{
  auto box = Box(1000.0_q_mm, 500.0_q_mm, 200.0_q_mm);
  box.set_contents_density(1000.0_q_kg_per_m3);

  const auto fill_time = 200.0_q_s;      // time since starting fill
  const auto measured_mass = 20.0_q_kg;  // measured mass at fill_time

  std::cout << "mp-units box example...\n";
  std::cout << "fill height at " << fill_time << " = " << box.fill_level(measured_mass) << " ("
            << (box.fill_level(measured_mass) / box.height) * 100 << "% full)\n";
  std::cout << "spare_capacity at " << fill_time << " = " << box.spare_capacity(measured_mass) << '\n';
  std::cout << "input flow rate after " << fill_time << " = " << measured_mass / fill_time << '\n';
  std::cout << "float rise rate = " << box.fill_level(measured_mass) / fill_time << '\n';
  const auto fill_time_left = (box.height / box.fill_level(measured_mass) - 1) * fill_time;
  std::cout << "box full E.T.A. at current flow rate = " << fill_time_left << '\n';
}
