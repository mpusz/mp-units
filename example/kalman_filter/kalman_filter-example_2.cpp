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

// Based on: https://www.kalmanfilter.net/alphabeta.html#ex2

template<class T>
  requires mp_units::is_scalar<T>
inline constexpr bool mp_units::is_vector<T> = true;

using namespace mp_units;

void print_header(const kalman::SystemState auto& initial)
{
  std::cout << MP_UNITS_STD_FMT::format("Initial: {}\n", initial);
  std::cout << MP_UNITS_STD_FMT::format("{:>2} | {:>8} | {:>23} | {:>23}\n", "N", "Measured", "Curr. Estimate",
                                        "Next Estimate");
}

void print(auto iteration, QuantityPoint auto measured, const kalman::SystemState auto& current,
           const kalman::SystemState auto& next)
{
  std::cout << MP_UNITS_STD_FMT::vformat("{:2} | {:8} | {:23:0[:N[.2f]]1[:N[.2f]]} | {:23:0[:N[.2f]]1[:N[.2f]]}\n",
                                         MP_UNITS_STD_FMT::make_format_args(iteration, measured, current, next));
}

int main()
{
  using namespace mp_units::si::unit_symbols;
  using qp = quantity_point<isq::position_vector[m]>;
  using state = kalman::system_state<qp, quantity_point<isq::velocity[m / s]>>;

  const quantity interval = isq::duration(5 * s);
  const state initial{qp{30 * km}, quantity_point{40 * m / s}};
  const std::array measurements = {qp{30'171 * m}, qp{30'353 * m}, qp{30'756 * m}, qp{30'799 * m}, qp{31'018 * m},
                                   qp{31'278 * m}, qp{31'276 * m}, qp{31'379 * m}, qp{31'748 * m}, qp{32'175 * m}};
  std::array gain = {0.2 * one, 0.1 * one};

  print_header(initial);
  state next = state_extrapolation(initial, interval);
  for (int index = 1; const auto& measurement : measurements) {
    const state& previous = next;
    const state current = state_update(previous, measurement, gain, interval);
    next = state_extrapolation(current, interval);
    print(index++, measurement, current, next);
  }
}
