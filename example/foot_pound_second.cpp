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
#include <mp_units/systems/imperial/imperial.h>
#include <mp_units/systems/international/international.h>
#include <mp_units/systems/isq/space_and_time.h>
#include <mp_units/systems/si/unit_symbols.h>
#include <iostream>
#include <string_view>

using namespace mp_units;
using namespace mp_units::international::unit_symbols;
using namespace mp_units::si::unit_symbols;


// Some basic specs for the warship
struct Ship {
  quantity<isq::length[ft]> length;
  quantity<isq::length[ft]> draft;
  quantity<isq::length[ft]> beam;

  quantity<isq::speed[ft / s]> speed;
  quantity<isq::mass[lb]> mass;

  quantity<isq::length[in]> mainGuns;
  quantity<isq::mass[lb]> shellMass;
  quantity<isq::speed[ft / s]> shellSpeed;
  quantity<isq::power[ft * pdl / s]> power;
};

// Print 'q' in its current units and print its value cast to the units in each of Us
template<Unit auto... Us, Quantity Q>
auto fmt_line(const Q& q)
{
  return STD_FMT::format("{:22}", q) + (STD_FMT::format(",{:20}", quantity_cast<Us>(q)) + ...);
}

// Print the ship details in the units as defined in the Ship struct, in other si::imperial units, and in SI
void print_details(std::string_view description, const Ship& ship)
{
  const auto waterDensity = isq::density[lb / cubic<ft>](62.4);
  std::cout << STD_FMT::format("{}\n", description);
  std::cout << STD_FMT::format("{:20} : {}\n", "length", fmt_line<yd, m>(ship.length))
            << STD_FMT::format("{:20} : {}\n", "draft", fmt_line<yd, m>(ship.draft))
            << STD_FMT::format("{:20} : {}\n", "beam", fmt_line<yd, m>(ship.beam))
            << STD_FMT::format("{:20} : {}\n", "mass", fmt_line<imperial::long_ton, t>(ship.mass))
            << STD_FMT::format("{:20} : {}\n", "speed", fmt_line<kt, km / h>(ship.speed))
            << STD_FMT::format("{:20} : {}\n", "power", fmt_line<hp, kW>(ship.power))
            << STD_FMT::format("{:20} : {}\n", "main guns", fmt_line<in, mm>(ship.mainGuns))
            << STD_FMT::format("{:20} : {}\n", "fire shells weighing", fmt_line<imperial::long_ton, kg>(ship.shellMass))
            << STD_FMT::format("{:20} : {}\n", "fire shells at", fmt_line<mph, km / h>(ship.shellSpeed))
            << STD_FMT::format("{:20} : {}\n", "volume underwater", fmt_line<m3, l>(ship.mass / waterDensity));
}

int main()
{
  using mp_units::international::unit_symbols::ft;  // collides with si::femto<si::tonne>

  // KMS Bismark, using the units the Germans would use, taken from Wiki
  auto bismark = Ship{.length{isq::length[m](251.)},
                      .draft{isq::length[m](9.3)},
                      .beam{isq::length[m](36)},
                      .speed{isq::speed[km / h](56)},
                      .mass{isq::mass[t](50'300)},
                      .mainGuns{isq::length[mm](380)},
                      .shellMass{isq::mass[kg](800)},
                      .shellSpeed{isq::speed[m / s](820.)},
                      .power{isq::power[kW](110.45)}};

  // USS Iowa, using units from the foot-pound-second system
  auto iowa = Ship{.length{isq::length[ft](860.)},
                   .draft{isq::length[ft](37.) + isq::length[in](2.)},
                   .beam{isq::length[ft](108.) + isq::length[in](2.)},
                   .speed{isq::speed[kt](33)},
                   .mass{isq::mass[imperial::long_ton](57'540)},
                   .mainGuns{isq::length[in](16)},
                   .shellMass{isq::mass[lb](2700)},
                   .shellSpeed{isq::speed[ft / s](2690.)},
                   .power{isq::power[hp](212'000)}};

  // HMS King George V, using units from the foot-pound-second system
  auto kgv = Ship{.length{isq::length[ft](745.1)},
                  .draft{isq::length[ft](33.) + isq::length[in](7.5)},
                  .beam{isq::length[ft](103.2) + isq::length[in](2.5)},
                  .speed{isq::speed[kt](28.3)},
                  .mass{isq::mass[imperial::long_ton](42'245)},
                  .mainGuns{isq::length[in](14)},
                  .shellMass{isq::mass[lb](1'590)},
                  .shellSpeed{isq::speed[ft / s](2483.)},
                  .power{isq::power[hp](110'000)}};

  print_details("KMS Bismark, defined in appropriate units from the SI system", bismark);
  std::cout << "\n\n";
  print_details("USS Iowa, defined in appropriate units foot-pound-second system", iowa);
  std::cout << "\n\n";
  print_details("HMS King George V, defined in appropriate units foot-pound-second system", kgv);
}
