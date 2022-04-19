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
#include <units/isq/si/length.h>
#include <units/isq/si/mass.h>
#include <units/isq/si/power.h>
#include <units/isq/si/speed.h>
#include <units/isq/si/volume.h>
#include <iostream>
#include <string_view>

using namespace units::aliases::isq;

// Some basic specs for the warship
struct Ship {
  si::fps::length::ft<> length;
  si::fps::length::ft<> draft;
  si::fps::length::ft<> beam;

  si::fps::speed::ft_per_s<> speed;
  si::fps::mass::lb<> mass;

  si::fps::length::in<> mainGuns;
  si::fps::mass::lb<> shellMass;
  si::fps::speed::ft_per_s<> shellSpeed;
  si::fps::power::ft_pdl_per_s<> power;
};

// Print 'a' in its current units and print its value cast to the units in each of Args
template<class... Args, units::Quantity Q>
auto fmt_line(const Q a)
{
  return STD_FMT::format("{:22}", a) + (STD_FMT::format(",{:20}", units::quantity_cast<Args>(a)) + ...);
}

// Print the ship details in the units as defined in the Ship struct, in other si::imperial units, and in SI
void print_details(std::string_view description, const Ship& ship)
{
  const auto waterDensity = si::fps::density::lb_per_ft3<>(62.4);
  std::cout << STD_FMT::format("{}\n", description);
  std::cout << STD_FMT::format("{:20} : {}\n", "length", fmt_line<si::fps::length::yd<>, si::length::m<>>(ship.length))
            << STD_FMT::format("{:20} : {}\n", "draft", fmt_line<si::fps::length::yd<>, si::length::m<>>(ship.draft))
            << STD_FMT::format("{:20} : {}\n", "beam", fmt_line<si::fps::length::yd<>, si::length::m<>>(ship.beam))
            << STD_FMT::format("{:20} : {}\n", "mass", fmt_line<si::fps::mass::lton<>, si::mass::t<>>(ship.mass))
            << STD_FMT::format("{:20} : {}\n", "speed",
                               fmt_line<si::fps::speed::kn<>, si::speed::km_per_h<>>(ship.speed))
            << STD_FMT::format("{:20} : {}\n", "power", fmt_line<si::fps::power::hp<>, si::power::kW<>>(ship.power))
            << STD_FMT::format("{:20} : {}\n", "main guns",
                               fmt_line<si::fps::length::in<>, si::length::mm<>>(ship.mainGuns))
            << STD_FMT::format("{:20} : {}\n", "fire shells weighing",
                               fmt_line<si::fps::mass::lton<>, si::mass::kg<>>(ship.shellMass))
            << STD_FMT::format("{:20} : {}\n", "fire shells at",
                               fmt_line<si::fps::speed::mph<>, si::speed::km_per_h<>>(ship.shellSpeed))
            << STD_FMT::format("{:20} : {}\n", "volume underwater",
                               fmt_line<si::volume::m3<>, si::volume::l<>>(ship.mass / waterDensity));
}

int main()
{
  using namespace units::aliases::isq::si;
  using namespace units::aliases::isq::si::fps;
  using units::aliases::isq::si::fps::length::ft;  // to disambiguate from si::femptotonne

  // KMS Bismark, using the units the Germans would use, taken from Wiki
  auto bismark = Ship{.length{m<>(251.)},
                      .draft{m<>(9.3)},
                      .beam{m<>(36)},
                      .speed{km_per_h<>(56)},
                      .mass{t<>(50'300)},
                      .mainGuns{mm<>(380)},
                      .shellMass{kg<>(800)},
                      .shellSpeed{m_per_s<>(820.)},
                      .power{kW<>(110.45)}};

  // USS Iowa, using units from the foot-pound-second system
  auto iowa = Ship{.length{ft<>(860.)},
                   .draft{ft<>(37.) + in<>(2.)},
                   .beam{ft<>(108.) + in<>(2.)},
                   .speed{kn<>(33)},
                   .mass{lton<>(57'540)},
                   .mainGuns{in<>(16)},
                   .shellMass{lb<>(2700)},
                   .shellSpeed{ft_per_s<>(2690.)},
                   .power{hp<>(212'000)}};

  // HMS King George V, using units from the foot-pound-second system
  auto kgv = Ship{.length{ft<>(745.1)},
                  .draft{ft<>(33.) + in<>(7.5)},
                  .beam{ft<>(103.2) + in<>(2.5)},
                  .speed{kn<>(28.3)},
                  .mass{lton<>(42'245)},
                  .mainGuns{in<>(14)},
                  .shellMass{lb<>(1'590)},
                  .shellSpeed{ft_per_s<>(2483)},
                  .power{hp<>(110'000)}};

  print_details("KMS Bismark, defined in appropriate units from the SI system", bismark);
  std::cout << "\n\n";
  print_details("USS Iowa, defined in appropriate units foot-pound-second system", iowa);
  std::cout << "\n\n";
  print_details("HMS King George V, defined in appropriate units foot-pound-second system", kgv);
}
