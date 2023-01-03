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

#include <catch2/catch_test_macros.hpp>
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
    CHECK(UNITS_STD_FMT::format("{}", 1_q_ns) == "1 ns");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_us) == "1 µs");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_ms) == "1 ms");

    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_us) == "1 us");
  }

  SECTION("length")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_mm) == "1 mm");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_cm) == "1 cm");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_km) == "1 km");
    CHECK(UNITS_STD_FMT::format("{}", 1 * ft) == "1 ft");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_ft_us) == "1 ft(us)");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_yd) == "1 yd");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_in) == "1 in");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_fathom) == "1 fathom");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_fathom_us) == "1 fathom(us)");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_mi) == "1 mi");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_mi_us) == "1 mi(us)");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_naut_mi) == "1 mi(naut)");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_ch) == "1 ch");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_rd) == "1 rd");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_thou) == "1 thou");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_pc) == "1 pc");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_ly) == "1 ly");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_pc) == "1 pc");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_angstrom) == "1 angstrom");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_au) == "1 au");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_pica_comp) == "1 pica(comp)");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_pica_prn) == "1 pica(prn)");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_point_comp) == "1 point(comp)");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_point_prn) == "1 point(prn)");
  }

  SECTION("mass") { CHECK(UNITS_STD_FMT::format("{}", 1_q_kg) == "1 kg"); }

  SECTION("area")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_m2) == "1 m²");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_mm2) == "1 mm²");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_cm2) == "1 cm²");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_km2) == "1 km²");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_ft2) == "1 ft²");

    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_m2) == "1 m^2");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_mm2) == "1 mm^2");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_cm2) == "1 cm^2");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_km2) == "1 km^2");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_ft2) == "1 ft^2");
  }

  SECTION("density")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_kg_per_m3) == "1 kg/m³");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_kg_per_m3) == "1 kg/m^3");
  }

  SECTION("resistance")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_R) == "1 Ω");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_kR) == "1 kΩ");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_mR) == "1 mΩ");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_MR) == "1 MΩ");

    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_R) == "1 ohm");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_kR) == "1 kohm");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_mR) == "1 mohm");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_MR) == "1 Mohm");
  }

  SECTION("voltage")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_V) == "1 V");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_mV) == "1 mV");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_uV) == "1 µV");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_nV) == "1 nV");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_pV) == "1 pV");

    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_uV) == "1 uV");
  }

  SECTION("volume")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_m3) == "1 m³");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_mm3) == "1 mm³");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_cm3) == "1 cm³");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_km3) == "1 km³");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_ft3) == "1 ft³");

    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_m3) == "1 m^3");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_mm3) == "1 mm^3");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_cm3) == "1 cm^3");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_km3) == "1 km^3");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_ft3) == "1 ft^3");
  }

  SECTION("frequency")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_mHz) == "1 mHz");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_kHz) == "1 kHz");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_MHz) == "1 MHz");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_GHz) == "1 GHz");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_THz) == "1 THz");
  }

  SECTION("speed")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_m_per_s) == "1 m/s");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_km_per_h) == "1 km/h");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_mi_per_h) == "1 mi/h");
  }

  SECTION("acceleration")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_m_per_s2) == "1 m/s²");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_m_per_s2) == "1 m/s^2");
  }

  SECTION("momentum")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_kg_m_per_s) == "1 kg⋅m/s");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_kg_m_per_s) == "1 kg m/s");
  }

  SECTION("energy")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_mJ) == "1 mJ");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_kJ) == "1 kJ");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_MJ) == "1 MJ");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_GJ) == "1 GJ");
  }

  SECTION("power")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_mW) == "1 mW");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_kW) == "1 kW");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_MW) == "1 MW");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_GW) == "1 GW");
  }

  SECTION("surface tension") { CHECK(UNITS_STD_FMT::format("{}", 1_q_N_per_m) == "1 N/m"); }

  SECTION("magnetic induction") { CHECK(UNITS_STD_FMT::format("{}", 1_q_T) == "1 T"); }

  SECTION("magnetic flux")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_Wb) == "1 Wb");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_G) == "1 G");
  }

  SECTION("inductance")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_H) == "1 H");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_mH) == "1 mH");
  }

  SECTION("conductance")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_S) == "1 S");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_nS) == "1 nS");
  }

  SECTION("catalytic activity")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_kat) == "1 kat");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_U) == "1 U");
  }

  SECTION("absorbed dose")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_Gy) == "1 Gy");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_kGy) == "1 kGy");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_mGy) == "1 mGy");
  }

  SECTION("addition with common ratio") { CHECK(UNITS_STD_FMT::format("{}", 1_q_in + 1_q_yd) == "37 in"); }

  SECTION("current density")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_A_per_m2) == "1 A/m²");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_A_per_m2) == "1 A/m^2");
  }

  SECTION("concentration")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_mol_per_m3) == "1 mol/m³");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_mol_per_m3) == "1 mol/m^3");
  }

  SECTION("luminance")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_cd_per_m2) == "1 cd/m²");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_cd_per_m2) == "1 cd/m^2");
  }

  SECTION("dynamic viscosity")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_Pa_s) == "1 Pa⋅s");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_Pa_s) == "1 Pa s");
  }

  SECTION("heat capacity") { CHECK(UNITS_STD_FMT::format("{}", 1_q_J_per_K) == "1 J/K"); }

  SECTION("specific heat capacity")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_J_per_kg_K) == "1 J⋅K⁻¹⋅kg⁻¹");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_J_per_kg_K) == "1 J K^-1 kg^-1");
  }

  SECTION("molar heath capacity")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_J_per_mol_K) == "1 J⋅K⁻¹⋅mol⁻¹");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_J_per_mol_K) == "1 J K^-1 mol^-1");
  }

  SECTION("thermal conductivity")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_W_per_m_K) == "1 W⋅m⁻¹⋅K⁻¹");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_W_per_m_K) == "1 W m^-1 K^-1");
  }

  SECTION("electric field strength") { CHECK(UNITS_STD_FMT::format("{}", 1_q_V_per_m) == "1 V/m"); }

  SECTION("charge density")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1_q_C_per_m3) == "1 C/m³");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_C_per_m3) == "1 C/m^3");
    CHECK(UNITS_STD_FMT::format("{}", 1_q_C_per_m2) == "1 C/m²");
    CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_C_per_m2) == "1 C/m^2");
  }

  SECTION("permittivity") { CHECK(UNITS_STD_FMT::format("{}", 1_q_F_per_m) == "1 F/m"); }

  SECTION("permeability") { CHECK(UNITS_STD_FMT::format("{}", 1_q_H_per_m) == "1 H/m"); }

  SECTION("molar energy") { CHECK(UNITS_STD_FMT::format("{}", 1_q_J_per_mol) == "1 J/mol"); }

  SECTION("torque") { CHECK(UNITS_STD_FMT::format("{}", 1_q_N_m_per_rad) == "1 N⋅m/rad"); }

  SECTION("storage_capacity")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1 * bit) == "1 bit");
    CHECK(UNITS_STD_FMT::format("{}", 1 * kbit) == "1 kbit");
    CHECK(UNITS_STD_FMT::format("{}", 1 * Tibit) == "1 Tibit");
    CHECK(UNITS_STD_FMT::format("{}", 1 * B) == "1 B");
    CHECK(UNITS_STD_FMT::format("{}", 1 * kB) == "1 kB");
    CHECK(UNITS_STD_FMT::format("{}", 1 * TiB) == "1 TiB");
  }

  SECTION("transfer_rate")
  {
    CHECK(UNITS_STD_FMT::format("{}", 1 * (B / s)) == "1 B/s");
    CHECK(UNITS_STD_FMT::format("{}", 1 * (kB / s)) == "1 kB/s");
    CHECK(UNITS_STD_FMT::format("{}", 1 * (TB / s)) == "1 TB/s");
  }

  SECTION("traffic_intesity") { CHECK(UNITS_STD_FMT::format("{}", 1 * E) == "1 E"); }

  SECTION("modulation_rate")
  {
    using namespace units::isq::iec80000;
    CHECK(UNITS_STD_FMT::format("{}", 1 * Bd) == "1 Bd");
    CHECK(UNITS_STD_FMT::format("{}", 1 * kBd) == "1 kBd");
    CHECK(UNITS_STD_FMT::format("{}", 1 * TBd) == "1 TBd");
    CHECK(UNITS_STD_FMT::format("{}", quantity_cast<baud>(4 / (2 * s))) == "2 Bd");
  }

  SECTION("incoherent units with powers")
  {
    // TODO(chogg): Reinstate after format/Magnitude redesign.
    // CHECK(UNITS_STD_FMT::format("{}", 1_q_mi * 1_q_mi * 1_q_mi) == "1 [15900351812136/3814697265625 × 10⁹] m³");
    // CHECK(UNITS_STD_FMT::format("{}", 1_q_au * 1_q_au) == "1 [2237952291797391849 × 10⁴] m²");
    //
    // CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_mi * 1_q_mi * 1_q_mi) == "1 [15900351812136/3814697265625 x 10^9]
    // m^3"); CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", 1_q_au * 1_q_au) == "1 [2237952291797391849 x 10^4] m^2");
  }

  SECTION("unknown scaled unit with reference different than the dimension's coherent unit")
  {
    // TODO(chogg): Reinstate after format/Magnitude redesign.
    // constexpr auto mag = units::mag<units::ratio{2, 3}>();
    // CHECK(UNITS_STD_FMT::format("{}", mass<units::scaled_unit<mag, gram>>(1)) == "1 [2/3 × 10⁻³] kg");
    // CHECK(UNITS_STD_FMT::format("{:%Q %Aq}", mass<units::scaled_unit<mag, gram>>(1)) == "1 [2/3 x 10^-3] kg");
  }
}
