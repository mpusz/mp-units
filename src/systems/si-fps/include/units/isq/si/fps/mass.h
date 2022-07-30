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
#include <units/isq/dimensions/mass.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/mass.h>
#include <units/unit.h>

namespace units::isq::si::fps {

// https://en.wikipedia.org/wiki/Pound_(mass)
struct pound : named_scaled_unit<pound, "lb", mag<ratio(45'359'237, 100'000'000)>(), si::kilogram> {};

struct dim_mass : isq::dim_mass<pound> {};

template<UnitOf<dim_mass> U, Representation Rep = double>
using mass = quantity<dim_mass, U, Rep>;

struct grain : named_scaled_unit<grain, "gr", mag<ratio(1, 7000)>(), pound> {};

struct dram : named_scaled_unit<dram, "dr", mag<ratio(1, 256)>(), pound> {};

struct ounce : named_scaled_unit<ounce, "oz", mag<ratio(1, 16)>(), pound> {};

struct stone : named_scaled_unit<stone, "st", mag<14>(), pound> {};

struct quarter : named_scaled_unit<quarter, "qr", mag<28>(), pound> {};

struct hundredweight : named_scaled_unit<hundredweight, "cwt", mag<112>(), pound> {};

struct short_ton : named_scaled_unit<short_ton, "ton (short)", mag<2'000>(), pound> {};

struct long_ton : named_scaled_unit<long_ton, "ton (long)", mag<2'240>(), pound> {};

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// Grain
constexpr auto operator"" _q_gr(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<grain, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_gr(long double l) { return mass<grain, long double>(l); }

// Dram
constexpr auto operator"" _q_dr(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<dram, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_dr(long double l) { return mass<dram, long double>(l); }

// Ounce
constexpr auto operator"" _q_oz(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<ounce, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_oz(long double l) { return mass<ounce, long double>(l); }

// Pound
constexpr auto operator"" _q_lb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<pound, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_lb(long double l) { return mass<pound, long double>(l); }

// Stone
constexpr auto operator"" _q_st(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<stone, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_st(long double l) { return mass<stone, long double>(l); }

// Quarter
constexpr auto operator"" _q_qr(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<quarter, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_qr(long double l) { return mass<quarter, long double>(l); }

// Hundredweight
constexpr auto operator"" _q_cwt(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<hundredweight, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_cwt(long double l) { return mass<hundredweight, long double>(l); }

// Short ton
constexpr auto operator"" _q_ston(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<short_ton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ston(long double l) { return mass<short_ton, long double>(l); }

// Long Ton
constexpr auto operator"" _q_lton(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return mass<long_ton, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_lton(long double l) { return mass<long_ton, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace mass_references {

inline constexpr auto gr = reference<dim_mass, grain>{};
inline constexpr auto dr = reference<dim_mass, dram>{};
inline constexpr auto oz = reference<dim_mass, ounce>{};
inline constexpr auto lb = reference<dim_mass, pound>{};
inline constexpr auto st = reference<dim_mass, stone>{};
inline constexpr auto qr = reference<dim_mass, quarter>{};
inline constexpr auto cwt = reference<dim_mass, hundredweight>{};
inline constexpr auto ston = reference<dim_mass, short_ton>{};
inline constexpr auto lton = reference<dim_mass, long_ton>{};

}  // namespace mass_references

namespace references {

using namespace mass_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si::fps

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::fps::inline mass {

template<Representation Rep = double>
using gr = units::isq::si::fps::mass<units::isq::si::fps::grain, Rep>;
template<Representation Rep = double>
using dr = units::isq::si::fps::mass<units::isq::si::fps::dram, Rep>;
template<Representation Rep = double>
using oz = units::isq::si::fps::mass<units::isq::si::fps::ounce, Rep>;
template<Representation Rep = double>
using lb = units::isq::si::fps::mass<units::isq::si::fps::pound, Rep>;
template<Representation Rep = double>
using st = units::isq::si::fps::mass<units::isq::si::fps::stone, Rep>;
template<Representation Rep = double>
using qr = units::isq::si::fps::mass<units::isq::si::fps::quarter, Rep>;
template<Representation Rep = double>
using cwt = units::isq::si::fps::mass<units::isq::si::fps::hundredweight, Rep>;
template<Representation Rep = double>
using ston = units::isq::si::fps::mass<units::isq::si::fps::short_ton, Rep>;
template<Representation Rep = double>
using lton = units::isq::si::fps::mass<units::isq::si::fps::long_ton, Rep>;

}  // namespace units::aliases::isq::si::fps::inline mass

#endif  // UNITS_NO_ALIASES
