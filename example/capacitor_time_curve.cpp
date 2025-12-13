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

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! Before you commit any changes to this file please make sure to check if it !!!
// !!! renders correctly in the documentation "Examples" section.                 !!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <iostream>
#endif
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/math.h>
#include <mp-units/systems/isq/electromagnetism.h>
#include <mp-units/systems/si.h>
#endif


int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  constexpr auto CC = isq::capacitance(0.47 * uF);
  constexpr auto V0 = isq::voltage(5.0 * V);
  constexpr auto RR = isq::resistance(4.7 * si::kilo<si::ohm>);

  std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
  std::cout.precision(3);
  std::cout << "Capacitor time curve example for the following parameters:\n";
  std::cout << "Capacitance: " << CC << "\n";
  std::cout << "Initial Voltage: " << V0 << "\n";
  std::cout << "Resistance: " << RR << "\n";

  std::cout << "Time curve:\n";
  for (auto tt = 0 * ms; tt <= 50 * ms; ++tt) {
    const QuantityOf<isq::voltage> auto Vt = V0 * exp(-tt / (RR * CC));
    std::cout << "- at " << tt << " voltage is ";
    si::invoke_with_prefixed([](auto q) { std::cout << q; }, Vt, V);
    std::cout << "\n";
  }
}
