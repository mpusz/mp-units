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
#include <units/generic/dimensionless.h>
#include <units/isq/mechanics.h>
#include <units/isq/space_and_time.h>
#include <units/si/constants.h>
#include <units/si/unit_symbols.h>
#include <units/si/units.h>
#include <cassert>
#include <iostream>
#include <utility>

template<class T>
  requires units::is_scalar<T>
inline constexpr bool units::is_vector<T> = true;

namespace {

using namespace units;
using namespace units::si::unit_symbols;

inline constexpr auto g = si::standard_gravity(1);
inline constexpr auto air_density = isq::mass_density(1.225, kg / m3);

class Box {
  quantity<isq::area[m2]> base_;
  quantity<isq::height[m]> height_;
  quantity<isq::mass_density[kg / m3]> density_ = air_density;
public:
  constexpr Box(const quantity<isq::length[m]>& length, const quantity<isq::length[m]>& width,
                quantity<isq::length[m]> height) :
      base_(length * width), height_(std::move(height))
  {
  }

  [[nodiscard]] constexpr quantity_of<isq::weight> auto filled_weight() const
  {
    const weak_quantity_of<isq::volume> auto volume = base_ * height_;
    const weak_quantity_of<isq::mass> auto mass = density_ * volume;
    return quantity_cast<isq::weight>(mass * g);
  }

  [[nodiscard]] constexpr quantity<isq::height[m]> fill_level(const quantity<isq::mass[kg]>& measured_mass) const
  {
    return height_ * measured_mass * g / filled_weight();
  }

  [[nodiscard]] constexpr quantity<isq::volume[m3]> spare_capacity(const quantity<isq::mass[kg]>& measured_mass) const
  {
    return (height_ - fill_level(measured_mass)) * base_;
  }

  constexpr void set_contents_density(const quantity<isq::mass_density[kg / m3]>& density_in)
  {
    assert(density_in > air_density);
    density_ = density_in;
  }
};

}  // namespace


int main()
{
  using namespace units::si;

  constexpr auto mm = isq::length[unit_symbols::mm];  // helper reference object

  const auto height = mm(200.0)[metre];
  auto box = Box(mm(1000.0), mm(500.0), height);
  box.set_contents_density(isq::mass_density(1000.0, kg / m3));

  const auto fill_time = isq::time(200.0, s);      // time since starting fill
  const auto measured_mass = isq::mass(20.0, kg);  // measured mass at fill_time
  const auto fill_level = box.fill_level(measured_mass);
  const auto spare_capacity = box.spare_capacity(measured_mass);
  const auto filled_weight = box.filled_weight();

  const weak_quantity_of<isq::mass_change_rate> auto input_flow_rate = measured_mass / fill_time;
  const weak_quantity_of<isq::speed> auto float_rise_rate = fill_level / fill_time;
  const quantity_of<isq::time> auto fill_time_left = (height / fill_level - 1) * fill_time;

  const auto fill_percent = (fill_level / height)[percent];

  std::cout << "mp-units box example...\n";
  std::cout << STD_FMT::format("fill height at {} = {} ({} full)\n", fill_time, fill_level, fill_percent);
  std::cout << STD_FMT::format("fill weight at {} = {} ({})\n", fill_time, filled_weight, filled_weight[N]);
  std::cout << STD_FMT::format("spare capacity at {} = {}\n", fill_time, spare_capacity);
  std::cout << STD_FMT::format("input flow rate after {} = {}\n", fill_time, input_flow_rate);
  std::cout << STD_FMT::format("float rise rate = {}\n", float_rise_rate);
  std::cout << STD_FMT::format("box full E.T.A. at current flow rate = {}\n", fill_time_left);
}
