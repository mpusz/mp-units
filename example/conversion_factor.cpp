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

#include <units/physical/si/length.h>
#include <units/format.h>
#include <iostream>

/*
  get conversion factor from one dimensionally equivalent
  quantity type to another
*/

namespace {

template<units::Quantity Target, units::Quantity Source>
  requires units::equivalent<typename Source::dimension, typename Target::dimension>
inline constexpr std::common_type_t<typename Target::rep, typename Source::rep> conversion_factor(Target, Source)
{
  // get quantities looking like inputs but with Q::rep that doesn't have narrowing conversion
  typedef std::common_type_t<typename Target::rep, typename Source::rep> rep;
  typedef units::quantity<typename Source::dimension, typename Source::unit, rep> source;
  typedef units::quantity<typename Target::dimension, typename Target::unit, rep> target;
  return target{source{1}}.count();
}

}  // namespace

int main()
{
  using namespace units::physical::si;

  std::cout << "conversion factor in mp-units...\n\n";

  constexpr length<metre> lengthA = 2.0q_m;
  constexpr length<millimetre> lengthB = lengthA;

  std::cout << fmt::format("lengthA( {} ) and lengthB( {} )\n", lengthA, lengthB)
            << "represent the same length in different units.\n\n";

  std::cout << fmt::format("therefore ratio lengthA / lengthB == {}\n\n", lengthA / lengthB);

  std::cout << fmt::format("conversion factor from lengthA::unit of {:%q} to lengthB::unit of {:%q}:\n\n", lengthA, lengthB)
            << fmt::format("lengthB.count( {} ) == lengthA.count( {} ) * conversion_factor( {} )\n",
                           lengthB.count(), lengthA.count(), conversion_factor(lengthB, lengthA));
}
