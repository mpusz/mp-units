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
#include <mp_units/format.h>
#include <mp_units/systems/isq/space_and_time.h>
#include <mp_units/systems/si/unit_symbols.h>
#include <array>
#include <iostream>

// Based on: https://www.kalmanfilter.net/alphabeta.html#ex1

using namespace mp_units;

void print_header(const kalman::State auto& initial)
{
  std::cout << STD_FMT::format("Initial: {}\n", initial);
  std::cout << STD_FMT::format("{:>2} | {:>9} | {:>8} | {:>14} | {:>14}\n", "N", "Gain", "Measured", "Curr. Estimate",
                               "Next Estimate");
}

void print(auto iteration, quantity_of<dimensionless> auto gain, Quantity auto measured,
           const kalman::State auto& current, const kalman::State auto& next)
{
  std::cout << STD_FMT::format("{:2} | {:9} | {:8} | {:14} | {:14}\n", iteration, gain, measured, current, next);
}

int main()
{
  using namespace mp_units::si::unit_symbols;
  using state = kalman::state<quantity<isq::mass[g]>>;

  const state initial = {1 * isq::mass[kg]};
  const std::array measurements = {1030 * isq::mass[g], 989 * isq::mass[g], 1017 * isq::mass[g], 1009 * isq::mass[g],
                                   1013 * isq::mass[g], 979 * isq::mass[g], 1008 * isq::mass[g], 1042 * isq::mass[g],
                                   1012 * isq::mass[g], 1011 * isq::mass[g]};

  print_header(initial);
  state next = initial;
  for (int index = 1; const auto& m : measurements) {
    const auto& previous = next;
    const quantity gain = 1. / index;
    const auto current = state_update(previous, m, gain);
    next = current;
    print(index++, gain, m, current, next);
  }
}
