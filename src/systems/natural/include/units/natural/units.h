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
#include <units/symbol_text.h>
// IWYU pragma: end_exports

#include <units/isq/si/prefixes.h>
#include <units/unit.h>

namespace units::isq::natural {

struct electronvolt : named_unit<electronvolt, "eV"> {};
struct gigaelectronvolt : prefixed_unit<gigaelectronvolt, si::giga, electronvolt> {};
struct inverted_gigaelectronvolt : named_unit<inverted_gigaelectronvolt, basic_symbol_text{"GeV⁻¹", "GeV^-1"}> {};
struct square_gigaelectronvolt : named_unit<square_gigaelectronvolt, basic_symbol_text{"GeV²", "GeV^2"}> {};

// NOTE: eV as a base unit with no relation to joule prevents us from going back
// from natural units to SI. Do we need such a support or should we treat
// natural units as an isolated island with ev = 1 which simplifies all
// the maths a lot?

}  // namespace units::isq::natural
