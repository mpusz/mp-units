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

// Based on: https://www.kalmanfilter.net/alphabeta.html#ex4

template<class T>
  requires mp_units::is_scalar<T>
inline constexpr bool mp_units::is_vector<T> = true;

using namespace mp_units;

void print_header(const kalman::SystemState auto& initial)
{
  std::cout << MP_UNITS_STD_FMT::format("Initial: {}\n", initial);
  std::cout << MP_UNITS_STD_FMT::format("{:>2} | {:>8} | {:>35} | {:>35}\n", "N", "Measured", "Curr. Estimate",
                                        "Next Estimate");
}

void print(auto iteration, QuantityPoint auto measured, const kalman::SystemState auto& current,
           const kalman::SystemState auto& next)
{
  std::cout << MP_UNITS_STD_FMT::format(
    "{:2} | {:8} | {:35:0[:N[.2f]]1[:N[.2f]]2[:N[.2f]]} | {:35:0[:N[.2f]]1[:N[.2f]]2[:N[.2f]]}\n", iteration, measured,
    current, next);
}

int main()
{
  using namespace mp_units::si::unit_symbols;
  using qp = quantity_point<isq::position_vector[m]>;
  using state =
    kalman::system_state<qp, quantity_point<isq::velocity[m / s]>, quantity_point<isq::acceleration[m / s2]>>;

  const quantity interval = isq::duration(5. * s);
  const state initial{qp{30 * km}, quantity_point{50 * m / s}, quantity_point{0 * m / s2}};
  const std::array measurements = {qp{30'221 * m}, qp{30'453 * m}, qp{30'906 * m}, qp{30'999 * m}, qp{31'368 * m},
                                   qp{31'978 * m}, qp{32'526 * m}, qp{33'379 * m}, qp{34'698 * m}, qp{36'275 * m}};
  std::array gain = {0.5 * one, 0.4 * one, 0.1 * one};

  print_header(initial);
  state next = state_extrapolation(initial, interval);
  for (int index = 1; const auto& measurement : measurements) {
    const state& previous = next;
    const state current = state_update(previous, measurement, gain, interval);
    next = state_extrapolation(current, interval);
    print(index++, measurement, current, next);
  }
}
