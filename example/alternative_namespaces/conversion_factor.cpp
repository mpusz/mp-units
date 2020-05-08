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


#include "./units_str.h"
#include "./length.h"
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

using namespace units::physical::si::literals;
using namespace units::experimental;

int main()
{
  std::cout << "conversion factor in mpusz/units...\n\n";

  constexpr length::m<> lengthA = 2.0q_m;
  constexpr length::mm<> lengthB = lengthA;

  std::cout << "lengthA( " << lengthA << " ) and lengthB( " << lengthB << " )\n"
            << "represent the same length in different units.\n\n";

  std::cout << "therefore ratio lengthA / lengthB == " << lengthA / lengthB << "\n\n";

  std::cout << "conversion factor from lengthA::unit of "
            << units_str(lengthA).standard() << " to lengthB::unit of " << units_str(lengthB).standard() << " :\n\n"
            << "lengthB.count( " << lengthB.count() << " ) == lengthA.count( " << lengthA.count()
            << " ) * conversion_factor( " << conversion_factor(lengthB, lengthA) << " )\n";
}
