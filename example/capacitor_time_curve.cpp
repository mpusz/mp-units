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

#include <units/math.h>  // IWYU pragma: keep
#include <units/quantity_io.h>
#include <units/systems/isq/electromagnetism.h>
#include <units/systems/si/si.h>
#include <iostream>

int main()
{
  using namespace units;
  using namespace units::si::unit_symbols;

  std::cout << "mp-units capacitor time curve example...\n";
  std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
  std::cout.precision(3);

  constexpr auto C = isq::capacitance(0.47, uF);
  constexpr auto V0 = isq::voltage(5.0, V);
  constexpr auto R = isq::resistance(4.7, si::kilo<si::ohm>);

  for (auto t = isq::time(0, ms); t <= isq::time(50, ms); ++t) {
    const weak_quantity_of<isq::voltage> auto Vt = V0 * units::exp(-t / (R * C));

    std::cout << "at " << t << " voltage is ";

    if (Vt >= isq::voltage(1, V))
      std::cout << Vt[V];
    else if (Vt >= isq::voltage(1, mV))
      std::cout << Vt[mV];
    else if (Vt >= isq::voltage(1, uV))
      std::cout << Vt[uV];
    else if (Vt >= isq::voltage(1, nV))
      std::cout << Vt[nV];
    else
      std::cout << Vt[pV];
    std::cout << "\n";
  }
}
