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
#include <units/prefix.h>
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

  namespace detail {

    template<int Value>
      requires (0 <= Value) && (Value < 10)
    inline constexpr basic_fixed_string superscript_number = "\u2070";

//    template<> inline constexpr basic_fixed_string superscript_number<0> = "\u2070";
    template<> inline constexpr basic_fixed_string superscript_number<1> = "\u00b9";
    template<> inline constexpr basic_fixed_string superscript_number<2> = "\u00b2";
    template<> inline constexpr basic_fixed_string superscript_number<3> = "\u00b3";
    template<> inline constexpr basic_fixed_string superscript_number<4> = "\u2074";
    template<> inline constexpr basic_fixed_string superscript_number<5> = "\u2075";
    template<> inline constexpr basic_fixed_string superscript_number<6> = "\u2076";
    template<> inline constexpr basic_fixed_string superscript_number<7> = "\u2077";
    template<> inline constexpr basic_fixed_string superscript_number<8> = "\u2078";
    template<> inline constexpr basic_fixed_string superscript_number<9> = "\u2079";

    template<int Value>
      requires (Value >= 0)
    constexpr auto superscript()
    {
      if constexpr(Value < 10)
        return superscript_number<Value>;
      else
        return superscript<Value / 10>() + superscript<Value % 10>();
    }

    template<int Value>
      requires (Value >= 0)
    constexpr auto regular()
    {
      if constexpr(Value < 10)
        return basic_fixed_string(static_cast<char>('0' + Value));
      else
        return regular<Value / 10>() + regular<Value % 10>();
    }


    template<typename Ratio>
    constexpr auto ratio_text()
    {
      if constexpr(Ratio::num != 1 || Ratio::den != 1) {
        auto txt = basic_fixed_string("[") + regular<Ratio::num>();
        if constexpr(Ratio::den == 1) {
          return txt + basic_fixed_string("]");
        }
        else {
          return txt + basic_fixed_string("/") + regular<Ratio::den>() + basic_fixed_string("]");
        }
      }
      else {
        return basic_fixed_string("");
      }
    }

    template<typename Ratio, typename PrefixType>
    constexpr auto prefix_or_ratio_text()
    {
      if constexpr(Ratio::num != 1 || Ratio::den != 1) {
        if constexpr (!std::same_as<PrefixType, no_prefix>) {
          using prefix = downcast<detail::prefix_base<PrefixType, Ratio>>;

          if constexpr(!std::same_as<prefix, prefix_base<PrefixType, Ratio>>) {
            // print as a prefixed unit
            return prefix::symbol;
          }
          else {
            // print as a ratio of the coherent unit
            return ratio_text<Ratio>();
          }
        }
        else {
          // print as a ratio of the coherent unit
          return ratio_text<Ratio>();
        }
      }
    }


    template<bool Divide, std::size_t Idx>
    constexpr auto operator_text()
    {
      if constexpr(Idx == 0) {
        if constexpr(Divide) {
          return basic_fixed_string("1/");
        }
        else {
          return basic_fixed_string("");
        }
      }
      else {
        if constexpr(Divide) {
          return basic_fixed_string("/");
        }
        else {
          return basic_fixed_string("⋅");
        }
      }
    }

    template<typename E, std::size_t Idx>
    constexpr auto exp_text()
    {
      // get calculation operator + symbol
      const auto txt = operator_text<E::num < 0, Idx>() + E::dimension::symbol;
      if constexpr(E::den != 1) {
        // add root part
        return txt + basic_fixed_string("^(") + regular<abs(E::num)>() + basic_fixed_string("/") + regular<E::den>() + basic_fixed_string(")");
      }
      else if constexpr(abs(E::num) != 1) {
        // add exponent part
        return txt + superscript<abs(E::num)>();
      }
      else {
        return txt;
      }
    }

    template<typename... Es, std::size_t... Idxs>
    constexpr auto symbol_text_impl(dimension<Es...>, std::index_sequence<Idxs...>)
    {
      return (exp_text<Es, Idxs>() + ...);
    }

    template<typename... Es>
    constexpr auto symbol_text(dimension<Es...> d)
    {
      return symbol_text_impl<>(d, std::index_sequence_for<Es...>());
    }

    template<typename Unit>
    constexpr auto unit_text()
    {
      if constexpr(!is_unit<Unit>) {
        // Unit is a downcasted derived unit child class already so just print defined symbol immediately 
        return Unit::symbol;
      }
      else {
        // we are dealing with a non-user-defined unit here
        using ratio = Unit::ratio;
        using dim = Unit::dimension;
        if constexpr(!is_dimension<dim>) {
          // downcasted user-defined dimension
          // print as a prefix or ratio of a coherent unit symbol defined by the user
          using coherent_unit = downcast<units::unit<dim, units::ratio<1>>>;
          return prefix_or_ratio_text<ratio, typename coherent_unit::prefix_type>() + coherent_unit::symbol;
        }
        else {
          // print as a ratio of a coherent unit + coherent unit dimensions and their exponents
          return ratio_text<ratio>() + symbol_text(dim{});
        }
      }
    }

  } // namespace detail


  // derived_unit

  template<typename Child, basic_fixed_string Symbol, Dimension D, typename PrefixType = no_prefix>
  struct named_coherent_derived_unit : downcast_child<Child, unit<D, ratio<1>>> {
    static constexpr auto symbol = Symbol;
    using prefix_type = PrefixType;
  };

  template<typename Child, Dimension D, typename PrefixType = no_prefix>
  struct coherent_derived_unit : downcast_child<Child, unit<D, ratio<1>>> {
    static constexpr auto symbol = detail::symbol_text(D());
    using prefix_type = PrefixType;
  };

  template<typename Child, basic_fixed_string Symbol, Dimension D, Ratio R>
  struct named_derived_unit : downcast_child<Child, unit<D, R>> {
    static constexpr auto symbol = Symbol;
  };

  template<typename Child, Prefix P, Unit U>
    requires requires { U::symbol; }
  struct prefixed_derived_unit : downcast_child<Child, unit<typename U::dimension, ratio_multiply<typename P::ratio, typename U::ratio>>> {
    static constexpr auto symbol = P::symbol + U::symbol;
    using prefix_type = P::prefix_type;
  };

  template<typename Child, basic_fixed_string Symbol, Dimension D, Unit U, Unit... Us>
  struct named_deduced_derived_unit : downcast_child<Child, detail::make_derived_unit<D, U, Us...>> {
    static constexpr auto symbol = Symbol;
  };

  template<typename Child, Dimension D, Unit U, Unit... Us>
  struct deduced_derived_unit : downcast_child<Child, detail::make_derived_unit<D, U, Us...>> {
    static constexpr auto symbol = basic_fixed_string("bbb");
  };

}  // namespace units
