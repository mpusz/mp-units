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
#include <units/isq/dimensions/force.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/fps/acceleration.h>
#include <units/isq/si/fps/mass.h>
#include <units/isq/si/prefixes.h>
#include <units/unit.h>

namespace units::isq::si::fps {

// https://en.wikipedia.org/wiki/Poundal
struct poundal : named_unit<poundal, "pdl"> {};

// https://en.wikipedia.org/wiki/Pound_(force)
struct pound_force : named_scaled_unit<pound_force, "lbf", mag<ratio(32'174'049, 1'000'000)>(), poundal> {};

struct kilopound_force : prefixed_unit<kilopound_force, si::kilo, pound_force> {};

// https://en.wikipedia.org/wiki/Kip_(unit),
struct kip : alias_unit<kilopound_force, "klbf"> {};

struct dim_force : isq::dim_force<dim_force, poundal, dim_mass, dim_acceleration> {};

template<UnitOf<dim_force> U, Representation Rep = double>
using force = quantity<dim_force, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// poundal
constexpr auto operator"" _q_pdl(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<poundal, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_pdl(long double l) { return force<poundal, long double>(l); }

// pound force
constexpr auto operator"" _q_lbf(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<pound_force, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_lbf(long double l) { return force<pound_force, long double>(l); }

// kilopound force
constexpr auto operator"" _q_klbf(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return force<kilopound_force, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_klbf(long double l) { return force<kilopound_force, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace force_references {

inline constexpr auto pdl = reference<dim_force, poundal>{};
inline constexpr auto lbf = reference<dim_force, pound_force>{};
inline constexpr auto klbf = reference<dim_force, kilopound_force>{};

}  // namespace force_references

namespace references {

using namespace force_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si::fps

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::fps::inline force {

template<Representation Rep = double>
using pdl = units::isq::si::fps::force<units::isq::si::fps::poundal, Rep>;
template<Representation Rep = double>
using lbf = units::isq::si::fps::force<units::isq::si::fps::pound_force, Rep>;
template<Representation Rep = double>
using klbf = units::isq::si::fps::force<units::isq::si::fps::kilopound_force, Rep>;

}  // namespace units::aliases::isq::si::fps::inline force

#endif  // UNITS_NO_ALIASES
