/*
 Copyright (c) 2003-2020 Andy Little.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see http://www.gnu.org/licenses./
*/

#include <units/format.h>
#include <units/systems/isq/space_and_time.h>
#include <units/systems/si/units.h>
#include <iostream>
#include <type_traits>

/*
  get conversion factor from one dimensionally equivalent
  quantity type to another
*/

namespace {

template<units::Quantity Target, units::Quantity Source>
  requires std::constructible_from<Target, Source>
inline constexpr double conversion_factor(Target, Source)
{
  return quantity_cast<Target::unit>(1. * Source::reference).number();
}

}  // namespace

int main()
{
  using namespace units;

  std::cout << "conversion factor in mp-units...\n\n";

  constexpr auto lengthA = isq::length(2.0, si::metre);
  constexpr auto lengthB = lengthA[si::milli<si::metre>];

  std::cout << STD_FMT::format("lengthA( {} ) and lengthB( {} )\n", lengthA, lengthB)
            << "represent the same length in different units.\n\n";

  std::cout << STD_FMT::format("therefore ratio lengthA / lengthB == {}\n\n", lengthA / lengthB);

  std::cout << STD_FMT::format("conversion factor from lengthA::unit of {:%q} to lengthB::unit of {:%q}:\n\n", lengthA,
                               lengthB)
            << STD_FMT::format("lengthB.number( {} ) == lengthA.number( {} ) * conversion_factor( {} )\n",
                               lengthB.number(), lengthA.number(), conversion_factor(lengthB, lengthA));
}
