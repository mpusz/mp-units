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

#include <catch2/catch.hpp>
#include <units/bits/external/hacks.h>  // IWYU pragma: keep
#include <units/format.h>
#include <units/isq/iec80000/iec80000.h>
#include <units/isq/si/iau/iau.h>
#include <units/isq/si/imperial/imperial.h>
#include <units/isq/si/international/international.h>
#include <units/isq/si/si.h>
#include <units/isq/si/typographic/typographic.h>
#include <units/isq/si/uscs/uscs.h>
#include <units/magnitude.h>

using namespace units::isq::si;
using namespace units::isq::si::references;
using namespace units::isq::si::international;
using namespace units::isq::si::uscs;
using namespace units::isq::si::iau;
using namespace units::isq::si::imperial;
using namespace units::isq::si::imperial::references;
using namespace units::isq::si::typographic;
using namespace units::isq::iec80000::references;

TEST_CASE("std::format on synthesized unit symbols", "[text][fmt]")
{
  SECTION("time")
  {
    CHECK(STD_FMT::format("{}", 1_q_ns) == "1 ns");
    CHECK(STD_FMT::format("{}", 1_q_us) == "1 µs");
    CHECK(STD_FMT::format("{}", 1_q_ms) == "1 ms");

    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_us) == "1 us");
  }

  SECTION("length")
  {
    CHECK(STD_FMT::format("{}", 1_q_mm) == "1 mm");
    CHECK(STD_FMT::format("{}", 1_q_cm) == "1 cm");
    CHECK(STD_FMT::format("{}", 1_q_km) == "1 km");
    CHECK(STD_FMT::format("{}", 1 * ft) == "1 ft");
    CHECK(STD_FMT::format("{}", 1_q_ft_us) == "1 ft(us)");
    CHECK(STD_FMT::format("{}", 1_q_yd) == "1 yd");
    CHECK(STD_FMT::format("{}", 1_q_in) == "1 in");
    CHECK(STD_FMT::format("{}", 1_q_fathom) == "1 fathom");
    CHECK(STD_FMT::format("{}", 1_q_fathom_us) == "1 fathom(us)");
    CHECK(STD_FMT::format("{}", 1_q_mi) == "1 mi");
    CHECK(STD_FMT::format("{}", 1_q_mi_us) == "1 mi(us)");
    CHECK(STD_FMT::format("{}", 1_q_naut_mi) == "1 nmi");
    CHECK(STD_FMT::format("{}", 1_q_ch) == "1 ch");
    CHECK(STD_FMT::format("{}", 1_q_rd) == "1 rd");
    CHECK(STD_FMT::format("{}", 1_q_thou) == "1 thou");
    CHECK(STD_FMT::format("{}", 1_q_pc) == "1 pc");
    CHECK(STD_FMT::format("{}", 1_q_ly) == "1 ly");
    CHECK(STD_FMT::format("{}", 1_q_pc) == "1 pc");
    CHECK(STD_FMT::format("{}", 1_q_angstrom) == "1 angstrom");
    CHECK(STD_FMT::format("{}", 1_q_au) == "1 au");
    CHECK(STD_FMT::format("{}", 1_q_pica_comp) == "1 pica(comp)");
    CHECK(STD_FMT::format("{}", 1_q_pica_prn) == "1 pica(prn)");
    CHECK(STD_FMT::format("{}", 1_q_point_comp) == "1 point(comp)");
    CHECK(STD_FMT::format("{}", 1_q_point_prn) == "1 point(prn)");
  }

  SECTION("mass") { CHECK(STD_FMT::format("{}", 1_q_kg) == "1 kg"); }

  SECTION("area")
  {
    CHECK(STD_FMT::format("{}", 1_q_m2) == "1 m²");
    CHECK(STD_FMT::format("{}", 1_q_mm2) == "1 mm²");
    CHECK(STD_FMT::format("{}", 1_q_cm2) == "1 cm²");
    CHECK(STD_FMT::format("{}", 1_q_km2) == "1 km²");
    CHECK(STD_FMT::format("{}", 1_q_ft2) == "1 ft²");

    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_m2) == "1 m^2");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_mm2) == "1 mm^2");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_cm2) == "1 cm^2");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_km2) == "1 km^2");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_ft2) == "1 ft^2");
  }

  SECTION("density")
  {
    CHECK(STD_FMT::format("{}", 1_q_kg_per_m3) == "1 kg/m³");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_kg_per_m3) == "1 kg/m^3");
  }

  SECTION("resistance")
  {
    CHECK(STD_FMT::format("{}", 1_q_R) == "1 Ω");
    CHECK(STD_FMT::format("{}", 1_q_kR) == "1 kΩ");
    CHECK(STD_FMT::format("{}", 1_q_mR) == "1 mΩ");
    CHECK(STD_FMT::format("{}", 1_q_MR) == "1 MΩ");

    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_R) == "1 ohm");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_kR) == "1 kohm");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_mR) == "1 mohm");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_MR) == "1 Mohm");
  }

  SECTION("voltage")
  {
    CHECK(STD_FMT::format("{}", 1_q_V) == "1 V");
    CHECK(STD_FMT::format("{}", 1_q_mV) == "1 mV");
    CHECK(STD_FMT::format("{}", 1_q_uV) == "1 µV");
    CHECK(STD_FMT::format("{}", 1_q_nV) == "1 nV");
    CHECK(STD_FMT::format("{}", 1_q_pV) == "1 pV");

    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_uV) == "1 uV");
  }

  SECTION("volume")
  {
    CHECK(STD_FMT::format("{}", 1_q_m3) == "1 m³");
    CHECK(STD_FMT::format("{}", 1_q_mm3) == "1 mm³");
    CHECK(STD_FMT::format("{}", 1_q_cm3) == "1 cm³");
    CHECK(STD_FMT::format("{}", 1_q_km3) == "1 km³");
    CHECK(STD_FMT::format("{}", 1_q_ft3) == "1 ft³");

    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_m3) == "1 m^3");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_mm3) == "1 mm^3");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_cm3) == "1 cm^3");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_km3) == "1 km^3");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_ft3) == "1 ft^3");
  }

  SECTION("frequency")
  {
    CHECK(STD_FMT::format("{}", 1_q_mHz) == "1 mHz");
    CHECK(STD_FMT::format("{}", 1_q_kHz) == "1 kHz");
    CHECK(STD_FMT::format("{}", 1_q_MHz) == "1 MHz");
    CHECK(STD_FMT::format("{}", 1_q_GHz) == "1 GHz");
    CHECK(STD_FMT::format("{}", 1_q_THz) == "1 THz");
  }

  SECTION("speed")
  {
    CHECK(STD_FMT::format("{}", 1_q_m_per_s) == "1 m/s");
    CHECK(STD_FMT::format("{}", 1_q_km_per_h) == "1 km/h");
    CHECK(STD_FMT::format("{}", 1_q_mi_per_h) == "1 mi/h");
  }

  SECTION("acceleration")
  {
    CHECK(STD_FMT::format("{}", 1_q_m_per_s2) == "1 m/s²");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_m_per_s2) == "1 m/s^2");
  }

  SECTION("momentum")
  {
    CHECK(STD_FMT::format("{}", 1_q_kg_m_per_s) == "1 kg ⋅ m/s");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_kg_m_per_s) == "1 kg m/s");
  }

  SECTION("energy")
  {
    CHECK(STD_FMT::format("{}", 1_q_mJ) == "1 mJ");
    CHECK(STD_FMT::format("{}", 1_q_kJ) == "1 kJ");
    CHECK(STD_FMT::format("{}", 1_q_MJ) == "1 MJ");
    CHECK(STD_FMT::format("{}", 1_q_GJ) == "1 GJ");
  }

  SECTION("power")
  {
    CHECK(STD_FMT::format("{}", 1_q_mW) == "1 mW");
    CHECK(STD_FMT::format("{}", 1_q_kW) == "1 kW");
    CHECK(STD_FMT::format("{}", 1_q_MW) == "1 MW");
    CHECK(STD_FMT::format("{}", 1_q_GW) == "1 GW");
  }

  SECTION("surface tension") { CHECK(STD_FMT::format("{}", 1_q_N_per_m) == "1 N/m"); }

  SECTION("magnetic induction") { CHECK(STD_FMT::format("{}", 1_q_T) == "1 T"); }

  SECTION("magnetic flux")
  {
    CHECK(STD_FMT::format("{}", 1_q_Wb) == "1 Wb");
    CHECK(STD_FMT::format("{}", 1_q_G) == "1 G");
  }

  SECTION("inductance")
  {
    CHECK(STD_FMT::format("{}", 1_q_H) == "1 H");
    CHECK(STD_FMT::format("{}", 1_q_mH) == "1 mH");
  }

  SECTION("conductance")
  {
    CHECK(STD_FMT::format("{}", 1_q_S) == "1 S");
    CHECK(STD_FMT::format("{}", 1_q_nS) == "1 nS");
  }

  SECTION("catalytic activity")
  {
    CHECK(STD_FMT::format("{}", 1_q_kat) == "1 kat");
    CHECK(STD_FMT::format("{}", 1_q_U) == "1 U");
  }

  SECTION("absorbed dose")
  {
    CHECK(STD_FMT::format("{}", 1_q_Gy) == "1 Gy");
    CHECK(STD_FMT::format("{}", 1_q_kGy) == "1 kGy");
    CHECK(STD_FMT::format("{}", 1_q_mGy) == "1 mGy");
  }

  SECTION("addition with common ratio") { CHECK(STD_FMT::format("{}", 1_q_in + 1_q_yd) == "37 in"); }

  SECTION("current density")
  {
    CHECK(STD_FMT::format("{}", 1_q_A_per_m2) == "1 A/m²");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_A_per_m2) == "1 A/m^2");
  }

  SECTION("concentration")
  {
    CHECK(STD_FMT::format("{}", 1_q_mol_per_m3) == "1 mol/m³");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_mol_per_m3) == "1 mol/m^3");
  }

  SECTION("luminance")
  {
    CHECK(STD_FMT::format("{}", 1_q_cd_per_m2) == "1 cd/m²");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_cd_per_m2) == "1 cd/m^2");
  }

  SECTION("dynamic viscosity")
  {
    CHECK(STD_FMT::format("{}", 1_q_Pa_s) == "1 Pa ⋅ s");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_Pa_s) == "1 Pa s");
  }

  SECTION("heat capacity") { CHECK(STD_FMT::format("{}", 1_q_J_per_K) == "1 J/K"); }

  SECTION("specific heat capacity")
  {
    CHECK(STD_FMT::format("{}", 1_q_J_per_kg_K) == "1 J ⋅ K⁻¹ ⋅ kg⁻¹");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_J_per_kg_K) == "1 J K^-1 kg^-1");
  }

  SECTION("molar heath capacity")
  {
    CHECK(STD_FMT::format("{}", 1_q_J_per_mol_K) == "1 J ⋅ K⁻¹ ⋅ mol⁻¹");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_J_per_mol_K) == "1 J K^-1 mol^-1");
  }

  SECTION("thermal conductivity")
  {
    CHECK(STD_FMT::format("{}", 1_q_W_per_m_K) == "1 W ⋅ m⁻¹ ⋅ K⁻¹");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_W_per_m_K) == "1 W m^-1 K^-1");
  }

  SECTION("electric field strength") { CHECK(STD_FMT::format("{}", 1_q_V_per_m) == "1 V/m"); }

  SECTION("charge density")
  {
    CHECK(STD_FMT::format("{}", 1_q_C_per_m3) == "1 C/m³");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_C_per_m3) == "1 C/m^3");
    CHECK(STD_FMT::format("{}", 1_q_C_per_m2) == "1 C/m²");
    CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_C_per_m2) == "1 C/m^2");
  }

  SECTION("permittivity") { CHECK(STD_FMT::format("{}", 1_q_F_per_m) == "1 F/m"); }

  SECTION("permeability") { CHECK(STD_FMT::format("{}", 1_q_H_per_m) == "1 H/m"); }

  SECTION("molar energy") { CHECK(STD_FMT::format("{}", 1_q_J_per_mol) == "1 J/mol"); }

  SECTION("torque") { CHECK(STD_FMT::format("{}", 1_q_N_m_per_rad) == "1 N ⋅ m/rad"); }

  SECTION("storage_capacity")
  {
    CHECK(STD_FMT::format("{}", 1 * bit) == "1 bit");
    CHECK(STD_FMT::format("{}", 1 * kbit) == "1 kbit");
    CHECK(STD_FMT::format("{}", 1 * Tibit) == "1 Tibit");
    CHECK(STD_FMT::format("{}", 1 * B) == "1 B");
    CHECK(STD_FMT::format("{}", 1 * kB) == "1 kB");
    CHECK(STD_FMT::format("{}", 1 * TiB) == "1 TiB");
  }

  SECTION("transfer_rate")
  {
    CHECK(STD_FMT::format("{}", 1 * (B / s)) == "1 B/s");
    CHECK(STD_FMT::format("{}", 1 * (kB / s)) == "1 kB/s");
    CHECK(STD_FMT::format("{}", 1 * (TB / s)) == "1 TB/s");
  }

  SECTION("traffic_intesity") { CHECK(STD_FMT::format("{}", 1 * E) == "1 E"); }

  SECTION("modulation_rate")
  {
    using namespace units::isq::iec80000;
    CHECK(STD_FMT::format("{}", 1 * Bd) == "1 Bd");
    CHECK(STD_FMT::format("{}", 1 * kBd) == "1 kBd");
    CHECK(STD_FMT::format("{}", 1 * TBd) == "1 TBd");
    CHECK(STD_FMT::format("{}", quantity_cast<baud>(4 / (2 * s))) == "2 Bd");
  }

  SECTION("incoherent units with powers")
  {
    // TODO(chogg): Reinstate after format/Magnitude redesign.
    // CHECK(STD_FMT::format("{}", 1_q_mi * 1_q_mi * 1_q_mi) == "1 [15900351812136/3814697265625 × 10⁹] m³");
    // CHECK(STD_FMT::format("{}", 1_q_au * 1_q_au) == "1 [2237952291797391849 × 10⁴] m²");
    //
    // CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_mi * 1_q_mi * 1_q_mi) == "1 [15900351812136/3814697265625 x 10^9] m^3");
    // CHECK(STD_FMT::format("{:%Q %Aq}", 1_q_au * 1_q_au) == "1 [2237952291797391849 x 10^4] m^2");
  }

  SECTION("unknown scaled unit with reference different than the dimension's coherent unit")
  {
    // TODO(chogg): Reinstate after format/Magnitude redesign.
    // constexpr auto mag = units::as_magnitude<units::ratio{2, 3}>();
    // CHECK(STD_FMT::format("{}", mass<units::scaled_unit<mag, gram>>(1)) == "1 [2/3 × 10⁻³] kg");
    // CHECK(STD_FMT::format("{:%Q %Aq}", mass<units::scaled_unit<mag, gram>>(1)) == "1 [2/3 x 10^-3] kg");
  }
}
