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

#include "type_list_no_concepts.h"
#include "downcasting_no_concepts.h"
#include "ratio_no_concepts.h"
#include <ratio>

namespace units {

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

  // exponent

  template<const base_dimension& BaseDimension, std::intmax_t Num, std::intmax_t Den = 1>
  struct exponent {
    static constexpr const base_dimension& dimension = BaseDimension;
    static constexpr std::intmax_t num = Num;
    static constexpr std::intmax_t den = Den;
  };

  // exp_dim_id_less

  template<typename E1, typename E2>
  struct exponent_less : base_dimension_less<E1::dimension, E2::dimension> {
  };

  // exponent_invert

  template<typename E>
  struct exponent_invert;

  template<const base_dimension& BaseDimension, std::intmax_t Num, std::intmax_t Den>
  struct exponent_invert<exponent<BaseDimension, Num, Den>> {
    using type = exponent<BaseDimension, -Num, Den>;
  };

  template<typename E>
  using exponent_invert_t = exponent_invert<E>::type;

  // dimension

  template<typename... Es>
  struct dimension : downcast_base<dimension<Es...>> {};

  // dim_invert

  template<typename E>
  struct dim_invert;

  template<typename... Es>
  struct dim_invert<dimension<Es...>> : std::type_identity<downcast_traits_t<dimension<exponent_invert_t<Es>...>>> {};

  template<typename D>
  using dim_invert_t = dim_invert<typename D::downcast_base_type>::type;


  // make_dimension

  namespace detail {

    template<typename D>
    struct dim_consolidate;

    template<typename D>
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
      using type = conditional<is_same_v<rest, dimension<>>, dimension<E1>, type_list_push_front<rest, E1>>;
    };

    template<const base_dimension& D, std::intmax_t Num1, std::intmax_t Den1, std::intmax_t Num2, std::intmax_t Den2, typename... ERest>
    struct dim_consolidate<dimension<exponent<D, Num1, Den1>, exponent<D, Num2, Den2>, ERest...>> {
      using r1 = std::ratio<Num1, Den1>;
      using r2 = std::ratio<Num2, Den2>;
      using r = std::ratio_add<r1, r2>;
      using type = conditional<r::num == 0, dim_consolidate_t<dimension<ERest...>>,
                               dim_consolidate_t<dimension<exponent<D, r::num, r::den>, ERest...>>>;
    };

  }  // namespace detail

  template<typename... Es>
  struct make_dimension {
    using type = detail::dim_consolidate_t<type_list_sort<dimension<Es...>, exponent_less>>;
  };

  template<typename... Es>
  using make_dimension_t = make_dimension<Es...>::type;

  template<typename D1, typename D2>
  struct merge_dimension {
    using type = detail::dim_consolidate_t<type_list_merge_sorted<D1, D2, exponent_less>>;
  };

  template<typename D1, typename D2>
  using merge_dimension_t = merge_dimension<D1, D2>::type;

  // dimension_multiply

  template<typename D1, typename D2>
  struct dimension_multiply;

  template<typename... E1, typename... E2>
  struct dimension_multiply<dimension<E1...>, dimension<E2...>> : std::type_identity<downcast_traits_t<merge_dimension_t<dimension<E1...>, dimension<E2...>>>> {};

  template<typename D1, typename D2>
  using dimension_multiply_t = dimension_multiply<typename D1::downcast_base_type, typename D2::downcast_base_type>::type;

  // dimension_divide

  template<typename D1, typename D2>
  struct dimension_divide;

  template<typename... E1, typename... E2>
  struct dimension_divide<dimension<E1...>, dimension<E2...>>
      : dimension_multiply<dimension<E1...>, dimension<exponent_invert_t<E2>...>> {
  };

  template<typename D1, typename D2>
  using dimension_divide_t = dimension_divide<typename D1::downcast_base_type, typename D2::downcast_base_type>::type;

}  // namespace units
