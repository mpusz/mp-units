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

#include <mp-units/format.h>
#include <mp-units/systems/imperial/imperial.h>
#include <mp-units/systems/international/international.h>
#include <mp-units/systems/isq/mechanics.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si/unit_symbols.h>
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
  return MP_UNITS_STD_FMT::format("{:22}", q) + (MP_UNITS_STD_FMT::format(",{:20}", value_cast<Us>(q)) + ...);
}

// Print the ship details in the units as defined in the Ship struct, in other si::imperial units, and in SI
void print_details(std::string_view description, const Ship& ship)
{
  const auto waterDensity = 62.4 * isq::density[lb / cubic(ft)];
  std::cout << MP_UNITS_STD_FMT::format("{}\n", description);
  std::cout << MP_UNITS_STD_FMT::format("{:20} : {}\n", "length", fmt_line<yd, m>(ship.length))
            << MP_UNITS_STD_FMT::format("{:20} : {}\n", "draft", fmt_line<yd, m>(ship.draft))
            << MP_UNITS_STD_FMT::format("{:20} : {}\n", "beam", fmt_line<yd, m>(ship.beam))
            << MP_UNITS_STD_FMT::format("{:20} : {}\n", "mass", fmt_line<imperial::long_ton, t>(ship.mass))
            << MP_UNITS_STD_FMT::format("{:20} : {}\n", "speed", fmt_line<kt, km / h>(ship.speed))
            << MP_UNITS_STD_FMT::format("{:20} : {}\n", "power", fmt_line<hp, kW>(ship.power))
            << MP_UNITS_STD_FMT::format("{:20} : {}\n", "main guns", fmt_line<in, mm>(ship.mainGuns))
            << MP_UNITS_STD_FMT::format("{:20} : {}\n", "fire shells weighing",
                                        fmt_line<imperial::long_ton, kg>(ship.shellMass))
            << MP_UNITS_STD_FMT::format("{:20} : {}\n", "fire shells at", fmt_line<mph, km / h>(ship.shellSpeed))
            << MP_UNITS_STD_FMT::format("{:20} : {}\n", "volume underwater", fmt_line<m3, l>(ship.mass / waterDensity));
}

int main()
{
  using mp_units::international::unit_symbols::ft;  // collides with si::femto<si::tonne>

  // KMS Bismark, using the units the Germans would use, taken from Wiki
  auto bismark = Ship{.length{251. * m},
                      .draft{9.3 * m},
                      .beam{36 * m},
                      .speed{56 * km / h},
                      .mass{50'300 * t},
                      .mainGuns{380 * mm},
                      .shellMass{800 * kg},
                      .shellSpeed{820. * m / s},
                      .power{110.45 * kW}};

  // USS Iowa, using units from the foot-pound-second system
  auto iowa = Ship{.length{860. * ft},
                   .draft{37. * ft + 2. * in},
                   .beam{108. * ft + 2. * in},
                   .speed{33 * kt},
                   .mass{57'540 * imperial::long_ton},
                   .mainGuns{16 * in},
                   .shellMass{2700 * lb},
                   .shellSpeed{2690. * ft / s},
                   .power{212'000 * hp}};

  // HMS King George V, using units from the foot-pound-second system
  auto kgv = Ship{.length{745.1 * ft},
                  .draft{33. * ft + 7.5 * in},
                  .beam{103.2 * ft + 2.5 * in},
                  .speed{28.3 * kt},
                  .mass{42'245 * imperial::long_ton},
                  .mainGuns{14 * in},
                  .shellMass{1590 * lb},
                  .shellSpeed{2483. * ft / s},
                  .power{110'000 * hp}};

  print_details("KMS Bismark, defined in appropriate units from the SI system", bismark);
  std::cout << "\n\n";
  print_details("USS Iowa, defined in appropriate units foot-pound-second system", iowa);
  std::cout << "\n\n";
  print_details("HMS King George V, defined in appropriate units foot-pound-second system", kgv);
}
