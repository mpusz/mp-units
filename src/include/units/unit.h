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

  // deduced_derived_unit

  namespace detail {

    template<typename D>
    struct get_unit_base_dim;

    template<typename E, typename... Rest>
    struct get_unit_base_dim<dimension<E, Rest...>> {
      static_assert(sizeof...(Rest) == 0, "Base unit expected");
      using dimension = E::dimension;
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

    template<typename E, typename... ERest, typename U, typename... URest>
    struct derived_ratio<dimension<E, ERest...>, U, URest...> {
      static_assert(same_dim<typename E::dimension, typename U::dimension>, "The order and number of units in `deduced_derived_unit<Us...>` should match dimensions provided in a `derived_dimension<>`");
      static_assert(sizeof...(ERest) == sizeof...(URest), "The number of `deduced_derived_unit<Us...>` units should match the number of exponents provided to `derived_dimension<>`");
      using rest_ratio = derived_ratio<dimension<ERest...>, URest...>::ratio;
      using ratio = ratio_op<rest_ratio, E::num, E::den, typename U::ratio>::ratio;
    };

    template<typename... Es>
    constexpr auto exp_count(dimension<Es...>)
    {
      return sizeof...(Es);
    }

    template<typename U>
    inline constexpr bool is_unit_of_base_dimension = (exp_count(typename U::dimension::base_type()) == 1);

    template<Unit... Us>
    inline constexpr bool are_units_of_base_dimension = (is_unit_of_base_dimension<Us> && ...);

    template<Dimension D, Unit... Us>
    using deduced_derived_unit =
      unit<D, typename detail::derived_ratio<std::conditional_t<are_units_of_base_dimension<Us...>,
                                                                typename D::base_type, typename D::recipe>, Us...>::ratio>;

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

    template<typename E, basic_fixed_string Symbol, std::size_t Idx>
    constexpr auto exp_text()
    {
      // get calculation operator + symbol
      const auto txt = operator_text<E::num < 0, Idx>() + Symbol;
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

    template<typename Dim>
    constexpr auto dimension_symbol()
    {
      if constexpr(BaseDimension<Dim>)
        return Dim::symbol;
      else 
        // coherent derived unit
        return downcast<unit<Dim, ratio<1>>>::symbol;
    }

    template<typename... Es, std::size_t... Idxs>
    constexpr auto base_symbol_text_impl(dimension<Es...>, std::index_sequence<Idxs...>)
    {
      return (exp_text<Es, dimension_symbol<typename Es::dimension>(), Idxs>() + ...);
    }

    template<typename... Es>
    constexpr auto base_symbol_text(dimension<Es...> d)
    {
      return base_symbol_text_impl(d, std::index_sequence_for<Es...>());
    }

    template<typename... Es>
    constexpr bool all_named(dimension<Es...>)
    {
      return (downcast<unit<typename Es::dimension, ratio<1>>>::is_named && ...);
    }

    template<typename Dim>
    constexpr auto base_symbol_text()
    {
      using recipe = typename Dim::recipe;
      if constexpr(all_named(recipe()))
        return base_symbol_text(recipe());
      else
        return base_symbol_text(Dim());
    }

    template<typename E, typename U, std::size_t Idx>
    constexpr auto exp_validate_and_text()
    {
      static_assert(same_dim<typename E::dimension, typename U::dimension>, "The order and number of units in `deduced_derived_unit<Us...>` should match dimensions provided in a `derived_dimension<>`");
      return exp_text<E, U::symbol, Idx>();
    }

    template<typename... Us, typename... Es, std::size_t... Idxs>
    constexpr auto deduced_symbol_text_impl(dimension<Es...>, std::index_sequence<Idxs...>)
    {
      return (exp_validate_and_text<Es, Us, Idxs>() + ...);
    }

    template<typename... Us, typename... Es>
    constexpr auto deduced_symbol_text(dimension<Es...> d)
    {
      static_assert(sizeof...(Es) == sizeof...(Us), "The number of `deduced_derived_unit<Us...>` units should match the number of exponents provided to `derived_dimension<>`");
      return deduced_symbol_text_impl<Us...>(d, std::index_sequence_for<Es...>());
    }

    template<typename Dim, typename... Us>
    constexpr auto deduced_symbol_text()
    {
      if constexpr(are_units_of_base_dimension<Us...>)
        return deduced_symbol_text<Us...>(typename Dim::base_type());
      else
        return deduced_symbol_text<Us...>(typename Dim::recipe());
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
          return ratio_text<ratio>() + base_symbol_text(dim{});
        }
      }
    }

  } // namespace detail


  // derived_unit

  template<typename Child, Dimension Dim, basic_fixed_string Symbol, PrefixType PT>
  struct named_coherent_derived_unit : downcast_child<Child, unit<Dim, ratio<1>>> {
    static constexpr bool is_named = true;
    static constexpr auto symbol = Symbol;
    using prefix_type = PT;
  };

  template<typename Child, Dimension Dim>
  struct coherent_derived_unit : downcast_child<Child, unit<Dim, ratio<1>>> {
    static constexpr bool is_named = false;
    static constexpr auto symbol = detail::base_symbol_text<Dim>();
    using prefix_type = no_prefix;
  };

  template<typename Child, Dimension Dim, basic_fixed_string Symbol, Ratio R, PrefixType PT = no_prefix>
  struct named_scaled_derived_unit : downcast_child<Child, unit<Dim, R>> {
    static constexpr bool is_named = true;
    static constexpr auto symbol = Symbol;
    using prefix_type = PT;
  };

  template<typename Child, Dimension Dim, basic_fixed_string Symbol, PrefixType PT, Unit U, Unit... Us>
  struct named_deduced_derived_unit : downcast_child<Child, detail::deduced_derived_unit<Dim, U, Us...>> {
    static constexpr bool is_named = true;
    static constexpr auto symbol = Symbol;
    using prefix_type = PT;
  };

  template<typename Child, Dimension Dim, Unit U, Unit... Us>
    requires U::is_named && (Us::is_named && ... && true)
  struct deduced_derived_unit : downcast_child<Child, detail::deduced_derived_unit<Dim, U, Us...>> {
    static constexpr bool is_named = false;
    static constexpr auto symbol = detail::deduced_symbol_text<Dim, U, Us...>();
    using prefix_type = no_prefix;
  };

  template<typename Child, Prefix P, Unit U>
    requires (!std::same_as<typename U::prefix_type, no_prefix>)
  struct prefixed_derived_unit : downcast_child<Child, unit<typename U::dimension, ratio_multiply<typename P::ratio, typename U::ratio>>> {
    static constexpr bool is_named = true;
    static constexpr auto symbol = P::symbol + U::symbol;
    using prefix_type = P::prefix_type;
  };

}  // namespace units
