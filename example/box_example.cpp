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

#include <mp_units/format.h>
#include <mp_units/systems/isq/mechanics.h>
#include <mp_units/systems/isq/space_and_time.h>
#include <mp_units/systems/si/constants.h>
#include <mp_units/systems/si/unit_symbols.h>
#include <mp_units/systems/si/units.h>
#include <cassert>
#include <iostream>
#include <utility>

template<class T>
  requires mp_units::is_scalar<T>
inline constexpr bool mp_units::is_vector<T> = true;

namespace {

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

inline constexpr auto g = 1 * si::standard_gravity;
inline constexpr auto air_density = isq::mass_density(1.225 * (kg / m3));

class Box {
  quantity<isq::area[m2]> base_;
  quantity<isq::height[m]> height_;
  quantity<isq::mass_density[kg / m3]> density_ = air_density;
public:
  constexpr Box(const quantity<isq::length[m]>& length, const quantity<isq::width[m]>& width,
                quantity<isq::height[m]> height) :
      base_(length * width), height_(std::move(height))
  {
  }

  [[nodiscard]] constexpr QuantityOf<isq::weight> auto filled_weight() const
  {
    const auto volume = isq::volume(base_ * height_);
    const QuantityOf<isq::mass> auto mass = density_ * volume;
    return isq::weight(mass * g);
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
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  const quantity<isq::height[m]> height = 200. * mm;
  auto box = Box(isq::length(1000. * mm), isq::width(500. * mm), height);
  box.set_contents_density(1000.0 * isq::mass_density[kg / m3]);

  const auto fill_time = 200. * s;       // time since starting fill
  const auto measured_mass = 20.0 * kg;  // measured mass at fill_time
  const auto fill_level = box.fill_level(measured_mass);
  const auto spare_capacity = box.spare_capacity(measured_mass);
  const auto filled_weight = box.filled_weight();

  const QuantityOf<isq::mass_change_rate> auto input_flow_rate = measured_mass / fill_time;
  const QuantityOf<isq::speed> auto float_rise_rate = fill_level / fill_time;
  const QuantityOf<isq::time> auto fill_time_left = (height / fill_level - 1) * fill_time;

  const auto fill_percent = (fill_level / height)[percent];

  std::cout << "mp-units box example...\n";
  std::cout << STD_FMT::format("fill height at {} = {} ({} full)\n", fill_time, fill_level, fill_percent);
  std::cout << STD_FMT::format("fill weight at {} = {} ({})\n", fill_time, filled_weight, filled_weight[N]);
  std::cout << STD_FMT::format("spare capacity at {} = {}\n", fill_time, spare_capacity);
  std::cout << STD_FMT::format("input flow rate after {} = {}\n", fill_time, input_flow_rate);
  std::cout << STD_FMT::format("float rise rate = {}\n", float_rise_rate);
  std::cout << STD_FMT::format("box full E.T.A. at current flow rate = {}\n", fill_time_left);
}
