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
#include <units/iau/iau.h>
#include <units/imperial/imperial.h>
#include <units/international/international.h>
#include <units/isq/space_and_time.h>
#include <units/quantity_io.h>
#include <units/si/si.h>
#include <units/typographic/typographic.h>
#include <units/usc/usc.h>
#include <iostream>

namespace {

void simple_quantities()
{
  using namespace units;
  using namespace units::si;
  using namespace units::international;

  using distance = quantity<isq::distance[kilo<metre>]>;
  using duration = quantity<isq::duration[second]>;

  constexpr distance km = isq::distance[kilo<metre>](1.0);
  constexpr distance miles = isq::distance[mile](1.0);

  constexpr duration sec = isq::duration[second](1);
  constexpr duration min = isq::duration[minute](1);
  constexpr duration hr = isq::duration[hour](1);

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
  using namespace units;
  using namespace units::si;
  using namespace units::international;

  constexpr auto km = isq::distance[kilo<metre>](1.0);
  constexpr auto miles = isq::distance[mile](1.0);

  std::cout.precision(6);

  constexpr auto sec = isq::duration[second](1);
  constexpr auto min = isq::duration[minute](1);
  constexpr auto hr = isq::duration[hour](1);

  std::cout << "A more flexible option is to provide separate types for each unit,\n\n";
  std::cout << km << '\n';
  std::cout << miles << '\n';
  std::cout << sec << '\n';
  std::cout << min << '\n';
  std::cout << hr << "\n\n";

  constexpr quantity<isq::length[metre]> meter{1};
  std::cout << "then a wide range of pre-defined units can be defined and converted,\n"
               " for consistency and repeatability across applications:\n\n";

  std::cout << meter << '\n';

  std::cout << " = " << meter[astronomical_unit] << '\n';
  std::cout << " = " << meter[iau::angstrom] << '\n';
  std::cout << " = " << meter[imperial::chain] << '\n';
  std::cout << " = " << meter[imperial::fathom] << '\n';
  std::cout << " = " << meter[usc::fathom] << '\n';
  std::cout << " = " << meter[international::foot] << '\n';
  std::cout << " = " << meter[usc::survey1893::us_survey_foot] << '\n';
  std::cout << " = " << meter[international::inch] << '\n';
  std::cout << " = " << meter[iau::light_year] << '\n';
  std::cout << " = " << meter[international::mile] << '\n';
  std::cout << " = " << meter[international::nautical_mile] << '\n';
  std::cout << " = " << meter[iau::parsec] << '\n';
  std::cout << " = " << meter[typographic::pica_dtp] << '\n';
  std::cout << " = " << meter[typographic::pica_us] << '\n';
  std::cout << " = " << meter[typographic::point_dtp] << '\n';
  std::cout << " = " << meter[typographic::point_us] << '\n';
  std::cout << " = " << meter[imperial::rod] << '\n';
  std::cout << " = " << meter[international::yard] << '\n';
}

void calcs_comparison()
{
  using namespace units;
  using namespace units::si::unit_symbols;

  std::cout << "\nA distinct unit for each type is efficient and accurate\n"
               "when adding two values of the same very big\n"
               "or very small type:\n\n";

  const auto L1A = isq::length[fm](2.f);
  const auto L2A = isq::length[fm](3.f);
  const auto LrA = L1A + L2A;
  std::cout << STD_FMT::format("{:%.30Q %q}\n + {:%.30Q %q}\n   = {:%.30Q %q}\n\n", L1A, L2A, LrA);

  std::cout << "The single unit method must convert large\n"
               "or small values in other units to the base unit.\n"
               "This is both inefficient and inaccurate\n\n";

  const auto L1B = L1A[m];
  const auto L2B = L2A[m];
  const auto LrB = L1B + L2B;
  std::cout << STD_FMT::format("{:%.30eQ %q}\n + {:%.30eQ %q}\n   = {:%.30eQ %q}\n\n", L1B, L2B, LrB);

  std::cout << "In multiplication and division:\n\n";

  const quantity<isq::area[square<fm>], float> ArA = L1A * L2A;
  std::cout << STD_FMT::format("{:%.30Q %q}\n * {:%.30Q %q}\n   = {:%.30Q %q}\n\n", L1A, L2A, ArA);

  std::cout << "similar problems arise\n\n";

  const quantity<isq::area[m2], float> ArB = L1B * L2B;
  std::cout << STD_FMT::format("{:%.30eQ %q}\n * {:%.30eQ %q}\n   = {:%.30eQ %q}\n\n", L1B, L2B, ArB);
}

}  // namespace

int main()
{
  std::cout << "This demo was originally posted on com.lang.c++.moderated in 2006\n";
  std::cout << "https://groups.google.com/g/comp.lang.c++.moderated/c/upv7hZExtf4/m/XruKUk8LhXYJ\n";
  std::cout << "Here converted to use mp-units library.\n\n";

  simple_quantities();
  quantities_with_typed_units();
  calcs_comparison();
}
