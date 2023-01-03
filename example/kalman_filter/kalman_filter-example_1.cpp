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

#include "kalman.h"
#include <units/format.h>
#include <units/generic/dimensionless.h>
#include <units/isq/si/mass.h>
#include <array>
#include <iostream>

// Based on: https://www.kalmanfilter.net/alphabeta.html#ex1

using namespace units;

void print_header(const kalman::State auto& initial)
{
  std::cout << UNITS_STD_FMT::format("Initial: {}\n", initial);
  std::cout << UNITS_STD_FMT::format("{:>2} | {:>9} | {:>8} | {:>14} | {:>14}\n", "N", "Gain", "Measured", "Curr. Estimate",
                               "Next Estimate");
}

void print(auto iteration, Dimensionless auto gain, Quantity auto measured, const kalman::State auto& current,
           const kalman::State auto& next)
{
  std::cout << UNITS_STD_FMT::format("{:2} | {:9} | {:8} | {:14} | {:14}\n", iteration, gain, measured, current, next);
}

int main()
{
  using namespace units::isq;
  using namespace units::isq::si::references;
  using state = kalman::state<si::mass<si::gram>>;

  const state initial = {1 * kg};
  const std::array measurements = {1030 * g, 989 * g,  1017 * g, 1009 * g, 1013 * g,
                                   979 * g,  1008 * g, 1042 * g, 1012 * g, 1011 * g};

  print_header(initial);
  state next = initial;
  for (int index = 1; const auto& m : measurements) {
    const auto& previous = next;
    const dimensionless<one> gain = 1. / index;
    const auto current = state_update(previous, m, gain);
    next = current;
    print(index++, gain, m, current, next);
  }
}
