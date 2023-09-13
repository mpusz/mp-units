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

#include <mp-units/format.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si/unit_symbols.h>
#include <mp-units/systems/si/units.h>
#include <iostream>
#include <type_traits>

/*
  get conversion factor from one dimensionally equivalent
  quantity type to another
*/

namespace {

template<mp_units::Quantity Target, mp_units::Quantity Source>
  requires std::constructible_from<Target, Source>
inline constexpr double conversion_factor(Target, Source)
{
  return (1. * Source::reference).force_numerical_value_in(Target::unit);
}

}  // namespace

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  std::cout << "conversion factor in mp-units...\n\n";

  constexpr auto lengthA = 2.0 * m;
  constexpr auto lengthB = lengthA.in(mm);

  std::cout << MP_UNITS_STD_FMT::format("lengthA( {} ) and lengthB( {} )\n", lengthA, lengthB)
            << "represent the same length in different units.\n\n";

  std::cout << MP_UNITS_STD_FMT::format("therefore ratio lengthA / lengthB == {}\n\n", lengthA / lengthB);

  std::cout << MP_UNITS_STD_FMT::format("conversion factor from lengthA::unit of {:%q} to lengthB::unit of {:%q}:\n\n",
                                        lengthA, lengthB)
            << MP_UNITS_STD_FMT::format("lengthB.value( {} ) == lengthA.value( {} ) * conversion_factor( {} )\n",
                                        lengthB.numerical_value_ref_in(lengthB.unit),
                                        lengthA.numerical_value_ref_in(lengthA.unit),
                                        conversion_factor(lengthB, lengthA));
}
