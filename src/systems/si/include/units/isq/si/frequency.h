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
#include <units/isq/dimensions/frequency.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/time.h>
#include <units/unit.h>

namespace units::isq::si {

struct hertz : named_unit<hertz, "Hz"> {};
struct yoctohertz : prefixed_unit<yoctohertz, yocto, hertz> {};
struct zeptohertz : prefixed_unit<zeptohertz, zepto, hertz> {};
struct attohertz : prefixed_unit<attohertz, atto, hertz> {};
struct femtohertz : prefixed_unit<femtohertz, femto, hertz> {};
struct picohertz : prefixed_unit<picohertz, pico, hertz> {};
struct nanohertz : prefixed_unit<nanohertz, nano, hertz> {};
struct microhertz : prefixed_unit<microhertz, micro, hertz> {};
struct millihertz : prefixed_unit<millihertz, milli, hertz> {};
struct kilohertz : prefixed_unit<kilohertz, kilo, hertz> {};
struct megahertz : prefixed_unit<megahertz, mega, hertz> {};
struct gigahertz : prefixed_unit<gigahertz, giga, hertz> {};
struct terahertz : prefixed_unit<terahertz, tera, hertz> {};
struct petahertz : prefixed_unit<petahertz, peta, hertz> {};
struct exahertz : prefixed_unit<exahertz, exa, hertz> {};
struct zettahertz : prefixed_unit<zettahertz, zetta, hertz> {};
struct yottahertz : prefixed_unit<yottahertz, yotta, hertz> {};

struct dim_frequency : isq::dim_frequency<dim_frequency, hertz, dim_time> {};

template<UnitOf<dim_frequency> U, Representation Rep = double>
using frequency = quantity<dim_frequency, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// Hz
constexpr auto operator"" _q_Hz(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return frequency<hertz, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_Hz(long double l) { return frequency<hertz, long double>(l); }

// yHz
constexpr auto operator"" _q_yHz(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return frequency<yoctohertz, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_yHz(long double l) { return frequency<yoctohertz, long double>(l); }

// zHz
constexpr auto operator"" _q_zHz(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return frequency<zeptohertz, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_zHz(long double l) { return frequency<zeptohertz, long double>(l); }

// aHz
constexpr auto operator"" _q_aHz(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return frequency<attohertz, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_aHz(long double l) { return frequency<attohertz, long double>(l); }

// fHz
constexpr auto operator"" _q_fHz(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return frequency<femtohertz, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_fHz(long double l) { return frequency<femtohertz, long double>(l); }

// pHz
constexpr auto operator"" _q_pHz(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return frequency<picohertz, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_pHz(long double l) { return frequency<picohertz, long double>(l); }

// nHz
constexpr auto operator"" _q_nHz(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return frequency<nanohertz, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_nHz(long double l) { return frequency<nanohertz, long double>(l); }

// uHz
constexpr auto operator"" _q_uHz(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return frequency<microhertz, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_uHz(long double l) { return frequency<microhertz, long double>(l); }

// mHz
constexpr auto operator"" _q_mHz(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return frequency<millihertz, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_mHz(long double l) { return frequency<millihertz, long double>(l); }

// kHz
constexpr auto operator"" _q_kHz(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return frequency<kilohertz, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_kHz(long double l) { return frequency<kilohertz, long double>(l); }

// MHz
constexpr auto operator"" _q_MHz(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return frequency<megahertz, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_MHz(long double l) { return frequency<megahertz, long double>(l); }

// GHz
constexpr auto operator"" _q_GHz(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return frequency<gigahertz, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_GHz(long double l) { return frequency<gigahertz, long double>(l); }

// THz
constexpr auto operator"" _q_THz(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return frequency<terahertz, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_THz(long double l) { return frequency<terahertz, long double>(l); }

// PHz
constexpr auto operator"" _q_PHz(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return frequency<petahertz, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_PHz(long double l) { return frequency<petahertz, long double>(l); }

// EHz
constexpr auto operator"" _q_EHz(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return frequency<exahertz, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_EHz(long double l) { return frequency<exahertz, long double>(l); }

// ZHz
constexpr auto operator"" _q_ZHz(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return frequency<zettahertz, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ZHz(long double l) { return frequency<zettahertz, long double>(l); }

// YHz
constexpr auto operator"" _q_YHz(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return frequency<yottahertz, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_YHz(long double l) { return frequency<yottahertz, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace frequency_references {

inline constexpr auto Hz = reference<dim_frequency, hertz>{};
inline constexpr auto yHz = reference<dim_frequency, yoctohertz>{};
inline constexpr auto zHz = reference<dim_frequency, zeptohertz>{};
inline constexpr auto aHz = reference<dim_frequency, attohertz>{};
inline constexpr auto fHz = reference<dim_frequency, femtohertz>{};
inline constexpr auto pHz = reference<dim_frequency, picohertz>{};
inline constexpr auto nHz = reference<dim_frequency, nanohertz>{};
inline constexpr auto uHz = reference<dim_frequency, microhertz>{};
inline constexpr auto mHz = reference<dim_frequency, millihertz>{};
inline constexpr auto kHz = reference<dim_frequency, kilohertz>{};
inline constexpr auto MHz = reference<dim_frequency, megahertz>{};
inline constexpr auto GHz = reference<dim_frequency, gigahertz>{};
inline constexpr auto THz = reference<dim_frequency, terahertz>{};
inline constexpr auto PHz = reference<dim_frequency, petahertz>{};
inline constexpr auto EHz = reference<dim_frequency, exahertz>{};
inline constexpr auto ZHz = reference<dim_frequency, zettahertz>{};
inline constexpr auto YHz = reference<dim_frequency, yottahertz>{};

}  // namespace frequency_references

namespace references {

using namespace frequency_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::inline frequency {

template<Representation Rep = double>
using Hz = units::isq::si::frequency<units::isq::si::hertz, Rep>;
template<Representation Rep = double>
using yHz = units::isq::si::frequency<units::isq::si::yoctohertz, Rep>;
template<Representation Rep = double>
using zHz = units::isq::si::frequency<units::isq::si::zeptohertz, Rep>;
template<Representation Rep = double>
using aHz = units::isq::si::frequency<units::isq::si::attohertz, Rep>;
template<Representation Rep = double>
using fHz = units::isq::si::frequency<units::isq::si::femtohertz, Rep>;
template<Representation Rep = double>
using pHz = units::isq::si::frequency<units::isq::si::picohertz, Rep>;
template<Representation Rep = double>
using nHz = units::isq::si::frequency<units::isq::si::nanohertz, Rep>;
template<Representation Rep = double>
using uHz = units::isq::si::frequency<units::isq::si::microhertz, Rep>;
template<Representation Rep = double>
using mHz = units::isq::si::frequency<units::isq::si::millihertz, Rep>;
template<Representation Rep = double>
using kHz = units::isq::si::frequency<units::isq::si::kilohertz, Rep>;
template<Representation Rep = double>
using MHz = units::isq::si::frequency<units::isq::si::megahertz, Rep>;
template<Representation Rep = double>
using GHz = units::isq::si::frequency<units::isq::si::gigahertz, Rep>;
template<Representation Rep = double>
using THz = units::isq::si::frequency<units::isq::si::terahertz, Rep>;
template<Representation Rep = double>
using PHz = units::isq::si::frequency<units::isq::si::petahertz, Rep>;
template<Representation Rep = double>
using EHz = units::isq::si::frequency<units::isq::si::exahertz, Rep>;
template<Representation Rep = double>
using ZHz = units::isq::si::frequency<units::isq::si::zettahertz, Rep>;
template<Representation Rep = double>
using YHz = units::isq::si::frequency<units::isq::si::yottahertz, Rep>;

}  // namespace units::aliases::isq::si::inline frequency

#endif  // UNITS_NO_ALIASES
