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
#include <units/isq/si/fps/density.h>
#include <units/isq/si/fps/length.h>
#include <units/isq/si/fps/mass.h>
#include <units/isq/si/fps/power.h>
#include <units/isq/si/fps/speed.h>
#include <units/isq/si/international/speed.h>
#include <units/isq/si/length.h>
#include <units/isq/si/mass.h>
#include <units/isq/si/power.h>
#include <units/isq/si/speed.h>
#include <units/isq/si/volume.h>
#include <iostream>
#include <string_view>

using namespace units::isq;


// Some basic specs for the warship
struct Ship {
  si::fps::length<si::fps::foot> length;
  si::fps::length<si::fps::foot> draft;
  si::fps::length<si::fps::foot> beam;

  si::fps::speed<si::fps::foot_per_second> speed;
  si::fps::mass<si::fps::pound> mass;

  si::fps::length<si::fps::inch> mainGuns;
  si::fps::mass<si::fps::pound> shellMass;
  si::fps::speed<si::fps::foot_per_second> shellSpeed;
  si::fps::power<si::fps::foot_poundal_per_second> power;
};

// Print 'a' in its current units and print its value cast to the units in each of Args
template<class... Args, units::Quantity Q>
auto fmt_line(const Q a)
{
  return UNITS_STD_FMT::format("{:22}", a) + (UNITS_STD_FMT::format(",{:20}", units::quantity_cast<Args>(a)) + ...);
}

// Print the ship details in the units as defined in the Ship struct, in other si::imperial units, and in SI
void print_details(std::string_view description, const Ship& ship)
{
  using namespace units::isq::si::fps::literals;
  const auto waterDensity = 62.4_q_lb_per_ft3;
  std::cout << UNITS_STD_FMT::format("{}\n", description);
  std::cout
    << UNITS_STD_FMT::format("{:20} : {}\n", "length",
                             fmt_line<si::fps::length<si::fps::yard>, si::length<si::metre>>(ship.length))
    << UNITS_STD_FMT::format("{:20} : {}\n", "draft",
                             fmt_line<si::fps::length<si::fps::yard>, si::length<si::metre>>(ship.draft))
    << UNITS_STD_FMT::format("{:20} : {}\n", "beam",
                             fmt_line<si::fps::length<si::fps::yard>, si::length<si::metre>>(ship.beam))
    << UNITS_STD_FMT::format("{:20} : {}\n", "mass",
                             fmt_line<si::fps::mass<si::fps::long_ton>, si::mass<si::tonne>>(ship.mass))
    << UNITS_STD_FMT::format(
         "{:20} : {}\n", "speed",
         fmt_line<si::speed<si::international::knot>, si::speed<si::kilometre_per_hour>>(ship.speed))
    << UNITS_STD_FMT::format("{:20} : {}\n", "power",
                             fmt_line<si::fps::power<si::fps::horse_power>, si::power<si::kilowatt>>(ship.power))
    << UNITS_STD_FMT::format("{:20} : {}\n", "main guns",
                             fmt_line<si::fps::length<si::fps::inch>, si::length<si::millimetre>>(ship.mainGuns))
    << UNITS_STD_FMT::format("{:20} : {}\n", "fire shells weighing",
                             fmt_line<si::fps::mass<si::fps::long_ton>, si::mass<si::kilogram>>(ship.shellMass))
    << UNITS_STD_FMT::format(
         "{:20} : {}\n", "fire shells at",
         fmt_line<si::fps::speed<si::fps::mile_per_hour>, si::speed<si::kilometre_per_hour>>(ship.shellSpeed))
    << UNITS_STD_FMT::format("{:20} : {}\n", "volume underwater",
                             fmt_line<si::volume<si::cubic_metre>, si::volume<si::litre>>(ship.mass / waterDensity));
}

int main()
{
  using namespace units::isq::si::literals;
  using namespace units::isq::si::fps::literals;

  // KMS Bismark, using the units the Germans would use, taken from Wiki
  auto bismark = Ship{.length{251._q_m},
                      .draft{9.3_q_m},
                      .beam{36_q_m},
                      .speed{56_q_km_per_h},
                      .mass{50'300_q_t},
                      .mainGuns{380_q_mm},
                      .shellMass{800_q_kg},
                      .shellSpeed{820._q_m_per_s},
                      .power{110.45_q_kW}};

  // USS Iowa, using units from the foot-pound-second system
  auto iowa = Ship{.length{860._q_ft},
                   .draft{37._q_ft + 2._q_in},
                   .beam{108._q_ft + 2._q_in},
                   .speed{si::speed<si::international::knot>{33}},
                   .mass{57'540_q_lton},
                   .mainGuns{16_q_in},
                   .shellMass{2700_q_lb},
                   .shellSpeed{2690._q_ft_per_s},
                   .power{212'000_q_hp}};

  // HMS King George V, using units from the foot-pound-second system
  auto kgv = Ship{.length{745.1_q_ft},
                  .draft{33._q_ft + 7.5_q_in},
                  .beam{103.2_q_ft + 2.5_q_in},
                  .speed{si::speed<si::international::knot>{28.3}},
                  .mass{42'245_q_lton},
                  .mainGuns{14_q_in},
                  .shellMass{1'590_q_lb},
                  .shellSpeed{2483._q_ft_per_s},
                  .power{110'000_q_hp}};

  print_details("KMS Bismark, defined in appropriate units from the SI system", bismark);
  std::cout << "\n\n";
  print_details("USS Iowa, defined in appropriate units foot-pound-second system", iowa);
  std::cout << "\n\n";
  print_details("HMS King George V, defined in appropriate units foot-pound-second system", kgv);
}
