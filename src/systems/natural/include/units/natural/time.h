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
#include <units/isq/dimensions/time.h>
#include <units/isq/natural/units.h>
#include <units/quantity.h>
#include <units/reference.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

namespace units::isq::natural {

struct dim_time : isq::dim_time<inverted_gigaelectronvolt> {};

template<UnitOf<dim_time> U, Representation Rep = double>
using time = quantity<dim_time, U, Rep>;

#ifndef UNITS_NO_REFERENCES

namespace time_references {

inline constexpr auto inv_GeV = reference<dim_time, inverted_gigaelectronvolt>{};

}  // namespace time_references

namespace references {

using namespace time_references;

}  // namespace references

#endif  // UNITS_NO_REFERENCES

}  // namespace units::isq::natural

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::natural::inline time {

template<Representation Rep = double>
using inv_GeV = units::isq::natural::time<units::isq::natural::inverted_gigaelectronvolt, Rep>;

}  // namespace units::aliases::isq::natural::inline time

#endif  // UNITS_NO_ALIASES
