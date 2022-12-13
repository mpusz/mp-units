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
#include <units/isq/dimensions/absorbed_dose.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/energy.h>
#include <units/isq/si/prefixes.h>
#include <units/unit.h>

namespace units::isq::si {

struct gray : named_unit<gray, "Gy"> {};
struct yoctogray : prefixed_unit<yoctogray, yocto, gray> {};
struct zeptogray : prefixed_unit<zeptogray, zepto, gray> {};
struct attogray : prefixed_unit<attogray, atto, gray> {};
struct femtogray : prefixed_unit<femtogray, femto, gray> {};
struct picogray : prefixed_unit<picogray, pico, gray> {};
struct nanogray : prefixed_unit<nanogray, nano, gray> {};
struct microgray : prefixed_unit<microgray, micro, gray> {};
struct milligray : prefixed_unit<milligray, milli, gray> {};
struct centigray : prefixed_unit<centigray, centi, gray> {};
struct decigray : prefixed_unit<decigray, deci, gray> {};
struct decagray : prefixed_unit<decagray, deca, gray> {};
struct hectogray : prefixed_unit<hectogray, hecto, gray> {};
struct kilogray : prefixed_unit<kilogray, kilo, gray> {};
struct megagray : prefixed_unit<megagray, mega, gray> {};
struct gigagray : prefixed_unit<gigagray, giga, gray> {};
struct teragray : prefixed_unit<teragray, tera, gray> {};
struct petagray : prefixed_unit<petagray, peta, gray> {};
struct exagray : prefixed_unit<exagray, exa, gray> {};
struct zettagray : prefixed_unit<zettagray, zetta, gray> {};
struct yottagray : prefixed_unit<yottagray, yotta, gray> {};

struct dim_absorbed_dose : isq::dim_absorbed_dose<dim_absorbed_dose, gray, dim_energy, dim_mass> {};

template<UnitOf<dim_absorbed_dose> U, Representation Rep = double>
using absorbed_dose = quantity<dim_absorbed_dose, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// Gy
constexpr auto operator"" _q_Gy(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return absorbed_dose<gray, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Gy(long double l) { return absorbed_dose<gray, long double>(l); }

// yGy
constexpr auto operator"" _q_yGy(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return absorbed_dose<yoctogray, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_yGy(long double l) { return absorbed_dose<yoctogray, long double>(l); }

// zGy
constexpr auto operator"" _q_zGy(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return absorbed_dose<zeptogray, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_zGy(long double l) { return absorbed_dose<zeptogray, long double>(l); }

// aGy
constexpr auto operator"" _q_aGy(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return absorbed_dose<attogray, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_aGy(long double l) { return absorbed_dose<attogray, long double>(l); }

// fGy
constexpr auto operator"" _q_fGy(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return absorbed_dose<femtogray, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_fGy(long double l) { return absorbed_dose<femtogray, long double>(l); }

// pGy
constexpr auto operator"" _q_pGy(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return absorbed_dose<picogray, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_pGy(long double l) { return absorbed_dose<picogray, long double>(l); }

// nGy
constexpr auto operator"" _q_nGy(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return absorbed_dose<nanogray, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_nGy(long double l) { return absorbed_dose<nanogray, long double>(l); }

// uGy
constexpr auto operator"" _q_uGy(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return absorbed_dose<microgray, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_uGy(long double l) { return absorbed_dose<microgray, long double>(l); }

// mGy
constexpr auto operator"" _q_mGy(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return absorbed_dose<milligray, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_mGy(long double l) { return absorbed_dose<milligray, long double>(l); }

// cGy
constexpr auto operator"" _q_cGy(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return absorbed_dose<centigray, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_cGy(long double l) { return absorbed_dose<centigray, long double>(l); }

// dGy
constexpr auto operator"" _q_dGy(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return absorbed_dose<decigray, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_dGy(long double l) { return absorbed_dose<decigray, long double>(l); }

// daGy
constexpr auto operator"" _q_daGy(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return absorbed_dose<decagray, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_daGy(long double l) { return absorbed_dose<decagray, long double>(l); }

// hGy
constexpr auto operator"" _q_hGy(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return absorbed_dose<hectogray, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_hGy(long double l) { return absorbed_dose<hectogray, long double>(l); }

// kGy
constexpr auto operator"" _q_kGy(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return absorbed_dose<kilogray, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_kGy(long double l) { return absorbed_dose<kilogray, long double>(l); }

// MGy
constexpr auto operator"" _q_MGy(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return absorbed_dose<megagray, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_MGy(long double l) { return absorbed_dose<megagray, long double>(l); }

// GGy
constexpr auto operator"" _q_GGy(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return absorbed_dose<gigagray, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_GGy(long double l) { return absorbed_dose<gigagray, long double>(l); }

// TGy
constexpr auto operator"" _q_TGy(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return absorbed_dose<teragray, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_TGy(long double l) { return absorbed_dose<teragray, long double>(l); }

// PGy
constexpr auto operator"" _q_PGy(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return absorbed_dose<petagray, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_PGy(long double l) { return absorbed_dose<petagray, long double>(l); }

// EGy
constexpr auto operator"" _q_EGy(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return absorbed_dose<exagray, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_EGy(long double l) { return absorbed_dose<exagray, long double>(l); }

// ZGy
constexpr auto operator"" _q_ZGy(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return absorbed_dose<zettagray, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ZGy(long double l) { return absorbed_dose<zettagray, long double>(l); }

// YGy
constexpr auto operator"" _q_YGy(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return absorbed_dose<yottagray, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_YGy(long double l) { return absorbed_dose<yottagray, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace absorbed_dose_references {

inline constexpr auto Gy = reference<dim_absorbed_dose, gray>{};
inline constexpr auto yGy = reference<dim_absorbed_dose, yoctogray>{};
inline constexpr auto zGy = reference<dim_absorbed_dose, zeptogray>{};
inline constexpr auto aGy = reference<dim_absorbed_dose, attogray>{};
inline constexpr auto fGy = reference<dim_absorbed_dose, femtogray>{};
inline constexpr auto pGy = reference<dim_absorbed_dose, picogray>{};
inline constexpr auto nGy = reference<dim_absorbed_dose, nanogray>{};
inline constexpr auto uGy = reference<dim_absorbed_dose, microgray>{};
inline constexpr auto mGy = reference<dim_absorbed_dose, milligray>{};
inline constexpr auto cGy = reference<dim_absorbed_dose, centigray>{};
inline constexpr auto dGy = reference<dim_absorbed_dose, decigray>{};
inline constexpr auto daGy = reference<dim_absorbed_dose, decagray>{};
inline constexpr auto hGy = reference<dim_absorbed_dose, hectogray>{};
inline constexpr auto kGy = reference<dim_absorbed_dose, kilogray>{};
inline constexpr auto MGy = reference<dim_absorbed_dose, megagray>{};
inline constexpr auto GGy = reference<dim_absorbed_dose, gigagray>{};
inline constexpr auto TGy = reference<dim_absorbed_dose, teragray>{};
inline constexpr auto PGy = reference<dim_absorbed_dose, petagray>{};
inline constexpr auto EGy = reference<dim_absorbed_dose, exagray>{};
inline constexpr auto ZGy = reference<dim_absorbed_dose, zettagray>{};
inline constexpr auto YGy = reference<dim_absorbed_dose, yottagray>{};

}  // namespace absorbed_dose_references

namespace references {

using namespace absorbed_dose_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline absorbed_dose {

template<Representation Rep = double>
using Gy = units::isq::si::absorbed_dose<units::isq::si::gray, Rep>;
template<Representation Rep = double>
using yGy = units::isq::si::absorbed_dose<units::isq::si::yoctogray, Rep>;
template<Representation Rep = double>
using zGy = units::isq::si::absorbed_dose<units::isq::si::zeptogray, Rep>;
template<Representation Rep = double>
using aGy = units::isq::si::absorbed_dose<units::isq::si::attogray, Rep>;
template<Representation Rep = double>
using fGy = units::isq::si::absorbed_dose<units::isq::si::femtogray, Rep>;
template<Representation Rep = double>
using pGy = units::isq::si::absorbed_dose<units::isq::si::picogray, Rep>;
template<Representation Rep = double>
using nGy = units::isq::si::absorbed_dose<units::isq::si::nanogray, Rep>;
template<Representation Rep = double>
using uGy = units::isq::si::absorbed_dose<units::isq::si::microgray, Rep>;
template<Representation Rep = double>
using mGy = units::isq::si::absorbed_dose<units::isq::si::milligray, Rep>;
template<Representation Rep = double>
using cGy = units::isq::si::absorbed_dose<units::isq::si::centigray, Rep>;
template<Representation Rep = double>
using dGy = units::isq::si::absorbed_dose<units::isq::si::decigray, Rep>;
template<Representation Rep = double>
using daGy = units::isq::si::absorbed_dose<units::isq::si::decagray, Rep>;
template<Representation Rep = double>
using hGy = units::isq::si::absorbed_dose<units::isq::si::hectogray, Rep>;
template<Representation Rep = double>
using kGy = units::isq::si::absorbed_dose<units::isq::si::kilogray, Rep>;
template<Representation Rep = double>
using MGy = units::isq::si::absorbed_dose<units::isq::si::megagray, Rep>;
template<Representation Rep = double>
using GGy = units::isq::si::absorbed_dose<units::isq::si::gigagray, Rep>;
template<Representation Rep = double>
using TGy = units::isq::si::absorbed_dose<units::isq::si::teragray, Rep>;
template<Representation Rep = double>
using PGy = units::isq::si::absorbed_dose<units::isq::si::petagray, Rep>;
template<Representation Rep = double>
using EGy = units::isq::si::absorbed_dose<units::isq::si::exagray, Rep>;
template<Representation Rep = double>
using ZGy = units::isq::si::absorbed_dose<units::isq::si::zettagray, Rep>;
template<Representation Rep = double>
using YGy = units::isq::si::absorbed_dose<units::isq::si::yottagray, Rep>;

}  // namespace units::aliases::isq::si::inline absorbed_dose

#endif  // UNITS_NO_ALIASES
