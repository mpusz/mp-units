#include <units/physical/si/acceleration.h>
#include <units/physical/si/density.h>
#include <units/physical/si/force.h>
#include <units/physical/si/length.h>
#include <units/physical/si/mass.h>
#include <units/physical/si/time.h>
#include <units/physical/si/volume.h>
#include <cassert>

namespace {

namespace length {

template<typename Rep = double>
using m = units::si::length<units::si::metre, Rep>;

template<typename Rep = double>
using mm = units::si::length<units::si::millimetre, Rep>;

}  // namespace length

namespace acceleration {

template<typename Rep = double>
using mps2 = units::si::acceleration<units::si::metre_per_second_sq, Rep>;

template<typename Rep = double>
constexpr mps2<> g{static_cast<Rep>(9.80665)};

}  // namespace acceleration

namespace force {

template<typename Rep = double>
using N = units::si::force<units::si::newton, Rep>;

}

namespace mass {

template<typename Rep = double>
using kg = units::si::mass<units::si::kilogram, Rep>;

}

namespace density {

template<typename Rep = double>
using kgpm3 = units::si::density<units::si::kilogram_per_metre_cub, Rep>;

}

namespace volume {

template<typename Rep = double>
using m3 = units::si::volume<units::si::cubic_metre, Rep>;

}
}  // namespace

struct Box {
  static constexpr density::kgpm3<> air_density{1.225};

  length::m<> length;
  length::m<> width;
  length::m<> height;

  struct contents {
    density::kgpm3<> density = air_density;
  } contents;

  Box(const length::m<>& l, const length::m<>& w, const length::m<>& h) : length{l}, width{w}, height{h} {}

  force::N<> filled_weight() const
  {
    const volume::m3<> volume = length * width * height;
    const mass::kg<> mass = contents.density * volume;
    return mass * acceleration::g<>;
  }

  length::m<> fill_level(const mass::kg<>& measured_mass) const
  {
    return height * (measured_mass * acceleration::g<>) / filled_weight();
  }

  volume::m3<> spare_capacity(const mass::kg<>& measured_mass) const
  {
    return (height - fill_level(measured_mass)) * width * length;
  }

  void set_contents_density(const density::kgpm3<>& density_in)
  {
    assert(density_in > air_density);
    contents.density = density_in;
  }

};

#include <iostream>

using namespace units::si::literals;
int main()
{
  auto box = Box{1000.0q_mm, 500.0q_mm, 200.0q_mm};
  box.set_contents_density(1000.0q_kg_per_m3);

  auto fill_time = 200.0q_s;      // time since starting fill
  auto measured_mass = 20.0q_kg;  // measured mass at fill_time

  std::cout << "mpusz/units box example...\n";
  std::cout << "fill height at " << fill_time << " = " << box.fill_level(measured_mass) << " ("
            << (box.fill_level(measured_mass) / box.height) * 100 << "% full)\n";
  std::cout << "spare_capacity at " << fill_time << " = " << box.spare_capacity(measured_mass) << '\n';
  std::cout << "input flow rate after " << fill_time << " = " << measured_mass / fill_time << '\n';
  std::cout << "float rise rate = " << box.fill_level(measured_mass) / fill_time << '\n';
  auto fill_time_left = (box.height / box.fill_level(measured_mass) - 1) * fill_time;
  std::cout << "box full E.T.A. at current flow rate = " << fill_time_left << '\n';
}
