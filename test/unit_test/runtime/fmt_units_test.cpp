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
    CHECK(fmt::format("{}", 1ns) == "1 ns");
    CHECK(fmt::format("{}", 1us) == "1 µs");
    CHECK(fmt::format("{}", 1ms) == "1 ms");
  }

  SECTION("length")
  {
    CHECK(fmt::format("{}", 1mm) == "1 mm");
    CHECK(fmt::format("{}", 1cm) == "1 cm");
    CHECK(fmt::format("{}", 1km) == "1 km");
  }

  SECTION("mass")
  {
    CHECK(fmt::format("{}", 1kg) == "1 kg");
  }

  SECTION("area")
  {
    CHECK(fmt::format("{}", 1sq_m) == "1 m²");
    CHECK(fmt::format("{}", 1sq_mm) == "1 mm²");
    CHECK(fmt::format("{}", 1sq_cm) == "1 cm²");
    CHECK(fmt::format("{}", 1sq_km) == "1 km²");
    CHECK(fmt::format("{}", 1sq_ft) == "1 ft²");
  }

  SECTION("volume")
  {
    CHECK(fmt::format("{}", 1cub_m) == "1 m³");
    CHECK(fmt::format("{}", 1cub_mm) == "1 mm³");
    CHECK(fmt::format("{}", 1cub_cm) == "1 cm³");
    CHECK(fmt::format("{}", 1cub_km) == "1 km³");
    CHECK(fmt::format("{}", 1cub_ft) == "1 ft³");
  }

  SECTION("frequency")
  {
    CHECK(fmt::format("{}", 1mHz) == "1 mHz");
    CHECK(fmt::format("{}", 1kHz) == "1 kHz");
    CHECK(fmt::format("{}", 1MHz) == "1 MHz");
    CHECK(fmt::format("{}", 1GHz) == "1 GHz");
    CHECK(fmt::format("{}", 1THz) == "1 THz");
  }

  SECTION("velocity")
  {
    CHECK(fmt::format("{}", 1mps) == "1 m/s");
    CHECK(fmt::format("{}", 1kmph) == "1 km/h");
    CHECK(fmt::format("{}", 1mph) == "1 mi/h");
  }

  SECTION("acceleration")
  {
    CHECK(fmt::format("{}", 1mps_sq) == "1 m/s²");
  }

  SECTION("energy")
  {
    CHECK(fmt::format("{}", 1mJ) == "1 mJ");
    CHECK(fmt::format("{}", 1kJ) == "1 kJ");
    CHECK(fmt::format("{}", 1MJ) == "1 MJ");
    CHECK(fmt::format("{}", 1GJ) == "1 GJ");
  }

  SECTION("power")
  {
    CHECK(fmt::format("{}", 1mW) == "1 mW");
    CHECK(fmt::format("{}", 1kW) == "1 kW");
    CHECK(fmt::format("{}", 1MW) == "1 MW");
    CHECK(fmt::format("{}", 1GW) == "1 GW");
  }

  SECTION("surface tension")
  {
    CHECK(fmt::format("{}", 1Npm) == "1 N/m");
  }
}
