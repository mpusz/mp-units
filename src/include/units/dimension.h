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

#include "bits/tools.h"
#include "bits/type_list.h"

namespace units {

  // dim_id

  template<int UniqueValue>
  using dim_id = std::integral_constant<int, UniqueValue>;

  // dim_id_less

  template<typename D1, typename D2>
      struct dim_id_less : std::bool_constant<D1::value<D2::value> {
  };

  // exp

  template<typename BaseDimension, int Value>
  struct exp {
    using dimension = BaseDimension;
    static constexpr int value = Value;
  };

  // exp_less

  template<typename E1, typename E2>
  struct exp_less : dim_id_less<typename E1::dimension, typename E2::dimension> {
  };

  // exp_invert

  template<typename Exponent>
  struct exp_invert;

  template<typename BaseDimension, int Value>
  struct exp_invert<exp<BaseDimension, Value>> {
    using type = exp<BaseDimension, -Value>;
  };

  template<typename Exponent>
  using exp_invert_t = typename exp_invert<Exponent>::type;

  // dimension

  template<typename... Exponents>
  using dimension = mp::type_list<Exponents...>;

  // make_dimension

  namespace detail {

    struct scalar;

    template<typename D>
    struct dim_consolidate;

    template<typename D>
    using dim_consolidate_t = typename dim_consolidate<D>::type;

    template<>
    struct dim_consolidate<dimension<>> {
      using type = dimension<scalar>;
    };

    template<typename E1>
    struct dim_consolidate<dimension<E1>> {
      using type = dimension<E1>;
    };

    template<typename... ERest>
    struct dim_consolidate<dimension<scalar, ERest...>> {
      using type = dim_consolidate_t<dimension<ERest...>>;
    };

    template<typename D, int V1, int V2, typename... ERest>
    struct dim_consolidate<dimension<exp<D, V1>, exp<D, V2>, ERest...>> {
      using type = std::conditional_t<V1 + V2 == 0, dim_consolidate_t<dimension<ERest...>>,
                                      dim_consolidate_t<dimension<exp<D, V1 + V2>, ERest...>>>;
    };

    template<typename E1, typename... ERest>
    struct dim_consolidate<dimension<E1, ERest...>> {
      using rest = dim_consolidate_t<dimension<ERest...>>;
      using type = std::conditional_t<std::is_same_v<rest, dimension<scalar>>, dimension<E1>,
                                      mp::type_list_push_front_t<rest, E1>>;
    };

  }  // namespace detail

  template<typename... Exponents>
  struct make_dimension {
    using type = detail::dim_consolidate_t<mp::type_list_sort_t<mp::type_list<Exponents...>, exp_less>>;
  };

  template<typename... Exponents>
  using make_dimension_t = typename make_dimension<Exponents...>::type;

  // dimension_multiply

  template<typename D1, typename D2>
  struct dimension_multiply;

  template<typename... Exp1, typename... Exp2>
  struct dimension_multiply<dimension<Exp1...>, dimension<Exp2...>> {
    using type = make_dimension_t<Exp1..., Exp2...>;
  };

  template<typename D1, typename D2>
  using dimension_multiply_t = typename dimension_multiply<D1, D2>::type;

  // dimension_divide

  template<typename D1, typename D2>
  struct dimension_divide;

  template<typename... Exp1, typename... Exp2>
  struct dimension_divide<dimension<Exp1...>, dimension<Exp2...>>
      : dimension_multiply<dimension<Exp1...>, dimension<exp_invert_t<Exp2>...>> {
  };

  template<typename D1, typename D2>
  using dimension_divide_t = typename dimension_divide<D1, D2>::type;

}  // namespace units
