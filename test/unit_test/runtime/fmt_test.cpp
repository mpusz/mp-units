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

#include "units/dimensions/area.h"
#include "units/dimensions/frequency.h"
#include "units/dimensions/power.h"
#include "units/dimensions/velocity.h"
#include "units/dimensions/volume.h"
#include "units/math.h"
#include <catch2/catch.hpp>
#include <fmt/locale.h>

using namespace units;

TEST_CASE("fmt on a quantity", "[text][fmt]")
{
  SECTION("quantity with a predefined unit")
  {
    std::locale ru_loc("ru_RU.UTF-8");
    REQUIRE(fmt::format(ru_loc, "{}", 2sq_m) == "2 м²");
    REQUIRE(fmt::format("{}", 2GHz) == "2 GHz");
  }
}