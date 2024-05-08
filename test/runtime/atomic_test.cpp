// The MIT License (MIT)
//
// Copyright (c) 2024 Nick Thompson
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

#include <catch2/catch_test_macros.hpp>
#include <atomic>
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/ostream.h>  // IWYU pragma: keep
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si.h>
#endif

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

TEST_CASE("std::atomic works with dimensioned types", "[atomic][assignment]")
{
  const std::atomic<quantity<isq::area[m2]>> a1 = 3.0 * isq::area[m2];
  const std::atomic<quantity<isq::area[m2]>> a2 = 3.0 * isq::area[m2];
  REQUIRE(a1.load() == a2.load());
}
