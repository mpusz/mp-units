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

#include <mp-units/math.h>  // IWYU pragma: keep
#include <mp-units/ostream.h>
#include <mp-units/systems/isq/electromagnetism.h>
#include <mp-units/systems/si/si.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  std::cout << "mp-units capacitor time curve example...\n";
  std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
  std::cout.precision(3);

  constexpr auto CC = isq::capacitance(0.47 * uF);
  constexpr auto V0 = isq::voltage(5.0 * V);
  constexpr auto RR = isq::resistance(4.7 * si::kilo<si::ohm>);

  for (auto tt = 0 * ms; tt <= 50 * ms; ++tt) {
    const QuantityOf<isq::voltage> auto Vt = V0 * exp(dimensionless(-tt / (RR * CC)));
    // TODO try to make the below work instead
    // const QuantityOf<isq::voltage> auto Vt = V0 * exp(-tt / (RR * CC));

    std::cout << "at " << tt << " voltage is ";

    if (Vt >= 1 * V)
      std::cout << Vt.in(V);
    else if (Vt >= 1 * mV)
      std::cout << Vt.in(mV);
    else if (Vt >= 1 * uV)
      std::cout << Vt.in(uV);
    else if (Vt >= 1 * nV)
      std::cout << Vt.in(nV);
    else
      std::cout << Vt.in(pV);
    std::cout << "\n";
  }
}
