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
#include <units/physical/fps/density.h>
#include <units/physical/fps/length.h>
#include <units/physical/fps/mass.h>
#include <units/physical/fps/power.h>
#include <units/physical/fps/speed.h>
#include <units/physical/fps/volume.h>
#include <units/physical/si/length.h>
#include <units/physical/si/mass.h>
#include <units/physical/si/power.h>
#include <units/physical/si/speed.h>
#include <units/physical/si/volume.h>

#include <units/concepts.h>
#include <units/quantity.h>

#include <iostream>

using namespace units::physical;


// Some basic specs for the warship   
struct Ship {
  fps::length<fps::foot> length;
  fps::length<fps::foot> draft;
  fps::length<fps::foot> beam;

  fps::speed<fps::foot_per_second> speed;
  fps::mass<fps::pound> mass;

  fps::length<fps::inch> mainGuns; 
  fps::mass<fps::pound> shellMass;
  fps::speed<fps::foot_per_second> shellSpeed;
  fps::power<fps::foot_poundal_per_second> power;
};

// Print 'a' in its current units and print its value cast to the units in each of Args
template<class ...Args, units::Quantity Q>
auto fmt_line(const Q a)
{
  return fmt::format("{:22}", a) + (fmt::format(",{:20}", units::quantity_cast<Args>(a)) + ...);
}

// Print the ship details in the units as defined in the Ship struct, in other imperial units, and in SI
void print_details(std::string_view description, const Ship& ship)
{
  using namespace units::physical::fps::literals;
  const auto waterDensity = 62.4q_lb_per_ft3;
  std::cout << fmt::format("{}\n", description);
  std::cout << fmt::format("{:20} : {}\n", "length",    fmt_line<fps::length<fps::yard>, si::length<si::metre>>(ship.length))
            << fmt::format("{:20} : {}\n", "draft",     fmt_line<fps::length<fps::yard>, si::length<si::metre>>(ship.draft))
            << fmt::format("{:20} : {}\n", "beam",      fmt_line<fps::length<fps::yard>, si::length<si::metre>>(ship.beam))
            << fmt::format("{:20} : {}\n", "mass",      fmt_line<fps::mass<fps::long_ton>, si::mass<si::tonne>>(ship.mass))
            << fmt::format("{:20} : {}\n", "speed",     fmt_line<fps::speed<fps::knot>, si::speed<si::kilometre_per_hour>>(ship.speed))
            << fmt::format("{:20} : {}\n", "power",     fmt_line<fps::power<fps::horse_power>, si::power<si::kilowatt>>(ship.power))
            << fmt::format("{:20} : {}\n", "main guns", fmt_line<fps::length<fps::inch>, si::length<si::millimetre>>(ship.mainGuns))
            << fmt::format("{:20} : {}\n", "fire shells weighing",fmt_line<fps::mass<fps::long_ton>, si::mass<si::kilogram>>(ship.shellMass))
            << fmt::format("{:20} : {}\n", "fire shells at",fmt_line<fps::speed<fps::mile_per_hour>, si::speed<si::kilometre_per_hour>>(ship.shellSpeed))
            << fmt::format("{:20} : {}\n", "volume underwater", fmt_line<si::volume<si::cubic_metre>, si::volume<si::litre>>(ship.mass / waterDensity));
}

int main()
{
  using namespace units::physical::si::literals;
  using namespace units::physical::fps::literals;
  
  // KMS Bismark, using the units the Germans would use, taken from Wiki
  auto bismark = Ship{.length{251.q_m}, .draft{9.3q_m}, .beam{36q_m}, .speed{56q_km_per_h}, .mass{50'300q_t}, .mainGuns{380q_mm}, .shellMass{800q_kg}, .shellSpeed{820.q_m_per_s}, .power{110.45q_kW}};

  // USS Iowa, using units from the foot-pound-second system
  auto iowa = Ship{.length{860.q_ft}, .draft{37.q_ft + 2.q_in}, .beam{108.q_ft + 2.q_in}, .speed{33q_knot}, .mass{57'540q_lton}, .mainGuns{16q_in}, .shellMass{2700q_lb}, .shellSpeed{2690.q_ft_per_s}, .power{212'000q_hp}};

  // HMS King George V, using units from the foot-pound-second system
  auto kgv = Ship{.length{745.1q_ft}, .draft{33.q_ft + 7.5q_in}, .beam{103.2q_ft + 2.5q_in}, .speed{28.3q_knot}, .mass{42'245q_lton}, .mainGuns{14q_in}, .shellMass{1'590q_lb}, .shellSpeed{2483.q_ft_per_s}, .power{110'000q_hp}};

  print_details("KMS Bismark, defined in appropriate units from the SI system", bismark);
  std::cout << "\n\n";
  print_details("USS Iowa, defined in appropriate units foot-pound-second system", iowa);
  std::cout << "\n\n";
  print_details("HMS King George V, defined in appropriate units foot-pound-second system", kgv);
}
