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
#include <units/isq/dimensions/pressure.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/area.h>
#include <units/isq/si/force.h>
#include <units/isq/si/prefixes.h>
#include <units/unit.h>

namespace units::isq::si {

struct pascal : named_unit<pascal, "Pa"> {};
struct yoctopascal : prefixed_unit<yoctopascal, yocto, pascal> {};
struct zeptopascal : prefixed_unit<zeptopascal, zepto, pascal> {};
struct attopascal : prefixed_unit<attopascal, atto, pascal> {};
struct femtopascal : prefixed_unit<femtopascal, femto, pascal> {};
struct picopascal : prefixed_unit<picopascal, pico, pascal> {};
struct nanopascal : prefixed_unit<nanopascal, nano, pascal> {};
struct micropascal : prefixed_unit<micropascal, micro, pascal> {};
struct millipascal : prefixed_unit<millipascal, milli, pascal> {};
struct centipascal : prefixed_unit<centipascal, centi, pascal> {};
struct decipascal : prefixed_unit<decipascal, deci, pascal> {};
struct decapascal : prefixed_unit<decapascal, deca, pascal> {};
struct hectopascal : prefixed_unit<hectopascal, hecto, pascal> {};
struct kilopascal : prefixed_unit<kilopascal, kilo, pascal> {};
struct megapascal : prefixed_unit<megapascal, mega, pascal> {};
struct gigapascal : prefixed_unit<gigapascal, giga, pascal> {};
struct terapascal : prefixed_unit<terapascal, tera, pascal> {};
struct petapascal : prefixed_unit<petapascal, peta, pascal> {};
struct exapascal : prefixed_unit<exapascal, exa, pascal> {};
struct zettapascal : prefixed_unit<zettapascal, zetta, pascal> {};
struct yottapascal : prefixed_unit<yottapascal, yotta, pascal> {};

struct dim_pressure : isq::dim_pressure<dim_pressure, pascal, dim_force, dim_area> {};

template<UnitOf<dim_pressure> U, Representation Rep = double>
using pressure = quantity<dim_pressure, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// Pa
constexpr auto operator"" _q_Pa(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return pressure<pascal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Pa(long double l) { return pressure<pascal, long double>(l); }

// yPa
constexpr auto operator"" _q_yPa(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return pressure<yoctopascal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_yPa(long double l) { return pressure<yoctopascal, long double>(l); }

// zPa
constexpr auto operator"" _q_zPa(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return pressure<zeptopascal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_zPa(long double l) { return pressure<zeptopascal, long double>(l); }

// aPa
constexpr auto operator"" _q_aPa(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return pressure<attopascal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_aPa(long double l) { return pressure<attopascal, long double>(l); }

// fPa
constexpr auto operator"" _q_fPa(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return pressure<femtopascal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_fPa(long double l) { return pressure<femtopascal, long double>(l); }

// pPa
constexpr auto operator"" _q_pPa(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return pressure<picopascal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_pPa(long double l) { return pressure<picopascal, long double>(l); }

// nPa
constexpr auto operator"" _q_nPa(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return pressure<nanopascal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_nPa(long double l) { return pressure<nanopascal, long double>(l); }

// uPa
constexpr auto operator"" _q_uPa(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return pressure<micropascal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_uPa(long double l) { return pressure<micropascal, long double>(l); }

// mPa
constexpr auto operator"" _q_mPa(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return pressure<millipascal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_mPa(long double l) { return pressure<millipascal, long double>(l); }

// cPa
constexpr auto operator"" _q_cPa(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return pressure<centipascal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_cPa(long double l) { return pressure<centipascal, long double>(l); }

// dPa
constexpr auto operator"" _q_dPa(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return pressure<decipascal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_dPa(long double l) { return pressure<decipascal, long double>(l); }

// daPa
constexpr auto operator"" _q_daPa(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return pressure<decapascal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_daPa(long double l) { return pressure<decapascal, long double>(l); }

// hPa
constexpr auto operator"" _q_hPa(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return pressure<hectopascal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_hPa(long double l) { return pressure<hectopascal, long double>(l); }

// kPa
constexpr auto operator"" _q_kPa(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return pressure<kilopascal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_kPa(long double l) { return pressure<kilopascal, long double>(l); }

// MPa
constexpr auto operator"" _q_MPa(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return pressure<megapascal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_MPa(long double l) { return pressure<megapascal, long double>(l); }

// GPa
constexpr auto operator"" _q_GPa(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return pressure<gigapascal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_GPa(long double l) { return pressure<gigapascal, long double>(l); }

// TPa
constexpr auto operator"" _q_TPa(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return pressure<terapascal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_TPa(long double l) { return pressure<terapascal, long double>(l); }

// PPa
constexpr auto operator"" _q_PPa(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return pressure<petapascal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_PPa(long double l) { return pressure<petapascal, long double>(l); }

// EPa
constexpr auto operator"" _q_EPa(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return pressure<exapascal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_EPa(long double l) { return pressure<exapascal, long double>(l); }

// ZPa
constexpr auto operator"" _q_ZPa(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return pressure<zettapascal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ZPa(long double l) { return pressure<zettapascal, long double>(l); }

// YPa
constexpr auto operator"" _q_YPa(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return pressure<yottapascal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_YPa(long double l) { return pressure<yottapascal, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace pressure_references {

inline constexpr auto Pa = reference<dim_pressure, pascal>{};
inline constexpr auto yPa = reference<dim_pressure, yoctopascal>{};
inline constexpr auto zPa = reference<dim_pressure, zeptopascal>{};
inline constexpr auto aPa = reference<dim_pressure, attopascal>{};
inline constexpr auto fPa = reference<dim_pressure, femtopascal>{};
inline constexpr auto pPa = reference<dim_pressure, picopascal>{};
inline constexpr auto nPa = reference<dim_pressure, nanopascal>{};
inline constexpr auto uPa = reference<dim_pressure, micropascal>{};
inline constexpr auto mPa = reference<dim_pressure, millipascal>{};
inline constexpr auto cPa = reference<dim_pressure, centipascal>{};
inline constexpr auto dPa = reference<dim_pressure, decipascal>{};
inline constexpr auto daPa = reference<dim_pressure, decapascal>{};
inline constexpr auto hPa = reference<dim_pressure, hectopascal>{};
inline constexpr auto kPa = reference<dim_pressure, kilopascal>{};
inline constexpr auto MPa = reference<dim_pressure, megapascal>{};
inline constexpr auto GPa = reference<dim_pressure, gigapascal>{};
inline constexpr auto TPa = reference<dim_pressure, terapascal>{};
inline constexpr auto PPa = reference<dim_pressure, petapascal>{};
inline constexpr auto EPa = reference<dim_pressure, exapascal>{};
inline constexpr auto ZPa = reference<dim_pressure, zettapascal>{};
inline constexpr auto YPa = reference<dim_pressure, yottapascal>{};

}  // namespace pressure_references

namespace references {

using namespace pressure_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline pressure {

template<Representation Rep = double>
using Pa = units::isq::si::pressure<units::isq::si::pascal, Rep>;
template<Representation Rep = double>
using yPa = units::isq::si::pressure<units::isq::si::yoctopascal, Rep>;
template<Representation Rep = double>
using zPa = units::isq::si::pressure<units::isq::si::zeptopascal, Rep>;
template<Representation Rep = double>
using aPa = units::isq::si::pressure<units::isq::si::attopascal, Rep>;
template<Representation Rep = double>
using fPa = units::isq::si::pressure<units::isq::si::femtopascal, Rep>;
template<Representation Rep = double>
using pPa = units::isq::si::pressure<units::isq::si::picopascal, Rep>;
template<Representation Rep = double>
using nPa = units::isq::si::pressure<units::isq::si::nanopascal, Rep>;
template<Representation Rep = double>
using uPa = units::isq::si::pressure<units::isq::si::micropascal, Rep>;
template<Representation Rep = double>
using mPa = units::isq::si::pressure<units::isq::si::millipascal, Rep>;
template<Representation Rep = double>
using cPa = units::isq::si::pressure<units::isq::si::centipascal, Rep>;
template<Representation Rep = double>
using dPa = units::isq::si::pressure<units::isq::si::decipascal, Rep>;
template<Representation Rep = double>
using daPa = units::isq::si::pressure<units::isq::si::decapascal, Rep>;
template<Representation Rep = double>
using hPa = units::isq::si::pressure<units::isq::si::hectopascal, Rep>;
template<Representation Rep = double>
using kPa = units::isq::si::pressure<units::isq::si::kilopascal, Rep>;
template<Representation Rep = double>
using MPa = units::isq::si::pressure<units::isq::si::megapascal, Rep>;
template<Representation Rep = double>
using GPa = units::isq::si::pressure<units::isq::si::gigapascal, Rep>;
template<Representation Rep = double>
using TPa = units::isq::si::pressure<units::isq::si::terapascal, Rep>;
template<Representation Rep = double>
using PPa = units::isq::si::pressure<units::isq::si::petapascal, Rep>;
template<Representation Rep = double>
using EPa = units::isq::si::pressure<units::isq::si::exapascal, Rep>;
template<Representation Rep = double>
using ZPa = units::isq::si::pressure<units::isq::si::zettapascal, Rep>;
template<Representation Rep = double>
using YPa = units::isq::si::pressure<units::isq::si::yottapascal, Rep>;

}  // namespace units::aliases::isq::si::inline pressure

#endif  // UNITS_NO_ALIASES
