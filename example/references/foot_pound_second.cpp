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
#include <units/isq/si/fps/density.h> // IWYU pragma: keep
#include <units/isq/si/fps/length.h>
#include <units/isq/si/fps/mass.h>
#include <units/isq/si/fps/power.h>
#include <units/isq/si/fps/speed.h>
#include <units/isq/si/fps/time.h>
#include <units/isq/si/fps/volume.h>
#include <units/isq/si/length.h>
#include <units/isq/si/mass.h>
#include <units/isq/si/power.h>
#include <units/isq/si/speed.h>
#include <units/isq/si/time.h>
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
template<class ...Args, units::Quantity Q>
auto fmt_line(const Q a)
{
  return fmt::format("{:22}", a) + (fmt::format(",{:20}", units::quantity_cast<Args>(a)) + ...);
}

// Print the ship details in the units as defined in the Ship struct, in other si::imperial units, and in SI
void print_details(std::string_view description, const Ship& ship)
{
  using namespace units::isq::si::fps::references;
  const auto waterDensity = 62.4 * (lb / ft3);
  std::cout << fmt::format("{}\n", description);
  std::cout << fmt::format("{:20} : {}\n", "length",               fmt_line<si::fps::length<si::fps::yard>, si::length<si::metre>>(ship.length))
            << fmt::format("{:20} : {}\n", "draft",                fmt_line<si::fps::length<si::fps::yard>, si::length<si::metre>>(ship.draft))
            << fmt::format("{:20} : {}\n", "beam",                 fmt_line<si::fps::length<si::fps::yard>, si::length<si::metre>>(ship.beam))
            << fmt::format("{:20} : {}\n", "mass",                 fmt_line<si::fps::mass<si::fps::long_ton>, si::mass<si::tonne>>(ship.mass))
            << fmt::format("{:20} : {}\n", "speed",                fmt_line<si::fps::speed<si::fps::knot>, si::speed<si::kilometre_per_hour>>(ship.speed))
            << fmt::format("{:20} : {}\n", "power",                fmt_line<si::fps::power<si::fps::horse_power>, si::power<si::kilowatt>>(ship.power))
            << fmt::format("{:20} : {}\n", "main guns",            fmt_line<si::fps::length<si::fps::inch>, si::length<si::millimetre>>(ship.mainGuns))
            << fmt::format("{:20} : {}\n", "fire shells weighing", fmt_line<si::fps::mass<si::fps::long_ton>, si::mass<si::kilogram>>(ship.shellMass))
            << fmt::format("{:20} : {}\n", "fire shells at",       fmt_line<si::fps::speed<si::fps::mile_per_hour>, si::speed<si::kilometre_per_hour>>(ship.shellSpeed))
            << fmt::format("{:20} : {}\n", "volume underwater",    fmt_line<si::volume<si::cubic_metre>, si::volume<si::litre>>(ship.mass / waterDensity));
}

int main()
{
  using namespace units::isq::si::references;
  using namespace units::isq::si::fps::references;
  using units::isq::si::fps::references::ft;  // collides with si::femtotonne (alias unit of mass)
  
  // KMS Bismark, using the units the Germans would use, taken from Wiki
  auto bismark = Ship{.length{251. * m}, .draft{9.3 * m}, .beam{36 * m}, .speed{56 * (km / h)}, .mass{50'300 * t}, .mainGuns{380 * mm}, .shellMass{800 * kg}, .shellSpeed{820. * (m / s)}, .power{110.45 * kW}};

  // USS Iowa, using units from the foot-pound-second system
  auto iowa = Ship{.length{860. * ft}, .draft{37. * ft + 2. * in}, .beam{108. * ft + 2. * in}, .speed{33 * knot}, .mass{57'540 * lton}, .mainGuns{16 * in}, .shellMass{2700 * lb}, .shellSpeed{2690. * (ft / s)}, .power{212'000 * hp}};

  // HMS King George V, using units from the foot-pound-second system
  auto kgv = Ship{.length{745.1 * ft}, .draft{33. * ft + 7.5 * in}, .beam{103.2 * ft + 2.5 * in}, .speed{28.3 * knot}, .mass{42'245 * lton}, .mainGuns{14 * in}, .shellMass{1'590 * lb}, .shellSpeed{2483. * (ft / s)}, .power{110'000 * hp}};

  print_details("KMS Bismark, defined in appropriate units from the SI system", bismark);
  std::cout << "\n\n";
  print_details("USS Iowa, defined in appropriate units foot-pound-second system", iowa);
  std::cout << "\n\n";
  print_details("HMS King George V, defined in appropriate units foot-pound-second system", kgv);
}
