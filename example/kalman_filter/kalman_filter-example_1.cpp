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
#include <array>
#include <iostream>
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/format.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si/unit_symbols.h>
#endif

// Based on: https://www.kalmanfilter.net/alphabeta.html#ex1

using namespace mp_units;

void print_header(const kalman::SystemState auto& initial)
{
  std::cout << MP_UNITS_STD_FMT::format("Initial: {}\n", initial);
  std::cout << MP_UNITS_STD_FMT::format("{:>2} | {:>9} | {:>8} | {:>14} | {:>14}\n", "N", "Gain", "Measured",
                                        "Curr. Estimate", "Next Estimate");
}

void print(auto iteration, QuantityOf<dimensionless> auto gain, QuantityPoint auto measured,
           const kalman::SystemState auto& current, const kalman::SystemState auto& next)
{
  std::cout << MP_UNITS_STD_FMT::format("{:2} | {:9:N[.2f]} | {:8} | {:14:0[:N[.2f]]} | {:14:0[:N[.2f]]}\n", iteration,
                                        gain, measured, current, next);
}

int main()
{
  using namespace mp_units::si::unit_symbols;
  using state = kalman::system_state<quantity_point<isq::mass[g]>>;
  using qp = quantity_point<isq::mass[g]>;

  const state initial_guess{qp{1 * kg}};
  const std::array measurements = {qp{996 * g},  qp{994 * g}, qp{1021 * g}, qp{1000 * g}, qp{1002 * g},
                                   qp{1010 * g}, qp{983 * g}, qp{971 * g},  qp{993 * g},  qp{1023 * g}};

  print_header(initial_guess);
  state next = initial_guess;
  for (int index = 1; const auto& measurement : measurements) {
    const state& previous = next;
    const quantity gain = 1. / index * one;
    const state current = state_update(previous, measurement, gain);
    next = current;
    print(index++, gain, measurement, current, next);
  }
}
