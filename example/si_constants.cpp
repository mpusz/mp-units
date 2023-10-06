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

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! Before you commit any changes to this file please make sure to check if it !!!
// !!! renders correctly in the documentation "Examples" section.                 !!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#include <mp-units/format.h>
#include <mp-units/systems/si/si.h>
#include <iostream>

template<class T>
  requires mp_units::is_scalar<T>
inline constexpr bool mp_units::is_vector<T> = true;

int main()
{
  using namespace mp_units;
  using namespace mp_units::si;
  using namespace mp_units::si::unit_symbols;

  std::cout << "The seven defining constants of the SI and the seven corresponding units they define:\n";
  std::cout << MP_UNITS_STD_FMT::format("- hyperfine transition frequency of Cs: {} = {:%.0Q %q}\n",
                                        1. * si2019::hyperfine_structure_transition_frequency_of_cs,
                                        (1. * si2019::hyperfine_structure_transition_frequency_of_cs).in(Hz));
  std::cout << MP_UNITS_STD_FMT::format("- speed of light in vacuum:             {} = {:%.0Q %q}\n",
                                        1. * si2019::speed_of_light_in_vacuum,
                                        (1. * si2019::speed_of_light_in_vacuum).in(m / s));
  std::cout << MP_UNITS_STD_FMT::format("- Planck constant:                      {} = {:%.8eQ %q}\n",
                                        1. * si2019::planck_constant, (1. * si2019::planck_constant).in(J * s));
  std::cout << MP_UNITS_STD_FMT::format("- elementary charge:                    {} = {:%.9eQ %q}\n",
                                        1. * si2019::elementary_charge, (1. * si2019::elementary_charge).in(C));
  std::cout << MP_UNITS_STD_FMT::format("- Boltzmann constant:                   {} = {:%.6eQ %q}\n",
                                        1. * si2019::boltzmann_constant, (1. * si2019::boltzmann_constant).in(J / K));
  std::cout << MP_UNITS_STD_FMT::format("- Avogadro constant:                    {} = {:%.8eQ %q}\n",
                                        1. * si2019::avogadro_constant, (1. * si2019::avogadro_constant).in(one / mol));
  std::cout << MP_UNITS_STD_FMT::format("- luminous efficacy:                    {} = {}\n",
                                        1. * si2019::luminous_efficacy, (1. * si2019::luminous_efficacy).in(lm / W));
}
