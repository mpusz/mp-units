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

#include <units/dimensions/si_base_dimensions.h>
#include <units/dimensions/si_prefixes.h>

namespace units {

  // base dimension

  template<basic_fixed_string Name>
  struct base_dimension {
    static constexpr auto name = Name;
  };

  template<typename T>
  concept BaseDimension = true;

  // base dimension

  template<basic_fixed_string Symbol, BaseDimension Dim, PrefixType PT, Ratio R = ratio<1>>
  struct base_unit {
    static constexpr auto symbol = Symbol;
    using dimension = Dim;
    using prefix_type = PT;
    using ratio = R;
  };

  template<typename T>
  concept BaseUnit = true;

  template<typename T, typename Dim>
  concept BaseUnitOf = BaseUnit<T> && BaseDimension<Dim> && std::same_as<typename T::dimension, Dim>;



  namespace physical {

    // base dimensions
    struct base_dim_length : base_dimension<"length"> {};
    struct base_dim_mass : base_dimension<"mass"> {};
    struct base_dim_time : base_dimension<"time"> {};
    struct base_dim_current : base_dimension<"current"> {};
    struct base_dim_temperature : base_dimension<"temperature"> {};
    struct base_dim_substance : base_dimension<"substance"> {};
    struct base_dim_luminous_intensity : base_dimension<"luminous intensity"> {};


    // dimensions
    template<UnitOf<base_dim_length> L>
    struct length : derived_dimension<length, exp<L, 1>> {};

    template<UnitOf<base_dim_mass> M>
    struct mass : derived_dimension<mass, exp<M, 1>> {};

    template<UnitOf<base_dim_time> T>
    struct time : derived_dimension<time, exp<T, 1>> {};

    template<UnitOf<base_dim_length> L, UnitOf<base_dim_time> T>
    struct velocity : derived_dimension<velocity, exp<L, 1>, exp<T, -1>> {};

    template<UnitOf<base_dim_length> L, UnitOf<base_dim_time> T>
    struct acceleration : derived_dimension<acceleration, exp<L, 1>, exp<T, -2>> {};

    template<UnitOf<base_dim_mass> M, UnitOf<acceleration> A>
    struct force : derived_dimension<force, exp<M, 1>, exp<A, 1>> {};

  } // physical
  
  // SI
  namespace si {
    struct si_prefix;

    // length
    struct metre : base_unit<"m", base_dim_length, si_prefix> {};
    struct length : physical::length<metre> {};

    // mass
    struct kilogram : base_unit<"kg", base_dim_mass, si_prefix> {};
    struct mass : physical::mass<kilogram> {};

    // time
    struct second : base_unit<"s", base_dim_time, si_prefix> {};
    struct time : physical::time<second> {};

    struct nanosecond : prefixed_derived_unit<nanosecond, nano, second> {};
    struct microsecond : prefixed_derived_unit<microsecond, micro, second> {};
    struct millisecond : prefixed_derived_unit<millisecond, milli, second> {};
    struct minute : named_derived_unit<minute, time, "min", ratio<60>> {};
    struct hour : named_derived_unit<hour, time, "h", ratio<3600>> {};

    // velocity
    struct velocity : physical::velocity<metre, second>;

    // acceleration
    struct acceleration : physical::acceleration<metre, second>;

    // acceleration
    struct acceleration : physical::acceleration<metre, second>;

  }

}  // namespace units
