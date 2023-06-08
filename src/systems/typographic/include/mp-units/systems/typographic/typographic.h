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

#include <mp-units/systems/international/international.h>
#include <mp-units/unit.h>

namespace mp_units::typographic {

// clang-format off
// https://en.wikipedia.org/wiki/Point_(typography)
inline constexpr struct pica_us : named_unit<"pica(us)", mag<ratio{166'044, 1'000'000}> * international::inch> {} pica_us;
inline constexpr struct point_us : named_unit<"point(us)", mag<ratio{1, 12}> * pica_us> {} point_us;

inline constexpr struct point_dtp : named_unit<"point(dtp)", mag<ratio{1, 72}> * international::inch> {} point_dtp;
inline constexpr struct pica_dtp : named_unit<"pica(dtp)", mag<12> * point_dtp> {} pica_dtp;
// clang-format on

}  // namespace mp_units::typographic
