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
#include "units/dimensions/surface_tension.h"
#include "units/format.h"
#include <catch2/catch.hpp>

using namespace units;

TEST_CASE("fmt::format on synthesized unit symbols", "[text][fmt]")
{
  SECTION("time")
  {
    REQUIRE(fmt::format("{}", 1ns) == "1 ns");
    REQUIRE(fmt::format("{}", 1us) == "1 µs");
    REQUIRE(fmt::format("{}", 1ms) == "1 ms");
  }

  SECTION("length")
  {
    REQUIRE(fmt::format("{}", 1mm) == "1 mm");
    REQUIRE(fmt::format("{}", 1cm) == "1 cm");
    REQUIRE(fmt::format("{}", 1km) == "1 km");
  }

  SECTION("mass")
  {
    REQUIRE(fmt::format("{}", 1kg) == "1 kg");
  }

  SECTION("area")
  {
    REQUIRE(fmt::format("{}", 1sq_m) == "1 m²");
    REQUIRE(fmt::format("{}", 1sq_mm) == "1 mm²");
    REQUIRE(fmt::format("{}", 1sq_cm) == "1 cm²");
    REQUIRE(fmt::format("{}", 1sq_km) == "1 km²");
    REQUIRE(fmt::format("{}", 1sq_ft) == "1 ft²");
  }

  SECTION("volume")
  {
    REQUIRE(fmt::format("{}", 1cub_m) == "1 m³");
    REQUIRE(fmt::format("{}", 1cub_mm) == "1 mm³");
    REQUIRE(fmt::format("{}", 1cub_cm) == "1 cm³");
    REQUIRE(fmt::format("{}", 1cub_km) == "1 km³");
    REQUIRE(fmt::format("{}", 1cub_ft) == "1 ft³");
  }

  SECTION("frequency")
  {
    REQUIRE(fmt::format("{}", 1mHz) == "1 mHz");
    REQUIRE(fmt::format("{}", 1kHz) == "1 kHz");
    REQUIRE(fmt::format("{}", 1MHz) == "1 MHz");
    REQUIRE(fmt::format("{}", 1GHz) == "1 GHz");
    REQUIRE(fmt::format("{}", 1THz) == "1 THz");
  }

  SECTION("velocity")
  {
    REQUIRE(fmt::format("{}", 1mps) == "1 m/s");
    REQUIRE(fmt::format("{}", 1kmph) == "1 km/h");
    REQUIRE(fmt::format("{}", 1mph) == "1 mi/h");
  }

  SECTION("acceleration")
  {
    REQUIRE(fmt::format("{}", 1mps_sq) == "1 m/s²");
  }

  SECTION("energy")
  {
    REQUIRE(fmt::format("{}", 1mJ) == "1 mJ");
    REQUIRE(fmt::format("{}", 1kJ) == "1 kJ");
    REQUIRE(fmt::format("{}", 1MJ) == "1 MJ");
    REQUIRE(fmt::format("{}", 1GJ) == "1 GJ");
  }

  SECTION("power")
  {
    REQUIRE(fmt::format("{}", 1mW) == "1 mW");
    REQUIRE(fmt::format("{}", 1kW) == "1 kW");
    REQUIRE(fmt::format("{}", 1MW) == "1 MW");
    REQUIRE(fmt::format("{}", 1GW) == "1 GW");
  }

  SECTION("surface tension")
  {
    REQUIRE(fmt::format("{}", 1Npm) == "1 N/m");
  }
}
