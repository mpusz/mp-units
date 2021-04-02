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
#include <units/quantity_io.h>
#include <iostream>

namespace {

void simple_quantities()
{
  using namespace units::isq;
  using namespace units::isq::si::references;
  using namespace units::isq::si::international::references;

  using distance = si::length<si::metre>;
  using duration = si::time<si::second>;

  constexpr distance kilometers = 1.0 * km;
  constexpr distance miles = 1.0 * mi;

  constexpr duration seconds = 1 * s;
  constexpr duration minutes = 1 * min;
  constexpr duration hours = 1 * h;

  std::cout << "A physical quantities library can choose the simple\n";
  std::cout << "option to provide output using a single type for each base unit:\n\n";
  std::cout << kilometers << '\n';
  std::cout << miles << '\n';
  std::cout << seconds << '\n';
  std::cout << minutes << '\n';
  std::cout << hours << "\n\n";
}

void quantities_with_typed_units()
{
  using namespace units::isq;
  using namespace units::isq::si::references;
  using namespace units::isq::si::international::references;

  constexpr si::length<si::kilometre> kilometers = 1.0 * km;
  constexpr si::length<si::international::mile> miles = 1.0 * mi;

  std::cout.precision(6);

  constexpr si::time<si::second> seconds = 1 * s;
  constexpr si::time<si::minute> minutes = 1 * min;
  constexpr si::time<si::hour> hours = 1 * h;

  std::cout << "A more flexible option is to provide separate types for each unit,\n\n";
  std::cout << kilometers << '\n';
  std::cout << miles << '\n';
  std::cout << seconds << '\n';
  std::cout << minutes << '\n';
  std::cout << hours << "\n\n";

  constexpr si::length<si::metre> meter = 1 * m;
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
  using namespace units::isq;
  using namespace units::isq::si::references;

  std::cout << "\nA distinct unit for each type is efficient and accurate\n"
               "when adding two values of the same very big\n"
               "or very small type:\n\n";

  si::length<si::femtometre, float> L1A = 2. * fm;
  si::length<si::femtometre, float> L2A = 3. * fm;
  si::length<si::femtometre, float> LrA = L1A + L2A;
  fmt::print("{:%.30Q %q}\n + {:%.30Q %q}\n   = {:%.30Q %q}\n\n", L1A, L2A, LrA);

  std::cout << "The single unit method must convert large\n"
               "or small values in other units to the base unit.\n"
               "This is both inefficient and inaccurate\n\n";

  si::length<si::metre, float> L1B = L1A;
  si::length<si::metre, float> L2B = L2A;
  si::length<si::metre, float> LrB = L1B + L2B;
  fmt::print("{:%.30Q %q}\n + {:%.30Q %q}\n   = {:%.30Q %q}\n\n", L1B, L2B, LrB);

  std::cout << "In multiplication and division:\n\n";

  si::area<si::square_femtometre, float> ArA = L1A * L2A;
  fmt::print("{:%.30Q %q}\n * {:%.30Q %q}\n   = {:%.30Q %q}\n\n", L1A, L2A, ArA);

  std::cout << "similar problems arise\n\n";

  si::area<si::square_metre, float> ArB = L1B * L2B;
  fmt::print("{:%.30Q %q}\n * {:%.30Q %q}\n   = {:%.30Q %q}\n\n", L1B, L2B, ArB);
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
