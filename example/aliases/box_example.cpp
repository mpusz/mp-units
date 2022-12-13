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
#include <units/isq/si/amount_of_substance.h>
#include <units/isq/si/area.h>
#include <units/isq/si/constants.h>
#include <units/isq/si/density.h>
#include <units/isq/si/force.h>
#include <units/isq/si/length.h>
#include <units/isq/si/mass.h>
#include <units/isq/si/speed.h>  // IWYU pragma: keep
#include <units/isq/si/time.h>
#include <units/isq/si/volume.h>
#include <cassert>
#include <iostream>
#include <utility>

namespace {

using namespace units::aliases::isq::si;

inline constexpr auto g = units::isq::si::standard_gravity<>;  // NOLINT(readability-identifier-length)
inline constexpr auto air_density = kg_per_m3<>(1.225);

class Box {
  area::m2<> base_;
  length::m<> height_;
  density::kg_per_m3<> density_ = air_density;
public:
  constexpr Box(const length::m<>& length, const length::m<>& width, length::m<> height) :
      base_(length * width), height_(std::move(height))
  {
  }

  [[nodiscard]] constexpr force::N<> filled_weight() const
  {
    const volume::m3<> volume = base_ * height_;
    const mass::kg<> mass = density_ * volume;
    return mass * g;
  }

  [[nodiscard]] constexpr length::m<> fill_level(const mass::kg<>& measured_mass) const
  {
    return height_ * measured_mass * g / filled_weight();
  }

  [[nodiscard]] constexpr volume::m3<> spare_capacity(const mass::kg<>& measured_mass) const
  {
    return (height_ - fill_level(measured_mass)) * base_;
  }

  constexpr void set_contents_density(const density::kg_per_m3<>& density_in)
  {
    assert(density_in > air_density);
    density_ = density_in;
  }
};

}  // namespace


int main()
{
  using namespace units;
  using namespace units::isq;

  const length::m<> height(mm<>(200.0));
  auto box = Box(mm<>(1000.0), mm<>(500.0), height);
  box.set_contents_density(kg_per_m3<>(1000.0));

  const auto fill_time = s<>(200.0);      // time since starting fill
  const auto measured_mass = kg<>(20.0);  // measured mass at fill_time

  const Length auto fill_level = box.fill_level(measured_mass);
  const Dimensionless auto fill_percent = quantity_cast<percent>(fill_level / height);
  const Volume auto spare_capacity = box.spare_capacity(measured_mass);
  const auto input_flow_rate = measured_mass / fill_time;  // unknown dimension
  const Speed auto float_rise_rate = fill_level / fill_time;
  const Time auto fill_time_left = (height / fill_level - 1) * fill_time;

  std::cout << "mp-units box example...\n";
  std::cout << STD_FMT::format("fill height at {} = {} ({} full)\n", fill_time, fill_level, fill_percent);
  std::cout << STD_FMT::format("spare_capacity at {} = {}\n", fill_time, spare_capacity);
  std::cout << STD_FMT::format("input flow rate after {} = {}\n", fill_time, input_flow_rate);
  std::cout << STD_FMT::format("float rise rate = {}\n", float_rise_rate);
  std::cout << STD_FMT::format("box full E.T.A. at current flow rate = {}\n", fill_time_left);
}
