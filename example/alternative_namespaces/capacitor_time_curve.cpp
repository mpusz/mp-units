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

/*
    capacitor discharge curve using compile_time
    physical_quantities
*/

#include <units/physical/si/si.h>
#include <units/math.h>
#include "./voltage.h"
#include <iostream>

using namespace units::experimental;
using namespace units::physical::si::literals;

int main()
{
  std::cout << "mpusz/units capacitor time curve example...\n";
  std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
  std::cout.precision(3);

  constexpr auto C = 0.47_q_uF;
  constexpr auto V0 = 5.0_q_V;
  constexpr auto R = 4.7_q_kR;

  for (auto t = 0_q_ms; t <= 50_q_ms; ++t) {
    const auto Vt = V0 * units::exp(-t / (R * C));

    std::cout << "at " << t << " voltage is ";

    if (Vt >= 1_q_V)
      std::cout << Vt;
    else if (Vt >= 1_q_mV)
      std::cout << voltage::mV<>{Vt};
    else if (Vt >= 1_q_uV)
      std::cout << voltage::uV<>{Vt};
    else if (Vt >= 1_q_nV)
      std::cout << voltage::nV<>{Vt};
    else
      std::cout << voltage::pV<>{Vt};
    std::cout << "\n";
  }
}
