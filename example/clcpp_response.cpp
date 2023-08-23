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

#include <mp-units/format.h>
#include <mp-units/ostream.h>
#include <mp-units/systems/iau/iau.h>
#include <mp-units/systems/imperial/imperial.h>
#include <mp-units/systems/international/international.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si/si.h>
#include <mp-units/systems/typographic/typographic.h>
#include <mp-units/systems/usc/usc.h>
#include <iostream>

namespace {

void simple_quantities()
{
  using namespace mp_units;
  using namespace mp_units::si;
  using namespace mp_units::international;

  using distance = quantity<isq::distance[kilo<metre>]>;
  using duration = quantity<isq::duration[second]>;

  constexpr distance km = 1. * kilo<metre>;
  constexpr distance miles = 1. * mile;

  constexpr duration sec = 1 * second;
  constexpr duration min = 1 * minute;
  constexpr duration hr = 1 * hour;

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
  using namespace mp_units;
  using namespace mp_units::si;
  using namespace mp_units::international;

  constexpr auto km = 1.0 * kilo<metre>;
  constexpr auto miles = 1.0 * mile;

  std::cout.precision(6);

  constexpr auto sec = 1 * second;
  constexpr auto min = 1 * minute;
  constexpr auto hr = 1 * hour;

  std::cout << "A more flexible option is to provide separate types for each unit,\n\n";
  std::cout << km << '\n';
  std::cout << miles << '\n';
  std::cout << sec << '\n';
  std::cout << min << '\n';
  std::cout << hr << "\n\n";

  constexpr auto meter = 1. * metre;
  std::cout << "then a wide range of pre-defined units can be defined and converted,\n"
               " for consistency and repeatability across applications:\n\n";

  std::cout << meter << '\n';

  std::cout << " = " << meter.in(astronomical_unit) << '\n';
  std::cout << " = " << meter.in(iau::angstrom) << '\n';
  std::cout << " = " << meter.in(imperial::chain) << '\n';
  std::cout << " = " << meter.in(imperial::fathom) << '\n';
  std::cout << " = " << meter.in(usc::fathom) << '\n';
  std::cout << " = " << meter.in(international::foot) << '\n';
  std::cout << " = " << meter.in(usc::survey1893::us_survey_foot) << '\n';
  std::cout << " = " << meter.in(international::inch) << '\n';
  std::cout << " = " << meter.in(iau::light_year) << '\n';
  std::cout << " = " << meter.in(international::mile) << '\n';
  std::cout << " = " << meter.in(international::nautical_mile) << '\n';
  std::cout << " = " << meter.in(iau::parsec) << '\n';
  std::cout << " = " << meter.in(typographic::pica_dtp) << '\n';
  std::cout << " = " << meter.in(typographic::pica_us) << '\n';
  std::cout << " = " << meter.in(typographic::point_dtp) << '\n';
  std::cout << " = " << meter.in(typographic::point_us) << '\n';
  std::cout << " = " << meter.in(imperial::rod) << '\n';
  std::cout << " = " << meter.in(international::yard) << '\n';
}

void calcs_comparison()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  std::cout << "\nA distinct unit for each type is efficient and accurate\n"
               "when adding two values of the same very big\n"
               "or very small type:\n\n";

  const auto L1A = 2.f * fm;
  const auto L2A = 3.f * fm;
  const auto LrA = L1A + L2A;
  std::cout << MP_UNITS_STD_FMT::format("{:%.30Q %q}\n + {:%.30Q %q}\n   = {:%.30Q %q}\n\n", L1A, L2A, LrA);

  std::cout << "The single unit method must convert large\n"
               "or small values in other units to the base unit.\n"
               "This is both inefficient and inaccurate\n\n";

  const auto L1B = L1A.in(m);
  const auto L2B = L2A.in(m);
  const auto LrB = L1B + L2B;
  std::cout << MP_UNITS_STD_FMT::format("{:%.30eQ %q}\n + {:%.30eQ %q}\n   = {:%.30eQ %q}\n\n", L1B, L2B, LrB);

  std::cout << "In multiplication and division:\n\n";

  const quantity<isq::area[square(fm)], float> ArA = L1A * L2A;
  std::cout << MP_UNITS_STD_FMT::format("{:%.30Q %q}\n * {:%.30Q %q}\n   = {:%.30Q %q}\n\n", L1A, L2A, ArA);

  std::cout << "similar problems arise\n\n";

  const quantity<isq::area[m2], float> ArB = L1B * L2B;
  std::cout << MP_UNITS_STD_FMT::format("{:%.30eQ %q}\n * {:%.30eQ %q}\n   = {:%.30eQ %q}\n\n", L1B, L2B, ArB);
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
