/*
 Copyright (c) 2003-2019 Andy Little.

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

#include <units/format.h>
#include <units/isq/si/area.h>
#include <units/isq/si/iau/length.h>
#include <units/isq/si/imperial/length.h>
#include <units/isq/si/international/length.h>
#include <units/isq/si/length.h>
#include <units/isq/si/time.h>
#include <units/isq/si/typographic/length.h>
#include <units/isq/si/us/length.h>
#include <units/isq/si/volume.h>
#include <units/quantity_io.h>
#include <iostream>

namespace {

using namespace units;

void simple_quantities()
{
  using namespace units::isq::si;
  using namespace units::isq::si::international;

  using distance = length<metre>;
  using duration = isq::si::time<second>;

  constexpr distance km = 1.0_q_km;
  constexpr distance miles = 1.0_q_mi;

  constexpr duration sec = 1_q_s;
  constexpr duration min = 1_q_min;
  constexpr duration hr = 1_q_h;

  std::cout << "A physical quantities library can choose the simple\n";
  std::cout << "option to provide output using a single type for each base unit:\n\n";
  std::cout << km << '\n';
  std::cout << miles << '\n';
  std::cout << sec << '\n';
  std::cout << min << '\n';
  std::cout << hr << "\n\n";
}

void quantities_with_typed_units()
{
  using namespace units::isq;
  using namespace units::isq::si;
  using namespace units::isq::si::international;

  constexpr length<kilometre> km = 1.0_q_km;
  constexpr length<mile> miles = 1.0_q_mi;

  std::cout.precision(6);

  constexpr si::time<second> sec = 1_q_s;
  constexpr si::time<minute> min = 1_q_min;
  constexpr si::time<hour> hr = 1_q_h;

  std::cout << "A more flexible option is to provide separate types for each unit,\n\n";
  std::cout << km << '\n';
  std::cout << miles << '\n';
  std::cout << sec << '\n';
  std::cout << min << '\n';
  std::cout << hr << "\n\n";

  constexpr length<metre> meter = 1_q_m;
  std::cout << "then a wide range of pre-defined units can be defined and converted,\n"
               " for consistency and repeatability across applications:\n\n";

  std::cout << meter << '\n';

  std::cout << " = " << quantity_cast<si::astronomical_unit>(meter) << '\n';
  std::cout << " = " << quantity_cast<si::iau::angstrom>(meter) << '\n';
  std::cout << " = " << quantity_cast<si::imperial::chain>(meter) << '\n';
  std::cout << " = " << quantity_cast<si::international::fathom>(meter) << '\n';
  std::cout << " = " << quantity_cast<si::us::fathom>(meter) << '\n';
  std::cout << " = " << quantity_cast<si::international::foot>(meter) << '\n';
  std::cout << " = " << quantity_cast<si::us::foot>(meter) << '\n';
  std::cout << " = " << quantity_cast<si::international::inch>(meter) << '\n';
  std::cout << " = " << quantity_cast<si::iau::light_year>(meter) << '\n';
  std::cout << " = " << quantity_cast<si::international::mile>(meter) << '\n';
  std::cout << " = " << quantity_cast<si::international::nautical_mile>(meter) << '\n';
  std::cout << " = " << quantity_cast<si::iau::parsec>(meter) << '\n';
  std::cout << " = " << quantity_cast<si::typographic::pica_comp>(meter) << '\n';
  std::cout << " = " << quantity_cast<si::typographic::pica_prn>(meter) << '\n';
  std::cout << " = " << quantity_cast<si::typographic::point_comp>(meter) << '\n';
  std::cout << " = " << quantity_cast<si::typographic::point_prn>(meter) << '\n';
  std::cout << " = " << quantity_cast<si::imperial::rod>(meter) << '\n';
  std::cout << " = " << quantity_cast<si::international::yard>(meter) << '\n';
}

void calcs_comparison()
{
  using namespace units::isq::si;

  std::cout << "\nA distinct unit for each type is efficient and accurate\n"
               "when adding two values of the same very big\n"
               "or very small type:\n\n";

  length<femtometre,float> L1A = 2._q_fm;
  length<femtometre,float> L2A = 3._q_fm;
  length<femtometre,float> LrA = L1A + L2A;
  fmt::print("{:%.30Q %q}\n + {:%.30Q %q}\n   = {:%.30Q %q}\n\n",L1A,L2A,LrA);

  std::cout << "The single unit method must convert large\n"
               "or small values in other units to the base unit.\n"
               "This is both inefficient and inaccurate\n\n";

  length<metre,float> L1B = L1A;
  length<metre,float> L2B = L2A;
  length<metre,float> LrB = L1B + L2B;
  fmt::print("{:%.30Q %q}\n + {:%.30Q %q}\n   = {:%.30Q %q}\n\n",L1B,L2B,LrB);

  std::cout << "In multiplication and division:\n\n";

  area<square_femtometre,float> ArA = L1A * L2A;
  fmt::print("{:%.30Q %q}\n * {:%.30Q %q}\n   = {:%.30Q %q}\n\n",L1A,L2A,ArA);

  std::cout << "similar problems arise\n\n";

  area<square_metre,float> ArB = L1B * L2B;
  fmt::print("{:%.30Q %q}\n * {:%.30Q %q}\n   = {:%.30Q %q}\n\n",L1B,L2B,ArB);
}

}  // namespace

int main()
{
  std::cout << "This demo was originally posted on com.lang.c++.moderated in 2006\n";
  std::cout << "http://compgroups.net/comp.lang.c++.moderated/dimensional-analysis-units/51712\n";
  std::cout << "Here converted to use mp-units library.\n\n";

  simple_quantities();
  quantities_with_typed_units();
  calcs_comparison();
}
