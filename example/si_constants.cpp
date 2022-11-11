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
#include <units/si/constants.h>
#include <units/si/unit_symbols.h>
#include <iostream>

int main()
{
  using namespace units::si;
  using namespace units::si::unit_symbols;

  std::cout << "The seven defining constants of the SI and the seven corresponding units they define:\n";
  std::cout << STD_FMT::format("- hyperfine transition frequency of Cs: {} = {:%.0Q %q}\n",
                               1. * si2019::hyperfine_structure_transition_frequency_of_cs,
                               (1. * si2019::hyperfine_structure_transition_frequency_of_cs)[Hz]);
  std::cout << STD_FMT::format("- speed of light in vacuum:             {} = {:%.0Q %q}\n",
                               1. * si2019::speed_of_light_in_vacuum, (1. * si2019::speed_of_light_in_vacuum)[m / s]);
  std::cout << STD_FMT::format("- Planck constant:                      {} = {:%.8eQ %q}\n",
                               1. * si2019::planck_constant, (1. * si2019::planck_constant)[J * s]);
  // TODO uncomment the below when ISQ is done
  // std::cout << STD_FMT::format("- elementary charge:                    {} = {:%.9eQ %q}\n",
  //                              1. * si2019::elementary_charge, (1. * si2019::elementary_charge)[C]);
  // std::cout << STD_FMT::format("- Boltzmann constant:                   {} = {:%.6eQ %q}\n",
  //                              1. * si2019::boltzmann_constant, (1. * si2019::boltzmann_constant)[J / K]);
  std::cout << STD_FMT::format("- Avogadro constant:                    {} = {:%.8eQ %q}\n",
                               1. * si2019::avogadro_constant, (1. * si2019::avogadro_constant)[1 / mol]);
  // std::cout << STD_FMT::format("- luminous efficacy:                    {} = {}\n", 1. * si2019::luminous_efficacy,
  //                              (1. * si2019::luminous_efficacy)[lm / W]);
}
