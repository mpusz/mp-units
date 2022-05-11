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
#include <units/isq/dimensions/volume.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/international/length.h>
#include <units/isq/si/volume.h>
#include <units/unit.h>

namespace units::isq::si::international {

struct cubic_foot : derived_scaled_unit<cubic_foot, si::dim_volume, si::international::foot> {};

#ifndef UNITS_NO_LITERALS

inline namespace literals {

// ft3
constexpr auto operator"" _q_ft3(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return si::volume<cubic_foot, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ft3(long double l) { return si::volume<cubic_foot, long double>(l); }

}  // namespace literals

#endif  // UNITS_NO_LITERALS

#ifndef UNITS_NO_REFERENCES

namespace volume_references {

inline constexpr auto ft3 = reference<si::dim_volume, cubic_foot>{};

}  // namespace volume_references

namespace references {

using namespace volume_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::si::international

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::si::international::inline volume {

template<Representation Rep = double>
using ft3 = units::isq::si::volume<units::isq::si::international::cubic_foot, Rep>;

}  // namespace units::aliases::isq::si::international::inline volume

#endif  // UNITS_NO_ALIASES
