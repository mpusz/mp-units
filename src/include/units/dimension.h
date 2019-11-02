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

#include <units/bits/type_list.h>
#include <units/bits/downcasting.h>
#include <units/bits/fixed_string.h>
#include <units/ratio.h>
#include <ratio>

namespace units {

  template<basic_fixed_string Name, basic_fixed_string Symbol>
  struct base_dimension {
    static constexpr auto name = Name;
    static constexpr auto symbol = Symbol;
  };

  template<typename T>
  concept BaseDimension = std::is_empty_v<T> &&
      requires {
        T::name;
        T::symbol;
      };// &&  // TODO file a bug for this gcc issue
//      std::derived_from<T, base_dimension<T::name, T::symbol>>;

  // base_dimension_less

  template<BaseDimension D1, BaseDimension D2>
  struct base_dimension_less : std::bool_constant<D1::name < D2::name> {
  };

  // is_exp
  namespace detail {

    template<typename T>
    inline constexpr bool is_exp = false;

    // partial specialization for an exp type provided below

  }  // namespace detail

  template<typename T>
  concept Exponent = detail::is_exp<T>;

  template<Exponent... Es>
  struct dimension;

  // is_dimension
  namespace detail {

    template<typename T>
    inline constexpr bool is_dimension = false;

    template<typename... Es>
    inline constexpr bool is_dimension<dimension<Es...>> = true;

  }  // namespace detail

  template<typename T>
  concept Dimension =
      std::is_empty_v<T> &&
      detail::is_dimension<downcast_base_t<T>>;

  // exp
  template<typename BaseOrDim, int Num, int Den = 1>
    requires BaseDimension<BaseOrDim> || Dimension<BaseOrDim>
  struct exp {
    using dimension = BaseOrDim;
    static constexpr int num = Num;
    static constexpr int den = Den;
  };

  // is_exp
  namespace detail {

    template<typename Dim, int Num, int Den>
    inline constexpr bool is_exp<exp<Dim, Num, Den>> = true;

  }  // namespace detail

  // exp_less
  template<Exponent E1, Exponent E2>
  struct exp_less : base_dimension_less<typename E1::dimension, typename E2::dimension> {
  };

  // exp_invert
  namespace detail {

    template<Exponent E>
    struct exp_invert_impl;

    template<typename Dim, int Num, int Den>
    struct exp_invert_impl<exp<Dim, Num, Den>> {
      using type = exp<Dim, -Num, Den>;
    };

  }

  template<Exponent E>
  using exp_invert = detail::exp_invert_impl<E>::type;

  // exp_multiply
  namespace detail {

    template<Exponent E, int Num, int Den>
    struct exp_multiply_impl {
      using r1 = ratio<E::num, E::den>;
      using r2 = ratio<Num, Den>;
      using r = ratio_multiply<r1, r2>;
      using type = exp<typename E::dimension, r::num, r::den>;
    };

  }

  template<Exponent E, int Num, int Den>
  using exp_multiply = detail::exp_multiply_impl<E, Num, Den>::type;

  // dimension
  template<Exponent... Es>
  struct dimension : downcast_base<dimension<Es...>> {};

  // same_dim
  template<Dimension D1, Dimension D2>
  inline constexpr bool same_dim = std::is_same_v<typename D1::base_type, typename D2::base_type>;

  // dim_invert
  namespace detail {

    template<Dimension E>
    struct dim_invert_impl;

    template<typename... Es>
    struct dim_invert_impl<dimension<Es...>> : std::type_identity<downcast<dimension<exp_invert<Es>...>>> {};

  }

  template<Dimension D>
  using dim_invert = detail::dim_invert_impl<downcast_base_t<D>>::type;

  // make_dimension
  namespace detail {

    template<Dimension D>
    struct dim_consolidate;

    template<>
    struct dim_consolidate<dimension<>> {
      using type = dimension<>;
    };

    template<typename E>
    struct dim_consolidate<dimension<E>> {
      using type = dimension<E>;
    };

    template<typename E1, typename... ERest>
    struct dim_consolidate<dimension<E1, ERest...>> {
      using type = type_list_push_front<typename dim_consolidate<dimension<ERest...>>::type, E1>;
    };

    template<BaseDimension D, int Num1, int Den1, int Num2, int Den2, typename... ERest>
    struct dim_consolidate<dimension<exp<D, Num1, Den1>, exp<D, Num2, Den2>, ERest...>> {
      // TODO: provide custom implementation for ratio_add
      using r1 = std::ratio<Num1, Den1>;
      using r2 = std::ratio<Num2, Den2>;
      using r = std::ratio_add<r1, r2>;
      using type = conditional<r::num == 0, typename dim_consolidate<dimension<ERest...>>::type,
                               typename dim_consolidate<dimension<exp<D, r::num, r::den>, ERest...>>::type>;
    };

    template<Exponent... Es>
    struct extract;

    template<>
    struct extract<> {
      using type = dimension<>;
    };

    template<BaseDimension Dim, int Num, int Den, Exponent... ERest>
    struct extract<exp<Dim, Num, Den>, ERest...> {
      using type = type_list_push_front<typename extract<ERest...>::type, exp<Dim, Num, Den>>;
    };

    template<Exponent... Es, int Num, int Den, Exponent... ERest>
    struct extract<exp<dimension<Es...>, Num, Den>, ERest...> {
      using type = type_list_push_front<typename extract<ERest...>::type, exp_multiply<Es, Num, Den>...>;
    };

    template<Dimension Dim, int Num, int Den, Exponent... ERest>
    struct extract<exp<Dim, Num, Den>, ERest...> {
      using type = extract<exp<downcast_base_t<Dim>, Num, Den>, ERest...>::type;
    };

    template<Exponent... Es>
    struct make_dimension {
      using type = detail::dim_consolidate<type_list_sort<typename detail::extract<Es...>::type, exp_less>>::type;
    };

  }  // namespace detail

  // derived_dimension
  template<typename Child, Exponent... Es>
  struct derived_dimension : downcast_child<Child, typename detail::make_dimension<Es...>> {};

  // merge_dimension
  namespace detail {

    template<Dimension D1, Dimension D2>
    struct merge_dimension_impl {
      using type = detail::dim_consolidate<type_list_merge_sorted<D1, D2, exp_less>>::type;
    };

  }

  template<Dimension D1, Dimension D2>
  using merge_dimension = detail::merge_dimension_impl<D1, D2>::type;

  // dimension_multiply
  namespace detail {

    template<Dimension D1, Dimension D2>
    struct dimension_multiply_impl;

    template<typename... E1, typename... E2>
    struct dimension_multiply_impl<dimension<E1...>, dimension<E2...>> : std::type_identity<downcast<merge_dimension<dimension<E1...>, dimension<E2...>>>> {};

  }

  template<Dimension D1, Dimension D2>
  using dimension_multiply = detail::dimension_multiply_impl<typename D1::base_type, typename D2::base_type>::type;

  // dimension_divide
  namespace detail {

    template<Dimension D1, Dimension D2>
    struct dimension_divide_impl;

    template<typename... E1, typename... E2>
    struct dimension_divide_impl<dimension<E1...>, dimension<E2...>>
        : dimension_multiply_impl<dimension<E1...>, dimension<exp_invert<E2>...>> {
    };

  }

  template<Dimension D1, Dimension D2>
  using dimension_divide = detail::dimension_divide_impl<typename D1::base_type, typename D2::base_type>::type;

  // dimension_sqrt
  namespace detail {

    template<Dimension D>
    struct dimension_sqrt_impl;

    template<typename... Es>
    struct dimension_sqrt_impl<dimension<Es...>> : std::type_identity<downcast<dimension<exp_multiply<Es, 1, 2>...>>> {};
  
  }

  template<Dimension D>
  using dimension_sqrt = detail::dimension_sqrt_impl<typename D::base_type>::type;

  // dimension_pow
  namespace detail {

    template<Dimension D, std::size_t N>
    struct dimension_pow_impl;

    template<typename... Es, std::size_t N>
    struct dimension_pow_impl<dimension<Es...>, N> : std::type_identity<downcast<dimension<exp_multiply<Es, N, 1>...>>> {};

  }

  template<Dimension D, std::size_t N>
  using dimension_pow = detail::dimension_pow_impl<typename D::base_type, N>::type;

}  // namespace units
