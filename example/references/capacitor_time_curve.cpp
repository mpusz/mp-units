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

#include <units/generic/dimensionless.h>
#include <units/isq/dimensions/electric_current.h>
#include <units/isq/si/capacitance.h>
#include <units/isq/si/resistance.h>
#include <units/isq/si/time.h>
#include <units/isq/si/voltage.h>
#include <units/math.h>  // IWYU pragma: keep
#include <units/quantity_io.h>
#include <iostream>

int main()
{
  using namespace units::isq;
  using namespace units::isq::si::capacitance_references;
  using namespace units::isq::si::voltage_references;
  using namespace units::isq::si::resistance_references;
  using namespace units::isq::si::time_references;

  std::cout << "mp-units capacitor time curve example...\n";
  std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
  std::cout.precision(3);

  constexpr auto C = 0.47 * uF;
  constexpr auto V0 = 5.0 * V;
  constexpr auto RR = 4.7 * kR;  // cannot use just 'R' here

  for (auto t = 0 * ms; t <= 50 * ms; ++t) {
    const Voltage auto Vt = V0 * units::exp(-t / (RR * C));

    std::cout << "at " << t << " voltage is ";

    if (Vt >= 1 * V)
      std::cout << Vt;
    else if (Vt >= 1 * mV)
      std::cout << quantity_cast<si::millivolt>(Vt);
    else if (Vt >= 1 * uV)
      std::cout << quantity_cast<si::microvolt>(Vt);
    else if (Vt >= 1 * nV)
      std::cout << quantity_cast<si::nanovolt>(Vt);
    else
      std::cout << quantity_cast<si::picovolt>(Vt);
    std::cout << "\n";
  }
}
