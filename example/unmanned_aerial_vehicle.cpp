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

#include <mp_units/iostream.h>
#include <mp_units/quantity_point.h>
#include <mp_units/systems/international/international.h>
#include <mp_units/systems/isq/space_and_time.h>
#include <mp_units/systems/si/unit_symbols.h>
#include <iostream>

using namespace mp_units;

// clang-format off
inline constexpr struct mean_sea_level : absolute_point_origin<isq::altitude> {} mean_sea_level;
inline constexpr struct height_above_launch : absolute_point_origin<isq::altitude> {} height_above_launch;
// clang-format on

using msl_altitude = quantity_point<isq::altitude[si::metre], mean_sea_level>;
using hal_altitude = quantity_point<isq::altitude[si::metre], height_above_launch>;

static_assert(!std::equality_comparable_with<msl_altitude, hal_altitude>);

class unmanned_aerial_vehicle {
  msl_altitude current_ = 0 * si::metre;
  msl_altitude launch_ = current_;
public:
  void take_off(msl_altitude alt) { launch_ = alt; }
  msl_altitude take_off() const { return launch_; }

  void current(msl_altitude alt) { current_ = alt; }
  msl_altitude current() const { return current_; }

  hal_altitude hal() const { return current_ - launch_; }
};


int main()
{
  using namespace mp_units::si::unit_symbols;
  using namespace mp_units::international::unit_symbols;

  unmanned_aerial_vehicle uav;
  uav.take_off(6'000 * ft);
  uav.current(10'000 * ft);
  std::cout << "hal = " << uav.hal().relative() << "\n";

  msl_altitude ground_level = 123 * m;
  std::cout << "agl = " << uav.current() - ground_level << "\n";
}
