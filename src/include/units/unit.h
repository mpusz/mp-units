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

#include <units/dimension.h>
#include <units/ratio.h>
#include <ratio>

namespace std::experimental::units {

  template<Dimension D, Ratio R = ratio<1>>
    requires (R::num * R::den > 0)
  struct unit : downcast_base<unit<D, R>> {
    using dimension = D;
    using ratio = R;
  };

  // is_unit

  namespace detail {

    template<typename T>
    inline constexpr bool is_unit = false;

    template<typename D, typename R>
    inline constexpr bool is_unit<unit<D, R>> = true;

  }

  template<typename T>
  concept bool Unit =
      std::is_empty_v<T> &&
      detail::is_unit<downcast_from<T>>;


  // derived_unit

  namespace detail {

    template<typename D>
    struct get_unit_base_dim;

    template<typename E, typename... Rest>
    struct get_unit_base_dim<dimension<E, Rest...>> {
      static_assert(sizeof...(Rest) == 0, "Base unit expected");
      static constexpr const base_dimension& dimension = E::dimension;
    };

    template<const base_dimension& BaseDimension, typename... Us>
    struct get_ratio {
      using ratio = ::std::experimental::units::ratio<1>;
    };

    template<const base_dimension& BaseDimension, typename U, typename... Rest>
    struct get_ratio<BaseDimension, U, Rest...> {
      static constexpr const base_dimension& unit_base_dim = get_unit_base_dim<typename U::dimension::base_type>::dimension;
      using ratio = conditional<&unit_base_dim == &BaseDimension, typename U::ratio,
                                typename get_ratio<BaseDimension, Rest...>::ratio>;
    };

    template<typename Result, int UnitExpNum, int UnitExpDen, typename UnitRatio>
    struct ratio_op;

    template<typename Result, int UnitExpDen, typename UnitRatio>
    struct ratio_op<Result, 0, UnitExpDen, UnitRatio> {
      using ratio = Result;
    };

    template<typename Result, int UnitExpNum, int UnitExpDen, typename UnitRatio>
    struct ratio_op {
      using calc_ratio = conditional<(UnitExpNum * UnitExpDen > 0), ratio_multiply<Result, UnitRatio>,
                                     ratio_divide<Result, UnitRatio>>;
      static constexpr int value = (UnitExpNum * UnitExpDen > 0) ? (UnitExpNum - UnitExpDen) : (UnitExpNum + UnitExpDen);
      using ratio = ratio_op<calc_ratio, value, UnitExpDen, UnitRatio>::ratio;
    };

    template<typename D, typename... Us>
    struct derived_ratio;

    template<typename... Us>
    struct derived_ratio<dimension<>, Us...> {
      using ratio = ::std::experimental::units::ratio<1>;
    };

    template<typename E, typename... Rest, typename... Us>
    struct derived_ratio<dimension<E, Rest...>, Us...> {
      using rest_ratio = derived_ratio<dimension<Rest...>, Us...>::ratio;
      using e_ratio = get_ratio<E::dimension, Us...>::ratio;
      using ratio = ratio_op<rest_ratio, E::num, E::den, e_ratio>::ratio;
    };

  }

  template<Dimension D, Unit... Us>
  using derived_unit = unit<D, typename detail::derived_ratio<typename D::base_type, Us...>::ratio>;

  // prefixes
  template<Unit U> using atto = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<1, std::atto::den>>>;
  template<Unit U> using femto = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<1, std::femto::den>>>;
  template<Unit U> using pico = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<1, std::pico::den>>>;
  template<Unit U> using nano = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<1, std::nano::den>>>;
  template<Unit U> using micro = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<1, std::micro::den>>>;
  template<Unit U> using milli = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<1, std::milli::den>>>;
  template<Unit U> using centi = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<1, std::centi::den>>>;
  template<Unit U> using deca = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<std::deca::num>>>;
  template<Unit U> using hecto = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<std::hecto::num>>>;
  template<Unit U> using kilo = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<std::kilo::num>>>;
  template<Unit U> using mega = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<std::mega::num>>>;
  template<Unit U> using giga = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<std::giga::num>>>;
  template<Unit U> using tera = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<std::tera::num>>>;
  template<Unit U> using peta = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<std::peta::num>>>;
  template<Unit U> using exa = unit<typename U::dimension, ratio_multiply<typename U::ratio, ratio<std::exa::num>>>;

}  // namespace std::experimental::units
