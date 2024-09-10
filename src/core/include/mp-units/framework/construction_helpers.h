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

#include <mp-units/framework/quantity.h>
#include <mp-units/framework/quantity_point.h>
#include <mp-units/framework/reference_concepts.h>
#include <mp-units/framework/representation_concepts.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <type_traits>
#endif
#endif

namespace mp_units {

template<Reference R>
struct delta_ {
  template<typename FwdRep, RepresentationOf<get_quantity_spec(R{}).character> Rep = std::remove_cvref_t<FwdRep>>
  [[nodiscard]] constexpr quantity<MP_UNITS_EXPRESSION_WORKAROUND(R{}), Rep> operator()(FwdRep&& lhs) const
  {
    return quantity{std::forward<FwdRep>(lhs), R{}};
  }
};

template<Reference R>
struct absolute_ {
  template<typename FwdRep, RepresentationOf<get_quantity_spec(R{}).character> Rep = std::remove_cvref_t<FwdRep>>
  [[nodiscard]] constexpr quantity_point<MP_UNITS_EXPRESSION_WORKAROUND(R{}), default_point_origin(R{}), Rep>
  operator()(FwdRep&& lhs) const
  {
    return quantity_point{quantity{std::forward<FwdRep>(lhs), R{}}};
  }
};

MP_UNITS_EXPORT_BEGIN

template<Reference auto R>
constexpr delta_<MP_UNITS_REMOVE_CONST(decltype(R))> delta{};

template<Reference auto R>
constexpr absolute_<MP_UNITS_REMOVE_CONST(decltype(R))> absolute{};

MP_UNITS_EXPORT_END

}  // namespace mp_units
