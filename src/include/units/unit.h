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
#include <units/bits/fixed_string.h>
#include <ratio>

namespace units {

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
  concept Unit =
      std::is_empty_v<T> &&
      detail::is_unit<downcast_base_t<T>>;

  // make_derived_unit

  namespace detail {

    template<typename D>
    struct get_unit_base_dim;

    template<typename E, typename... Rest>
    struct get_unit_base_dim<dimension<E, Rest...>> {
      static_assert(sizeof...(Rest) == 0, "Base unit expected");
      using dimension = E::dimension;
    };

    template<BaseDimension BD, typename... Us>
    struct get_ratio {
      using ratio = ::units::ratio<1>;
    };

    template<BaseDimension BD, typename U, typename... Rest>
    struct get_ratio<BD, U, Rest...> {
      using unit_base_dim = get_unit_base_dim<typename U::dimension::base_type>::dimension;
      using ratio = conditional<std::is_same_v<unit_base_dim, BD>, typename U::ratio,
                                typename get_ratio<BD, Rest...>::ratio>;
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
      using ratio = ::units::ratio<1>;
    };

    template<typename E, typename... Rest, typename... Us>
    struct derived_ratio<dimension<E, Rest...>, Us...> {
      using rest_ratio = derived_ratio<dimension<Rest...>, Us...>::ratio;
      using e_ratio = get_ratio<typename E::dimension, Us...>::ratio;
      using ratio = ratio_op<rest_ratio, E::num, E::den, e_ratio>::ratio;
    };

    template<Dimension D, Unit... Us>
    using make_derived_unit = unit<D, typename detail::derived_ratio<typename D::base_type, Us...>::ratio>;

  }

  // derived_unit

  //   TODO gcc:92101
  // Gated by the following gcc bug
  // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=92101
// template<typename Child, fixed_string Symbol, typename...>
  // struct derived_unit;
  
  // template<typename Child, fixed_string Symbol, Dimension D>
  // struct derived_unit<Child, Symbol, D> : downcast_helper<Child, unit<D, ratio<1>>> {
  //   static constexpr auto symbol = Symbol;
  // };

  // template<typename Child, fixed_string Symbol, Dimension D, Ratio R>
  // struct derived_unit<Child, Symbol, D, R> : downcast_helper<Child, unit<D, R>> {
  //   static constexpr auto symbol = Symbol;
  // };

  // template<typename Child, fixed_string Symbol, Unit U>
  // struct derived_unit<Child, Symbol, U> : downcast_helper<Child, U> {
  //   static constexpr auto symbol = Symbol;
  // };

  // template<typename Child, fixed_string Symbol, Dimension D, Unit U, Unit... Us>
  // struct derived_unit<Child, Symbol, D, U, Us...> : downcast_helper<Child, detail::make_derived_unit<D, U, Us...>> {
  //   static constexpr auto symbol = Symbol;
  // };

  template<typename Child, typename Symbol, typename...>
  struct derived_unit;
  
  template<typename Child, typename Symbol, Dimension D>
  struct derived_unit<Child, Symbol, D> : downcast_helper<Child, unit<D, ratio<1>>> {
    static constexpr auto symbol = Symbol::c_str();
  };

  template<typename Child, typename Symbol, Dimension D, Ratio R>
  struct derived_unit<Child, Symbol, D, R> : downcast_helper<Child, unit<D, R>> {
    static constexpr auto symbol = Symbol::c_str();
  };

  template<typename Child, typename Symbol, Unit U>
  struct derived_unit<Child, Symbol, U> : downcast_helper<Child, U> {
    static constexpr auto symbol = Symbol::c_str();
  };

  template<typename Child, typename Symbol, Dimension D, Unit U, Unit... Us>
  struct derived_unit<Child, Symbol, D, U, Us...> : downcast_helper<Child, detail::make_derived_unit<D, U, Us...>> {
    static constexpr auto symbol = Symbol::c_str();
  };

  // SI prefixes

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

}  // namespace units
