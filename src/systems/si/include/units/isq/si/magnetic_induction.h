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

// IWYU pragma: begin_exports
#include <units/isq/dimensions/magnetic_induction.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/length.h>
#include <units/isq/si/prefixes.h>
#include <units/isq/si/time.h>
#include <units/isq/si/voltage.h>
#include <units/unit.h>

namespace units::isq::si {

struct tesla : named_unit<tesla, "T"> {};

struct yoctotesla : prefixed_unit<yoctotesla, yocto, tesla> {};
struct zeptotesla : prefixed_unit<zeptotesla, zepto, tesla> {};
struct attotesla : prefixed_unit<attotesla, atto, tesla> {};
struct femtotesla : prefixed_unit<femtotesla, femto, tesla> {};
struct picotesla : prefixed_unit<picotesla, pico, tesla> {};
struct nanotesla : prefixed_unit<nanotesla, nano, tesla> {};
struct microtesla : prefixed_unit<microtesla, micro, tesla> {};
struct millitesla : prefixed_unit<millitesla, milli, tesla> {};
struct kilotesla : prefixed_unit<kilotesla, kilo, tesla> {};
struct megatesla : prefixed_unit<megatesla, mega, tesla> {};
struct gigatesla : prefixed_unit<gigatesla, giga, tesla> {};
struct teratesla : prefixed_unit<teratesla, tera, tesla> {};
struct petatesla : prefixed_unit<petatesla, peta, tesla> {};
struct exatesla : prefixed_unit<exatesla, exa, tesla> {};
struct zettatesla : prefixed_unit<zettatesla, zetta, tesla> {};
struct yottatesla : prefixed_unit<yottatesla, yotta, tesla> {};

struct gauss : named_scaled_unit<gauss, "G", mag<ratio(1, 10'000)>(), tesla> {};

struct dim_magnetic_induction :
    isq::dim_magnetic_induction<dim_magnetic_induction, tesla, dim_voltage, dim_time, dim_length> {};

template<UnitOf<dim_magnetic_induction> U, Representation Rep = double>
using magnetic_induction = quantity<dim_magnetic_induction, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// T
constexpr auto operator"" _q_T(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_induction<tesla, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_T(long double l) { return magnetic_induction<tesla, long double>(l); }

// yT
constexpr auto operator"" _q_yT(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_induction<yoctotesla, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_yT(long double l) { return magnetic_induction<yoctotesla, long double>(l); }

// zT
constexpr auto operator"" _q_zT(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_induction<zeptotesla, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_zT(long double l) { return magnetic_induction<zeptotesla, long double>(l); }

// aT
constexpr auto operator"" _q_aT(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_induction<attotesla, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_aT(long double l) { return magnetic_induction<attotesla, long double>(l); }

// fT
constexpr auto operator"" _q_fT(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_induction<femtotesla, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_fT(long double l) { return magnetic_induction<femtotesla, long double>(l); }

// pT
constexpr auto operator"" _q_pT(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_induction<picotesla, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_pT(long double l) { return magnetic_induction<picotesla, long double>(l); }

// nT
constexpr auto operator"" _q_nT(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_induction<nanotesla, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_nT(long double l) { return magnetic_induction<nanotesla, long double>(l); }

// µT
constexpr auto operator"" _q_uT(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_induction<microtesla, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_uT(long double l) { return magnetic_induction<microtesla, long double>(l); }

// mT
constexpr auto operator"" _q_mT(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_induction<millitesla, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_mT(long double l) { return magnetic_induction<millitesla, long double>(l); }

// kT
constexpr auto operator"" _q_kT(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_induction<kilotesla, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_kT(long double l) { return magnetic_induction<kilotesla, long double>(l); }

// MT
constexpr auto operator"" _q_MT(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_induction<megatesla, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_MT(long double l) { return magnetic_induction<megatesla, long double>(l); }

// GT
constexpr auto operator"" _q_GT(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_induction<gigatesla, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_GT(long double l) { return magnetic_induction<gigatesla, long double>(l); }

// TT
constexpr auto operator"" _q_TT(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_induction<teratesla, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_TT(long double l) { return magnetic_induction<teratesla, long double>(l); }

// PT
constexpr auto operator"" _q_PT(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_induction<petatesla, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_PT(long double l) { return magnetic_induction<petatesla, long double>(l); }

// ET
constexpr auto operator"" _q_ET(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_induction<exatesla, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ET(long double l) { return magnetic_induction<exatesla, long double>(l); }

// ZT
constexpr auto operator"" _q_ZT(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_induction<zettatesla, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ZT(long double l) { return magnetic_induction<zettatesla, long double>(l); }

// YT
constexpr auto operator"" _q_YT(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_induction<yottatesla, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_YT(long double l) { return magnetic_induction<yottatesla, long double>(l); }

// G
constexpr auto operator"" _q_G(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return magnetic_induction<gauss, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_G(long double l) { return magnetic_induction<gauss, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace magnetic_induction_references {

inline constexpr auto T = reference<dim_magnetic_induction, tesla>{};
inline constexpr auto yT = reference<dim_magnetic_induction, yoctotesla>{};
inline constexpr auto zT = reference<dim_magnetic_induction, zeptotesla>{};
inline constexpr auto aT = reference<dim_magnetic_induction, attotesla>{};
inline constexpr auto fT = reference<dim_magnetic_induction, femtotesla>{};
inline constexpr auto pT = reference<dim_magnetic_induction, picotesla>{};
inline constexpr auto nT = reference<dim_magnetic_induction, nanotesla>{};
inline constexpr auto uT = reference<dim_magnetic_induction, microtesla>{};
inline constexpr auto mT = reference<dim_magnetic_induction, millitesla>{};
inline constexpr auto kT = reference<dim_magnetic_induction, kilotesla>{};
inline constexpr auto MT = reference<dim_magnetic_induction, megatesla>{};
inline constexpr auto GT = reference<dim_magnetic_induction, gigatesla>{};
inline constexpr auto TT = reference<dim_magnetic_induction, teratesla>{};
inline constexpr auto PT = reference<dim_magnetic_induction, petatesla>{};
inline constexpr auto ET = reference<dim_magnetic_induction, exatesla>{};
inline constexpr auto ZT = reference<dim_magnetic_induction, zettatesla>{};
inline constexpr auto YT = reference<dim_magnetic_induction, yottatesla>{};
inline constexpr auto G = reference<dim_magnetic_induction, gauss>{};

}  // namespace magnetic_induction_references

namespace references {

using namespace magnetic_induction_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline magnetic_induction {

template<Representation Rep = double>
using T = units::isq::si::magnetic_induction<units::isq::si::tesla, Rep>;
template<Representation Rep = double>
using yT = units::isq::si::magnetic_induction<units::isq::si::yoctotesla, Rep>;
template<Representation Rep = double>
using zT = units::isq::si::magnetic_induction<units::isq::si::zeptotesla, Rep>;
template<Representation Rep = double>
using aT = units::isq::si::magnetic_induction<units::isq::si::attotesla, Rep>;
template<Representation Rep = double>
using fT = units::isq::si::magnetic_induction<units::isq::si::femtotesla, Rep>;
template<Representation Rep = double>
using pT = units::isq::si::magnetic_induction<units::isq::si::picotesla, Rep>;
template<Representation Rep = double>
using nT = units::isq::si::magnetic_induction<units::isq::si::nanotesla, Rep>;
template<Representation Rep = double>
using uT = units::isq::si::magnetic_induction<units::isq::si::microtesla, Rep>;
template<Representation Rep = double>
using mT = units::isq::si::magnetic_induction<units::isq::si::millitesla, Rep>;
template<Representation Rep = double>
using kT = units::isq::si::magnetic_induction<units::isq::si::kilotesla, Rep>;
template<Representation Rep = double>
using MT = units::isq::si::magnetic_induction<units::isq::si::megatesla, Rep>;
template<Representation Rep = double>
using GT = units::isq::si::magnetic_induction<units::isq::si::gigatesla, Rep>;
template<Representation Rep = double>
using TT = units::isq::si::magnetic_induction<units::isq::si::teratesla, Rep>;
template<Representation Rep = double>
using PT = units::isq::si::magnetic_induction<units::isq::si::petatesla, Rep>;
template<Representation Rep = double>
using ET = units::isq::si::magnetic_induction<units::isq::si::exatesla, Rep>;
template<Representation Rep = double>
using ZT = units::isq::si::magnetic_induction<units::isq::si::zettatesla, Rep>;
template<Representation Rep = double>
using YT = units::isq::si::magnetic_induction<units::isq::si::yottatesla, Rep>;
template<Representation Rep = double>
using G = units::isq::si::magnetic_induction<units::isq::si::gauss, Rep>;

}  // namespace units::aliases::isq::si::inline magnetic_induction

#endif  // UNITS_NO_ALIASES
