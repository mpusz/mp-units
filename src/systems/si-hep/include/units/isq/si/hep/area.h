// The MIT License (MIT)
//
// Copyright (c) 2021 Mateusz Pusz
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
#include <units/isq/dimensions/area.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/area.h>
#include <units/unit.h>

namespace units::isq::si::hep {

// effective cross-sectional area according to EU council directive 80/181/EEC
// https://eur-lex.europa.eu/legal-content/EN/TXT/PDF/?uri=CELEX:01980L0181-20090527#page=10
// https://www.fedlex.admin.ch/eli/cc/1994/3109_3109_3109/de
struct barn : named_scaled_unit<barn, "b", mag_power<10, -28>(), square_metre> {};
struct yocto_barn : prefixed_unit<yocto_barn, yocto, barn> {};
struct zepto_barn : prefixed_unit<zepto_barn, zepto, barn> {};
struct atto_barn : prefixed_unit<atto_barn, atto, barn> {};
struct femto_barn : prefixed_unit<femto_barn, femto, barn> {};
struct pico_barn : prefixed_unit<pico_barn, pico, barn> {};
struct nano_barn : prefixed_unit<nano_barn, nano, barn> {};
struct micro_barn : prefixed_unit<micro_barn, micro, barn> {};
struct milli_barn : prefixed_unit<milli_barn, milli, barn> {};

#ifndef UNITS_NO_LITERALS

inline namespace literals {
constexpr auto operator"" _q_yb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<yocto_barn, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_yb(long double l) { return area<yocto_barn, long double>(l); }
constexpr auto operator"" _q_zb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<zepto_barn, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_zb(long double l) { return area<zepto_barn, long double>(l); }
constexpr auto operator"" _q_ab(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<atto_barn, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ab(long double l) { return area<atto_barn, long double>(l); }
constexpr auto operator"" _q_fb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<femto_barn, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_fb(long double l) { return area<femto_barn, long double>(l); }
constexpr auto operator"" _q_pb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<pico_barn, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_pb(long double l) { return area<pico_barn, long double>(l); }
constexpr auto operator"" _q_nb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<nano_barn, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_nb(long double l) { return area<nano_barn, long double>(l); }
constexpr auto operator"" _q_ub(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<micro_barn, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ub(long double l) { return area<micro_barn, long double>(l); }
constexpr auto operator"" _q_mb(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<milli_barn, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_mb(long double l) { return area<milli_barn, long double>(l); }
constexpr auto operator"" _q_b(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return area<barn, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_b(long double l) { return area<barn, long double>(l); }
}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace area_references {

inline constexpr auto barn = reference<dim_area, ::units::isq::si::hep::barn>{};

}  // namespace area_references

namespace references {

using namespace area_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si::hep

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::hep::inline area {

template<Representation Rep = double>
using barn = units::isq::si::area<units::isq::si::hep::barn, Rep>;

}  // namespace units::aliases::isq::si::hep::inline area

#endif  // UNITS_NO_ALIASES
