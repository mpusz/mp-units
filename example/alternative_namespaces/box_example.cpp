
#include "./length.h"
#include "./acceleration.h"
#include "./density.h"
#include "./force.h"
#include "./mass.h"
#include "./time.h"
#include "./volume.h"

#include <cassert>

using namespace units::experimental;

struct Box {

  static constexpr auto air_density = density::kg_per_m3<>{1.225};

  length::m<> length;
  length::m<> width;
  length::m<> height;

  constexpr Box(const length::m<>& l, const length::m<>& w, const length::m<>& h) : length{l}, width{w}, height{h} {}

  struct contents {
    density::kg_per_m3<> density = air_density;
  } contents;

  constexpr force::N<> filled_weight() const
  {
    const volume::m3<> volume = length * width * height;
    const mass::kg<> mass = contents.density * volume;
    return mass * acceleration::g<>;
  }

  constexpr length::m<> fill_level(const mass::kg<>& measured_mass) const
  {
    return height * (measured_mass * acceleration::g<>) / filled_weight();
  }

  constexpr volume::m3<> spare_capacity(const mass::kg<>& measured_mass) const
  {
    return (height - fill_level(measured_mass)) * width * length;
  }

  constexpr void set_contents_density(const density::kg_per_m3<>& density_in)
  {
    assert(density_in > air_density);
    contents.density = density_in;
  }

};

#include <iostream>

using namespace units::physical::si::literals;

int main()
{
  auto box = Box{1000.0q_mm, 500.0q_mm, 200.0q_mm};
  box.set_contents_density(1000.0q_kg_per_m3);

  auto fill_time = 200.0q_s;      // time since starting fill
  auto measured_mass = 20.0q_kg;  // measured mass at fill_time

  std::cout << "mpusz/units box example ( using experimental alternative syntax for defining quantities) ...\n";
  std::cout << "fill height at " << fill_time << " = " << box.fill_level(measured_mass) << " ("
            << (box.fill_level(measured_mass) / box.height) * 100 << "% full)\n";
  std::cout << "spare_capacity at " << fill_time << " = " << box.spare_capacity(measured_mass) << '\n';
  std::cout << "input flow rate after " << fill_time << " = " << measured_mass / fill_time << '\n';
  std::cout << "float rise rate = " << box.fill_level(measured_mass) / fill_time << '\n';
  auto fill_time_left = (box.height / box.fill_level(measured_mass) - 1) * fill_time;
  std::cout << "box full E.T.A. at current flow rate = " << fill_time_left << '\n';
}
