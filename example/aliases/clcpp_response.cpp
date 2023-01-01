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
#include <units/isq/si/uscs/length.h>
#include <units/quantity_io.h>
#include <iostream>

namespace {

using namespace units;

void simple_quantities()
{
  using namespace units::aliases::isq;
  using namespace units::aliases::isq::si;
  using namespace units::aliases::isq::si::international;

  using distance = si::length::m<>;
  using duration = time::s<>;

  constexpr distance km = si::length::km<>(1.0);
  constexpr distance miles = mi<>(1.0);

  constexpr duration sec = s<>(1);
  constexpr duration min = time::min<>(1);
  constexpr duration hr = h<>(1);

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
  using namespace units::aliases::isq;
  using namespace units::aliases::isq::si;
  using namespace units::aliases::isq::si::international;

  constexpr si::length::km<> km = si::km<>(1.0);
  constexpr si::international::length::mi<> miles = mi<>(1.0);

  std::cout.precision(6);

  constexpr time::s<> sec = s<>(1);
  constexpr time::min<> min = si::min<>(1);
  constexpr time::h<> hr = h<>(1);

  std::cout << "A more flexible option is to provide separate types for each unit,\n\n";
  std::cout << km << '\n';
  std::cout << miles << '\n';
  std::cout << sec << '\n';
  std::cout << min << '\n';
  std::cout << hr << "\n\n";

  constexpr si::length::m<> meter = m<>(1);
  std::cout << "then a wide range of pre-defined units can be defined and converted,\n"
               " for consistency and repeatability across applications:\n\n";

  std::cout << meter << '\n';

  std::cout << " = " << au<>(meter) << '\n';
  std::cout << " = " << iau::angstrom<>(meter) << '\n';
  std::cout << " = " << imperial::ch<>(meter) << '\n';
  std::cout << " = " << international::fathom<>(meter) << '\n';
  std::cout << " = " << uscs::fathom<>(meter) << '\n';
  std::cout << " = " << international::ft<>(meter) << '\n';
  std::cout << " = " << uscs::ft<>(meter) << '\n';
  std::cout << " = " << international::in<>(meter) << '\n';
  std::cout << " = " << iau::ly<>(meter) << '\n';
  std::cout << " = " << international::mi<>(meter) << '\n';
  std::cout << " = " << international::mi_naut<>(meter) << '\n';
  std::cout << " = " << iau::pc<>(meter) << '\n';
  std::cout << " = " << typographic::pica_comp<>(meter) << '\n';
  std::cout << " = " << typographic::pica_prn<>(meter) << '\n';
  std::cout << " = " << typographic::point_comp<>(meter) << '\n';
  std::cout << " = " << typographic::point_prn<>(meter) << '\n';
  std::cout << " = " << imperial::rd<>(meter) << '\n';
  std::cout << " = " << international::yd<>(meter) << '\n';
}

void calcs_comparison()
{
  using namespace units::aliases::isq::si;

  std::cout << "\nA distinct unit for each type is efficient and accurate\n"
               "when adding two values of the same very big\n"
               "or very small type:\n\n";

  length::fm<float> L1A = fm<>(2.f);
  length::fm<float> L2A = fm<>(3.f);
  length::fm<float> LrA = L1A + L2A;
  std::cout << UNITS_STD_FMT::format("{:%.30Q %q}\n + {:%.30Q %q}\n   = {:%.30Q %q}\n\n", L1A, L2A, LrA);

  std::cout << "The single unit method must convert large\n"
               "or small values in other units to the base unit.\n"
               "This is both inefficient and inaccurate\n\n";

  length::m<float> L1B = L1A;
  length::m<float> L2B = L2A;
  length::m<float> LrB = L1B + L2B;
  std::cout << UNITS_STD_FMT::format("{:%.30Q %q}\n + {:%.30Q %q}\n   = {:%.30Q %q}\n\n", L1B, L2B, LrB);

  std::cout << "In multiplication and division:\n\n";

  area::fm2<float> ArA = L1A * L2A;
  std::cout << UNITS_STD_FMT::format("{:%.30Q %q}\n * {:%.30Q %q}\n   = {:%.30Q %q}\n\n", L1A, L2A, ArA);

  std::cout << "similar problems arise\n\n";

  area::m2<float> ArB = L1B * L2B;
  std::cout << UNITS_STD_FMT::format("{:%.30Q %q}\n * {:%.30Q %q}\n   = {:%.30Q %q}\n\n", L1B, L2B, ArB);
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
