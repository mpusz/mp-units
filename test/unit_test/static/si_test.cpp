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

#include <units/bits/unit_text.h>
#include <units/generic/angle.h>
#include <units/generic/dimensionless.h>
#include <units/isq/si/si.h>

namespace {

using namespace units;
using namespace units::isq::si;

/* ************** BASE DIMENSIONS **************** */

// length

static_assert(1_q_km == 1000_q_m);
static_assert(1_q_m == 100_q_cm);
static_assert(1_q_m == 10_q_dm);
static_assert(1_q_m == 1000_q_mm);
static_assert(1_q_hm == 100_q_m);
static_assert(1_q_au == 149'597'870'700_q_m);
static_assert(1_q_km + 1_q_m == 1001_q_m);
static_assert(10_q_km / 5_q_km == 2);
static_assert(10_q_km / 5_q_km < 3);
static_assert(100_q_mm / 5_q_cm == dimensionless<scaled_unit<ratio(1, 1, -1), one>>(20));
static_assert(100_q_mm / 5_q_cm == dimensionless<one>(2));
static_assert(10_q_km / 2 == 5_q_km);

static_assert(millimetre::symbol == "mm");
static_assert(centimetre::symbol == "cm");
static_assert(decimetre::symbol == "dm");
static_assert(kilometre::symbol == "km");

// mass

static_assert(1_q_kg == 1000_q_g);
static_assert(1_q_t == 1000_q_kg);
static_assert(1_q_kt == 1000000_q_kg);

static_assert(kilogram::symbol == "kg");
static_assert(tonne::symbol == "t");
static_assert(kilotonne::symbol == "kt");

// time

static_assert(1_q_us == 1000_q_ns);
static_assert(1_q_ms == 1000_q_us);
static_assert(1_q_s == 1000_q_ms);
static_assert(1_q_min == 60_q_s);
static_assert(1_q_h == 60_q_min);
static_assert(1_q_h == 3600_q_s);
static_assert(1_q_d == 24_q_h);
static_assert(1_q_d == 86'400_q_s);

static_assert(nanosecond::symbol == "ns");
static_assert(microsecond::symbol == basic_symbol_text("µs", "us"));
static_assert(millisecond::symbol == "ms");

// current

// temperature

// substance

// luminous intensity

/* ************** DERIVED DIMENSIONS WITH NAMED UNITS **************** */

// frequency

static_assert(1000_q_mHz == 1_q_Hz);
static_assert(1000_q_Hz == 1_q_kHz);
static_assert(1000_q_kHz == 1_q_MHz);
static_assert(1000_q_MHz == 1_q_GHz);
static_assert(1000_q_GHz == 1_q_THz);

static_assert(millihertz::symbol == "mHz");
static_assert(kilohertz::symbol == "kHz");
static_assert(megahertz::symbol == "MHz");
static_assert(gigahertz::symbol == "GHz");
static_assert(terahertz::symbol == "THz");

static_assert(2 / 1_q_s == 2_q_Hz);
static_assert(120 / 1_q_min == 2_q_Hz);
static_assert(1000 / 1_q_s == 1_q_kHz);
static_assert(1 / 1_q_ms == 1_q_kHz);
static_assert(3.2_q_GHz == 3'200'000'000_q_Hz);
static_assert((10_q_Hz * 1_q_min).number() == 10);
static_assert(10_q_Hz * 1_q_min == dimensionless<scaled_unit<ratio(60), one>>(10));
static_assert(10_q_Hz * 1_q_min == dimensionless<one>(600));
static_assert(2 / 1_q_Hz == 2_q_s);

// force
static_assert(10_q_kg * 10_q_m_per_s2 == 100_q_N);
static_assert(100_q_N / 1_q_m_per_s2 == 100_q_kg);
static_assert(100_q_N / 1_q_kg == 100_q_m_per_s2);

// pressure

static_assert(10_q_N / 10_q_m2 == 1_q_Pa);
static_assert(10_q_N / 1_q_Pa == 10_q_m2);
static_assert(1_q_Pa * 10_q_m2 == 10_q_N);

// energy

static_assert(1000_q_mJ == 1_q_J);
static_assert(1000_q_J == 1_q_kJ);
static_assert(1000_q_kJ == 1_q_MJ);
static_assert(1000_q_MJ == 1_q_GJ);

static_assert(millijoule::symbol == "mJ");
static_assert(kilojoule::symbol == "kJ");
static_assert(megajoule::symbol == "MJ");
static_assert(gigajoule::symbol == "GJ");

static_assert(10_q_N * 10_q_m == 100_q_J);
static_assert(100_q_J / 10_q_m == 10_q_N);
static_assert(100_q_J / 10_q_N == 10_q_m);
static_assert(10_q_Pa * 10_q_m3 == 100_q_J);
static_assert(100_q_J / 10_q_Pa == 10_q_m3);
static_assert(100_q_J / 10_q_m3 == 10_q_Pa);

// power

static_assert(1000_q_mW == 1_q_W);
static_assert(1000_q_W == 1_q_kW);
static_assert(1000_q_kW == 1_q_MW);
static_assert(1000_q_MW == 1_q_GW);

static_assert(milliwatt::symbol == "mW");
static_assert(kilowatt::symbol == "kW");
static_assert(megawatt::symbol == "MW");
static_assert(gigawatt::symbol == "GW");

static_assert(10_q_J / 10_q_s == 1_q_W);
static_assert(1_q_W * 10_q_s == 10_q_J);
static_assert(10_q_J / 1_q_W == 10_q_s);

// electric charge

static_assert(10_q_A * 10_q_s == 100_q_C);
static_assert(100_q_C / 10_q_A == 10_q_s);
static_assert(100_q_C / 10_q_s == 10_q_A);

// voltage

static_assert(10_q_W / 10_q_A == 1_q_V);
static_assert(10_q_W / 1_q_V == 10_q_A);
static_assert(1_q_V * 10_q_A == 10_q_W);
static_assert(10_q_J / 10_q_C == 1_q_V);
static_assert(10_q_J / 1_q_V == 10_q_C);
static_assert(10_q_C * 1_q_V == 10_q_J);

// capacitance

static_assert(10_q_C / 10_q_V == 1_q_F);
static_assert(10_q_C / 1_q_F == 10_q_V);
static_assert(10_q_V * 1_q_F == 10_q_C);

// magnetic induction

static_assert(1_q_T == 1_q_V * 1_q_s / (1_q_m * 1_q_m));
static_assert(10_q_T / 1_q_s == 10_q_V / (1_q_m * 1_q_m));
static_assert(10_q_T * (1_q_m * 1_q_m) == 10_q_s * 1_q_V);
static_assert(10_q_N / (1_q_A * 1_q_m) == 10_q_T);

static_assert(millitesla::symbol == "mT");
static_assert(microtesla::symbol == basic_symbol_text("µT", "uT"));
static_assert(nanotesla::symbol == "nT");
static_assert(picotesla::symbol == "pT");

// magnetic flux

static_assert(1_q_Wb == 1_q_T * 1_q_m2);
static_assert(1_q_J == 1_q_Wb * 1_q_A);
static_assert(1_q_N * 1_q_s == 1_q_Wb * 1_q_C / 1_q_m);

static_assert(milliweber::symbol == "mWb");
static_assert(microweber::symbol == basic_symbol_text("µWb", "uWb"));
static_assert(nanoweber::symbol == "nWb");
static_assert(picoweber::symbol == "pWb");

// inductance

static_assert(1_q_H == 1_q_Wb / 1_q_A);
static_assert(1_q_V == 1_q_H * 1_q_A / 1_q_s);
static_assert(1_q_J == 1_q_H * 1_q_A * 1_q_A);

static_assert(millihenry::symbol == "mH");
static_assert(microhenry::symbol == basic_symbol_text("µH", "uH"));
static_assert(nanohenry::symbol == "nH");
static_assert(picohenry::symbol == "pH");

// conductance

static_assert(1_q_S * 1_q_R == 1);
static_assert(1_q_S == 1_q_A / 1_q_V);
static_assert(1_q_W == 1_q_A * 1_q_A / 1_q_S);

static_assert(millisiemens::symbol == "mS");
static_assert(microsiemens::symbol == basic_symbol_text("µS", "uS"));
static_assert(nanosiemens::symbol == "nS");

// catalytic activity

static_assert(1_q_kat == 1_q_mol / 1_q_s);
static_assert(1'000'000_q_U == 1_q_mol / 1_q_min);

static_assert(katal::symbol == "kat");
static_assert(enzyme_unit::symbol == "U");

// absorbed dose

static_assert(1_q_Gy == 1_q_J / 1_q_kg);
static_assert(9._q_W * 3_q_s / 60_q_kg == 450_q_mGy);

static_assert(gray::symbol == "Gy");
static_assert(milligray::symbol == "mGy");
static_assert(kilogray::symbol == "kGy");

/* ************** DERIVED DIMENSIONS IN TERMS OF BASE UNITS **************** */

// speed

static_assert(10_q_m / 5_q_s == 2_q_m_per_s);
static_assert(10 / 5_q_s * 1_q_m == 2_q_m_per_s);
static_assert(1_q_km / 1_q_s == 1000_q_m_per_s);
static_assert(1_q_km / 1_q_h == 1_q_km_per_h);
static_assert(1.0_q_km / 1_q_h == 1_q_km_per_h);
static_assert(1000.0_q_m / 3600.0_q_s == 1_q_km_per_h);

static_assert(2_q_km_per_h * 2_q_h == 4_q_km);
static_assert(2_q_km_per_h * 15_q_min == 500_q_m);
static_assert(2_q_km_per_h * 15.0_q_min == 500_q_m);
static_assert(2.0_q_km_per_h * 15_q_min == 500_q_m);

static_assert(2_q_km / 2_q_km_per_h == 1_q_h);
static_assert(2000_q_m / 2_q_km_per_h == 1_q_h);
static_assert(quantity_cast<kilometre>(2000_q_m) / 2_q_km_per_h == 1_q_h);

static_assert(detail::unit_text<dim_speed, metre_per_second>() == "m/s");
static_assert(kilometre_per_hour::symbol == "km/h");

// acceleration

static_assert(10_q_m_per_s / 10_q_s == 1_q_m_per_s2);
static_assert(10_q_m_per_s / 1_q_m_per_s2 == 10_q_s);
static_assert(1_q_m_per_s2 * 10_q_s == 10_q_m_per_s);

static_assert(detail::unit_text<dim_acceleration, metre_per_second_sq>() == basic_symbol_text("m/s²", "m/s^2"));

// area

static_assert(10_q_m * 10_q_m == 100_q_m2);
static_assert(100_q_m2 / 10_q_m == 10_q_m);
static_assert(10_q_km * 10_q_km == 100_q_km2);
static_assert(1_q_m2 == 10'000_q_cm2);

static_assert(1_q_ca == 1_q_m2);
static_assert(1_q_da == 10_q_m2);
static_assert(1_q_a == 100_q_m2);
static_assert(1_q_daa == 1'000_q_m2);
static_assert(1_q_ha == 10'000_q_m2);

static_assert(centiare::symbol == "ca");
static_assert(deciare::symbol == "da");
static_assert(are::symbol == "a");
static_assert(decare::symbol == "daa");
static_assert(hectare::symbol == "ha");

static_assert(detail::unit_text<dim_area, square_metre>() == basic_symbol_text("m²", "m^2"));

// volume

static_assert(1_q_m * 1_q_m * 1_q_m == 1_q_m3);
static_assert(10_q_m2 * 10_q_m == 100_q_m3);
static_assert(10_q_km * 10_q_km * 10_q_km == 1000_q_km3);
static_assert(1_q_m3 == 1'000'000_q_cm3);
static_assert(1_q_dm * 1_q_dm * 1_q_dm == 1_q_l);
static_assert(1000_q_l == 1_q_m3);
static_assert(1_q_kl == 1_q_m3);

static_assert(litre::symbol == "l");
static_assert(kilolitre::symbol == "kl");

static_assert(detail::unit_text<dim_volume, cubic_metre>() == basic_symbol_text("m³", "m^3"));

/* ************** DERIVED DIMENSIONS IN TERMS OF OTHER UNITS **************** */

static_assert(10_q_N / 2_q_m == 5_q_N_per_m);
static_assert(10_q_N / 5_q_N_per_m == 2_q_m);
static_assert(2_q_m * 5_q_N_per_m == 10_q_N);

static_assert(detail::unit_text<dim_surface_tension, newton_per_metre>() == "N/m");

// current density

static_assert(12_q_A_per_m2 == 60_q_A / 5_q_m2);
static_assert(1_q_A_per_m2 == 1_q_S * 1_q_V / 1_q_m2);

static_assert(detail::unit_text<dim_current_density, ampere_per_metre_sq>() == basic_symbol_text("A/m²", "A/m^2"));

// concentration

static_assert(1_q_mol_per_m3 == 1_q_kg_per_m3 * 1_q_mol / 1_q_kg);
static_assert(detail::unit_text<dim_concentration, ampere_per_metre_sq>() == basic_symbol_text("mol/m³", "mol/m^3"));

// luminance

static_assert(1_q_cd_per_m2 == 1_q_cd / 1_q_m2);
static_assert(detail::unit_text<dim_luminance, candela_per_metre_sq>() == basic_symbol_text("cd/m²", "cd/m^2"));

// dynamic viscosity

static_assert(1_q_Pa_s == 1_q_N * 1_q_s / 1_q_m2);
static_assert(detail::unit_text<dim_dynamic_viscosity, pascal_second>() == basic_symbol_text("Pa ⋅ s", "Pa s"));

// [specific|molar] heath capacity

static_assert(1_q_J_per_K == 1_q_J_per_kg_K * 1_q_kg);
static_assert(1_q_J_per_K * 1_q_K == 1_q_s * 1_q_N * 1_q_m_per_s);
static_assert(1_q_J_per_mol_K == 1_q_J_per_K / 1_q_mol);

static_assert(detail::unit_text<dim_heat_capacity, joule_per_kelvin>() == "J/K");
static_assert(detail::unit_text<dim_specific_heat_capacity, joule_per_kilogram_kelvin>() ==
              basic_symbol_text("J ⋅ K⁻¹ ⋅ kg⁻¹", "J K^-1 kg^-1"));

// thermal conductivity

static_assert(20_q_W_per_m_K * 10_q_m * 300_q_K == 60'000_q_W);
static_assert(detail::unit_text<dim_thermal_conductivity, watt_per_metre_kelvin>() ==
              basic_symbol_text("W ⋅ m⁻¹ ⋅ K⁻¹", "W m^-1 K^-1"));

// electric field strength

static_assert(100_q_N / 20_q_C == 5_q_V_per_m);
static_assert(1_q_C * 10_q_V_per_m * 3_q_m == 30_q_J);

static_assert(detail::unit_text<dim_electric_field_strength, volt_per_metre>() == "V/m");

// [surface] charge density

static_assert(20._q_C / 40_q_m3 == 0.5_q_C_per_m3);
static_assert(10._q_C / 20_q_m2 == 0.5_q_C_per_m2);
static_assert(20._q_C_per_m3 == 10._q_C_per_m2 / 0.5_q_m);
static_assert(1_q_C / 1_q_m / 1_q_m == 1_q_C / 1_q_m2);
static_assert(1_q_C_per_m2 == 1_q_C_per_m3 * 1_q_m);
static_assert(1_q_V_per_m * 10_q_C_per_m3 * 1_q_m3 == 10_q_N);

static_assert(detail::unit_text<dim_charge_density, coulomb_per_metre_cub>() == basic_symbol_text("C/m³", "C/m^3"));
static_assert(detail::unit_text<dim_surface_charge_density, coulomb_per_metre_sq>() ==
              basic_symbol_text("C/m²", "C/m^2"));

// permittivity

static_assert(1_q_F_per_m == 1_q_F / 1_q_m);
static_assert(1 / (1_q_F_per_m) * 1_q_C * 1_q_C / 1_q_m2 == 1_q_N);
static_assert(1_q_C_per_m3 / 1_q_F_per_m * 1_q_m == 1_q_V_per_m);

static_assert(detail::unit_text<dim_permittivity, farad_per_metre>() == "F/m");

// permeability

static_assert(1_q_H_per_m * 1_q_A / 1_q_m == 1_q_T);
static_assert(1_q_H_per_m * 1_q_A * 1_q_A == 1_q_N);

static_assert(detail::unit_text<dim_permeability, henry_per_metre>() == "H/m");

// molar energy

static_assert(1_q_J_per_mol * 1_q_mol_per_m3 * 1_q_m3 == 1_q_N * 1_q_m);

static_assert(detail::unit_text<dim_molar_energy, joule_per_mole>() == "J/mol");

// angular velocity

static_assert(1_q_rad / 1_q_s == 1_q_rad_per_s);
static_assert(detail::unit_text<dim_angular_velocity, radian_per_second>() == basic_symbol_text("ω", "w"));

}  // namespace
