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
#include <units/math.h> // IWYU pragma: keep
#include <units/quantity_io.h>
#include <iostream>

int main()
{
  using namespace units::isq;
  using namespace units::aliases::isq::si;

  std::cout << "mp-units capacitor time curve example...\n";
  std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
  std::cout.precision(3);

  constexpr auto C = capacitance::uF<>(0.47);
  constexpr auto V0 = voltage::V<>(5.0);
  constexpr auto R = resistance::kR<>(4.7);

  for (auto t = ms<int>(0); t <= ms<int>(50); ++t) {
    const Voltage auto Vt = V0 * units::exp(-t / (R * C));

    std::cout << "at " << t << " voltage is ";

    if (Vt >= V<>(1))
      std::cout << Vt;
    else if (Vt >= mV<>(1))
      std::cout << mV<>(Vt);
    else if (Vt >= uV<>(1))
      std::cout << uV<>(Vt);
    else if (Vt >= nV<>(1))
      std::cout << nV<>(Vt);
    else
      std::cout << pV<>(Vt);
    std::cout << "\n";
  }
}
