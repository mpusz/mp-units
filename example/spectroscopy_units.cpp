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

#include <mp-units/format.h>
#include <mp-units/systems/isq/isq.h>
#include <mp-units/systems/si/si.h>
#include <iostream>
#include <tuple>

// This example implements a table of units provided in the following article
// http://cds.cern.ch/record/1481609/files/978-3-642-18018-7_BookBackMatter.pdf

using namespace mp_units;

// import selected unit symbols only
using mp_units::si::unit_symbols::cm;
using mp_units::si::unit_symbols::eV;
using mp_units::si::unit_symbols::K;
using mp_units::si::unit_symbols::kJ;
using mp_units::si::unit_symbols::mol;
using mp_units::si::unit_symbols::THz;
using mp_units::si::unit_symbols::um;

// physical constants
constexpr auto c = 1 * si::si2019::speed_of_light_in_vacuum;
constexpr auto h = 1 * si::si2019::planck_constant;
constexpr auto kb = 1 * si::si2019::boltzmann_constant;

// prints quantities in the resulting unit
template<QuantityOf<isq::energy> T1, QuantityOf<isq::wavenumber> T2, QuantityOf<isq::frequency> T3,
         QuantityOf<isq::thermodynamic_temperature> T4, QuantityOf<isq::wavelength> T5>
void print_line(const std::tuple<T1, T2, T3, T4, T5>& t)
{
  MP_UNITS_STD_FMT::println("| {:<15} | {:<15} | {:<15} | {:<15} | {:<15} |", std::get<0>(t), std::get<1>(t),
                            std::get<2>(t), std::get<3>(t), std::get<4>(t));
}

// prints quantities in semi-SI units
// (eV is not an official SI unit)
template<QuantityOf<isq::energy> T1, QuantityOf<isq::wavenumber> T2, QuantityOf<isq::frequency> T3,
         QuantityOf<isq::thermodynamic_temperature> T4, QuantityOf<isq::wavelength> T5>
void print_line_si(const std::tuple<T1, T2, T3, T4, T5>& t)
{
  MP_UNITS_STD_FMT::println("| {:<15} | {:<15} | {:<15} | {:<15} | {:<15} |", std::get<0>(t).in(eV),
                            std::get<1>(t).in(one / cm), std::get<2>(t).in(THz), std::get<3>(t).in(K),
                            std::get<4>(t).in(um));
}

int main()
{
  const auto q1 = isq::energy(1. * eV);
  const auto t1 = std::make_tuple(q1, isq::wavenumber(q1 / (h * c)), isq::frequency(q1 / h),
                                  isq::thermodynamic_temperature(q1 / kb), isq::wavelength(h * c / q1));

  const auto q2 = 1. * isq::wavenumber[one / cm];
  const auto t2 = std::make_tuple(isq::energy(q2 * h * c), q2, isq::frequency(q2 * c),
                                  isq::thermodynamic_temperature(q2 * h * c / kb), isq::wavelength(1 / q2));

  const auto q3 = isq::frequency(1. * THz);
  const auto t3 = std::make_tuple(isq::energy(q3 * h), isq::wavenumber(q3 / c), q3,
                                  isq::thermodynamic_temperature(q3 * h / kb), isq::wavelength(c / q3));

  const auto q4 = isq::thermodynamic_temperature(1. * K);
  const auto t4 = std::make_tuple(isq::energy(q4 * kb), isq::wavenumber(q4 * kb / (h * c)), isq::frequency(q4 * kb / h),
                                  q4, isq::wavelength(h * c / (q4 * kb)));

  const auto q5 = isq::wavelength(1. * um);
  const auto t5 = std::make_tuple(isq::energy(h * c / q5), isq::wavenumber(1 / q5), isq::frequency(c / q5),
                                  isq::thermodynamic_temperature(h * c / (q5 * kb)), q5);

  MP_UNITS_STD_FMT::println("| {:<15} | {:<15} | {:<15} | {:<15} | {:<15} |", "Energy", "Wavenumber", "Frequency",
                            "Temperature", "Wavelength");
  MP_UNITS_STD_FMT::println("| {0:-^15} | {0:-^15} | {0:-^15} | {0:-^15} | {0:-^15} |", "");
  print_line(t1);
  print_line(t2);
  print_line(t3);
  print_line(t4);
  print_line(t5);

  MP_UNITS_STD_FMT::println("| {0:-^15} | {0:-^15} | {0:-^15} | {0:-^15} | {0:-^15} |", "");
  print_line_si(t1);
  print_line_si(t2);
  print_line_si(t3);
  print_line_si(t4);
  print_line_si(t5);
}
