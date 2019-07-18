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

#include <units/base_dimensions.h>
#include <units/quantity.h>

namespace units {

  struct dimension_mass : make_dimension_t<exp<base_dim_mass, 1>> {};
  template<> struct downcasting_traits<downcast_from<dimension_mass>> : downcast_to<dimension_mass> {};

  template<typename T>
  concept bool Mass = Quantity<T> && std::Same<typename T::dimension, dimension_mass>;

  template<Unit U = class kilogram, Number Rep = double>
  using mass = quantity<dimension_mass, U, Rep>;

  struct gram : unit<dimension_mass, ratio<1, 1000>> {};
  template<> struct downcasting_traits<downcast_from<gram>> : downcast_to<gram> {};

  struct kilogram : kilo<gram> {};
  template<> struct downcasting_traits<downcast_from<kilogram>> : downcast_to<kilogram> {};

  inline namespace literals {

    // g
    constexpr auto operator""_g(unsigned long long l) { return mass<gram, std::int64_t>(l); }
    constexpr auto operator""_g(long double l) { return mass<gram, long double>(l); }

    // kg
    constexpr auto operator""_kg(unsigned long long l) { return mass<kilogram, std::int64_t>(l); }
    constexpr auto operator""_kg(long double l) { return mass<kilogram, long double>(l); }

  }  // namespace literals

}  // namespace units
