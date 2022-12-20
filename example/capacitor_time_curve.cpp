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

#include <units/isq/electromagnetism.h>
#include <units/math.h>  // IWYU pragma: keep
#include <units/quantity_io.h>
#include <units/si/si.h>
#include <iostream>

int main()
{
  using namespace units;
  using namespace units::si::unit_symbols;

  std::cout << "mp-units capacitor time curve example...\n";
  std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
  std::cout.precision(3);

  constexpr auto C = 0.47 * isq::capacitance[uF];
  constexpr auto V0 = 5.0 * isq::voltage[V];
  constexpr auto R = 4.7 * isq::resistance[si::kilo<si::ohm>];

  for (auto t = 0 * isq::time[ms]; t <= 50 * isq::time[ms]; ++t) {
    const weak_quantity_of<isq::voltage> auto Vt = V0 * units::exp(-t / (R * C));

    std::cout << "at " << t << " voltage is ";

    if (Vt >= 1 * isq::voltage[V])
      std::cout << Vt[V];
    else if (Vt >= 1 * isq::voltage[mV])
      std::cout << Vt[mV];
    else if (Vt >= 1 * isq::voltage[uV])
      std::cout << Vt[uV];
    else if (Vt >= 1 * isq::voltage[nV])
      std::cout << Vt[nV];
    else
      std::cout << Vt[pV];
    std::cout << "\n";
  }
}
