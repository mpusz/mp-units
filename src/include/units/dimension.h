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
#include <units/ratio.h>
#include <ratio>

namespace std::experimental::units {

  struct base_dimension {
    const char* name;
  };

  constexpr bool operator==(const base_dimension& lhs, const base_dimension& rhs)
  {
    const char* p1 = lhs.name;
    const char* p2 = rhs.name;
    for(; (*p1 != '\0') && (*p2 != '\0'); ++p1, (void)++p2) {
      if(*p1 != *p2) return false;
    }
    return *p1 == *p2;
  }

  constexpr bool operator<(const base_dimension& lhs, const base_dimension& rhs)
  {
    const char* p1 = lhs.name;
    const char* p2 = rhs.name;
    for(; (*p1 != '\0') && (*p2 != '\0'); ++p1, (void)++p2) {
      if(*p1 < *p2) return true;
      if(*p2 < *p1) return false;
    }
    return (*p1 == '\0') && (*p2 != '\0');
  }

  // base_dimension_less

  template<const base_dimension& D1, const base_dimension& D2>
  struct base_dimension_less : std::bool_constant<D1 < D2> {
  };

  // exp

  template<const base_dimension& BaseDimension, int Num, int Den = 1>
  struct exp {
    static constexpr const base_dimension& dimension = BaseDimension;
    static constexpr int num = Num;
    static constexpr int den = Den;
  };

  // is_exp
  namespace detail {
    template<typename T>
    inline constexpr bool is_exp = false;

    template<const base_dimension& BaseDimension, int Num, int Den>
    inline constexpr bool is_exp<exp<BaseDimension, Num, Den>> = true;
  }  // namespace detail

  template<typename T>
  concept bool Exponent = detail::is_exp<T>;

  // exp_dim_id_less

  template<Exponent E1, Exponent E2>
  struct exp_less : base_dimension_less<E1::dimension, E2::dimension> {
  };

  // exp_invert

  template<Exponent E>
  struct exp_invert;

  template<const base_dimension& BaseDimension, int Num, int Den>
  struct exp_invert<exp<BaseDimension, Num, Den>> {
    using type = exp<BaseDimension, -Num, Den>;
  };

  template<Exponent E>
  using exp_invert_t = exp_invert<E>::type;

  // dimension

  template<Exponent... Es>
  struct dimension : downcast_base<dimension<Es...>> {};

  // is_dimension
  namespace detail {

    template<typename T>
    inline constexpr bool is_dimension = false;

    template<typename... Es>
    inline constexpr bool is_dimension<dimension<Es...>> = true;

  }  // namespace detail

  template<typename T>
  concept bool Dimension =
      std::is_empty_v<T> &&
      detail::is_dimension<downcast_from<T>>;


  // dim_invert

  template<Dimension E>
  struct dim_invert;

  template<typename... Es>
  struct dim_invert<dimension<Es...>> : std::type_identity<downcasting_traits_t<dimension<exp_invert_t<Es>...>>> {};

  template<Dimension D>
  using dim_invert_t = dim_invert<typename D::base_type>::type;


  // todo: force as the only user interface to create dimensions through modules
  // make_dimension

  namespace detail {

    template<Dimension D>
    struct dim_consolidate;

    template<Dimension D>
    using dim_consolidate_t = dim_consolidate<D>::type;

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
      using rest = dim_consolidate_t<dimension<ERest...>>;
      using type = conditional<std::is_same_v<rest, dimension<>>, dimension<E1>, type_list_push_front<rest, E1>>;
    };

    template<const base_dimension& D, int Num1, int Den1, int Num2, int Den2, typename... ERest>
    struct dim_consolidate<dimension<exp<D, Num1, Den1>, exp<D, Num2, Den2>, ERest...>> {
      // todo: provide custom implementation for ratio_add
      using r1 = std::ratio<Num1, Den1>;
      using r2 = std::ratio<Num2, Den2>;
      using r = std::ratio_add<r1, r2>;
      using type = conditional<r::num == 0, dim_consolidate_t<dimension<ERest...>>,
                               dim_consolidate_t<dimension<exp<D, r::num, r::den>, ERest...>>>;
    };

  }  // namespace detail

  template<Exponent... Es>
  struct make_dimension {
    using type = detail::dim_consolidate_t<type_list_sort<dimension<Es...>, exp_less>>;
  };

  template<Exponent... Es>
  using make_dimension_t = make_dimension<Es...>::type;

  template<Dimension D1, Dimension D2>
  struct merge_dimension {
    using type = detail::dim_consolidate_t<type_list_merge_sorted<D1, D2, exp_less>>;
  };

  template<Dimension D1, Dimension D2>
  using merge_dimension_t = merge_dimension<D1, D2>::type;

  // dimension_multiply

  template<Dimension D1, Dimension D2>
  struct dimension_multiply;

  template<typename... E1, typename... E2>
  struct dimension_multiply<dimension<E1...>, dimension<E2...>> : std::type_identity<downcasting_traits_t<merge_dimension_t<dimension<E1...>, dimension<E2...>>>> {};

  template<Dimension D1, Dimension D2>
  using dimension_multiply_t = dimension_multiply<typename D1::base_type, typename D2::base_type>::type;

  // dimension_divide

  template<Dimension D1, Dimension D2>
  struct dimension_divide;

  template<typename... E1, typename... E2>
  struct dimension_divide<dimension<E1...>, dimension<E2...>>
      : dimension_multiply<dimension<E1...>, dimension<exp_invert_t<E2>...>> {
  };

  template<Dimension D1, Dimension D2>
  using dimension_divide_t = dimension_divide<typename D1::base_type, typename D2::base_type>::type;

}  // namespace std::experimental::units
