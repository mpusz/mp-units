// The MIT License (MIT)
//
// Copyright (c) 2018 Mateusz Pusz
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <units/format.h>
#include <units/isq/si/constants.h>
#include <iostream>

int main()
{
  using namespace units::isq::si::si2019;

  std::cout << "The seven defining constants of the SI and the seven corresponding units they define:\n";
  std::cout << UNITS_STD_FMT::format("- hyperfine transition frequency of Cs: {:%.0Q %q}\n",
                               hyperfine_structure_transition_frequency<>);
  std::cout << UNITS_STD_FMT::format("- speed of light in vacuum:             {:%.0Q %q}\n", speed_of_light<>);
  std::cout << UNITS_STD_FMT::format("- Planck constant:                      {}\n", planck_constant<>);
  std::cout << UNITS_STD_FMT::format("- elementary charge:                    {}\n", elementary_charge<>);
  std::cout << UNITS_STD_FMT::format("- Boltzmann constant:                   {}\n", boltzmann_constant<>);
  std::cout << UNITS_STD_FMT::format("- Avogadro constant:                    {}\n", avogadro_constant<>);
  std::cout << UNITS_STD_FMT::format("- luminous efficacy:                    {}\n", luminous_efficacy<>);
}
