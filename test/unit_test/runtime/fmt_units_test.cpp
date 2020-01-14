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

#include "units/physical/si/area.h"
#include "units/physical/si/frequency.h"
#include "units/physical/si/power.h"
#include "units/physical/si/velocity.h"
#include "units/physical/si/volume.h"
#include "units/physical/si/surface_tension.h"
#include "units/physical/us/length.h"
#include "units/physical/imperial/length.h"
#include "units/physical/international/length.h"
#include "units/physical/international/area.h"
#include "units/physical/international/volume.h"
#include "units/physical/international/velocity.h"
#include "units/physical/iau/length.h"
#include "units/physical/typographic/length.h"
#include "units/physical/si/density.h"
#include "units/physical/si/resistance.h"
#include "units/physical/si/voltage.h"
#include "units/format.h"
#include <catch2/catch.hpp>

using namespace units::si;
using namespace units::international;
using namespace units::us;
using namespace units::iau;
using namespace units::imperial;
using namespace units::typographic;

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
    CHECK(fmt::format("{}", 1ft) == "1 ft");
    CHECK(fmt::format("{}", 1ft_us) == "1 ft(us)");
    CHECK(fmt::format("{}", 1yd) == "1 yd");
    CHECK(fmt::format("{}", 1in) == "1 in");
    CHECK(fmt::format("{}", 1fathom) == "1 fathom");
    CHECK(fmt::format("{}", 1fathom_us) == "1 fathom(us)");
    CHECK(fmt::format("{}", 1mi) == "1 mi");
    CHECK(fmt::format("{}", 1mi_us) == "1 mi(us)");
    CHECK(fmt::format("{}", 1naut_mi) == "1 mi(naut)");
    CHECK(fmt::format("{}", 1ch) == "1 ch");
    CHECK(fmt::format("{}", 1rd) == "1 rd");
    CHECK(fmt::format("{}", 1thou) == "1 thou");
    CHECK(fmt::format("{}", 1pc) == "1 pc");
    CHECK(fmt::format("{}", 1ly) == "1 ly");
    CHECK(fmt::format("{}", 1pc) == "1 pc");
    CHECK(fmt::format("{}", 1angstrom) == "1 angstrom");
    CHECK(fmt::format("{}", 1au) == "1 au");
    CHECK(fmt::format("{}", 1pica_comp) == "1 pica(comp)");
    CHECK(fmt::format("{}", 1pica_prn) == "1 pica(prn)");
    CHECK(fmt::format("{}", 1point_comp) == "1 point(comp)");
    CHECK(fmt::format("{}", 1point_prn) == "1 point(prn)");
  }

  SECTION("mass")
  {
    CHECK(fmt::format("{}", 1kg) == "1 kg");
  }

  SECTION("area")
  {
    CHECK(fmt::format("{}", 1m2) == "1 m²");
    CHECK(fmt::format("{}", 1mm2) == "1 mm²");
    CHECK(fmt::format("{}", 1cm2) == "1 cm²");
    CHECK(fmt::format("{}", 1km2) == "1 km²");
    CHECK(fmt::format("{}", 1ft2) == "1 ft²");
  }

  SECTION("density")
  {
      CHECK(fmt::format("{}", 1kgpm3) == "1 kg/m³");
  }

  SECTION("resistance")
  {
      CHECK(fmt::format("{}", 1_R) == "1 Ω");
      CHECK(fmt::format("{}", 1kR) == "1 kΩ");
      CHECK(fmt::format("{}", 1mR) == "1 mΩ");
      CHECK(fmt::format("{}", 1MR) == "1 MΩ");
  }

  SECTION("voltage")
  {
     CHECK(fmt::format("{}", 1V) == "1 V");
     CHECK(fmt::format("{}", 1mV) == "1 mV");
     CHECK(fmt::format("{}", 1uV) == "1 µV");
     CHECK(fmt::format("{}", 1nV) == "1 nV");
     CHECK(fmt::format("{}", 1pV) == "1 pV");
  }

  SECTION("volume")
  {
    CHECK(fmt::format("{}", 1m3) == "1 m³");
    CHECK(fmt::format("{}", 1mm3) == "1 mm³");
    CHECK(fmt::format("{}", 1cm3) == "1 cm³");
    CHECK(fmt::format("{}", 1km3) == "1 km³");
    CHECK(fmt::format("{}", 1ft3) == "1 ft³");
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
    CHECK(fmt::format("{}", 1mps2) == "1 m/s²");
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
