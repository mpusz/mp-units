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

#include "units/physical/si.h"
#include "units/physical/us/length.h"
#include "units/physical/imperial/length.h"
#include "units/physical/international/length.h"
#include "units/physical/international/area.h"
#include "units/physical/international/volume.h"
#include "units/physical/international/velocity.h"
#include "units/physical/iau/length.h"
#include "units/physical/typographic/length.h"
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
    CHECK(fmt::format("{}", 1q_ns) == "1 ns");
    CHECK(fmt::format("{}", 1q_us) == "1 µs");
    CHECK(fmt::format("{}", 1q_ms) == "1 ms");

    CHECK(fmt::format("{:%Q %Aq}", 1q_us) == "1 us");
  }

  SECTION("length")
  {
    CHECK(fmt::format("{}", 1q_mm) == "1 mm");
    CHECK(fmt::format("{}", 1q_cm) == "1 cm");
    CHECK(fmt::format("{}", 1q_km) == "1 km");
    CHECK(fmt::format("{}", 1q_ft) == "1 ft");
    CHECK(fmt::format("{}", 1q_ft_us) == "1 ft(us)");
    CHECK(fmt::format("{}", 1q_yd) == "1 yd");
    CHECK(fmt::format("{}", 1q_in) == "1 in");
    CHECK(fmt::format("{}", 1q_fathom) == "1 fathom");
    CHECK(fmt::format("{}", 1q_fathom_us) == "1 fathom(us)");
    CHECK(fmt::format("{}", 1q_mi) == "1 mi");
    CHECK(fmt::format("{}", 1q_mi_us) == "1 mi(us)");
    CHECK(fmt::format("{}", 1q_naut_mi) == "1 mi(naut)");
    CHECK(fmt::format("{}", 1q_ch) == "1 ch");
    CHECK(fmt::format("{}", 1q_rd) == "1 rd");
    CHECK(fmt::format("{}", 1q_thou) == "1 thou");
    CHECK(fmt::format("{}", 1q_pc) == "1 pc");
    CHECK(fmt::format("{}", 1q_ly) == "1 ly");
    CHECK(fmt::format("{}", 1q_pc) == "1 pc");
    CHECK(fmt::format("{}", 1q_angstrom) == "1 angstrom");
    CHECK(fmt::format("{}", 1q_au) == "1 au");
    CHECK(fmt::format("{}", 1q_pica_comp) == "1 pica(comp)");
    CHECK(fmt::format("{}", 1q_pica_prn) == "1 pica(prn)");
    CHECK(fmt::format("{}", 1q_point_comp) == "1 point(comp)");
    CHECK(fmt::format("{}", 1q_point_prn) == "1 point(prn)");
  }

  SECTION("mass")
  {
    CHECK(fmt::format("{}", 1q_kg) == "1 kg");
  }

  SECTION("area")
  {
    CHECK(fmt::format("{}", 1q_m2) == "1 m²");
    CHECK(fmt::format("{}", 1q_mm2) == "1 mm²");
    CHECK(fmt::format("{}", 1q_cm2) == "1 cm²");
    CHECK(fmt::format("{}", 1q_km2) == "1 km²");
    CHECK(fmt::format("{}", 1q_ft2) == "1 ft²");

    CHECK(fmt::format("{:%Q %Aq}", 1q_m2) == "1 m^2");
    CHECK(fmt::format("{:%Q %Aq}", 1q_mm2) == "1 mm^2");
    CHECK(fmt::format("{:%Q %Aq}", 1q_cm2) == "1 cm^2");
    CHECK(fmt::format("{:%Q %Aq}", 1q_km2) == "1 km^2");
    CHECK(fmt::format("{:%Q %Aq}", 1q_ft2) == "1 ft^2");
  }

  SECTION("density")
  {
      CHECK(fmt::format("{}", 1q_kg_per_m3) == "1 kg/m³");
      CHECK(fmt::format("{:%Q %Aq}", 1q_kg_per_m3) == "1 kg/m^3");
  }

  SECTION("resistance")
  {
    CHECK(fmt::format("{}", 1q_R) == "1 Ω");
    CHECK(fmt::format("{}", 1q_kR) == "1 kΩ");
    CHECK(fmt::format("{}", 1q_mR) == "1 mΩ");
    CHECK(fmt::format("{}", 1q_MR) == "1 MΩ");

    CHECK(fmt::format("{:%Q %Aq}", 1q_R) == "1 ohm");
    CHECK(fmt::format("{:%Q %Aq}", 1q_kR) == "1 kohm");
    CHECK(fmt::format("{:%Q %Aq}", 1q_mR) == "1 mohm");
    CHECK(fmt::format("{:%Q %Aq}", 1q_MR) == "1 Mohm");
  }

  SECTION("voltage")
  {
     CHECK(fmt::format("{}", 1q_V) == "1 V");
     CHECK(fmt::format("{}", 1q_mV) == "1 mV");
     CHECK(fmt::format("{}", 1q_uV) == "1 µV");
     CHECK(fmt::format("{}", 1q_nV) == "1 nV");
     CHECK(fmt::format("{}", 1q_pV) == "1 pV");

     CHECK(fmt::format("{:%Q %Aq}", 1q_uV) == "1 uV");
  }

  SECTION("volume")
  {
    CHECK(fmt::format("{}", 1q_m3) == "1 m³");
    CHECK(fmt::format("{}", 1q_mm3) == "1 mm³");
    CHECK(fmt::format("{}", 1q_cm3) == "1 cm³");
    CHECK(fmt::format("{}", 1q_km3) == "1 km³");
    CHECK(fmt::format("{}", 1q_ft3) == "1 ft³");

    CHECK(fmt::format("{:%Q %Aq}", 1q_m3) == "1 m^3");
    CHECK(fmt::format("{:%Q %Aq}", 1q_mm3) == "1 mm^3");
    CHECK(fmt::format("{:%Q %Aq}", 1q_cm3) == "1 cm^3");
    CHECK(fmt::format("{:%Q %Aq}", 1q_km3) == "1 km^3");
    CHECK(fmt::format("{:%Q %Aq}", 1q_ft3) == "1 ft^3");
  }

  SECTION("frequency")
  {
    CHECK(fmt::format("{}", 1q_mHz) == "1 mHz");
    CHECK(fmt::format("{}", 1q_kHz) == "1 kHz");
    CHECK(fmt::format("{}", 1q_MHz) == "1 MHz");
    CHECK(fmt::format("{}", 1q_GHz) == "1 GHz");
    CHECK(fmt::format("{}", 1q_THz) == "1 THz");
  }

  SECTION("velocity")
  {
    CHECK(fmt::format("{}", 1q_m_per_s) == "1 m/s");
    CHECK(fmt::format("{}", 1q_km_per_h) == "1 km/h");
    CHECK(fmt::format("{}", 1q_mi_per_h) == "1 mi/h");
  }

  SECTION("acceleration")
  {
    CHECK(fmt::format("{}", 1q_m_per_s2) == "1 m/s²");
    CHECK(fmt::format("{:%Q %Aq}", 1q_m_per_s2) == "1 m/s^2");
  }

  SECTION("momentum")
  {
    CHECK(fmt::format("{}", 1q_kg_m_per_s) == "1 kg ⋅ m/s");
    CHECK(fmt::format("{:%Q %Aq}", 1q_kg_m_per_s) == "1 kg m/s");
  }

  SECTION("energy")
  {
    CHECK(fmt::format("{}", 1q_mJ) == "1 mJ");
    CHECK(fmt::format("{}", 1q_kJ) == "1 kJ");
    CHECK(fmt::format("{}", 1q_MJ) == "1 MJ");
    CHECK(fmt::format("{}", 1q_GJ) == "1 GJ");
  }

  SECTION("power")
  {
    CHECK(fmt::format("{}", 1q_mW) == "1 mW");
    CHECK(fmt::format("{}", 1q_kW) == "1 kW");
    CHECK(fmt::format("{}", 1q_MW) == "1 MW");
    CHECK(fmt::format("{}", 1q_GW) == "1 GW");
  }

  SECTION("surface tension")
  {
    CHECK(fmt::format("{}", 1q_N_per_m) == "1 N/m");
  }

  SECTION("magnetic induction")
  {
    CHECK(fmt::format("{}", 1q_T) == "1 T");
  }

  SECTION("magnetic flux")
  {
    CHECK(fmt::format("{}", 1q_Wb) == "1 Wb");
    CHECK(fmt::format("{}", 1q_G) == "1 G");
  }

  SECTION("inductance")
  {
    CHECK(fmt::format("{}", 1q_H) == "1 H");
    CHECK(fmt::format("{}", 1q_mH) == "1 mH");
  }

  SECTION("conductance")
  {
    CHECK(fmt::format("{}", 1q_S) == "1 S");
    CHECK(fmt::format("{}", 1q_nS) == "1 nS");
  }

  SECTION("catalytic activity")
  {
    CHECK(fmt::format("{}", 1q_kat) == "1 kat");
    CHECK(fmt::format("{}", 1q_U) == "1 U");
  }

  SECTION("absorbed dose")
  {
    CHECK(fmt::format("{}", 1q_Gy) == "1 Gy");
    CHECK(fmt::format("{}", 1q_kGy) == "1 kGy");
    CHECK(fmt::format("{}", 1q_mGy) == "1 mGy");
  }

  SECTION("addition with common ratio")
  {
    CHECK(fmt::format("{}", 1q_in + 1q_yd) == "37 in");
  }

  SECTION("current density")
  {
    CHECK(fmt::format("{}", 1q_A_per_m2) == "1 A/m²");
    CHECK(fmt::format("{:%Q %Aq}", 1q_A_per_m2) == "1 A/m^2");
  }

  SECTION("concentration")
  {
    CHECK(fmt::format("{}", 1q_mol_per_m3) == "1 mol/m³");
    CHECK(fmt::format("{:%Q %Aq}", 1q_mol_per_m3) == "1 mol/m^3");
  }

  SECTION("luminance")
  {
    CHECK(fmt::format("{}", 1q_cd_per_m2) == "1 cd/m²");
    CHECK(fmt::format("{:%Q %Aq}", 1q_cd_per_m2) == "1 cd/m^2");
  }

  SECTION("dynamic viscosity")
  {
    CHECK(fmt::format("{}", 1q_Pa_s) == "1 Pa ⋅ s");
    CHECK(fmt::format("{:%Q %Aq}", 1q_Pa_s) == "1 Pa s");
  }

  SECTION("heat capacity")
  {
    CHECK(fmt::format("{}", 1q_J_per_K) == "1 J/K");
  }

  SECTION("specific heat capacity")
  {
    CHECK(fmt::format("{}", 1q_J_per_kg_K) == "1 J ⋅ K⁻¹ ⋅ kg⁻¹");
    CHECK(fmt::format("{:%Q %Aq}", 1q_J_per_kg_K) == "1 J K^-1 kg^-1");
  }

  SECTION("thermal conductivity")
  {
    CHECK(fmt::format("{}", 1q_W_per_m_K) == "1 W ⋅ m⁻¹ ⋅ K⁻¹");
    CHECK(fmt::format("{:%Q %Aq}", 1q_W_per_m_K) == "1 W m^-1 K^-1");
  }

  SECTION("electric field strength")
  {
    CHECK(fmt::format("{}", 1q_V_per_m) == "1 V/m");
  }

  SECTION("incoherent units with powers")
  {
    CHECK(fmt::format("{}", 1q_mi * 1q_mi * 1q_mi) == "1 [15900351812136/3814697265625 × 10⁹] m³");
    CHECK(fmt::format("{}", 1q_au * 1q_au) == "1 [2237952291797391849 × 10⁴] m²");

    CHECK(fmt::format("{:%Q %Aq}", 1q_mi * 1q_mi * 1q_mi) == "1 [15900351812136/3814697265625 x 10^9] m^3");
    CHECK(fmt::format("{:%Q %Aq}", 1q_au * 1q_au) == "1 [2237952291797391849 x 10^4] m^2");
  }

  SECTION("unknown scaled unit with reference different than the dimension's coherent unit")
  {
    CHECK(fmt::format("{}", mass<units::scaled_unit<units::ratio<2, 3>, gram>>(1)) == "1 [2/3 × 10⁻³] kg");
    CHECK(fmt::format("{:%Q %Aq}", mass<units::scaled_unit<units::ratio<2, 3>, gram>>(1)) == "1 [2/3 x 10^-3] kg");
  }
}
