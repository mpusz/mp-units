#include <units/physical/si/constants.h>
#include <units/physical/si/area.h>
#include <units/physical/si/speed.h>
#include <units/physical/si/density.h>
#include <units/physical/si/force.h>
#include <units/physical/si/mass.h>
#include <units/physical/si/time.h>
#include <units/physical/si/volume.h>
#include <units/format.h>
#include <cassert>
#include <iostream>

namespace {

using namespace units::physical;

using m = si::metre;
using m2 = si::square_metre;
using m3 = si::cubic_metre;
using kg = si::kilogram;
using N = si::newton;
using kgpm3 = si::kilogram_per_metre_cub;

inline constexpr auto g = si::si2019::standard_gravity<>;
inline constexpr si::density<kgpm3> air_density(1.225);


class Box {
  si::area<m2> base_;
  si::length<m> height_;
  si::density<kgpm3> density_ = air_density;
public:
  constexpr Box(const si::length<m>& length, const si::length<m>& width, si::length<m> height) : base_(length * width), height_(std::move(height)) {}

  [[nodiscard]] constexpr si::force<N> filled_weight() const
  {
    const si::volume<m3> volume = base_ * height_;
    const si::mass<kg> mass = density_ * volume;
    return mass * g;
  }

  [[nodiscard]] constexpr si::length<m> fill_level(const si::mass<kg>& measured_mass) const
  {
    return height_ * measured_mass * g / filled_weight();
  }

  [[nodiscard]] constexpr si::volume<m3> spare_capacity(const si::mass<kg>& measured_mass) const
  {
    return (height_ - fill_level(measured_mass)) * base_;
  }

  constexpr void set_contents_density(const si::density<kgpm3>& density_in)
  {
    assert(density_in > air_density);
    density_ = density_in;
  }
};

}  // namespace


int main()
{
  using namespace units;
  using namespace si::literals;

  const si::length<m> height(200.0_q_mm);
  auto box = Box(1000.0_q_mm, 500.0_q_mm, height);
  box.set_contents_density(1000.0_q_kg_per_m3);

  const auto fill_time = 200.0_q_s;        // time since starting fill
  const auto measured_mass = 20.0_q_kg;    // measured mass at fill_time

  const Length auto fill_level = box.fill_level(measured_mass);
  const Dimensionless auto fill_percent = quantity_cast<percent>(fill_level / height);
  const Volume auto spare_capacity = box.spare_capacity(measured_mass);
  const auto input_flow_rate = measured_mass / fill_time;    // unknown dimension
  const Speed auto float_rise_rate = fill_level / fill_time;
  const Time auto fill_time_left = (height / fill_level - 1) * fill_time;

  std::cout << "mp-units box example...\n";
  std::cout << fmt::format("fill height at {} = {} ({} full)\n", fill_time, fill_level, fill_percent);
  std::cout << fmt::format("spare_capacity at {} = {}\n", fill_time, spare_capacity);
  std::cout << fmt::format("input flow rate after {} = {}\n", fill_time, input_flow_rate);
  std::cout << fmt::format("float rise rate = {}\n", float_rise_rate);
  std::cout << fmt::format("box full E.T.A. at current flow rate = {}\n", fill_time_left);
}
