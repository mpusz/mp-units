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

#include <mp-units/compat_macros.h>

#if MP_UNITS_HOSTED
#include <mp-units/ext/format.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <iostream>
#endif
#endif

#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#endif

using namespace mp_units;

constexpr QuantityOf<MP_UNITS_IS_VALUE_WORKAROUND(isq::speed)> auto avg_speed(
  QuantityOf<MP_UNITS_IS_VALUE_WORKAROUND(isq::distance)> auto d,
  QuantityOf<MP_UNITS_IS_VALUE_WORKAROUND(isq::duration)> auto t)
{
  return d / t;
}

int main()
{
  using namespace mp_units::si::unit_symbols;
#if MP_UNITS_HOSTED
  std::cout << MP_UNITS_STD_FMT::format("Average speed = {}\n", avg_speed(240 * km, 2 * h));
#else
  [[maybe_unused]] auto value = avg_speed(240 * km, 2 * h);
#endif
}
