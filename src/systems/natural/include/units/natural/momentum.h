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

#include <units/isq/natural/mass.h>
#include <units/isq/natural/speed.h>

// IWYU pragma: begin_exports
#include <units/isq/dimensions/momentum.h>
#include <units/isq/natural/units.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

namespace units::isq::natural {

struct dim_momentum : isq::dim_momentum<dim_momentum, gigaelectronvolt, dim_mass, dim_speed> {};

template<UnitOf<dim_momentum> U, Representation Rep = double>
using momentum = quantity<dim_momentum, U, Rep>;

#ifndef UNITS_NO_REFERENCES

namespace momentum_references {

inline constexpr auto GeV = reference<dim_momentum, gigaelectronvolt>{};

}  // namespace momentum_references

namespace references {

using namespace momentum_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::natural

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::natural::inline momentum {

template<Representation Rep = double>
using GeV = units::isq::natural::momentum<units::isq::natural::gigaelectronvolt, Rep>;

}  // namespace units::aliases::isq::natural::inline momentum

#endif  // UNITS_NO_ALIASES
