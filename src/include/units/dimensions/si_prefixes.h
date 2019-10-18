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

#include <units/unit.h>
#include <units/format.h>

namespace units {

  // prefix tags
  struct si_prefix;

  // SI prefixes

  struct atto : prefix<atto, si_prefix, ratio<1, std::atto::den>, "a"> {};
  struct femto : prefix<femto, si_prefix, ratio<1, std::femto::den>, "f"> {};
  struct pico : prefix<pico, si_prefix, ratio<1, std::pico::den>, "p"> {};
  struct nano : prefix<nano, si_prefix, ratio<1, std::nano::den>, "n"> {};
  struct micro : prefix<micro, si_prefix, ratio<1, std::micro::den>, "µ"> {};
  struct milli : prefix<milli, si_prefix, ratio<1, std::milli::den>, "m"> {};
  struct centi : prefix<centi, si_prefix, ratio<1, std::centi::den>, "c"> {};
  struct deci : prefix<deci, si_prefix, ratio<1, std::deci::den>, "d"> {};
  struct deca : prefix<deca, si_prefix, ratio<std::deca::num>, "da"> {};
  struct hecto : prefix<hecto, si_prefix, ratio<std::hecto::num>, "h"> {};
  struct kilo : prefix<kilo, si_prefix, ratio<std::kilo::num>, "k"> {};
  struct mega : prefix<mega, si_prefix, ratio<std::mega::num>, "M"> {};
  struct giga : prefix<giga, si_prefix, ratio<std::giga::num>, "G"> {};
  struct tera : prefix<tera, si_prefix, ratio<std::tera::num>, "T"> {};
  struct peta : prefix<peta, si_prefix, ratio<std::peta::num>, "P"> {};
  struct exa : prefix<exa, si_prefix, ratio<std::exa::num>, "E"> {};

}
