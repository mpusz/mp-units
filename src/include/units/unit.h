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

namespace units {

  template<Dimension D, Ratio R>
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

  template<typename PrefixType, Ratio R, basic_fixed_string Symbol>
  struct prefix {
    using prefix_type = PrefixType;
    using ratio = R;
    static constexpr auto symbol = Symbol;
  };


  // TODO gcc:92150
  // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=92150
  // namespace detail {

  //   template<typename T>
  //   inline constexpr bool is_prefix = false;

  //   template<typename PrefixType, Ratio R, basic_fixed_string Symbol>
  //   inline constexpr bool is_prefix<prefix<PrefixType, R, Symbol>> = true;

  // }  // namespace detail

  template<typename T>
//  concept Prefix = detail::is_prefix<T>;
  concept Prefix = true;

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

  struct no_prefix;

  template<typename Child, basic_fixed_string Symbol, Dimension D, typename PrefixType = no_prefix>
  struct coherent_derived_unit : downcast_helper<Child, unit<D, ratio<1>>> {
    static constexpr auto symbol = Symbol;
    using prefix_type = PrefixType;
  };

  template<typename Child, basic_fixed_string Symbol, Dimension D, Ratio R>
  struct derived_unit : downcast_helper<Child, unit<D, R>> {
    static constexpr auto symbol = Symbol;
  };

  template<typename Child, Prefix P, Unit U>
    requires requires { U::symbol; } && std::same_as<typename U::prefix_type, typename P::prefix_type>
  struct prefixed_derived_unit : downcast_helper<Child, unit<typename U::dimension, ratio_multiply<typename P::ratio, typename U::ratio>>> {
    static constexpr auto symbol = P::symbol + U::symbol;
    using prefix_type = P::prefix_type;
  };

  template<typename Child, basic_fixed_string Symbol, Dimension D, Unit U, Unit... Us>
  struct deduced_derived_unit : downcast_helper<Child, detail::make_derived_unit<D, U, Us...>> {
    static constexpr auto symbol = Symbol;
  };

}  // namespace units
