#!/usr/bin/env python3
# The MIT License (MIT)
#
# Copyright (c) 2018 Mateusz Pusz
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

"""Generate `src/systems/include/mp-units/systems/codata.h` from the NIST tables.

The source of truth are the fixed-width "allascii" tables published by NIST for each
CODATA adjustment, checked in verbatim under `scripts/codata/`. Every row of every
table must be accounted for in exactly one way:

- generated as a constant transcribing the row's value and standard uncertainty,
- generated from a curated defining relation (rows printed truncated with `...` are
  exact but non-terminating decimals, so copying their digits would define an inexact
  constant while labelling it exact - the symbolic expression is the correct value),
- emitted as an alias of another constant (a row that restates an entity that already
  has a home, either in `si` or earlier in the same namespace),
- skipped as a unit variant of another row (the tabulated forms restate one constant in
  different units, a use-time choice in a library where constants are units), or
- skipped for a curated, documented reason.

An unaccounted row is an error, so adding a new adjustment cannot silently drop data.

After emission the generated header is verified independently: each constant's
expression is re-parsed and re-evaluated numerically in the row's own unit and compared
against the printed digits (exactly for transcribed rows, by decimal prefix for the
truncated exact ones). That check is the point of generating rather than transcribing
by hand.

Usage:
    python3 scripts/codata_constants.py            # regenerate + verify
    python3 scripts/codata_constants.py --report   # per-row disposition report
    python3 scripts/codata_constants.py --check    # verify; fail if the file differs
"""

from __future__ import annotations

import argparse
import re
import sys
from dataclasses import dataclass, field
from fractions import Fraction
from pathlib import Path
from typing import Callable, Optional

REPO_ROOT = Path(__file__).resolve().parent.parent
DATA_DIR = Path(__file__).resolve().parent / "codata"
SYSTEMS_DIR = REPO_ROOT / "src/systems/include/mp-units/systems"

ADJUSTMENTS = ("2014", "2018", "2022")
INLINE_ADJUSTMENT = "2022"

# 50 digits of pi as an exact Fraction, plenty for comparing against <=15 printed digits
PI = Fraction(31415926535897932384626433832795028841971693993751, 10**49)

# Solutions of the Wien displacement transcendental equations `x = n (1 - exp(-x))`,
# for n = 5 (wavelength form) and n = 3 (frequency form), to 50 significant digits.
# They are exact mathematical constants (like pi), which is what makes the Wien rows
# "(exact)" in the post-2019 tables.
WIEN_X_WAVELENGTH = Fraction(
    496511423174427630369875913132289394405558498679725, 10**50
)
WIEN_X_FREQUENCY = Fraction(282143937212207889340319133029448519534588174407311, 10**50)


# --------------------------------------------------------------------------------------
# Table parsing
# --------------------------------------------------------------------------------------


@dataclass
class Row:
    name: str
    value: str  # verbatim value column
    uncertainty: str  # verbatim uncertainty column ("(exact)" for exact rows)
    unit: str  # verbatim unit column ("" for dimensionless)

    @property
    def exact(self) -> bool:
        return self.uncertainty == "(exact)"

    @property
    def truncated(self) -> bool:
        return "..." in self.value


def parse_table(adjustment: str) -> list[Row]:
    lines = (DATA_DIR / f"allascii_{adjustment}.txt").read_text().splitlines()
    start = next(i for i, line in enumerate(lines) if line.startswith("----"))
    rows = []
    for line in lines[start + 1 :]:
        if not line.strip():
            continue
        rows.append(
            Row(
                name=line[:60].strip(),
                value=line[60:85].strip(),
                uncertainty=line[85:110].strip(),
                unit=line[110:].strip(),
            )
        )
    return rows


# --------------------------------------------------------------------------------------
# Number handling
# --------------------------------------------------------------------------------------


def split_number(text: str) -> tuple[str, int]:
    """Split a table number into its digit string (spaces removed) and base-10 exponent.

    `"6.674 30 e-11"` becomes `("6.67430", -11)`. The digit string keeps the printed
    precision (trailing zeros included), which is what the emitted magnitude transcribes.
    """
    text = text.replace("...", "").strip()
    match = re.fullmatch(r"([-0-9. ]+?)(?:\s*e(-?\d+))?", text)
    if not match:
        raise ValueError(f"unparsable number: {text!r}")
    return match.group(1).replace(" ", ""), int(match.group(2) or 0)


def number_fraction(text: str) -> Fraction:
    digits, exp = split_number(text)
    return Fraction(digits) * Fraction(10) ** exp


def group_digits(number: int) -> str:
    """Format an integer with C++ digit separators (`'`), matching the repo style."""
    sign, digits = ("-", str(-number)) if number < 0 else ("", str(number))
    grouped = []
    while len(digits) > 3:
        grouped.append(digits[-3:])
        digits = digits[:-3]
    grouped.append(digits)
    return sign + "'".join(reversed(grouped))


def magnitude_expr(text: str) -> str:
    """Spell a table number as an exact magnitude expression.

    The printed mantissa becomes a `mag_ratio` (or a plain `mag` when integral) and the
    printed exponent a `mag_power<10, N>`, so the emitted definition transcribes the row
    digit for digit.
    """
    digits, exp = split_number(text)
    negative = digits.startswith("-")
    if negative:
        digits = digits[1:]
    if "." in digits:
        integral, fraction = digits.split(".")
        numerator = int(integral + fraction)
        denominator = 10 ** len(fraction)
        mag = f"mag_ratio<{group_digits(numerator)}, {group_digits(denominator)}>"
    else:
        mag = f"mag<{group_digits(int(digits))}>"
    if exp != 0:
        mag += f" * mag_power<10, {exp}>"
    return f"-{mag}" if negative else mag


def uncertainty_magnitude_expr(text: str) -> str:
    """Spell an uncertainty as a normalized-scientific exact magnitude.

    Uncertainties are published to two significant digits; `"0.000 15 e-11"` becomes
    `mag_ratio<15, 10> * mag_power<10, -15>` (1.5 x 10^-15).
    """
    digits, exp = split_number(text)
    if "." in digits:
        integral, fraction = digits.split(".")
        exp -= len(fraction)
        digits = integral + fraction
    significant = digits.lstrip("0")
    exp += len(significant) - 1
    if len(significant) == 1:
        mag = f"mag<{significant}>"
    else:
        denominator = 10 ** (len(significant) - 1)
        mag = (
            f"mag_ratio<{group_digits(int(significant))}, {group_digits(denominator)}>"
        )
    if exp != 0:
        mag += f" * mag_power<10, {exp}>"
    return mag


# --------------------------------------------------------------------------------------
# Unit handling
# --------------------------------------------------------------------------------------

# Token -> (C++ unit expression, SI factor as a Fraction). The factors serve validation
# only; every value comparison happens in the row's own unit, so only the ratio of the
# two factors matters and epoch-dependent choices (the eV) cancel exactly.
UNIT_TOKENS: dict[str, tuple[str, Fraction]] = {
    "m": ("si::metre", Fraction(1)),
    "kg": ("si::kilogram", Fraction(1)),
    "s": ("si::second", Fraction(1)),
    "A": ("si::ampere", Fraction(1)),
    "K": ("si::kelvin", Fraction(1)),
    "mol": ("si::mole", Fraction(1)),
    "C": ("si::coulomb", Fraction(1)),
    "J": ("si::joule", Fraction(1)),
    "N": ("si::newton", Fraction(1)),
    "T": ("si::tesla", Fraction(1)),
    "V": ("si::volt", Fraction(1)),
    "F": ("si::farad", Fraction(1)),
    "W": ("si::watt", Fraction(1)),
    "Pa": ("si::pascal", Fraction(1)),
    "Hz": ("si::hertz", Fraction(1)),
    "S": ("si::siemens", Fraction(1)),
    "ohm": ("si::ohm", Fraction(1)),
    "Wb": ("si::weber", Fraction(1)),
    "H": ("si::henry", Fraction(1)),
    "lm": ("si::lumen", Fraction(1)),
    "sr": ("si::steradian", Fraction(1)),
    "eV": ("si::electronvolt", Fraction(1_602_176_634, 10**28)),
    "GeV": ("si::giga<si::electronvolt>", Fraction(1_602_176_634, 10**19)),
    "u": ("si::dalton", Fraction(166_053_906_892, 10**38)),
}


def parse_unit(unit: str) -> list[tuple[str, int]]:
    """Parse a table unit string into `(token, exponent)` pairs."""
    if not unit:
        return []
    factors = []
    for token in unit.split():
        match = re.fullmatch(r"([A-Za-z_]+)(?:\^(-?\d+))?", token)
        if not match:
            raise ValueError(f"unparsable unit token {token!r} in {unit!r}")
        symbol, exp = match.group(1), int(match.group(2) or 1)
        if symbol not in UNIT_TOKENS:
            raise ValueError(f"unknown unit token {symbol!r} in {unit!r}")
        factors.append((symbol, exp))
    return factors


def unit_power_expr(cpp: str, exp: int) -> str:
    if exp == 1:
        return cpp
    if exp == 2:
        return f"square({cpp})"
    if exp == 3:
        return f"cubic({cpp})"
    return f"pow<{exp}>({cpp})"


def unit_expr(unit: str) -> str:
    """Spell a table unit string as a C++ unit expression (`"one"` when dimensionless)."""
    factors = parse_unit(unit)
    if not factors:
        return "one"
    numerator = [unit_power_expr(UNIT_TOKENS[t][0], e) for t, e in factors if e > 0]
    denominator = [unit_power_expr(UNIT_TOKENS[t][0], -e) for t, e in factors if e < 0]
    parts = numerator if numerator else ["one"]
    return " * ".join(parts) + "".join(f" / {d}" for d in denominator)


def unit_si_factor(unit: str) -> Fraction:
    factor = Fraction(1)
    for token, exp in parse_unit(unit):
        factor *= UNIT_TOKENS[token][1] ** exp
    return factor


# --------------------------------------------------------------------------------------
# Curated knowledge
# --------------------------------------------------------------------------------------

# Rows that restate another row in a different unit or with an exact numeric factor
# (2 pi). In a library where constants are units, those forms are a use-time choice, so
# they carry no extra information here - exactly the policy `hep` follows.
VARIANT_RE = re.compile(
    r"("
    r" energy equivalent( in MeV)?$"
    r"| in u$| in MeV$| in GeV$| in eV$| in K$| in Hz$| in J$| in kg$"
    r"| in eV s$| in eV/K$| in Hz/K$| in eV/T$| in Hz/T$| in K/T$| in MHz/T$"
    r"| in eV/Hz$| in MeV/c$| in MeV fm$"
    r"| in inverse meters? per (kelvin|tesla)$"
    r"| in inverse meter$"
    r"|^(atomic mass unit|electron volt|hartree|hertz|inverse meter|joule|kelvin"
    r"|kilogram)-.* relationship$"
    r"| over 2 pi( times c in MeV fm)?$"
    r")"
)

# The NIST "Frequently used constants" selection (plus the free `si` aliases). These rows
# form the `codataYYYY_essential.h` tier: a cheap-to-compile subset with the constants most
# code actually names, included by the full per-adjustment header (so both spell the same
# types) and by `iau.h`. Curated relations referenced by these rows must be essential too.
ESSENTIAL_ROWS = {
    "atomic mass constant",
    "Avogadro constant",
    "Bohr magneton",
    "Bohr radius",
    "Boltzmann constant",
    "Compton wavelength",
    "conductance quantum",
    "electron mass",
    "elementary charge",
    "Faraday constant",
    "fine-structure constant",
    "Hartree energy",
    "hyperfine transition frequency of Cs-133",
    "inverse fine-structure constant",
    "Josephson constant",
    "luminous efficacy",
    "mag. constant",
    "mag. flux quantum",
    "molar gas constant",
    "muon mass",
    "neutron mass",
    "Newtonian constant of gravitation",
    "nuclear magneton",
    "Planck constant",
    "Planck constant over 2 pi",
    "proton mass",
    "proton-electron mass ratio",
    "reduced Planck constant",
    "Rydberg constant",
    "speed of light in vacuum",
    "standard acceleration of gravity",
    "Stefan-Boltzmann constant",
    "electric constant",
    "vacuum electric permittivity",
    "vacuum mag. permeability",
    "von Klitzing constant",
}

# Rows deliberately not represented in one specific adjustment only (currently none).
YEAR_SKIPS: dict[tuple[str, str], str] = {}

# Rows deliberately not represented, with the reason recorded in the report.
CURATED_SKIPS = {
    "Newtonian constant of gravitation over h-bar c": (
        "a natural-units presentation of G; comes free as G / (hbar c) at use time"
    ),
    "Faraday constant for conventional electric current": (
        "expressed in the conventional coulomb C_90, a unit this library does not define"
    ),
    "electron volt": "provided as the unit si::electronvolt",
}

# Rows from NIST's "adopted values" category that are identical in every adjustment
# (conventional values fixed by CGPM/CIPM resolutions rather than measured). They are
# emitted once, directly in `mp_units::codata` (in `codata/adopted_values.h`), so a unit
# defined through one of them (e.g. `yard_pound`'s `pound_force` through the standard
# gravity) keeps a single stable type that does not drift when the inline adjustment
# namespace moves. The generator verifies the rows really are identical across all tables.
ADOPTED_VALUES = {
    "standard acceleration of gravity": "standard_gravity",
    "standard atmosphere": "standard_atmosphere",
    "standard-state pressure": "standard_state_pressure",
    "conventional value of Josephson constant": "conventional_value_of_josephson_constant",
    "conventional value of von Klitzing constant": "conventional_value_of_von_klitzing_constant",
}

# Rows that alias an entity that already has a home in the `si` system. The expected
# exact value pins the equivalence; a table that disagreed would fail generation.
SI_ALIASES = {
    "speed of light in vacuum": ("si::si2019::speed_of_light_in_vacuum", "299 792 458"),
    "Planck constant": ("si::si2019::planck_constant", "6.626 070 15 e-34"),
    "elementary charge": ("si::si2019::elementary_charge", "1.602 176 634 e-19"),
    "Boltzmann constant": ("si::si2019::boltzmann_constant", "1.380 649 e-23"),
    "Avogadro constant": ("si::si2019::avogadro_constant", "6.022 140 76 e23"),
    "luminous efficacy": ("si::si2019::luminous_efficacy", "683"),
    "hyperfine transition frequency of Cs-133": (
        "si::si2019::hyperfine_structure_transition_frequency_of_cs",
        "9 192 631 770",
    ),
}
# Rows that alias an exact `si` constant only where they are themselves exact; in
# CODATA 2014 h, e, k, and N_A were measured and get transcribed instead.
SI_ALIASES_MEASURED_IN_2014 = {
    "Planck constant",
    "elementary charge",
    "Boltzmann constant",
    "Avogadro constant",
}

# Rows whose value/uncertainty/unit exactly duplicate another row of the same
# adjustment: the name is an alias for the very same entity, and re-declaring it would
# create a second constant type whose uncertainty the conversion engine would wrongly
# treat as independent. Candidate target names are tried in order; when none is present
# (or the row disagrees with every candidate), the row is transcribed on its own.
DUPLICATE_ALIASES: dict[str, tuple[str, ...]] = {
    "atomic unit of action": ("reduced Planck constant", "Planck constant over 2 pi"),
    "natural unit of action": ("reduced Planck constant", "Planck constant over 2 pi"),
    "atomic unit of charge": ("elementary charge",),
    "atomic unit of length": ("Bohr radius",),
    "atomic unit of mass": ("electron mass",),
    "atomic unit of energy": ("Hartree energy",),
    "natural unit of mass": ("electron mass",),
    "natural unit of length": ("reduced Compton wavelength",),
    "natural unit of velocity": ("speed of light in vacuum",),
    "unified atomic mass unit": ("atomic mass constant",),
}

# Identifier overrides where plain normalization of the printed name is not wanted. A
# row named here is never classified as a unit variant.
IDENTIFIER_OVERRIDES = {
    "vacuum mag. permeability": "magnetic_constant",
    "mag. constant": "magnetic_constant",
    "electric constant": "vacuum_electric_permittivity",
    "Planck constant over 2 pi": "reduced_planck_constant",
    "fine-structure constant": "fine_structure_constant",
    "inverse fine-structure constant": "inverse_fine_structure_constant",
    "Compton wavelength": "electron_compton_wavelength",
    "reduced Compton wavelength": "reduced_electron_compton_wavelength",
    "{220} lattice spacing of silicon": "lattice_spacing_of_silicon_220",
    "lattice spacing of ideal Si (220)": "lattice_spacing_of_silicon_220",
    "Cu x unit": "copper_x_unit",
    "Mo x unit": "molybdenum_x_unit",
    "atomic unit of mom.um": "atomic_unit_of_momentum",
    "natural unit of mom.um": "natural_unit_of_momentum",
    "molar volume of ideal gas (273.15 K, 100 kPa)": "molar_volume_of_ideal_gas_stp",
    "molar volume of ideal gas (273.15 K, 101.325 kPa)": "molar_volume_of_ideal_gas_atm",
    "Loschmidt constant (273.15 K, 100 kPa)": "loschmidt_constant_stp",
    "Loschmidt constant (273.15 K, 101.325 kPa)": "loschmidt_constant_atm",
    "Sackur-Tetrode constant (1 K, 100 kPa)": "sackur_tetrode_constant_100_kpa",
    "Sackur-Tetrode constant (1 K, 101.325 kPa)": "sackur_tetrode_constant_101_325_kpa",
    "standard acceleration of gravity": "standard_gravity",
}

# Display symbols for constants with a conventional one; everything else falls back to
# its identifier, which is verbose but unambiguous (precedent: hep::gas_threshold).
# A single string is used verbatim; a pair is (unicode, ascii).
SYMBOLS: dict[str, object] = {
    "alpha_particle_mass": ("m_α", "m_alpha"),
    "atomic_mass_constant": "m_u",
    "bohr_magneton": ("μ_B", "mu_B"),
    "bohr_radius": "a_0",
    "boltzmann_constant": "k",
    "avogadro_constant": "N_A",
    "planck_constant": "h",
    "elementary_charge": "e",
    "characteristic_impedance_of_vacuum": "Z_0",
    "classical_electron_radius": "r_e",
    "electron_compton_wavelength": ("λ_C", "lambda_C"),
    "reduced_electron_compton_wavelength": ("ƛ_C", "lambdabar_C"),
    "conductance_quantum": "G_0",
    "deuteron_g_factor": "g_d",
    "deuteron_mass": "m_d",
    "deuteron_magnetic_moment": ("μ_d", "mu_d"),
    "electron_g_factor": "g_e",
    "electron_gyromagnetic_ratio": ("γ_e", "gamma_e"),
    "electron_magnetic_moment": ("μ_e", "mu_e"),
    "electron_mass": "m_e",
    "faraday_constant": "F",
    "fine_structure_constant": ("α", "alpha"),
    "first_radiation_constant": "c_1",
    "first_radiation_constant_for_spectral_radiance": "c_1L",
    "hartree_energy": "E_h",
    "helion_g_factor": "g_h",
    "helion_mass": "m_h",
    "josephson_constant": "K_J",
    "conventional_value_of_josephson_constant": "K_J-90",
    "conventional_value_of_von_klitzing_constant": "R_K-90",
    "loschmidt_constant_stp": "n_0",
    "magnetic_constant": ("μ₀", "u_0"),
    "magnetic_flux_quantum": ("Φ₀", "Phi_0"),
    "molar_gas_constant": "R",
    "molar_mass_constant": "M_u",
    "molar_planck_constant": "N_Ah",
    "muon_g_factor": ("g_μ", "g_mu"),
    "muon_mass": ("m_μ", "m_mu"),
    "muon_magnetic_moment": ("μ_μ", "mu_mu"),
    "neutron_g_factor": "g_n",
    "neutron_gyromagnetic_ratio": ("γ_n", "gamma_n"),
    "neutron_magnetic_moment": ("μ_n", "mu_n"),
    "neutron_mass": "m_n",
    "newtonian_constant_of_gravitation": "G",
    "nuclear_magneton": ("μ_N", "mu_N"),
    "planck_length": "l_P",
    "planck_mass": "m_P",
    "planck_temperature": "T_P",
    "planck_time": "t_P",
    "proton_g_factor": "g_p",
    "proton_gyromagnetic_ratio": ("γ_p", "gamma_p"),
    "proton_magnetic_moment": ("μ_p", "mu_p"),
    "proton_mass": "m_p",
    "proton_rms_charge_radius": "r_p",
    "deuteron_rms_charge_radius": "r_d",
    "reduced_planck_constant": ("ℏ", "hbar"),
    "rydberg_constant": ("R_∞", "R_inf"),
    "second_radiation_constant": "c_2",
    "speed_of_light_in_vacuum": "c",
    "stefan_boltzmann_constant": ("σ", "sigma"),
    "standard_atmosphere": "atm",
    "standard_gravity": ("g₀", "g_0"),
    "tau_mass": ("m_τ", "m_tau"),
    "thomson_cross_section": ("σ_e", "sigma_e"),
    "triton_g_factor": "g_t",
    "triton_mass": "m_t",
    "triton_magnetic_moment": ("μ_t", "mu_t"),
    "vacuum_electric_permittivity": ("ε₀", "eps_0"),
    "von_klitzing_constant": "R_K",
    "wien_wavelength_displacement_law_constant": "b",
    "wien_frequency_displacement_law_constant": ("b′", "b_prime"),
}


@dataclass
class Relation:
    """A curated defining relation for an exact but non-terminating table value."""

    # C++ expression referencing units and previously emitted constants
    cpp: str
    # numeric equivalent in the row's own unit; receives a dict of previously computed
    # constant values (each in the SI-coherent value of its own row's unit)
    value: Callable[[dict[str, Fraction]], Fraction]
    # extra comment lines to emit above the definition
    note: str = ""


def _v90(v: dict[str, Fraction]) -> Fraction:
    """K_J-90 / K_J: the conventional-to-actual Josephson ratio (the volt-90 in V)."""
    return v["conventional_value_of_josephson_constant"] / v["josephson_constant"]


def _o90(v: dict[str, Fraction]) -> Fraction:
    """R_K / R_K-90: the actual-to-conventional von Klitzing ratio (the ohm-90 in ohm)."""
    return v["von_klitzing_constant"] / v["conventional_value_of_von_klitzing_constant"]


# Defining relations, keyed by row name; each applies in every adjustment where the row
# is exact and printed truncated. Order matters: a relation may reference constants
# defined by earlier relations of the same adjustment.
RELATIONS: dict[str, Relation] = {
    "mag. constant": Relation(
        cpp="mag<4> * mag_power<10, -7> * π * si::henry / si::metre",
        value=lambda v: 4 * PI * Fraction(10) ** -7,
        note=(
            "Exact by definition: until the 2019 redefinition the ampere was defined"
            " through this constant."
        ),
    ),
    "electric constant": Relation(
        cpp="one / magnetic_constant / square(speed_of_light_in_vacuum)",
        value=lambda v: 1
        / (v["magnetic_constant"] * v["speed_of_light_in_vacuum"] ** 2),
    ),
    "characteristic impedance of vacuum": Relation(
        cpp="magnetic_constant * speed_of_light_in_vacuum",
        value=lambda v: v["magnetic_constant"] * v["speed_of_light_in_vacuum"],
    ),
    "reduced Planck constant": Relation(
        cpp="planck_constant / (mag<2> * π)",
        value=lambda v: v["planck_constant"] / (2 * PI),
    ),
    "atomic unit of permittivity": Relation(
        cpp="mag<4> * π / magnetic_constant / square(speed_of_light_in_vacuum)",
        value=lambda v: 4
        * PI
        / (v["magnetic_constant"] * v["speed_of_light_in_vacuum"] ** 2),
        note="4 π ε₀; exact only while μ₀ was exact, hence a relation only pre-2019.",
    ),
    "elementary charge over h-bar": Relation(
        cpp="elementary_charge / reduced_planck_constant",
        value=lambda v: v["elementary_charge"] / v["reduced_planck_constant"],
    ),
    "Josephson constant": Relation(
        cpp="mag<2> * elementary_charge / planck_constant",
        value=lambda v: 2 * v["elementary_charge"] / v["planck_constant"],
    ),
    "von Klitzing constant": Relation(
        cpp="planck_constant / square(elementary_charge)",
        value=lambda v: v["planck_constant"] / v["elementary_charge"] ** 2,
    ),
    "conductance quantum": Relation(
        cpp="mag<2> * square(elementary_charge) / planck_constant",
        value=lambda v: 2 * v["elementary_charge"] ** 2 / v["planck_constant"],
    ),
    "inverse of conductance quantum": Relation(
        cpp="planck_constant / (mag<2> * square(elementary_charge))",
        value=lambda v: v["planck_constant"] / (2 * v["elementary_charge"] ** 2),
    ),
    "mag. flux quantum": Relation(
        cpp="planck_constant / (mag<2> * elementary_charge)",
        value=lambda v: v["planck_constant"] / (2 * v["elementary_charge"]),
    ),
    "Faraday constant": Relation(
        cpp="avogadro_constant * elementary_charge",
        value=lambda v: v["avogadro_constant"] * v["elementary_charge"],
    ),
    "molar gas constant": Relation(
        cpp="avogadro_constant * boltzmann_constant",
        value=lambda v: v["avogadro_constant"] * v["boltzmann_constant"],
    ),
    "molar Planck constant": Relation(
        cpp="avogadro_constant * planck_constant",
        value=lambda v: v["avogadro_constant"] * v["planck_constant"],
    ),
    "first radiation constant": Relation(
        cpp="mag<2> * π * planck_constant * square(speed_of_light_in_vacuum)",
        value=lambda v: 2
        * PI
        * v["planck_constant"]
        * v["speed_of_light_in_vacuum"] ** 2,
    ),
    "first radiation constant for spectral radiance": Relation(
        cpp="mag<2> * planck_constant * square(speed_of_light_in_vacuum) / si::steradian",
        value=lambda v: 2 * v["planck_constant"] * v["speed_of_light_in_vacuum"] ** 2,
    ),
    "second radiation constant": Relation(
        cpp="planck_constant * speed_of_light_in_vacuum / boltzmann_constant",
        value=lambda v: v["planck_constant"]
        * v["speed_of_light_in_vacuum"]
        / v["boltzmann_constant"],
    ),
    "Stefan-Boltzmann constant": Relation(
        cpp=(
            "mag_ratio<2, 15> * pow<5>(π) * pow<4>(boltzmann_constant) /"
            " cubic(planck_constant) / square(speed_of_light_in_vacuum)"
        ),
        value=lambda v: Fraction(2, 15)
        * PI**5
        * v["boltzmann_constant"] ** 4
        / v["planck_constant"] ** 3
        / v["speed_of_light_in_vacuum"] ** 2,
    ),
    "Wien wavelength displacement law constant": Relation(
        cpp="second_radiation_constant / wien_x_wavelength",
        value=lambda v: v["second_radiation_constant"] / WIEN_X_WAVELENGTH,
        note=(
            "b = c₂ / x, where x solves the transcendental equation x = 5 (1 - e^-x);"
            " see `wien_x_wavelength` above."
        ),
    ),
    "Wien frequency displacement law constant": Relation(
        cpp="wien_x_frequency * boltzmann_constant / planck_constant",
        value=lambda v: WIEN_X_FREQUENCY
        * v["boltzmann_constant"]
        / v["planck_constant"],
        note=(
            "b' = x k / h, where x solves the transcendental equation x = 3 (1 - e^-x);"
            " see `wien_x_frequency` above."
        ),
    ),
    "Loschmidt constant (273.15 K, 100 kPa)": Relation(
        cpp="mag_ratio<10'000'000, 27'315> * si::pascal / boltzmann_constant / si::kelvin",
        value=lambda v: Fraction(10_000_000, 27_315) / v["boltzmann_constant"],
    ),
    "Loschmidt constant (273.15 K, 101.325 kPa)": Relation(
        cpp="mag_ratio<10'132'500, 27'315> * si::pascal / boltzmann_constant / si::kelvin",
        value=lambda v: Fraction(10_132_500, 27_315) / v["boltzmann_constant"],
    ),
    "molar volume of ideal gas (273.15 K, 100 kPa)": Relation(
        cpp=(
            "mag_ratio<27'315, 10'000'000> * avogadro_constant * boltzmann_constant *"
            " si::kelvin / si::pascal"
        ),
        value=lambda v: Fraction(27_315, 10_000_000)
        * v["avogadro_constant"]
        * v["boltzmann_constant"],
    ),
    "molar volume of ideal gas (273.15 K, 101.325 kPa)": Relation(
        cpp=(
            "mag_ratio<27'315, 10'132'500> * avogadro_constant * boltzmann_constant *"
            " si::kelvin / si::pascal"
        ),
        value=lambda v: Fraction(27_315, 10_132_500)
        * v["avogadro_constant"]
        * v["boltzmann_constant"],
    ),
    "conventional value of ampere-90": Relation(
        cpp=(
            "conventional_value_of_josephson_constant / josephson_constant *"
            " conventional_value_of_von_klitzing_constant / von_klitzing_constant *"
            " si::ampere"
        ),
        value=lambda v: _v90(v) / _o90(v),
        note="A_90 = V_90 / Ω_90 = (K_J-90/K_J) (R_K-90/R_K) A.",
    ),
    "conventional value of coulomb-90": Relation(
        cpp=(
            "conventional_value_of_josephson_constant / josephson_constant *"
            " conventional_value_of_von_klitzing_constant / von_klitzing_constant *"
            " si::coulomb"
        ),
        value=lambda v: _v90(v) / _o90(v),
    ),
    "conventional value of farad-90": Relation(
        cpp=(
            "conventional_value_of_von_klitzing_constant / von_klitzing_constant *"
            " si::farad"
        ),
        value=lambda v: 1 / _o90(v),
    ),
    "conventional value of henry-90": Relation(
        cpp=(
            "von_klitzing_constant / conventional_value_of_von_klitzing_constant *"
            " si::henry"
        ),
        value=_o90,
    ),
    "conventional value of ohm-90": Relation(
        cpp=(
            "von_klitzing_constant / conventional_value_of_von_klitzing_constant *"
            " si::ohm"
        ),
        value=_o90,
    ),
    "conventional value of volt-90": Relation(
        cpp="conventional_value_of_josephson_constant / josephson_constant * si::volt",
        value=_v90,
    ),
    "conventional value of watt-90": Relation(
        cpp=(
            "square(conventional_value_of_josephson_constant / josephson_constant) *"
            " conventional_value_of_von_klitzing_constant / von_klitzing_constant *"
            " si::watt"
        ),
        value=lambda v: _v90(v) ** 2 / _o90(v),
        note="W_90 = V_90 A_90 = (K_J-90/K_J)² (R_K-90/R_K) W.",
    ),
}

# Prose notes preserved on specific transcribed constants.
NOTES = {
    ("2018", "vacuum mag. permeability"): (
        "Measured since the 2019 SI redefinition fixed `e` instead of the ampere."
    ),
    (
        "2022",
        "Newtonian constant of gravitation",
    ): "Unchanged from the 2018 adjustment.",
}


# --------------------------------------------------------------------------------------
# Name normalization
# --------------------------------------------------------------------------------------


def identifier(name: str) -> str:
    if name in IDENTIFIER_OVERRIDES:
        return IDENTIFIER_OVERRIDES[name]
    text = name
    text = text.replace("mom.um", "momentum")
    text = text.replace("mag.", "magnetic")
    text = text.replace("mom.", "moment")
    return re.sub(r"[^A-Za-z0-9]+", "_", text).strip("_").lower()


def symbol_expr(ident: str) -> str:
    sym = SYMBOLS.get(ident, ident)
    if isinstance(sym, tuple):
        unicode_sym, ascii_sym = sym
        return f'symbol_text{{u8"{unicode_sym}", "{ascii_sym}"}}'
    return f'"{sym}"'


# --------------------------------------------------------------------------------------
# Generation
# --------------------------------------------------------------------------------------


@dataclass
class Emitted:
    ident: str
    kind: str  # "constant" | "relation" | "alias" | "si-alias"
    row: Optional[Row]
    lines: list[str] = field(default_factory=list)

    @property
    def essential(self) -> bool:
        return self.row is not None and self.row.name in ESSENTIAL_ROWS


@dataclass
class Disposition:
    generated: list[str] = field(default_factory=list)
    aliases: list[str] = field(default_factory=list)
    variants: list[str] = field(default_factory=list)
    skips: list[tuple[str, str]] = field(default_factory=list)


def row_comment(row: Row) -> str:
    unit = f" {row.unit}" if row.unit else ""
    if row.exact:
        return f"// {row.name}: {row.value}{unit} (exact)"
    return f"// {row.name}: {row.value}{unit}, u = {row.uncertainty}{unit}"


def emit_adjustment(
    rows: list[Row], adjustment: str
) -> tuple[list[Emitted], Disposition]:
    emitted: list[Emitted] = []
    seen_idents: set[str] = set()
    disposition = Disposition()
    by_name = {row.name: row for row in rows}
    values: dict[str, Fraction] = {
        "wien_x_wavelength": WIEN_X_WAVELENGTH,
        "wien_x_frequency": WIEN_X_FREQUENCY,
    }
    idents: dict[str, str] = {}  # row name -> emitted identifier

    def add(entry: Emitted):
        if entry.ident in seen_idents:
            raise ValueError(
                f"identifier collision in codata{adjustment}: {entry.ident}"
            )
        seen_idents.add(entry.ident)
        emitted.append(entry)

    def si_alias(row: Row) -> bool:
        if row.name not in SI_ALIASES:
            return False
        if adjustment == "2014" and row.name in SI_ALIASES_MEASURED_IN_2014:
            return False
        target, expected_value = SI_ALIASES[row.name]
        if not row.exact or row.value != expected_value:
            raise ValueError(
                f"si alias mismatch for {row.name} ({adjustment}): {row.value!r}"
            )
        ident = target.rsplit("::", 1)[1]
        entry = Emitted(ident=ident, kind="si-alias", row=row)
        entry.lines = [row_comment(row), f"using {target};"]
        add(entry)
        values[ident] = number_fraction(row.value)
        idents[row.name] = ident
        disposition.aliases.append(f"{row.name} -> {target}")
        return True

    def duplicate_alias(row: Row) -> bool:
        for target_name in DUPLICATE_ALIASES.get(row.name, ()):
            target_row = by_name.get(target_name)
            if target_row is None or target_name not in idents:
                continue
            if (row.value, row.uncertainty, row.unit) != (
                target_row.value,
                target_row.uncertainty,
                target_row.unit,
            ):
                raise ValueError(
                    f"rows {row.name!r} and {target_name!r} differ in {adjustment};"
                    " the duplicate-alias curation is stale"
                )
            target_ident = idents[target_name]
            ident = identifier(row.name)
            entry = Emitted(ident=ident, kind="alias", row=row)
            entry.lines = [
                row_comment(row),
                f"inline constexpr auto {ident} = {target_ident};",
            ]
            add(entry)
            values[ident] = values[target_ident]
            idents[row.name] = ident
            disposition.aliases.append(f"{row.name} -> {target_ident}")
            return True
        return False

    def relation(row: Row) -> bool:
        rel = RELATIONS.get(row.name)
        if rel is None or not row.exact:
            return False
        ident = identifier(row.name)
        value = rel.value(values)
        entry = Emitted(ident=ident, kind="relation", row=row)
        lines = [row_comment(row)]
        if rel.note:
            lines += [f"// {rel.note}"]
        lines += [
            f"inline constexpr struct {ident} final :",
            f"    named_constant<{symbol_expr(ident)}, {rel.cpp}> {{}} {ident};",
        ]
        entry.lines = lines
        add(entry)
        values[ident] = value
        idents[row.name] = ident
        disposition.generated.append(f"{row.name} (relation)")
        return True

    def transcribed(row: Row):
        ident = identifier(row.name)
        definition = f"{magnitude_expr(row.value)} * {unit_expr(row.unit)}"
        entry = Emitted(ident=ident, kind="constant", row=row)
        lines = [row_comment(row)]
        note = NOTES.get((adjustment, row.name))
        if note:
            lines += [f"// {note}"]
        if row.exact:
            lines += [
                f"inline constexpr struct {ident} final :",
                f"    named_constant<{symbol_expr(ident)}, {definition}> {{}} {ident};",
            ]
        else:
            unc = (
                f"{uncertainty_magnitude_expr(row.uncertainty)} * {unit_expr(row.unit)}"
            )
            lines += [
                f"inline constexpr struct {ident} final :",
                f"    named_constant<{symbol_expr(ident)}, {definition},"
                f" standard_uncertainty{{{unc}}}> {{}} {ident};",
            ]
        entry.lines = lines
        add(entry)
        values[ident] = number_fraction(row.value)
        idents[row.name] = ident
        disposition.generated.append(row.name)

    # Pass 1: literal rows (measured and exact non-truncated) plus si aliases, in table
    # order. Deferred rows (relations and duplicate aliases) may reference these.
    deferred: list[Row] = []
    for row in rows:
        if row.name in ADOPTED_VALUES:
            # emitted once in codata/adopted_values.h; register the identifier so relations
            # and duplicate aliases of this adjustment may reference it
            ident = ADOPTED_VALUES[row.name]
            values[ident] = number_fraction(row.value)
            idents[row.name] = ident
            disposition.aliases.append(
                f"{row.name} -> codata::{ident} (adopted, shared)"
            )
        elif (adjustment, row.name) in YEAR_SKIPS:
            disposition.skips.append((row.name, YEAR_SKIPS[(adjustment, row.name)]))
        elif row.name in CURATED_SKIPS:
            disposition.skips.append((row.name, CURATED_SKIPS[row.name]))
        elif VARIANT_RE.search(row.name) and row.name not in IDENTIFIER_OVERRIDES:
            disposition.variants.append(row.name)
        elif si_alias(row):
            pass
        elif any(by_name.get(t) for t in DUPLICATE_ALIASES.get(row.name, ())):
            deferred.append(row)
        elif row.exact and row.truncated:
            deferred.append(row)
        else:
            transcribed(row)

    # Pass 2: relations in curated order, then duplicate aliases.
    deferred_names = {row.name for row in deferred}
    for name in RELATIONS:
        if name in deferred_names and relation(by_name[name]):
            deferred_names.discard(name)
    for row in deferred:
        if row.name in deferred_names and duplicate_alias(row):
            deferred_names.discard(row.name)
    if deferred_names:
        raise ValueError(
            f"unaccounted rows in codata{adjustment}: {sorted(deferred_names)};"
            " add a relation, alias, or curated skip"
        )

    return emitted, disposition


LICENSE = """\
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

#pragma once

// This file is GENERATED by `scripts/codata_constants.py` from the NIST "allascii"
// tables checked in under `scripts/codata/`. Do not edit it manually; rerun the script.
"""

MATH_CONSTANTS_HEADER = LICENSE + """\
//
// Solutions of the Wien displacement transcendental equations `x = n (1 - exp(-x))`
// (n = 5 for the wavelength form, n = 3 for the frequency form). Like `π`, they are
// exact mathematical constants; the truncated literals below are the approximations
// used when a conversion factor is materialized into a representation type. The Wien
// displacement law constants of the post-2019 adjustments are defined through them.

#include <mp-units/bits/hacks.h>
#include <mp-units/bits/module_macros.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework/unit.h>
#endif

MP_UNITS_EXPORT
namespace mp_units::codata {

// clang-format off
#if defined MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 18
inline constexpr struct wien_x_wavelength_c final : mag_constant<symbol_text{u8"x_λ", "x_lambda"}> {
  static constexpr auto _value_ = 4.965114231744276303698759131322893944L;
} wien_x_wavelength_c;
inline constexpr struct wien_x_frequency_c final : mag_constant<symbol_text{u8"x_ν", "x_nu"}> {
  static constexpr auto _value_ = 2.821439372122078893403191330294485195L;
} wien_x_frequency_c;
#else
inline constexpr struct wien_x_wavelength_c final :
    mag_constant<symbol_text{u8"x_λ", "x_lambda"}, 4.965114231744276303698759131322893944L> {} wien_x_wavelength_c;
inline constexpr struct wien_x_frequency_c final :
    mag_constant<symbol_text{u8"x_ν", "x_nu"}, 2.821439372122078893403191330294485195L> {} wien_x_frequency_c;
#endif
inline constexpr struct wien_x_wavelength final :
    named_constant<symbol_text{u8"x_λ", "x_lambda"}, mag<wien_x_wavelength_c> * one> {} wien_x_wavelength;
inline constexpr struct wien_x_frequency final :
    named_constant<symbol_text{u8"x_ν", "x_nu"}, mag<wien_x_frequency_c> * one> {} wien_x_frequency;
// clang-format on

}  // namespace mp_units::codata
"""

UMBRELLA_HEADER = LICENSE + """\
//
// The CODATA recommended values of the fundamental physical constants, expressed in SI
// units, with one namespace (and one header) per adjustment. Include this umbrella for
// all of them, or `mp-units/systems/codata/codataYYYY.h` for a single adjustment.
//
// This is a system of its own rather than a part of `si`. The SI fixes exactly seven
// constants (see `si2019` in `si/constants.h`, aliased in every adjustment namespace);
// everything CODATA publishes beyond those is *measured*, so its value carries a
// standard uncertainty and shifts with each adjustment. Keeping them out of `si` means
// `mp-units/systems/si.h`, which is what most translation units reach for, does not pay
// for constants that most of them never name.
//
// Measured constants declare the absolute standard uncertainty exactly as the table
// publishes it (`standard_uncertainty`); exact-by-definition values printed by NIST as
// truncated non-terminating decimals ("...") are defined by their exact symbolic
// relation instead, so nothing in these headers is rounded by us. Rows that restate a
// constant in other units ("... in MeV", "energy equivalent", unit relationships) are
// not repeated: constants are units in this library, so those forms are a conversion at
// the point of use.
//
// The most recent adjustment is `inline`, so `codata::magnetic_constant` is the current
// value and `codata::codata2018::magnetic_constant` pins a specific one. Moving that
// `inline` to a newer adjustment is a deliberate, documented change of the default.
//
// NOTE: these are *not* interchangeable with the constants of the same name in
// `mp_units::hep`. HEP declares its own system of quantities (base dimensions L, T, Q,
// E, ...; energy is a base quantity and there is no mass dimension), so its constants
// are unrelated types. The published uncertainties differ too: before the 2019
// redefinition the elementary charge was itself measured, so a mass expressed in kg and
// the same mass expressed in MeV/c² are separate CODATA table entries with different
// uncertainties.

#include <mp-units/systems/codata/codata2014.h>
#include <mp-units/systems/codata/codata2018.h>
#include <mp-units/systems/codata/codata2022.h>
"""

ADJUSTMENT_DOC = {
    "2014": "// The CODATA 2014 adjustment: the last one before the 2019 SI"
    " redefinition, so h, e,\n// k, and N_A are measured here and μ₀ is exact.",
    "2018": "// The CODATA 2018 adjustment: the basis of the 2019 SI redefinition,"
    " whose defining\n// constants are aliased from `si::si2019`.",
    "2022": "// The CODATA 2022 adjustment (the current one, hence the `inline`"
    " namespace).",
}

ESSENTIAL_DOC = (
    '// This is the `essential` tier: the NIST "Frequently used constants" selection,'
    " kept in its\n// own header because the complete table costs two orders of"
    " magnitude more to compile. The\n// full <mp-units/systems/codata/codataYYYY.h>"
    " includes this header, so both spell the very\n// same constant types and may be"
    " mixed freely."
)


def adjustment_header(adjustment: str, body: str, uses_wien: bool, tier: str) -> str:
    includes = ["#include <mp-units/bits/module_macros.h>"]
    if tier == "essential":
        includes.append("#include <mp-units/systems/codata/adopted_values.h>")
    if tier == "full":
        includes.append(
            f"#include <mp-units/systems/codata/codata{adjustment}_essential.h>"
        )
    if uses_wien:
        includes.append("#include <mp-units/systems/codata/math_constants.h>")
    includes += [
        "#include <mp-units/systems/si/constants.h>",
        "#include <mp-units/systems/si/units.h>",
    ]
    inline = "inline " if adjustment == INLINE_ADJUSTMENT else ""
    doc = ADJUSTMENT_DOC[adjustment]
    if tier == "essential":
        doc += "\n//\n" + ESSENTIAL_DOC
    return (
        LICENSE
        + "//\n"
        + doc
        + "\n"
        + "// See <mp-units/systems/codata.h> for the full description of the system.\n"
        + "\n"
        + "\n".join(includes)
        + "\n\n#ifndef MP_UNITS_IN_MODULE_INTERFACE\n"
        + "#include <mp-units/framework/unit.h>\n"
        + "#endif\n"
        + "\nMP_UNITS_EXPORT\nnamespace mp_units::codata {\n"
        + f"\n{inline}namespace codata{adjustment} {{\n"
        + "\n// clang-format off"
        + body
        + "\n// clang-format on\n"
        + f"\n}}  // namespace codata{adjustment}\n"
        + "\n}  // namespace mp_units::codata\n"
    )


ADOPTED_DOC = (
    "//\n"
    '// The NIST "adopted values": conventional constants fixed by CGPM/CIPM resolutions'
    " rather\n// than measured, identical in every CODATA adjustment (the generator verifies"
    " that). They\n// live directly in `mp_units::codata`, outside the adjustment namespaces,"
    " so entities\n// defined through them (e.g. `yard_pound`'s `pound_force` through the"
    " standard gravity)\n// keep a single stable type that does not change when the inline"
    " adjustment namespace\n// moves to a newer release."
)


def emit_adopted_values() -> str:
    tables = {adjustment: parse_table(adjustment) for adjustment in ADJUSTMENTS}
    rows_2022 = {row.name: row for row in tables["2022"]}
    entries = []
    for name, ident in ADOPTED_VALUES.items():
        row = rows_2022[name]
        for adjustment, rows in tables.items():
            other = next((r for r in rows if r.name == name), None)
            if other is None or (other.value, other.uncertainty, other.unit) != (
                row.value,
                row.uncertainty,
                row.unit,
            ):
                raise ValueError(
                    f"adopted value {name!r} differs (or is missing) in {adjustment};"
                    " it must not be shared"
                )
        assert row.exact and not row.truncated
        definition = f"{magnitude_expr(row.value)} * {unit_expr(row.unit)}"
        entries.append(
            f"{row_comment(row)}\n"
            f"inline constexpr struct {ident} final :\n"
            f"    named_constant<{symbol_expr(ident)}, {definition}> {{}} {ident};"
        )
    return (
        LICENSE
        + ADOPTED_DOC
        + "\n\n#include <mp-units/bits/module_macros.h>\n"
        + "#include <mp-units/systems/si/units.h>\n"
        + "\n#ifndef MP_UNITS_IN_MODULE_INTERFACE\n"
        + "#include <mp-units/framework/unit.h>\n"
        + "#endif\n"
        + "\nMP_UNITS_EXPORT\nnamespace mp_units::codata {\n"
        + "\n// clang-format off\n\n"
        + "\n\n".join(entries)
        + "\n// clang-format on\n"
        + "\n}  // namespace mp_units::codata\n"
    )


def generate() -> tuple[dict[str, str], dict[str, Disposition]]:
    files: dict[str, str] = {
        "codata/adopted_values.h": emit_adopted_values(),
        "codata/math_constants.h": MATH_CONSTANTS_HEADER,
        "codata.h": UMBRELLA_HEADER,
    }
    dispositions: dict[str, Disposition] = {}
    for adjustment in ADJUSTMENTS:
        rows = parse_table(adjustment)
        emitted, disposition = emit_adjustment(rows, adjustment)
        dispositions[adjustment] = disposition
        for tier, entries in (
            ("essential", [e for e in emitted if e.essential]),
            ("full", [e for e in emitted if not e.essential]),
        ):
            body = "".join("\n\n" + "\n".join(entry.lines) for entry in entries)
            uses_wien = "wien_x_" in body
            suffix = "_essential" if tier == "essential" else ""
            files[f"codata/codata{adjustment}{suffix}.h"] = adjustment_header(
                adjustment, body, uses_wien, tier
            )
    return files, dispositions


# --------------------------------------------------------------------------------------
# Verification: re-parse the generated header and recompute every value
# --------------------------------------------------------------------------------------


def evaluate_cpp_expression(expr: str, values: dict[str, Fraction]) -> Fraction:
    """Numerically evaluate an emitted C++ unit/magnitude expression.

    Coherent SI units evaluate to 1, so the result of a definition is the value in the
    row's own unit once the caller divides by the row unit's factor.
    """
    text = expr.replace("'", "")
    text = re.sub(r"mag_ratio<([^>]+)>", r"__ratio(\1)", text)
    text = re.sub(r"mag_power<10,\s*(-?\d+)>", r"(Fraction(10) ** (\1))", text)
    text = re.sub(r"mag<(\d+)>", r"Fraction(\1)", text)
    text = text.replace("π", "PI")
    text = re.sub(r"pow<(\d+)>\(", r"__pow(\1, ", text)
    text = text.replace("square(", "__pow(2, ")
    text = text.replace("cubic(", "__pow(3, ")
    for token in sorted(UNIT_TOKENS.values(), key=lambda entry: -len(entry[0])):
        cpp, factor = token
        text = text.replace(cpp, f"Fraction({factor.numerator}, {factor.denominator})")

    def ident_sub(match: re.Match) -> str:
        name = match.group(0)
        if name in ("Fraction", "PI", "__ratio", "__pow"):
            return name
        if name == "one":
            return "Fraction(1)"
        if name in values:
            return f"values[{name!r}]"
        raise ValueError(f"unknown identifier {name!r} in expression {expr!r}")

    text = re.sub(r"(?<![\w'\"])[A-Za-z_π][A-Za-z0-9_:]*", ident_sub, text)

    namespace = {
        "Fraction": Fraction,
        "PI": PI,
        "__ratio": lambda num, den=1: Fraction(num, den),
        "__pow": lambda exp, base: base**exp,
        "values": values,
    }
    return eval(text, {"__builtins__": {}}, namespace)  # noqa: S307


CONSTANT_RE = re.compile(
    r"inline constexpr struct (\w+) final :\s*named_constant<(.+?)>\s*\{\s*\}\s*\1;",
    re.DOTALL,
)
ALIAS_RE = re.compile(r"inline constexpr auto (\w+) = (\w+);")
USING_RE = re.compile(r"using si::(?:\w+::)?(\w+);")

SI_EXACT_VALUES = {
    "speed_of_light_in_vacuum": Fraction(299_792_458),
    "planck_constant": Fraction(662_607_015, 10**8) * Fraction(10) ** -34,
    "elementary_charge": Fraction(1_602_176_634, 10**9) * Fraction(10) ** -19,
    "boltzmann_constant": Fraction(1_380_649, 10**6) * Fraction(10) ** -23,
    "avogadro_constant": Fraction(602_214_076, 10**8) * Fraction(10) ** 23,
    "luminous_efficacy": Fraction(683),
    "hyperfine_structure_transition_frequency_of_cs": Fraction(9_192_631_770),
}


def split_named_constant_args(args: str) -> list[str]:
    parts = []
    depth = 0
    current: list[str] = []
    for char in args:
        if char in "<{(":
            depth += 1
        elif char in ">})":
            depth -= 1
        if char == "," and depth == 0:
            parts.append("".join(current).strip())
            current = []
        else:
            current.append(char)
    parts.append("".join(current).strip())
    return parts


def rows_by_identifier(adjustment: str) -> dict[str, Row]:
    rows = {}
    for row in parse_table(adjustment):
        ident = identifier(row.name)
        rows.setdefault(ident, row)
    return rows


def decimal_prefix_matches(value: Fraction, printed: str) -> bool:
    """Check a computed exact value against a `...`-truncated printed decimal.

    NIST truncates the non-terminating expansion, so the printed digits must be the
    truncation (or, defensively, the rounding) of the computed value at that length.
    """
    digits, exp = split_number(printed)
    negative = digits.startswith("-")
    digits = digits.lstrip("-")
    point = digits.index(".") if "." in digits else len(digits)
    plain = digits.replace(".", "")
    scaled = abs(value) / Fraction(10) ** exp / Fraction(10) ** (point - len(plain))
    truncated = int(scaled)
    rounded = int(scaled + Fraction(1, 2))
    return int(plain) in (truncated, rounded) and (value < 0) == negative


def verify(files: dict[str, str]) -> list[str]:
    """Re-parse the generated headers and compare every constant against its table row."""
    errors = []

    # the adopted-values header first: exact literals validated against the 2022 rows
    adopted_values: dict[str, Fraction] = {}
    adopted_units: dict[str, str] = {}
    rows_2022 = rows_by_identifier("2022")
    for match in CONSTANT_RE.finditer(files["codata/adopted_values.h"]):
        ident, args = match.group(1), match.group(2)
        row = rows_2022.get(ident)
        if row is None:
            errors.append(f"adopted: no table row found for {ident}")
            continue
        value = evaluate_cpp_expression(
            split_named_constant_args(args)[1], {}
        ) / unit_si_factor(row.unit)
        if value != number_fraction(row.value):
            errors.append(f"adopted: {ident} value {float(value)} != {row.value!r}")
        adopted_values[ident] = value
        adopted_units[ident] = row.unit

    # the essential tier precedes the full one, exactly as the full header includes it
    header = "".join(
        files[f"codata/codata{adjustment}_essential.h"]
        + files[f"codata/codata{adjustment}.h"]
        for adjustment in ADJUSTMENTS
    )
    chunks = re.split(r"(?:inline )?namespace (codata\d{4}) \{", header)[1:]
    state: dict[str, tuple[dict[str, Fraction], dict[str, str]]] = {}
    for namespace, body in zip(chunks[0::2], chunks[1::2]):
        adjustment = namespace.removeprefix("codata")
        rows = rows_by_identifier(adjustment)
        if adjustment not in state:
            state[adjustment] = (
                {
                    "wien_x_wavelength": WIEN_X_WAVELENGTH,
                    "wien_x_frequency": WIEN_X_FREQUENCY,
                }
                | adopted_values,
                dict(adopted_units),
            )
        values, units = state[adjustment]  # identifier -> value / row unit string

        for match in USING_RE.finditer(body):
            ident = match.group(1)
            values[ident] = SI_EXACT_VALUES[ident]
            row = rows.get(ident) or rows.get(
                {
                    "hyperfine_structure_transition_frequency_of_cs": (
                        "hyperfine_transition_frequency_of_cs_133"
                    ),
                    "standard_gravity": "standard_acceleration_of_gravity",
                }.get(ident, ident)
            )
            units[ident] = row.unit if row else ""

        for match in CONSTANT_RE.finditer(body):
            ident, args = match.group(1), match.group(2)
            parts = split_named_constant_args(args)
            definition = parts[1]
            unc_expr = None
            if len(parts) > 2:
                inner = parts[2]
                assert inner.startswith("standard_uncertainty{") and inner.endswith("}")
                unc_expr = inner[len("standard_uncertainty{") : -1]
            row = rows.get(ident)
            if row is None:
                errors.append(f"{adjustment}: no table row found for {ident}")
                continue
            row_factor = unit_si_factor(row.unit)
            si_value = evaluate_cpp_expression(definition, values)
            value = si_value / row_factor
            values[ident] = value
            units[ident] = row.unit
            if row.truncated:
                if not decimal_prefix_matches(value, row.value):
                    errors.append(
                        f"{adjustment}: {ident} = {float(value)} does not match"
                        f" truncated {row.value!r}"
                    )
            elif value != number_fraction(row.value):
                errors.append(
                    f"{adjustment}: {ident} value {float(value)} != {row.value!r}"
                )
            if unc_expr is not None:
                unc = evaluate_cpp_expression(unc_expr, values) / row_factor
                if unc != number_fraction(row.uncertainty):
                    errors.append(
                        f"{adjustment}: {ident} uncertainty {float(unc)} !="
                        f" {row.uncertainty!r}"
                    )
            elif not row.exact:
                errors.append(f"{adjustment}: measured row {ident} has no uncertainty")

        for match in ALIAS_RE.finditer(body):
            ident, target = match.group(1), match.group(2)
            if target not in values:
                errors.append(f"{adjustment}: alias {ident} -> unknown {target}")
                continue
            row = rows.get(ident)
            if row is None:
                errors.append(f"{adjustment}: no table row found for alias {ident}")
                continue
            # the duplicate-alias curation guarantees identical unit strings, so the
            # target's value is already in the alias row's own unit
            if row.truncated:
                if not decimal_prefix_matches(values[target], row.value):
                    errors.append(f"{adjustment}: alias {ident} value mismatch")
            elif number_fraction(row.value) != values[target]:
                errors.append(f"{adjustment}: alias {ident} value mismatch")
            values[ident] = values[target]
            units[ident] = units.get(target, "")
    return errors


# --------------------------------------------------------------------------------------
# Main
# --------------------------------------------------------------------------------------


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--report", action="store_true", help="print row dispositions")
    parser.add_argument(
        "--check", action="store_true", help="verify only; fail if the file differs"
    )
    args = parser.parse_args()

    files, dispositions = generate()

    if args.report:
        for adjustment, disposition in dispositions.items():
            print(f"== codata{adjustment} ==")
            print(f"  generated: {len(disposition.generated)}")
            print(f"  aliases:   {len(disposition.aliases)}")
            for alias in disposition.aliases:
                print(f"    {alias}")
            print(f"  variants:  {len(disposition.variants)}")
            print(f"  skipped:   {len(disposition.skips)}")
            for name, reason in disposition.skips:
                print(f"    {name}: {reason}")
        return 0

    errors = verify(files)
    if errors:
        for error in errors:
            print(f"VERIFY: {error}", file=sys.stderr)
        return 1

    if args.check:
        stale = [
            name
            for name, content in files.items()
            if not (SYSTEMS_DIR / name).exists()
            or (SYSTEMS_DIR / name).read_text() != content
        ]
        if stale:
            print(f"out of date: {stale}; rerun scripts/codata_constants.py")
            return 1
        print("codata headers are up to date and verified")
        return 0

    (SYSTEMS_DIR / "codata").mkdir(exist_ok=True)
    for name, content in files.items():
        (SYSTEMS_DIR / name).write_text(content)
    total = sum(len(d.generated) + len(d.aliases) for d in dispositions.values())
    print(
        f"wrote {len(files)} headers under {SYSTEMS_DIR}"
        f" ({total} constants across {len(ADJUSTMENTS)} adjustments)"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
